using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GameLogic;
/// <summary>
/// 光效行为
/// </summary>
public sealed class LightingEffectBehavior_Speed : LightingEffectBehavior
{
    private LightingEffect_Speed speedConfig = null;

    private void startEffect()
    {
        if (Mathf.Abs(Time.timeScale - speedConfig.globalSpeed) > 0.001f && (speedConfig.globalSpeed > 1.001f || speedConfig.globalSpeed < 0.999f))
        {
            Time.timeScale = speedConfig.globalSpeed;
        }

        CreatureStateMachine machine = GetStateMachine(EffectParam.nSrcEntityID) as CreatureStateMachine;
        if ((speedConfig.animatorSpeed < 0.999f || speedConfig.animatorSpeed > 1.001f) && machine != null && machine.animatorCtrl != null)
        {
            machine.animatorCtrl.changeAnbSpeed(speedConfig.nID, speedConfig.animatorSpeed, true);
        }
    }

    public override void Close()
    {
        if (!Valid)
        {
            return ;
        }
        base.Close();
        if (Mathf.Abs(Time.timeScale - speedConfig.globalSpeed) < 0.001f && (Time.timeScale < 0.999f || Time.timeScale > 1.001f))
        {
            Time.timeScale = 1.0f;
        }

        CreatureStateMachine machine = GetStateMachine(EffectParam.nSrcEntityID) as CreatureStateMachine;
        if ((speedConfig.animatorSpeed < 0.999f || speedConfig.animatorSpeed > 1.001f) && machine != null && machine.animatorCtrl != null)
        {
            machine.animatorCtrl.changeAnbSpeed(speedConfig.nID, speedConfig.animatorSpeed, false);
        }
    }

    private uint m_startTick;
    private uint m_endTick;
    //private bool m_bStartedEffect = false;

    public override void ResetAllMembers()
    {
        m_startTick = 0;
        m_endTick = 0;
        base.ResetAllMembers();
    }
    public override void FillData(LightingEffect effectConfig, LightEffectParam param, LightingEffectResHandleInstance instance, Transform transformAgent)
    {
        base.FillData(effectConfig, param, instance, transformAgent);
        this.speedConfig = effectConfig as LightingEffect_Speed;
        if (!speedConfig && effectConfig)
        {
            Debug.LogWarning(effectConfig.nID + "配置不匹配,创建光效失败---" + GetType().ToString());
        }
    }

    public override bool Begin()
    {
        if (!speedConfig || null == EffectParam)
        {
            return false;
        }
        base.Begin();
        //如果在有效距离范围外，则不触发
        Vector3 vec = EffectParam.srcObject.transform.position - EntityFactory.MainHero.transform.position;
        if (vec.magnitude > speedConfig.effectRad)
        {
            //创建失败,要进行回收，要不然缓存无法更新
            LightingEffectBehavior temp = this;
            LightingEffectFactory.Instance.CacheEffectBehavior(ref temp);
            return false;
        }

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
                //创建失败,要进行回收，要不然缓存无法更新
                LightingEffectBehavior temp = this;
                LightingEffectFactory.Instance.CacheEffectBehavior(ref temp);
                return false;
            }
        }

        uint tick = GameLogicAPI.getTickCount();
        m_startTick = (uint)speedConfig.nStartTick + tick;
        m_endTick = (uint)speedConfig.nEndTick + tick;
        //m_bStartedEffect = false;
        return true;
    }

    public override void extendEffectLift()
    {
        m_endTick += (uint)speedConfig.nEndTick;
    }

    public override void recoverEffectLift(LightEffectParam newParam)
    {
        effectParam = newParam;
    }


    public override bool LateUpdate()
    {
        if (!Valid)
        {
            return false;
        }
        uint tick = GameLogicAPI.getTickCount();
        if (tick >= m_startTick)
        {
            startEffect();
            //m_bStartedEffect = true;
        }

        if (tick >= m_endTick)
        {
            needClose = true;
            return false;
        }

        return true;
    }

}
