using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GameLogic;

[System.Serializable]
//残影效果参数
public class LightingEffect_Ward : LightingEffect
{
    [Desc("开始时刻，单位毫秒")]
    public int nStartTick;

    [Desc("结束时刻，单位毫秒")]
    public int nEndTick;

    [Desc("无视阵营阻挡一切")]
    public bool blockAllCreature = false;


    public override LightingEffectConfigType GetEffectType()
    {
        return LightingEffectConfigType.Ward;
    }
}
