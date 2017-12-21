using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GameLogic;
/// <summary>
/// 光效行为
/// </summary>
public sealed class LightingEffectBehavior_Ward : LightingEffectBehavior
{
    private LightingEffect_Ward wardConfig = null;

    private bool bStartedEfect = false;
    private uint m_startTick;
    private uint m_endTick;


    public override void ResetAllMembers()
    {
        m_endTick = 0;
        m_startTick = 0;
        bStartedEfect = false;
        base.ResetAllMembers();
    }
    public override void FillData(LightingEffect effectConfig, LightEffectParam param, LightingEffectResHandleInstance instance, Transform transformAgent)
    {
        base.FillData(effectConfig, param, instance, transformAgent);
        this.wardConfig = effectConfig as LightingEffect_Ward;
        if (!wardConfig && effectConfig)
        {
            Debug.LogWarning(effectConfig.nID + "配置不匹配,创建光效失败---" + GetType().ToString());
        }
    }

    public override bool Begin()
    {
        if (!wardConfig || null == EffectParam)
        {
            return false;
        }
        base.Begin();
        uint tick = GameLogicAPI.getTickCount();
        m_startTick = (uint)wardConfig.nStartTick + tick;
        m_endTick = (uint)wardConfig.nEndTick + tick;
        return true;
    }

    public override bool LateUpdate()
    {
        if (!Valid)
        {
            return false;
        }
        uint tick = GameLogicAPI.getTickCount();

        if (tick >= m_endTick)
        {
            needClose = true;
            return false;
        }

        if (tick >= m_startTick && !bStartedEfect)
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
                    return startEffect();
                }
            }
            else
            {
                return startEffect();
            }
        }



        return true;
    }

    public override void extendEffectLift()
    {
        m_endTick += (uint)wardConfig.nEndTick;
    }

    public override void recoverEffectLift(LightEffectParam newParam)
    {
        effectParam = newParam;
        behaviorTransformAgent.SetPosition( effectParam.targetPos);
    }


    private bool startEffect()
    {
        U3D_Render.EntityView view = EntityFactory.getEntityViewByID(EffectParam.nSrcEntityID);
        if (view == null)
        {
            return false;
        }
        bStartedEfect = true;
        behaviorTransformAgent.SetParent(EntityFactory.EffectGroup.transform);
        behaviorTransformAgent.SetPosition(EffectParam.targetPos);
        ResHandlePlayData.bPlay = true;
        ResHandlePlayData.parent = behaviorTransformAgent;

        if (wardConfig.bUseSpeedScale)
        {
            ResHandlePlayData.Speed = EffectParam.speed;
        }
        ResHandlePlayData.isEnemy = EffectParam.nCreatorSide == 0;
        ResHandlePlayData.entityID = EffectParam.nTargetEntityID;
        ResHandlePlayData.SrcTransform = null;
        ResHandlePlayData.DstTransform = null;


        //调整光效所在层，制造结界是否能进出的效果
        if (wardConfig.blockAllCreature)
        {
            ResHandlePlayData.effectColliderLayer = LayerMask.NameToLayer(Config.LayerNeutrality);
        }
        else if (view.CampFlag == SPELL.CampFlag.CampFlag_Friend || view.CampFlag == SPELL.CampFlag.CampFlag_Self)
        {
            ResHandlePlayData.effectColliderLayer = LayerMask.NameToLayer(Config.LayerWard_Friend);
        }
        else
        {
            ResHandlePlayData.effectColliderLayer = LayerMask.NameToLayer(Config.LayerWard_Enemy);
        }


        return true;
    }

}
