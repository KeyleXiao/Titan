using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GameLogic;
using Effect;
public enum FlyType
{
    TargetObject = 0,
    TargetPosition = 1,
    TargetPositionToTargetObject=2,
}

public enum StartPosType
{
    SrcEntityBone,
    DstEntityBone,
    TargetPosition,
    SourcePosition,
}
//飞行类光效
[System.Serializable]
public class LightingEffect_Fly : LightingEffect
{

    [Desc("光效统一缩放比例")]
    public float scale = 1.0f;

    [Desc("光效开始播放时间（单位毫秒）")]
    public int effectStartTick = 0;

    [Desc("光效存在时间（单位毫秒）")]
    public int effectLiveTick = 3000;

    [Desc("光效开始时原地滞留时间（单位毫秒）")]
    public int residenceTick = 0;

    [Desc("光效消失时间（单位毫秒），即淡出时间，等于0时立即销毁")]
    public int effectDispearTick = 0;

    [Desc("源点和目标点最大距离，超过距离关闭")]
    public float maxDistance=0.0f;

    [Desc("光效位置绑定骨骼名字(建议写全路径)")]
    public string posBindBone;

    [Desc("光效旋转绑定骨骼名字(建议写全路径)")]
    public string rotationBindBone;

    [Desc("光效相对骨骼的偏移")]
    public Vector3 posOffset;


    [Desc("光效开始时的偏移")]
    public Vector3 startPosOffset;

    [Desc("飞向光效目标对象骨骼")]
    public string targetTransformBone = "Wound";

    [Desc("目标点的偏移")]
    public Vector3 targetOffset=Vector3.zero;
    
    [Desc("光效飞行类型：TargetObject飞向目标物体（追踪）；TargetPosition飞向目标位置（定点）")]
    public FlyType flyType = FlyType.TargetObject;

    [Desc("飞行光效起点类型[没有用]")]
    //这个类型还会受到上面的flyType影响，因为是历史遗留问题，flytype里把source和dest都整合到一起了
    public StartPosType startType = StartPosType.SrcEntityBone;

    [Desc("光效是否始终面向目标")]
    public bool faceTarget=true;

        
    [Desc("光效相对地面最低高度，为了使得光效尽量不穿到地面下，如果是负数则此设定无效。")]
    public float lowestHeight=-1.0f;

    [Desc("光效相对地面最高高度，为了使得光效贴地，如果是负数则此设定无效。")]
    public float highestHeight = -1.0f;

    [Desc("光效飞行速度（可以等于0）")]
    public float flySpeed = 5.0f;

    [Desc("光效飞行时间（单位：秒，大于0时生效，生效时覆盖flySpeed效果）")]
    public float AbsFlyTime = 0.0f;
    [Desc("光效到达目的地时是否消失")]
    public bool destroyWhenReached = true;
    [Desc("对手死亡时关闭光效")]
    public bool destroyWhenTargetDead = false;
    [Desc("光效是否进行碰撞检测")]
    public bool useCollision = false;
    [Desc("光效产生碰撞时是否消失")]
    public bool destroyWhenCollision = true;
    [Desc("反路径飞行")]
    public bool reviseFly = false;
    [Desc("到达目的后返回")]
    public bool effectReturn = false;
    [Desc("光效关闭时返回")]
    public bool closeReturn = false;
    [Desc("光效返回时的飞行速度（等于0是表示与出发速度相同）")]
    public float returnSpeed = 0.0f;
    [Desc("始终有srcObject指向targetObject")]
    public bool faceSourceToTarget = false;
    [Desc("击中人时的后续光效，多个光效时用分号分隔")]
    public string effectIDWhenHitPlayer = "";
    [Desc("击中怪物时的后续光效，多个光效时用分号分隔")]
    public string effectIDWhenHitMonster = "";
    [Desc("击中地面时的后续光效，多个光效时用分号分隔")]
    public string effectIDWhenHitGround = "";
    [Desc("飞到最远处的后续光效，多个光效时用分号分隔")]
    public string effectIDWhenReached = "";
    [Desc("使用碰撞盒检测光效是否到达")]
    public bool triggerHitDetect = false;
    [Desc("结束时后续动作")]
    public string playAnimWhenClose = "";
    [Desc("是否做上抛运动")]
    public bool upcastMotion = false;
    [Desc("上抛运动的高度")]
    public float upcastHeight = 0.5f;
    [Desc("上抛最小偏转角")]
    public float minUpcastAngle = 0.0f;
    [Desc("上抛最大偏转角")]
    public float maxUpcastAngle = 0.0f;
    [Desc("上抛运动最高点在路径上的百分比")]
    public float upcastTopPrec = 0.5f;
    [Desc("音效ID")]
    public int SoundID = -1;


    [Desc("光效到达目标位置后延迟消失的时间（单位毫秒），当立即消失destroyWhenReached为无效时才生效，0表示不起效")]
    public int delayDestroyTick = 0;
    [Desc("光效延迟消失的时候隐藏美术指定部件多少毫秒，必须小于delayDestroyTick")]
    public int delayHideTick = 0;
    [Desc("光效延迟消失时吸附在目标上")]
    public bool attackToObject = false;

    [Desc("光效是否会影响护盾，当光效装到护盾上时，会关闭这个光效")]
    public bool AffectedShield = true;

    public override LightingEffectConfigType GetEffectType()
    {
        return LightingEffectConfigType.Fly;
    }
}
