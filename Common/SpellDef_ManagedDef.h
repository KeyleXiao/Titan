/*******************************************************************
** 文件名:	SpellDef_ManagedDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	1/8/2015
** 版  本:	1.0
** 描  述:	

技能系统定义
********************************************************************/

#pragma once

#include "Entity_ManagedDef.h"
#include "DamageDef_ManagedDef.h"

namespace SPELL
{
	DESC("技能模版")
	ENUM SPELL_TYPE
	{
		DESC("投掷技能")
		SPELL_TYPE_CAST = 1,
		DESC("瞬发技能")
        SPELL_TYPE_INSTANT,
		DESC("被动技能")
		SPELL_TYPE_PASSIVE,
		DESC("吟唱技能")
		SPELL_TYPE_CHANNEL,
		DESC("二次技能")
		SPELL_TYPE_SECONDARY,
        DESC("蓄力技能")
        SPELL_TYPE_FOCO_ENERGIA,
	};

	/**
	技能释放后，创建攻击对象，攻击对象根据自己的轨迹和伤害方式，给命中的目标发送伤害消息
	攻击对象类型:
	1. 单体		【必命中】
	2. 定点范围	【圆形】
	3. 直线		【长方形】
	4. 飞行单体	【锁定】【只需要计算命中时间】
	5. 飞行直线	【圆形】【沿途的所有目标都被击中】
	6. 飞行爆炸	【圆形】【沿途不管，目的地爆炸】
	*/
	DESC("攻击类型")
	ENUM ATTACK_TYPE
	{
		DESC("瞬发单体")
		INSTANT_SINGLE = 1,
		DESC("瞬发范围")
        INSTANT_AOE,
		DESC("瞬发直线")
		INSTANT_LINE,
		DESC("飞行单体")
		FLY_SINGLE,
		DESC("飞行直线")
		FLY_LINE,
		DESC("飞行爆炸")
		FLY_BOMB,
		DESC("瞬发扇形")
		INSTANT_SECTOR,
		DESC("往返")
		ROUND_TRIP,
        DESC("范围选择")
        RANGE_SELECTION,
        DESC("随机选择")
        RANDOM_SELECTION,
        DESC("弹射")
        RANDOM_CATAPULT,
        DESC("飞行直线击退")
        FLY_LINE_REPULSE,
        DESC("范围扩散")
        AOE_DIFFUSE,
        DESC("AOE传递")
        AOE_TRANSMIT,
        DESC("结界")
        BARRIER,
        DESC("锁定飞行爆炸")
		LOCK_FLY_BOMB,
	};

	DESC("技能描画类型")
	ENUM SPELL_DRAW_TYPE
	{
        DESC("不选择")
        DRAW_TYPE_NONE                      = 0x1,
        DESC("矩形")
		DRAW_TYPE_RECT                      = 0x2,
        DESC("扇形")
		DRAW_TYPE_SECTOR                    = 0x4,
        DESC("圆形")
        DRAW_TYPE_ROUND                     = 0x8,
        DESC("以自身为中心)")
        DRAW_TYPE_SELF                      = 0x10,
        DESC("以选择区域为中心")
        DRAW_TYPE_SELECTION_REGION          = 0x20,
        DESC("描边大小取技能距离")
        DRAW_TYPE_SPELL_DISTANCE            = 0x40,
        DESC("描边大小取攻击范围")
        DRAW_TYPE_SPELL_RANGE               = 0x80,
        DESC("准备施法阶段显示")
        DRAW_TYPE_PREPARE_CAST_SHOW         = 0x100,
		DESC("将绘制区域内选中的对象描边")
		DRAW_TYPE_DRAW_OUTLINE			    = 0x200,
		DESC("用大圈绘制技能距离")
		DRAW_TYPE_DRAW_DISTANCE			    = 0x400,
		DESC("描边大小取选择范围")
		DRAW_TYPE_CHOOSE_RANGE              = 0x800,
        DESC("水平光圈提示")
        DRAW_TYPE_HORIZONTAL_APERTURE       = 0x1000,
	};

	DESC("目标过滤")
	ENUM TARGET_FILTER
	{
		DESC("所有类型")
		TARGET_ALL				            = 0x7FFFFFFF,
		DESC("自己")
		TARGET_SELF				            = 0x1,
		DESC("友方英雄(不包括自己)")
		TARGET_FRIENDLY_ROLE	            = 0x2,
		DESC("友方小兵")
		TARGET_FRIENDLY_SOLDIER             = 0x4,
		DESC("敌方英雄")
		TARGET_ENEMY_ROLE		            = 0x8,
		DESC("敌方小兵")
		TARGET_ENEMY_SOLDIER	            = 0x10,
		DESC("中立英雄")
		TARGET_NEUTRAL_ROLE		            = 0x20,
		DESC("中立小怪物(不含大小龙)")
		TARGET_NEUTRAL_LITTLE_MONSTER	    = 0x40,
		DESC("友方堡垒")
		TARGET_FRIENDLY_TOWER	            = 0x80,
		DESC("敌方堡垒")
		TARGET_ENEMY_TOWER		            = 0x100,
		DESC("中立堡垒")
		TARGET_NEUTRAL_TOWER	            = 0x200,
		DESC("可对尸体使用")
		TARGET_BODY_USE			            = 0x400,
		DESC("不可对活体使用")
		TARGET_NO_ALIVE_USE		            = 0x800,
        DESC("不可对地面上实体使用")
        TARGET_EXCLUDE_GROUND               = 0x1000,
        DESC("不可对高台上的实体使用")
        TARGET_EXCLUDE_HIGH_FLOOR           = 0x2000,
        DESC("不可对空中的实体使用")
        TARGET_EXCLUDE_SKY                  = 0x4000,
        DESC("中立小龙")
        TARGET_NEUTRAL_SMALL_DRAGON         = 0x8000,
        DESC("中立大龙")
        TARGET_NEUTRAL_HUGE_DRAGON          = 0x10000,
        DESC("不可对AI使用")
        TARGET_EXCLUDE_AI                   = 0x20000,
        DESC("己方推塔小龙")
        TARGET_FRIENDLY_SMALL_DRAGON        = 0x40000,
        DESC("敌方推塔小龙")
        TARGET_ENEMY_SMALL_DRAGON           = 0x80000,
        DESC("己方召唤物")
        TARGET_FRIENDLY_SUMMON_ENTITY      = 0x100000,
        DESC("敌方召唤物")
        TARGET_ENEMY_SUMMON_ENTITY         = 0x200000,
	};

	DESC("阵营关系")
	ENUM CampFlag
	{
		DESC("未知")
		CampFlag_Unknow = 0      ,
		DESC("自身")
		CampFlag_Self			 ,
		DESC("友方")
		CampFlag_Friend          ,
		DESC("敌方")
		CampFlag_Enemy           ,
		DESC("中立")
		CampFlag_Neutral         ,
	};

	DESC("打断标志")
	ENUM BREAK_FLAG
	{
		DESC("移动打断")
		BREAK_MOVE				= 0x1,
		DESC("被击打断")
		BREAK_HIT				= 0x2,
		DESC("死亡打断")
		BREAK_DEAD				= 0x4,
		DESC("超出距离打断")
		BREAK_DISTANCE			= 0x8,
		DESC("效果打断")
		BREAK_EFFECT			= 0x10,
		DESC("取消施法打断")
		BREAK_CANCEL_CAST		= 0x20,
		DESC("施法技能打断")
		BREAK_CAST_SPELL		= 0x40,
	};

	/////////////////////////// 冷却组定义  ////////////////////////////
	DESC("冷却组")
	ENUM EFREEZE_CLASS
	{
		DESC("技能冷却组")
		EFreeze_ClassID_Spell = 1,
		DESC("物品冷却组")
		EFreeze_ClassID_Goods,
		DESC("聊天冷却组")
		EFreeze_ClassID_Chat,
		DESC("最大冷却组")
		EFreeze_ClassID_Max,
	};

	DESC("伤害类型")
	ENUM DAMAGE_TYPE
	{
		DESC("物理伤害")
		DAMAGE_TYPE_PHYSICS					= 0x01,
		DESC("魔法伤害")
		DAMAGE_TYPE_MAGIC					= 0x02,
		DESC("免伤")
		DAMAGE_TYPE_FREE					= 0x04,
		DESC("不创建攻击对象")
		DAMAGE_TYPE_NO_CREATE_ATTACK		= 0x08,
		DESC("效果攻击")
		DAMAGE_TYPE_EFFECT					= 0x10,
		DESC("反弹攻击")
		DAMAGE_TYPE_REBOUND					= 0x20,
        DESC("额外伤害")
        DAMAGE_TYPE_APEND                   = 0x40,
		//DESC("免疫吸血")
		//DAMAGE_TYPE_IMMUNE_DRAINS           = 0x80,
		DESC("真实伤害")
		DAMAGE_TYPE_TURE_DAMAGE				= 0x100,
        DESC("特效伤害")
        DAMAGE_TYPE_SPECIAL_EFFECT_DAMAGE	= 0x200,
	};

    DESC("施法选择类型")
	ENUM CAST_SELECT_TYPE
	{
		DESC("默认类型")
		CAST_SELECT_DEFAULT = 0,
		DESC("智能施法类型")
		CAST_SELECT_SMART_CAST =1,
        DESC("选择类型")
        CAST_SELECT_SELECT =2,
		DESC("快捷施法类型")
		CAST_SELECT_SPEED_CAST =3,

	};

	// 1.技能持续时间： 用来管理技能内部流程持续的时间,技能持续的时候有些可以动有些不能动（比如盖伦的旋风斩边转边伤害）
	// 2.技能僵持时间： 释放技能过程中不能动的时间
	// 3.技能冷却时间： 控制什么时候允许放下一个技能
	// 4.技能攻击次数和攻击时间： 技能可能召唤一出个实体(可破坏),也可能创造一个持续的伤害对象（原地AOE或者飞行伤害），伤害对象自己管理生命周期，与技能持续时间无关

	// 技能数值定义枚举： 指技能逻辑中，可能被天赋或者其他系统修改的数值，这些数值可以被条件和效用使用
    // 增加枚举，需要在setSpellData里面对应增加
	DESC("技能数值定义枚举")
	ENUM SPELL_DATA_ID
	{
		DESC("技能ID")
		SPDATA_ID    = 0,
		DESC("技能职业")
		SPDATA_VOCATION,
		DESC("技能类型")
		SPDATA_TYPE,
		DESC("技能名字")
		SPDATA_NAME,
		DESC("技能描述")
		SPDATA_DESC,
		DESC("技能图标")
		SPDATA_ICON,
		DESC("预备光效")
		SPDATA_PREPARE_LIGHTING,
		DESC("施法光效")
		SPDATA_CAST_LIGHTING,
		DESC("被击光效")
		SPDATA_DAMAGE_LIGHTING,
		DESC("怪物被击光效")
		SPDATA_MONSTER_DAMAGE_LIGHTING,
		DESC("预备动画名字")
		SPDATA_PREPARE_ANIMATION,
		DESC("施法动画名字")
		SPDATA_CAST_ANIMATION,
		DESC("被击动画名字")
		SPDATA_DAMAGE_ANIMATION,
		DESC("怪物被击动画名字")
		SPDATA_MONSTER_DAMAGE_ANIMATION,
		DESC("预备音效")
		SPDATA_PREPARE_SOUND,
		DESC("施法音效")
		SPDATA_CAST_SOUND,
		DESC("被击音效")
		SPDATA_DAMAGE_SOUND,
		DESC("击中音效")
		SPDATA_HIT_SOUND,
		DESC("准备时间")
		SPDATA_PREPARE_TIME,
		DESC("打断标志")
		SPDATA_BREAK_FLAG,
		DESC("持续时间")
		SPDATA_DURATION,
		DESC("僵持时间")
		SPDATA_RIGIDITY,
		DESC("移动僵持时间")
		SPDATA_MOVE_RIGIDITY,
        DESC("转向僵持时间")
        SPDATA_TURN_RIGIDITY,
		DESC("冷却ID")
		SPDATA_COOLID,
		DESC("消耗法力")
		SPDATA_NEED_MP,
		DESC("消耗体力")
		SPDATA_NEED_STAMINA,
		DESC("消耗能量")
		SPDATA_NEED_ENERGY,
		DESC("技能威力")
		SPDATA_POWER,
        DESC("等级系数")
        SPDATA_LEVEL_FACTOR,
		DESC("攻击百分比(float)")
		SPDATA_ATTACK_PCT,
		DESC("穿透百分比(float)")
		SPDATA_PENETRATION_PCT,
		DESC("固定仇恨值")
		SPDATA_STOCK_ENMITY,
		DESC("仇恨值百分比")
		SPDATA_ENMITY_PCT,
		DESC("选择目标类型")
		SPDATA_SELECT_TARGET,
		DESC("目标类型")
		SPDATA_TARGET,
		DESC("伤害类型(魔法or物理)")
		SPDATA_DAMAGE_TYPE,
		DESC("技能效果类型")
		SPDATA_SPELL_EFFECT_TYPE,
		DESC("命中时间")
		SPDATA_HIT_TIME,
		DESC("攻击类型")
		SPDATA_ATTACK_TYPE,
		DESC("攻击次数")
		SPDATA_ATTACK_COUNT,
		DESC("攻击时间")
		SPDATA_ATTACK_TIME,
		DESC("同目标伤害次数")
		SPDATA_TARGET_DAMAGE_COUNT,
		DESC("最大攻击目标数量")
		SPDATA_TARGET_COUNT,
		// 此处约定(float)值默认扩大SPELL_FLOAT_VALUE倍
		DESC("选择范围(float)")
		SPDATA_CHOOSE_RANGE,
		DESC("攻击范围(float)")
		SPDATA_ATTACK_RANGE,
		DESC("攻击衰减(float)")
		SPDATA_ATTACK_DECLINE,
		DESC("攻击衰减上限(float)")
		SPDATA_ATTACK_DECLINE_LIMIT,
		DESC("技能距离(float)")
		SPDATA_DISTANCE,
		DESC("飞行速度(float)")
		SPDATA_FLYING_SPEED,
		DESC("使用技能高度(float)")
		SPDATA_USE_HEIGHT,
		DESC("技能攻击高度(float)")
		SPDATA_ATTACK_HEIGHT,
		DESC("死亡效果类型")
		SPDATA_DEAD_EFFECT_TYPE,
        DESC("技能默认选择类型")
        SPDATA_DEFAULT_SELECT_TYPE,
        DESC("技能智能选择类型")
        SPDATA_SMART_SELECT_TYPE,
		DESC("技能选择类型")
		SPDATA_SELECT_TYPE,
        DESC("技能快捷施法选择类型")
        SPDATA_SPEED_SELECT_TYPE,
		DESC("技能使用类型")
		SPDATA_CAST_TYPE,
        DESC("技能使用标识")
        SPDATA_CAST_FLAG,
		DESC("技能被动事件")
		SPDATA_PASSIVE_EVENT,
		DESC("技能标识")
		SPDATA_FLAG,
		DESC("技能命令优先级")
		SPDATA_COMMAND_LEVEL,
        DESC("技能描画类型")
		SPDATA_DRAW_TYPE,
	};

    // 修改此结构，要修改EffectClient_SetSingleSpellData,EffectClient_SetSingleStrSpellData,
	// EffectServer_SetSingleSpellData,EffectServer_SetSingleStrSpellData，重新导效果
	DESC("技能数值定义数据")
	STRUCT SPELL_DATA
	{
        DESC("技能ID")
        int nID;
        DESC("职业")
        VOCATION nVocation;
        DESC("技能类型")
        SPELL_TYPE nType;
        DESC("技能名称")
        sp_char_array(szName,SPELL_NAME_LEN);        
        DESC("技能描述")
        sp_char_array(szDesc,SPELL_DESC_LEN);
        DESC("技能图标")
        int nIcon;            
        DESC("预备光效")
        sp_char_array(strPrepareLighting,LIGHTING_NAME_LEN);
        DESC("施法光效")
        sp_char_array(strCastLighting,LIGHTING_NAME_LEN);
        DESC("被击光效")
        sp_char_array(strDamageLighting,LIGHTING_NAME_LEN);
        DESC("怪物被击光效")
        sp_char_array(strMonsterDamageLighting,LIGHTING_NAME_LEN);
        DESC("预备动画")
        sp_char_array(strPrepareAnimation,ANIMATION_NAME_LEN); 
        DESC("施法动画")
        sp_char_array(strCastAnimation,ANIMATION_NAME_LEN);
        DESC("被击动画")
        sp_char_array(strDamageAnimation,ANIMATION_NAME_LEN);
        DESC("怪物被击动画")
        sp_char_array(strMonsterDamageAnimation,ANIMATION_NAME_LEN);
        DESC("预备音效")
        sp_char_array(strPrepareSound,SOUND_NAME_LEN);
        DESC("施法音效")
        sp_char_array(strCastSound,SOUND_NAME_LEN);
        DESC("被击音效")
        sp_char_array(strDamageSound,SOUND_NAME_LEN);
        DESC("击中音效")
        sp_char_array(strHitSound,SOUND_NAME_LEN);
        DESC("准备时间")
        int nPrepareTime;
        DESC("打断标志")BITMASK("SPELL.BREAK_FLAG")
        int nBreakFlag;
        DESC("持续时间")
        int nDuration;
        DESC("僵持时间")
        int	nRigidity;
        DESC("移动僵持时间")
        int	nMoveRigidity;
        DESC("转向僵持时间")
        int	nTurnRigidity;
        DESC("冷却ID")
        int nCoolID; 
        DESC("消耗法力")
        int nMP;
        DESC("消耗体力")
        int nStamina;
        DESC("消耗能量")
        int	nEnergy;
        DESC("技能威力")
        int nPower;
        DESC("等级系数")
        int nLevelFactor;
        DESC("攻击百分比(float)")
        int nAttackPCT;
        DESC("穿透百分比(float)")
        int nPenetrationPCT;
        DESC("固定仇恨值")
        int nStockEnmity;
        DESC("仇恨值百分比")
        int nEnmityPCT;
        DESC("选择目标类型")
        int nSelectTargetType;
        DESC("目标类型")BITMASK("SPELL.TARGET_FILTER")
        int nTargetType;
        DESC("伤害类型")
        int nDamageType;
        DESC("技能效果类型")BITMASK("SPELL.SPELL_EFFECT_TYPE")
        int nSpellEffectType;
        DESC("命中时间")
        int nHitTime;
        DESC("攻击类型")
        ATTACK_TYPE nAttackType;
        DESC("攻击次数")
        int nAttackCount;
        DESC("攻击时间")
        int nAttackTime;
        DESC("同目标伤害次数")
        int nTargetDamageCount;
        DESC("最大攻击目标数量")
        int nTargetCount;
        DESC("选择范围(float)")
        int nChooseRange;
        DESC("攻击范围(float)")
        int nAttackRange;
        DESC("攻击衰减(float)")
        int nAttackDecline;
        DESC("攻击衰减上限(float)")
        int nAttackDeclineLimit;
        DESC("技能距离(float)")
        int nDistance;
        DESC("飞行速度(float)")
        int nFlyingSpeed;
        DESC("使用技能高度(float)")
        int nUseHeight;
        DESC("技能攻击高度(float)")
        int nAttackHeight;
        DESC("死亡效果类型")
        DeadEffectType nDeadType;
        DESC("技能默认选择类型")
        int nDefaultSelectType;
		DESC("技能智能选择类型")BITMASK("SPELL.SPELL_SELECT_TYPE")
		int nSmartSelectType;
        DESC("技能常规选择类型")BITMASK("SPELL.SPELL_SELECT_TYPE")
        int nSelectType;
        DESC("技能快捷施法选择类型")BITMASK("SPELL.SPELL_SELECT_TYPE")
        int nSpeedSelectType;
        DESC("技能使用类型") BITMASK("SPELL.SPELL_CAST_TYPE")
        int nCastType;
        DESC("技能使用标识")BITMASK("SPELL.SPELL_CAST_FLAG")
        int nCastFlag;
        DESC("技能被动事件")BITMASK("SPELL.SPELL_PASSIVE_EVENT")
        int nPassiveEvent;
        DESC("技能标识")BITMASK("SPELL.SPELL_FLAG")
        int nSpellFlag;
        DESC("技能命令优先级")
        int nSpellCommandLevel;
        DESC("技能描画类型")BITMASK("SPELL.SPELL_DRAW_TYPE")
        int nDrawType;

		STRUCT_INIT(SPELL_DATA)

#	ifndef _MANAGED
		int getInt( int nIndex )
		{
			switch(nIndex)
			{
			case SPDATA_ID: return nID;
			case SPDATA_VOCATION: return nVocation;
			case SPDATA_TYPE: return nType;
			case SPDATA_ICON: return nIcon;
			case SPDATA_PREPARE_TIME: return nPrepareTime;
			case SPDATA_BREAK_FLAG: return nBreakFlag;
			case SPDATA_DURATION: return nDuration;
			case SPDATA_RIGIDITY: return nRigidity;
			case SPDATA_MOVE_RIGIDITY: return nMoveRigidity;
            case SPDATA_TURN_RIGIDITY: return nTurnRigidity;
			case SPDATA_COOLID: return nCoolID;
			case SPDATA_NEED_MP: return nMP;
			case SPDATA_NEED_STAMINA: return nStamina;
			case SPDATA_NEED_ENERGY: return nEnergy;
			case SPDATA_POWER: return nPower;
            case SPDATA_LEVEL_FACTOR: return nLevelFactor;
			case SPDATA_ATTACK_PCT: return nAttackPCT;
			case SPDATA_PENETRATION_PCT: return nPenetrationPCT;
			case SPDATA_STOCK_ENMITY: return nStockEnmity;
			case SPDATA_ENMITY_PCT: return nEnmityPCT;
			case SPDATA_SELECT_TARGET: return nSelectTargetType;
			case SPDATA_TARGET: return nTargetType;
			case SPDATA_DAMAGE_TYPE: return nDamageType;
			case SPDATA_SPELL_EFFECT_TYPE: return nSpellEffectType;
			case SPDATA_HIT_TIME: return nHitTime;
			case SPDATA_ATTACK_TYPE: return nAttackType;
			case SPDATA_ATTACK_COUNT: return nAttackCount;
			case SPDATA_ATTACK_TIME: return nAttackTime;
			case SPDATA_TARGET_DAMAGE_COUNT: return nTargetDamageCount;
			case SPDATA_TARGET_COUNT: return nTargetCount;
			case SPDATA_CHOOSE_RANGE: return nChooseRange;
			case SPDATA_ATTACK_RANGE: return nAttackRange;
			case SPDATA_ATTACK_DECLINE: return nAttackDecline;
			case SPDATA_ATTACK_DECLINE_LIMIT: return nAttackDeclineLimit;
			case SPDATA_DISTANCE: return nDistance;
			case SPDATA_FLYING_SPEED: return nFlyingSpeed;
			case SPDATA_USE_HEIGHT: return nUseHeight;
			case SPDATA_ATTACK_HEIGHT: return nAttackHeight;
			case SPDATA_DEAD_EFFECT_TYPE: return nDeadType;
            case SPDATA_DEFAULT_SELECT_TYPE: return nDefaultSelectType;
			case SPDATA_SELECT_TYPE: return nSelectType;
			case SPDATA_SMART_SELECT_TYPE: return nSmartSelectType;
            case SPDATA_SPEED_SELECT_TYPE: return nSpeedSelectType;
			case SPDATA_CAST_TYPE: return nCastType;
            case SPDATA_CAST_FLAG: return nCastFlag;
			case SPDATA_PASSIVE_EVENT: return nPassiveEvent;
			case SPDATA_FLAG: return nSpellFlag;
			case SPDATA_COMMAND_LEVEL: return nSpellCommandLevel;
            case SPDATA_DRAW_TYPE: return nDrawType;
			}
			return 0;
		}

		const char * getStr( int nIndex )
		{
			switch( nIndex )
			{
			case SPDATA_NAME: return szName;
			case SPDATA_DESC: return szDesc;
			case SPDATA_PREPARE_LIGHTING: return strPrepareLighting;
			case SPDATA_CAST_LIGHTING: return strCastLighting;
			case SPDATA_DAMAGE_LIGHTING: return strDamageLighting;
			case SPDATA_MONSTER_DAMAGE_LIGHTING: return strMonsterDamageLighting;
			case SPDATA_PREPARE_ANIMATION: return strPrepareAnimation;
			case SPDATA_CAST_ANIMATION: return strCastAnimation;
			case SPDATA_DAMAGE_ANIMATION: return strDamageAnimation;
			case SPDATA_MONSTER_DAMAGE_ANIMATION: return strMonsterDamageAnimation;
            case SPDATA_PREPARE_SOUND: return strPrepareSound;
            case SPDATA_CAST_SOUND: return strCastSound;
            case SPDATA_DAMAGE_SOUND: return strDamageSound;
            case SPDATA_HIT_SOUND: return strHitSound;
			}
			return 0;
		}
#endif
	};

	DESC("天赋类型")
	ENUM TALENT_TYPE
	{
		DESC("天赋类型,添加整数")
		TALENT_ADD_INT = 0,
		DESC("天赋类型,设置整数")
		TALENT_SET_INT,
		DESC("天赋类型,设置字符串")
		TALENT_STRING,
	};

	DESC("技能事件")
	ENUM SPELL_EVENT
	{
		DESC("准备施法")
		SPELL_EVENT_PREPARE = 1,
		DESC("技能丢出去")
		SPELL_EVENT_CASTING,
		DESC("打中了人")
		SPELL_EVENT_HIT,
		DESC("收招结束")
		SPELL_EVENT_FINISH,
		DESC("定时事件")
		SPELL_EVENT_TIMER,
		DESC("被打断事件")
		SPELL_EVENT_BREAK,
		DESC("二次激活技能事件")
		SPELL_EVENT_SECONDARY,
		DESC("被动定时器事件")
		SPELL_EVENT_PASSIVE_TIMER,
		DESC("实体死亡事件")
		SPELL_EVENT_ENTITY_DIE,
		DESC("技能数据准备事件")
		SPELL_EVENT_DATA_PREPARE,
		DESC("技能学习事件")
		SPELL_EVENT_LEARN,
		DESC("技能范围选择事件")
		SPELL_EVENT_RANGE_SELECTION,
        DESC("技能吟唱")
        SPELL_EVENT_CHANNEL,
        DESC("打中目标-技能施法者处理事件")
        SPELL_EVENT_HIT_TARGET,
	};

	DESC("技能效果类型")
	ENUM SPELL_EFFECT_TYPE
	{
		DESC("所有类型")
		SpellEffectType_ALL					= 0x7FFFFFFF,
		DESC("远程")
		SpellEffectType_LongRange			= 0x1,
		DESC("近战")
		SpellEffectType_Infighting			= 0x2,
		DESC("普通攻击")
		SpellEffectType_NormalAttack		= 0x4,
		DESC("技能攻击")
		SpellEffectType_SpellAttack			= 0x8,
		DESC("物理攻击")
		SpellEffectType_PhysicalAttack		= 0x10,
		DESC("法术攻击")
		SpellEffectType_MagicAttack			= 0x20,
		DESC("真实伤害")
		SpellEffectType_TrueDamage			= 0x40,
		DESC("有益技能")
		SpellEffectType_UsefulSpell			= 0x80,
		DESC("有害技能")
		SpellEffectType_HarmfulSpell		= 0x100,
        DESC("特效攻击")
        SpellEffectType_SpecialEffect		= 0x200,
        DESC("无属性攻击")
        SpellEffectType_NoAttributeAttack	= 0x400,
        DESC("受普攻系数影响")
        SpellEffectType_AttackFactor	    = 0x800,
        DESC("翻滚")
        SpellEffectType_Roll	            = 0x1000,
        DESC("冲锋")
        SpellEffectType_Charge	            = 0x2000,
        DESC("可触发普攻吸血")
        SpellEffectType_AttackDrains	    = 0x4000,
        DESC("可触发技能吸血")
        SpellEffectType_SpellDrains	        = 0x8000,
        DESC("群体性伤害")
        SpellEffectType_GroupDamage	        = 0x10000,
        DESC("单体性伤害")
        SpellEffectType_SingleDamage	    = 0x20000,
        DESC("持续性伤害")
        SpellEffectType_ContinueDamage	    = 0x40000,
        DESC("XP技能")
        SpellEffectType_XP	                = 0x80000,
        DESC("响应NPC")
        SpellEffectType_TouchNPC	        = 0x100000,
        DESC("受攻击速度影响")
        SpellEffectType_PASDFactor          = 0x200000,
	};

	DESC("技能选择类型")
	ENUM SPELL_SELECT_TYPE
	{
		DESC("不需要选择")
		SpellSelectType_NoSelect				= 0x1,
		DESC("按鼠标位置选择")
		SpellSelectType_SelectMousePos			= 0x2,
		DESC("按面向方向选择")
		SpellSelectType_SelectFaceDirection		= 0x4,
		DESC("按前进方向选择")
		SpellSelectType_SelectWalkDirection		= 0x8,
		DESC("按准心位置选择")
		SpellSelectType_SelectAimPos			= 0x10,
		DESC("取选择方向的右向")
		SpellSelectType_SelectRightDirection	= 0x20,
		DESC("模糊选择，吸附到实体")
		SpellSelectType_SnapToObject			= 0x40,
		DESC("投影到地面")
		SpellSelectType_ProjectToGround			= 0x80,
		DESC("穿越物体拾取地面或建筑")
		SpellSelectType_CrossObject				= 0x100,
		DESC("需要左键确认")
		SpellSelectType_MouseClick				= 0x200,
		DESC("英雄降低到和小兵一个级别")
		SpellSelectType_DownGradeHero			= 0x400,
		DESC("塔降低到和小兵一个级别")
		SpellSelectType_DownGradeTower			= 0x800,
		DESC("考虑地面阻挡")
		SpellSelectType_EnableGroundBlock		= 0x1000,
		DESC("选择高度")
		SpellSelectType_SelectHeight			= 0x2000,
		DESC("按技能距离取点")
		SpellSelectType_SelectDistance			= 0x4000,
        DESC("按技能小地图取点")
        SpellSelectType_MiniMapPoint			= 0x8000,
		DESC("再按一次技能快捷键确认")
		SpellSelectType_KeyConfirm				= 0x10000,
		DESC("按前进方向为0时选择按键方向")
		SpellSelectType_MoveDirDefaultRawInputDir	= 0x20000,
		DESC("考虑结界阻挡")
		SpellSelectType_EnableWardBlock			= 0x40000,
		DESC("投影到地面时不考虑建筑物")
		SpellSelectType_ProjectToGroundSkipBuilding = 0x80000,
		DESC("取移动方向时，移动速度为0，默认向后")
		SpellSelectType_MoveDirDefaultBack          = 0x100000,
		DESC("选取技能时英雄坐标贴地")
		SpellSelectType_HeroProjectToFloor          = 0x200000,
        DESC("取点时检测建筑层")
        SpellSelectType_CheckBuildingLayer          = 0x400000, 
    	DESC("松开按键施法")
	    SpellSelectType_SkillUsePressUp             = 0x800000,
		DESC("不点击时延迟2秒施法")
		SpellSelectType_SkillUseDelay               = 0x1000000,
        DESC("施法前重设目标坐标")
        SpellSelectType_ResetTargetPosition         = 0x2000000,
		DESC("模糊选择，允许选择小兵")
		SpellSelectType_SnapToObject_AllowSoldier = 0x4000000, 
	};

	DESC("技能使用类型")
	ENUM SPELL_CAST_TYPE
	{
		DESC("使用技能瞬间不转向目标点")
		SpellCastType_NoTurn					= 0x1,
		DESC("使用技能可移动")
		SpellCastType_Removable					= 0x2,
		DESC("禁止移动时可转向。若此项为0，则在禁止移动的时间段内不可转向")
		SpellCastType_TurnCamera				= 0x4,
		DESC("使用技能攻击速度影响技能冷却")
		SpellCastType_PASDEffectCoolTime		= 0x8,
		DESC("持续时间到结束技能")
		SpellCastType_DurationTimeToEndSpell	= 0x10,
		DESC("技能不使用攻击对象坐标")
		SpellCastType_NoUseAttackTile			= 0x20,
		DESC("技能打中进入技能结束")
		SpellCastType_HitToFinish				= 0x40,
		DESC("技能正向抵挡")
		SpellCastType_PositiveResist			= 0x80,
		DESC("技能不显示读条")
		SpellCastType_NoProgressBar				= 0x100,
		DESC("技能不触发暴击")
		SpellCastType_NoCRC						= 0x200,
		DESC("技能无目标时单体变成直线使用")
		SpellCastType_NoTargetSingToLine		= 0x400,
		DESC("技能使用可跳跃")
		SpellCastType_Jump						= 0x800,
		DESC("技能使用客户端不检测冷却")
		SpellCastType_ClientNotCheckCool		= 0x1000,
		DESC("技能使用不检测僵直")
		SpellCastType_NoCheckRigidity			= 0x2000,
		DESC("技能使用自身坐标")
		SpellCastType_SelfLocation				= 0x4000,
		DESC("技能使用改变模式")
		SpellCastType_ChangeOperation			= 0x8000,
		DESC("技能使用寻路")
		SpellCastType_Pathing					= 0x10000,
		DESC("技能使用排除目标")
		SpellCastType_ExcludeTarget				= 0x20000,
		DESC("技能使用不飘字提示")
		SpellCastType_NoText					= 0x40000,
		DESC("技能使用打断进度条")
		SpellCastType_BreakProgressBar			= 0x80000,
		DESC("技能使用检查阻碍")
		SpellCastType_CheckHinder				= 0x100000,
		DESC("技能使用受冷却缩减")
		SpellCastType_CoolReduce				= 0x200000,
        DESC("使用实体面向点")
        SpellCastType_FaceLocation	            = 0x400000,
        DESC("不可使用其他技能")
        SpellCastType_NotUsedOtherSpell	        = 0x800000,
		DESC("使用时镜头自动转向攻击对象")
		SpellCastType_RotateCameraToTarget      = 0x1000000,
		DESC("使用时Mode2下不转身")
		SpellCastType_PauseSkillRotate			= 0x2000000,
        DESC("使用时重现取最新坐标和目标")
        SpellCastType_GetNewTarget			    = 0x4000000,
        DESC("有目标时变成锁定飞行爆炸")
        SpellCastType_HasTargetToLockFlyBomb	= 0x8000000,
        DESC("击中不关闭光效")
        SpellCastType_HitNotCloseLight		    = 0x10000000,
	};

    DESC("技能使用标识")
    ENUM SPELL_CAST_FLAG
    {
        DESC("最大蓄力使用")
        SpellCastFlag_MaxFocoEnergiaUse		    = 0x1,
        DESC("持续蓄力")
        SpellCastFlag_ContinueFocoEnergia		= 0x2,
        DESC("持续蓄力取消使用")
        SpellCastFlag_ContinueFocoEnergiaCancel	= 0x4,
        DESC("攻击不停止移动")
        SpellCastFlag_AttackNoStopMove	        = 0x8,
        DESC("攻击效果检测障碍")
        SpellCastFlag_CheckObstacle	            = 0x10,
        DESC("扫射状态")
        SpellCastFlag_StarfeState               = 0x20,
        DESC("死亡击退选取技能点")
        SpellCastFlag_DieEffectChooseTargetTile = 0x40,
        DESC("技能使用排除隐形目标")
        SpellCastType_ExcludeStealth			= 0x80,
        DESC("技能使用排除不可选择目标")
        SpellCastType_ExcludeNoSelect			= 0x100,
        DESC("技能使用鼠标松开取消技能")
        SpellCastType_MouseUpCancelSpell		= 0x200,
        DESC("打断技能增加冷却时间")
        SpellCastType_BreakSpellAddCoolTime		= 0x400,
        DESC("飞行单体击中")
        SpellCastType_FlySingHit		        = 0x800,
        DESC("增加普攻僵直时间")
        SpellCastType_AddRigidityTime		    = 0x1000,
        DESC("技能广播所有玩家")
        SpellCastType_ToAllPlayer		        = 0x2000,
        DESC("施法转向-扫射需要转向,不然别人看到的方向有问题")
        SpellCastType_TurnTo		            = 0x4000,
        DESC("圆环结界")
        SpellCastFlag_CircleBarrier             = 0x8000,
        DESC("直线结界")
        SpellCastFlag_LineBarrier               = 0x10000,
        DESC("技能到目标点时技能结束")
        SpellCastFlag_ToTargetPosFinish			= 0x20000,
		DESC("弹射目标随机")
		SPellCastFlag_RandCatapultTarget		= 0x40000,
        DESC("弹射结束时收招")
        SPellCastFlag_SpellFinishOnCatapultEnd  = 0x80000,
        DESC("死亡不打断")
        SPellCastFlag_NotDieBreak               = 0x100000,
    };

	DESC("技能被动事件")
	ENUM SPELL_PASSIVE_EVENT
	{
		DESC("被动服务器定时事件")
		SpellPassiveEvent_Timer					= 0x1,
		DESC("技能标志位更新")
		SpellPassiveEvente_FlagUpdate			= 0x2,
		DESC("被动客户端定时事件")
		SpellPassiveEvent_ClientTimer			= 0x4,
		DESC("实体死亡事件")
		SpellPassiveEvent_EntityDie				= 0x8,
		DESC("不可主动使用")
		SpellPassiveEvent_NotActiveUser			= 0x10,
	};

	DESC("技能标识")
	ENUM SPELL_FLAG
	{
		DESC("大招")
		SpellFlag_Space							= 0x1,
		DESC("怪物闪白效果")
		SpellFlag_MonsterWhiteEffect			= 0x2,
		DESC("暴露角色")
		SpellFlag_ExposeRole					= 0x4,
		DESC("可对死亡实体使用")
		SpellFlag_UseDieEntity					= 0x8,
        DESC("累积技能")
        SpellFlag_Overlay					    = 0x10,
        DESC("技能结束释放攻击对象")
        SpellFlag_CloseAttackSerial			    = 0x20,
        DESC("循环技能")
        SpellFlag_LoopSpell			            = 0x40,
        DESC("使用技能翅膀不降落")
        SpellFlag_WingNoLand			        = 0x80,
        DESC("翻滚技能")
        SpellFlag_Roll			                = 0x100,
        DESC("连招标识")
        SpellFlag_Combo			                = 0x200,
        DESC("可被被动位移打断")
        SpellFlag_BreakableByPassiveShift      = 0x400,
        DESC("障碍阻挡类型:阻挡移动")
        SpellFlag_BarrierBlockMove              = 0x800,
        DESC("客户端先行")
        SpellFlag_ClientFirst                  = 0x1000,
        DESC("障碍阻挡类型:阻挡技能-暂时无效")
        SpellFlag_BarrierBlockSpell            = 0x2000,
		DESC("优先攻击英雄(弹射技能)")
		SpellFlag_HeroFirst_Catapult		   = 0x4000,
        DESC("扫射技能")
        SpellFlag_Strafe                       = 0x8000,
        DESC("广播受击实体位置") 
        SpellFlag_SyncBeHitEntityPos           = 0x10000,
	};
}