using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;

// 光效基类
// 主要的光效类型:
// 1.绑定型光效 配置的骨骼绑定
// 2.范围光效   创建的位置是tile
// 3.飞行光效   目标点,增加碰撞

public class LightEffectParam
{
    public enum EFFECT_FLAG //特殊需求标识位
    {
        BindInTargetPos = 1,
        CustomSourcePos = 2,
        FlyToTargetPos = 4,     // 飞行到目标位置
    }

    //以下为光效创建时需要的参数，有逻辑层提供
    public GameObject srcObject = null; //光效的攻击者
    public int nSrcEntityID = 0; //光效的攻击者ID
    public GameObject targetObject = null; //光效的受击者
    public int nTargetEntityID = 0; //光效受击者ID
    public Vector3 sourcePos = Vector3.zero;//光效的起始位置
    public Vector3 targetPos = Vector3.zero; //光效的目标位置
    public int nFeedbackID = 0; //光效的反馈ID (用于碰撞等反馈)
    public bool bIsHero=false; //挂接对象是否为游戏可控制的主角
    public float fDistance;		// 技能距离
    public float fAttackRange;		// 攻击范围，当为范围光效是，此参数小于零表示所有光效指向目标点
    public int nEffectFlag=0; //特殊需求标识位
    /// <summary>
    /// 0-地方，1-友方
    /// </summary>
    public int nCreatorSide = 0;

    public float speed = 1.0f; //播放速度倍率

    public void ColoneTo(ref LightEffectParam targetParam)
    {
        targetParam.srcObject = srcObject;
        targetParam.nSrcEntityID = nSrcEntityID;
        targetParam.nTargetEntityID = nTargetEntityID;
        targetParam.targetObject = targetObject;
        targetParam.targetPos = targetPos;
        targetParam.nFeedbackID = nFeedbackID;
        targetParam.bIsHero = bIsHero;
        targetParam.fDistance = fDistance;
        targetParam.fAttackRange = fAttackRange;
        targetParam.nEffectFlag = nEffectFlag;
        targetParam.speed = speed;
    }
}

[System.Serializable]
public class LightingEffect : ScriptableObject
{

    public enum LightingEffectType
    {
        /// <summary>
        /// 受击光效
        /// </summary>
        Hit,

        /// <summary>
        /// 普通光效
        /// </summary>
        Normal,
    }

    public string AssetGUID = string.Empty;

    [Desc("光效预制体(编辑预览用，在游戏中无用)")]
    public GameObject effectPrefb = null;

    [Desc("光效ID")]
    public int nID = 0;

    [Desc("光效描述")]
    public string strDesc = "";

    [Desc("是否全局光效，全局光效则不依赖于某个人物生存")]
    public bool bGlobal = false;

    [Desc("一个人物是否只能有一个光效")]
    public bool bSingle = false;

    [Desc("单一光效时是否新发参数覆盖就参数")]
    public bool bRecoverSingleEffect = false;

    /// <summary>
    /// 是否需要速度缩放,有的光效不需要速度缩放，也能正常，缩放了反而不正常，比如根据距离产生粒子的特效
    /// </summary>
    [Desc("是否需要速度缩放(此参数已无用)")]
    public bool bUseSpeedScale = true;

    /// <summary>
    /// 某些技能是要在技能时间里重复创建多个同一光效，比如女枪的大招
    /// </summary>
    [Desc("忽略单一光效优化")]
    public bool bIgnoreSingleEffectOptimtized = false;

    /// <summary>
    /// 某些地图炮不能开启这个距离优化
    /// </summary>
    [Desc("启用距离优化")]
    public bool EnableDistanceOptimtized = true;

    /// <summary>
    /// 施放者死亡时，关闭此光效
    /// </summary>
    [Desc("施放者死亡时，关闭此光效。此功能适用于一次性类光效（例如技能），不建议用于BUFF等永久类光效，因为复活时不会自动加回")]
    public bool CloseWhenDead = false;

    public LightingEffectType effectType = LightingEffectType.Normal;

    public virtual string GetAssetGUID()
    {
        return AssetGUID;
    }

    public virtual LightingEffectConfigType GetEffectType()
    {
        return LightingEffectConfigType.Invalid;
    }
}
