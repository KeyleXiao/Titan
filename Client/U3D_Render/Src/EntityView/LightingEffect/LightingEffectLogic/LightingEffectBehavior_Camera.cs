using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Effect;
using GameLogic;
using U3D_Render.CameraControl;
/// <summary>
/// 光效行为
/// </summary>
public sealed class LightingEffectBehavior_Camera : LightingEffectBehavior
{
    private LightingEffect_Camera cameraConfig = null;

    private uint mStartTick;
    private uint mEndTick;
    private bool bStartEffect = false;

    public override void FillData(LightingEffect effectConfig, LightEffectParam param, LightingEffectResHandleInstance instance, Transform transformAgent)
    {
        base.FillData(effectConfig, param, instance, transformAgent);
        this.cameraConfig = effectConfig as LightingEffect_Camera;
        if (!cameraConfig && effectConfig)
        {
            Debug.LogWarning(effectConfig.nID + "配置不匹配,创建光效失败---" + GetType().ToString());
        }
    }

    public override bool Begin()
    {
        if (!cameraConfig || null == EffectParam)
        {
            return false;
        }
        base.Begin();
        uint tick = GameLogicAPI.getTickCount();
        mStartTick = tick;
        mEndTick = mStartTick + (uint)cameraConfig.effectLiveTick + (uint)cameraConfig.delayTick;
        if (cameraConfig.effectRad > 0.01f && EntityFactory.MainHero)
        {
            Vector3 deta = EffectParam.targetPos - EntityFactory.MainHero.transform.position;
            if (deta.magnitude > cameraConfig.effectRad)
            {
                //创建失败,要进行回收，要不然缓存无法更新
                LightingEffectBehavior temp = this;
                LightingEffectFactory.Instance.CacheEffectBehavior(ref temp);
                return false;
            }
        }

        bStartEffect = true;
        return true;
    }

    private SoldierCamera m_camera;

    private void ApplyResHandle()
    {
        if (!bStartEffect)
        {
            return;
        }
        
        if(LightingEffectResHandleInstance.isNullOrEmpry(ResHandleInstance))
        {
            needClose = true;
            return;
        }

        if(!ResHandleInstance.CameraAnimation)
        {
            needClose = true;
            return;
        }
        if (cameraConfig.attackType == EffectAttackType.Source)
        {
            if (EffectParam.nSrcEntityID != EntityFactory.MainHeroID)
            {
                needClose = true;
                return;
            }
        }
        else if (cameraConfig.attackType == EffectAttackType.Target)
        {
            if (EffectParam.nTargetEntityID != EntityFactory.MainHeroID)
            {
                needClose = true;
                return;
            }
        }
        else if (cameraConfig.attackType == EffectAttackType.SourceAndTarget)
        {
            if (EffectParam.nSrcEntityID != EntityFactory.MainHeroID && EffectParam.nTargetEntityID != EntityFactory.MainHeroID)
            {
                needClose = true;
                return;
            }
        }

        uint tick = GameLogicAPI.getTickCount();

        m_camera = SoldierCamera.MainInstance<SoldierCamera>();
        if (!m_camera)
        {
            needClose = true;
            return;
        }

        if (ResHandleInstance.CameraAnimation.animationClip)
        {
            Transform sourceTransform = null;
            if (EffectParam.srcObject)
            {
                sourceTransform = EffectParam.srcObject.transform;
            }
             CamAnimStateParam param = new CamAnimStateParam(ResHandleInstance.CameraAnimation.animationClip, cameraConfig.animationOrgPos, cameraConfig.animationOrgProjectFloor, EffectParam.targetPos, sourceTransform, cameraConfig.animationMainCamera);
             m_camera.EnableMode(CameraMode.CameraAnim, true, param);

            return;
        }

        m_camera.playEffect(cameraConfig.delayTick, cameraConfig.effectLiveTick, ResHandleInstance.CameraAnimation.cameraAxis, cameraConfig.recoverTransform);
    }

    public override bool LateUpdate()
    {
        if (!Valid)
        {
            return false;
        }
        uint tick = GameLogicAPI.getTickCount();

        if (needClose == true || (cameraConfig.effectLiveTick > 0) && (tick > mEndTick))
        {
            return false;
        }

        if (tick < mStartTick)
        {
            return true;
        }

        return true;
    }

    public override void Close()
    {
        if (!Valid)
        {
            return;
        }
        uint tick = GameLogicAPI.getTickCount();

        if (m_camera != null)
        {
            if (tick < mEndTick)
            {
                m_camera.finishEffect((uint)cameraConfig.effectFinishTick);
            }
            else
            {
                CameraEffectUtil.MainInstance.stopEffect();
            }

            SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.CameraAnim, false);

        }

        base.Close();
    }

    public override void ResetAllMembers()
    {
        bStartEffect = false;
        mStartTick = 0;
        mEndTick = 0;
        m_camera = null;
        base.ResetAllMembers();
    }

    public override void OnResourcesLoadFinish(LightingEffectResHandleInstance resHandle)
    {
        ApplyResHandle();
    }
}
