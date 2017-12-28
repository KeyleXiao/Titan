using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameLogic;
using SPELL;
using System;
using System.Runtime.InteropServices;
using USpeedUI;

/// <summary>
/// 屏幕射线检测
/// </summary>
public class ScreenRaycast : MonoBehaviour
{
    /// <summary>
    /// 用于返回检测对象信息
    /// </summary>
    public struct CastInfo
    {
        public int CastObjectID;
        public int CastObjectType;
        public Vector3 CastObjectPoint;
        public RaycastHit CastObject;
        public Transform CastObjectTransform;
        //public bool IsPlayerInRange;
    }

    public enum SelectInputType
    {
        MousePos = 0,
        FaceDirection,
        WalkingDirection,
        MoveDirDefaultRawInputDir,
        MouseGuideAngle,
        CrossHairPos,
        MousePosAndCrossHairPos,
    }

    public enum SelectViewType //选择对象时候的类型
    {
        InAngle = 0,
        InSceen,
        Out,
    }

    /// <summary>
    /// 检测层
    /// </summary>
    private LayerMask collisionLayers = -1;
    private int LayerMaskOnPos; //选择Position的时候用这个Mask，排除Object用于校准的ScreenRaycast盒子

    //场景锁定框对象
    public GameObject TargetFrame = null;
    private Transform rootTrans = null; //记录Root点的位置，测距离用。

    private float m_defaultAimDistance = 10.0f;
    private bool m_isDefaultAim = false;
    public float defaultAimDistance
    {
        set
        {
            m_defaultAimDistance = value;
            if (m_isDefaultAim)
            {
                SetDefaultSelection();
            }
        }
        get
        {
            return m_defaultAimDistance;
        }
    }

    private float lastTime = 0.0f; //记录创建时间。创建1秒内不更新，因为视口创建未完成时，射线提取会报错

    public void Start()
    {
        lastTime = Time.time;

        TargetFrame = GameObject.Find("TargetFrame");

        zeroTarget();

        machine = transform.GetComponentInParent<CreatureStateMachine>();
        if (machine != null)
        {
            CreatureProperty cp = machine.creaturePropety;
            if (cp != null)
            {
                CurCP = cp;
                SetCrossHairPos(cp.CrossHairOffset);

            }
            rootTrans = machine.transform;//.FindTransformEx(transform, "Main");    //技能施法不要与皮肤挂钩
        }
        SetDefaultSelection();

    }

    public void OnDestroy()
    {
    }



    #region 主要参与的参数

    public bool showLog = false;

    public bool bLockTarget = false; //锁定对象
    private SelectInputType m_selectInputType; //输入类型
    private bool m_bRight; //是否取右方向
    private bool m_bSnapToObject; //是否模糊拾取，当鼠标/准心/面向没有精确命中对象的时候，取最近的对象
    private bool m_bSnapToObject_AllowSoldier;  //模糊选取是否可以选择小兵（要先允许，模糊选取）
    private bool m_bProjectToFloor; //是否将拾取点投影到地面
    private bool m_bProjectToFloorSkipBuilding; //将拾取点投影到地面是否忽略建筑物
    private bool m_bSelectMaxDistancePoint;//是否按技能距离取点
    private bool m_bCrossObject; //target点选择为拾取目标对象身后的点
    private float m_selectDistance; //技能/拾取距离    
    private bool m_bPlayerSelectFirst; //是否优先拾取玩家英雄
    private int m_nTargetFilter; //排除目标的类型
    private bool m_bEnableGroundBlock; //是否考虑地形阻挡
    private bool m_bEnableWardBlock; //是否考虑结界阻挡
    private bool m_bDownGradeTower; //将塔的选择优先级降低至小兵
    private bool m_bWalkDirectionDefaultBack; //取移动方向时，移动速度为0，默认向后
    private bool m_bHeroProjectToFloor; //选取技能时英雄坐标贴地
    private bool m_bPickBuilding; //取点时检测建筑层
    private bool m_bIsStrafe; //是否扫射

    float m_fTraceHeight = 1.5f; //沿地面拾取时防碰撞的高度
    private MouseCtrl mouseCtrl = null;

    [System.NonSerialized]
    public int targetID;

    [System.NonSerialized]
    public Vector3 targetPos;
    [System.NonSerialized]
    public GameObject targetObject;
    private BaseStateMachine targetMachine;
    private U3D_Render.EntityView targetEntityView;
    private BaseStateMachine attackHeroMachine; //记录攻击我的英雄
    public float MAX_CAST_DISTANCE = 100.0f; //射线检测最远长度

    #endregion

    #region 拾取函数
    //----------------------------------------------------------------------------------
    //以下是一些实用功能函数
    //----------------------------------------------------------------------------------    
    /// 计算拾取线和距离
    /// </summary>
    /// <param name="nInputType">输入，输入类型0鼠标；1屏幕准心；2面向；3前进方向</param>  
    /// <param name="bRight">输入，是否取右向</param>    
    /// <param name="_Distance">输入+输出，检测距离</param>
    /// <param name="Raycast">输出，拾取线</param>
    /// 

    private Ray calRotateRay(Vector3 lastSelectPos, float detaAngle)
    {
        Vector3 selectPoint = GetCrossHairPos();
        if (mouseCtrl == null)
        {
            mouseCtrl = transform.GetComponent<MouseCtrl>();
        }
        if (mouseCtrl != null)
        {
            selectPoint = mouseCtrl.getMousePosition();
        }
        else
        {
            selectPoint = InputManager.MousePosition;
        }
        Ray castRay = Initialize.mainCam.ScreenPointToRay(selectPoint);
        RaycastHit hitInfo;

        Vector3 heroPos = transform.position;
        if (m_bHeroProjectToFloor)
        {
            heroPos = ProjectToFloor(heroPos);
        }

        Physics.Raycast(castRay, out hitInfo, 100.0f, LayerMaskOnPos);
        Vector3 forward = lastSelectPos - heroPos;
        forward.y = 0.0f;
        forward.Normalize();
        Vector3 right = Vector3.Cross(forward, Vector3.up);
        right.Normalize();
        Vector3 mouseVec = hitInfo.point - heroPos;
        Vector3 mouseDir = mouseVec;
        mouseDir.y = 0.0f;
        mouseDir.Normalize();
        float offsetAngleCos = Vector3.Dot(mouseDir, forward);
        if (offsetAngleCos > Mathf.Cos(detaAngle))
        {
            //Vector3 newDirection = lastSelectPos-heroPos;
            //newDirection.y = 0;
            //newDirection.Normalize();
            castRay.origin = heroPos + new Vector3(0, m_fTraceHeight, 0);
            castRay.direction = mouseDir;
        }
        else
        {
            detaAngle = Mathf.Abs(detaAngle);
            float projectAngle = Vector3.Dot(mouseDir, right);
            if (projectAngle < 0.0f)
            {
                detaAngle = -detaAngle;
            }
            float angle = Mathf.Atan2(forward.z, forward.x);
            angle = angle + detaAngle;
            Vector3 newDirection = new Vector3(Mathf.Cos(angle), 0.0f, Mathf.Sin(angle));
            castRay.origin = heroPos + new Vector3(0, m_fTraceHeight, 0);
            castRay.direction = newDirection;
        }
        return castRay;
    }


    private Ray calCastRay(SelectInputType nInputType, bool bRight, ref float fDistance, Vector3 lastSelectPos, float detaAngle)
    {
        if (nInputType == SelectInputType.MouseGuideAngle)
        {
            return calRotateRay(lastSelectPos, detaAngle);
        }
        if (!Initialize.mainCam)
        {
            return new Ray();
        }
        Vector3 selectPoint = GetCrossHairPos();//new Vector3(Screen.width / 2, Screen.height / 2, 0);

        if (!rootTrans)
        {
            return new Ray();
        }
        Vector3 heroPos = rootTrans.position;
        if (m_bHeroProjectToFloor)
        {
            heroPos = ProjectToFloor(heroPos);
        }

        Ray castRay = new Ray(Camera.main.transform.position, Camera.main.transform.forward);
        //Initialize.mainCam.ScreenPointToRay(selectPoint);

        SelectInputType inputType = nInputType;
        if (!rootTrans) return castRay;

        //WalkingDirToFaceDir，当有移动速度时优先选择移动方向，否则选择面向
        if (inputType == SelectInputType.MoveDirDefaultRawInputDir)
        {
            inputType = SelectInputType.FaceDirection;
            if (machine != null)
            {
                Vector3 vel = machine.rawInputMoveDirection;
                if (vel.sqrMagnitude > 0.001f)
                {
                    inputType = SelectInputType.WalkingDirection;
                }
                else
                {
                    if (m_bWalkDirectionDefaultBack)
                    {
                        fDistance *= -1.0f;
                    }
                }
            }
        }

        if (inputType == SelectInputType.MousePos || inputType == SelectInputType.CrossHairPos || inputType == SelectInputType.MousePosAndCrossHairPos)
        {
            if (inputType == SelectInputType.MousePos || ((!SoldierCamera.MainInstance<SoldierCamera>().bRotateWithMouse) && (inputType == SelectInputType.MousePosAndCrossHairPos)))
            {
                if (mouseCtrl == null)
                {
                    mouseCtrl = transform.GetComponent<MouseCtrl>();
                }
                if (mouseCtrl != null)
                {
                    selectPoint = mouseCtrl.getMousePosition();
                }
                else
                {
                    selectPoint = InputManager.MousePosition;
                }
            }

            castRay = Initialize.mainCam.ScreenPointToRay(selectPoint);
            if (bRight) //如果是前向，则从摄像机位置出发。如果是右向，则射线从人物位置出发，因为从摄像机出发的右向屏幕看不到。
            {
                castRay.direction = Initialize.mainCam.transform.right;
                //更精确的方法，根据地面法向前进
                if (machine != null && machine.IsGrounded() && machine.groundNormal.y > 0.01f)
                {
                    castRay.direction = Vector3.Cross(machine.groundNormal.normalized, Initialize.mainCam.transform.up.normalized);
                }
                castRay.origin = heroPos + new Vector3(0, m_fTraceHeight, 0);
                if (fDistance < 0.0f)
                {
                    fDistance *= -1.0f;
                    castRay.direction *= -1.0f;
                }
            }
            else if (Mathf.Abs(fDistance) <= MAX_CAST_DISTANCE * 1.1f)
            {
                if (fDistance > 0.0f)
                {
                    //从角色出发的射线可以忽略此段
                    //角色到摄像机还有一段距离，而_Distance是指从角色开始算的距离skillRad，所以rayDistance要重新算
                    /*
                          Camera
                             \*
                              \     *   rayDistance
                        deta   \         *
                                \              *
                              Hero------------HitPoint
                                    skillRad
             
                     */

                    Vector3 deta = heroPos - castRay.origin;
                    Vector3 detaDir = deta.normalized;
                    float cosAngle = Vector3.Dot(castRay.direction, detaDir);
                    float sinAngle = Mathf.Sin(Mathf.Acos(cosAngle));
                    float dis2 = sinAngle * deta.magnitude;
                    float newDis = cosAngle * deta.magnitude + Mathf.Sqrt(fDistance * fDistance + dis2 * dis2);
                    fDistance = newDis;
                    //Trace.Log("rad="+_Distance+" deta="+deta.magnitude+ " rayDistance=" + rayDistance);
                }
                else
                {
                    fDistance *= -1.0f;
                    if (machine != null && machine.IsGrounded() && machine.groundNormal.y > 0.01f)
                    {
                        castRay.direction = Vector3.Cross(machine.groundNormal.normalized, Initialize.mainCam.transform.right.normalized);
                    }
                    else
                    {
                        castRay.direction = Initialize.mainCam.transform.forward.normalized;
                        castRay.direction *= -1.0f;
                    }
                    castRay.origin = heroPos + new Vector3(0, m_fTraceHeight, 0);
                }
            }
        }
        else if (inputType == SelectInputType.FaceDirection)
        {
            Vector3 orgPos = heroPos;
            orgPos.y += m_fTraceHeight; //抬高避免撞地面
            Vector3 face = transform.forward;
            if (machine != null && machine.IsGrounded() && machine.groundNormal.y > 0.01f)
            {
                Vector3 groundNormal = machine.groundNormal;

                face = Vector3.Cross(transform.right.normalized, groundNormal.normalized);
                if (bRight)
                {
                    face = Vector3.Cross(groundNormal.normalized, transform.forward.normalized);
                }
            }
            else if (bRight)
            {
                face = transform.right;
            }
            face.Normalize();
            if (fDistance < 0.0f)
            {
                face *= -1.0f;
                fDistance *= -1.0f;
            }
            castRay = new Ray(orgPos, face);
        }
        else if (inputType == SelectInputType.WalkingDirection)
        {
            Vector3 orgPos = heroPos;
            orgPos.y += m_fTraceHeight; //抬高避免撞地面
            Vector3 face = transform.forward;
            if (m_bWalkDirectionDefaultBack)
            {
                face *= -1.0f;
            }
            if (machine != null)
            {
                Vector3 vel = machine.rawInputMoveDirection;
                if (vel.sqrMagnitude > 0.001f)
                {
                    face = vel.normalized;
                }
            }
            if (bRight)
            {
                face = Vector3.Cross(face, transform.up);
            }
            castRay = new Ray(orgPos, face);
        }
        return castRay;
    }

    /// 判断选中对象是否合法
    /// </summary>
    /// <param name="target">输入选中对象</param>  
    /// <param name="selectFilter">输入，选中过滤</param>    
    /// <param name="fDistance">输入，检测距离</param>
    /// <param name="enableBlock">输入，是否考虑遮挡</param>
    /// 输出bool，是否合法
    private bool isTargetValid(GameObject t_targetObject, BaseStateMachine t_targetMachine, float selectDistance, U3D_Render.EntityView entityView)
    {
        if (!t_targetObject || !t_targetMachine || !t_targetMachine.GetVisible()) return false;

        if (!t_targetMachine.GetVisible())
            return false;
        ////如果只有逻辑实体，没有皮肤（网格），那么会没有wound点，在游戏中不应该被选中
        ////此功能暂时注释，用于排查有些怪没正确生成皮肤的BUG，正式发布时请打开
        //if (!t_targetMachine.wound)
        //{
        //    return false;
        //}

        int entityLayer = LayerMaskOnPos;
        //关于ward结界类型的特殊处理
        //结界类型的物理盒子放在LayerWard层，用于阻挡指定类型的玩家
        //但是结界有可以选中和不可以选中，
        //这里只将配置的ScreenCastObject改为其他层，那么不可选中的结界不配置ScreenCastObject，就不影响结界后面实体的技能选中，
        //除了ScreenCastObject之外，实体的其他部分仍然为LayerWard，在raycast射线里排除LayerWard，则只有ScreenCastObject的实体可以被hit到
        //hit到了之后判断实体有效性的时候，取实体对象会发现它在LayerWard层，因此将LayerWard加入有效层的范围内，这样就结界的选中就是合法的了。

        entityLayer |= 1 << LayerMask.NameToLayer(Config.LayerWard_Friend);
        entityLayer |= 1 << LayerMask.NameToLayer(Config.LayerWard_Enemy);

        if (0 == ((1 << t_targetObject.layer) & entityLayer))
        {
            return false;
        }

        if (0 == (m_nTargetFilter & (int)TARGET_FILTER.TARGET_BODY_USE))
        {
            if (t_targetMachine.GetState() == EntityState.Dead)
            {
                return false;
            }
        }

        if (0 != (m_nTargetFilter & (int)TARGET_FILTER.TARGET_NO_ALIVE_USE))
        {
            if (t_targetMachine.GetState() != EntityState.Dead)
            {
                return false;
            }
        }

        //if (entityView != null)
        //{
        //    if ((entityView.Type == ENTITY_TYPE.TYPE_MONSTER) && ((int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_NPC == entityView.Flag))
        //    {
        //        selectDistance = 2.0f; //NPC只在2米内可以选中
        //    }
        //}

        Vector3 heroPos = transform.position;
        if (m_bHeroProjectToFloor)
        {
            heroPos = ProjectToFloor(heroPos);
        }

        Vector3 vecDistance = t_targetObject.transform.position - heroPos;

        if (vecDistance.magnitude > selectDistance)
        {
            return false;
        }

        if (m_bEnableGroundBlock || m_bEnableWardBlock)
        {
            if (isBlocked(t_targetObject.transform.position + new Vector3(0, m_fTraceHeight, 0)))
            {
                return false;
            }
        }

        if (entityView != null)
        {
            if (0 != (entityView.InitMaskFlag & (int)ENTITY_MASKFLAG.MASKFLAG_NOT_SELECT))
            {
                Debug.Log("英雄无法选取了！！！：" + entityView.InitMaskFlag);
                return false;
            }

            if (!isTypeValid(entityView))
            {
                return false;
            }
        }

        return true;
    }


    private bool isTypeValid(U3D_Render.EntityView entityView)
    {
        if (entityView.Type == ENTITY_TYPE.TYPE_MONSTER)
        {
            int type = entityView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX);
            if ((int)MONSTER_TYPE.MONSTER_TYPE_TOWER == type)
            {
                if (0 == (m_nTargetFilter & (((int)TARGET_FILTER.TARGET_FRIENDLY_TOWER) | ((int)TARGET_FILTER.TARGET_ENEMY_TOWER) | ((int)TARGET_FILTER.TARGET_NEUTRAL_TOWER))))
                {
                    return false;
                }
            }
            else //属于怪物
            {
                if (0 == (m_nTargetFilter &
                    (((int)TARGET_FILTER.TARGET_FRIENDLY_SOLDIER)
                    | ((int)TARGET_FILTER.TARGET_ENEMY_SOLDIER)
                    | ((int)TARGET_FILTER.TARGET_NEUTRAL_LITTLE_MONSTER)
                    | ((int)TARGET_FILTER.TARGET_FRIENDLY_SUMMON_ENTITY)
                    | ((int)TARGET_FILTER.TARGET_ENEMY_SUMMON_ENTITY)
                    | ((int)TARGET_FILTER.TARGET_FRIENDLY_SMALL_DRAGON)
                    | ((int)TARGET_FILTER.TARGET_ENEMY_SMALL_DRAGON)
                    | ((int)TARGET_FILTER.TARGET_NEUTRAL_SMALL_DRAGON)
                    | ((int)TARGET_FILTER.TARGET_NEUTRAL_HUGE_DRAGON)
                    )))
                {
                    return false;
                }
            }
        }
        else if (entityView.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
        {
            if (0 == (m_nTargetFilter & (((int)TARGET_FILTER.TARGET_SELF) | ((int)TARGET_FILTER.TARGET_FRIENDLY_ROLE) | ((int)TARGET_FILTER.TARGET_ENEMY_ROLE) | ((int)TARGET_FILTER.TARGET_NEUTRAL_ROLE))))
            {
                return false;
            }
        }
        return true;
    }

    //点了右边头像，判断是否符合技能要求
    public bool onClickPlayer(int uid)
    {
        U3D_Render.EntityView view = EntityFactory.getEntityViewByID(uid);
        if (view == null)
        {
            return false;
        }
        if (isTargetValid(view.gameObject, view.StateMachine, m_selectDistance, view))
        {
            //合法，更新目标输出
            targetPos = view.gameObject.transform.position;
            targetID = uid;
            BaseStateMachine t_targetMachine = view.StateMachine;
            if (t_targetMachine != null)
            {
                //CreatureProperty property = t_targetMachine.creaturePropety;
                //if (property != null)
                //{
                if (t_targetMachine.wound != null)
                {
                    targetPos = t_targetMachine.wound.position;
                }
                //}
            }
            return true;
        }
        return false;
    }

    //供SelectEffectManager使用，判断对象是否符合选取过滤对象
    //与isTargetValid不同，哪怕配置了穿越物体对象，也会按照m_nTargetFilter判断，与拾取线过滤对象不一样
    static public bool checkTargetFilter(U3D_Render.EntityView ev, int m_nTargetFilter)
    {
        //bool a = (ev.CampFlag == CampFlag.CampFlag_Enemy);
        //int b = ((int)TARGET_FILTER.TARGET_ENEMY_ROLE | (int)TARGET_FILTER.TARGET_ENEMY_MONSTER | (int)TARGET_FILTER.TARGET_ENEMY_TOWER);
        //int c = m_nTargetFilter & b;
        //Debug.Log("filter" + a.ToString() + b.ToString() +" "+c.ToString());

        if (ev == null || !ev.StateMachine || !ev.StateMachine.GetVisible())
        {
            return false;
        }

        if (ev.CampFlag == CampFlag.CampFlag_Enemy &&
        (0 == (m_nTargetFilter
            & ((int)TARGET_FILTER.TARGET_ENEMY_ROLE
            | (int)TARGET_FILTER.TARGET_ENEMY_SOLDIER
            | (int)TARGET_FILTER.TARGET_ENEMY_TOWER
            | (int)TARGET_FILTER.TARGET_ENEMY_SUMMON_ENTITY
            | (int)TARGET_FILTER.TARGET_ENEMY_SMALL_DRAGON
            )))
        )
        {
            return false;
        }
        else if (ev.CampFlag == CampFlag.CampFlag_Friend &&
            (0 == (m_nTargetFilter &
            ((int)TARGET_FILTER.TARGET_FRIENDLY_ROLE
            | (int)TARGET_FILTER.TARGET_FRIENDLY_SOLDIER
            | (int)TARGET_FILTER.TARGET_FRIENDLY_TOWER
            | (int)TARGET_FILTER.TARGET_FRIENDLY_SUMMON_ENTITY
            | (int)TARGET_FILTER.TARGET_FRIENDLY_SMALL_DRAGON
            )))
            )
        {
            return false;
        }
        else if (ev.CampFlag == CampFlag.CampFlag_Self &&
            (0 == (m_nTargetFilter & ((int)TARGET_FILTER.TARGET_SELF)))
            )
        {
            return false;
        }
        else if (ev.CampFlag == CampFlag.CampFlag_Neutral &&
            (0 == (m_nTargetFilter
                & ((int)TARGET_FILTER.TARGET_NEUTRAL_ROLE)
                | (int)TARGET_FILTER.TARGET_NEUTRAL_LITTLE_MONSTER
                | (int)TARGET_FILTER.TARGET_NEUTRAL_TOWER
                | (int)TARGET_FILTER.TARGET_NEUTRAL_SMALL_DRAGON
                | (int)TARGET_FILTER.TARGET_NEUTRAL_HUGE_DRAGON
            ))
            )
        {
            return false;
        }

        if (0 == (m_nTargetFilter & (int)TARGET_FILTER.TARGET_BODY_USE))
        {
            if (ev.StateMachine.GetState() == EntityState.Dead)
            {
                return false;
            }
        }

        if (0 != (m_nTargetFilter & (int)TARGET_FILTER.TARGET_NO_ALIVE_USE))
        {
            if (ev.StateMachine.GetState() != EntityState.Dead)
            {
                return false;
            }
        }
        return true;
    }

    /// 模糊选取
    /// </summary>
    /// <param name="traceRay">输入拾取线，按拾取线取最近</param>      
    /// 输出bool，是否选取成功


    private bool isSnapMonster(int Flag)
    {
        if ((int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_SOLDIER == Flag && m_bSnapToObject_AllowSoldier)
            return true;
        if ((int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER == Flag)
        {
            return true;
        }
        if ((int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_BASE == Flag)
        {
            return true;
        }

        if ((int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_CRYSTAL_TOWER == Flag)
        {
            return true;
        }

        if ((int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_SMALL_DRAGON == Flag)
        {
            return true;
        }

        if ((int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_HUGE_DRAGON == Flag)
        {
            return true;
        }

        return false;
    }
    private string appearHeroName = "";
    private bool IsEnemyAround(Vector3 startPos)
    {
        bool isEnemyAround = false;     //判断周围有没有敌方英雄，有的话就不能选择小兵了
        ICollection<U3D_Render.EntityView> heros = EntityFactory.GetPlayerList();
        foreach (U3D_Render.EntityView hero in heros)
        {
            if (hero != null && hero.CampFlag == CampFlag.CampFlag_Enemy)
            {
                startPos.y = 0;
                Vector3 enemyPos = new Vector3(hero.StateMachine.transform.position.x, 0, hero.StateMachine.transform.position.z);
                float dist = Vector3.Distance(startPos, enemyPos);
                if (dist < 20)
                {
                    isEnemyAround = true;
                    appearHeroName = hero.StateMachine.transform.name;
                    break;
                }
            }
        }
        return isEnemyAround;
    }

    public bool calCastSnap(Ray traceRay, bool rayHit = false)
    {
        if (SoldierCamera.MainInstance() == null)
        {
            return false;
        }

        float rayAngleRad = SoldierCamera.MainInstance<SoldierCamera>().GetRayTraceAngle() * Mathf.PI / 180.0f;
        float rayThroad = Mathf.Cos(rayAngleRad);

        Vector3 heroPos = transform.position;
        if (m_bHeroProjectToFloor)
        {
            heroPos = ProjectToFloor(heroPos);
        }


        //算一条英雄的面向线，与面向线夹角超过rayTraceAngle度的怪物不取
        //垫底方案：将拾取射线方向投影到英雄面向的平面上（即以up为法向的平面上）
        Vector3 faceDirection = Vector3.Cross(transform.up, Vector3.Cross(traceRay.direction, transform.up));
        //更好的方案，去拾取射线与地面的焦点，面向线为英雄向焦点的射线
        int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault)) | (1 << LayerMask.NameToLayer(Config.LayerBuilding));
        RaycastHit hitGroundInfo;
        bool hitGround = Physics.Raycast(traceRay, out hitGroundInfo, MAX_CAST_DISTANCE, LayerMaskOnGround);
        if (hitGround)
        {
            faceDirection = hitGroundInfo.point - heroPos;
        }
        faceDirection.Normalize();

        float nearestDistance = m_selectDistance;
        ENTITY_TYPE nearestType = ENTITY_TYPE.TYPE_MONSTER;
        SelectViewType nearestViewAngle = SelectViewType.Out; //目前选中的目标是否在选择角度内，如果是的话则优先选择
        bool isEnemyAround = IsEnemyAround(this.transform.position);
        bool bHitObject = false;
        Vector3 rayEndPos = traceRay.GetPoint(m_selectDistance + (heroPos - traceRay.origin).magnitude);
        if (targetID > 0) //优先选择已经选择的target，防止抖动
        {
            U3D_Render.EntityView snapView = EntityFactory.getEntityViewByID(targetID);
            if (snapView != null)
            {
                SelectViewType viewType = calViewType(snapView, faceDirection, rayThroad);
                if (viewType != SelectViewType.Out)
                {
                    Vector3 projectPos = Vector3.zero;
                    ENTITY_TYPE t_type = snapView.Type;
                    if (!rayHit && !m_bDownGradeTower && snapView.Type == ENTITY_TYPE.TYPE_MONSTER) //rayHit情况下不提升塔的优先级
                    {
                        //if ((int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER == snapView.Flag)
                        //if ((int)MONSTER_TYPE.MONSTER_TYPE_TOWER == snapView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX))
                        if (isSnapMonster(snapView.Flag))
                        {
                            t_type = ENTITY_TYPE.TYPE_PLAYER_ROLE;
                        }
                        else
                        {
                            if (!m_bPlayerSelectFirst || (!isEnemyAround && (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_SOLDIER == snapView.Flag))
                            {   //如果周围没有敌方英雄，就可以选择小兵了                 
                                t_type = ENTITY_TYPE.TYPE_PLAYER_ROLE;
                            }
                        }
                    }

                    if (t_type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
                    {
                        float dis = CollisionDetection.DisPoint2Segment(snapView.gameObject.transform.position, traceRay.origin, rayEndPos, ref projectPos);
                        nearestDistance = dis * 0.9f; //优先选择已经选择的target，防止抖动
                        nearestType = t_type;
                        nearestViewAngle = viewType;
                        targetPos = targetObject.transform.position;
                        if (targetMachine.wound != null)
                        {
                            //因为在优先级2没有选中实体的话，会给出一个地面的targetPos，
                            //这里表示模糊选取成功，恢复成实体的targetPos
                            targetPos = targetMachine.wound.position;
                        }
                        bHitObject = true;
                    }
                }
            }
            //if (!bHitObject)
            //{ 之后还有第4个优先级，所以这里先不清空了
            //    zeroTarget();
            //}            
        }

        ICollection<U3D_Render.EntityView> all_entities = EntityFactory.GetEntityList();
        foreach (U3D_Render.EntityView ev in all_entities)
        {
            if (ev == null) continue;

            int id = ev.ID;
            if (id == targetID)
            {
                continue;
            }

            ENTITY_TYPE t_Type = ev.Type;

            if (!ev.gameObject || !ev.Property)
            {
                continue;
            }

            //if (!checkTargetFilter(ev, m_nTargetFilter)) continue;

            if (!rayHit && !m_bDownGradeTower && ev.Type == ENTITY_TYPE.TYPE_MONSTER && ev.Property) //rayHit情况下不提升塔的优先级
            {
                //if ((int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER == ev.Flag)
                //if ((int)MONSTER_TYPE.MONSTER_TYPE_TOWER == ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX))
                if (isSnapMonster(ev.Flag))
                {
                    t_Type = ENTITY_TYPE.TYPE_PLAYER_ROLE;
                }
                else
                {
                    if (!bHitObject && (!m_bPlayerSelectFirst || (!isEnemyAround && (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_SOLDIER == ev.Flag)))    //如果周围没有敌方英雄，就可以选择小兵了                 
                        t_Type = ENTITY_TYPE.TYPE_PLAYER_ROLE;
                }
            }

            if (t_Type != ENTITY_TYPE.TYPE_PLAYER_ROLE)
            {
                continue;
            }

            //if (m_bPlayerSelectFirst && nearestType == ENTITY_TYPE.TYPE_PLAYER_ROLE && t_Type != ENTITY_TYPE.TYPE_PLAYER_ROLE)
            //{
            //    continue;
            //}

            if (!isTargetValid(ev.gameObject, ev.StateMachine, m_selectDistance, ev))
            {
                continue;
            }

            SelectViewType viewType = calViewType(ev, faceDirection, rayThroad);

            if ((viewType == SelectViewType.Out) || (nearestViewAngle < viewType))
            {
                continue;
            }

            //算距离
            Vector3 projectPos = Vector3.zero;
            bool selectFirst = false;
            float dis = CollisionDetection.DisPoint2Segment(ev.gameObject.transform.position, traceRay.origin, rayEndPos, ref projectPos);
            if (m_bPlayerSelectFirst && t_Type == ENTITY_TYPE.TYPE_PLAYER_ROLE && nearestType != ENTITY_TYPE.TYPE_PLAYER_ROLE)
            {
                selectFirst = true;
            }

            if (nearestViewAngle > viewType)
            {
                selectFirst = true;
            }

            if (!rayHit && nearestDistance > dis)
            {
                //rayHit==true 表示已经精确选择到某怪了，距离上已经是最近的了，只有英雄优先于怪的情况才可能覆盖
                selectFirst = true;
            }

            if (selectFirst)
            {
                nearestDistance = dis;
                bHitObject = true;
                nearestType = t_Type;
                nearestViewAngle = viewType;
                targetID = id;
                targetObject = ev.gameObject;
                targetPos = ev.gameObject.transform.position;
                targetMachine = ev.StateMachine;
                targetEntityView = ev;
                if (targetMachine.wound != null)
                {
                    targetPos = targetMachine.wound.position;
                }

                rayHit = false; //已经被覆盖，目前最优已经不是精确选择的结果了，所以rayHit失效
            }
        }

        return bHitObject;
    }

    private void zeroTarget()
    {
        targetID = 0;
        targetObject = null;
        targetPos = Vector3.zero;
        targetMachine = null;
        targetEntityView = null;
    }

    /// 拾取工具
    private int lockDrawingTargetID = -1;
    private Vector3 m_lastRecordPos = Vector3.zero;
    private void rayCast(bool recordLastPos = false, float deltaAngle = -1.0f)
    {
        int layMask = LayerMaskOnPos;
        float traceDis = m_selectDistance;

        if (!machine)
        {
            return;
        }

        if (!Camera.main || !Camera.main.transform)
        {
            return;
        }


        //死亡时取消所有选择
        if (machine.GetState() == EntityState.Dead)
        {
            if (targetID > 0)
            {
                zeroTarget();
                //更新锁定框
                updateTargetFrame();
            }
            if (bLockTarget)
            {
                //脱离锁定状态
                bLockTarget = false;
                m_bKeyQuoteLock = false;
                setSkillLock(false);//m_bSkillLock = false;
                //SoldierCamera.MainInstance<SoldierCamera>().setLockTarget(null);
            }

            return;
        }

        bool bHitObject = false;

        //第一优先级，锁定目标
        if (m_bSnapToObject && bLockTarget && targetID > 0)
        {
            if (!isTargetValid(targetObject, targetMachine, machine.creaturePropety.lockAimDistance, targetEntityView))
            { //不合法时，脱离锁定状态
                bLockTarget = false;
                m_bKeyQuoteLock = false;
                setSkillLock(false); //m_bSkillLock = false;
                //SoldierCamera.MainInstance<SoldierCamera>().setLockTarget(null);
            }
            else
            {
                //bHitObject = true;
                //if (targetID != lockDrawingTargetID)
                //{
                //    TargetFrame.SelectTargetParam param;
                //    param.nTargetId = targetID;
                //    param.bIngoreBossMark = true;
                //    //GfxBloodManager.OnSelectTarget(param);
                //    USpeedUI.UBloodManager.Instance.OnSelectTarget(param);
                //    lockDrawingTargetID = targetID;
                //}
            }
        }

        if (!bHitObject)
        {
            if (lockDrawingTargetID > 0)
            {
                TargetFrame.SelectTargetParam param;
                param.nTargetId = -1;
                param.bIngoreBossMark = true;
                //GfxBloodManager.OnSelectTarget(param);
                USpeedUI.Blood.UBloodManager.Instance.OnSelectTarget(param);
                lockDrawingTargetID = -1;
            }
        }

        if (!bHitObject && targetID > 0)
        {
            //没有锁定的情况下，按技能情况选取，此时如果已选目标不合法，则清空已选目标

            float checkDistance = m_selectDistance;
            //if (targetMachine == attackHeroMachine) 2017.03.14撤销选中攻击我的对象时，按瞄准框距离的设定
            //{
            //    checkDistance = machine.creaturePropety.attackedAimDistance;
            //}
            if (!isTargetValid(targetObject, targetMachine, checkDistance, targetEntityView))
            {
                zeroTarget();
            }
        }

        //第二优先级，鼠标正好选中
        SelectInputType selectType = m_selectInputType;
        if (deltaAngle > 0.0f)
        {
            selectType = SelectInputType.MouseGuideAngle;
        }

        bool rayHit = false;

        Ray castRay = calCastRay(selectType, m_bRight, ref traceDis, m_lastRecordPos, deltaAngle);

        Vector3 heroPos = transform.position;
        if (m_bHeroProjectToFloor)
        {
            heroPos = ProjectToFloor(heroPos);
        }

        ENTITY_TYPE hitType = ENTITY_TYPE.TYPE_MONSTER;
        if (!bHitObject)
        {
            RaycastHit hitInfo;
            bool hit = false;
            calRayHitInfo(out hitInfo, out hit, castRay, layMask, traceDis);

            targetPos = hitInfo.point; //即使没有选中人物，也有targetPos
            bool hitTarget = false;
            if (hit && hitInfo.transform && calHitID(hitInfo.transform))
            {
                //2016-03-17新要求，英雄必须优先选中，精确选中怪也没用     
                if (targetID > 0)
                {
                    U3D_Render.EntityView hitView = EntityFactory.getEntityViewByID(targetID);
                    if (hitView != null)
                    {
                        //对于敌方的NPC，不允许选中
                        if (hitView.CampFlag != CampFlag.CampFlag_Enemy || hitView.Flag != (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_NPC)
                        {
                            hitType = hitView.Type;
                            if (hitView.StateMachine.isNotAttack) //2017.03.06 NPC必须精确选中，但是优先级和英雄一个级别
                            {
                                hitType = ENTITY_TYPE.TYPE_PLAYER_ROLE;
                            }
                            bHitObject = true;
                            hitTarget = true;
                        }
                    }
                }
                rayHit = true;
            }
            if (!hitTarget)
            {
                //算精确距离
                Vector3 deta = targetPos - heroPos;

                float absSelectDistance = Mathf.Abs(m_selectDistance);
                if (deta.magnitude > absSelectDistance)
                {
                    deta.Normalize();
                    targetPos = heroPos + deta * absSelectDistance;
                }

            }
        }


        //第三优先级，模糊选取
        //2016-10-12，需求：小兵不能模糊选取
        //if (!bHitObject && m_bSnapToObject)
        if ((!bHitObject || hitType == ENTITY_TYPE.TYPE_MONSTER) && (m_bSnapToObject))
        {
            if (calCastSnap(castRay, rayHit))
            {
                bHitObject = true;
            }
        }

        if (!bHitObject)
        {
            //可以选自己的情况下，如果其他方式都选不到，则选择自己
            if (1 == (m_nTargetFilter & (int)TARGET_FILTER.TARGET_SELF))
            {
                calHitID(transform);
                bHitObject = true;
            }
        }

        //2017.11.7 取消第四优先级概念，不再自动选择攻击我的英雄
        if (attackHeroMachine != null)
        {
            attackHeroMachine = null;
        }

        //if (bHitObject && attackHeroMachine!=targetMachine)
        //{
        //    //如果其他方式能选中，那么不选择攻击我的英雄，所以这里把这攻击者清空
        //    attackHeroMachine = null;
        //}
        //else if (attackHeroMachine != null)
        //{
        //    //如果是按准心位置选择，则以准心位置为准，不强制进入第四个优先级，选择攻击我的英雄
        //    if (selectType == SelectInputType.MousePosAndCrossHairPos || selectType == SelectInputType.CrossHairPos)
        //    {
        //        attackHeroMachine = null;
        //    }
        //    else
        //    {
        //        //否则进入第4个优先级，选择攻击我的英雄
        //        U3D_Render.EntityView entityview = EntityFactory.getEntityViewByID(attackHeroMachine.entityID);
        //        //if (isTargetValid(attackHeroMachine.gameObject, attackHeroMachine, machine.creaturePropety.attackedAimDistance,entityview)) 2017.03.14撤销选中攻击我的对象时，按瞄准框距离的设定
        //        if (isTargetValid(attackHeroMachine.gameObject, attackHeroMachine, m_selectDistance, entityview))
        //        {
        //            calHitID(attackHeroMachine.transform);
        //            bHitObject = true;
        //        }
        //        else //一旦不合法，就不再选他了
        //        {
        //            attackHeroMachine = null;
        //        }
        //    }
        //}

        if (!bHitObject)
        {
            targetID = 0;
            targetObject = null;
            targetMachine = null;
            targetEntityView = null;
        }

        //更新锁定框
        updateTargetFrame();

        //如果需要投影到地面,则targetPos取向下与地面或建筑物的焦点
        if (m_bProjectToFloor)
        {
            targetPos = ProjectToFloor(targetPos);
        }


        if (m_bSelectMaxDistancePoint)
        {
            Vector3 startPos = heroPos;
            if (machine && machine.wound)
            {
                //靠近墙壁的时候鼠标选墙，再算最远距离的时候，从脚底开始的方向会朝天，因此采用中心的wound点作为起点，targetDirection会更合理
                if (targetPos.y > machine.wound.position.y)
                {
                    startPos = machine.wound.position;
                }
            }
            Vector3 targetDirection = targetPos - startPos;
            targetDirection.Normalize();
            targetPos = startPos + targetDirection * m_selectDistance;
            //如果需要投影到地面,这里需要再投影一次
            int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault)) | (1 << LayerMask.NameToLayer(Config.LayerBuilding));
            RaycastHit hitGroundInfo;
            bool hitGround = false;
            if (m_bProjectToFloor)
            {
                targetPos = ProjectToFloor(targetPos);
            }

            if (m_bEnableGroundBlock)
            {
                //再检测一次碰撞，如果有地形或者建筑物阻挡的话，则在建筑物以内
                targetDirection = targetPos - heroPos;
                targetDirection.Normalize();
                Ray detectRay = new Ray(heroPos + new Vector3(0.0f, m_fTraceHeight, 0.0f), targetDirection);
                hitGround = Physics.Raycast(detectRay, out hitGroundInfo, m_selectDistance, LayerMaskOnGround);
                if (hitGround)
                {
                    if (Vector3.Distance(heroPos, hitGroundInfo.point) < Vector3.Distance(heroPos, targetPos)) //如果障碍物在中间
                        targetPos = hitGroundInfo.point;
                }
            }
        }
        else if (m_bProjectToFloor)
        {
            RaycastHit hitGroundInfo;
            bool hitGround = false;
            if (m_bEnableGroundBlock)
            {
                //再检测一次碰撞，如果有地形或者建筑物阻挡的话，则在建筑物以内
                Vector3 targetDirection = targetPos - heroPos;
                targetDirection.Normalize();
                Ray detectRay = new Ray(heroPos + new Vector3(0.0f, m_fTraceHeight, 0.0f), targetDirection);
                int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault)) | (1 << LayerMask.NameToLayer(Config.LayerBuilding));
                hitGround = Physics.Raycast(detectRay, out hitGroundInfo, m_selectDistance, LayerMaskOnGround);
                if (hitGround)
                {
                    if (Vector3.Distance(heroPos, hitGroundInfo.point) < Vector3.Distance(heroPos, targetPos)) //如果障碍物在中间
                        targetPos = hitGroundInfo.point;
                }
            }
            targetPos = ProjectToFloor(targetPos);
        }


        if (recordLastPos)
        {
            m_lastRecordPos = targetPos;
        }
    }

    //-----------------------------------------------------
    //以下为SkillCtrl的调用，得到targetID和targetPos
    //-----------------------------------------------------

    private int m_nDefaultSelectType = 0;
    private int m_nDefaultTargetFilter = 0;
    private int m_nDefaultSelectDrawType = 0;
    private float m_fDefaultSelectDrawAttackRange = 0.0f;
    private float m_fDefaultSelectDrawChooseRange = 0.0f;
    private int m_nDefaultSelectDrawTargetType = 0;
    private SelectEffectManager m_selectEffects = null;
    private bool drawingDefaultSelectDrawShape = false; //是否正在画普攻的圆圈，在setSelection刷成false，在SetDefaultSelection按需求刷成True
    public void setDefaultSelectionParam(int nDefaultSelectType, int nDefaultTargetFilter, int nDefaultSelectDrawType, float fDefaultSelectDrawAttackRange, float fDefaultSelectDrawChooseRange, int nDefaultSelectDrawTargetType, SelectEffectManager selectEffects, bool resetSelection)
    {
        m_nDefaultSelectType = nDefaultSelectType;
        m_nDefaultTargetFilter = nDefaultTargetFilter;
        m_nDefaultSelectDrawType = nDefaultSelectDrawType;
        m_fDefaultSelectDrawAttackRange = fDefaultSelectDrawAttackRange;
        m_fDefaultSelectDrawChooseRange = fDefaultSelectDrawChooseRange;
        m_nDefaultSelectDrawTargetType = nDefaultSelectDrawTargetType;
        if (m_selectEffects != null)
        {
            m_selectEffects.disableEffect();
        }
        m_selectEffects = selectEffects;
        if (resetSelection)
        {
            SetDefaultSelection();
        }
    }
    public void SetDefaultSelection()
    {
        if (m_nDefaultSelectType > 0)
        {
            SetSelection(m_nDefaultSelectType, defaultAimDistance, 0.0f, m_nDefaultTargetFilter);
        }
        else
        {
            int nSelectType = (((int)SPELL_SELECT_TYPE.SpellSelectType_SelectMousePos) | ((int)SPELL_SELECT_TYPE.SpellSelectType_SnapToObject));
            m_nTargetFilter = (((int)TARGET_FILTER.TARGET_ENEMY_ROLE) | ((int)TARGET_FILTER.TARGET_ENEMY_SOLDIER) | ((int)TARGET_FILTER.TARGET_ENEMY_SUMMON_ENTITY)
                | ((int)TARGET_FILTER.TARGET_ENEMY_SMALL_DRAGON)
                | ((int)TARGET_FILTER.TARGET_NEUTRAL_ROLE) | ((int)TARGET_FILTER.TARGET_NEUTRAL_LITTLE_MONSTER)
                | ((int)TARGET_FILTER.TARGET_NEUTRAL_SMALL_DRAGON) | ((int)TARGET_FILTER.TARGET_NEUTRAL_HUGE_DRAGON)
                | ((int)TARGET_FILTER.TARGET_ENEMY_TOWER) | ((int)TARGET_FILTER.TARGET_NEUTRAL_TOWER));
            SetSelection(nSelectType, defaultAimDistance, 0.0f, m_nTargetFilter);
        }

        if (m_nDefaultSelectDrawType > 0)
        {
            if (m_selectEffects != null)
            {
                int drawShape = (m_nDefaultSelectDrawType & ((int)SPELL_DRAW_TYPE.DRAW_TYPE_RECT | (int)SPELL_DRAW_TYPE.DRAW_TYPE_SECTOR | (int)SPELL_DRAW_TYPE.DRAW_TYPE_ROUND));
                m_selectEffects.enableEffect(drawShape, m_nDefaultSelectDrawType, defaultAimDistance, m_fDefaultSelectDrawAttackRange, m_fDefaultSelectDrawChooseRange, m_nDefaultSelectDrawTargetType, 0);   //DefaultSelect不需要管build层，所以SelectType传0
                m_selectEffects.updateEffect(targetPos);
                drawingDefaultSelectDrawShape = true;
            }
        }


        m_isDefaultAim = true;
    }

    public void SetPKTargetSelection()
    {
        int nSelectType = (((int)SPELL_SELECT_TYPE.SpellSelectType_SelectMousePos) | ((int)SPELL_SELECT_TYPE.SpellSelectType_SnapToObject) | ((int)SPELL_SELECT_TYPE.SpellSelectType_MouseClick));
        m_nTargetFilter = (((int)TARGET_FILTER.TARGET_FRIENDLY_ROLE));
        SetSelection(nSelectType, defaultAimDistance, 0.0f, m_nTargetFilter);
        m_isDefaultAim = true;
    }

    public void SetFriendTargetSelection()
    {
        int nSelectType = (((int)SPELL_SELECT_TYPE.SpellSelectType_SelectMousePos));
        m_nTargetFilter = ((int)TARGET_FILTER.TARGET_FRIENDLY_ROLE);
        SetSelection(nSelectType, defaultAimDistance, 0.0f, m_nTargetFilter);
    }


    //nTargetFilter默认7，排除自己，友方英雄和友方怪物
    public void SetSelection(int selectType, float dis = 0.0f, float height = 0.0f, int nTargetFilter = 888, bool isSkillSelect = false, bool isStrafe = false)
    {
        //Debug.Log("SetSelection: selectType=" + selectType + "dis=" + dis + "height=" + height + "nTargetFilter=" + nTargetFilter + "isSkillSelect=" + isSkillSelect + "isStrafe=" + isStrafe);
        m_selectInputType = SelectInputType.MousePos;
        m_bIsStrafe = isStrafe;

        if ((selectType & ((int)SPELL_SELECT_TYPE.SpellSelectType_NoSelect)) != 0)
        {
            dis = 0.0f;
        }
        else if ((selectType & ((int)SPELL_SELECT_TYPE.SpellSelectType_SelectFaceDirection)) != 0)
        {
            m_selectInputType = SelectInputType.FaceDirection;
        }
        else if ((selectType & ((int)SPELL_SELECT_TYPE.SpellSelectType_SelectWalkDirection)) != 0)
        {
            m_selectInputType = SelectInputType.WalkingDirection;
            if ((selectType & ((int)SPELL_SELECT_TYPE.SpellSelectType_MoveDirDefaultRawInputDir)) != 0)
            {
                m_selectInputType = SelectInputType.MoveDirDefaultRawInputDir;
            }
        }
        else if ((selectType & ((int)SPELL_SELECT_TYPE.SpellSelectType_SelectAimPos)) != 0)
        {
            m_selectInputType = SelectInputType.CrossHairPos;

            if ((selectType & ((int)SPELL_SELECT_TYPE.SpellSelectType_SelectMousePos)) != 0)
            {
                m_selectInputType = SelectInputType.MousePosAndCrossHairPos;
            }
        }

        m_bRight = false;
        if ((selectType & ((int)SPELL_SELECT_TYPE.SpellSelectType_SelectRightDirection)) != 0)
        {
            m_bRight = true;
        }

        m_bSnapToObject = false;
        if ((selectType & ((int)SPELL_SELECT_TYPE.SpellSelectType_SnapToObject)) != 0)
        {
            m_bSnapToObject = true;
        }

        m_bSnapToObject_AllowSoldier = false;
        if ((selectType & ((int)SPELL_SELECT_TYPE.SpellSelectType_SnapToObject_AllowSoldier)) != 0)
        {
            m_bSnapToObject_AllowSoldier = true;
        }

        //如果是锁定对象的技能，而且瞄准框有选中对象，那么就直接把对象输出就可以了
        //if (isSkillSelect && bDrawingTarget && ((selectType & (int)SPELL_SELECT_TYPE.SpellSelectType_NoShowAperture) == 0))
        //{
        //    return;
        //}

        m_bProjectToFloor = false;
        if ((selectType & (int)SPELL_SELECT_TYPE.SpellSelectType_ProjectToGround) != 0)
        {
            m_bProjectToFloor = true;
        }

        m_bProjectToFloorSkipBuilding = false;
        if ((selectType & (int)SPELL_SELECT_TYPE.SpellSelectType_ProjectToGroundSkipBuilding) != 0)
        {
            m_bProjectToFloorSkipBuilding = true;
        }


        m_bCrossObject = false; //target点选择为拾取目标对象身后的点
        if ((selectType & (int)SPELL_SELECT_TYPE.SpellSelectType_CrossObject) != 0)
        {
            m_bCrossObject = true;
        }

        m_bPlayerSelectFirst = true; //是否优先拾取玩家英雄
        if ((selectType & (int)SPELL_SELECT_TYPE.SpellSelectType_DownGradeHero) != 0)
        {
            m_bPlayerSelectFirst = false;
        }

        m_bEnableGroundBlock = false; //是否考虑地形阻挡
        if ((selectType & (int)SPELL_SELECT_TYPE.SpellSelectType_EnableGroundBlock) != 0)
        {
            m_bEnableGroundBlock = true;
        }

        m_bEnableWardBlock = false; //是否考虑结界阻挡
        if ((selectType & (int)SPELL_SELECT_TYPE.SpellSelectType_EnableWardBlock) != 0)
        {
            m_bEnableWardBlock = true;
        }

        m_bSelectMaxDistancePoint = false; //是否考虑地形遮挡
        if ((selectType & (int)SPELL_SELECT_TYPE.SpellSelectType_SelectDistance) != 0)
        {
            m_bSelectMaxDistancePoint = true;
        }

        m_bDownGradeTower = false; //将塔的选择优先级降低至小兵
        if ((selectType & (int)SPELL_SELECT_TYPE.SpellSelectType_DownGradeTower) != 0)
        {
            m_bDownGradeTower = true;
        }

        m_bWalkDirectionDefaultBack = false; //将塔的选择优先级降低至小兵
        if ((selectType & (int)SPELL_SELECT_TYPE.SpellSelectType_MoveDirDefaultBack) != 0)
        {
            m_bWalkDirectionDefaultBack = true;
        }

        m_bHeroProjectToFloor = false; //选取技能时英雄坐标贴地
        if ((selectType & (int)SPELL_SELECT_TYPE.SpellSelectType_HeroProjectToFloor) != 0)
        {
            m_bHeroProjectToFloor = true;
        }

        m_bPickBuilding = false; //取点时检测建筑层
        if ((selectType & (int)SPELL_SELECT_TYPE.SpellSelectType_CheckBuildingLayer) != 0)
        {
            m_bPickBuilding = true;
        }

        m_selectDistance = dis;
        m_fTraceHeight = height;
        if (m_fTraceHeight <= 0.001f)
        {
            m_fTraceHeight = 2.0f;
        }

        m_nTargetFilter = nTargetFilter;
        updateLayerMask();
        if (m_bIsStrafe)
        {
            rayCast(true); //强制更新一遍
        }
        else
        {
            m_lastRecordPos = Vector3.zero;
            rayCast(); //强制更新一遍
        }


        m_isDefaultAim = false;
        drawingDefaultSelectDrawShape = true;
    }


    public void updateStrafeRotate(float angle)
    {
        rayCast(true, angle);
    }

    private void updateLayerMask()
    {
        LayerMaskOnPos = collisionLayers.value;
        if (m_bCrossObject)
        {
            LayerMaskOnPos = 1 << LayerMask.NameToLayer(Config.LayerDefault);
            if (m_bPickBuilding)
            {
                LayerMaskOnPos = LayerMaskOnPos | (1 << LayerMask.NameToLayer(Config.LayerBuilding));
            }
            return;
        }

        if (0 == (m_nTargetFilter
            & ((int)TARGET_FILTER.TARGET_NEUTRAL_ROLE
                | (int)TARGET_FILTER.TARGET_NEUTRAL_LITTLE_MONSTER
                | (int)TARGET_FILTER.TARGET_NEUTRAL_SMALL_DRAGON
                | (int)TARGET_FILTER.TARGET_NEUTRAL_HUGE_DRAGON
                )))
        {
            LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerNeutrality)));
        }

        if (0 == (m_nTargetFilter & (int)TARGET_FILTER.TARGET_SELF))
        {
            LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerHero)));
        }
        if (0 == (m_nTargetFilter & (int)TARGET_FILTER.TARGET_FRIENDLY_ROLE))
        {
            LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerFriend)));
        }
        if (0 == (m_nTargetFilter & (int)TARGET_FILTER.TARGET_FRIENDLY_SOLDIER) && (0 == (m_nTargetFilter & (int)TARGET_FILTER.TARGET_FRIENDLY_TOWER)))
        {
            LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterFriend)));
        }
        if (0 == (m_nTargetFilter & (int)TARGET_FILTER.TARGET_ENEMY_ROLE))
        {
            LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerEnemy)));
        }
        if ((0 == (m_nTargetFilter & ((int)TARGET_FILTER.TARGET_ENEMY_SOLDIER | (int)TARGET_FILTER.TARGET_ENEMY_SUMMON_ENTITY))) && (0 == (m_nTargetFilter & (int)TARGET_FILTER.TARGET_ENEMY_TOWER)))
        {
            LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterEnemy)));
        }

        //if ((0 == (m_nTargetFilter & (int)TARGET_FILTER.TARGET_FRIENDLY_MONSTER)) || (0 == (m_nTargetFilter & (int)TARGET_FILTER.TARGET_FRIENDLY_MONSTER)))
        //{
        //    LayerMaskOnObject = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastFriend)));
        //}

        //if ((0 == (m_nTargetFilter & (int)TARGET_FILTER.TARGET_ENEMY_MONSTER)) || (0 == (m_nTargetFilter & (int)TARGET_FILTER.TARGET_ENEMY_ROLE)))
        //{
        //    LayerMaskOnObject = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastEnemy)));
        //}

        LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerOcclusCulling)));
        LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastFriend)));
        LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastEnemy)));
        LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerNeutrality)));


        //关于ward结界类型的特殊处理
        //结界类型的物理盒子放在LayerWard层，用于阻挡指定类型的玩家
        //但是结界有可以选中和不可以选中，
        //这里只将配置的ScreenCastObject改为其他层，那么不可选中的结界不配置ScreenCastObject，就不影响结界后面实体的技能选中，
        //除了ScreenCastObject之外，实体的其他部分仍然为LayerWard，在raycast射线里排除LayerWard，则只有ScreenCastObject的实体可以被hit到
        //hit到了之后判断实体有效性的时候，取实体对象会发现它在LayerWard层，因此将LayerWard加入有效层的范围内，这样就结界的选中就是合法的了。
        LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerWard_Enemy)));
        LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerWard_Friend)));

        //LayerMaskWithoutCreature = collisionLayers.value;
        //LayerMaskWithoutCreature = LayerMaskWithoutCreature & (~(1 << LayerMask.NameToLayer(Config.LayerHero)));
        //LayerMaskWithoutCreature = LayerMaskWithoutCreature & (~(1 << LayerMask.NameToLayer(Config.LayerNeutrality)));
        //LayerMaskWithoutCreature = LayerMaskWithoutCreature & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastEnemy)));
        //LayerMaskWithoutCreature = LayerMaskWithoutCreature & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastFriend)));
        //LayerMaskWithoutCreature = LayerMaskWithoutCreature & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerEnemy)));
        //LayerMaskWithoutCreature = LayerMaskWithoutCreature & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerFriend)));
        //LayerMaskWithoutCreature = LayerMaskWithoutCreature & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterEnemy)));
        //LayerMaskWithoutCreature = LayerMaskWithoutCreature & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterFriend)));
        //LayerMaskWithoutCreature = LayerMaskWithoutCreature & (~(1 << LayerMask.NameToLayer(Config.LayerWard_Friend)));
        //LayerMaskWithoutCreature = LayerMaskWithoutCreature & (~(1 << LayerMask.NameToLayer(Config.LayerWard_Enemy)));

        ////去掉TransparentFX这个层
        //LayerMaskWithoutCreature = LayerMaskWithoutCreature & (~(1 << LayerMask.NameToLayer(Config.LayerTransparentFX)));
        LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerTransparentFX)));
        LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerIngoreRayCast)));
        LayerMaskOnPos = LayerMaskOnPos & (~(1 << LayerMask.NameToLayer(Config.LayerLightingEffectOnly)));
        //LayerMaskOnObject = LayerMaskOnObject & (~(1 << LayerMask.NameToLayer(Config.LayerTransparentFX)));
    }


    #endregion

    #region 基础运算函数
    public float lineDistance;
    /// 计算射线求交
    /// </summary>
    /// <param name="_ClosestHitInfo">输出，最近点实体信息</param>
    /// <param name="bHitObject">输出，是否命中实体</param>
    /// <param name="traceRay">输入，射线信息</param>
    /// <param name="CastLayerMask">输入，检测时排除哪些层</param>
    /// <param name="_Distance">检测距离，默认场景最大值MAX_CAST_DISTANCE</param>
    /// 
    void calRayHitInfo(out RaycastHit _ClosestHitInfo, out bool bHitObject, Ray traceRay, int CastLayerMask, float _Distance = 0.0f)
    {
        lineDistance = _Distance;
        float rayDistance = _Distance;
        if (Mathf.Abs(rayDistance) < 0.0001f)
        {
            rayDistance = MAX_CAST_DISTANCE;
        }


        RaycastHit hitInfo;
        bool isHit;

        if (rayDistance < 0.0f)
        {
            //Scene::raycastClosestShape: The maximum distance must be greater than zero!
            traceRay.direction = traceRay.direction * -1.0f;
            rayDistance = rayDistance * -1.0f;
        }

        isHit = Physics.Raycast(traceRay, out hitInfo, rayDistance, CastLayerMask);
        if (isHit)
        {
            _ClosestHitInfo = hitInfo;
            //Debug.DrawLine(traceRay.origin, traceRay.origin + traceRay.direction.normalized * rayDistance, Color.green);
            Debug.DrawLine(traceRay.origin, hitInfo.point, Color.green);
        }
        else
        {
            _ClosestHitInfo = hitInfo;
            Debug.DrawLine(traceRay.origin, traceRay.origin + traceRay.direction.normalized * rayDistance, Color.yellow);
            _ClosestHitInfo.point = traceRay.origin + traceRay.direction.normalized * rayDistance;
        }
        bHitObject = isHit;
    }

    /// <summary>
    /// 是否是实体，取ID的时候，如果不是实体，就不用取了
    /// </summary>
    /// <param name="tr"></param>
    /// <returns></returns>
    bool IsEntityObj(Transform tr)
    {
        int lay = 1 << tr.gameObject.layer;
        int entityLayer = 0;
        entityLayer |= 1 << LayerMask.NameToLayer(Config.LayerPlayerEnemy);
        entityLayer |= 1 << LayerMask.NameToLayer(Config.LayerPlayerFriend);
        entityLayer |= 1 << LayerMask.NameToLayer(Config.LayerNeutrality);
        entityLayer |= 1 << LayerMask.NameToLayer(Config.LayerMonsterEnemy);
        entityLayer |= 1 << LayerMask.NameToLayer(Config.LayerMonsterFriend);

        //关于ward结界类型的特殊处理
        //结界类型的物理盒子放在LayerWard层，用于阻挡指定类型的玩家
        //但是结界有可以选中和不可以选中，
        //这里只将配置的ScreenCastObject改为其他层，那么不可选中的结界不配置ScreenCastObject，就不影响结界后面实体的技能选中，
        //除了ScreenCastObject之外，实体的其他部分仍然为LayerWard，在raycast射线里排除LayerWard，则只有ScreenCastObject的实体可以被hit到
        //hit到了之后判断实体有效性的时候，取实体对象会发现它在LayerWard层，因此将LayerWard加入有效层的范围内，这样就结界的选中就是合法的了。
        entityLayer |= 1 << LayerMask.NameToLayer(Config.LayerWard_Enemy);
        entityLayer |= 1 << LayerMask.NameToLayer(Config.LayerWard_Friend);

        if ((lay & entityLayer) != 0)
        {
            return true;
        }
        return false;
    }

    /// 根据命中信息获得实体ID和相关属性
    /// 返回值，输入为有效数据，并且已经重写target
    bool calHitID(Transform hitTransform)
    {
        int t_targetID = 0;
        GameObject t_targetObject = null;
        Vector3 t_targetPos = Vector3.zero;
        BaseStateMachine t_targetMachine = null;


        if (hitTransform == null || !IsEntityObj(hitTransform))
        {
            return false;
        }

        //GetComponentInParent会有2KB的GC，想办法干掉
        t_targetMachine = hitTransform.GetComponentInParent<BaseStateMachine>();

        if (t_targetMachine == null)
        {
            return false;
        }

        CreatureProperty property = t_targetMachine.creaturePropety;

        if (t_targetMachine != null && property != null)
        {
            t_targetObject = t_targetMachine.transform.gameObject;
            t_targetPos = t_targetMachine.transform.position;
            if (t_targetMachine.wound != null)
            {
                t_targetPos = t_targetMachine.wound.position;
            }
            if (property.Master != null)
            {
                t_targetID = property.Master.ID;
            }
        }
        else
        {
            return false;
        }

        U3D_Render.EntityView t_entityView = EntityFactory.getEntityViewByID(t_targetMachine.entityID);

        if (isTargetValid(t_targetObject, t_targetMachine, m_selectDistance, t_entityView))
        {
            targetID = t_targetID;
            targetObject = t_targetObject;
            targetPos = t_targetPos;
            targetMachine = t_targetMachine;
            targetEntityView = t_entityView;
            return true;
        }
        return false;
    }

    //判断拾取对象是否在限定角度内
    private SelectViewType calViewType(U3D_Render.EntityView ev, Vector3 faceDirection, float rayThroad)
    {
        bool inAngle = true;

        Vector3 heroPos = transform.position;
        if (m_bHeroProjectToFloor)
        {
            heroPos = ProjectToFloor(heroPos);
        }

        //在ray的角度外的剔除
        Vector3 t_targetPos = ev.gameObject.transform.position;
        Vector3 dir2 = t_targetPos - heroPos;
        dir2.Normalize();
        if (Vector3.Dot(faceDirection, dir2) < rayThroad)
        {
            //如果在角度外，那么再判与怪物包围球的角度，在角度外的再剔除
            if (ev.Property == null)
            {
                inAngle = false;
            }
            else
            {
                float boundRad = ev.Property.CreatureBoundingRad.x;
                float t_targetDis = (heroPos - t_targetPos).magnitude;
                if (t_targetDis > 0 && boundRad < t_targetDis)
                {
                    //float arcAngle = Mathf.Acos(Vector3.Dot(dir1, dir2));
                    //float boundAngle = Mathf.Asin(boundRad / targetDis);
                    //if (arcAngle - boundAngle > rayAngleRad)
                    //{
                    //    continue;
                    //}

                    //以下方案用以替代上文耗时的Asin
                    Vector3 rad = Vector3.Cross(transform.up, dir2);
                    rad.Normalize();
                    rad = rad * boundRad;
                    Vector3 targetBoundPos = t_targetPos + rad;

                    Vector3 dir3 = targetBoundPos - heroPos;
                    dir3.Normalize();
                    if (Vector3.Dot(faceDirection, dir3) < rayThroad)
                    {
                        inAngle = false;
                    }
                }
            }
        }
        if (inAngle) { return SelectViewType.InAngle; }
        if (SoldierCamera.MainInstance<SoldierCamera>().cameraMode != CameraMode.FPSMode) //2016.12.15,策划要求，FPS模式不考虑屏幕范围
        {
            if (isInScreen(t_targetPos))
            {
                return SelectViewType.InSceen;
            }
        }
        return SelectViewType.Out;
    }


    //判断点是否在屏幕内
    //targetPos为判断点
    //screenThroad为阈值0-1，例如阈值为0.1表示在屏幕空间0.1-0.9的范围内才有效
    static public bool isInScreen(Vector3 targetPos, float screenThroad = 0.0f)
    {
        if (!Initialize.mainCam)
        {
            return false;
        }
        targetPos.y += 0.5f; //判断点抬高些
        if (Initialize.mainCam == null)
        {
            return false;
        }
        Vector3 t_viewPos = Initialize.mainCam.WorldToViewportPoint(targetPos);
        if (t_viewPos.z >= 0.0f && t_viewPos.x >= screenThroad && t_viewPos.x <= (1.0f - screenThroad) && t_viewPos.y >= screenThroad && t_viewPos.y <= (1.0f - screenThroad))
        {
            return true;
        }
        return false;
    }




    Vector3 GetCrossHairPos()
    {
        return DataCenter.LogicDataCenter.frontSightDataManager.FrontSightScreenPos;
    }


    void SetCrossHairPos(Vector2 pos)
    {
        //DataCenter.LogicDataCenter.frontSightDataManager.FrontSightPos = pos;
        DataCenter.LogicDataCenter.frontSightDataManager.FrontSightPosOffset = pos;
    }

    void SetCrossHairVisible(bool _visible)
    {
        DataCenter.LogicDataCenter.frontSightDataManager.FrontSightVisible = _visible;
    }

    public bool isBlocked(Vector3 targetPos)
    {
        Vector3 sourcePos = transform.position;
        if (m_bHeroProjectToFloor)
        {
            sourcePos = ProjectToFloor(sourcePos);
        }

        sourcePos.y += m_fTraceHeight;
        if (m_bProjectToFloor)
        {
            targetPos.y += m_fTraceHeight;
        }
        Vector3 ray = targetPos - sourcePos;
        Vector3 rayDir = ray.normalized;
        Ray castRay = new Ray(sourcePos, rayDir);
        int LayerMaskOnBlocked = 0;

        if (m_bEnableWardBlock)
        {
            LayerMaskOnBlocked |= 1 << LayerMask.NameToLayer(Config.LayerDefault);
            LayerMaskOnBlocked |= 1 << LayerMask.NameToLayer(Config.LayerBuilding);
        }

        if (m_bEnableWardBlock)
        {
            LayerMaskOnBlocked |= 1 << LayerMask.NameToLayer(Config.LayerWard_Enemy);
        }

        RaycastHit hitInfo;
        bool isHit = Physics.Raycast(castRay, out hitInfo, ray.magnitude, LayerMaskOnBlocked);

        if (isHit)
        {
            return true;
        }
        return false;
    }

    #endregion


    #region 为每帧更新锁定框的代码    
    public Transform drawingTargetTransform = null;
    private void updateTargetFrame()
    {
        if (!machine)
        {
            return;
        }
        //无Player目标则移除
        if (!bDrawTarget || !targetObject || !targetObject.transform || !targetMachine || !machine.GetVisible())
        {
            if (drawingTargetTransform)
            {
                DisAttachLockFrame();

                //失去目标时打断自动攻击
                cmd_creature_break_look_attack data = new cmd_creature_break_look_attack();
                data.nBreak = 1;
                EntityEventHelper.Instance.SendCommand<cmd_creature_break_look_attack>(machine.entityID, EntityLogicDef.ENTITY_CMD_BREAK_LOOK_ATTACK, ref data);
                if (machine.showLog)
                {
                    Trace.Log("ENTITY_CMD_BREAK_LOOK_ATTACK " + machine.entityID + " nBreak = 1");
                }
                USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_WAR_FRONTSIGHT_TOGGLELOCKTARGET, new USpeedUI.UIMsgCmdData((int)USpeedUI.WndMsgID.WND_MSG_WAR_FRONTSIGHT_TOGGLELOCKTARGET, 0, string.Empty, IntPtr.Zero, 0));

            }
            return;
        }

        if ((targetObject) && (targetObject.transform != drawingTargetTransform) && (TargetFrame))
        {
            DisAttachLockFrame();
            TargetFrame frame = TargetFrame.GetComponent<TargetFrame>();
            if (frame)
            {
                frame.AttachToTarget(targetObject.transform);
                drawingTargetTransform = targetObject.transform;
                MouseCursorManager.Instance.setTraceSnap(true);
            }

            cmd_creature_Set_Target_Info data = new cmd_creature_Set_Target_Info();
            data.uidTarget = (uint)targetID;
            data.fX = targetPos.x;
            data.fY = targetPos.y;
            data.fZ = targetPos.z;
            if (machine.showLog)
            {
                Trace.Log("ENTITY_CMD_CAST_SPELL " + machine.entityID);
                Trace.Log("TargetID" + targetID + "TargetPos " + targetPos.ToString());
            }

            EntityEventHelper.Instance.SendCommand<cmd_creature_Set_Target_Info>(machine.entityID, EntityLogicDef.ENTITY_CMD_SET_TARGET_INFO, ref data);
            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_WAR_FRONTSIGHT_TOGGLELOCKTARGET, new USpeedUI.UIMsgCmdData((int)USpeedUI.WndMsgID.WND_MSG_WAR_FRONTSIGHT_TOGGLELOCKTARGET, 1, string.Empty, IntPtr.Zero, 0));
        }
    }

    /// 无玩家时移除锁定框
    void DisAttachLockFrame()
    {
        if (!drawingTargetTransform)
        {
            return;
        }
        TargetFrame frame = TargetFrame.GetComponent<TargetFrame>();
        if (frame)
        {
            frame.DisAttachToTarget();
        }
        drawingTargetTransform = null;
        MouseCursorManager.Instance.setTraceSnap(false);
    }
    #endregion

    public bool bDrawTarget = true;
    private CreatureProperty CurCP = null;
    private CreatureStateMachine machine = null;
    private bool m_bSkillLock = false;
    private bool m_bKeyQuoteLock = false;

    public void setSkillLock(bool enable)
    {
        if (enable)
        {
            m_bSkillLock = true;
            m_lastSkillLockTick = GameLogicAPI.getTickCount();
            m_lastMouseLockPos = InputManager.MousePosition;
        }
        else
        {
            m_bSkillLock = false;
            m_lastSkillLockTick = 0;
            m_lastMouseLockPos = Vector3.zero;
        }

    }

    /// <summary>
    /// 每帧更新函数，画准心对准后的锁定框
    /// </summary>
    private uint m_lastSkillLockTick = 0;
    private Vector3 m_lastMouseLockPos = Vector3.zero;
    public int SkillLockTick = 1000;
    public float MouseLockDistance = 100.0f;
    public void Update()
    {

        if (UISystem.Instance.IsMouseOverUI())
        {
            //鼠标在UI上时，取消选中框，暂停选中功能
            if (targetID > 0)
            {
                zeroTarget();
                //更新锁定框
                updateTargetFrame();
            }
            if (rootTrans != null&&machine.SkinControl.CurrentSkinInstance != null)
            {
                rayCast();
            }
            return;
        }



        if (Time.time - lastTime < 1.0f) return; //创建1秒内不更新，因为视口创建未完成时，射线提取会报错
        bool lockKey = false;
        uint tick = GameLogicAPI.getTickCount();
        //`键开关选取，鼠标左键按住不放选取
        //m_bMouseLeftLock=InputManager.GetKey(KeyCode.Mouse0);

        //if (InputManager.GetKeyDown(KeyCode.Mouse0))
        //{
        //    m_lastMouseLockDownTick = tick;
        //}
        //else if (InputManager.GetKeyUp(KeyCode.Mouse0))
        //{
        //    m_bMouseLeftLock = false;
        //    m_lastMouseLockDownTick = 0;
        //}
        ////鼠标左键按住1秒开始锁定
        //if (m_lastMouseLockDownTick > 0 && tick - m_lastMouseLockDownTick > MouseLockTick && InputManager.GetKey(KeyCode.Mouse0))
        //{
        //    m_bMouseLeftLock = true;
        //    m_lastMouseLockDownTick = 0;
        //}


        //技能锁定功能
        //技能按下后锁定目标
        //目标不合法时解除锁定
        //鼠标移动一段距离解除锁定
        //锁定后一秒解除锁定
        //2016-03-19，屏蔽技能选中锁定
        //m_bSkillLock = false;
        //if (m_bSkillLock)
        //{
        //    /*鼠标左键锁定方案
        //    if (!InputManager.GetKey(KeyCode.Mouse0))
        //    {
        //        if (m_lastSkillLockTick == 0)
        //        {
        //            m_lastSkillLockTick = tick;
        //        }
        //        else if (tick - m_lastSkillLockTick > SkillLockTick)
        //        {
        //            m_lastSkillLockTick = 0;
        //            m_bSkillLock = false;
        //            m_lastMouseLockPos = Vector3.zero;
        //        }
        //    }
        //    else
        //    {
        //        Vector3 mouseOffset = InputManager.MousePosition - m_lastMouseLockPos;
        //        if (mouseOffset.magnitude > MouseLockDistance)
        //        {
        //            m_lastSkillLockTick = 0;
        //            m_bSkillLock = false;
        //            m_lastMouseLockPos = Vector3.zero;
        //        }
        //    }
        //     */

        //    //技能锁定方案
        //    if (tick - m_lastSkillLockTick > SkillLockTick)
        //    {
        //        setSkillLock(false);
        //        //m_lastSkillLockTick = 0;
        //        //m_bSkillLock = false;
        //        //m_lastMouseLockPos = Vector3.zero;
        //    }
        //    else
        //    {
        //        Vector3 mouseOffset = InputManager.MousePosition - m_lastMouseLockPos;
        //        if (mouseOffset.magnitude > MouseLockDistance)
        //        {
        //            setSkillLock(false);
        //            //m_lastSkillLockTick = 0;
        //            //m_bSkillLock = false;
        //            //m_lastMouseLockPos = Vector3.zero;
        //        }
        //    }
        //}
        /*鼠标左键锁定方案
        else
        {
            if (InputManager.GetKey(KeyCode.Mouse0))
            {
                m_lastSkillLockTick = 0;
                m_bSkillLock = true;
                m_lastMouseLockPos = InputManager.MousePosition;
            }
        }
        */


        //鼠标左键

        if (InputManager.GetKeyUp(KeyCode.BackQuote))
        {
            m_bKeyQuoteLock = !m_bKeyQuoteLock;
        }

        lockKey = m_bSkillLock || m_bKeyQuoteLock;

        if (lockKey)
        {
            if (bLockTarget == false && targetID > 0 && isTargetValid(targetObject, targetMachine, m_selectDistance, targetEntityView))
            {
                bLockTarget = true;
            }
        }
        else
        {
            bLockTarget = false;
        }

        if (!bLockTarget)
        {
            m_bKeyQuoteLock = false;
            m_bSkillLock = false;
            //SoldierCamera.MainInstance<SoldierCamera>().setLockTarget(null);
        }
        else
        {
            //if (targetObject)
            //{
            //    SoldierCamera.MainInstance<SoldierCamera>().setLockTarget(targetObject.transform);
            //}
            //else
            //{
            //    SoldierCamera.MainInstance<SoldierCamera>().setLockTarget(null);
            //}
        }

        if (machine.SkinControl.CurrentSkinInstance != null)    //如果切换皮肤，新皮肤没加载完，就不做raycast
        {
            if (rootTrans == null)
            {
                if (machine != null)
                {
                    rootTrans = machine.SkinControl.CurrentSkinInstance.GetBonesTransform("Main");
                }
            }
            else
            {
                //射线检测
                rayCast();
            }
        }

        if (drawingDefaultSelectDrawShape && m_selectEffects != null)
        {
            m_selectEffects.updateEffect(targetPos);
        }
    }



    public void onDamage(U3D_Render.EntityView attacker)
    {
        if (attacker.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
        {
            this.attackHeroMachine = attacker.StateMachine;
        }

    }

    private Vector3 ProjectToFloor(Vector3 posBeforeProject)
    {
        Vector3 posAfterProject = posBeforeProject;
        //int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault)) | (1 << LayerMask.NameToLayer(Config.LayerBuilding));
        int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault));
        if (!m_bProjectToFloorSkipBuilding)
        {
            LayerMaskOnGround = LayerMaskOnGround | (1 << LayerMask.NameToLayer(Config.LayerBuilding));
        }


        Ray upRay = new Ray(posAfterProject + new Vector3(0, 0.5f, 0), new Vector3(0, 1, 0));
        RaycastHit hitInfo;
        //先向上取10米，并且确保不会撞到顶，如果是在石头内部的话，需要确保石头法向向外，这样就不会判定为hit到。
        bool hitUp = Physics.Raycast(upRay, out hitInfo, 10.0f, LayerMaskOnGround);
        if (hitUp)
        {
            posAfterProject = hitInfo.point;
        }
        else
        {
            posAfterProject = posAfterProject + new Vector3(0, 10.0f, 0);
        }

        Ray downRay = new Ray(posAfterProject - new Vector3(0, 0.1f, 0), new Vector3(0, -1, 0));
        bool hitGround = Physics.Raycast(downRay, out hitInfo, MAX_CAST_DISTANCE, LayerMaskOnGround);
        if (hitGround)
        {
            posAfterProject = hitInfo.point;
        }
        else
        {
            posAfterProject = posBeforeProject;
            posAfterProject.y = transform.position.y; //拿不到贴地坐标，就取原来点，但是y是用人物本身的y
        }
        return posAfterProject;
    }

}
