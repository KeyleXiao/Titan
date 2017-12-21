using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using GameLogic;
using System.Runtime.InteropServices;
using U3D_Render.CameraControl;

public class CreatureState_Floating : ICreatureState {
    
    // 状态机指针
    CreatureStateMachine m_pFSM;

    //人物的transform
    Transform tr;

    [HideInInspector]
    public bool grounded = false;
    public bool bIsMoving = false;

    private GameObject wingGO = null;
    private Animator wingAnimator = null;
    private float m_maxSpeed = 5.0f;
    private float m_maxHeight= 100.0f;
    private float m_accSpeed = 5.0f;
    private bool m_bWing = false; //是否为翅膀状态，false表示是普通技能导致的飞行，没有翅膀动作和加速功能
    //private float m_basicSpeed = 5.0f;


    public bool accelerate = false;

    public enum FloatingState
    {        
        Down,
        Float,
    }

    //private bool useIdleFeetPlacement = true;

    [HideInInspector]
    public LayerMask groundLayers = -1;
    private int hashid_isMoving;
    private int hashid_moveright;
    private int hashid_moveforward;
    private int hashid_velocity;
    private ResNode WingAssets = null;
    // 构造
    public CreatureState_Floating(CreatureStateMachine pFsm)
    {
        m_pFSM = pFsm;
        //m_pFSM.controller = pFsm.GetComponent<CharacterController>();
        hashid_isMoving = Animator.StringToHash("isMoving");
        hashid_moveright = Animator.StringToHash("moveright");
        hashid_moveforward = Animator.StringToHash("moveforward");
        hashid_velocity = Animator.StringToHash("velocity");

    }

    // 取得ID
    public int GetID() { return (int)EntityState.Floating; }
    cmd_Entity_Change_Wing_State currWing_State = default(cmd_Entity_Change_Wing_State);

    private void DeleteFloatingAssets()
    {
        if (!ResNode.isNullOrEmpty(WingAssets))
        {
            WingAssets.DestroyInstanceRes(ref wingGO);
            AssetBundleManager.DeleteAssets(ref WingAssets, false);
            wingGO = null;
            wingAnimator = null; 
        }
    }

    public void ApplyAnimation()
    {
        if(!m_pFSM.animator)
        {
            return;
        }

        if (!m_pFSM.SkinConfig)
        {
            return;
        }

        DeleteFloatingAssets();

        if (currWing_State.szPath != "" && currWing_State.szPath != "0" && !wingGO && m_pFSM.SkinConfig && m_pFSM.SkinConfig.chibangguadian)
        {
            m_bWing = true;
            WingAssets = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, currWing_State.szPath);

            if (WingAssets != null)
            {
                wingGO = WingAssets.InstanceMainRes();
                wingGO.transform.SetParent(m_pFSM.SkinConfig.chibangguadian, false);
                wingAnimator = wingGO.GetComponent<Animator>();

                //绑翅膀音效
                BindSound m_bindAnimSound;
                m_bindAnimSound = wingGO.GetComponent<BindSound>();
                if (m_bindAnimSound == null)
                {
                    m_bindAnimSound = wingGO.AddComponent<BindSound>();
                    if (m_bindAnimSound != null)
                    {
                        m_bindAnimSound.bindSoundEvent += SoundManager.CreateSound;
                    }
                }
            }
        }

        if (m_pFSM.animatorCtrl.ana)
        {
            m_pFSM.animatorCtrl.ana.SetBool("isLanding", false);
            m_pFSM.animatorCtrl.ana.SetBool("floating", true);
            m_pFSM.animatorCtrl.ana.SetBool("flyDown", false);
        }

        if (wingAnimator)
        {
            wingAnimator.SetBool("isLanding", false);
            wingAnimator.SetBool("floating", true);
            wingAnimator.SetBool("flyDown", false);
        }


        if (m_pFSM.isHero && SoldierCamera.MainInstance())
        {
            Vector2 cameraAngle = Vector2.zero;
            float cameraDistance = 0.0f;
            if (!m_bWing)
            {
                cameraAngle = new Vector2(currWing_State.fMinCameraAngle, currWing_State.fMaxCameraAngle);
                cameraDistance = currWing_State.fCameraDistance;
                RidingStateParam camParam = new RidingStateParam(cameraDistance, cameraAngle);
                SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.RidingControl, true, camParam);
            }
            else
            {
                SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.FloatingControl, true);
            }
        }
        else
        {
            if (currWing_State.nType == WING_STATE_TYPE.WING_STATE_NORMAL_FLIGHT)
            {
                m_pFSM.animatorCtrl.ana.SetTrigger("skipFlyStart");
            }
            else if (currWing_State.nType == WING_STATE_TYPE.WING_STATE_ACCELERATION)
            {
                m_pFSM.animatorCtrl.ana.SetTrigger("skipFlyStart");
                EnableAccelerate(true, currWing_State.fMaxSpeed);
            }
        }
    }

    public void ClearState()
    {
        if (wingAnimator)
        {
            if(wingAnimator.isActiveAndEnabled)
            {
                wingAnimator.SetBool("floating", false);
            }
            
            wingAnimator = null;
        }
        DeleteFloatingAssets();
    }

    // 进入状态
    public bool OnEnter(IntPtr lParam)
    {
        cmd_Entity_Change_Wing_State param = IntPtrHelper.toData<cmd_Entity_Change_Wing_State>(lParam);
        currWing_State = param;
        m_bWing=false;


        tr = m_pFSM.transform;
        lastFrameTick = GameLogicAPI.getTickCount();

        ApplyAnimation();

        floatingState = FloatingState.Float;
        fMinFloatHeight = param.fMinFlyHeight;
        if (fMinFloatHeight < 0.01f) //2016-09-01暂时方案，param可能还没有参数传过来，所以变成0，此时先取预制体的浮空最低高度
        {
            Debug.Log(fMinFloatHeight);
            fMinFloatHeight=m_pFSM.creaturePropety.floatingMinHeight;
        }
        fUpSpeed = param.fUpSpeed; //m_pFSM.creaturePropety.floatUpSpeed;
        m_pFSM.groundNormal = Vector3.zero;
        m_ySpeed = 0.0f;
        m_lastvelocity = Vector3.zero;
        m_maxSpeed = param.fSpeed;
        m_accSpeed = param.fAcceleration;
        m_maxHeight = param.fMaxFlyHeight;
        m_lastFrameDeltaTime=0.0f;
        startHeight = m_pFSM.transform.position.y;
        return true;
    }


    // 反复进入状态，例如跳跃时再次起跳
    public bool ReEnter(IntPtr lParam)
    {
        return false;
    }


    // 离开状态
    public void OnLeave()
    {
        if (!m_pFSM || !SoldierCamera.MainInstance<SoldierCamera>())
        {
            return;
        }
        if (null == EntityFactory.MainHeroView || !EntityFactory.MainHeroView.StateMachine)
        {
            return;
        }
        if (m_pFSM.animator)
        {
            m_pFSM.animator.SetBool("floating", false);
        }
        if (wingAnimator)
        {
            wingAnimator.SetBool("floating", false);
        }
        DeleteFloatingAssets();
        EnableAccelerate(false, 0.0f);

        if (m_pFSM.isHero)
        {
            if (!m_bWing)
            {
                SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.RidingControl, false);
            }
            else
            {
                SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.FloatingControl, false);
            }
        }
        else
        {
            //主角死亡的时候，切换的队友结束飞行需要切回上帝视角
            if(EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.StateMachine != null)
            {
                if (EntityFactory.MainHeroView.StateMachine.GetState() == EntityState.Dead)
                    SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.MapGodControl, true);
            }
        }
    }

    //返回是否在地面
    public bool IsGrounded()
    {
        return false;
    }

    public bool isFloating()
    {
        return (m_bWing && !(floatingState == FloatingState.Down));
    }

    public void ChangeState(IntPtr lParam)
    {
        cmd_Entity_Change_Wing_State param = IntPtrHelper.toData<cmd_Entity_Change_Wing_State>(lParam);
        currWing_State = param;
        if (currWing_State.nType == WING_STATE_TYPE.WING_STATE_FLOOR
            || (currWing_State.nType == WING_STATE_TYPE.WING_STATE_INVALID && floatingState != FloatingState.Down))
        {
            // 服务器可能没通知floor而在进视野时直接通知invalid
            floatingState = FloatingState.Down;
            if (accelerate == true)
            {
                //申请退出加速
                applyAccelerate(false);
            }

            if (m_bWing && m_pFSM.animator)
            {
                m_pFSM.animator.SetBool("flyDown", true);
            }

            if (m_bWing && wingAnimator)
            {
                wingAnimator.SetBool("flyDown", true);

                startHeight = m_pFSM.transform.position.y;
            }
        }
        else if (currWing_State.nType == WING_STATE_TYPE.WING_STATE_ACCELERATION)
        {
            EnableAccelerate(true, currWing_State.fMaxSpeed);
        }
        else if (currWing_State.nType == WING_STATE_TYPE.WING_STATE_NORMAL_FLIGHT)
        {
            EnableAccelerate(false, currWing_State.fSpeed);
        }
        else if (currWing_State.nType == WING_STATE_TYPE.WING_STATE_START_FLY)
        {
            if (floatingState != FloatingState.Float)
            {
                floatingState = FloatingState.Float;
            }
        }
    }

    public void EnableAccelerate(bool acc,float speed)
    {
        if (!m_bWing) //不是翅膀不加速
        {
            return;
        }
        if (accelerate != acc)
        {
            if (m_pFSM.animator)
            {
                m_pFSM.animator.SetBool("AccelerateFloat", acc);
            }
            if (wingAnimator)
            {
                wingAnimator.SetBool("AccelerateFloat", acc);
            }

            if (m_pFSM.isHero)
            {
                if (acc)
                {
                    ImageEffects.ImageEffectManager.API_EnableImageEffect<ImageEffects.RadialBlur>();
                    LightingEffectManager lightEffect = m_pFSM.GetComponent<LightingEffectManager>();
                    lightEffect.BindLighting(99998); //加速飞行默认99998号光效
                    lightEffect.BindLighting(281); //加速飞行默认281号光效
                    lightEffect.BindLighting(282); //加速飞行默认282号光效
                }
                else
                {
                    ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.RadialBlur>();
                    LightingEffectManager lightEffect = m_pFSM.GetComponent<LightingEffectManager>();
                    lightEffect.RemoveLighting(99998);
                    lightEffect.RemoveLighting(281);
                    lightEffect.RemoveLighting(282);
                }
            }
        }        

        m_maxSpeed = speed;
        accelerate = acc;
    }

    // 更新状态逻辑
    public void Update()
    {

        if(m_bWing)
        {
            if(m_pFSM.isHero)
            {
                if(!wingGO)
                {
                    if(WingAssets == null)
                    {
                        Trace.LogWarning("翅膀消失调试信息:WingAssets is null");
                    }
                    else
                    {
                        if (!WingAssets.Cleared)
                        {
                            Trace.LogWarning("翅膀消失调试信息:WingAssets :" + WingAssets.AllAssets.Count);
                        }
                        else
                        {
                            Trace.LogWarning("翅膀消失调试信息:WingAssets cleared");
                        }
                        
                    }
                }
            }
        }
        if (m_bWing && m_pFSM.isHero)
        {
            bool isAcc = accelerate;
            if (!accelerate && (InputManager.GetKey(KeyCode.LeftShift) || InputManager.GetKey(KeyCode.RightShift)))
            {
                isAcc = true;
            }
            if (!bIsMoving)
            {
                isAcc = false;
            }
            if (accelerate != isAcc)
            {
                //申请进入/退出加速
                applyAccelerate(isAcc);
            }
        }
        UpdateFunction();
    }

    private void applyAccelerate(bool acc)
    {
        if (!m_pFSM.isHero)
            return;

        cmd_creature_set_wing_state data;
        data.nEncounterFlag = 0;
        if (acc)
        {
            data.nType = WING_STATE_TYPE.WING_STATE_ACCELERATION;
        }
        else
        {
            data.nType = WING_STATE_TYPE.WING_STATE_NORMAL_FLIGHT;
        }


        IntPtrHelper helper = new IntPtrHelper();
        int len = Marshal.SizeOf(data);
        IntPtr ptr = helper.toPtr<cmd_creature_set_wing_state>(ref data);
        int eventID = (int)EntityLogicDef.ENTITY_CMD_SET_WING_STATE;
        GameLogicAPI.onEntityEvent(m_pFSM.entityID, eventID, len, "", ptr, len);
    }

    /// <summary>
    /// 传送，清楚掉移动消息
    /// </summary>
    public void OnTransport()
    {

    }

    public void OnForceMove()
    {
    }

    public void OnForceFlight()
    {
    }

  
    float approximatethre = 5.0f;		// 采用逼近公式的阈值，保证强制收敛到同步目的地 
	float apmtspeedratio = 5.0f;		// 逼近时的速度系数

    float lastupdatetime = 0.0f;
    Vector3 m_lastvelocity = new Vector3(0.0f, 0.0f, 0.0f);
    Vector3 lastoffset = new Vector3(0.0f, 0.0f, 0.0f);
    Vector3 currentvelocity = new Vector3(0.0f, 0.0f, 0.0f);

    MoveSyncFlag sendSync = MoveSyncFlag.NOT_2_SERVER;
    public bool physenable=false;

    int heroMoveTimes = 0;
    private uint lastFrameTick=0;
    private float deltaTime = 0.0f;
    private float m_lastFrameDeltaTime = 0.0f;
    private float startHeight;

    private void UpdateFunction()
    {
        if (floatingState == FloatingState.Down && grounded)
        {
            m_pFSM.ChangeState((int)EntityState.Standing, IntPtr.Zero);

            //float landingSpeed = m_lastvelocity.y;
            //float maxFallSpeed = m_pFSM.SkinConfig == null ? -30 : -m_pFSM.SkinConfig.maxFallSpeed * 2.0f;

            float fallHeight = startHeight - m_pFSM.transform.position.y;
            //if (landingSpeed > maxFallSpeed / 2.0f) //着陆大于最大速度的一半，可能需要计算伤害，上报逻辑层
            if (m_pFSM.isHero && fallHeight > 5.0f) //着陆时落差大于5米，则上报逻辑层            
            {
                cmd_creature_act_landspeed data = new cmd_creature_act_landspeed();

                data.fValue = fallHeight;
                IntPtrHelper helper = new IntPtrHelper();
                int len = Marshal.SizeOf(data);
                IntPtr ptr = helper.toPtr<cmd_creature_act_landspeed>(ref data);
                int eventID = (int)EntityLogicDef.ENTITY_CMD_LANDING_SPEED;
                GameLogicAPI.onEntityEvent(m_pFSM.entityID, eventID, len, "", ptr, len);

                if (m_pFSM.showLog)
                {
                    Trace.Log("Big Landing! fallHeight=" + fallHeight.ToString());
                }
            }
        }

        uint currentTick = GameLogicAPI.getTickCount();
        int deltaTick = (int)(currentTick - lastFrameTick);
        deltaTick = deltaTick < 1 ? 1 : deltaTick;
        lastFrameTick = currentTick;
        deltaTime = ((float)deltaTick) / 1000.0f;
        if (m_lastFrameDeltaTime <= 0.01f)
        {
            m_lastFrameDeltaTime = deltaTime;
        }

        if (m_pFSM.moveCtrl.isCharge())
        {
            //Trace.LogWarning("can't move: m_pFSM.bForceMoving=" + m_pFSM.bForceMoving.ToString() + ",m_pFSM.bForceFlighting=" + m_pFSM.bForceFlighting.ToString());
            return;
        }

        sendSync = MoveSyncFlag.NOT_2_SERVER;

        //--------------------------------------------------------
        // 下面算速度
        //--------------------------------------------------------
        //float finalInertiaAcceleration = m_accSpeed; //惯性加速度
        currentvelocity = new Vector3(0.0f, 0.0f, 0.0f);
        bool forceUp = false;
        if (m_pFSM.isHero && !m_pFSM.bControlledBySever) //主角的话下面多折腾下速度
        {
            // Calculate the velocity based on the current and previous position.  
            // This means our velocity will only be the amount the character actually moved as a result of collisions.
            //Vector3 oldHVelocity = new Vector3(lastvelocity.x, 0, lastvelocity.z);
            //Vector3 newHVelocity = lastoffset / deltaTime;
            //newHVelocity = new Vector3(newHVelocity.x, 0, newHVelocity.z);

            //// The CharacterController can be moved in unwanted directions when colliding with things.
            //// We want to prevent this from influencing the recorded velocity.
            //if (oldHVelocity == Vector3.zero)
            //{
            //    newHVelocity = new Vector3(0, newHVelocity.y, 0);
            //}
            //else
            //{
            //    float projectedNewVelocity = Vector3.Dot(newHVelocity, oldHVelocity) / oldHVelocity.sqrMagnitude;
            //    newHVelocity = oldHVelocity * Mathf.Clamp01(projectedNewVelocity) + newHVelocity.y * Vector3.up;
            //}


            //if (newHVelocity.y < currentvelocity.y - 0.001)
            //{
            //    if (newHVelocity.y < 0)
            //    {
            //        // Something is forcing the CharacterController down faster than it should.
            //        // Ignore this
            //        newHVelocity.y = lastvelocity.y;
            //    }
            //}

            // Update velocity based on input
            //newHVelocity = ApplyInputVelocityChange(newHVelocity);
            Vector3 newHVelocity = ApplyInputVelocityChange(m_lastvelocity);

            //if (newHVelocity.sqrMagnitude < 0.001f) //如果是停止的，就不计算惯性速度，免得技能僵直之类的效果生效不够及时
            //{
            //    currentvelocity = newHVelocity;
            //}
            //else
            //{
                currentvelocity = newHVelocity;
            //}

            //这里记录的是没经过ApplyYSpeed的Y轴调整的速度
            m_lastvelocity = currentvelocity;



            // 判断本地的模拟坐标和真实坐标的差别, 只有达到一定阀值才需要同步
            // 暂时在行走过程中200ms同步一次
            int curTick = System.Environment.TickCount;


            if (currentvelocity.sqrMagnitude <= 0.001f)
            {
                currentvelocity = Vector3.zero;
            }

            // 启动和站立强制同步一次
            if ((m_pFSM.syncVelocity.sqrMagnitude > 0.001f && currentvelocity.sqrMagnitude <= 0.001f) ||
                (m_pFSM.syncVelocity.sqrMagnitude <= 0.001f && currentvelocity.sqrMagnitude > 0.001f))
            {
                sendSync = MoveSyncFlag.TO_BROADCAST;
            }
            // 每200毫秒根据速度或者移动距离考虑是否同步一次
            else if (curTick >= m_pFSM.syncTick + m_pFSM.syncInterval)
            {
                //速度/位置/旋转分别跟上一次同步的数据比对看是否需要同步
                Vector3 velSub = currentvelocity - m_pFSM.syncVelocity;
                Vector3 posSub = tr.position - m_pFSM.syncPosition;
                Vector3 rotSub = tr.eulerAngles - m_pFSM.syncAngle;
                if (velSub.sqrMagnitude > 0.01f || posSub.sqrMagnitude > 0.01f || rotSub.sqrMagnitude > 0.01f)
                    sendSync = MoveSyncFlag.TO_SERVER;
            }

            if (floatingState == FloatingState.Down && curTick >= m_pFSM.syncTick + m_pFSM.syncInterval / 4)
            {
                sendSync = MoveSyncFlag.TO_SERVER; 
            }

            currentvelocity = ApplyYSpeed(currentvelocity, ref forceUp);
        }
        else//不是主角直接用同步的速度
        {
            currentvelocity = m_pFSM.syncVelocity;
            Vector3 deltapos = m_pFSM.syncPosition - tr.position;
            float dis = deltapos.magnitude;
            if (m_pFSM.syncVelocity.sqrMagnitude < 0.001f && dis > 0.01f)//速度为0，但还没到同步的目的地，补偿一下误差
            {
                float alpha = Mathf.Clamp01((dis - 0.01f) / approximatethre); //对速度根据离同步位置误差距离作逼近收敛修正
                float speed = (1.0f - alpha) * 0.0f + alpha * approximatethre * apmtspeedratio;
                currentvelocity = deltapos.normalized * speed;
            }
            else
            {
                //Vector3 newHVelocity = m_pFSM.syncVelocity;
                //Vector3 oldHVelocity = new Vector3(lastvelocity.x, 0, lastvelocity.z);
                //Vector3 HVelocityDelta = newHVelocity - oldHVelocity;
                //float advance = deltaTime * finalInertiaAcceleration;
                //if (advance > HVelocityDelta.magnitude) advance = HVelocityDelta.magnitude;
                //currentvelocity = oldHVelocity + HVelocityDelta.normalized * advance;
                currentvelocity = m_pFSM.syncVelocity;
            }
            currentvelocity = ApplyYSpeed(currentvelocity, ref forceUp);
        }


        //--------------------------------------------------------
        // 下面算位置
        //--------------------------------------------------------
        // We always want the movement to be framerate independent.  Multiplying by Time.deltaTime does this.

        if (deltaTime > m_lastFrameDeltaTime * 2.0f || deltaTime < m_lastFrameDeltaTime / 2.0f) //缓冲帧率突变产生的跳跃
        {
            deltaTime = deltaTime * m_pFSM.frameAccFactor + m_lastFrameDeltaTime * (1 - m_pFSM.frameAccFactor);
        }
        m_lastFrameDeltaTime = deltaTime;


        Vector3 currentOffset = currentvelocity * deltaTime;
        m_pFSM.syncDetalTime += deltaTime;

        // Find out how much we need to push towards the ground to avoid loosing grouning
        // when walking down a step or over a sharp change in slope.
        if (m_pFSM.isHero && !m_pFSM.bControlledBySever)//主角
        {
            if (m_pFSM.ShiftEnable)
            {
                //if (InputManager.GetKey(KeyCode.LeftShift) || InputManager.GetKey(KeyCode.RightShift))//主角暂时添加加速快捷键以便测试
                {
                    currentOffset *= 5.0f;
                }
            }
        }
        else //非主角
        {
            //Vector3 idealPos = tr.position;
            //calSyncOffset(currentvelocity, ref currentOffset, ref idealPos);
            if (/*floatingState != FloatingState.Down &&*/ !forceUp) //下落的时候采用同步位置会上下拉扯，没到规定高度的时候不等同步包强制上升
            {
                if (floatingState == FloatingState.Down)
                {
                    // 下落过程中不用同步位置会在这段不同步 所以还是要使用水平方向的同步位置
                    // 下落时垂直速度要向下 不能因为同步位置还没过来掉一段再往上个同步位置拉
                    Vector3 dirHrz = m_pFSM.syncPosition - tr.position;
                    dirHrz.y = 0;
                    Vector3 dirNormal = dirHrz.normalized;
                    dirNormal.y = -1;
                    dirNormal = dirNormal.normalized;

                    currentvelocity = dirNormal * currentvelocity.magnitude;   
                }
                else
                {
                    Vector3 dir = (m_pFSM.syncPosition - tr.position).normalized;
                    currentvelocity = dir * currentvelocity.magnitude;
                }
            }
            currentOffset = currentvelocity * deltaTime;
            if (currentOffset.sqrMagnitude < 0.001f)
            {
                currentvelocity = Vector3.zero;
            }
        }

        //--------------------------------------------------------
        // 下面更新animator
        //--------------------------------------------------------
        if (m_pFSM.animator != null)
        {
            if (!m_pFSM.isHero || m_pFSM.bControlledBySever)
            {
                //m_lastvelocity是特殊处理飞行时候的移动速度，是它播放合适的动作
                currentvelocity = m_lastvelocity;
            }
            Vector3 planeVel = new Vector3(currentvelocity.x, 0, currentvelocity.z);
            if (planeVel.sqrMagnitude > 0.001f)
            {
                m_pFSM.animator.SetBool("isMoving", true);
                //m_pFSM.animatorCtrl.ana.SetBool("isMoving", true);
                bIsMoving = true;
            }
            else
            {
                m_pFSM.animator.SetBool("isMoving", false);
                //m_pFSM.animatorCtrl.ana.SetBool("isMoving", false);
                bIsMoving = false;
            }

            //取移动速度相对于主角面向的向前和向左的标量
            Vector3 projectVel = Vector3.zero;
            projectVel.x = Vector3.Dot(m_lastvelocity, tr.right); //m_lastvelocity是排除Y方向操作速度的原始速度，可以用来计算方向动作
            projectVel.z = Vector3.Dot(m_lastvelocity, tr.forward);
            float fRight = 0.0f;
            float fForward = 0.0f;
            if (projectVel.sqrMagnitude > 0.0036f)//同步过来的速度为0之后，点乘还会有误差，造成轻微移动动作
            {
                fForward = projectVel.z / m_maxSpeed;
                fRight = projectVel.x / m_maxSpeed;
            }

            if (Mathf.Abs(fRight) < 0.06f)
            {
                fRight = 0.0f;
            }
            if (Mathf.Abs(fForward) < 0.06f)
            {
                fForward = 0.0f;
            }

            fForward = Mathf.Min(fForward, 1.0f);
            fForward = Mathf.Max(fForward, -1.0f);
            fRight = Mathf.Min(fRight, 1.0f);
            fRight = Mathf.Max(fRight, -1.0f);

            //移动速度考虑了服务器速度和基本速度的比率后，按服务器速度算animator参数值

            //fForward = fForward / m_maxSpeed;
            //fRight = fRight / m_maxSpeed;

            m_pFSM.animator.SetFloat("moveright", fRight);
            m_pFSM.animator.SetFloat("moveforward", fForward);
            m_pFSM.animator.SetFloat("velocity", currentvelocity.magnitude);


            if (wingAnimator)
            {
                wingAnimator.SetFloat("moveright", fRight);
                wingAnimator.SetFloat("moveforward", fForward);
            }

            if (m_pFSM.animatorCtrl.anb != null)
            {
                m_pFSM.animatorCtrl.anb.SetFloat("moveright", fRight);
                m_pFSM.animatorCtrl.anb.SetFloat("moveforward", fForward);
            }

        }


        //--------------------------------------------------------
        // 下面更新transform
        //--------------------------------------------------------
        // Save lastPosition for velocity calculation.
        Vector3 oldPosition = tr.position;
        float curtime = Time.time;

        if (m_pFSM.isHero && !m_pFSM.bControlledBySever)//是主角，都用耗时的物理行走方法
        {
            m_pFSM.collisionFlags = m_pFSM.controller.Move(currentOffset);
            ++heroMoveTimes;
        }
        else//非主角
        {

            float pos_dis = (m_pFSM.syncPosition - tr.transform.position).magnitude;
            if (m_pFSM.syncVelocity.sqrMagnitude < 0.01f && m_pFSM.animatorCtrl.bDoingAttackMove)
            {
                pos_dis = 0.0f; //正在利用动作控制位移，就不按同步包移动了
            }
            if ((floatingState==FloatingState.Down) ||  (physenable && pos_dis > approximatethre) || (forceUp)) //上下落或者跟同步位置差距较大时用耗时的物理行走方法
            {
                m_pFSM.collisionFlags = m_pFSM.controller.Move(currentOffset);
            }
            else if (pos_dis > 0.01f) //否则用较小耗时的渐趋逼近公式
            {
                Vector3 deltapos = m_pFSM.syncPosition - tr.transform.position;
                Vector3 curPos;
                if (deltapos.sqrMagnitude > currentOffset.sqrMagnitude)
                {
                    curPos = tr.transform.position + currentOffset;
                }
                else
                {
                    curPos = tr.transform.position * 0.5f + m_pFSM.syncPosition * 0.5f;
                }
                tr.transform.SetPosition(curPos);
            }

            
            //特殊处理飞行时候的移动速度，是它播放合适的动作
            Vector3 deltaDis = m_pFSM.syncPosition - tr.transform.position;
            Vector3 newVectory = Vector3.zero;
            if (deltaDis.sqrMagnitude > 0.01f)
            {
                newVectory = currentOffset;
                newVectory.y = 0;
                newVectory = newVectory.normalized * m_maxSpeed;
            }
            m_lastvelocity = m_lastvelocity * 0.3f + newVectory*0.7f;

 
        }

        if (m_pFSM.isHero && !m_pFSM.bControlledBySever)//主角不用平滑角度
        {

        }
        else//非主角才平滑角度角度
        {
            float angle_dis = (m_pFSM.syncAngle - tr.transform.eulerAngles).magnitude;
            if (angle_dis > 0.1f) //角度差距较大时采用逼近公式
            {
                Vector3 newangle;
                newangle.x = Mathf.LerpAngle(tr.eulerAngles.x, m_pFSM.syncAngle.x, 0.2f);
                newangle.y = Mathf.LerpAngle(tr.eulerAngles.y, m_pFSM.syncAngle.y, 0.2f);
                newangle.z = Mathf.LerpAngle(tr.eulerAngles.z, m_pFSM.syncAngle.z, 0.2f);
                tr.eulerAngles = newangle;
            }
        }

        lastupdatetime = curtime;
        lastoffset = currentOffset;

        //最后算完才上报
        if (sendSync != MoveSyncFlag.NOT_2_SERVER && (m_pFSM.isHero && !m_pFSM.bControlledBySever))
        {
            int curTick = System.Environment.TickCount;
            m_pFSM.syncTick = curTick;            
            Vector3 sendVelocity = currentvelocity;
            m_pFSM.syncVelocity = sendVelocity;
            m_pFSM.syncPosition = tr.position;
            m_pFSM.syncAngle = tr.eulerAngles;
            Send_Sync_Position(sendSync);

            
        }
        else if (heroMoveTimes >= 8)
        {
            Vector3 sendVelocity = new Vector3(currentvelocity.x, 0.0f, currentvelocity.z);
            updatePosWithoutSend(sendVelocity,tr.position,tr.eulerAngles);
            heroMoveTimes = 0;
        }

        //移动完更新地面判断
        grounded = m_pFSM.groundNormal.y > 0.01f;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 上报逻辑层位置，但是不上报给服务器
    private void updatePosWithoutSend(Vector3 velocity,Vector3 position,Vector3 angles)
    {
        //如果上报位置范围不对，则输出至LOG，退出
        if (float.IsNaN(position.x) || float.IsNaN(position.y) || float.IsNaN(position.z) || Math.Abs(position.x) > SceneManager.MAX_SCENE_SIZE || Math.Abs(position.y) > SceneManager.MAX_SCENE_SIZE || Math.Abs(position.z) > SceneManager.MAX_SCENE_SIZE)
        {
            Debug.LogError(m_pFSM.name + " Floating updatePosWithoutSend Position Error " + position);
            return;
        }
        

        cmd_creature_rigidbody_sync data = new cmd_creature_rigidbody_sync();
        data.nCreatureState = (int)EntityState.Floating;
        data.fPosition_x = position.x;
        data.fPosition_y = position.y;
        data.fPosition_z = position.z;

        // 直接用欧拉角
        data.fRotation_x = angles.x;
        data.fRotation_y = angles.y;
        data.fRotation_z = angles.z;

        data.fVelocity_x = velocity.x;
        data.fVelocity_y = velocity.y;
        data.fVelocity_z = velocity.z;
        data.nSyn2Server = (int)MoveSyncFlag.NOT_2_SERVER;

        EntityEventHelper.Instance.SendCommand<cmd_creature_rigidbody_sync>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_SET_POS, ref data);

    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 发送一次位置同步
    public void Send_Sync_Position(MoveSyncFlag reportToSever)
    {
        if (m_pFSM == null)
        {
            Trace.LogError("CreatureState_Standing.cs Send_Sync_Position m_pFSM == null");
            return;
        }

        //如果上报位置范围不对，则输出至LOG，退出
        if (float.IsNaN(m_pFSM.syncPosition.x) || float.IsNaN(m_pFSM.syncPosition.y) || float.IsNaN(m_pFSM.syncPosition.z) || Math.Abs(m_pFSM.syncPosition.x) > SceneManager.MAX_SCENE_SIZE || Math.Abs(m_pFSM.syncPosition.y) > SceneManager.MAX_SCENE_SIZE || Math.Abs(m_pFSM.syncPosition.z) > SceneManager.MAX_SCENE_SIZE)
        {
            Debug.LogError(m_pFSM.name + " Floating Send_Sync_Position Position Error " + m_pFSM.syncPosition);
            return;
        }

        cmd_creature_rigidbody_sync data = new cmd_creature_rigidbody_sync();
        data.nCreatureState = (int)EntityState.Floating;
        data.fPosition_x = m_pFSM.syncPosition.x;
        data.fPosition_y = m_pFSM.syncPosition.y;
        data.fPosition_z = m_pFSM.syncPosition.z;

        // 直接用欧拉角
        data.fRotation_x = m_pFSM.syncAngle.x;
        data.fRotation_y = m_pFSM.syncAngle.y;
        data.fRotation_z = m_pFSM.syncAngle.z;

        data.fVelocity_x = m_pFSM.syncVelocity.x;
        data.fVelocity_y = m_pFSM.syncVelocity.y;
        data.fVelocity_z = m_pFSM.syncVelocity.z;
        data.nSyn2Server = (int)reportToSever;

        EntityEventHelper.Instance.SendCommand<cmd_creature_rigidbody_sync>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_SET_POS, ref data);


        // 通知主角位置发生变化
        U3D_Render.EntityLocUpdate.SendActorLocationChange(new Vector3(m_pFSM.syncPosition.x, m_pFSM.syncPosition.y, m_pFSM.syncPosition.z));

        if (tr == null)
        {
            Trace.LogError("CreatureState_Floating.cs Send_Sync_Position tr == null");
            return;
        }

        if (tr.position == null)
        {
            Trace.LogError("CreatureState_Floating.cs Send_Sync_Position tr.SetPosition(= null");
            return;
        }

        if (tr.rotation == null)
        {
            Trace.LogError("CreatureState_Floating.cs Send_Sync_Position tr.rotation == null");
            return;
        }

        if (tr.rotation.eulerAngles == null)
        {
            Trace.LogError("CreatureState_Floating.cs Send_Sync_Position tr.rotation.eulerAngles == null");
            return;
        }

        EntityBatchCommunicator.SendEntityPro(m_pFSM.entityID, tr.position, tr.rotation.eulerAngles);

        if (m_pFSM.isHero)
        {
            U3D_Render.EntityView ev = EntityFactory.getPlayerViewByID(m_pFSM.entityID);
            MapFinderManager.MoveObject(ref ev, tr.position);
        }   
    }


    private Vector3 syncDir;
    //非主角玩家，计算同步的offset，考虑延时和平滑
    private void calSyncOffset(Vector3 velocity, ref Vector3 movementOffset, ref Vector3 idealPos)
    {

        idealPos = m_pFSM.syncPosition + velocity * m_pFSM.syncDetalTime; //根据同步包算出来的理想位置

        Vector3 dis = idealPos - m_pFSM.syncPosition;
        dis.y = 0.0f;
        //张嘉华要求，走了100米还没有新的更新包，就拉回上个同步位置
        if (dis.sqrMagnitude > 10000.0f)
        {
            m_pFSM.syncVelocity = Vector3.zero;
            tr.SetPosition(m_pFSM.syncPosition);
            idealPos = m_pFSM.syncPosition;
        }


        ////向理想位置按混合比例偏移，此方案最后同步的位置比较精准
        //Vector3 currentPos=lastPos+velocity*Time.deltaTime;
        //Vector3 blendPos=idealPos*syncOffsetWeight+currentPos*(1-syncOffsetWeight);
        //Vector3 blendOffset=blendPos-lastPos;

        //向理想位置按同步的速度前进，此方案看起来拉扯感比较小
        Vector3 moveVel = idealPos - tr.position;
        moveVel = moveVel.normalized * velocity.magnitude;
        Vector3 blendOffset = moveVel * deltaTime;

        Vector3 blendDir = blendOffset.normalized;

        if (Vector3.Dot(blendDir, syncDir) < -0.1f) //平滑后前进方向与同步包的方向夹角大于95度（arccos(-0.1)），会发生后退拉扯
        {
            if (blendOffset.magnitude < m_pFSM.syncDisThread)
            {
                ////后退拉扯距离小于规定阈值，则前进速度减半，不后退了
                //currentPos = lastPos + velocity / 2.0f * Time.deltaTime;
                //blendPos = currentPos;
                //blendOffset = blendPos - lastPos;
                //blendDir = blendOffset.normalized;


                //后退拉扯距离小于规定阈值，直接停止
                blendOffset = Vector3.zero;
            }
        }

        Vector2 planeVel = new Vector2(velocity.x, velocity.z);
        if (planeVel.sqrMagnitude < 0.0025f && blendOffset.magnitude < m_pFSM.syncDisThread) //不移动的时候就不移动了
        {
            blendOffset = Vector3.zero;
            blendOffset.y = m_pFSM.syncPosition.y - tr.position.y;
        }

        movementOffset.x = blendOffset.x;
        movementOffset.y = blendOffset.y;
        movementOffset.z = blendOffset.z;

    }

    private Vector3 ApplyInputVelocityChange(Vector3 velocity)
    {
        if (!m_pFSM.canControl)
            m_pFSM.inputMoveDirection = Vector3.zero;

        //Debug.Log("1:" + velocity.ToString());

        // Find desired velocity

        Vector3 inputMoveDirection = m_pFSM.inputMoveDirection;
        if (m_pFSM.moveCtrl.isAutoMoving)
        {
            inputMoveDirection = m_pFSM.autoMoveDirection;
        }

        Vector3 desiredVelocity;

        desiredVelocity = GetDesiredHorizontalVelocity(inputMoveDirection);

        // Enforce max velocity change
        float maxVelocityChange = deltaTime * m_accSpeed; //10.0f;//m_pFSM.maxAcceleration * Time.deltaTime;
        Vector3 velocityChangeVector = (desiredVelocity - velocity);
        if (velocityChangeVector.sqrMagnitude > maxVelocityChange * maxVelocityChange)
        {
            velocityChangeVector = velocityChangeVector.normalized * maxVelocityChange;
        }
        // If we're in the air and don't have control, don't apply any velocity change at all.
        // If we're on the ground and don't have control we do apply it - it will correspond to friction.
        if (m_pFSM.canControl)
            velocity += velocityChangeVector;
        
        return velocity;
    }


    //private float m_ySpeed = 0.0f;
    private FloatingState floatingState;
    public float fMinFloatHeight = 1.0f;
    public float fUpSpeed = 1.0f;
    private float floatDis()
    {
        int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault)) | (1 << LayerMask.NameToLayer(Config.LayerBuilding));
        //int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault));

        Ray groundRay = new Ray(m_pFSM.transform.position+new Vector3(0,0.5f,0), new Vector3(0, -1.0f, 0)); //抬高0.5米检测
        RaycastHit hitGroundInfo;
       bool hitGround = Physics.Raycast(groundRay, out hitGroundInfo, m_maxHeight+100.0f, LayerMaskOnGround);

        if (hitGround)
        {
            float height=(hitGroundInfo.point - m_pFSM.transform.position).magnitude-0.5f;
            if (height>0.6f)
            {
                grounded=false;
                m_pFSM.groundNormal = Vector3.zero;
            }
            return height;
        }

        //向下探测不到，可能是镂空了，这里也设为浮空
        grounded = false;
        m_pFSM.groundNormal = Vector3.zero;
        return 10.0f;
    }


    private float m_ySpeed = 0.0f; //记录y轴速度，自由落体用    
    private Vector3 ApplyYSpeed(Vector3 velocity,ref bool forceUp)
    {
        float mag = velocity.magnitude;
        float height = floatDis(); //floatDis()每帧都算，放第一位
        forceUp = false;
        if (floatingState == FloatingState.Float)
        {
            if (height < fMinFloatHeight || grounded) 
            {
                Vector3 hoSpeed = velocity;
                hoSpeed.y = 0.0f;
                float dis = fMinFloatHeight - height;
                float hoDis=hoSpeed.magnitude*0.5f;
                if (dis < hoDis) //小于0.5s能爬上来的坡，就不用上升速度了，直接移动位置，免得有顿挫感
                {
                    m_pFSM.controller.Move(new Vector3(0,dis,0));
                }
                else
                {
                    velocity.y = fUpSpeed;
                    forceUp = true;
                }
            }
            else if (height+velocity.y*deltaTime<fMinFloatHeight)
            {
                velocity.y = 0.0f;
            }
            if ((m_bWing) && (m_pFSM.isHero && !m_pFSM.bControlledBySever) && (InputManager.GetKey(KeyCode.Space))) //空格键上升
            {
                velocity.y += fUpSpeed;
            }
        }
        else
        {
            if (!grounded)
            {
                velocity.y = m_ySpeed - m_pFSM.currentGravity*2.0f * deltaTime; //根据策划需求，飞行掉落速度是普通自由落体的2倍
                velocity.y = Mathf.Max(velocity.y, m_pFSM.SkinConfig == null ? -30 : -m_pFSM.SkinConfig.maxFallSpeed*2.0f);
            }
        }
         
        if (height >= m_maxHeight && velocity.y > 0.0f) //2017-02-06，根据策划需求，最大高度限制取离地高度
        {
            velocity.y = 0.0f;
        }

        m_ySpeed = velocity.y;
        return velocity;
    }

    private Vector3 GetDesiredHorizontalVelocity(Vector3 inputMoveDirection)
    {
        // Find desired velocity
        Vector3 desiredLocalDirection = tr.InverseTransformDirection(inputMoveDirection);        
        return tr.TransformDirection(desiredLocalDirection * m_maxSpeed);
    }

    private Vector3 projectVec(Vector3 input)
    {
        Vector3 output = input;
        output.y = 0.0f;
        output = output.normalized * input.magnitude;
        return output;
    }

    public void PositionSync(ref cmd_creature_rigidbody_sync sync)
    {
        if (m_pFSM.isHero && !m_pFSM.bControlledBySever)
        {
           return;
        }

        //Debug.Log("PosotionSync" + m_pFSM.name+ m_pFSM.syncPosition.ToString("f8"));
        // 直接通过欧拉角来设置人物朝向
        //tr.rotation = Quaternion.Euler(sync.fRotation_x, sync.fRotation_y, sync.fRotation_z);
        //Vector3 angle = new Vector3(sync.fRotation_x, sync.fRotation_y, sync.fRotation_z);
        //tr.eulerAngles = angle;

        Vector3 inputV = new Vector3(sync.fVelocity_x, sync.fVelocity_y, sync.fVelocity_z);
        syncDir = inputV.normalized;

        //Trace.Log("PositionSync Pos:"+m_pFSM.syncPosition.ToString()+"Vel:"+inputV.ToString());

    }

    // 获得基本速度
    public Vector3 getVelocity()
    {
        //currentvelocity包含了Y轴方向的调整，SoliderController用来判断方向会有偏差，所以用没经过applyYSpeed的m_lastvelocity
        return m_lastvelocity;
    }
    public void setVelocity(Vector3 vel)
    {
        currentvelocity = vel;
    }

}
