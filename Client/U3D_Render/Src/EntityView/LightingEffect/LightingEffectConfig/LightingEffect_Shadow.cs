using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GameLogic;

[System.Serializable]
//残影效果参数
public class LightingEffect_Shadow : LightingEffect
{
    [Desc("开始时刻，单位毫秒")]
    public int nStartTick;

    [Desc("结束时刻，单位毫秒")]
    public int nEndTick;
    
    [Desc("每个残影开始的透明度")]
    public float fStartAlpha=1.0f;

    [Desc("每个残影存在的时间，单位毫秒")]
    public int nShadowMaintainTick;

    [Desc("每个残影淡出的时间，单位毫秒")]
    public int nShadowFadeoutTick;

    [Desc("插入残影的时间间隔，单位毫秒，大于0时关键帧失效")]
    public int shadowIntervalTicks=0;
   
    [Desc("需要插入残影的关键帧，单位毫秒")]
    public int [] shadowFrameTicks;

    [Desc("替换为的材质")]
    public Material alphaMaterial = null;

    [Desc("替换为的材质的透明参数名字")]
    public string alphaAttributeName = "";

    [Desc("武器无残影")]
    public bool DisableWeapon = false;

    [Desc("替换为的材质的透明参数类型")]
    public AlphaAttributeType alphaAttributeType = AlphaAttributeType.Color;

    [Desc("残影与残影/人物间最短间隔距离，少于这个距离则不建立残影，小于0.01f时此设置无效")]
    public float minDistance = 0.2f;


    public enum AlphaAttributeType
    {
        Color = 0,
        Float = 1,
        Int = 2,
    }

    public override LightingEffectConfigType GetEffectType()
    {
        return LightingEffectConfigType.Shadow;
    }
}
