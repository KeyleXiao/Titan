/*******************************************************************
** 文件名:	DamageDef_ManagedDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/24/2015
** 版  本:	1.0
** 描  述:	伤害定义数据
********************************************************************/

#pragma once

DESC("伤害效果类型")
ENUM DamageEffectType
{
	DamageEffect_Spell = 0,		// 技能产生伤害	
	DamageEffect_Effect,		// 效果产生伤害
};


DESC("攻击结果类型")
ENUM EAttackResultType
{
	AttackResult_Damage		= 0x1,		// 正常伤害(含格挡)
	AttackResult_Cure		= 0x2,		// 治疗(吸血)
	AttackResult_Fatal		= 0x4,		// 致命
	AttackResult_Resist		= 0x8,		// 格挡
	AttackResult_Evade		= 0x10,		// 躲闪
	AttackResult_Accuracy	= 0x20,		// 命中
	AttackResult_Immune		= 0x40,		// 免疫
};


DESC("死亡效果类型")
ENUM DeadEffectType
{
	DESC("正常")
	DeadEffect_Normal = 0,
	DESC("击退")
	DeadEffect_Back,
	DESC("击碎")
	DeadEffect_Smash,
	DESC("爆裂")
	DeadEffect_Crack,
	DESC("火烧")
	DeadEffect_Fire,
	DESC("冰碎")
	DeadEffect_Frost,
	DESC("中毒")
	DeadEffect_Poison,
    DESC("击飞")
    DeadEffect_Fly,
    DESC("击爆")
    DeadEffect_HitBlast,
    DESC("爆飞")
    DeadEffect_BlastFly,
    DESC("火爆")
    DeadEffect_FireBlast,
    DESC("冰爆")
    DeadEffect_FrostBlast,
    DESC("毒爆")
    DeadEffect_PoisonBlast,
	DESC("最大")
	DeadEffect_Max,
};


// 死亡效果击退距离
#define DIE_EFFECT_BACK_DISTANCE 5.0f


DESC("治疗效果类型")
ENUM CureEffectType
{
	DESC("正常治疗")
	CureEffect_Normal = 0,
	DESC("吸血治疗")
	CureEffect_Drains,
    DESC("自身恢复")
    CureEffect_Self,
	DESC("最大")
	CureEffect_Max,
};