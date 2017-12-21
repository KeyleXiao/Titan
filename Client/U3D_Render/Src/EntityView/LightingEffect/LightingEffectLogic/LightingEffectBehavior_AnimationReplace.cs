using UnityEngine;
using System.Collections;

/// <summary>
/// 光效行为
/// </summary>
public sealed class LightingEffectBehavior_AnimationReplace : LightingEffectBehavior
{
    private LightingEffect_AnimationReplace aniRepConfig = null;
    private CreatureAnimatorCtrl animatorCtrl;

    private void startEffect()
    {
        if (null == ResourceHandle || null == animatorCtrl)
        {
            return;
        }
        if (!ResourceHandle.ReplaceAnimation)
        {
            return;
        }
        if (ResourceHandle.ReplaceAnimation.clipPairsA.Length > 0)
        {
            foreach (Effect.AnimationReplacePair animPair in ResourceHandle.ReplaceAnimation.clipPairsA)
            {
                if (animPair.orgClip == null || animPair.replaceClip == null)
                {
                    Trace.LogError("替换动作类光效ID=" + aniRepConfig.nID + "的动作为空，请检查配置");
                    return;
                }
            }

            animatorCtrl.replaceAnimationClipA(ResourceHandle.ReplaceAnimation.clipPairsA, false);
        }
        if (ResourceHandle.ReplaceAnimation.clipPairsB.Length > 0)
        {
            foreach (Effect.AnimationReplacePair animPair in ResourceHandle.ReplaceAnimation.clipPairsB)
            {
                if (animPair.orgClip == null || animPair.replaceClip == null)
                {
                    Trace.LogError("替换动作类光效ID=" + aniRepConfig.nID + "的动作为空，请检查配置");
                    return;
                }
            }

            animatorCtrl.replaceAnimationClipB(ResourceHandle.ReplaceAnimation.clipPairsB, false);
        }

    }

    public override void Close()
    {
        if (!Valid)
        {
            return ;
        }
        if (!m_bStartedEffect)
        {
            return;
        }

        if (null != ResourceHandle)
        {
            if (!ResourceHandle.ReplaceAnimation)
            {
                return;
            }

            if (ResourceHandle.ReplaceAnimation.clipPairsA.Length > 0)
            {
                animatorCtrl.replaceAnimationClipA(ResourceHandle.ReplaceAnimation.clipPairsA, true);
            }
            if (ResourceHandle.ReplaceAnimation.clipPairsB.Length > 0)
            {
                animatorCtrl.replaceAnimationClipB(ResourceHandle.ReplaceAnimation.clipPairsB, true);
            }
        }

    }

    private uint m_startTick;
    private uint m_endTick;

    public override void FillData(LightingEffect effectConfig, LightEffectParam param, LightingEffectResHandleInstance instance, Transform transformAgent)
    {
        base.FillData(effectConfig, param, instance, transformAgent);

        this.aniRepConfig = effectConfig as LightingEffect_AnimationReplace;
        if (!aniRepConfig && effectConfig)
        {
            Debug.LogWarning(effectConfig.nID + "配置不匹配,创建光效失败---" + GetType().ToString());
        }
    }

    public override bool Begin()
    {
        if (!aniRepConfig || null == EffectParam)
        {
            return false;
        }
        base.Begin();
        uint tick = GameLogicAPI.getTickCount();

        m_startTick = (uint)aniRepConfig.nStartTick + tick;
        m_endTick = (uint)aniRepConfig.nEndTick + tick;
        m_bStartedEffect = false;
        if (!EffectParam.srcObject)
        {
            //创建失败,要进行回收，要不然缓存无法更新
            LightingEffectBehavior temp = this;
            LightingEffectFactory.Instance.CacheEffectBehavior(ref temp);
            return false;
        }

        CreatureStateMachine state = GetStateMachine(EffectParam.nSrcEntityID) as CreatureStateMachine;
        if (state == null)
        {
            //创建失败,要进行回收，要不然缓存无法更新
            LightingEffectBehavior temp = this;
            LightingEffectFactory.Instance.CacheEffectBehavior(ref temp);
            return false;
        }
        animatorCtrl = state.animatorCtrl;
        if (animatorCtrl == null)
        {
            //创建失败,要进行回收，要不然缓存无法更新
            LightingEffectBehavior temp = this;
            LightingEffectFactory.Instance.CacheEffectBehavior(ref temp);
            return false;
        }
        return true;
    }

    public override void extendEffectLift()
    {
        m_endTick += (uint)aniRepConfig.nEndTick;
    }

    public override void recoverEffectLift(LightEffectParam newParam)
    {
        effectParam = newParam;
    }

    private bool m_bStartedEffect = false;

    public override bool LateUpdate()
    {
        if (!Valid)
        {
            return false;
        }
        uint tick = GameLogicAPI.getTickCount();
        if (!m_bStartedEffect && tick >= m_startTick)
        {
            
            m_bStartedEffect = true;
        }

        if (tick >= m_endTick)
        {
            Close();
            return false;
        }

        return true;
    }

    public override void ResetAllMembers()
    {
        animatorCtrl = null;
        m_startTick = 0;
        m_endTick = 0;
        base.ResetAllMembers();
    }

    public override void OnResourcesLoadFinish(LightingEffectResHandleInstance resHandle)
    {
        startEffect();
    }
}
