using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GameLogic;
/// <summary>
/// 光效行为
/// </summary>
public sealed class LightingEffectBehavior_Fly : LightingEffectBehavior
{
    private LightingEffect_Fly flyConfig = null;
    private int[] effectIDWhenHitPlayerIDs;
    private int[] effectIDWhenHitMonsterIDs;
    private int[] effectIDWhenHitGroundIDs;
    private int[] effectIDWhenReachedIDs;

    private uint mStartTick;
    private uint mStartMoveTick; //开始移动的时间
    private uint mEndTick;
    private Vector3 mTargetPos = Vector3.zero;
    private Vector3 mStartPos = Vector3.zero;
    private Vector3 mLastTargetPos = Vector3.zero; //记录最近一个更新飞行方向的目标位置，update过程中如果lastTargetPos和TargetPos不同再重新算飞行方向
    private bool mReturning = false; //记录是否正在返回，如果有返回效果而且已经是返回状态的话，到目标点就删掉

    private bool m_bHitPlayer = false;
    private bool m_bHitGround = false;
    private bool m_bHitMonster = false;
    private Sound soundInstance = null;
    private bool m_bDetectHit = false; //是否需要判断击中，当前面effectID不为空时，此值为true
    private bool bCallCreateEffectFunc = false;

    private int currTargetEntityID = 0;//当前的目标ID，因为有些技能是会返回的，比如机器人的手，这些是会改变id的
    private FlyType oldFlyType = FlyType.TargetObject;

    public override void FillData(LightingEffect effectConfig, LightEffectParam param, LightingEffectResHandleInstance instance, Transform transformAgent)
    {
        base.FillData(effectConfig, param, instance, transformAgent);
        this.flyConfig = effectConfig as LightingEffect_Fly;
        if (!flyConfig && effectConfig)
        {
            Debug.LogWarning(effectConfig.nID + "配置不匹配,创建光效失败---" + GetType().ToString());
        }
    }

    public override bool Begin()
    {
        if (!flyConfig || null == EffectParam)
        {
            return false;
        }
        base.Begin();
        if (EffectParam.speed <= 0.0001f)
        {
            EffectParam.speed = 1.0f;
        }

        uint tick = GameLogicAPI.getTickCount();
        mStartTick = tick + (uint)((float)flyConfig.effectStartTick / EffectParam.speed);
        mEndTick = mStartTick + (uint)((float)flyConfig.effectLiveTick / EffectParam.speed);
        m_delayingDestroy = false;
        m_delayingHidingObj = false;
        m_upcastAngle = UnityEngine.Random.Range(flyConfig.minUpcastAngle, flyConfig.maxUpcastAngle);
        oldFlyType = flyConfig.flyType;

        if ((effectParam.nEffectFlag & (int)LightEffectParam.EFFECT_FLAG.FlyToTargetPos) != 0)
        {
            flyConfig.flyType = FlyType.TargetPosition;
        }
        currTargetEntityID = EffectParam.nTargetEntityID;
        if (isNeedToChangeFlyTypeToTargetPosition(flyConfig, currTargetEntityID))
        {
            flyConfig.flyType = FlyType.TargetPosition;
            TargetPositionWhenEntityInvalid = EffectParam.targetPos;
        }
        return true;
    }

    private Collider m_hitCollider; //与人或者与怪碰撞的时候，记录碰撞体，以便targetObject移动的时候修正碰撞点
    private bool createEffect()
    {
        bCallCreateEffectFunc = true;
        if (EffectParam == null)
        {
            return false;
        }
        if (EffectParam.speed <= 0.0001f)
        {
            EffectParam.speed = 1.0f;
        }
        //behaviorDestroyTimer = LightingEffectFactory.Instance.GetBehaviorDestroyTimer();

        mReturning = false;

        //if (flyConfig.effectLiveTick >= 0)
        //{
        //    behaviorDestroyTimer.destroyTick = GameLogicAPI.getTickCount() + (uint)(((float)flyConfig.effectLiveTick + (float)flyConfig.effectDispearTick) / EffectParam.speed + 500.0f); //比这个类延迟500毫秒，这个类没销毁的再自我销毁。
        //}
        //else
        //{
        //    behaviorDestroyTimer.destroyTick = 0; //0的时候表示无限循环
        //}
        //behaviorDestroyTimer.hostEffect = this;


        if (flyConfig.destroyWhenTargetDead && EffectParam.targetObject)
        {
            BaseStateMachine targetState = EffectParam.targetObject.GetComponent<BaseStateMachine>();
            if (targetState)
            {
                targetState.deadEvent += onTargetDead;
            }
        }

        initTransform();

        int effectNodeMask = 0;
        if (EffectParam.nSrcEntityID == EntityFactory.MainHeroID)
        {
            effectNodeMask |= (int)Effect.EffectNodeMask.Mask_EnableLihgt;
        }

        ResHandlePlayData.parent = behaviorTransformAgent;
        ResHandlePlayData.broadcastfuncName = "renewActiveForDelayDestroy";
        ResHandlePlayData.isEnemy = EffectParam.nCreatorSide == 0;
        ResHandlePlayData.entityID = effectParam.nTargetEntityID;
        ResHandlePlayData.SrcTransform = sorceTransform;
        ResHandlePlayData.DstTransform = targetTransform;
        ResHandlePlayData.bPlay = true;
        ResHandlePlayData.playMask = effectNodeMask;
        if (flyConfig.scale != 1.0f)
        {
            ResHandlePlayData.SizeScale = flyConfig.scale;
        }

        if (flyConfig.bUseSpeedScale)
        {
            ResHandlePlayData.Speed = EffectParam.speed;
        }

        soundInstance = SoundManager.CreateSound(behaviorTransformAgent.gameObject, flyConfig.SoundID);

        if (flyConfig.useCollision && EffectParam.bIsHero && EffectParam.nFeedbackID != 0)
        {
            
            attachCollision();
        }

        //射线检测，是否碰撞到地面或者人
        m_bHitEffectCreated = false;
        m_bDetectHit = false;

        if (flyConfig.effectIDWhenHitGround != "" || flyConfig.effectIDWhenHitPlayer != "" || flyConfig.effectIDWhenHitMonster != "")
        {
            m_bDetectHit = true;
        }

        if (m_bDetectHit && flyConfig.triggerHitDetect)
        {
            createHitEventMask();
            ResHandlePlayData.AddhitEvent += onHitEvent;
            if (flyConfig.effectIDWhenHitPlayer != "") { effectIDWhenHitPlayerIDs = splitIDs(flyConfig.effectIDWhenHitPlayer); }
            if (flyConfig.effectIDWhenHitMonster != "") { effectIDWhenHitMonsterIDs = splitIDs(flyConfig.effectIDWhenHitMonster); }
            if (flyConfig.effectIDWhenHitGround != "") { effectIDWhenHitGroundIDs = splitIDs(flyConfig.effectIDWhenHitGround); }
        }

        if (m_bDetectHit && !flyConfig.triggerHitDetect)
        {
            createHitEventMask();
            Vector3 rayTargetPos = mTargetPos;
            Vector3 raySourcePos = behaviorTransformAgent.position;
            Vector3 rayDir = rayTargetPos - raySourcePos;
            float rayDistance = rayDir.magnitude;
            rayDir.Normalize();
            Ray flyRay = new Ray(raySourcePos, rayDir);
            RaycastHit _ClosestHitInfo;
            if (flyConfig.effectIDWhenHitPlayer != "" || flyConfig.effectIDWhenHitMonster != "")
            {
                bool isHit = Physics.Raycast(flyRay, out _ClosestHitInfo, rayDistance, LightingEffectFactory.Instance.LayerMaskOnPlayerOrMonster);
                if (isHit)
                {
                    //碰撞到人或者怪需要播光效，但这里排除掉碰撞到出发者自己，只是GetComponentInParent会耗GC，不知道如何解决
                    BaseStateMachine t_targetMachine = _ClosestHitInfo.transform.GetComponentInParent<BaseStateMachine>();
                    if (t_targetMachine != null)
                    {
                        if (t_targetMachine.entityID == EffectParam.nSrcEntityID)
                        {
                            raySourcePos = _ClosestHitInfo.point;
                            rayDir = rayTargetPos - raySourcePos;
                            rayDistance = rayDir.magnitude;
                            rayDir.Normalize();
                            flyRay = new Ray(raySourcePos, rayDir);
                        }
                    }
                }
            }

            m_bHitGround = false;
            m_bHitPlayer = false;
            m_bHitMonster = false;
            m_hitCollider = null;

            if (flyConfig.effectIDWhenHitGround != "")
            {
                //与地面做射线检测
                bool isHit = Physics.Raycast(flyRay, out _ClosestHitInfo, rayDistance * 1.1f, m_hitEventMaskOnGround);
                if (isHit)
                {
                    rayTargetPos = _ClosestHitInfo.point;
                    mTargetPos = rayTargetPos;
                    m_bHitGround = true;
                    effectIDWhenHitGroundIDs = splitIDs(flyConfig.effectIDWhenHitGround);
                    rayDir = rayTargetPos - raySourcePos;
                    rayDistance = rayDir.magnitude;
                }
            }
            if (flyConfig.effectIDWhenHitPlayer != "")
            {
                if (targetTransform) //有绑定的目标点，穿过非指定的物件，优先碰撞指定的怪物/英雄
                {

                    Collider[] cols = null; ;
                    U3D_Render.EntityView view = EntityFactory.getEntityViewByID(EffectParam.nTargetEntityID);
                    if (view != null && view.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
                    {
                        CreatureStateMachine playerMachine = (CreatureStateMachine)view.StateMachine;
                        if (playerMachine != null && playerMachine.creaturePropety != null && playerMachine.creaturePropety.ScreenCastObject != null)
                        {
                            cols = playerMachine.creaturePropety.GetComponents<Collider>();
                        }

                        if ((cols == null || (cols != null && cols.Length <= 0)) && playerMachine != null && playerMachine.controller != null)
                        {
                            cols = playerMachine.controller.GetComponents<Collider>();
                        }
                    }

                    if (cols != null)
                    {
                        foreach (Collider collider in cols)
                        {
                            if (collider != null)
                            {
                                m_hitCollider = collider;
                                bool isHit = collider.Raycast(flyRay, out _ClosestHitInfo, rayDistance);
                                if (isHit)
                                {
                                    rayTargetPos = _ClosestHitInfo.point;
                                    mTargetPos = rayTargetPos;
                                    m_bHitPlayer = true;
                                    effectIDWhenHitPlayerIDs = splitIDs(flyConfig.effectIDWhenHitPlayer);
                                    rayDir = rayTargetPos - raySourcePos;
                                    rayDistance = rayDir.magnitude;
                                    break;
                                }
                            }
                        }
                    }


                }
                else
                {
                    //与人做射线检测
                    bool isHit = Physics.Raycast(flyRay, out _ClosestHitInfo, rayDistance, m_hitEventMaskOnPlayer);
                    if (isHit)
                    {
                        rayTargetPos = _ClosestHitInfo.point;
                        mTargetPos = rayTargetPos;
                        m_bHitPlayer = true;
                        effectIDWhenHitPlayerIDs = splitIDs(flyConfig.effectIDWhenHitPlayer);
                        rayDir = rayTargetPos - raySourcePos;
                        rayDistance = rayDir.magnitude;
                    }
                }
            }
            if (flyConfig.effectIDWhenHitMonster != "")
            {
                if (targetTransform) //有绑定的目标点，穿过非指定的物件，优先碰撞指定的怪物/英雄
                {
                    Collider[] cols = null;
                    U3D_Render.EntityView view = EntityFactory.getEntityViewByID(EffectParam.nTargetEntityID);
                    if (view != null && view.Type == ENTITY_TYPE.TYPE_MONSTER)
                    {
                        MonsterStateMachine monsterMachine = (MonsterStateMachine)view.StateMachine;
                        if (monsterMachine != null && monsterMachine.creaturePropety != null && monsterMachine.creaturePropety.ScreenCastObject != null)
                        {
                            cols = monsterMachine.creaturePropety.GetComponents<Collider>();
                        }

                        if (cols != null)
                        {
                            foreach (Collider collider in cols)
                            {
                                if (collider != null)
                                {
                                    m_hitCollider = collider;
                                    bool isHit = collider.Raycast(flyRay, out _ClosestHitInfo, rayDistance);
                                    if (isHit)
                                    {
                                        rayTargetPos = _ClosestHitInfo.point;
                                        mTargetPos = rayTargetPos;
                                        m_bHitMonster = true;
                                        effectIDWhenHitMonsterIDs = splitIDs(flyConfig.effectIDWhenHitMonster);
                                        rayDir = rayTargetPos - raySourcePos;
                                        rayDistance = rayDir.magnitude;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    //与怪物做射线检测
                    bool isHit = Physics.Raycast(flyRay, out _ClosestHitInfo, rayDistance, m_hitEventMaskOnMonster);
                    if (isHit)
                    {
                        rayTargetPos = _ClosestHitInfo.point;
                        mTargetPos = rayTargetPos;
                        m_bHitMonster = true;
                        effectIDWhenHitMonsterIDs = splitIDs(flyConfig.effectIDWhenHitMonster);
                        rayDir = rayTargetPos - raySourcePos;
                        rayDistance = rayDir.magnitude;
                    }
                }
            }
        }

        if (flyConfig.effectIDWhenReached != "")
        {
            effectIDWhenReachedIDs = splitIDs(flyConfig.effectIDWhenReached);
        }

        mStartMoveTick = (uint)((float)flyConfig.residenceTick / EffectParam.speed) + (uint)GameLogicAPI.getTickCount();

        return true;
    }

    public override void extendEffectLift()
    {
        mEndTick += (uint)flyConfig.effectLiveTick;
    }

    public override void recoverEffectLift(LightEffectParam newParam)
    {
        effectParam = newParam;

        //出发位置为已经到达的位置
        //重新计算targetpos

        if (((flyConfig.flyType == FlyType.TargetPosition) || ((flyConfig.flyType == FlyType.TargetObject || flyConfig.flyType == FlyType.TargetPositionToTargetObject) && (EffectParam.targetObject == null))) && (EffectParam.targetPos.magnitude > 0.0f))
        {
            mTargetPos = EffectParam.targetPos;
            if (showLog)
            {
                Trace.Log("Effect " + behaviorTransformAgent.name + " fly to pos " + mTargetPos.ToString());
            }
        }
        else if (EffectParam.targetObject != null && objTransform)
        {
            if (showLog)
            {
                Trace.Log("Effect " + behaviorTransformAgent.name + objTransform + " fly object " + EffectParam.targetObject.name);
            }
            targetTransform = FindTransformEx(EffectParam.nTargetEntityID, flyConfig.targetTransformBone);
            if (targetTransform == null)
            {
                targetTransform = EffectParam.targetObject.transform;
            }
            targetTransformPosition = Vector3.zero;
            if (targetTransform != null)
            {
                targetTransformPosition = targetTransform.position;
            }
            mTargetPos = targetTransformPosition;
        }
        m_targetPosBeforeRefine = mTargetPos;
        refineTargetPos();

        if (flyConfig.reviseFly)
        {
            objTransform.SetPosition(mTargetPos);
            switchSourceAndTarget();
        }

        faceToTarget();
    }

    public override void Update()
    {

        //base.Update();
        ////在update阶段取wound骨骼位置，不然的话lateUpdate经过混合会跑到头顶上，具体原因未明
        //targetTransformPosition = Vector3.zero;
        //if (targetTransform != null)
        //{
        //    targetTransformPosition = targetTransform.position;
        //    //if (this.nID == 291)
        //    //{
        //    //    Debug.Log("<color=orange>" + targetTransform.root.name + "/" + targetTransform.parent.name + "/" + targetTransform.name + " : " + " </color>" + targetTransform.position);
        //    //    Vector3 pos = GameObject.Find("Yegeruite-skin14_C").transform.FindChild("Main/RootCopy/Root_M/BackA_M/BackB_M/Chest_M/Wound").transform.position;
        //    //    Debug.Log("<color=orange>" + "wound pos : " + " </color>" + pos);
        //    //}
        //}


        if (!Valid)
        {
            return;
        }
        base.Update();
        if (targetTransform != null)
        {
             if ((flyConfig.flyType == FlyType.TargetObject || flyConfig.flyType == FlyType.TargetPositionToTargetObject))
             {
                 if (isNeedToChangeFlyTypeToTargetPosition(flyConfig, currTargetEntityID))
                 {
                     flyConfig.flyType = FlyType.TargetPosition;
                     effectParam.targetPos = TargetPositionWhenEntityInvalid;
                     mTargetPos = TargetPositionWhenEntityInvalid;
                     mLastTargetPos = TargetPositionWhenEntityInvalid;
                     return;
                 }

                 if(targetTransform.gameObject.activeInHierarchy)
                 {
                     TargetPositionWhenEntityInvalid = targetTransformPosition;
                     //在update阶段取wound骨骼位置，不然的话lateUpdate经过混合会跑到头顶上，具体原因未明
                     targetTransformPosition = targetTransform.position;
                 }

             }

            //if (this.nID == 291)
            //{
            //    Debug.Log("<color=orange>" + targetTransform.root.name + "/" + targetTransform.parent.name + "/" + targetTransform.name + " : " + " </color>" + targetTransform.position);
            //    Vector3 pos = GameObject.Find("Yegeruite-skin14_C").transform.FindChild("Main/RootCopy/Root_M/BackA_M/BackB_M/Chest_M/Wound").transform.position;
            //    Debug.Log("<color=orange>" + "wound pos : " + " </color>" + pos);
            //}
        }

    }

    public override bool LateUpdate()
    {
        if (!Valid)
        {
            return false;
        }
        uint tick = GameLogicAPI.getTickCount();

        if (m_delayingDestroy == true)
        {
            if (tick > mEndTick)
            {
                needClose = true;
                m_delayingDestroy = false;
                return false;
            }
            else
            {
                if ((flyConfig.attackToObject) && (flyConfig.flyType == FlyType.TargetObject || flyConfig.flyType == FlyType.TargetPositionToTargetObject))
                {
                    if (targetTransform != null)
                    {
                        behaviorTransformAgent.SetPosition(targetTransformPosition);
                    }
                }
                if (!m_delayingHidingObj && flyConfig.delayHideTick > 0 && tick > mEndTick - flyConfig.delayHideTick)
                {
                    ResHandlePlayData.broadcastfuncName = "disableActiveForDelayDestroy";
                    m_delayingHidingObj = true;
                }
                return true;
            }
        }

        if (needClose == true || (flyConfig.effectLiveTick > 0) && (tick > mEndTick))
        {
            return false;
        }

        if (tick < mStartTick)
        {
            return true;
        }

        if (!bCallCreateEffectFunc)
        {
            if (EffectParam.srcObject && Initialize.mainCam)
            {
                //距离超过创建距离，不创建光效了。
                //为了保证上帝视角和切换回英雄附近都能正确看到光效，这里同时考虑光效到英雄以及光效到摄像机的距离

                Vector3 p0 = EffectParam.srcObject.transform.position;
                Vector3 p1 = Initialize.mainCam.transform.position;
                Vector3 p2 = Vector3.zero;
                if (EntityFactory.MainHero)
                {
                    p2 = EntityFactory.MainHero.transform.position;
                }


                //不要Y
                p0.y = 0;
                p1.y = 0;
                p2.y = 0;

                Vector3 dis1 = p1 - p0;
                Vector3 dis2 = p2 - p0;
                float disSqr = LightingEffectFactory.CreateDistance * LightingEffectFactory.CreateDistance;

                if (EffectConfig.EnableDistanceOptimtized && (dis1.sqrMagnitude > disSqr) && (dis2.sqrMagnitude > disSqr))
                {
                    return false;
                }
            }
            return createEffect();
        }

        if (tick < mStartMoveTick)
        {
            if (flyConfig.posBindBone != null && flyConfig.posBindBone != "") //光效延时发出，一直绑在手上
            {
                initTransform();
            }
        }

        if (flyConfig.flySpeed > 0.0f && tick >= mStartMoveTick)
        {
            flyToTarget();
        }
        return true;
    }

    public override void Close()
    {

        if (!Valid)
        {
            return;
        }
        flyConfig.flyType = oldFlyType;
        if (flyConfig.closeReturn)
        {
            m_delayingDestroy = false; //延迟销毁取消
            if (mReturning)
            {
                Vector3 dis = mTargetPos - behaviorTransformAgent.position;
                if ((GameLogicAPI.getTickCount() < mEndTick) && (dis.sqrMagnitude > 0.01f)) //还没到达目的地，继续飞
                {
                    return;
                }
            }
            else
            {
                needClose = false;
                mEndTick = GameLogicAPI.getTickCount() + (uint)flyConfig.effectLiveTick;
                //if (behaviorDestroyTimer != null)
                //{
                //    behaviorDestroyTimer.destroyTick = GameLogicAPI.getTickCount() + (uint)flyConfig.effectLiveTick + 500;
                //}
                //光效返回
                switchSourceAndTarget();
                mReturning = true;
                return;
            }
        }
        mReturning = false; //开始删除了，mReturned清零

        if (flyConfig.playAnimWhenClose != "" && EffectParam.srcObject)
        {
            BaseStateMachine machine = EffectParam.srcObject.transform.GetComponent<BaseStateMachine>();
            if (machine != null)
            {
                machine.PlayAnim(flyConfig.playAnimWhenClose);
            }
        }
        //光效关闭的时候必须把enable设为true，要不然光效不会消失。这个的愿意是服务器的消息有慢有快
        setEnable(true);
        if (flyConfig.scale != 1.0f && flyConfig.scale != 0.0f)
        {
            ResHandlePlayData.SizeScale = 1 / flyConfig.scale;
        }

        if (flyConfig.useCollision && EffectParam.bIsHero && EffectParam.nFeedbackID != 0)
        {
            ResHandlePlayData.ReduceAttackCollisionHandle += onCollisionEvent;
        }

        if (m_bDetectHit && flyConfig.triggerHitDetect)
        {
            ResHandlePlayData.ReducehitEvent += onHitEvent;
        }

        if (flyConfig.destroyWhenTargetDead && EffectParam.targetObject)
        {
            BaseStateMachine targetState = GetStateMachine(EffectParam.nTargetEntityID);
            if (targetState)
            {
                targetState.deadEvent -= onTargetDead;
            }
        }

        //behaviorDestroyTimer.destroyTick = GameLogicAPI.getTickCount() + (uint)flyConfig.effectDispearTick;
        if (soundInstance && flyConfig.SoundID == soundInstance.ConfigID)
        {
            SoundManager.DeleteSound(ref soundInstance);
        }
        soundInstance = null;
        base.Close();
    }

    public override void setEnable(bool enable)
    {
        if(!Valid)
        {
            return;
        }
        if (!enable)
        {
            ResHandlePlayData.bPlay = false;
        }
        else
        {
            int effectNodeMask = 0;
            ResHandlePlayData.bPlay = true;
            ResHandlePlayData.isEnemy = EffectParam.nCreatorSide == 0;
            ResHandlePlayData.entityID = EffectParam.nSrcEntityID;
            ResHandlePlayData.SrcTransform = null;
            ResHandlePlayData.DstTransform = null;
            ResHandlePlayData.playMask = effectNodeMask;
        }
    }

    public override void ResetAllMembers()
    {
        bCallCreateEffectFunc = false;
        mStartTick = 0;
        mStartMoveTick = 0;
        mEndTick = 0;
        mTargetPos = Vector3.zero;
        mStartPos = Vector3.zero;
        mLastTargetPos = Vector3.zero;
        mReturning = false;
        m_bHitPlayer = false;
        m_bHitGround = false;
        m_bHitMonster = false;
        //behaviorDestroyTimer = null;
        m_targetPosBeforeRefine = Vector3.zero;
        m_bHitEffectCreated = false;
        m_upcastOffset = Vector3.zero;
        m_upcastAngle = 0.0f;
        m_delayingDestroy = false;
        m_delayingHidingObj = false;
        sorceTransform = null;
        targetTransform = null;
        targetTransformPosition = Vector3.zero;
        objTransform = null;
        lastHitTargetID = -1;
        m_TargetPositionWhenEntityInvalid = Vector3.zero;
        base.ResetAllMembers();
    }
    private void faceToTarget()
    {
        if (!flyConfig.faceTarget)
        {
            return;
        }
        Vector3 lastDis = mTargetPos - mLastTargetPos;
        if (lastDis.sqrMagnitude < 0.001f)
        {
            return;
        }

        mLastTargetPos = mTargetPos;

        Vector3 dir = mTargetPos - behaviorTransformAgent.position;

        if (flyConfig.faceSourceToTarget)
        {
            if (EffectParam.targetObject && EffectParam.srcObject)
            {
                dir = EffectParam.targetObject.transform.position - EffectParam.srcObject.transform.position;
            }
            else if (mReturning) //机器人锁链没有targetObject的时候，飞回来，方向保持不变
            {
                dir = Vector3.zero;
            }
        }

        if (dir.sqrMagnitude > 0.001f)
        {
            dir.Normalize();
            behaviorTransformAgent.forward = dir;
        }
    }

    Vector3 m_targetPosBeforeRefine = Vector3.zero;
    private bool m_bHitEffectCreated = false;

    private Vector3 m_upcastOffset; //做上抛运动时候的偏移量
    //private float upcastV0; //做上抛运动的V0
    private float m_upcastAngle = 0.0f; //做上抛运动偏转角度
    private bool m_delayingDestroy = false; //正在延迟删除阶段
    private bool m_delayingHidingObj = false; //在延迟删除阶段隐藏部分子部件
    private void flyToTarget()
    {
        if (EffectParam == null)
        {
            return;
        }

        if (flyConfig.flyType == FlyType.TargetObject || flyConfig.flyType == FlyType.TargetPositionToTargetObject)
        {
            if (targetTransform != null)
            {
                if (flyConfig.maxDistance > 0.01f && EffectParam.srcObject && EffectParam.targetObject && objTransform)
                {
                    Vector3 far = EffectParam.srcObject.transform.position - EffectParam.targetObject.transform.position;
                    if (far.magnitude > flyConfig.maxDistance)
                    {
                        //Vector3 objDis = objTransform.position - param.srcObject.transform.position;
                        //if (objDis.magnitude > maxDistance)
                        //{
                        //    objDis.Normalize();
                        //    objTransform.SetPosition( param.srcObject.transform.position + objDis * maxDistance;
                        //}
                        mTargetPos = targetTransformPosition;
                        Trace.Log("飞行光效超过距离关闭! EffectObject=" + objTransform.name + " maxDistance=" + flyConfig.maxDistance + " Far=" + far.magnitude + "srcObject " + EffectParam.srcObject.name + EffectParam.srcObject.transform.position + "target Object " + EffectParam.targetObject.name + EffectParam.targetObject.transform.position);
                        needClose = true;
                        if (flyConfig.effectReturn && !mReturning)
                        {
                            //光效返回
                            switchSourceAndTarget();
                            mReturning = true;
                        }
                        return;
                    }
                }

                Vector3 dis = m_targetPosBeforeRefine - targetTransformPosition;
                if (dis.sqrMagnitude > 0.01f)
                {
                    mTargetPos = targetTransformPosition;
                    m_targetPosBeforeRefine = mTargetPos;
                    refineTargetPos();
                    faceToTarget();
                }
            }
            else
            {
                if (mTargetPos.sqrMagnitude < 0.01f)
                {
                    needClose = true;
                    return;
                }
            }
        }

        if (!objTransform) //按照正常逻辑这里光效是不会被删的，以后再查原因，先挡了
        {
            needClose = true;
            return;
        }

        Vector3 currentPositios = objTransform.position - m_upcastOffset;

        float step = flyConfig.flySpeed * Time.deltaTime;

        if (mReturning && flyConfig.returnSpeed > 0.0f)
        {
            step = flyConfig.returnSpeed * Time.deltaTime;
        }
        Vector3 dir = mTargetPos - currentPositios;
        Vector3 srcDis = currentPositios - EffectParam.srcObject.transform.position;

        if (flyConfig.maxDistance > 0.01f && srcDis.magnitude > flyConfig.maxDistance && !mReturning)
        {
            Trace.Log("飞行光效超过距离关闭! EffectObject=" + objTransform.name + " maxDistance=" + flyConfig.maxDistance + " Far=" + srcDis.magnitude + "srcObject " + EffectParam.srcObject.name + EffectParam.srcObject.transform.position + "Effect Position:" + currentPositios);
            if (flyConfig.effectReturn && !mReturning)
            {
                //光效返回
                switchSourceAndTarget();
                mReturning = true;
                return;
            }
            else
            {
                needClose = true; //立即析构
                return;
            }
        }
        if (dir.magnitude < step)
        {
            objTransform.SetPosition(mTargetPos);

            if (flyConfig.effectReturn && !mReturning)
            {
                //光效返回
                switchSourceAndTarget();
                mReturning = true;
                return;
            }

            if (m_bHitEffectCreated == false && flyConfig.effectIDWhenHitMonster != "" && m_bHitMonster)
            {
                creatureHitEffect(effectIDWhenHitMonsterIDs);
            }
            if (m_bHitEffectCreated == false && flyConfig.effectIDWhenHitPlayer != "" && m_bHitPlayer)
            {
                creatureHitEffect(effectIDWhenHitPlayerIDs);
            }
            if (m_bHitEffectCreated == false && flyConfig.effectIDWhenHitGround != "" && m_bHitGround)
            {
                creatureHitEffect(effectIDWhenHitGroundIDs);
            }
            else if (m_bHitEffectCreated == false && flyConfig.effectIDWhenReached != "")
            {
                creatureHitEffect(effectIDWhenReachedIDs);
            }

            if (flyConfig.destroyWhenReached || mReturning)
            {
                needClose = true; //立即析构
            }
            else if (flyConfig.delayDestroyTick > 0) //延迟析构
            {
                m_delayingDestroy = true;
                m_delayingHidingObj = false;
                mEndTick = GameLogicAPI.getTickCount() + (uint)flyConfig.delayDestroyTick;
            }

            return;
        }
        dir.Normalize();

        //更新上抛运动的deltaY
        m_upcastOffset = Vector3.zero;
        if (flyConfig.upcastMotion)
        {
            float deltaY = 0.0f;
            float pathDis = (mTargetPos - mStartPos).magnitude;
            float targetDis = (mTargetPos - (currentPositios + dir * step)).magnitude;
            if (pathDis > 0.0f && targetDis < pathDis)
            {
                float scalePrec = 1.0f - targetDis / pathDis; //已经走过路径的百分比，把0.5拉伸到和upcastTopPrec对齐
                if (scalePrec < flyConfig.upcastTopPrec)
                {
                    if (flyConfig.upcastTopPrec > 0.0f)
                    {
                        scalePrec = scalePrec / flyConfig.upcastTopPrec * 0.5f;
                    }
                }
                else
                {
                    if (flyConfig.upcastTopPrec < 1.0f)
                    {
                        scalePrec = (scalePrec - flyConfig.upcastTopPrec) / (1.0f - flyConfig.upcastTopPrec) * 0.5f + 0.5f;
                    }
                }
                deltaY = flyConfig.upcastHeight * Mathf.Sin(scalePrec * Mathf.PI);  //用正弦曲线模拟抛物线
            }

            m_upcastOffset = new Vector3(0, deltaY, 0);
            if (Mathf.Abs(m_upcastAngle) > 0.01f)
            {
                Quaternion qua = Quaternion.AngleAxis(m_upcastAngle, dir);
                m_upcastOffset = qua * m_upcastOffset;
            }

        }

        Vector3 newPosition = currentPositios + dir * step + m_upcastOffset;

        if (flyConfig.upcastMotion)
        {
            objTransform.LookAt(newPosition);
        }

        objTransform.SetPosition(newPosition);
    }

    private int[] splitIDs(string strID)
    {
        string[] ids = strID.Split(';');
        int[] outIDs = new int[ids.Length];
        for (int i = 0; i < outIDs.Length; i++)
        {
            int effectID = 0;
            string strid = ids[i];
            bool br = int.TryParse(strid, out effectID);
            outIDs[i] = effectID;
        }
        return outIDs;
    }


    private void creatureHitEffect(int[] effectIDs)
    {
        foreach (int effectID in effectIDs)
        {
            if (effectID > 0)
            {
                LightingEffectManager leManager = EffectParam.srcObject.GetComponent<LightingEffectManager>();
                if (leManager != null)
                {
                    LightEffectParam newParam = new LightEffectParam();
                    EffectParam.ColoneTo(ref newParam);
                    newParam.targetPos = mTargetPos;
                    newParam.nEffectFlag |= (int)LightEffectParam.EFFECT_FLAG.BindInTargetPos;
                    leManager.AddLighting(effectID, newParam);
                    m_bHitEffectCreated = true;
                }
            }
        }
    }

    //记录位移和旋转所绑定的骨骼
    private Transform sorceTransform = null;
    private Transform targetTransform = null;
    private Transform objTransform = null;
    private Vector3 targetTransformPosition = Vector3.zero;

    /// <summary>
    /// 实体无效时的目标点
    /// </summary>
    private Vector3 TargetPositionWhenEntityInvalid
    {
        get
        {
            return m_TargetPositionWhenEntityInvalid;
        }

        set
        {
            if (value.sqrMagnitude < 0.001f || value.sqrMagnitude > 2430000)
            {
                return;
            }
            m_TargetPositionWhenEntityInvalid = value;
        }
    }
    private Vector3 m_TargetPositionWhenEntityInvalid = Vector3.zero;
    private void initTransform()
    {
        if (EntityFactory.EffectGroup != null)
        {
            behaviorTransformAgent.SetParent(EntityFactory.EffectGroup.transform);
        }


        if (!EffectParam.srcObject)//SrcObject，正常情况下不会为null，但是现在出现问题，挡掉。看了下，是在泉水的恢复光效有问题
        {
            needClose = true;
            return;
        }

        //将光效绑定到指定位置
        Vector3 pos = EffectParam.srcObject.transform.position;
        mLastTargetPos = Vector3.zero;

        pos = getStartPos();

        objTransform = behaviorTransformAgent;
        objTransform.SetPosition(pos);

        Transform rotateTransform = null;
        if (flyConfig.rotationBindBone != null && flyConfig.rotationBindBone != "" && EffectParam.srcObject)
        {
            rotateTransform = FindTransformEx(EffectParam.nSrcEntityID, flyConfig.rotationBindBone);
        }

        Quaternion rot = Quaternion.identity;
        if (EffectParam.srcObject)
        {
            rot = EffectParam.srcObject.transform.rotation;
        }
        if (rotateTransform != null)
        {
            rot = rotateTransform.rotation;
        }
        objTransform.rotation = rot;
        objTransform.Translate(flyConfig.posOffset);

        if (flyConfig.startPosOffset.sqrMagnitude > 0.0001f)
        {
            objTransform.Translate(flyConfig.startPosOffset);
        }


        //objTransform.transform.parent = param.srcObject.transform;

        if (((flyConfig.flyType == FlyType.TargetPosition) || ((flyConfig.flyType == FlyType.TargetObject || flyConfig.flyType == FlyType.TargetPositionToTargetObject) && (EffectParam.targetObject == null))) && (EffectParam.targetPos.magnitude > 0.0f))
        {
            mTargetPos = EffectParam.targetPos;

            if (showLog)
            {
                Trace.Log("Effect " + behaviorTransformAgent.name + " fly to pos " + mTargetPos.ToString());
            }
        }
        else if (EffectParam.targetObject != null)
        {
            if (showLog)
            {
                Trace.Log("Effect " + behaviorTransformAgent.name + objTransform + " fly object " + EffectParam.targetObject.name);
            }
            targetTransform = FindTransformEx(EffectParam.nTargetEntityID, flyConfig.targetTransformBone);
            if (targetTransform == null)
            {
                targetTransform = EffectParam.targetObject.transform;
                targetTransformPosition = targetTransform.position;
            }
            TargetPositionWhenEntityInvalid = targetTransform.position;
            if (targetTransformPosition.sqrMagnitude > 0.0001f)
            {
                mTargetPos = targetTransformPosition;
            }
        }
        m_targetPosBeforeRefine = mTargetPos;


        //if (upcastMotion && upcastHeight>0.01f)
        //{
        //    //预估飞行时间，用于计算上抛运动的v0
        //    float flyTime = (objTransform.position - mTargetPos).magnitude / flySpeed /2.0f;

        //    //在这段时间内速度要由v0变为0，所以v0=gt
        //    //高度由策划配置，上抛公式得 h=(v0^2)/(2*g)
        //    //解方程得
        //    upcastGravity = upcastHeight / (flyTime * flyTime) * 2.0f;
        //    upcastV0 = upcastGravity * flyTime;
        //}
        refineTargetPos();

        if (flyConfig.AbsFlyTime > 0.001f)
        {
            Vector3 pathVec = mTargetPos - objTransform.position;
            flyConfig.flySpeed = pathVec.magnitude / flyConfig.AbsFlyTime;
        }

        if (flyConfig.reviseFly)
        {
            objTransform.SetPosition(mTargetPos);
            switchSourceAndTarget();
        }

        faceToTarget();

        mStartPos = objTransform.position;
    }

    private Vector3 getStartPos()
    {
        Vector3 pos = Vector3.zero;
        //if (flyConfig.startType == StartPosType.SrcEntityBone)
        //{
        //    sorceTransform = EffectParam.srcObject.transform;
        //    pos = sorceTransform.position;
        //    if (flyConfig.posBindBone != null && flyConfig.posBindBone != "")
        //    {
        //        sorceTransform = FindTransformEx(EffectParam.nSrcEntityID, flyConfig.posBindBone);
        //        if (sorceTransform != null)
        //        {
        //            pos = sorceTransform.position;
        //        }
        //    }
        //}
        //else if (flyConfig.startType == StartPosType.DstEntityBone)
        //{
        //    sorceTransform = EffectParam.targetObject.transform;
        //    pos = sorceTransform.position;
        //    if (flyConfig.posBindBone != null && flyConfig.posBindBone != "")
        //    {
        //        sorceTransform = FindTransformEx(EffectParam.nTargetEntityID, flyConfig.posBindBone);
        //        if (sorceTransform != null)
        //        {
        //            pos = sorceTransform.position;
        //        }
        //    }
        //}
        //else if (flyConfig.startType == StartPosType.TargetPosition || flyConfig.flyType == FlyType.TargetPositionToTargetObject)
        //{
        //    pos = EffectParam.targetPos;
        //}
        //else if (flyConfig.startType == StartPosType.SourcePosition || 0 != (EffectParam.nEffectFlag & (int)LightEffectParam.EFFECT_FLAG.CustomSourcePos))
        //{
        //    pos = EffectParam.sourcePos;
        //}

        sorceTransform = EffectParam.srcObject.transform;
        pos = sorceTransform.position;
        if (flyConfig.posBindBone != null && flyConfig.posBindBone != "")
        {
            sorceTransform = FindTransformEx(EffectParam.nSrcEntityID, flyConfig.posBindBone);
            if (sorceTransform != null)
            {
                pos = sorceTransform.position;
            }
        }
        if (0 != (EffectParam.nEffectFlag & (int)LightEffectParam.EFFECT_FLAG.CustomSourcePos))
        {
            pos = EffectParam.sourcePos;
        }
        if (flyConfig.flyType == FlyType.TargetPositionToTargetObject)
        {
            pos = EffectParam.targetPos;
        }

        return pos;
    }
    private void switchSourceAndTarget()
    {
        if (EffectParam == null || !EffectParam.srcObject)
        {
            needClose = true;
            return;
        }

        //将光效绑定到指定位置
        Vector3 pos = EffectParam.srcObject.transform.position;
        if (flyConfig.posBindBone != null && flyConfig.posBindBone != "")
        {
            currTargetEntityID = EffectParam.nSrcEntityID;
            Transform srcTrans = FindTransformEx(EffectParam.nSrcEntityID, flyConfig.posBindBone);
            if (srcTrans != null)
            {
                pos = srcTrans.position;
                targetTransform = srcTrans;
                targetTransformPosition = Vector3.zero;
                if (targetTransform != null)
                {
                    targetTransformPosition = targetTransform.position;
                }
                TargetPositionWhenEntityInvalid = targetTransformPosition;
                flyConfig.flyType = FlyType.TargetObject;
            }
        }
        if (0 != (EffectParam.nEffectFlag & (int)LightEffectParam.EFFECT_FLAG.CustomSourcePos))
        {
            pos = EffectParam.sourcePos;
        }
        mTargetPos = pos;
    }

    private void attachCollision()
    {
        ResHandlePlayData.Attack_nFeedBackID = EffectParam.nFeedbackID;
        ResHandlePlayData.Attack_nAttackEntityID = EffectParam.nSrcEntityID;
        ResHandlePlayData.Attack_bEnableCollision = true;
        ResHandlePlayData.AddAttackCollisionHandle += onCollisionEvent;
        ResHandlePlayData.Attack_eventHandlerCount++;
        lastHitTargetID = -1;
    }

    private int lastHitTargetID = -1;
    void onCollisionEvent(AttackCollisionGroup sender, CollisionArgs arg)
    {
        GameObject m_object = sender.gameObject;
        if (arg.type == CollisionArgType.CollisionDestroy)
        {
            AttackCollisionGroup attackGroup = m_object.GetComponent<AttackCollisionGroup>();
            if (null != attackGroup)
            {
                attackGroup.collisionEvent -= onCollisionEvent;
                attackGroup.eventHandlerCount--;
            }
            return;
        }

        if (arg.type == CollisionArgType.CollisionDetected)
        {

            if (lastHitTargetID != arg.nAttackedEntityID) //不反复发同一个
            {
                cmd_creature_hitNow data = new cmd_creature_hitNow();
                data.nSerialID = arg.nFeedBackID;
                data.nHitTargetID = arg.nAttackedEntityID;
                data.fCollisionPos_x = arg.collisionPos.x;
                data.fCollisionPos_y = arg.collisionPos.y;
                data.fCollisionPos_z = arg.collisionPos.z;


                IntPtrHelper param1 = new IntPtrHelper();
                System.IntPtr ptr = param1.toPtr<cmd_creature_hitNow>(ref data);
                int len = Marshal.SizeOf(data);


                // 发出命令就可
                GameLogicAPI.onEntityEvent(arg.nAttackEntityID, (int)EntityLogicDef.ENTITY_CMD_HIT_NOW, len, "", ptr, len);

                if (showLog)
                {
                    Trace.Log("Hit! FeedBackID=" + data.nSerialID + ", HitTargetID" + data.nHitTargetID + ", AttackEntityID" + arg.nAttackEntityID);
                }

                lastHitTargetID = arg.nAttackedEntityID;
            }


            if (flyConfig.destroyWhenCollision == true)
            {
                AttackCollisionGroup attackGroup = m_object.GetComponent<AttackCollisionGroup>();
                if (null != attackGroup)
                {
                    attackGroup.bEnableCollision = false;
                }
                needClose = true;
            }
        }
    }


    private int m_hitEventMaskOnPlayer;
    private int m_hitEventMaskOnMonster;
    private int m_hitEventMaskOnGround;
    private void createHitEventMask()
    {
        m_hitEventMaskOnPlayer = 0;
        m_hitEventMaskOnMonster = 0;
        m_hitEventMaskOnGround = 0;

        U3D_Render.EntityView view = EntityFactory.getEntityViewByID(EffectParam.nSrcEntityID);
        if (view == null)
        {
            return;
        }

        if (view.CampFlag == SPELL.CampFlag.CampFlag_Friend || view.CampFlag == SPELL.CampFlag.CampFlag_Self)
        {
            if (flyConfig.effectIDWhenHitPlayer != "")
            {
                m_hitEventMaskOnPlayer |= (1 << (LayerMask.NameToLayer(Config.LayerPlayerEnemy)));
            }
            if (flyConfig.effectIDWhenHitMonster != "")
            {
                m_hitEventMaskOnMonster |= (1 << (LayerMask.NameToLayer(Config.LayerMonsterEnemy)));
                m_hitEventMaskOnMonster |= (1 << (LayerMask.NameToLayer(Config.LayerNeutrality)));
            }
        }
        else
        {
            if (flyConfig.effectIDWhenHitPlayer != "")
            {
                m_hitEventMaskOnPlayer |= (1 << (LayerMask.NameToLayer(Config.LayerPlayerFriend)));
                m_hitEventMaskOnPlayer |= (1 << (LayerMask.NameToLayer(Config.LayerHero)));
            }
            if (flyConfig.effectIDWhenHitMonster != "")
            {
                m_hitEventMaskOnMonster |= (1 << (LayerMask.NameToLayer(Config.LayerMonsterFriend)));
                m_hitEventMaskOnMonster |= (1 << (LayerMask.NameToLayer(Config.LayerNeutrality)));
            }
        }

        if (flyConfig.effectIDWhenHitGround != "")
        {
            m_hitEventMaskOnGround |= (1 << (LayerMask.NameToLayer(Config.LayerDefault)));
            m_hitEventMaskOnGround |= (1 << (LayerMask.NameToLayer(Config.LayerBuilding)));
        }
    }

    void onHitEvent(Collider collider)
    {
        if (!m_bDetectHit || !flyConfig.triggerHitDetect)
        {
            return;
        }
        int layer = collider.transform.gameObject.layer;
        int layerMask = 1 << layer;
        if (effectIDWhenHitPlayerIDs != null && effectIDWhenHitPlayerIDs.Length > 0)
        {
            if (0 != (layerMask & m_hitEventMaskOnPlayer))
            {
                m_bHitPlayer = true;
                Vector3 currentPositios = objTransform.position - m_upcastOffset;
                mTargetPos = currentPositios;
                return;
            }
        }
        if (effectIDWhenHitMonsterIDs != null && effectIDWhenHitMonsterIDs.Length > 0)
        {
            if (0 != (layerMask & m_hitEventMaskOnMonster))
            {
                m_bHitMonster = true;
                Vector3 currentPositios = objTransform.position - m_upcastOffset;
                mTargetPos = currentPositios;
                return;
            }
        }
        if (effectIDWhenHitGroundIDs != null && effectIDWhenHitGroundIDs.Length > 0)
        {
            if (0 != (layerMask & m_hitEventMaskOnGround))
            {
                m_bHitGround = true;
                Vector3 currentPositios = objTransform.position - m_upcastOffset;
                mTargetPos = currentPositios;
                return;
            }
        }
    }

    void onTargetDead(int entityID)
    {
        mEndTick = GameLogicAPI.getTickCount();
    }


    private void refineTargetPos()
    {
        //对目标点进行修正，针对飞向targetObject并且中途撞人/怪的，重新计算打击点
        if (targetTransform && (m_bHitPlayer || m_bHitMonster) && m_hitCollider != null)
        {
            Vector3 raySourcePos = behaviorTransformAgent.position;
            Vector3 rayTargetPos = mTargetPos;
            Vector3 rayDir = rayTargetPos - raySourcePos;
            float rayDistance = rayDir.magnitude;
            rayDir.Normalize();
            Ray flyRay = new Ray(raySourcePos, rayDir);
            RaycastHit _ClosestHitInfo;
            bool isHit = m_hitCollider.Raycast(flyRay, out _ClosestHitInfo, rayDistance);
            if (isHit)
            {
                mTargetPos = _ClosestHitInfo.point;
            }
        }


        //对目标点进行修正，如果低于光效设定的最低离地高度，则往上拉一下,如果高于光效设定的最低离地高度，则往下拉一下。
        if (flyConfig.lowestHeight > 0.0f || flyConfig.highestHeight > 0.0f)
        {
            RaycastHit _ClosestHitInfo;
            float _Distance = flyConfig.lowestHeight + 10.0f;
            Ray PointToGroundRay = new Ray(mTargetPos + new Vector3(0, 1.0f, 0), new Vector3(0, -1, 0));

            int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault));
            bool hit = Physics.Raycast(PointToGroundRay, out _ClosestHitInfo, _Distance, LayerMaskOnGround);
            if (!hit)
            {
                //向下没拾取到，可能已经在地底下，向上拾取试试
                PointToGroundRay = new Ray(mTargetPos, new Vector3(0, 1, 0));
                hit = Physics.Raycast(PointToGroundRay, out _ClosestHitInfo, _Distance, LayerMaskOnGround);
            }
            Vector3 hitPoint = _ClosestHitInfo.point;
            if (hit)
            {

                if (showLog)
                {
                    Trace.Log("TargetPos=" + mTargetPos.ToString() + "hitPoint=" + hitPoint.ToString());
                }

                if (flyConfig.lowestHeight >= 0.0f && mTargetPos.y < flyConfig.lowestHeight + hitPoint.y)
                {
                    mTargetPos.y = flyConfig.lowestHeight + hitPoint.y;
                }

                if (flyConfig.highestHeight >= 0.0f && mTargetPos.y > flyConfig.highestHeight + hitPoint.y)
                {
                    mTargetPos.y = flyConfig.highestHeight + hitPoint.y;
                }
            }

        }

        mTargetPos += flyConfig.targetOffset;
    }


    /// <summary>
    /// 当实体已经无效的时候，不能继续往实体飞了，要改为飞向目标点
    /// </summary>
    /// <param name="config"></param>
    /// <param name="param"></param>
    /// <returns></returns>
    private bool isNeedToChangeFlyTypeToTargetPosition(LightingEffect_Fly config, int entityID)
    {
        if (config.flyType == FlyType.TargetPosition || entityID <= 0)
        {
            return false;
        }
        U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(entityID);
        if (U3D_Render.EntityView.isNullOrEmpty(ev))
        {
            return true;
        }

        if (!ev.StateMachine)
        {
            return true;
        }

        if (!ev.StateMachine.GetVisible())
        {
            return true;
        }

        if (!ev.gameObject.activeSelf)
        {
            return true;
        }

        return false;
    }
}
