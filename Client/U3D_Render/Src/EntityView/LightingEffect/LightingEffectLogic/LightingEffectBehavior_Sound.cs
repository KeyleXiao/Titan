using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GameLogic;
/// <summary>
/// 光效行为
/// </summary>
public sealed class LightingEffectBehavior_Sound : LightingEffectBehavior
{
    private LightingEffect_Sound soundConfig = null;

    public override void FillData(LightingEffect effectConfig, LightEffectParam param, LightingEffectResHandleInstance instance, Transform transformAgent)
    {
        base.FillData(effectConfig, param, instance, transformAgent);
        this.soundConfig = effectConfig as LightingEffect_Sound;
        if (!soundConfig && effectConfig)
        {
            Debug.LogWarning(effectConfig.nID + "配置不匹配,创建光效失败---" + GetType().ToString());
        }
    }

    public override bool Begin()
    {
        if (!soundConfig || null == EffectParam)
        {
            return false;
        }
        base.Begin();
        SoundManager.CreateSound(null, soundConfig.soundID);

        return true;
    }

    public override bool LateUpdate()
    {
        return true;
    }

    public override void Close()
    {
        base.Close();
    }

    public override void ResetAllMembers()
    {
        base.ResetAllMembers();
    }

}
