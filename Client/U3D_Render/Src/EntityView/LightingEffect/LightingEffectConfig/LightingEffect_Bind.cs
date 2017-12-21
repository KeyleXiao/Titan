
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GameLogic;

public enum BindType
{
    StayAlone = 0,
    BindWithBone = 1,
    StayInTargetPos= 2,
    BindWithWound = 3,
    StayInSourcePos = 4,
}

//绑定类光效
[System.Serializable]
public class LightingEffect_Bind : LightingEffect
{
    [Desc("在屏幕外时仍然创建，适用于持续效果，短时间光效（例如受击）请不要勾")]
    public bool bCreateOutsideScreen = true;

    [Desc("光效统一缩放比例")]
    public float scale=1.0f;

    [Desc("光效开始播放时间（单位毫秒）")]
    public int effectStartTick = 0;

    [Desc("光效存在时间（单位毫秒）")]
    public int effectLiveTick = 3000;

    [Desc("光效消失时间（单位毫秒），即淡出时间，等于0时立即销毁")]
    public int effectDispearTick = 0;

    [Desc("光效位置绑定骨骼名字(建议写全路径)")]
    public string posBindBone;

    [Desc("光效位置绑定挂点名字")]
    public string posBindWound;

    [Desc("光效相对骨骼或挂点的偏移")]
    public Vector3 posOffset;

    [Desc("光效旋转绑定骨骼名字(建议写全路径)")]
    public string rotationBindBone;

    [Desc("光效相对绑定骨骼的旋转")]
    public Vector3 rotationOffset;

    [Desc("光效面向镜头")]
    public bool faceToCamera=false;

    [Desc("光效向镜头偏移到物体外部")]
    public bool offsetToSurface = false;

    
    [Desc("光效绑定类型：StayAlone创建后不再运动；BindWithBone创建后跟随骨骼运动")]
    public BindType bindType = BindType.StayAlone;


    [Desc("光效面向，勾选后背向对手")]
    public bool faceAganistTarget = false;

    [Desc("光效贴地")]
    public bool bProjectOnFloor = false;

    [Desc("光效根据移动方向旋转")]
    public bool faceToWalkingDirection = false;

    [Desc("光效不随人物旋转")]
    public bool bNotRotate = false;

    [Desc("将target绑定到source上")]
    public bool bindTargetToSource = false;

    [Desc("将target绑定到source上时target的骨骼名")]
    public string bindTargetBone = "";

    //[Desc("增加一个材质")]
    //public Material addMaterial = null;

    [Desc("使用碰撞盒检测光效是否到达")]
    public bool triggerHitDetect = false;

    [Desc("音效ID")]
    public int SoundID = -1;

    public override LightingEffectConfigType GetEffectType()
    {
        return LightingEffectConfigType.Bind;
    }
}
