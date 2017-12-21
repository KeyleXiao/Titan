using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using GameLogic;
using DataCenter;
using USpeedUI;
using U3D_Render;

/**
状态机的定义：

2层状态机叠加的方式实现，常规的说法叫上身下身，我们定义为

姿态状态机(Posture Layer)：
管理角色姿态，分解实现运动位置变化逻辑，以及在不同的姿态能做不同的动作
比如直立模式下，根据速度不同，可以做走跑站立后退动作，运动规则是速度+贴地

浮空模式有跳跃击飞下落等不同的动作表现，但基本物理规律是速度+下落。
直立  浮空 游泳 攀爬 飞檐走壁 翻滚

动作状态机(Act Layer)：
攻击，被击，扭头，招手等等，不同姿态能做的动作不同  
 */

public sealed class CreatureStateMachine : BaseStateMachine
{

    #region 废弃代码

    // 状态控制器
    //public RuntimeAnimatorController m_Stand;      // 战立状态
    //public RuntimeAnimatorController m_Jump;       // 跳跃状态
    //public RuntimeAnimatorController m_Dead;       // 死亡状态
    //public RuntimeAnimatorController m_Swim;       // 游泳状态

    // 状态映射表
    //private RuntimeAnimatorController[] m_stateAnimators = new RuntimeAnimatorController[(int)EntityState.MaxCount]; 


    //[HideInInspector]
    //public CreatureProperty   property;                 // 生物属性对象

    // public int jumpDeltyTick = 10; //跳跃起高度跳延迟毫秒
    #endregion
    public float syncDisThread = 200.0f; // 同步后退拉扯阈值，与同步包只是位置距离小于此阈值时角色不过后退方向的拉扯
    public int rebornTick = 5000; //死亡重生时间（毫秒）


    //[HideInInspector]
    //public CapsuleCollider col;                         // 生物轮廓的胶囊体对象

    //与控制类交互用的参数
    //public float maxFallSpeed = 20.0f;

    //[HideInInspector]
    public Vector3 inputMoveDirection; //键盘输入方向，同时考虑各种屏蔽
    //[HideInInspector]
    public Vector3 rawInputMoveDirection; //原始键盘输入的方向，用于判断翻滚技能等方向
    [HideInInspector]
    public Vector3 autoMoveDirection = Vector3.zero; //自动寻路方向
    [HideInInspector]
    public bool inputJump; //是否按了跳跃键
    [HideInInspector]
    public float maxAcceleration = 20.0f; //最大加速度

    public bool bAnimPauseControl = false; //BlendAttack动作暂停键盘控制移动和转向


    [HideInInspector]
    public int attackAnimatorLayer = 0;
    [HideInInspector]
    public int moveAnimatorLayer = 0;


    [HideInInspector]
    public CollisionFlags collisionFlags;

    public delegate bool CREATURE_DOACT_HANDLER(ActID id, BaseStateMachine fsm);
    public CREATURE_DOACT_HANDLER[] doActHandlers = new CREATURE_DOACT_HANDLER[GLOBAL.MAX_ACT];     // 动作实现函数，因为是委托也可以用+=的方式实现订阅


    public SoldierController soliderController;


    private LayerMask collisionLayers;

    /// <summary>
    /// 第一次走跟visible相关的mask，就是创建时候的mask，屏蔽掉
    /// </summary>
    private bool bFirstVisibleMask = false;

    [System.NonSerialized]
    public CharacterController controller;

    public int syncTick = 0;
    public Vector3 syncPosition = Vector3.zero; //用于同步的速度，主角的话设置此向量上报给服务器，非主角的话读取此向量作为速度
    public Vector3 syncVelocity = Vector3.zero; // 同步下来的位置/模拟位置
    public Vector3 syncAngle = Vector3.zero;      // 同步下来的角度
    public float syncDetalTime = 0;
    public int syncInterval = 300;               // 位置同步给服务器的频率
    public bool ShiftEnable = false;            //是否允许加速

    public float frameAccFactor = 0.5f;         //帧率突变缓冲因子

    public SidekickEntity sidekickEntity;

    public bool showAnimLog; //动作日志，检查动作时输出

    public uint cameraSwitchAfterDead = 4000;
    /// ///////////////////////////////////////////////////////////////////////////////////////////

    public PersonSkinConfig SkinConfig
    {
        get { return (PersonSkinConfig)entitySkinConfig; }
    }


    //负责控制动画
    public CreatureAnimatorCtrl animatorCtrl = null;

    //负责控制非自住控制的位移
    public CreatureMoveCtrl moveCtrl = null;


    public EntityState getEntityState()
    {
        return (EntityState)m_CurStateHandler.GetID();
    }

    public override void ResetAllMembers()
    {
        base.ResetAllMembers();

        if (animatorCtrl != null)
        {
            animatorCtrl.ResetBones(); ////还原层次结构，避免节点动画丢失
        }

        animatorCtrl = null;
        moveCtrl = null;
        syncDisThread = 200.0f; // 同步后退拉扯阈值，与同步包只是位置距离小于此阈值时角色不过后退方向的拉扯
        rebornTick = 5000; //死亡重生时间（毫秒

        inputMoveDirection = Vector3.zero; //键盘输入方向，同时考虑各种屏蔽
        rawInputMoveDirection = Vector3.zero; //原始键盘输入的方向，用于判断翻滚技能等方向
        autoMoveDirection = Vector3.zero; //自动寻路方向
        inputJump = false; //是否按了跳跃键
        maxAcceleration = 20.0f; //最大加速度
        collisionFlags = CollisionFlags.Above;

        if (null != doActHandlers)
        {
            doActHandlers.Initialize();
        }

        soliderController = null;

        bFirstVisibleMask = false;
        controller = null;

        syncTick = 0;
        syncPosition = Vector3.zero; //用于同步的速度，主角的话设置此向量上报给服务器，非主角的话读取此向量作为速度
        syncVelocity = Vector3.zero; // 同步下来的位置/模拟位置
        syncAngle = Vector3.zero;           // 同步下来的角度
        syncDetalTime = 0;
        syncInterval = 300;
        ShiftEnable = false;  //是否允许加速

        frameAccFactor = 0.5f; //帧率突变缓冲因子
        lastHitPoint = Vector3.zero;
        lastGroundNormal = Vector3.zero;

        m_hitCharacter = null;
        m_lastHitInfo = null;
        m_formtype = FORM_TYPE.FORM_GROUND;

        dodge = false; //切换皮肤是停止轻功，避免轻功状态传递给下个英雄
        CurrErrorGroundLayerThreshold = 0;
    }

    public override void Init()
    {
        #region 废弃代码
        //hips = null;
        //spine = null;
        ////根据与美术的约定找到上下半身分离点,目前由策划手动拖动到预制体中
        //root = transform.Find("Main");
        //if (root != null)
        //{
        //    hips = root.Find("Root_M");
        //    hipsCopy = root.Find("Root_MCopy");
        //    if (hips != null)
        //    {
        //        spine = hips.Find("BackA_M");
        //    }
        //}
        #endregion

        if (animatorCtrl == null)
        {
            animatorCtrl = new CreatureAnimatorCtrl(this);
        }

        if (moveCtrl == null)
        {
            moveCtrl = new CreatureMoveCtrl(this);
        }

        animator = GetComponent<Animator>();
        creature = GetComponent<Transform>();
        controller = GetComponent<CharacterController>();
        controller.height = 2;
        controller.center = new Vector3(0, 1.0f, 0);
        creaturePropety = GetComponent<CreatureProperty>();
        //castLayer = GetComponent<ScreenCastLayer>();

        if (null == m_stateHandlers)
        {
            m_stateHandlers = new ICreatureState[(int)EntityState.MaxCount];
        }
        m_stateHandlers[(int)EntityState.Standing] = new CreatureState_Standing(this);
        m_stateHandlers[(int)EntityState.Jumping] = new CreatureState_Jump(this);
        m_stateHandlers[(int)EntityState.Dead] = new CreatureState_Dead(this);
        m_stateHandlers[(int)EntityState.Swimming] = new CreatureState_Swimming(this);
        m_stateHandlers[(int)EntityState.Floating] = new CreatureState_Floating(this);
        m_stateHandlers[(int)EntityState.WallWalking] = new CreatureState_WallWalking(this);
        m_stateHandlers[(int)EntityState.Glide] = new CreatureState_Glide(this);

        collisionLayers = -1;

        collisionLayers = collisionLayers.value & (~(1 << LayerMask.NameToLayer(Config.LayerHero)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerNeutrality)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerOcclusCulling)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastEnemy)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastFriend)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerEnemy)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerFriend)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterEnemy)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterFriend)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerWard_Enemy)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerWard_Friend)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerLightingEffectOnly)));

        ChangeState((int)EntityState.Standing, IntPtr.Zero);

        if (null == doActHandlers)
        {
            doActHandlers = new CREATURE_DOACT_HANDLER[GLOBAL.MAX_ACT];
        }
        doActHandlers[(int)ActID.Jump] = DoJump;
    }



    public override void Clear()
    {
        if (animatorCtrl != null)
        {
            animatorCtrl.ResetBones();
        }

        foreach (ICreatureState handler in m_stateHandlers)
        {
            if (null != handler)
            {
                handler.ClearState();
            }
        }

        base.Clear();

        animator = null;
        creature = null;
        controller = null;
        creaturePropety = null;
        //castLayer = null;

        controller = null;
        animatorCtrl = null;
        moveCtrl = null;


        m_stateHandlers[(int)EntityState.Standing] = null;
        m_stateHandlers[(int)EntityState.Jumping] = null;
        m_stateHandlers[(int)EntityState.Dead] = null;
        m_stateHandlers[(int)EntityState.Swimming] = null;
        m_stateHandlers[(int)EntityState.Floating] = null;

    }

    public void useWing()
    {
        if (!isHero) return;

        if (m_CurStateHandler.GetID() == (int)EntityState.Floating)
        {
            applyWing(false);
        }
        else
        {
            applyWing(true);
        }
    }

    /// ///////////////////////////////////////////////////////////////////////////////////////////
    /// 
    public void Update()
    {
        ////如果有混合情况，需要在LateUpdate保存算好的给下一帧Update时恢复，因为每帧顺序是Update->Animator.update->LateUpdate，这段代码目前上下身混合时用到
        //if (m_CurStateMachineParams.adjustSpine && m_CurStateMachineParams.separingBody && m_CurStateMachineParams.spine != null && m_CurStateMachineParams.hipsCopy != null) //hips!=null 不用判了，因为spine是根据hips取的
        //{
        //    if (animator.GetBool("isMoving"))
        //    {
        //        //restore last frame
        //        if (HipBlend > 0.0f && m_CurStateMachineParams.root != null && m_CurStateMachineParams.root.parent != null)
        //        {
        //            RestoreBoneTransformRecursive(m_CurStateMachineParams.root, m_CurStateMachineParams.root.parent, ref oldtrans);
        //            oldtrans.Clear();
        //        }
        //    }
        //}


        //if (InputManager.GetKeyDown(KeyCode.Alpha1))
        //{
        //    RollToPos(transform.position + transform.forward * 3);
        //}
        //if (InputManager.GetKeyDown(KeyCode.Alpha2))
        //{
        //    RollToPos(transform.position + transform.right * 3);
        //}
        //if (InputManager.GetKeyDown(KeyCode.Alpha3))
        //{
        //    RollToPos(transform.position - transform.forward * 3);
        //}
        //if (InputManager.GetKeyDown(KeyCode.Alpha4))
        //{
        //    RollToPos(transform.position - transform.right * 3);
        //}

        base.update();

        if (isHero)
        {
            updateFormType(); //上报离地高度
        }

        if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.UseWing))
        {
            useWing(); //使用飞行 
            return;
        }



        if (bPauseMachineUpdate) //暂停Update
        {
            return;
        }

        //if (soliderController == null || (!soliderController.isStopMove()))
        //{
        //    if ((isHero == true) && (inputJump == true) && (m_CurStateHandler.GetID() != (int)EntityState.Jumping))
        //    {
        //        DoAct(ActID.Jump);
        //    }
        //}


        //轻功测试专用
        //if (InputManager.GetKeyUp(KeyCode.G))
        //{
        //    dodge = !dodge;
        //}


        if (moveCtrl != null)
        {
            moveCtrl.update(); //非键盘控制位移
            if (!moveCtrl.isCharge())//不处于被动位移状态检测位置 
            {
                updateState();
            }
        }

        if (animatorCtrl != null)
        {
            if (dodge && animatorCtrl.ana != null && !animatorCtrl.ana.GetBool("isMoving"))
            {
                //停止移动时，退出轻功状态dodge = false;
                cmd_creature_act_sync data = new cmd_creature_act_sync();
                data.pos.fPosition_x = transform.position.x;
                data.pos.fPosition_y = transform.position.y;
                data.pos.fPosition_z = transform.position.z;
                data.pos.fRotation_x = transform.eulerAngles.x;//gameObject.transform.rotation.w;
                data.pos.fRotation_y = transform.eulerAngles.y;//gameObject.transform.rotation.y;
                data.pos.fRotation_z = transform.eulerAngles.z;// fsm.rb.rotation.z; 

                data.nCtrID = (int)ActID.Attack; ;
                data.nCtrParam = dodgeSkillID;
                data.nCtrParam2 = entityID;
                data.fCtrParam_x = transform.position.x;
                data.fCtrParam_y = transform.position.y;
                data.fCtrParam_z = transform.position.z;
                data.nSlotType = 0;
                data.nSlotIndex = 0;

                EntityEventHelper.Instance.SendCommand<cmd_creature_act_sync>(entityID, EntityLogicDef.ENTITY_CMD_CAST_SPELL, ref data);
            }

            animatorCtrl.update();
        }


        // 当前状态更新
        if (m_CurStateHandler != null && creaturePropety != null)
        {
            //更新当前重力加速度
            if (m_CurStateHandler.getVelocity().y > 0.0f)
            {
                currentGravity = creaturePropety.upGravity;//起跳速度
            }
            else
            {
                currentGravity = creaturePropety.downGravity;//坠落速度
            }

            m_CurStateHandler.Update();//当前状态的UPDATE
        }

        //向前运动速度
        UpdatemaxForwardSpeed();

        if (m_hitCharacter != null)
        {
            //旧的处理两个人物叠在一起的问题
            ////Debug.Log("m_hitCharacter.radius" + m_hitCharacter.radius);
            //if (m_hitCharacter.radius < 1.0f)
            //{
            //    Vector3 dis = transform.position - m_hitCharacter.transform.position;
            //    dis.y = 0;
            //    Vector3 offset = (controller.radius + m_hitCharacter.radius - dis.magnitude + m_hitCharacter.radius * 0.5f) * dis.normalized;
            //    //Debug.Log("this.position" + transform.position);
            //    //Debug.Log("m_hitCharacter.position" + m_hitCharacter.transform.position);
            //    //this.controller.Move(transform.forward * m_hitCharacter.radius * 2.0f);
            //    this.controller.Move(offset);

            //    //crossCharacterCount++; //踩到其他玩家了，往前走一步，已经走过的记录一下，最多走3次，不再走了
            //}
            //m_hitCharacter = null;

            if (isHero) //是主角，并且速度向下
            {
                CharacterControllerInfo HitChartacterinfo = CharacterControllerInfo.getInfo(m_hitCharacter);
                CharacterControllerInfo selfChartacterinfo = CharacterControllerInfo.getInfo(controller);
                //m_lastHitInfo
                if (null != HitChartacterinfo && null != selfChartacterinfo && null != m_lastHitInfo)
                {
                    Vector3 hitPoint = m_lastHitInfo.point;
                    //如果碰撞点在自己碰撞盒下面点的下方并且在别人头顶的上方，说明自己踩到别人了
                    if (hitPoint.y < (selfChartacterinfo.DownPointInWorldSpace.y + selfChartacterinfo.RadiusInWorldSpace + 0.001f) &&
                        hitPoint.y > (HitChartacterinfo.UpPointInWorldSpace.y - HitChartacterinfo.RadiusInWorldSpace - 0.001f))
                    {
                        Vector3 dis = creature.position - m_hitCharacter.transform.position;
                        dis.y = 0;
                        Vector3 offset = (controller.radius + m_hitCharacter.radius - dis.magnitude + m_hitCharacter.radius * 0.5f) * dis.normalized;
                        controller.Move(offset);
                    }
                }

                CharacterControllerInfo.Destroy(ref HitChartacterinfo);
                CharacterControllerInfo.Destroy(ref selfChartacterinfo);
            }

            m_hitCharacter = null;
        }
    }

    private void applyWing(bool fly)
    {
        cmd_creature_set_wing_state data;
        if (fly)
        {
            data.nType = WING_STATE_TYPE.WING_STATE_START_FLY;
        }
        else
        {
            data.nType = WING_STATE_TYPE.WING_STATE_FLOOR;
        }
        data.nEncounterFlag = 0;

        IntPtrHelper helper = new IntPtrHelper();
        int len = Marshal.SizeOf(data);
        IntPtr ptr = helper.toPtr<cmd_creature_set_wing_state>(ref data);
        int eventID = (int)EntityLogicDef.ENTITY_CMD_SET_WING_STATE;
        GameLogicAPI.onEntityEvent(entityID, eventID, len, "", ptr, len);
    }

    /// ///////////////////////////////////////////////////////////////////////////////////////////
    public override bool DoAct(ActID id)
    {
        // 当前状态下不能做某种行为
        if (m_CurStateHandler == null)// || m_CurStateHandler.CanAct(id) == false)
        {
            return false;
        }


        if (doActHandlers[(int)id] != null)
        {
            return doActHandlers[(int)id](id, this);
        }

        return false;
    }

    // 状态切换完成，处理一下小地图的显示隐藏
    // 因为各个状态onLeave onEnter的时候 状态还没切换完
    private void AfterStateChange(int oldState)
    {
        int curState = m_CurStateHandler.GetID();

        if (curState == (int)EntityState.Dead)
        {
            // 进入死亡状态 通知小地图隐藏
            U3D_Render.EntityView objev = EntityFactory.getEntityViewByID(entityID);
            if (objev != null)
            {

                LogicDataCenter.warMinimapDataManager.UpdateObjectVisiblity(objev);
            }
        }

        if (oldState == (int)EntityState.Dead)
        {
            // 离开死亡状态 通知小地图显示
            U3D_Render.EntityView objev = EntityFactory.getEntityViewByID(entityID);
            if (objev != null)
            {
                LogicDataCenter.warMinimapDataManager.UpdateObjectVisiblity(objev);
            }

        }
    }

    /// ///////////////////////////////////////////////////////////////////////////////////////////
    public override bool ChangeState(int id, IntPtr lParam)
    {
        if (m_CurStateHandler != null && m_CurStateHandler.GetID() == id)
        {
            return m_stateHandlers[id].ReEnter(lParam);
        }

        if (id < 0 || id >= (int)EntityState.MaxCount)
            return false;

        if (m_stateHandlers[id].OnEnter(lParam) == false) { return false; }

        int oldState = (int)EntityState.Standing;
        if (m_CurStateHandler != null)
        {
            m_CurStateHandler.OnLeave();
            oldState = m_CurStateHandler.GetID();
        }

        if (m_CurStateHandler != null && m_stateHandlers[id] != null)
        {
            m_stateHandlers[id].setVelocity(m_CurStateHandler.getVelocity());
        }

        m_CurStateHandler = m_stateHandlers[id];
        string newStateName = ((EntityState)id).ToString();
        if (showLog)
        {
            Trace.Log("Change to State" + newStateName);
        }
        //a.SetTrigger(newStateName);
        //a.runtimeAnimatorController = m_stateAnimators[id];

        AfterStateChange(oldState);

        return true;
    }

    public override EntityState GetState()
    {
        if (m_CurStateHandler == null)
        {
            return EntityState.Dead;
        }
        return (EntityState)m_CurStateHandler.GetID();
    }

    public override bool IsCurAct(string tag, int layer = 0)
    {
        AnimatorStateInfo st = animator.GetCurrentAnimatorStateInfo(layer);
        return st.IsTag(tag);
    }

    //public void OnAnimatorIK(int layerIndex) // Unity Pro only!
    //{
    //    if (m_CurStateHandler != null)
    //    {
    //        m_CurStateHandler.OnAnimatorIK(layerIndex);
    //    }
    //}

    //以下代码处理地面相关的属性    
    public Vector3 lastHitPoint = Vector3.zero; //Transport会更新此值
    private Vector3 lastGroundNormal = Vector3.zero;

    public void OnTriggerEnter(Collider c)
    {
        if (c != null)
        {
            MonsterStateMachine m = c.transform.parent.GetComponent<MonsterStateMachine>();
            if (m != null)
            {
                EntityView cView = EntityFactory.getEntityViewByID(m.entityID);
                if (cView != null && cView.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_HP_SUPPLIES)
                {
                    cmd_pick_hp data = new cmd_pick_hp();
                    data.entityID = m.entityID;
                    //Debug.Log("pick HP！！！！！！！！！！！！:   " + c.name);
                    EntityEventHelper.Instance.SendCommand<cmd_pick_hp>(entityID, EntityLogicDef.ENTITY_CMD_PICK_HP, ref data);
                }
            }
        }
    }


    private CharacterController m_hitCharacter = null;
    private ControllerColliderHit m_lastHitInfo = null;
    public FORM_TYPE m_formtype = FORM_TYPE.FORM_GROUND;

    //private int crossCharacterCount = 0;
    public void OnControllerColliderHit(ControllerColliderHit hit)
    {
        // Since box colliders report problematic normals when colliding with edges, ignore those.

        if (hit != m_lastHitInfo) //判断人踩人
        {
            //所有状态都有判断，避免冲锋之类的冲到头顶上，具体的判断逻辑再后面进行
            //if (GetState() == EntityState.Jumping) 
            {
                m_hitCharacter = hit.collider as CharacterController;
                m_lastHitInfo = hit;
            }
        }

        Vector3 normal = (hit.collider.GetType() == typeof(BoxCollider) ? Vector3.up : hit.normal);
        if (normal.y > 0 && normal.y > groundNormal.y && hit.moveDirection.y < 0)
        {
            //if (hitCharacter != null && crossCharacterCount < 4)
            //{
            //    if (hitCharacter.radius < 1.0f)
            //    {
            //        this.controller.Move(transform.forward * hitCharacter.radius * 2.0f);

            //        crossCharacterCount++; //踩到其他玩家了，往前走一步，已经走过的记录一下，最多走3次，不再走了
            //        return;
            //    }
            //}
            //crossCharacterCount = 0;

            Vector3 recGroundNormal = groundNormal;

            if ((hit.point - lastHitPoint).sqrMagnitude > 0.001f || lastGroundNormal == Vector3.zero)
            {
                groundNormal = normal;
                lastHitPoint = hit.point;
                lastGroundNormal = groundNormal;
            }
            else
            {
                groundNormal = lastGroundNormal;
            }
        }

    }

    private uint m_uLastReportFormTick = 0;
    private uint m_uLastCheckFormTick = 0;
    //客户端每100毫秒获取一次离地高度，获取的信息传给客户端逻辑层。
    //逻辑层根据离地高度和翅膀状态进行着地点类型判断：
    //1、翅膀状态为飞行或下落，高度大于等于5米，为高空
    //2、翅膀状态为飞行或下落，高度小于5米，为地面
    //3、不在翅膀状态，高度大于等于5米，为高台
    //4、不在翅膀状态，高度小于5米，为地面
    //判断后将结果传给服务器，判断结果跟角色当前类型不符时立即传一次，结果跟当前相符时间隔5秒传一次
    public void updateFormType()
    {
        uint tick = GameLogicAPI.getTickCount();

        if (Math.Abs(tick - m_uLastCheckFormTick) < 100)
        {
            return;
        }
        m_uLastCheckFormTick = tick;


        FORM_TYPE lastFormtype = m_formtype;

        RaycastHit hitInfo;
        LayerMask collisionLayers = (1 << LayerMask.NameToLayer(Config.LayerDefault));
        Vector3 pos = transform.position;
        pos.y += 0.5f;
        Ray PointToGroundRay = new Ray(pos, new Vector3(0, -1, 0));
        Physics.Raycast(PointToGroundRay, out hitInfo, 100, collisionLayers);
        if ((pos.y - hitInfo.point.y) > 5.5f)
        {
            if (GetState() == EntityState.Floating)
            {
                m_formtype = FORM_TYPE.FORM_SKY;
            }
            else
            {
                m_formtype = FORM_TYPE.FORM_HATHPACE;
            }
        }
        else
        {
            m_formtype = FORM_TYPE.FORM_GROUND;
        }

        //上报
        if (lastFormtype != m_formtype || Math.Abs(tick - m_uLastReportFormTick) > 5000)
        {
            cmd_creature_act_set_formtype data = new cmd_creature_act_set_formtype();
            data.nValue = (int)m_formtype;
            EntityEventHelper.Instance.SendCommand<cmd_creature_act_set_formtype>(entityID, EntityLogicDef.ENTITY_CMD_SET_FORMTYPE, ref data);
            m_uLastReportFormTick = tick;
        }
    }

    private void updateState()
    {
        if (m_CurStateHandler == null)
        {
            return;
        }

        if (isHero)
        {
            if (m_CurStateHandler.GetID() == (int)EntityState.Standing) //判断离地高度，进入跌落状态
            {
                Vector3 pos = transform.position + new Vector3(0, 0.1f, 0);
                if (Mathf.Abs(pos.y - lastHitPoint.y) > 0.5f)
                {
                    RaycastHit hitInfo;
                    Ray PointToGroundRay = new Ray(pos, new Vector3(0, -1, 0));
                    Physics.Raycast(PointToGroundRay, out hitInfo, 100, collisionLayers);
                    if ((pos.y - hitInfo.point.y) > controller.height + 0.6f)
                    {
                        ChangeState((int)EntityState.Jumping, IntPtr.Zero);
                    }
                    else
                    {
                        lastHitPoint = pos;
                    }
                }
            }
        }
    }

    public override bool IsGrounded()
    {
        return m_CurStateHandler.IsGrounded();
    }


    public override bool isFloating()
    {
        return m_CurStateHandler.isFloating();
    }

    /// ///////////////////////////////////////////////////////////////////////////////////////////
    public override bool DoJump(ActID id, BaseStateMachine fsm)
    {
        return fsm.ChangeState((int)EntityState.Jumping, IntPtr.Zero);
    }


    public override bool ChangeWingState(IntPtr ptrParam)
    {
        cmd_Entity_Change_Wing_State param = IntPtrHelper.toData<cmd_Entity_Change_Wing_State>(ptrParam);

        if (GetState() != EntityState.Floating)
        {
            if (param.nType == WING_STATE_TYPE.WING_STATE_START_FLY)
            {
                return ChangeState((int)EntityState.Floating, ptrParam);
            }
            else if (!isHero && (param.nType == WING_STATE_TYPE.WING_STATE_NORMAL_FLIGHT || param.nType == WING_STATE_TYPE.WING_STATE_ACCELERATION))
            {
                return ChangeState((int)EntityState.Floating, ptrParam);
            }
            return false;
        }

        m_CurStateHandler.ChangeState(ptrParam);

        return true;
    }

    public override bool UpdateGlideData(IntPtr ptrParam)
    {
        cmd_creature_glide param = IntPtrHelper.toData<cmd_creature_glide>(ptrParam);
        glideSpeedGround = param.fGlideSpeed; //滑翔状态下,在平地时候的速度增幅
        glideDownSpeedAcceleration = param.fFallSpeed;  //滑翔下落加速度
        isGlideState = true;
        return true;
    }
    public override bool ChangeGlideState()
    {
        if (m_CurStateHandler.GetID() == (int)EntityState.Jumping)
        {
            ChangeState((int)EntityState.Glide, IntPtr.Zero);
            m_CurStateHandler.ChangeState(IntPtr.Zero);
        }

        Debug.Log("Change Glide State:");
        return true;
    }


    public override void CtrlSync(ref cmd_creature_act_sync ctrl)
    {

        // 同步状态

        // 同步操作
        switch ((ActID)ctrl.nCtrID)
        {
            case ActID.Jump:
                inputJump = true;
                DoAct(ActID.Jump);
                break;
            default:
                Trace.Log("invalid ctrl id" + ctrl.nCtrID.ToString());
                break;

        }

        // 同步位置 2017-05-05暂时取消位置同步，这个位置是000
        //PositionSync(ref ctrl.pos);

        if (m_CurStateHandler == null)
        {
            Trace.LogError("CreatureStateMachine.cs CtrlSync m_CurStateHandler == null");
            return;
        }

        EntityBatchCommunicator.SendEntityPro(m_CurStateHandler.GetID(),
            new Vector3(ctrl.pos.fPosition_x, ctrl.pos.fPosition_y, ctrl.pos.fPosition_z),
            new Vector3(ctrl.pos.fRotation_x, ctrl.pos.fRotation_y, ctrl.pos.fRotation_z));
    }

    public override void PositionSync(ref cmd_creature_rigidbody_sync sync)
    {
        // 同步位置
        syncVelocity = new Vector3(sync.fVelocity_x, sync.fVelocity_y, sync.fVelocity_z);
        syncDetalTime = (float)sync.nDelayTick / 1000.0f;
        syncPosition = new Vector3(sync.fPosition_x, sync.fPosition_y, sync.fPosition_z);
        syncAngle = new Vector3(sync.fRotation_x, sync.fRotation_y, sync.fRotation_z);

        m_CurStateHandler.PositionSync(ref sync);
    }



    public override void FaceTo(Vector3 targetPos)
    {
        //if (showLog)
        //{
        //    Trace.Log("Rotate to" + targetPos.ToString());
        //}

        targetPos.y = transform.position.y;
        animatorCtrl.FaceTo(targetPos);
    }

    public void syncPos()
    {
        int curTick = System.Environment.TickCount;
        syncTick = curTick;
        //syncVelocity = Vector3.zero;
        syncDetalTime = 0;
        syncPosition = transform.position;
        syncAngle = transform.eulerAngles;
        if (isHero && !bControlledBySever)
        {
            //完成后向逻辑层回报一次同步，请根据需求取舍
           m_CurStateHandler.Send_Sync_Position(MoveSyncFlag.TO_BROADCAST);
        }
    }


    public void reportPos()
    {
        if ((syncPosition - transform.position).sqrMagnitude < 0.01f) //位移小于0.1不上报
        {
            return;
        }

        //如果上报位置范围不对，则输出至LOG，退出
        if (float.IsNaN(transform.position.x) || float.IsNaN(transform.position.y) || float.IsNaN(transform.position.z) || Math.Abs(transform.position.x) > SceneManager.MAX_SCENE_SIZE || Math.Abs(transform.position.y) > SceneManager.MAX_SCENE_SIZE || Math.Abs(transform.position.z) > SceneManager.MAX_SCENE_SIZE)
        {
            Debug.LogError(name + " reportPos Position Error " + transform.position);
            return;
        }

        //逻辑层回报一次同步，请根据需求取舍
        cmd_creature_rigidbody_sync data = new cmd_creature_rigidbody_sync();
        data.nCreatureState = (int)GetState();
        data.fPosition_x = transform.position.x;
        data.fPosition_y = transform.position.y;
        data.fPosition_z = transform.position.z;

        // 直接用欧拉角
        data.fRotation_x = 0.0f;
        data.fRotation_y = transform.eulerAngles.y;
        data.fRotation_z = 0.0f;

        data.fVelocity_x = 0.0f;
        data.fVelocity_y = 0.0f;
        data.fVelocity_z = 0.0f;
        data.nSyn2Server = (int)MoveSyncFlag.TO_BROADCAST;

        EntityEventHelper.Instance.SendCommand<cmd_creature_rigidbody_sync>(entityID, EntityLogicDef.ENTITY_CMD_SET_POS, ref data);

        syncPosition = transform.position;
        syncAngle = transform.eulerAngles;
        syncVelocity = Vector3.zero;
    }

    //非主角在技能移动时也上报位移，用于同步电脑玩家
    public bool reportSkillMove
    {
        get
        {
            U3D_Render.EntityView view = EntityFactory.getEntityViewByID(entityID);
            if (view != null)
            {
                return view.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_AI_SCHEME_ID) > 0;
            }
            return false;
        }
    }

    public override void ForceMove(cmd_force_move param)
    {
        if (moveCtrl != null)
        {
            moveCtrl.ForceMove(param);
        }
    }
    public override void StopAutoMove(bool userStop = true)
    {
        if (moveCtrl != null)
        {
            moveCtrl.StopAutoMove(userStop);
        }
        //速度改为服务器下发，注释
        //if (animatorCtrl != null)
        //{

        //    animatorCtrl.setAnimatorSpeed(!userStop, 0.0f);
        //}
    }
    public override void AutoMoveTo(Vector3 targetPos, bool rotateCamera = true)
    {
        if (moveCtrl != null)
        {
            moveCtrl.AutoMoveTo(targetPos, this.isFloating(), rotateCamera);
        }
    }
    public override void ForceFlight(cmd_effect_Flight param)
    {
        if (moveCtrl != null)
        {
            moveCtrl.ForceFlight(param);
        }
    }


    public Vector3 getInputMoveDirection()
    {
        if (inputMoveDirection.sqrMagnitude < 0.0001f)
        {
            return rawInputMoveDirection;
        }
        else
        {
            return inputMoveDirection;
        }
    }
    public void UpdatemaxForwardSpeed()
    {
        var MaxForwardCache = maxForwardSpeed;
        maxForwardSpeed = maxRunForwardSpeed;
        if (creaturePropety != null && creaturePropety.enableDodge && dodge)
        {
            // maxForwardSpeed = maxRunForwardSpeed + dodgeSpeedGround;
            maxForwardSpeed += dodgeSpeedGround;
        }
        else if (m_CurStateHandler.GetID() == (int)EntityState.Glide)
        {
            maxForwardSpeed = maxGlideSpeed;
        }
        else if (isAutoForward)
        {
            maxForwardSpeed = autoForwardSpeed;
        }
        //else
        //{
        //        maxForwardSpeed = maxRunForwardSpeed;
        //}
        if (MaxForwardCache!= maxForwardSpeed)
        {
            updateMovingAnimSpeed();
        }
    }

    public override void PlayAnim(string name, bool react = false, float speed = 1.0f, float durationTime = 0.0f, float normalizedTime = 0.0f)
    {
        animatorCtrl.PlayAnim(name, react, speed, durationTime, normalizedTime);
        if (sidekickEntity)
        {
            sidekickEntity.playAnim(name);
        }
    }
    public override void PlayWeaponAnim(string name, int targetID, float speed = 1.0f)
    {
        animatorCtrl.PlayWeaponAnim(name, targetID, speed);
    }
    public override void BreakAttackAnim(string animName = "", float crossFadeTime = 0.1f)
    {
        if (showLog)
        {
            Trace.Log("BreakAttackAnim");
        }
        if (animatorCtrl != null)
        {
            animatorCtrl.BreakAttackAnim(animName, crossFadeTime);
        }
        if (sidekickEntity)
        {
            sidekickEntity.breakAnim();
        }
    }
    //服务器传下来的移动速度改变时，调用这个函数，更新下移动动画的速率，造成快跑或者慢跑的效果
    public override void updateMovingAnimSpeed()
    {
        if (maxForwardSpeed > 0.0f) //其他玩家正常速度的时候传下来可能是0，这里先挡一下
        {
            var param = 1.2f;//移动动画增加系数
            float animatorSpeed = maxForwardSpeed / creaturePropety.runSpeed_Forward;
            if (animatorSpeed > 1) animatorSpeed = 1 + (animatorSpeed - 1) * param;
            animatorCtrl.updateMovingAnimSpeed(animatorSpeed);
        }
    }

    public void LateUpdate()
    {
        if (animatorCtrl != null)
        {
            animatorCtrl.LateUpdate();
        }

        CheckGroundLayer();
    }

    public override void Transport(cmd_creature_transport pos)
    {
        if (moveCtrl != null)
        {
            Vector3 moveTo = new Vector3(pos.fPosition_x, pos.fPosition_y, pos.fPosition_z);
            Vector3 dis = transform.position - moveTo;
            moveCtrl.Transport(pos);

            //传送的时候移动一点点，应用碰撞
            Vector3 tempPos = Vector3.one;
            tempPos.y = 0;
            controller.Move(tempPos * 0.01f);

            //2017-04-12，光效离主角很近时不会被距离优化，所以暂时屏蔽此代码
            //if (isHero && dis.sqrMagnitude>40*40)
            //{
            //    //目前最大技能冲锋30
            //    //因为镜头采用的是渐进是跟随，当遇到死亡后复活之类大距离传送时，
            //    //镜头不能马上跟上，会造成在此期间加的光效因为与摄像机距离过大而无法加上去
            //    //所以这里做一个强制的摄像机位置更新，不用渐进式更新。
            //    SoldierCamera.MainInstance<SoldierCamera>().ForceUpdateTransformNormally();
            //}
        }
    }

    public override void SyncPos2Server()
    {
        if (m_CurStateHandler != null)
        {
            m_CurStateHandler.Send_Sync_Position(MoveSyncFlag.TO_SERVER);
        }
    }

    public override void SetVisible(bool bVisible)
    {
        if (SkinControl)
        {
            SkinControl.SetVisible(bVisible);
        }
        base.SetVisible(bVisible);

        if (sidekickEntity)
        {
            sidekickEntity.setActive(bVisible);
        }
    }

    public override void gotoGuard(bool bGuard)
    {
        if (bGuard)
            syncInterval = 50;
        else
            syncInterval = 300;
    }

    //private int m_controllerEnableCount=1;
    public override void enableCollider(bool enable)
    {
        if (enable)
        {
            pickableCount++;
        }
        else
        {
            pickableCount--;
        }
        //if (enable)
        //{
        //    m_controllerEnableCount++;
        //}
        //else
        //{
        //    m_controllerEnableCount--;
        //}

        //if (controller)
        //{
        //    if (m_controllerEnableCount > 0)
        //    {
        //        controller.enabled = true;
        //    }
        //    else
        //    {
        //        controller.enabled = false;
        //    }
        //}
    }


    #region Mask相关

    /// <summary>
    /// 改变mask
    /// </summary>
    /// <param name="mas"></param>
    /// <param name="ev"></param>
    public override void ChangeMask(U3D_Render.EntityView ev)
    {
        if (null == ev)
        {
            return;
        }
        int mas = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);

        ////0不处理
        //if (mas == 0)
        //{
        //    return;
        //}
        Mask_Visible(ev);
        Mask_Collider(ev);
        Mask_Not_Move(ev);
    }

    /// <summary>
    /// Mask--可见性
    /// </summary>
    /// <param name="ev"></param>
    /// <param name="bUseInitMask">使用初始化时候的Mask</param>
    /// <returns></returns>
    private void Mask_Visible(U3D_Render.EntityView ev, bool bUseInitMask = false)
    {

        int mas = 0;
        if (bUseInitMask)
        {
            mas = ev.InitMaskFlag;
        }
        else
        {
            mas = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);
            if (!bFirstVisibleMask)
            {
                bFirstVisibleMask = true;
                return;
            }

        }

        bool bHideFriend = (mas & (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_FRIEND) == (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_FRIEND;
        bool bHideEnemy = (mas & (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_ENEMY) == (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_ENEMY;
        //两者都不隐藏
        if (!bHideFriend && !bHideEnemy)
        {
            // setAlpha(1.0f);

            m_MaskVisible = true;
        }
        //对友方隐藏
        else if (bHideFriend && !bHideEnemy)
        {
            if (ev.CampFlag == SPELL.CampFlag.CampFlag_Friend ||
                ev.CampFlag == SPELL.CampFlag.CampFlag_Self)
            {
                //setAlpha(0.5f);

                m_MaskVisible = false;
            }
        }
        //对非友方隐藏
        else if (!bHideFriend && bHideEnemy)
        {
            if (ev.CampFlag != SPELL.CampFlag.CampFlag_Friend &&
                ev.CampFlag != SPELL.CampFlag.CampFlag_Self)
            {
                //setAlpha(0.5f);

                m_MaskVisible = false;
            }
        }
        //对两者都隐藏
        else
        {
            //setAlpha(1.0f);
            m_MaskVisible = false;

        }

        UpdateVisible();
    }

    /// <summary>
    /// Mask--碰撞
    /// </summary>
    /// <param name="ev"></param>
    private void Mask_Collider(U3D_Render.EntityView ev)
    {

    }

    /// <summary>
    /// Mask-不移动
    /// </summary>
    /// <param name="ev"></param>
    private void Mask_Not_Move(U3D_Render.EntityView ev)
    {
        int mas = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);

        int flag = (int)ENTITY_MASKFLAG.MASKFLAG_NOT_MOVE;
        bNotMove = (mas & flag) == flag;

    }
    #endregion

    #region 皮肤相关

    /// <summary>
    /// 当皮肤已经被改变-消息通知
    /// </summary>
    /// <param name="nSkinID">皮肤ID</param>
    /// <param name="nPart">哪个部位</param>
    protected override void OnSkinChanged(SkinInstance sk, int nPart)
    {
        if (SkinInstance.isNullOrEmpry(sk))
        {
            return;
        }
        //皮肤切换的时候刷新 一下控制器
        RefershCharacterController();

        int lastCameraType = -1;
        if (creaturePropety != null)
        {
            shineScale = creaturePropety.shineScale;
            shineSeconds = creaturePropety.shineSeconds;
            g_highLightColor = creaturePropety.highLightColor;
            fDelightTime = creaturePropety.delightTime;
            fMaxHighLightAlpha = creaturePropety.maxHighLightAlpha;
            lastCameraType = creaturePropety.cameraType;
        }

        ResetBone();
        animatorCtrl.firsttime = true;


        ChangeStateMachineParams();
        ChangeCreaturePropety();
        CopyCharacterController();
        if (SoldierCamera.MainInstance() != null && lastCameraType != creaturePropety.cameraType && EntityFactory.MainHeroID == entityID)
        {
            SoldierCamera.MainInstance<SoldierCamera>().updateCameraParam(-1, creaturePropety.cameraType);
        }

        ChangeCameraProperty();

        gameObject.name = SkinManager.GetSkinName(sk.nSkinID) + "(entityID:" + entityID + ")";

        base.OnSkinChanged(sk, nPart);

        //dodge = false; //切换皮肤是停止轻功，避免轻功状态传递给下个英雄//--
        soliderController = GetComponent<SoldierController>();
        if (animatorCtrl != null)
        {
            animatorCtrl.OnSkinChanged(sk);
            animatorCtrl.UpdateAnimatorScript(true);
            updateMovingAnimSpeed();
        }
        EntityView ev = EntityFactory.getPlayerViewByID(this.entityID);
        if (sk.playEntityStaticEffect)
        {
            sk.playEntityStaticEffect.ResetLayer();
            sk.playEntityStaticEffect.Stop();
            sk.playEntityStaticEffect.Play(ev);
        }
        if (sk.switchEffectWithHeroDistance)
        {
            sk.switchEffectWithHeroDistance.ResetLayer();
            sk.switchEffectWithHeroDistance.SetEntitiyView(ev);
        }

        if (GetState() == EntityState.Floating)
        {
            m_CurStateHandler.ApplyAnimation();
        }
        else if (GetState() == EntityState.Dead)
        {
            m_CurStateHandler.ApplyAnimation();
        }
        else if (dodge)
        {
            if (creaturePropety.enableDodge)
            {
                Vector3 pos = transform.position + new Vector3(0, 0.1f, 0);
                RaycastHit hitInfo;
                Ray PointToGroundRay = new Ray(pos, new Vector3(0, -1, 0));
                Physics.Raycast(PointToGroundRay, out hitInfo, 100, collisionLayers);
                animatorCtrl.ana.SetBool("isDodge", true);
                if ((pos.y - hitInfo.point.y) > controller.height + 0.6f)
                {
                    ChangeState((int)EntityState.Jumping, IntPtr.Zero);
                    animator.CrossFadeInFixedTime("DodgeJump_Front", 0.1f);
                    animatorCtrl.ana.SetBool("isMoving", true);
                    //在空中
                }
                else
                {
                    //animatorCtrl.ana.SetBool("isMoving", true);
                    //animatorCtrl.ana.SetBool("isLanding", true);

                    //在地面
                    //animator.CrossFadeInFixedTime("DodgeRun_Front", 0.1f);
                }

            }
            else
                dodge = false;
        }

        //if (controller && controller.enabled)
        //{
        //    m_controllerEnableCount = 1;
        //}
        //else
        //{
        //    m_controllerEnableCount = 0;
        //}
      //  Debug.Log(" 状态机初始化完成");
    }


    protected override void OnSkinCull()
    {
        if (animatorCtrl != null)
        {
            ResetBone();
            animatorCtrl.OnSkinCull();
        }

    }

    protected override void OnSkinNotCull()
    {
        if (animatorCtrl != null)
        {
            animatorCtrl.OnSkinNotCull();
        }
    }

    public void ResetBone()
    {
        if (animatorCtrl != null)
        {
            animatorCtrl.ResetBones();
        }
    }

    private void ChangeStateMachineParams()
    {
        if (null == SkinConfig)
        {
            return;
        }

        if (SkinConfig.Main == null || SkinConfig.Root_M == null)
        {
            SkinConfig.Root_M = null;
            SkinConfig.BackA_M = null;
        }


        SkinConfig.adjustSpine = true;
        //SkinConfig.separingBody = false;

        //mWeaponAnimator = null;
        //if (SkinConfig.WeaponObject != null)
        //{
        //    mWeaponAnimator = SkinConfig.WeaponObject.GetComponent<Animator>();
        //}
        //if (castLayer)
        //{
        //    castLayer.CastlayerObject = SkinConfig.ScreenCastObject;
        //    castLayer.Start();
        //}
    }

    private void ChangeCreaturePropety()
    {
        CreatureProperty dst = SkinControl.CurrentSkinInstance.creatureProperty;
        if (!dst)
        {
            return;
        }

        //速度服务器会下发，客户端不用赋值
        //maxRunBackwardSpeed = dst.runSpeed_Back;
        //maxRunForwardSpeed = dst.runSpeed_Forward;
        //maxRunSidewaySpeed = dst.runSpeed_LeftRight;

        checkPosition = dst.checkPosition;

        creaturePropety.Copy(dst);

        SkillCtrl skillctrl = GetComponent<SkillCtrl>();
        if (skillctrl != null)
        {
            skillctrl.resetRayCast();
        }

        if (isHero)
        {
            if (SoldierCamera.MainInstance() != null)
            {
                if (dst.FPSMode)
                {
                    SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.FPSMode, true);
                }
                else
                {
                    //SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.FPSMode, false);
                    SoldierCamera.MainInstance<SoldierCamera>().SwitchCamModeDefault();
                }
            }
        }

        ////主角需要更改按键信息
        //if (EntityFactory.MainHeroID == entityID)
        //{
        //    if (skillctrl == null)
        //    {
        //        skillctrl = creature.GetComponent<SkillCtrl>();
        //    }

        //    skillctrl.resetOrderset();//creaturePropety.SkillOrderSetName,false);
        //    //在载入英雄插槽后通知，通知逻辑层
        //    GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_INITAL_SLOT, 0, "", IntPtr.Zero, 0);
        //}

    }

    private void CopyCharacterController()
    {
        if (SkinInstance.isNullOrEmpry(SkinControl.CurrentSkinInstance))
        {
            return;
        }
        CharacterController src = SkinControl.CurrentSkinInstance.characterController;
        if (!src)
        {
            return;
        }


        controller.slopeLimit = src.slopeLimit;
        controller.stepOffset = src.stepOffset;
        controller.center = src.center;
        controller.radius = src.radius;
        controller.height = src.height;
        controller.skinWidth = src.radius * 0.1f;

        //Unity的组件是native，无法使用反射
        // Api.CopyComponentFields(src, controller);

    }

    private void ChangeCameraProperty()
    {
        //CameraProperty cpp = SkinControl.CurrentSkinInstance.cameraProperty;
        //if (!cpp)
        //{
        //    return;
        //}

        ////只有主角才会控制主相机
        //if (EntityFactory.MainHeroID == entityID)
        //{
        //    //互相绑定
        //    SoldierController sc = GetComponent<SoldierController>();
        //    if (sc != null)
        //    {
        //        sc.solidermode = cpp.soldiermode;
        //        if (cpp.soldiermode == 2) //控制模式2的时候初始默认1，（策划特殊需求）
        //        {
        //            sc.controlmode = 1;
        //        }
        //        else
        //        {
        //            sc.controlmode = cpp.soldiermode;
        //        }
        //        sc.bindSoldierCamera = SoldierCamera.MainInstance<SoldierCamera>();
        //        sc.desireAngle.y = sc.bindSoldierCamera.userDesireAngle.y;
        //        SoldierCamera.MainInstance<SoldierCamera>().soldier = sc.transform;
        //    }

        //    if (SoldierCamera.MainInstance<SoldierCamera>().CurCameraProperty==null || SoldierCamera.MainInstance<SoldierCamera>().CurCameraProperty.configType == 0)
        //    {
        //        SoldierCamera.MainInstance<SoldierCamera>().Copy(cpp);
        //    }
        //    else
        //    {
        //        //其他的不需要覆盖，但是FPS优先配置需要覆盖
        //        SoldierCamera.MainInstance<SoldierCamera>().CurCameraProperty.bFPSPriority = cpp.bFPSPriority;
        //        if (cpp.bFPSPriority)
        //        {
        //            SoldierCamera.MainInstance<SoldierCamera>().CurCameraProperty.NormalDistance = cpp.NormalDistance;
        //            SoldierCamera.MainInstance<SoldierCamera>().CurCameraProperty.DistanceClamp = cpp.DistanceClamp;
        //            SoldierCamera.MainInstance<SoldierCamera>().CurCameraProperty.FPSTargetOffset = cpp.FPSTargetOffset;
        //        }
        //
        //        SoldierCamera.MainInstance<SoldierCamera>().CurCameraProperty.InitedEulerAngle.y = gameObject.transform.eulerAngles.y;
        //        SoldierCamera.MainInstance<SoldierCamera>().Reset();
        //    }


        ////只有主角才会控制主相机
        if (EntityFactory.MainHeroID == entityID)
        {
            //互相绑定
            SoldierController sc = GetComponent<SoldierController>();
            if (sc != null)
            {
                if (sc.controlmode != 2)
                {
                    sc.controlmode = 1; //2017-02-20龙母上龙的时候需要mode2
                }
                sc.bindSoldierCamera = SoldierCamera.MainInstance<SoldierCamera>();
                sc.desireAngle.y = sc.bindSoldierCamera.userDesireAngle.y;
                SoldierCamera.MainInstance<SoldierCamera>().soldier = EntityFactory.MainHeroView;
            }

            SoldierCamera.MainInstance<SoldierCamera>().CurCameraProperty.InitedEulerAngle.y = gameObject.transform.eulerAngles.y;
            SoldierCamera.MainInstance<SoldierCamera>().Reset(true);
        }


    }


    #endregion

    #region 自动前进系统
    private bool isAutoForward = false;
    private float autoForwardSpeed = 1.0f;

    public void EnableAutoForward(float maxspeed, bool enable = true)
    {
        autoForwardSpeed = maxspeed;
        isAutoForward = enable;
    }

    #endregion


    #region 轻功系统
    private bool m_isDodge = false;

    public float dodgeSpeedGround = 1.0f; //轻功状态下,在平地时候的速度增幅

    public float[] dodgeSpeeds = new float[3] { 3, 6, 9 };   //轻功状态的第一次加速速度



    public int dodgeSkillID = 0; //为轻功设置一个SkillID，退出轻功时使用此技能
    public bool dodge
    {
        get { return m_isDodge; }
        set
        {
            m_isDodge = value;
            //Debug.Log("isDogSetvalue " + value);
            //if (creaturePropety != null)
            //    Debug.Log("   creaturePropety.enableDodge: " + creaturePropety.enableDodge);//-
            if (creaturePropety != null && !creaturePropety.enableDodge)
            {
                m_isDodge = false;
                return;
            }
            if (GetState() == EntityState.Dead && m_isDodge)
            {
                Debug.Log("死亡不能进入轻功");
                m_isDodge = false;
                return;
            }

            //只有向前行走一种方向，所以只改一个最大速度
            //  maxDodgeSpeed = maxForwardSpeed + dodgeSpeedGround;

            if (soliderController != null && soliderController.isAttackMode() && m_isDodge)
            {
                Debug.Log("轻功功能在攻击状态是不能进入的，这里强制退出攻击状态");
                soliderController.controlmode = 1;
            }
            if (animatorCtrl != null && animatorCtrl.ana != null)
            {
                //Debug.Log("m_isDodge:" + m_isDodge);
                animatorCtrl.ana.SetBool("isDodge", m_isDodge);
            }
        }
    }
    #endregion

    #region 滑翔系统
    public bool isGlideState = false;
    public float maxGlideSpeed = 0;
    public float glideSpeedGround = 6.0f; //滑翔状态下,在平地时候的速度增幅
    public float glideUpHeight = 10.0f;   //滑翔状态的跳跃高度
    public float glideDownSpeedAcceleration = 2.5f; //降落状态的下落加速度
    public cmd_creature_glide GlideParam;
    #endregion

    #region 透视人的效果
    private List<int> m_XRayEntityID = new List<int>();
    public void setXRayEffects(cmd_creature_PerspectiveShow data)
    {
        if (m_XRayEntityID == null)
        {
            m_XRayEntityID = new List<int>();
        }
        bool enable = (data.nType != 0);
        if (enable)
        {
            List<int> newXRayEntityID = new List<int>(data.uidTarget);
            if (newXRayEntityID == null)
            {
                return;
            }
            foreach (int nID in m_XRayEntityID)
            {
                if (nID > 0 && !newXRayEntityID.Contains(nID))
                {
                    U3D_Render.EntityView view = EntityFactory.getEntityViewByID(nID);
                    if (view != null && view.StateMachine != null)
                    {
                        view.StateMachine.setXRay(false);
                    }
                }
            }

            foreach (int nID in newXRayEntityID)
            {
                if (nID > 0 && !m_XRayEntityID.Contains(nID))
                {
                    U3D_Render.EntityView view = EntityFactory.getEntityViewByID(nID);
                    if (view != null && view.StateMachine != null)
                    {
                        view.StateMachine.setXRay(true);
                    }
                }
            }

            m_XRayEntityID.Clear();
            m_XRayEntityID = newXRayEntityID;
        }

        else
        {
            foreach (int nID in m_XRayEntityID)
            {
                U3D_Render.EntityView view = EntityFactory.getEntityViewByID(nID);
                if (view != null && view.StateMachine != null)
                {
                    view.StateMachine.setXRay(false);
                }
            }
            m_XRayEntityID.Clear();
        }
    }
    #endregion

    #region AI翻滚

    public void RollToPos(Vector3 targetPos)
    {
        int face = calFace(ref targetPos);

        string rollAnimationName = GetRollAnimName(face);

        this.animatorCtrl.PlayAnim(rollAnimationName);

        //同步
        //syncPos();
    }

    private string GetRollAnimName(int face)
    {
        string[] rollAnimName = new string[] { "fg01", "fg02", "fg03", "fg04" };
        return rollAnimName[face];
    }
    private int calFace(ref Vector3 targetPos)
    {
        Vector3 targetDir = targetPos - transform.position;
        targetDir.y = 0.0f;
        targetDir.Normalize();
        Vector3 faceRight = Vector3.Cross(transform.up, targetDir);
        faceRight.Normalize();
        Vector3 turnTo = targetDir;

        float forward = Vector3.Dot(targetDir, transform.forward);
        float right = Vector3.Dot(targetDir, transform.right);

        int output = 0;

        if (Mathf.Abs(forward) > Mathf.Abs(right))
        {
            right = 0.0f;
            if (forward < 0.0f)
            {
                forward = -1.0f;
                turnTo *= -1.0f;
                output = 1;
            }
            else
            {
                forward = 1.0f;
                output = 0;
            }
        }
        else
        {
            forward = 0.0f;
            if (right < 0.0f)
            {
                right = -1.0f;
                turnTo = faceRight;
                output = 2;
            }
            else
            {
                right = 1.0f;
                turnTo = -faceRight;
                output = 3;
            }
        }

        targetPos = transform.position + turnTo * 6.0f;
        return output;
    }
    #endregion

    //飞檐走壁
    public override bool startWallWalk(IntPtr lParam)
    {
        //飞行或者跳跃时不触发
        if (getEntityState() == EntityState.Jumping || getEntityState() == EntityState.Floating)
        {
            return false;
        }

        ChangeState((int)EntityState.WallWalking, lParam);
        return true;
    }


    #region 掉地下相关

    private static int ErrorGroundLayerThreshold = 3;
    private static float GroundLayerCheckOffset = 1.5f;
    private int CurrErrorGroundLayerThreshold = 0;

    /// <summary>
    /// 检测地面
    /// </summary>
    private void CheckGroundLayer()
    {
        if (!controller || !isHero || null == moveCtrl) //其他玩家是用同步位置
        {
            return;
        }

        if (moveCtrl.bForceMoving || moveCtrl.bForceFlighting)
        {
            return;
        }

        if (SkinInstance.isNullOrEmpry(SkinControl.CurrentSkinInstance))
        {
            return;
        }

        float layerValue = 0;
        Vector3 pos = creature.position;
        if (!IsOnGroundLayer(creature, out layerValue))
        {
            CurrErrorGroundLayerThreshold++;
            if (CurrErrorGroundLayerThreshold > ErrorGroundLayerThreshold)
            {
                CurrErrorGroundLayerThreshold = 0;
                //Debug.LogError(name + "已坐上开往十八层地狱的列车，但是我帮你阻止了" + ",安全区域:" + layerValue + ",你所在区域:" + pos);


                pos.y = layerValue + GetControllerHeightInWorldSpace(controller);
                creature.SetPosition(pos);

                RefershCharacterController();
            }
        }
        else
        {
            CurrErrorGroundLayerThreshold = 0;
        }
    }

    private bool IsOnGroundLayer(Transform trs, out float layer)
    {
        bool reslut = true;
        Vector3 CenterPos = trs.position;
        Vector3 posForward = CenterPos + trs.forward * GroundLayerCheckOffset;
        Vector3 posBack = CenterPos - trs.forward * GroundLayerCheckOffset;
        Vector3 posRight = CenterPos + trs.right * GroundLayerCheckOffset;
        Vector3 posLeft = CenterPos - trs.right * GroundLayerCheckOffset;

        float CenterLayer = GameLogicAPI.getGroundLayer(CenterPos.x, CenterPos.z, controller.height);
        float ForwardLayer = GameLogicAPI.getGroundLayer(posForward.x, posForward.z, controller.height);
        float BackLayer = GameLogicAPI.getGroundLayer(posBack.x, posBack.z, controller.height);
        float RightLayer = GameLogicAPI.getGroundLayer(posRight.x, posRight.z, controller.height);
        float LeftLayer = GameLogicAPI.getGroundLayer(posLeft.x, posLeft.z, controller.height);

        layer = Mathf.Min(CenterLayer, ForwardLayer, BackLayer, RightLayer, LeftLayer);
        if ((CenterPos.y + GetControllerHeightInWorldSpace(controller)) < layer)
        {
            reslut = false;
        }
        return reslut;
    }

    private static float GetControllerHeightInWorldSpace(CharacterController col)
    {
        if (!col)
        {
            return 0;
        }

        float radiusInWorldSpace = col.radius * Mathf.Max(col.transform.lossyScale.x, col.transform.lossyScale.z);
        float heightInWorldSpace = col.height * col.transform.lossyScale.y;

        return Mathf.Max(radiusInWorldSpace, heightInWorldSpace);
    }


    public void RefershCharacterController()
    {
        if (controller)
        {
            GameObject go = controller.gameObject;
            GameObject.DestroyImmediate(controller);
            controller = go.AddComponent<CharacterController>();
            CopyCharacterController();
        }
    }

    #endregion

}


