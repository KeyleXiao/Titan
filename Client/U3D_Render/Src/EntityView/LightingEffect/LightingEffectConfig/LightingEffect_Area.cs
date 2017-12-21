using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GameLogic;


//范围类光效
[System.Serializable]
public class LightingEffect_Area : LightingEffect
{
    [Desc("光效开始播放时间（单位毫秒）")]
    public int effectStartTick = 0;

    [Desc("诞生光效间隔时间（单位毫秒）")]
    public int effectIntervalTick = 3000;

    [Desc("诞生光效ID")]
    public int subEffectID = -1;

    [Desc("光效位置绑定骨骼名字(建议写全路径)")]
    public string posBindBone;

    [Desc("光效指定相对落地点的位移（指定后其他初始点无效）")]
    public Vector3 customOffset;

    [Desc("光效范围是投影到地面的，用于指定地面范围光效")]
    public bool projectOnGround=true;

    [Desc("光效距离一直延长到最大距离，用于设计与地面有爆炸效果的光效")]
    public bool extentToMaxDistance = true;

    [Desc("光效数量")]
    public int effectCount = 10;

    [Desc("光效最长生命时间，超过这个时间必须销毁（单位毫秒）")]
    public int effectTotalTick = 3000;

    public override LightingEffectConfigType GetEffectType()
    {
        return LightingEffectConfigType.Area;
    }
}
