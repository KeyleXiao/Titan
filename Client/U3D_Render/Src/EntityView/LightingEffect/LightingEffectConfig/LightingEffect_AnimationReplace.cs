using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GameLogic;
[System.Serializable]

public class LightingEffect_AnimationReplace : LightingEffect
{
    [Desc("开始时刻，单位毫秒")]
    public int nStartTick;

    [Desc("结束时刻，单位毫秒")]
    public int nEndTick;

    public override LightingEffectConfigType GetEffectType()
    {
        return LightingEffectConfigType.AniReplace;
    }
}
