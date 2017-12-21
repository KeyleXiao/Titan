using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GameLogic;
/// <summary>
/// 光效行为
/// </summary>
public sealed class LightingEffectBehavior_Bind : LightingEffectBehavior
{
    private LightingEffect_Bind BindConfig = null;

    private Transform m_BehaviorRootTransform = null;
    private Transform m_BindToTargetBoneTransform = null;
    private Quaternion m_BindToTargetOrgRotation;
    private Transform m_BindToTargetTransform = null;

    private uint mStartTick;
    private uint mEndTick;
    private Sound soundInstance = null;

    private CreatureStateMachine m_statemachine = null;
    private bool isCreated = false;
    public override void FillData(LightingEffect effectConfig, LightEffectParam param, LightingEffectResHandleInstance instance, Transform transformAgent)
    {
        base.FillData(effectConfig, param, instance, transformAgent);
        this.BindConfig = effectConfig as LightingEffect_Bind;
        if (!BindConfig && effectConfig)
        {
            Debug.LogWarning(effectConfig.nID + "配置不匹配,创建光效失败---" + GetType().ToString());
        }
    }

    public override bool Begin()
    {
        if (!BindConfig || null == EffectParam)
        {
            return false;
        }
        base.Begin();

        Camera cam = SoldierCamera.MainInstance().CurrentCamera;
        if (EffectParam.srcObject && cam && Initialize.mainCam)
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
            float disSqr =  LightingEffectFactory.CreateDistance * LightingEffectFactory.CreateDistance;

            if (EffectConfig.EnableDistanceOptimtized && (dis1.sqrMagnitude > disSqr) && (dis2.sqrMagnitude > disSqr))
            {
                //创建失败,要进行回收，要不然缓存无法更新
                LightingEffectBehavior temp = this;
                LightingEffectFactory.Instance.CacheEffectBehavior(ref temp);
                return false;
            }
        }


        if (EffectParam.speed <= 0.0f)
        {
            EffectParam.speed = 1.0f;
        }
        uint tick = GameLogicAPI.getTickCount();
        mStartTick = tick + (uint)((float)BindConfig.effectStartTick / EffectParam.speed);
        mEndTick = mStartTick + (uint)((float)BindConfig.effectLiveTick / EffectParam.speed);
        UnRegsiterSkinChangedEvent(EffectParam.nSrcEntityID, OnSkinChanged);
        RegsiterSkinChangedEvent(EffectParam.nSrcEntityID,OnSkinChanged);
        return true;
    }

    private void OnSkinChanged(SkinInstance si,int nPart)
    {
        if(!Valid)
        {
            return;
        }
        UpdateBindBoneTransform();
    }

    public override bool RePlay(LightEffectParam newParam)
    {
        if (!isCreated)
        {
            return true;
        }

        //if (!behaviorDestroyTimer)
        //{
        //    return false;
        //}
        needClose = false;
        effectParam = newParam;
        if (BindConfig.bindTargetToSource)
        {
            if (!EffectParam.targetObject)
            {
                return true;
            }
            m_BindToTargetTransform = EffectParam.targetObject.transform;
            BaseStateMachine machine = GetStateMachine(EffectParam.nTargetEntityID);
            if (machine != null)
            {
                machine.bPauseMachineUpdate = true;
                machine.enableCollider(false);
            }

            m_BindToTargetBoneTransform = null;
            if (BindConfig.bindTargetBone != "")
            {
                m_BindToTargetBoneTransform = FindTransformRoot(m_BehaviorRootTransform, BindConfig.bindTargetBone);
            }
            m_BindToTargetOrgRotation = m_BindToTargetTransform.rotation;
            initTransform();
            return true;
        }

        int effectNodeMask = 0;
        if (BindConfig.effectType ==LightingEffect.LightingEffectType.Hit)
        {
            //受击光效，不是主角的，则低一个等级
            if (EffectParam.nSrcEntityID != EntityFactory.MainHeroID &&
                EffectParam.nTargetEntityID != EntityFactory.MainHeroID)
            {
                effectNodeMask |= (int)Effect.EffectNodeMask.Mask_DownLevelQuality;
            }

        }
        if (EffectParam.nSrcEntityID == EntityFactory.MainHeroID)
        {
            effectNodeMask |= (int)Effect.EffectNodeMask.Mask_EnableLihgt;
        }
        //uint tick = GameLogicAPI.getTickCount();
        //if (BindConfig.effectLiveTick >= 0)
        //{
        //    behaviorDestroyTimer.destroyTick = GameLogicAPI.getTickCount() + (uint)(((float)BindConfig.effectLiveTick + (float)BindConfig.effectDispearTick) / EffectParam.speed + 500.0f); //比这个类延迟500毫秒，这个类没销毁的再自我销毁。
        //}
        //else
        //{
        //    behaviorDestroyTimer.destroyTick = 0; //0的时候表示无限循环
        //}

        initTransform();

        if (BindConfig.scale != 1.0f)
        {
            ResHandlePlayData.SizeScale = BindConfig.scale;
        }

        if (BindConfig.bUseSpeedScale)
        {
            ResHandlePlayData.Speed = EffectParam.speed;
        }

        ResHandlePlayData.parent = behaviorTransformAgent;
        ResHandlePlayData.bRePlay = true;
        ResHandlePlayData.isEnemy = EffectParam.nCreatorSide == 0;
        ResHandlePlayData.entityID = EffectParam.nSrcEntityID;
        ResHandlePlayData.SrcTransform = null;
        ResHandlePlayData.DstTransform = null;
        ResHandlePlayData.playMask = effectNodeMask;//绑定类光效，目标是自己(梁成、董哲宇、徐凡强一起决定的，2016/5/12);

        if (soundInstance && BindConfig.SoundID == soundInstance.ConfigID)
        {
            SoundManager.DeleteSound(ref soundInstance);
        }
        soundInstance = SoundManager.CreateSound(behaviorTransformAgent.gameObject, BindConfig.SoundID);
        return true;
    }

    private bool createEffect()
    {
        //如果bindTargetToSource为true，那么mEffectObject就是target
        isCreated = true;
        if (BindConfig.bindTargetToSource)
        {
            if (!EffectParam.targetObject)
            {
                return false;
            }
            m_BindToTargetTransform = EffectParam.targetObject.transform;
            BaseStateMachine machine = GetStateMachine(EffectParam.nTargetEntityID);
            if (machine != null)
            {
                machine.bPauseMachineUpdate = true;
                machine.enableCollider(false);
            }
            m_BehaviorRootTransform = m_BindToTargetTransform;
            m_BindToTargetBoneTransform = null;
            if (BindConfig.bindTargetBone != "")
            {
                m_BindToTargetBoneTransform = FindTransformEx(EffectParam.nTargetEntityID, BindConfig.bindTargetBone);
            }
            m_BindToTargetOrgRotation = m_BindToTargetTransform.rotation;
            initTransform();
            return true;
        }

        int effectNodeMask = 0;
        if (BindConfig.effectType ==LightingEffect.LightingEffectType.Hit)
        {
            //受击光效，不是主角的，则低一个等级
            if (EffectParam.nSrcEntityID != EntityFactory.MainHeroID &&
                EffectParam.nTargetEntityID != EntityFactory.MainHeroID)
            {
                effectNodeMask |= (int)Effect.EffectNodeMask.Mask_DownLevelQuality;
            }

        }
        if(EffectParam.nSrcEntityID == EntityFactory.MainHeroID)
        {
            effectNodeMask |= (int)Effect.EffectNodeMask.Mask_EnableLihgt;
        }
        m_BehaviorRootTransform = behaviorTransformAgent;
        //uint tick = GameLogicAPI.getTickCount();
        //behaviorDestroyTimer = LightingEffectFactory.Instance.GetBehaviorDestroyTimer();
        //m_BindToTargetRootTransform = m_BindToTargetTransform;
        //if (BindConfig.effectLiveTick >= 0)
        //{
        //    behaviorDestroyTimer.destroyTick = GameLogicAPI.getTickCount() + (uint)(((float)BindConfig.effectLiveTick + (float)BindConfig.effectDispearTick) / EffectParam.speed + 500.0f); //比这个类延迟500毫秒，这个类没销毁的再自我销毁。
        //}
        //else
        //{
        //    behaviorDestroyTimer.destroyTick = 0; //0的时候表示无限循环
        //}
        //behaviorDestroyTimer.hostEffect = this;

        //这样重置一下特效,应用变换后，在回来，防止某些拖尾光效不正确
        initTransform();

        if (BindConfig.scale != 1.0f)
        {
            ResHandlePlayData.SizeScale = BindConfig.scale;
        }

        if (BindConfig.bUseSpeedScale)
        {
            ResHandlePlayData.Speed = EffectParam.speed;
        }

        ResHandlePlayData.parent = behaviorTransformAgent;
        ResHandlePlayData.bPlay = true;
        ResHandlePlayData.isEnemy = EffectParam.nCreatorSide == 0;
        ResHandlePlayData.entityID = EffectParam.nSrcEntityID;
        ResHandlePlayData.SrcTransform = null;
        ResHandlePlayData.DstTransform = null;
        ResHandlePlayData.playMask = effectNodeMask;//绑定类光效，目标是自己(梁成、董哲宇、徐凡强一起决定的，2016/5/12);


        soundInstance = SoundManager.CreateSound(behaviorTransformAgent.gameObject, BindConfig.SoundID);
        if (BindConfig.triggerHitDetect)
        {
            ResHandlePlayData.AddhitEvent += onHitEvent;
        }
        dealWithMaterial(true);
        if (BindConfig.effectType ==LightingEffect.LightingEffectType.Hit)
        {
            LightingEffectManager.HitEffectCount++;
        }

        return true;
    }

    public override void extendEffectLift()
    {
        mEndTick += (uint)BindConfig.effectLiveTick;
    }

    public override void recoverEffectLift(LightEffectParam newParam)
    {
        effectParam = newParam;
        initTransform();
    }


    public override bool LateUpdate()
    {
        if (!Valid)
        {
            return false;
        }
        uint tick = GameLogicAPI.getTickCount();

        if (needClose == true || (BindConfig.effectLiveTick > 0) && (tick > mEndTick))
        {
            return false;
        }

        if (tick < mStartTick)
        {
            return true;
        }

        if (!isCreated)
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

                if (!EffectConfig.EnableDistanceOptimtized || (dis1.sqrMagnitude < disSqr) || (dis2.sqrMagnitude < disSqr))
                {
                    return createEffect();
                }
            }
            else
            {
                return createEffect();
            }
        }
        else if (BindConfig.bindType != BindType.StayAlone)
        {
            updateObjTrasform();
        }
        return true;
    }

    public override void setEnable(bool enable)
    {
        if (!Valid)
        {
            return;
        }
        if (!BindConfig.bindTargetToSource)
        {
            if (!Valid)
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
                if (BindConfig.effectType == LightingEffect.LightingEffectType.Hit)
                {
                    //受击光效，不是主角的，则低一个等级
                    if (EffectParam.nSrcEntityID != EntityFactory.MainHeroID &&
                        EffectParam.nTargetEntityID != EntityFactory.MainHeroID)
                    {
                        effectNodeMask |= (int)Effect.EffectNodeMask.Mask_DownLevelQuality;
                    }

                }
                if (EffectParam.nSrcEntityID == EntityFactory.MainHeroID)
                {
                    effectNodeMask |= (int)Effect.EffectNodeMask.Mask_EnableLihgt;
                }
                ResHandlePlayData.bPlay = true;
                ResHandlePlayData.isEnemy = EffectParam.nCreatorSide == 0;
                ResHandlePlayData.entityID = EffectParam.nSrcEntityID;
                ResHandlePlayData.SrcTransform = null;
                ResHandlePlayData.DstTransform = null;
                ResHandlePlayData.playMask = effectNodeMask;//绑定类光效，目标是自己(梁成、董哲宇、徐凡强一起决定的，2016/5/12);
            }
        }
    }

    public override void Close()
    {
        if (!Valid)
        {
            return ;
        }
        if (BindConfig.triggerHitDetect)
        {
            ResHandlePlayData.ReducehitEvent -= onHitEvent;
        }

        //光效关闭的时候必须把enable设为true，要不然光效不会消失。这个的愿意是服务器的消息有慢有快
        setEnable(true);
        if (BindConfig.scale != 1.0f && BindConfig.scale != 0.0f)
        {
            ResHandlePlayData.SizeScale = 1 / BindConfig.scale;
        }
        if (m_BindToTargetTransform && BindConfig.bindTargetToSource)
        {
            m_BindToTargetTransform.rotation = m_BindToTargetOrgRotation;

            BaseStateMachine machine = GetStateMachine(EffectParam.nTargetEntityID);
            if (machine != null)
            {
                machine.bPauseMachineUpdate = false;
                machine.enableCollider(true);
            }
        }
        if (!BindConfig.bindTargetToSource)
        {
            ResHandlePlayData.bPlay = false;
            if (BindConfig.effectType == LightingEffect.LightingEffectType.Hit)
            {
                LightingEffectManager.HitEffectCount--;
            }
            //behaviorDestroyTimer.destroyTick = GameLogicAPI.getTickCount() + (uint)BindConfig.effectDispearTick;
        }

        if (soundInstance && BindConfig.SoundID == soundInstance.ConfigID)
        {
            SoundManager.DeleteSound(ref soundInstance);
        }
        soundInstance = null;
        dealWithMaterial(false);

        base.Close();
    }


    public override void ResetAllMembers()
    {
        if(null != EffectParam)
        {
            UnRegsiterSkinChangedEvent(EffectParam.nSrcEntityID, OnSkinChanged);
        }
        isCreated = false;
        newMaterial = null;
        BindBonePosTransform = null;
        BindWoundPosTransform = null;
        BindBoneRotateTransform = null;
        //behaviorDestroyTimer = null;
        m_BindToTargetTransform = null;
        m_BehaviorRootTransform = null;
        m_BindToTargetBoneTransform = null;
        m_BindToTargetOrgRotation = Quaternion.identity;

        mStartTick = 0;
        mEndTick = 0;
        base.ResetAllMembers();
    }

    private void updateObjTrasform()
    {
        if (BindConfig.bindType == BindType.BindWithBone || BindConfig.bindType == BindType.BindWithWound)
        {
            UpdateBindBoneTransform();
            bindTransform();
            if (BindConfig.bProjectOnFloor)
            {
                behaviorTransformAgent.SetPosition(projectOnFloor(behaviorTransformAgent.position));
            }
        }


        if (m_statemachine != null && BindConfig.faceToWalkingDirection)
        {
            Vector3 vel = m_statemachine.rawInputMoveDirection;
            if (vel.sqrMagnitude > 0.001f)
            {
                if (BindConfig.faceToWalkingDirection)
                {
                    Vector3 lookPoint = vel;
                    vel.y = 0.0f;
                    lookPoint = lookPoint * 5.0f + behaviorTransformAgent.position;
                    behaviorTransformAgent.LookAt(lookPoint);
                }
            }
        }
    }

    //记录位移和旋转所绑定的骨骼
    private Transform BindBonePosTransform = null;
    private Transform BindWoundPosTransform = null;
    private Transform BindBoneRotateTransform = null;


    private void UpdateBindBoneTransform()
    {
        if(BindConfig.bindType == BindType.BindWithBone)
        {
            if (BindConfig.posBindBone != null && BindConfig.posBindBone != "" && EffectParam.srcObject)
            {
                BindBonePosTransform = FindTransformEx(EffectParam.nSrcEntityID, BindConfig.posBindBone);
            }

            if (BindConfig.rotationBindBone != null && BindConfig.rotationBindBone != "" && EffectParam.srcObject)
            {
                BindBoneRotateTransform = FindTransformEx(EffectParam.nSrcEntityID, BindConfig.rotationBindBone);
            }
        }
        else if(BindConfig.bindType == BindType.BindWithWound)
        {
            if (BindConfig.posBindWound != null && BindConfig.posBindWound != "" && EffectParam.srcObject)
            {
                BindWoundPosTransform = FindTransformWound(EffectParam.nSrcEntityID, BindConfig.posBindWound);
            }
        }
    }

    private void initTransform()
    {
        if (!BindConfig.bindTargetToSource  && EntityFactory.EffectGroup != null)
        {
            behaviorTransformAgent.SetParent(EntityFactory.EffectGroup.transform);
        }

        UpdateBindBoneTransform();

        if (BindConfig.faceAganistTarget == true)
        {
            rotateAsTargetForward();
        }

        m_statemachine = null;
        if (EffectParam.srcObject && BindConfig.faceToWalkingDirection)
        {
            m_statemachine = EffectParam.srcObject.GetComponent<CreatureStateMachine>();
        }


        bindTransform();

        if (BindConfig.bProjectOnFloor)
        {
            behaviorTransformAgent.SetPosition(projectOnFloor(behaviorTransformAgent.position));
        }
    }

    private void bindTransform()
    {
        if ((BindConfig.bindType == BindType.StayInSourcePos))
        {
            behaviorTransformAgent.SetPosition(EffectParam.sourcePos + BindConfig.posOffset);
            return;
        }

        if ((BindConfig.bindType == BindType.StayInTargetPos) || (0 != (EffectParam.nEffectFlag & (int)LightEffectParam.EFFECT_FLAG.BindInTargetPos)))
        {
            behaviorTransformAgent.SetPosition(EffectParam.targetPos + BindConfig.posOffset);
            return;
        }

        if (m_BehaviorRootTransform == null)
        {
            return;
        }

        //将光效绑定到指定位置
        Vector3 pos = Vector3.zero;

        if (BindConfig.bindType == BindType.BindWithBone && BindBonePosTransform)
        {
            pos = BindBonePosTransform.position;
            
        }
        else if(BindConfig.bindType == BindType.BindWithWound && BindWoundPosTransform)
        {
            pos = BindWoundPosTransform.position;
        }
        else if (EffectParam.srcObject)
        {
            pos = EffectParam.srcObject.transform.position;
        }
        else
        {
            needClose = true;
            return;
        }

        if (BindConfig.offsetToSurface)
        {
            //镜头向绑点射线，求交，因为求交必须由外向内，所以只能用这个方向
            Vector3 seg = pos - Initialize.mainCam.transform.position;
            Vector3 dir = seg.normalized;
            Ray castRay = new Ray(Initialize.mainCam.transform.position, dir);
            int LayerMaskWithoutGournd = -1 & (~(1 << LayerMask.NameToLayer(Config.LayerDefault)));
            LayerMaskWithoutGournd = LayerMaskWithoutGournd & (~1 << LayerMask.NameToLayer(Config.LayerBuilding));
            RaycastHit[] hitInfos;
            hitInfos = Physics.RaycastAll(castRay, seg.magnitude + 1.0f, LayerMaskWithoutGournd);

            foreach (RaycastHit hiti in hitInfos)
            {
                BaseStateMachine stateMachine = hiti.transform.GetComponentInParent<BaseStateMachine>();
                if (stateMachine != null && stateMachine.entityID == EffectParam.nSrcEntityID)
                {
                    pos = hiti.point;
                    break;
                }
            }
        }
        
        Quaternion rot = Quaternion.identity;
        if (EffectParam.srcObject)
        {
            rot = EffectParam.srcObject.transform.rotation;
        }
        if (BindBoneRotateTransform != null)
        {
            rot = BindBoneRotateTransform.rotation;
        }

        if (BindConfig.faceAganistTarget)
        {
            rot = behaviorTransformAgent.rotation;
        }

        if (BindConfig.faceToCamera)
        {
            behaviorTransformAgent.LookAt(Initialize.mainCam.transform.position);
            rot = behaviorTransformAgent.rotation;
        }

        Vector3 rotationDir = BindConfig.rotationOffset;
        Quaternion rot_c = Quaternion.identity;
        if (rotationDir.sqrMagnitude > 0.001f)
        {
            rot_c = Quaternion.Euler(BindConfig.rotationOffset);
        }
        rot = rot * rot_c;


        m_BehaviorRootTransform.SetPosition(pos);
        if (!BindConfig.bNotRotate)
        {
            m_BehaviorRootTransform.rotation = rot;
        }
        m_BehaviorRootTransform.Translate(BindConfig.posOffset);

        if (m_BindToTargetBoneTransform != null)
        {
            Vector3 offset = m_BehaviorRootTransform.position - m_BindToTargetBoneTransform.position;
            m_BehaviorRootTransform.position += offset;
        }


    }

    private Vector3 projectOnFloor(Vector3 position)
    {
        Vector3 result = position;

        RaycastHit _ClosestHitInfo;
        float _Distance = 100.0f;
        Ray PointToGroundRay = new Ray(position + new Vector3(0, 1.0f, 0), new Vector3(0, -1, 0));

        int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault));
        bool hit = Physics.Raycast(PointToGroundRay, out _ClosestHitInfo, _Distance, LayerMaskOnGround);
        if (!hit)
        {
            //向下没拾取到，可能已经在地底下，向上拾取试试
            PointToGroundRay = new Ray(position, new Vector3(0, 1, 0));
            hit = Physics.Raycast(PointToGroundRay, out _ClosestHitInfo, _Distance, LayerMaskOnGround);
        }
        Vector3 hitPoint = _ClosestHitInfo.point;
        if (hit)
        {

            if (showLog)
            {
                Trace.Log("TargetPos=" + position.ToString() + "FloorPoint=" + hitPoint.ToString());
            }

            result = hitPoint;
        }


        return result;
    }

    private void rotateAsTargetForward()
    {
        Vector3 srcPos = behaviorTransformAgent.position;
        if(BindConfig.bindType == BindType.StayInSourcePos)
        {
            srcPos = EffectParam.sourcePos;
        }
        else
        {
            if (EffectParam.srcObject != null)
            {
                srcPos = EffectParam.srcObject.transform.position;
            }

        }


        Vector3 targetPos = EffectParam.targetPos;
        if ((BindConfig.bindType != BindType.StayInTargetPos) && EffectParam.targetObject != null)
        {
            targetPos = EffectParam.targetObject.transform.position;
        }

        Vector3 dir = srcPos - targetPos;
        if (dir.sqrMagnitude > 0.001f)
        {
            dir.Normalize();
            behaviorTransformAgent.forward = dir;
        }


        //以下是旧版的功能，继承target的面向，现在改成target指向source的方向
        //if (bindType == BindType.StayInTargetPos)
        //{
        //    if (param.srcObject == null)
        //    {
        //        Trace.Log("Effect parent is null!");
        //        return;
        //    }


        //    Vector3 dir = param.srcObject.transform.position - param.targetPos;
        //    if (dir.sqrMagnitude > 0.001f)
        //    {
        //        dir.Normalize();
        //        m_effectObject.transform.forward = dir;
        //    }
        //    return;
        //}


        //if (param.targetObject==null)
        //{
        //    Trace.Log("Effect target is null!");
        //    return;
        //}

        //m_effectObject.transform.forward=param.targetObject.transform.forward;
    }

    //Add or remove material
    private Material newMaterial = null;
    public void dealWithMaterial(bool enable)
    {
        //if (!BindConfig.addMaterial) return;
        //if (!newMaterial)
        //{
        //    newMaterial = new Material(BindConfig.addMaterial);
        //}

        //Transform SkinDataObj = null;
        //System.Collections.Generic.List<SkinnedMeshRenderer> rs = null;
       
        //BaseStateMachine machine = GetStateMachine(EffectParam.nTargetEntityID);

        //if (machine)
        //{
        //    SkinInstance sk = machine.SkinControl.CurrentSkinInstance;
        //    if(!SkinInstance.isNullOrEmpry(sk))
        //    {
        //        SkinDataObj = sk.SkinDataObj;
        //        rs = sk.SkinnedMeshRenderList;
        //    }
        //}
        //if (rs == null)
        //{
        //    return;
        //} 
        //foreach (SkinnedMeshRenderer r in rs)
        //{
        //    if (r.materials != null)
        //    {
        //        if (enable && newMaterial != null)
        //        {
        //            //新增一个material
        //            int mLength = r.materials.Length;
        //            Material[] newMaterials = new Material[mLength + 1];
        //            Array.Copy(r.materials, newMaterials, mLength);
        //            newMaterials[mLength] = newMaterial;
        //            r.materials = newMaterials;

        //        }
        //        else if (newMaterial != null)
        //        {
        //            //删除新增的material
        //            List<Material> recoverMaterials = new List<Material>();
        //            foreach (Material mat in r.materials)
        //            {
        //                string instanceName = newMaterial.name + " (Instance)";
        //                if ((mat.name != instanceName) && (mat.name != newMaterial.name))
        //                {
        //                    recoverMaterials.Add(mat);
        //                }

        //            }
        //            r.materials = recoverMaterials.ToArray();
        //        }
        //    }
        //}

    }

    void onHitEvent(Collider collider)
    {
        if (!BindConfig.triggerHitDetect)
        {
            return;
        }
        int layer = collider.transform.gameObject.layer;
        int layerMask = 1 << layer;
        BaseStateMachine state = collider.transform.gameObject.GetComponentInParent<BaseStateMachine>();
        if (state != null)
        {
            Debug.Log("Entity" + EffectParam.nSrcEntityID + "'s Effect" + BindConfig.nID + " hit by Entity:" + state.entityID);
        }
    }
}
