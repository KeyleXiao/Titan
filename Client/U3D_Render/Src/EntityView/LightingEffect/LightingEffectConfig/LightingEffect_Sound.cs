using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GameLogic;


//范围类光效
[System.Serializable]
public class LightingEffect_Sound : LightingEffect
{
    [Desc("光效声音ID")]
    public int soundID = -1;

    public override LightingEffectConfigType GetEffectType()
    {
        return LightingEffectConfigType.Sound;
    }
}
