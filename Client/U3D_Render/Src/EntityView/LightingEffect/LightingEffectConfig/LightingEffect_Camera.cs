using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Effect;
using GameLogic;

//摄像机控制特效
[System.Serializable]
public class LightingEffect_Camera : LightingEffect
{

    [Desc("动画镜头的原点取的对象")]
    public CameraAnimOrgType animationOrgPos = CameraAnimOrgType.Hero;

    [Desc("动画镜头的原点需要贴地")]
    public bool animationOrgProjectFloor = false;

    [Desc("动画镜头强制取主摄像机（其他UI之类的场景需要挂镜头动画时请勾选）")]
    public bool animationMainCamera = false;

    [Desc("震屏光效需要附加到哪些人身上")]
    public EffectAttackType attackType = 0;

    [Desc("震屏光效影响范围,0表示无视范围")]
    public float effectRad = 10.0f;

    [Desc("光效存在时间（单位毫秒）")]
    public int effectLiveTick = 3000;

    [Desc("光效延迟开始时间（单位毫秒）")]
    public int delayTick = 0;

    [Desc("光效中断时摄像机恢复用的秒数（单位毫秒）")]
    public int effectFinishTick = 500;

    [Desc("结束时是否恢复一开始的摄像机")]
    public bool recoverTransform = false;
    public override LightingEffectConfigType GetEffectType()
    {
        return LightingEffectConfigType.Camera;
    }
}
