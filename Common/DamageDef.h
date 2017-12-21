/*******************************************************************
** 文件名:	DamageDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	石亮
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			伤害定义文件
********************************************************************/

#pragma once

#include "NetMessageDef.h"
#include "DamageDef_ManagedDef.h"
#include "EventRefBase.h"
#include "SpellDef_ManagedDef.h"
using namespace SPELL;

// 消息码定义键值
enum DAMAGE_NET_MESSAGE
{
	DAMAGE_MSG_DAMAGE = 0,		    // 伤害消息
	DAMAGE_MSG_CURE,				// 治疗消息
};


// 伤害消息(技能模块发给伤害计算模块)
struct msg_entity_damage
{
	int		nID;					// 技能ID
	UID     uidOperator;			// 操作者UID
	int     nDamageType;			// 伤害类型(DAMAGE_TYPE)
	int		nAttack;				// 攻击力
	int		nPenetration;			// 穿透
	int		nPenetrationPCT;		// 穿透百分比
	int		nSpellPower;			// 技能威力
    int     nLevel;                 // 玩家等级
    int     nLevelFactor;           // 等级系数
	int		nSpellAttackPCT;		// 技能攻击百分比
	int		nSpellPenetrationPCT;	// 技能穿透百分比
	int		nCRD;					// 暴击伤害
	int		nAttackResultType;		// 攻击结果类型(EAttackResultType)
	DWORD	dwAttackSerialID;		// 攻击对象序号
	int		nCastType;				// 技能使用类型
    int     nCastFlag;              // 技能使用标识
	int		nStockEnmity;			// 固定仇恨值
	int		nEnmityPCT;				// 仇恨值百分比
	DeadEffectType nDeadType;		// 死亡效果类型
	Vector3	vStartTile;				// 技能对象起点
	Vector3	vTargetTile;			// 技能对象当前点
	int		nSpellEffectType;		// 技能效果类型
	int		nDamageHP;				// 伤害血量
	int		nAttackCount;			// 攻击目标数量，第几个目标(0表示首个目标)
	float	fAttackDecline;			// 攻击衰减
	float	fAttackDeclineLimit;	// 攻击衰减上限
	int		nSpellFlag;				// 技能标识
	int		nDamageLimitType;		// 伤害上限类型
	int		nDamageLimit;			// 伤害上限
    int     nDamageBonus;           // 伤害加成(固定值)
    float   fDamageBonusPCT;        // 伤害加成百分比
    bool	bRebound;			    // 是否反弹伤害
    float   fAttackFactor;          // 普攻系数
    float   fPromoteFactor;         // 提升系数
    int     nPASD;                  // 攻击速度
    float   fAppendPCTPDP;          // 附加额外百分比护甲穿透
    float   fAppendPCTPMP;          // 附加额外百分比魔抗穿透

	msg_entity_damage()
	{
		memset(this, 0, sizeof(*this));
		nAttackResultType = AttackResult_Damage;
	}
};


/// 伤害计算上下文(大部分参数来自于技能脚本)
struct DamageCalcContext : public event_base
{
	int			nID;						/// 技能ID
	UID			uidOperator;				/// 操作者(填充)(有些召唤物的操作者替换成了主人)
    UID         uidRealOperator;            /// 真实的操作者（可以是召唤物）
	UID			uidTarget;					/// 伤害承受者(填充)
	int			nDamageType;				/// 伤害类型(DAMAGE_TYPE)
	int			nAttack;					/// 实体攻击
	int			nPenetration;				/// 穿透
	int			nPenetrationPCT;			/// 穿透百分比
	int			nSpellPower;				/// 技能威力
    int         nLevel;                     /// 玩家等级
    int         nLevelFactor;               /// 等级系数
	int			nSpellAttackPCT;			/// 技能攻击百分比
	int			nSpellPenetrationPCT;		/// 技能穿透百分比
	int			nSpellImm;					/// 伤害减免百分比
	int			nCRD;						/// 暴击伤害
	int			nDefend;					/// 防御力
	int			nDamageHP;					/// 计算出的HP伤害值(计算)
	int			nDamageMP;					/// 计算出的MP伤害值(计算，由Buff转换)
	int			nAttackResultType;			/// 攻击结果类型(EAttackResultType)
	DWORD		dwAttackSerialID;			/// 攻击对象序号
	int			nCastType;					/// 使用类型
    int         nCastFlag;                  /// 使用标识
	int			nStockEnmity;				/// 固定仇恨值
	int			nEnmityPCT;					/// 仇恨系数百分比(技能填充)
	DeadEffectType nDeadType;				/// 死亡效果类型
	Vector3		vStartTile;					/// 技能对象起点
	Vector3		vTargetTile;				/// 技能对象当前点
	int			nSpellEffectType;			/// 技能效果类型
	int			nAttackCount;				/// 攻击目标数量，第几个目标(0表示首个目标)
	bool		bImmuneDrains;				/// 免疫吸血
	float		fAttackDecline;				/// 攻击衰减
	float		fAttackDeclineLimit;		/// 攻击衰减上限
	int			nInitialDamageHP;			/// 初始伤害,未经过效果改变的伤害值
	int			nSpellFlag;					/// 技能标识
	int			nDamageLimitType;			/// 伤害上限类型
	int			nDamageLimit;				/// 伤害上限
    int         nDamageBonus;               /// 伤害加成(固定值)
    float       fDamageBonusPCT;            /// 伤害加成百分比
    bool		bRebound;			        /// 是否反弹伤害
    float       fAttackFactor;              /// 普攻系数
    float       fPromoteFactor;             /// 提升系数
    int         nPASD;                      /// 攻速

	DamageCalcContext()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-客户端 伤害数据
struct SDamageData_SC
{
	UID					uidOperator;	/// 操作者的UID
	UID					uidTarget;		/// 目标对象UID
	int					nID;			/// 技能ID

	int					nDeltaHP;		/// 要增减的HP值
	int					nEqualHP;		/// 增减后的HP值

	int					nDeltaMP;		/// 要增减的MP值
	int					nEqualMP;		/// 增减后的MP值

	int					nAttackResultType;		/// 攻击结果类型(EAttackResultType)
	DeadEffectType		nDeadType;				/// 死亡类型(DeadEffectType)

	int					nMask;				/// 实体标志位
	bool				bDead;				/// 是否死亡
	Vector3				vTargetEndLoc;		/// 目标结束坐标
	int					nSpellEffectType;	/// 技能效果类型
	int					nSpellFlag;			/// 技能标识
    Vector3             vTurnTargetTile;    /// 转向目标点

	SDamageData_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 治疗消息
struct msg_entity_cure
{
	UID		uidOperator;			/// 操作者的UID
	UID		uidTarget;				/// 目标对象UID
	int		nCureHP;				/// 治疗血量
	int		nCureMP;				/// 治疗魔法
	int		nCureEffectType;		/// 治疗类型CureEffectType
    int     nSpellEffectType;       /// 技能效果类型

	msg_entity_cure(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


/// 治疗上下文
struct CureContext : public event_base
{
	UID		uidOperator;			/// 操作者
	UID		uidTarget;				/// 目标对象UID
	int		nCureHP;				/// 治疗血量
	int		nCureMP;				/// 治疗魔法
	int		enmity;					/// 该次治疗对附近怪物产生的仇恨值
	int		nCureEffectType;		/// 治疗类型CureEffectType
    int     nSpellEffectType;       /// 技能效果类型

	CureContext()
	{
		memset(this, 0, sizeof(*this));
	}
};

/// 增加护盾
struct AddShield : public event_base
{
    float     fValue;                 /// 值

    AddShield()
    {
        memset(this, 0, sizeof(*this));
    }
};