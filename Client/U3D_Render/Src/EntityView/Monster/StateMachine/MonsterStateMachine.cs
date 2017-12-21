/*******************************************************************
** 文件名:	MonsterStateMachine.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	怪物状态机
** 应  用:  	怪物状态机中的main部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameLogic;
using System;
using U3D_Render;


// 每新增一个成员，都要在ResetAllMembers()写一个初始值，
// 要不然缓存会有错哦
public sealed partial class MonsterStateMachine : BaseStateMachine
{
    [System.NonSerialized]
    public Vector3 syncPosition = Vector3.zero; //用于同步的速度，主角的话设置此向量上报给服务器，非主角的话读取此向量作为速度
    [System.NonSerialized]
    public Vector3 syncVelocity = Vector3.zero; // 同步下来的位置/模拟位置
    [System.NonSerialized]
    public Vector3 syncAngles = Vector3.zero;    // 同步下来的方向
    [System.NonSerialized]
    public float deltaerr = 0.0f;

    private Vector3 defaultAngle = Vector3.zero;

    [System.NonSerialized]
    public bool m_DistanceVisible = true;

    /// <summary>
    /// Y适配Mask，贴地面的层
    /// </summary>
    public static int Y_AdjustLayerMask = -1;

    /// <summary>
    /// 怪物当前的位置
    /// </summary>
    private Vector3 curMosnterPos = Vector3.zero;

    private Coroutine cor_CheckOcclus = null;
    public void SetPosition(Vector3 pos)
    {
        curMosnterPos = pos;
        creature.SetPosition(curMosnterPos);
    }
    public Vector3 GetPosition()
    {
        return curMosnterPos;
    }

    public void LookAtPoint(Vector3 pt)
    {
        creature.LookAt(pt);
    }

    public override void enableCollider(bool enable)
    {
        if (creaturePropety && creaturePropety.ScreenCastObject)
        {
            creaturePropety.ScreenCastObject.SetActive(enable);
        }
    }

    #region 移动信息调试

    //public bool ShowMoveTargetBall = false;

    //private GameObject MoveTargetBall = null;

    //public bool ShowAttackTargetBall = false;

    //private GameObject AttackTargetBall = null;

    //public void SetMoveTargetBall(Vector3 pos)
    //{
    //    if (!ShowMoveTargetBall)
    //    {
    //        return;
    //    }
    //    if (null == MoveTargetBall)
    //    {
    //        MoveTargetBall = GameObject.CreatePrimitive(PrimitiveType.Sphere);
    //        MoveTargetBall.transform.localScale = Vector3.one * 0.3f;
    //    }
    //    MoveTargetBall.SetActive(ShowMoveTargetBall);
    //    MoveTargetBall.transform.SetPosition( pos;
    //}

    //public void SetAttackTargetBall(Vector3 pos)
    //{
    //    if (!ShowAttackTargetBall)
    //    {
    //        return;
    //    }
    //    if (null == AttackTargetBall)
    //    {
    //        AttackTargetBall = GameObject.CreatePrimitive(PrimitiveType.Sphere);
    //        AttackTargetBall.transform.localScale = Vector3.one * 0.3f;
    //    }
    //    AttackTargetBall.SetActive(ShowAttackTargetBall);
    //    AttackTargetBall.transform.SetPosition( pos;
    //}

    #endregion


    /// <summary>
    /// 重置所有成员变量，恢复到实例化之前的状态
    /// </summary>
    public override void ResetAllMembers()
    {
        base.ResetAllMembers();

        //MonsterStateMachine.cs部分
        //层级检测不用清0
        //Y_AdjustLayerMask = 0;
        m_DistanceVisible = true;
        defaultAngle = Vector3.zero;
        curMosnterPos = Vector3.zero;
        goRootTransform = null;


        //MonsterStateMachine_ForceMove.cs部分
        bIsForceMoving = false;
        m_nForceMoveStartTick = 0;
        m_nForceMoveEndTick = 0;
        m_ForceMovePos = Vector3.zero;
        m_fForceMoveSpeed = Vector3.zero;
        m_fForceStartPos = Vector3.zero;

        m_bForceFlighting = false;
        m_fFlightV0 = 0.0f;
        m_fFlightTime = 0.0f;
        m_fFlightDownTime = 0.0f;
        m_fFlightVel = 0.0f;
        m_uResidenceStopTick = 0;
        m_flightStartPosition = Vector3.zero; //开始上升时候的位置记录，如果是连续浮空的话，会记最后一次浮空的开始位置
        m_flightEndPosition = Vector3.zero; //浮空的结束位置，如果是连续浮空的话，会记第一次浮空的开始位置
        m_uResidenceTick = 0;
        m_fFlightHeight = 0.0f;
        upAcc = 0.0f;
        downAcc = 0.0f;

        //MonsterStateMachine_Mask.cs部分
        bNotTurnWhenAttackMask = false;
        bFirstVisibleMask = false;

        //MonsterStateMachine_Skin.cs部分
        bFadeInSkin = false;
        bFadeOutSkin = false;
        FadeTimer = 0.0f;
        FadeDuation = 0.0f;
        occlusCamera = false;
        lastocclusCamera = false;
        startCheckOcclusCoroutine = false;
        if (null != cor_CheckOcclus)
        {
            StopCoroutine(cor_CheckOcclus);
        }
        ResetAnimatorDataWhenSkinChangedFinish();
    }


    #region Unity消息
    private Transform goRootTransform;
#if USE_TIMMER_BEHAVIOUR
    private bool isAddToTimer = false;
#endif
    public override void Init()
    {
#if USE_TIMMER_BEHAVIOUR
        if (!isAddToTimer)
        {
            startTimmer(2, UpdateBehaviourOnTimer.TIMER_THREAD_MID);
            isAddToTimer = true;
        }
        
#endif

        //animator = GetComponent<Animator>();
        creature = GetComponent<Transform>();
        creaturePropety = GetComponent<CreatureProperty>();
        //castLayer = GetComponent<ScreenCastLayer>();

        if (null == m_stateHandlers)
        {
            m_stateHandlers = new ICreatureState[(int)EntityState.MaxCount];
        }
        m_stateHandlers[(int)EntityState.Standing] = new MonsterState_Standing(this);
        m_stateHandlers[(int)EntityState.Dead] = new MonsterState_Dead(this);

        ChangeState((int)EntityState.Standing, IntPtr.Zero);

        //只和default做检测
        Y_AdjustLayerMask = 1 << LayerMask.NameToLayer(Config.LayerDefault) | 1 << LayerMask.NameToLayer(Config.LayerBuilding);
    }

    public override void Clear()
    {
        base.Clear();

        if (serverpossphere)
        {
            ResNode.DestroyRes(ref serverpossphere);
            serverpossphere = null;
        }
        foreach (ICreatureState handler in m_stateHandlers)
        {
            if (null != handler)
            {
                handler.ClearState();
            }
        }
        goRootTransform = null;
        m_stateHandlers = null;
        m_CurStateHandler = null;
        animator = null;
        creature = null;
        wound = null;
        creaturePropety = null;
        //castLayer = null;
    }

#if USE_TIMMER_BEHAVIOUR
    public override void OnUpdate()
#else
    public void Update()
#endif
    {
        base.update();

        EntityOptimize();

        // 当前状态更新
        if (m_CurStateHandler != null)
        {
            m_CurStateHandler.Update();
        }

        if (bPauseMachineUpdate)
        {
            return;
        }
        if (bIsForceMoving || m_bForceFlighting)
        {
            forceMoveToTarget();
        }

        //怪物没有带位移的攻击
        //updateAnimation();
    }

    //private int lastAttackNameHash = -1;
    //private bool bDoingAttackMove = false;
    //private bool animatorAttackMove = false;
    //private Vector3 attackMoveOrgPos;
    //private Vector3 posBeforeAttackMove;
    //private void updateAnimation()
    //{
    //    if (animator == null)
    //    {
    //        return;
    //    }
    //    bool stateChange = false;
    //    int attackHash = animator.GetCurrentAnimatorStateInfo(0).fullPathHash;
    //    if (attackHash != lastAttackNameHash)
    //    {
    //        stateChange = true;
    //    }

    //    lastAttackNameHash = attackHash;

    //    if (stateChange)
    //    {
    //        bool lastAnimatorAttackMove = animatorAttackMove;
    //        animatorAttackMove = animator.GetCurrentAnimatorStateInfo(0).IsTag("AttackMove");
    //        if (animatorAttackMove)
    //        {
    //            attackMoveOrgPos = transform.position;
    //        }
    //        //else if (lastAnimatorAttackMove)
    //        //{
    //        //    transform.SetPosition( posBeforeAttackMove;
    //        //}
    //        if (!animatorAttackMove || !bDoingAttackMove)
    //        {
    //            posBeforeAttackMove = transform.position;
    //        }
    //    }

    //    bDoingAttackMove = false;
    //    if (animatorAttackMove && goRootTransform != null)
    //    {

    //        AnimatorClipInfo[] info = animator.GetCurrentAnimatorClipInfo(0);
    //        if (info.Length > 0 && info[0].weight > 0.99)
    //        {
    //            //带有位移的攻击动作，把位移转移到controller上
    //            //但是这种操作只在攻击动作权重为1的时候有效，在攻击动作权重下降，与IDLE融合的时候，不这么做，否则与IDLE融合会倒退
    //            Vector3 animatorOffset = goRootTransform.localPosition;
    //            if (goRootTransform.parent != null)
    //            {
    //                animatorOffset = goRootTransform.parent.rotation * animatorOffset;
    //            }
    //            //Debug.Log("goBRootTransform=" + goBRootTransform.localPosition*1000.0f);
    //            Vector3 anbPos = attackMoveOrgPos + animatorOffset;
    //            Vector3 controllerPos = anbPos - (goRootTransform.transform.position - transform.position);
    //            transform.SetPosition( controllerPos;
    //            //Debug.Log("anb.transform.SetPosition(" + anb.transform.position*1000.0f);
    //            bDoingAttackMove = true;
    //        }
    //        goRootTransform.localPosition = Vector3.zero;
    //    }
    //}

    public override bool ChangeState(int id, IntPtr lParam)
    {
        if (m_CurStateHandler != null && m_CurStateHandler.GetID() == id) { return false; }

        ////对于怪物来说，进入死亡状态，就不可能在出来了。
        //if (null != m_CurStateHandler)
        //{
        //    if (m_CurStateHandler.GetID() == (int)EntityState.Dead)
        //    {
        //        return true;
        //    }
        //}

        if (id < 0 || id >= (int)EntityState.MaxCount)
            return false;
        if (m_stateHandlers[id].OnEnter(lParam) == false) { return false; }

        if (m_CurStateHandler != null)
        {
            m_CurStateHandler.OnLeave();
        }

        m_CurStateHandler = m_stateHandlers[id];
        return true;
    }

    public override EntityState GetState()
    {
        if (null == m_CurStateHandler)
        {
            return EntityState.Dead;
        }
        return (EntityState)m_CurStateHandler.GetID();
    }

    public override bool IsGrounded()
    {
        if (null == m_CurStateHandler)
        {
            return true;
        }
        return m_CurStateHandler.IsGrounded();
    }


    #endregion

    #region 辅助计算工具函数

    /// <summary>
    /// 计算距离，只考虑XZ平面
    /// </summary>
    /// <param name="pos1"></param>
    /// <param name="pos"></param>
    /// <returns></returns>
    public static float CaclDistance(Vector3 pos1, Vector3 pos)
    {
        Vector3 pos11 = pos1;
        Vector3 pos22 = pos;
        pos11.y = 0.0f;
        pos22.y = 0.0f;
        float dis = Vector3.Distance(pos11, pos22);
        return dis;
    }

    /// <summary>
    /// 修正看向的点，主要是考虑X轴方向旋转的问题。
    /// </summary>
    /// <param name="targetPos"></param>
    /// <returns></returns>
    public Vector3 CorrectLookAtPoint(Vector3 targetPos)
    {
        Vector3 resultPoint = targetPos;

        float srcheight = creature.position.y;
        float dstheight = targetPos.y;

        float disheight = srcheight - dstheight;
        disheight = Mathf.Abs(disheight);

        ////如果自己的点和目标点的高度差超过自己高度 *4，就不能忽略Y了
        //if (disheight > creaturePropety.CreatureHeightInMeters * 4)
        //{
        //    //减去自己的高度
        //    resultPoint.y = creaturePropety.CreatureHeightInMeters + targetPos.y;
        //}
        //else
        {
            resultPoint.y = creature.position.y;
        }
        return resultPoint;

    }


    /// <summary>
    /// 通过物理纠正位置，俗称贴地面，检测区间为需要检测点的Y坐标为中心，UpCheckDistance +  DownGroundCheckDistance;
    /// </summary>
    /// <param name="Pos">需要检测的点</param>
    /// <param name="UpCheckDistance">上方向距离,如无特俗情况，使用实体的高度最佳</param>
    /// <param name="DownGroundCheckDistance">下方向距离,默认5.0</param>
    /// <returns></returns>
    public static Vector3 CorrectPosFromPhysic(Vector3 Pos, float UpCheckDistance, float DownGroundCheckDistance = 5.0f)
    {
        Vector3 res = Pos;
        RaycastHit hitInfo;

        //其实位置
        Vector3 startPos = Pos;
        startPos.y += UpCheckDistance;

        //结束位置
        Vector3 endPos = Pos;
        endPos.y -= DownGroundCheckDistance;

        if (Application.isEditor)
        {
            Debug.DrawLine(startPos, endPos, Color.white);
        }

        //如果检测到碰撞体，则设置为碰撞体的Y值，使得怪物贴地面
        if (Physics.Linecast(startPos, endPos, out hitInfo, Y_AdjustLayerMask))
        {
            res.y = hitInfo.point.y;
        }

        return res;
    }

    public bool ContainState(Animator animator, string name)
    {
        for (int i = 0; i < animator.layerCount; i++)
        {
            if (animator.HasState(i, Animator.StringToHash(name)))
            {
                return true;
            }
        }
        return false;
    }

    #endregion

    #region 实体优化

    /// <summary>
    /// 实体优化
    /// </summary>
    private void EntityOptimize()
    {
        if (!SkinConfig || SoldierCamera.MainInstance() ==null || Initialize.mainCam == null || Initialize.Instance.SoliderTargetGO == null)
        {
            return;
        }

        float dragonOptimitzedDistance = 7.0f;  //策划控制
        U3D_Render.EntityView mainEv = EntityFactory.getMainPlayer();
        if(null == mainEv)
        {
            return;
        }
        //大小龙体积过大，透明度优化
        int vocationID = creaturePropety.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
        if (vocationID > 11 && vocationID < 15)    
        {
            //检测距离，如果距离太近，不降低透明度
            float dis = CaclDistance(mainEv.gameObject.transform.position, creature.position);
            if (dragonOptimitzedDistance > dis)
            {
                SkinControl.setAlpha(0.5f);
            }
            else
                SkinControl.setAlpha(1.0f);
        }

        if (SkinConfig.OptimitzedDistance > 0)
        {
            //检测距离，如果距离太远，不渲染了
            //参考点分两种情况：上帝模式、非上帝模式


            Vector3 calcPos = new Vector3();
           if (SoldierCamera.MainInstance().cameraMode == CameraMode.MapGodControl || SoldierCamera.MainInstance().cameraMode == CameraMode.OBMoveFree)
                calcPos = Initialize.mainCam.transform.position;
            else
                calcPos = Initialize.Instance.SoliderTargetGO.transform.position;

#if ENABLE_YUNYING_MODE
           if (SoldierCamera.MainInstance().cameraMode == CameraMode.YUNYING_OBMoveFree)
           {
               calcPos = Initialize.mainCam.transform.position;
           }
           return;
#endif
           calcPos.y = creature.position.y;

            float dis = CaclDistance(calcPos, creature.position);
            if (dis < SkinConfig.OptimitzedDistance)
            {
                if (!m_DistanceVisible)
                {
                    m_DistanceVisible = true;
                    //淡入的时候是不可见的，所以要立马更新可见性
                    UpdateVisible();

                    //淡入皮肤
                    FadeInSkin();
                }
                //位置往前移2m，再判断
                Vector3 p = creature.position + Initialize.mainCam.transform.forward * 2.0f;
                bool isOnFrustmView = CameraUtil.isOnCameraView(p);
                if (isOnFrustmView)
                {
                    GameUtil.EnabledAnimator(animator);
                }
                else
                {
                    GameUtil.DisabledAnimator(animator);
                }
            }
            else
            {
                if (m_DistanceVisible)
                {
                    m_DistanceVisible = false;
                    //淡出皮肤
                    FadeOutSkin();
                }
                GameUtil.DisabledAnimator(animator);
            }
            UpdateFadeSkin();
        }
        //如果没有开启优化，走老套路
        else
        {
            if (!m_DistanceVisible)
            {
                m_DistanceVisible = true;
                UpdateVisible();
                SkinControl.setAlpha(1.0f);
                GameUtil.EnabledAnimator(animator);
            }
        }
    }

    #endregion

    //飞檐走壁
    public override bool startWallWalk(IntPtr lParam)
    {
        return true;
    }
}
