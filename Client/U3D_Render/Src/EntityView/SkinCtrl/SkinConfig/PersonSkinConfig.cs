/*******************************************************************
** 文件名:	EntitySkinConfig.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2015-06-30
** 版  本:	1.0
** 描  述:	人物皮肤配置
********************************************************************/
using UnityEngine;
using System.Collections;
using System;
/// <summary>
/// 人物皮肤配置
/// </summary>
public class PersonSkinConfig : EntitySkinConfig
{
    public Transform Main = null;
    public Transform Root_M = null;
    public Transform BackA_M = null;
    public Transform Root_MCopy = null;
    
    public Transform weaponObject = null;
    public Transform weaponObject2 = null;

    public bool adjustSpine = false;
    //public bool separingBody = false;
    public bool RootAReplace = false; //上下半身分离采用RootA过渡节点
    public int AttackAnimatorLayer = 0; //将ab状态机合成一个

    public Vector3 rootAngleWithAim = Vector3.zero;

    public bool progressWeight = true;  //上下半身分离时渐变
    public float lookAngleScale = 0.0f;  //弯腰瞄准程度(0-1)--应该丢到实体属性
    public Transform lookRefTransform = null;  //弯腰瞄准参考骨骼--应该丢到实体属性

    public GameObject ScreenCastObject = null;
    public int jumpDeltyTick = 10; //跳跃起高度跳延迟毫秒 --应该丢到实体属性
    public float maxFallSpeed = 20.0f;//与控制类交互用的参数 --应该丢到实体属性

    public RuntimeAnimatorController controllerA;
    public RuntimeAnimatorController controllerB;

    public bool MorphHip = true;
}
