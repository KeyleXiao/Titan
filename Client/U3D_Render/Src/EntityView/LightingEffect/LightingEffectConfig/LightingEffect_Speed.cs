using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GameLogic;

[System.Serializable]
//速度效果参数
public class LightingEffect_Speed : LightingEffect
{
    [Desc("开始时刻，单位毫秒")]
    public int nStartTick;

    [Desc("结束时刻，单位毫秒")]
    public int nEndTick;

    [Desc("全局时间速度（全局慢/快镜用）")]
    public float globalSpeed = 1.0f;

    [Desc("个人动作速度（全局慢/快镜用）")]
    public float animatorSpeed = 1.0f;

    [Desc("效果有效范围")]
    public float effectRad = 5.0f;

    public override LightingEffectConfigType GetEffectType()
    {
        return LightingEffectConfigType.Speed;
    }
}
