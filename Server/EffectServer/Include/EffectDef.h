/*******************************************************************
** 文件名:	EffectDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - All Rights Reserved
** 创建人:	penglu
** 日  期:	2014-12-24
** 版  本:	1.0
** 描  述:	

			效果模块公用定义
********************************************************************/

#pragma once

#include "ManagedCode.h"
#include "SpellDef.h"
#include "Slot_ManagedDef.h"
#include "EffectCommon_ManagedDef.h"
#include "War_ManagedDef.h "
using namespace SPELL;
using namespace War;

namespace EFFECT_SERVER
{
	DESC("持续设置属性-血量,魔法,体力,能量等")
	STRUCT EffectServer_ContinuedSetProperty
	{
		int nID;

		DESC("数值类型")
		MA_VALUE_TYPE nValueType;

		DESC("属性类型")
		MA_PROPERTY_TYPE nPropertyType;

		DESC("使用对象")
		MA_TAGET_TYPE nTagetType;

		DESC("间隔，毫秒")
		int nInterval;

		DESC("每次增加数值;固定值,百分比扩大100倍")
		int nAddValue;

		DESC("次数")
		int nCount;

		DESC("是否有首次调用")
		int	nFirst;

		STRUCT_INIT(EffectServer_ContinuedSetProperty)
	};

	DESC("设置属性-血量,魔法,体力,能量等")
	STRUCT EffectServer_SetProperty
	{
		int nID;

		DESC("属性类型")
		MA_PROPERTY_TYPE nPropertyType;

		DESC("数值")
		int nValue;

		STRUCT_INIT(EffectServer_SetProperty)
	};

	DESC("定身")
	STRUCT EffectServer_Root
	{
		int nID;

		DESC("暂时无用,通过buff时间控制定身时间")
		int nType;

		STRUCT_INIT(EffectServer_Root)
	};

	DESC("冷却")
	STRUCT EffectServer_CoolDown
	{
		int nID;

		DESC("冷却组ID")
		EFREEZE_CLASS nClassID;

		DESC("冷却ID")
		int nCoolDownID;

		STRUCT_INIT(EffectServer_CoolDown)
	};

	DESC("增加Buff")
	STRUCT EffectServer_AddBuff
	{
		int nID;

		DESC("BuffID")
		int nBuffID;

		DESC("BuffLevel")
		int nBuffLevel;

		DESC("目标类型")
		MA_TAGET_TYPE nTargetType;

		DESC("BUFF类型")
		BUFF_TYPE nType;

		DESC("增加目标类型")
		MA_TAGET_TYPE nAddTargetType;

        DESC("直接加BUFF")
        MA_BOOL nDirectAdd;

        DESC("目标类型过滤")
        int nTargetTypeFilter;

		STRUCT_INIT(EffectServer_AddBuff)
	};

	DESC("设置技能标志")
	STRUCT EffectServer_SetSpellFlag
	{
		int nID;

		DESC("设置技能标志类型")
		SPELLFLAG_TYPE nType;

		DESC("设置技能标志位某位置索引")
		int nIndex;

		DESC("数值")
		int nValue;

		DESC("增加数值时最终数值最大值")
		int nMaxValue;

        DESC("设置技能ID,0时是调用技能")
        int nSpellID;

		STRUCT_INIT(EffectServer_SetSpellFlag)
	};

	//=================================================================================
	DESC("改变位置")
	STRUCT EffectServer_TransLocation
	{
		int nID;

		DESC("改变位置类型")
		TRANSLOCATION_TYPE nType;

		DESC("是否传送")
		MA_BOOL nTransPort;

		STRUCT_INIT(EffectServer_TransLocation)
	};

	DESC("切换连招")
	STRUCT EffectServer_SpellCombo
	{
		int nID;

		DESC("连招个数")
		int nComboCount;

		DESC("连招ID列表")
		sp_char_array(strComboID,64);

		DESC("连招重置时间列表")
		sp_char_array(strResetTime,64);

		STRUCT_INIT(EffectServer_SpellCombo)
	};

	DESC("冷却相关")
	STRUCT EffectServer_Freeze
	{
		int nID;

		DESC("冷却类型")
		FREEZE_TYPE	nType;

		DESC("冷却组ID")
		EFREEZE_CLASS nClassID;

		DESC("冷却ID")
		int	dwFreezeID;

		DESC("增加冷却时间,为百分比时扩大100倍")
		int nAddTime;

        DESC("效果结束减掉增加时间")
        MA_BOOL nSubtractAddTime;

        DESC("是否受冷却缩减")
        MA_BOOL nCoolReduce;

		STRUCT_INIT(EffectServer_Freeze)
	};

	DESC("定时技能累积")
	STRUCT EffectServer_TimerSpellOverlay
	{
		int nID;

		DESC("技能累积类型")
		SPELLOVERLAY_TYPE nType;

		DESC("上限值")
		int nUpperLimit;

		DESC("下限值")
		int nLowerLimit;

        DESC("槽位类型")
        SLOT_TYPE nSlotType;

        DESC("槽位索引")
        int nSlotIndex;

        DESC("冷却组ID")
        EFREEZE_CLASS nClassID;

        DESC("冷却ID")
        int	dwFreezeID;

        DESC("是否受冷却缩减影响")
        MA_BOOL	isActiveByFreeze;

		STRUCT_INIT(EffectServer_TimerSpellOverlay)
	};	

	DESC("改变实体属性")
	STRUCT EffectServer_ChangeProperty 
	{
		int nID;

		DESC("属性类型")
		ENTITY_PROPERTY_ nType;

		DESC("增减/重设")
		MA_CHANGE_TYPE nChangeType;

		DESC("属性值")
		int nVal;

		DESC("是否有属性改变事件")
		MA_BOOL nEvent;

		STRUCT_INIT(EffectServer_ChangeProperty)
	};

	DESC("持续改变实体属性")
	STRUCT EffectServer_ContinuedChangeProperty
	{
		int nID;

		DESC("属性类型")
		ENTITY_PROPERTY_CONTINUED nType;

		DESC("基础增加值")
		int nBaseVal;

		DESC("每次增加数值")
		int nAddValue;

		DESC("间隔，毫秒")
		int nInterval;

		DESC("次数")
		int nCount;

		DESC("是否有属性改变事件")
		MA_BOOL nEvent;

		STRUCT_INIT(EffectServer_ContinuedChangeProperty)
	};

	DESC("封魔")
	STRUCT EffectServer_Silence
	{
		int nID;

		DESC("技能效果类型SPELL_EFFECT_TYPE")
		int	nSpellEffectType;

        DESC("是否打断读条正在放技能")
        MA_BOOL nBreak;

		STRUCT_INIT(EffectServer_Silence)
	};

	DESC("免疫(无敌)")
	STRUCT EffectServer_Immune
	{
		int nID;

		DESC("暂时无用,通过buff时间控制")
		int nType;

		STRUCT_INIT(EffectServer_Immune)
	};

	
	DESC("抵挡伤害")
	STRUCT EffectServer_ResistDamage
	{
		int nID;

		DESC("抵挡伤害类型")
		RESISTDAMAGE_TYPE nType;

		DESC("系数(float)")
		float fValue;

		DESC("固定值")
		int nFixedValue;

		DESC("抵挡后增加buffID,为0不加buff,不为0加buff")
		int nBuffID;

		DESC("抵挡后增加buffLevel")
		int nBuffLvel;

		DESC("伤害类型DAMAGE_TYPE")
		int nDamageType;

        DESC("是否有增加护盾事件")
        MA_BOOL nAddShieldEvent;

        DESC("等级系数(扩大100倍)")
        int	nLevelFactor;

        DESC("使用对象")
        MA_TAGET_TYPE nTagetType;

		STRUCT_INIT(EffectServer_ResistDamage)
	};

	DESC("创建怪物")
	STRUCT EffectServer_CreateMonster
	{
		int nID;

		DESC("怪物点类型")
		MONSTER_POS_TYPE nType;

		DESC("刷怪ID")
        sp_char_array(nMonsterID, 256);

		DESC("刷怪数量")
		int	nBuildNum;

		DESC("刷怪坐标x(float)")
		float fPosx;

		DESC("刷怪坐标y(float)")
		float fPosy;

		DESC("刷坐标z(float)")
		float fPosz;

		DESC("技能方向")
		SPELL_DIR_TYPE nDirType;

		DESC("方向距离(float)")
		float fDistance;
		
		DESC("怪物角度(float)")
		float fAngle;

        DESC("使用对象")
        MA_TAGET_TYPE nTagetType;

        DESC("基础等级")
        int nBaseLevel;

        DESC("影响属性")
        EXTRAATTACK_TYPE nExtraAttackType;

        DESC("攻击百分比，扩大100倍")
        int	nSpellAttackPCT;

        DESC("阵营ID(-1表示召唤者阵营)")
        int nCampID;

		STRUCT_INIT(EffectServer_CreateMonster)
	};

	DESC("使用技能")
	STRUCT EffectServer_UseSpell
	{
		int nID;

		DESC("技能ID")
		int nSpellID;

		DESC("使用技能点类型")
		USE_SPELL_POS_TYPE nUseSpellPosType;

        DESC("是否使用方向计算点")
        MA_BOOL nUseDirPos;

        DESC("方向")
        DIRECTION_TYPE nDirectionType;

        DESC("方向距离(float)")
        float fDistance;

		STRUCT_INIT(EffectServer_UseSpell)
	};

	DESC("冲锋,击退，击飞,撞飞等")
	STRUCT EffectServer_BeatBack
	{
		int nID;

		DESC("速度(float)")
		float fSpeed;

		DESC("距离(float)")
		float fDistance;

		DESC("高度(float)")
		float fHeight;

        DESC("最大时间(毫秒)")
        int nMaxTime;

		DESC("移动类型")
		MOVE_TYPE nMoveType;

		DESC("目标类型")
		MOVE_LOC_TYPE nType;
		
        DESC("移动方向")
        MOVE_DIR_TYPE nMoveDir;

		DESC("离目标点距离(float)")
		float fLocationDistance;

		DESC("冲锋打断距离,0不打断(float)")
		float fBreakDistance;

		DESC("锁定目标")
		MA_BOOL nLockTarget;

		DESC("操作者对象")
		BEAT_BACK_TARGET_TYPE nOperatorType;

		DESC("目标对象")
		BEAT_BACK_TARGET_TYPE nTargetType;

		DESC("锁定目标时间,超过时间打断(毫秒)")
		int nLockTime;

        DESC("被动位移类型")
        PASSIVE_MOVE_TYPE nPassiveMoveType;

        DESC("冲锋优先级")
        int nPriority;

        DESC("冲锋最大仰角(度)")
        int nMaxUpAngle;

        DESC("冲锋是否可以跨越障碍")
        MA_BOOL bCanCrossObstacle;

        DESC("冲锋遇到障碍加buffID")
        int nHitObstacleAddBuffID;

        DESC("冲锋遇到障碍加buff等级")
        int nHitObstacleAddBuffLevel;

        DESC("是否是抛物线")
        MA_BOOL bUseHeight;

        DESC("冲锋完加buffID,默认填0")
        int nFinishBuff_ID;

        DESC("冲锋完buffLevel")
        int nFinishBuff_Level;

        DESC("冲锋完加buff目标对象")
        BEAT_BACK_TARGET_TYPE nFinishBuff_Target;

        DESC("冲锋完buff:buff添加者")
        BEAT_BACK_TARGET_TYPE nFinishBuff_Operator;

		STRUCT_INIT(EffectServer_BeatBack)
	};

	DESC("自杀")
	STRUCT EffectServer_KillSelf
	{
		int nID;

		DESC("延迟自杀时间（ms）")
		int nDelayTime;

        DESC("自杀操作者类型-造成伤害者")
        MA_TAGET_TYPE nOperatorType;

        DESC("自杀目标类型-死亡者")
        MA_TAGET_TYPE nTargetType;

        DESC("伤害类型DAMAGE_TYPE")
        int nDamageType;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int	nSpellEffectType;

        DESC("死亡效果类型")
        DeadEffectType nDeadType;

		STRUCT_INIT(EffectServer_KillSelf)
	};

	DESC("免疫buff")
	STRUCT EffectServer_ImmuneBuff
	{
		int nID;

		DESC("免疫buff类型")
		int nBuffType;

		DESC("免疫概率(1~10000)")
		int nPer;

        DESC("免疫成功给自己加buff")
        int nBuffID;

		STRUCT_INIT(EffectServer_ImmuneBuff)
	};

	DESC("增加范围buff")
	STRUCT EffectServer_AddRangeBuff
	{
		int nID;

		DESC("加buff次数，小于等于0无限次")
		int	nCount;

		DESC("加buff间隔时间")
		int	nTime;

		DESC("加buff距离(float)")
		float fDistance;

		DESC("目标过滤")
		int nTargetFilter;
		
		DESC("BuffID")
		int nBuffID;

		DESC("Buff等级")
		int nBuffLevel;

		DESC("离开范围保留Buff,0不保留,不为0保留")
		int nLeaveReserveBuff;

        DESC("添加buff目标")
        MA_TAGET_TYPE nAddBuffType;

        DESC("最多增加BUFF数量,为0时,默认为8个")
        int nAddBuffCount;

        DESC("是否可重复添加")
        MA_BOOL nRepeatAdd;

        DESC("离开范围Buff是否全部移除")
        MA_BOOL nAllRemove;

        DESC("效果MASK标识")
        EFFECT_MASK  nMaskFlag;

		DESC("忽略垂直距离")
		MA_BOOL bIgnoreY;

		DESC("是否首次执行")
		int	nFirst;

		DESC("是否需要互相可见")
		int	nVisible2EachOther;

		STRUCT_INIT(EffectServer_AddRangeBuff)
	};

	DESC("增加召唤怪物")
	STRUCT EffectServer_AddSummonMonster
	{
		int nID;

		DESC("怪物点类型")
		MONSTER_POS_TYPE nType;

		DESC("刷怪ID")
		int nMonsterID;

		DESC("刷怪数量")
		int	nBuildNum;

		DESC("刷怪坐标x(float)")
		float fPosx;

		DESC("刷怪坐标y(float)")
		float fPosy;

		DESC("刷坐标z(float)")
		float fPosz;

		DESC("技能方向")
		SPELL_DIR_TYPE nDirType;

		DESC("方向距离(float)")
		float fDistance;

		DESC("召唤怪物上限,不要求创建怪物上限的设置为0")
		int nMaxCount;

        DESC("使用对象")
        MA_TAGET_TYPE nTagetType;

        DESC("基础等级")
        int nBaseLevel;

        DESC("影响属性")
        EXTRAATTACK_TYPE nExtraAttackType;

        DESC("攻击百分比，扩大100倍")
        int	nSpellAttackPCT;

        DESC("怪物方向")
        DIRECTION_TYPE nMonsterDirType;

		STRUCT_INIT(EffectServer_AddSummonMonster)
	};

	DESC("减少召唤怪物")
	STRUCT EffectServer_LessSummonMonster
	{
		int nID;

		DESC("怪物ID")
		int nMonsterID;

		STRUCT_INIT(EffectServer_LessSummonMonster)
	};

	DESC("打断技能")
	STRUCT EffectServer_BreakSpell
	{
		int nID;

		DESC("暂时无用")
		int nType;

		STRUCT_INIT(EffectServer_BreakSpell)
	};

	DESC("免疫打断技能")
	STRUCT EffectServer_ImmuneBreakSpell
	{
		int nID;

		DESC("暂时无用")
		int nType;

		STRUCT_INIT(EffectServer_ImmuneBreakSpell)
	};

	DESC("设置隐身属性")
	STRUCT EffectServer_Invisible
	{
		int nID;

		DESC("是否也对友方隐身")
		MA_BOOL	bHideFriend;

		STRUCT_INIT(EffectServer_Invisible)
	};

    DESC("获得天赋")
    STRUCT EffectServer_ActiveTalent
    {
        int nID;

        DESC("天赋ID")
        int nTalentID;

        DESC("效果天赋类型")
        EFFECT_TALENT_TYPE nTalentType;

        DESC("效果结束保留")
        MA_BOOL nReserve;

        STRUCT_INIT(EffectServer_ActiveTalent)
    };

	DESC("持续伤害-攻击力相关")
	STRUCT EffectServer_ContinuedDamage
	{
		int nID;

		DESC("间隔，毫秒")
		int nInterval;

		DESC("次数")
		int nCount;

		DESC("伤害类型DAMAGE_TYPE")
		int nDamageType;

		DESC("附加攻击")
		int	nSpellPower;

        DESC("等级系数(扩大100倍)")
        int	nLevelFactor;

		DESC("影响属性")
		EXTRAATTACK_TYPE nExtraAttackType;

		DESC("攻击百分比，扩大100倍")
		int	nSpellAttackPCT;

		DESC("死亡效果类型")
		DeadEffectType nDeadType;

		DESC("是否有首次伤害")
		int	nFirst;

		DESC("技能效果类型SPELL_EFFECT_TYPE")
		int	nSpellEffectType;

		DESC("使用对象")
		MA_TAGET_TYPE nTagetType;

		DESC("是否触发暴击")
		MA_BOOL nActivateCRD;

        DESC("是否蓄力攻击")
        MA_BOOL nFocoEnergia;

        DESC("附加攻击上限")
        int	nSpellPowerLimit;

        DESC("攻击百分比上限，扩大100倍")
        int	nSpellAttackPCTLimit;

        DESC("附加攻击上限等级系数(扩大100倍)")
        int	nLimitLevelFactor;

		STRUCT_INIT(EffectServer_ContinuedDamage)
	};

	DESC("禁用技能")
	STRUCT EffectServer_DisableSpell
	{
		int nID;

		DESC("禁用技能列表")
		sp_char_array(strSpellID,64);

		STRUCT_INIT(EffectServer_DisableSpell)
	};

    DESC("禁用槽位索引")
    STRUCT EffectServer_DisableSlotIndex
    {
        int nID;

        DESC("槽位类型")
        SLOT_TYPE nSlotType;

        DESC("禁用槽位索引")
        sp_char_array(strSlotIndex,32);

        STRUCT_INIT(EffectServer_DisableSlotIndex)
    };

	DESC("回赠数值")
	STRUCT EffectServer_Feedback
	{
		int nID;

		DESC("目标类型")
		int nTarget;

		DESC("技能类型")
		int nSpellType;

		DESC("回赠类型")
		FEEDBACK_TYPE nNumType;

		DESC("添加数量")
		int nNum;

		STRUCT_INIT(EffectServer_Feedback)
	};

	DESC("复活时间")
	STRUCT EffectServer_ReliveTime
	{
		int nID;

		DESC("固定、百分比")
		MA_VALUE_TYPE eValueType;	

		DESC("数值(毫秒),百分比扩大100倍")
		int nValue;

		DESC("触发频率（毫秒）")
		int nInterval;

		STRUCT_INIT(EffectServer_ReliveTime)
	};

	DESC("反弹伤害")
	STRUCT EffectServer_Rebound
	{
		int nID;

		DESC("伤害类型DAMAGE_TYPE")
		int nDamageType;

		DESC("技能效果类型SPELL_EFFECT_TYPE")
		int	nSpellEffectType;

		DESC("固定、百分比")
		MA_VALUE_TYPE nValueType;

		DESC("数值,百分比扩大100倍")
		int nValue;

		DESC("触发频率(毫秒)")
		int nInterval;

		DESC("是否使用初始伤害")
		MA_BOOL	nInitialDamage;
        
        DESC("反弹类型")
        REBOUND_TYPE nReboundType;

        DESC("属性对象")
        MA_TAGET_TYPE nTagetType;

        DESC("影响属性")
        EXTRAATTACK_TYPE nExtraAttackType;

        DESC("影响属性系数(float)")
        float fExtraFactor;

        DESC("反弹出去的伤害效果类型SPELL_EFFECT_TYPE")
        int nMySpellEffectType;

		STRUCT_INIT(EffectServer_Rebound)
	};

	DESC("持续伤害-效果相关")
	STRUCT EffectServer_ContinuedDamageEffect
	{
		int nID;

		DESC("间隔，毫秒")
		int nInterval;

		DESC("次数")
		int nCount;

		DESC("固定、百分比")
		MA_VALUE_TYPE nValueType;

		DESC("数值,百分比扩大100倍")
		int nValue;

		DESC("是否有首次伤害")
		int	nFirst;

		DESC("伤害类型DAMAGE_TYPE")
		int nDamageType;

		DESC("技能效果类型SPELL_EFFECT_TYPE")
		int	nSpellEffectType;

		DESC("死亡效果类型")
		DeadEffectType nDeadType;

		STRUCT_INIT(EffectServer_ContinuedDamageEffect)
	};

    DESC("持续伤害-属性效果相关")
    STRUCT EffectServer_ContinuedDamagePropertyEffect
    {
        int nID;

        DESC("间隔，毫秒")
        int nInterval;

        DESC("次数")
        int nCount;

        DESC("使用对象")
        MA_TAGET_TYPE nTagetType;

        DESC("数值")
        int nValue;

        DESC("影响属性")
        EXTRAATTACK_TYPE nExtraAttackType;

        DESC("影响数值(float)")
        float fExtraValue;

        DESC("系数(float)")
        float fFactor;

        DESC("持续伤害属性类型对象")
        MA_TAGET_TYPE nPropertyTagetType;

        DESC("持续伤害属性类型")
        DAMAGE_PROPERTY_EFFECT_TYPE nPropertyEffectType;

        DESC("是否有首次伤害")
        int	nFirst;

        DESC("伤害类型DAMAGE_TYPE")
        int nDamageType;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int	nSpellEffectType;

        DESC("死亡效果类型")
        DeadEffectType nDeadType;

        STRUCT_INIT(EffectServer_ContinuedDamagePropertyEffect)
    };

	DESC("持续治疗-效果相关")
	STRUCT EffectServer_ContinuedCureEffect
	{
		int nID;

		DESC("间隔，毫秒")
		int nInterval;

		DESC("次数")
		int nCount;

		DESC("固定、百分比")
		MA_VALUE_TYPE nValueType;

		DESC("数值,百分比扩大100倍")
		int nValue;

		DESC("是否有首次治疗")
		int	nFirst;

		DESC("治疗类型")
		CURE_TYPE nCureType;

		DESC("影响属性类型")
		EXTRAATTACK_TYPE nExtraCureType;

		DESC("属性加成百分比，百分比扩大一百倍")
		int nCurePCT;

        DESC("等级系数(扩大100倍)")
        int	nLevelFactor;

		STRUCT_INIT(EffectServer_ContinuedCureEffect)
	};

	DESC("增加，减少治疗效果")
	STRUCT EffectServer_AddCureEffect
	{
		int nID;

		DESC("固定、百分比")
		MA_VALUE_TYPE nValueType;

		DESC("数值,百分比扩大100倍")
		int nValue;

        DESC("技能效果类型")
        int nSpellEffectType;

		STRUCT_INIT(EffectServer_AddCureEffect)
	};

    DESC("额外伤害")
    STRUCT EffectServer_ApendDamage
    {
        int nID;

        DESC("冷却，毫秒")
        int nCD;

        DESC("触发概率(float)")
        float fRate;
        
        DESC("伤害类型DAMAGE_TYPE")
        int nDamageType;

        DESC("固定伤害")
        int	nSpellPower;

        DESC("等级系数(扩大100倍)")
        int	nLevelFactor;

        DESC("影响属性")
        EXTRAATTACK_TYPE nExtraAttackType;

        DESC("攻击百分比，扩大100倍")
        int	nSpellAttackPCT;

        DESC("死亡效果类型")
        DeadEffectType nDeadType;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int	nSpellEffectType;

		DESC("攻击目标过滤")
		int nTargetFilter;

		DESC("使用对象")
		MA_TAGET_TYPE nTagetType;

		DESC("是否触发暴击")
		MA_BOOL nActivateCRD;

		DESC("最大额外伤害上限类型,针对某类实体有额外上限")
		ENTITY_TYPE	nDamageLimitType;

		DESC("最大额外伤害,0无上限,目前只针对怪物")
		int nDamageLimit;

        DESC("是否蓄力攻击")
        MA_BOOL nFocoEnergia;

        DESC("附加攻击上限")
        int	nSpellPowerLimit;

        DESC("攻击百分比上限，扩大100倍")
        int	nSpellAttackPCTLimit;

        DESC("伤害技能效果类型SPELL_EFFECT_TYPE")
        int	nDamageSpellEffectType;

		STRUCT_INIT(EffectServer_ApendDamage)
    };

    DESC("抵挡伤害效果")
    STRUCT EffectServer_ReduceDamage
    {
        int nID;

        DESC("冷却，毫秒")
        int nCD;

        DESC("触发概率(float)")
        float fRate;

        DESC("百分比/固定值")
        MA_VALUE_TYPE nValueType;

        DESC("数值(float)")
        float fValue;

        DESC("抵挡伤害次数")
        int nTimes;

		DESC("技能效果类型SPELL_EFFECT_TYPE")
		int	nSpellEffectType;

		DESC("攻击目标过滤")
		int nTargetFilter;

		DESC("伤害类型DAMAGE_TYPE")
		int nDamageType;

		DESC("抵挡后增加buffID,为0不加buff,不为0加buff")
		int nBuffID;

		DESC("抵挡后增加buffLevel")
		int nBuffLvel;

		STRUCT_INIT(EffectServer_ReduceDamage)
    };

    DESC("被伤害时触发buff")
    STRUCT EffectServer_AddBuffOnDamage
    {
        int nID;

        DESC("冷却，毫秒")
        int nCD;

        DESC("触发概率(float)")
        float fRate;

        DESC("buffid")
        int nBuffID;

        DESC("bufflevel")
        int nBuffLevel;

        DESC("使用对象")
        MA_TAGET_TYPE nTagetType;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int	nSpellEffectType;

		DESC("攻击目标过滤")
		int nTargetFilter;

		DESC("伤害类型DAMAGE_TYPE")
		int nDamageType;

        DESC("buff添加者")
        MA_TAGET_TYPE nAddType;

		STRUCT_INIT(EffectServer_AddBuffOnDamage)
    };

	DESC("战场增加Buff")
	STRUCT EffectServer_WarAddBuff
	{
		int nID;

		DESC("BuffID")
		int nBuffID;

		DESC("BuffLevel")
		int nBuffLevel;

		DESC("操作BUFF类型")
		BUFF_TYPE nType;

		DESC("目标过滤")
		int nTargetFilter;

		STRUCT_INIT(EffectServer_WarAddBuff)
	};

	DESC("假死")
	STRUCT EffectServer_ApparentDeath
	{
		int nID;

		DESC("冷却组ID")
		EFREEZE_CLASS nClassID;

		DESC("冷却ID")
		int nCoolID;

		DESC("BuffID")
		int nBuffID;

		DESC("BuffLevel")
		int nBuffLevel;

        DESC("假死给自己加Buff之添加者")
        APPARENT_DEATH_ADD_BUFF_TARGET_TYPE nAddBuffTargetType;

        DESC("生存模式标识")
        MA_BOOL isFakeDead;    // 用于控制识别假死玩法

		STRUCT_INIT(EffectServer_ApparentDeath)
	};

	DESC("不可选择")
	STRUCT EffectServer_NoSelect
	{
		int nID;

        DESC("暂时无用")
        int nType;

		STRUCT_INIT(EffectServer_NoSelect)
	};

	DESC("命中")
	STRUCT EffectServer_Accuracy
	{
		int nID;

		DESC("技能效果类型SPELL_EFFECT_TYPE")
		int nSpellEffectType;

		STRUCT_INIT(EffectServer_Accuracy)
	};

	DESC("躲闪")
	STRUCT EffectServer_Evade
	{
		int nID;

		DESC("技能效果类型SPELL_EFFECT_TYPE")
		int nSpellEffectType;

		DESC("概率(1~10000)")
		int nProb;

        DESC("Buff添加者0自己1施法敌人")
        int nBuffAddTarget;

        DESC("BuffID")
        int nBuffID;

        DESC("BuffLevel")
        int nBuffLevel;

		STRUCT_INIT(EffectServer_Evade)
	};

	DESC("攻击获得BUFF")
	STRUCT EffectServer_AttackAddBuff
	{
		int nID;

		DESC("技能效果类型SPELL_EFFECT_TYPE")
		int nSpellEffectType;

        DESC("伤害类型DAMAGE_TYPE")
        int nDamageType;

		DESC("概率(1~10000)")
		int nProb;

		DESC("BuffID")
		int nBuffID;

		DESC("BuffLevel")
		int nBuffLevel;

		DESC("间隔(毫秒)")
		int nInterval;

		DESC("使用对象")
		MA_TAGET_TYPE nTagetType;

		DESC("目标过滤")
		int nTargetFilter;

		DESC("检查类型")
		CHECK_TYPE nCheckType;

		DESC("检查值，百分比扩大100倍")
		int nValue;

		DESC("增加Buff目标对象")
		MA_TAGET_TYPE nAddBuffTagetType;

		DESC("攻击增加Buff类型")
		ATTACK_ADD_BUFF_TYPE nAttackAddBuffType;

		STRUCT_INIT(EffectServer_AttackAddBuff)
	};

	DESC("施法触发技能")
	STRUCT EffectServer_CastSpell
	{
		int nID;

		DESC("目标过滤")
		int nTargetFilter;

		DESC("技能效果类型SPELL_EFFECT_TYPE")
		int nSpellEffectType;

		DESC("概率(1~10000)")
		int nProb;

		DESC("使用对象")
		MA_TAGET_TYPE nTagetType;

		DESC("SpellID")
		int nSpellID;

		DESC("间隔(毫秒)")
		int nInterval;

		DESC("施法技能类型")
		CAST_SPELL_TYPE nCastSpellType;

		STRUCT_INIT(EffectServer_CastSpell)
	};

	DESC("持续增加天赋点数")
	STRUCT EffectServer_ContinuedAddTalentPoint
	{
		int nID;

		DESC("间隔(毫秒)")
		int nInterval;

		DESC("次数")
		int nCount;

		DESC("是否首次执行")
		int	nFirst;

		DESC("数值")
		int nValue;

		STRUCT_INIT(EffectServer_ContinuedAddTalentPoint)
	};

	DESC("实体死亡附加增加天赋点数")
	STRUCT EffectServer_EntityDieNearAddTalentPoint
	{
		int nID;

		DESC("实体类型;TYPE_ENTITY,TYPE_MAX表示所有实体类型")
		ENTITY_TYPE	nEntityType;

		DESC("凶手过滤")
		int	nOperatorFilter;

		DESC("死亡者过滤")
		int	nTargetFilter;

		DESC("离死亡者距离")
		float fDistance;

		DESC("数值")
		int nValue;

		STRUCT_INIT(EffectServer_EntityDieNearAddTalentPoint)
	};

	DESC("击杀额外获得天赋点数")
	STRUCT EffectServer_EntityKillAddTalentPoint
	{
		int nID;

		DESC("数值类型")
		MA_VALUE_TYPE nValueType;

		DESC("每次增加数值;固定值,百分比扩大100倍")
		int nValue;

		STRUCT_INIT(EffectServer_EntityKillAddTalentPoint)
	};

	DESC("移除Buff类型")
	STRUCT EffectServer_RemoveBuffType
	{
		int nID;

		DESC("自己BUFF中移除添加BUFF对象")
		MA_TAGET_TYPE nTagetType;

		DESC("Buff类型")
		int nBuffType;

		STRUCT_INIT(EffectServer_RemoveBuffType)
	};

	DESC("清除仇恨列表")
	STRUCT EffectServer_CleanEnmity
	{
		int nID;

		DESC("自己仇恨列表清除仇恨对象")
		MA_TAGET_TYPE nTagetType;

		STRUCT_INIT(EffectServer_CleanEnmity)
	};

	DESC("击杀获得BUFF")
	STRUCT EffectServer_KillAddBuff
	{
		int nID;

		DESC("死亡实体类型")
		ENTITY_TYPE nEntityType;

		DESC("技能效果类型SPELL_EFFECT_TYPE")
		int nSpellEffectType;

		DESC("击杀目标过滤")
		int nKillTargetFilter;

		DESC("死亡目标过滤")
		int nDieTargetFilter;
		
		DESC("击杀目标位置类型")
		KILL_TARGET_LOC_TYPE nKillTagetLocType;
		
		DESC("死亡坐标范围,大于0判断范围,否则不判断范围")
		float fDieLocRange;

		DESC("概率(1~10000)")
		int nProb;

		DESC("间隔(毫秒)")
		int nInterval;

		DESC("BuffID")
		int nBuffID;

		DESC("BuffLevel")
		int nBuffLevel;

		DESC("Buff添加者类型")
		ADD_BUFF_TARGET_TYPE nAddBuffTargetType;

		DESC("Buff添加者目标过滤")
		int nAddTargetFilter;

		DESC("Buff是否摘除1摘0不摘")
		int nIsBuffRemove;

        DESC("最多增加BUFF数量,为0时,默认为8个")
        int nAddBuffCount;

		STRUCT_INIT(EffectServer_KillAddBuff)
	};

    DESC("击杀怪物获得BUFF")
    STRUCT EffectServer_KillMonsterAddBuff
    {
        int nID;

        DESC("怪物ID,0为所有怪物")
        int nMonsterID;

        DESC("击杀目标过滤")
        int nKillTargetFilter;

        DESC("死亡目标过滤")
        int nDieTargetFilter;

        DESC("间隔(毫秒)")
        int nInterval;

        DESC("增加buff目标")
        MA_TAGET_TYPE nBuffType;

        DESC("添加buff目标")
        MA_TAGET_TYPE nAddBuffType;

        DESC("BuffID")
        int nBuffID;

        DESC("BuffLevel")
        int nBuffLevel;

        STRUCT_INIT(EffectServer_KillMonsterAddBuff)
    };

    DESC("属性转换")
    STRUCT EffectServer_PropertyTransform
    {
        int nID;

        DESC("属性转换类型")
        PROPERTY_TRANSFROM_TYPE nPropertyTransfromType;

        DESC("属性转换后类型")
        PROPERTY_TRANSFROM_BACK_TYPE nPropertyTransfromBackType;

        DESC("转换比例(float)")
        float fValue;

        STRUCT_INIT(EffectServer_PropertyTransform)
    };

    DESC("带目标属性转换")
    STRUCT EffectServer_TargetPropertyTransform
    {
        int nID;

        DESC("目标类型")
        MA_TAGET_TYPE nSourceType;

        DESC("属性转换类型")
        PROPERTY_TRANSFROM_TYPE nPropertyTransfromType;

        DESC("增加目标类型")
        MA_TAGET_TYPE nTargetType;

        DESC("属性转换后类型")
        PROPERTY_TRANSFROM_BACK_TYPE nPropertyTransfromBackType;

        DESC("转换比例(float)")
        float fValue;

        STRUCT_INIT(EffectServer_TargetPropertyTransform)
    };

	DESC("属性转换循环")
	STRUCT EffectServer_PropertyTransformLoop
	{
		int nID;

		DESC("属性转换循环前者类型")
		PROPERTY_TRANSFROM_LOOP_TYPE nFrontType;

		DESC("属性转换循环附加前者类型")
		PROPERTY_TRANSFROM_BACK_TYPE nAppendFrontType;

		DESC("属性转换循环后者类型")
		PROPERTY_TRANSFROM_LOOP_TYPE nBackType;

		DESC("属性转换循环附加后者类型")
		PROPERTY_TRANSFROM_BACK_TYPE nAppendBackType;

		DESC("前者转换比例(float)")
		float fFrontValue;

		DESC("后者转换比例(float)")
		float fBackValue;

		STRUCT_INIT(EffectServer_PropertyTransformLoop)
	};

	DESC("改变状态增加Buff")
	STRUCT EffectServer_ChangeStateAddBuff
	{
		int nID;

		DESC("实体状态")
		ENTITY_STATE nEntityState;

		DESC("间隔(毫秒)")
		int nInterval;

		DESC("过渡时间")
		int nTransitTime;

		DESC("BuffID")
		int nBuffID;

		DESC("BuffLevel")
		int nBuffLevel;

		DESC("增加BuffCount")
		int nAddBuffCount;

		STRUCT_INIT(EffectServer_ChangeStateAddBuff)
	};
	
	DESC("设置商城购买")
	STRUCT EffectServer_SetShopBuy
	{
		int nID;

		DESC("是否可购买")
		MA_BOOL nBuy;

		STRUCT_INIT(EffectServer_SetShopBuy)
	};

	DESC("拖动目标移动")
	STRUCT EffectServer_DragToMove
	{
		int nID;

		DESC("拖动时离目标距离(float)")
		float fDistance;

		DESC("拖动时间")
		int nTime;

		DESC("拖动方向")
		DIRECTION_TYPE nDirectionType;

		DESC("拖动操作者对象")
		MA_TAGET_TYPE nDragOperatorType;

        DESC("效果停止时拖动者增加BuffID")
        int nDragBuffID;

        DESC("效果停止时拖动者增加BuffLevel")
        int nDragBuffLevel;

        DESC("效果停止时被拖动者增加BuffID")
        int nByDragBuffID;

        DESC("效果停止时被拖动者增加BuffLevel")
        int nByDragBuffLevel;

		STRUCT_INIT(EffectServer_DragToMove)
	};

	DESC("改变属性增加Buff")
	STRUCT EffectServer_ChangePropertyAddBuff
	{
		int nID;

		DESC("改变属性类型")
		CHANGE_PROPERTY_TYPE nChangePropertyType;

		DESC("间隔(毫秒)")
		int nInterval;

		DESC("固定、百分比")
		MA_VALUE_TYPE nValueType;

		DESC("低数值,百分比扩大100倍")
		int nLowValue;

		DESC("高数值,百分比扩大100倍")
		int nHighValue;

		DESC("BuffID")
		int nBuffID;

		DESC("BuffLevel")
		int nBuffLevel;

		STRUCT_INIT(EffectServer_ChangePropertyAddBuff)
	};

	DESC("免疫效果")
	STRUCT EffectServer_ImmuneEffect
	{
		int nID;

        DESC("免疫效果类型")
        IMMUNE_EFFECT_TYPE nType;

		STRUCT_INIT(EffectServer_ImmuneEffect)
	};


    DESC("免疫效果(否决)")
    STRUCT EffectServer_ImmuneVoteEffect
    {
        int nID;

        DESC("免疫效果类型")
        IMMUNE_EFFECT_VOTE_TYPE nType;

        STRUCT_INIT(EffectServer_ImmuneVoteEffect)
    };

	DESC("饮血抵挡效果")
	STRUCT EffectServer_DrinkingBlood
	{
		int nID;

		DESC("间隔(毫秒)")
		int nInterval;

		DESC("是否溢出")
		MA_BOOL nOverflow;

		DESC("转化率(float)")
		float fConversionRate;
		
		DESC("饮血抵挡效果类型")
		DRINKING_BLOOD_TYPE	nDrinkingBloodType;

		DESC("系数(float)")
		float fValue;

		DESC("固定值")
		int nFixedValue;

		DESC("过渡时间")
		int nTransitTime;

		DESC("固定、百分比")
		MA_VALUE_TYPE nValueType;

		DESC("数值,百分比扩大100倍")
		int nValue;

		DESC("BuffID")
		int nBuffID;

		DESC("BuffLevel")
		int nBuffLevel;

        DESC("是否有增加护盾事件")
        MA_BOOL nAddShieldEvent;

		STRUCT_INIT(EffectServer_DrinkingBlood)
	};

	DESC("改变buff")
	STRUCT EffectServer_ChangeBuff
	{
		int nID;

		DESC("改变BUFF类型")
		CHANGE_BUFF_TYPE nChangeBuffType;

		DESC("检测BUFFID")
		int nCheckBuffID;

		DESC("增加BUFFID")
		int nAddBuffID;

		DESC("固定、百分比")
		MA_VALUE_TYPE nValueType;

		DESC("数值,百分比扩大100倍")
		int nValue;

		DESC("增减/重设")
		MA_CHANGE_TYPE nChangeType;

        DESC("Buff最大等级")
        int nMaxBuffLevel;

		STRUCT_INIT(EffectServer_ChangeBuff)
	};

	DESC("充能一击")
	STRUCT EffectServer_ChargedStrike
	{
		int nID;

		DESC("充能匹配类型")
		CHARGED_MATCHING_TYPE nMatchingChargedType;

		DESC("充能匹配增加值")
		int	nMatchingValue;

		DESC("充能匹配数值类型")
		int	nMatchingType;

		DESC("充能类型")
		CHARGED_TYPE nChargedType;

		DESC("充能增加值")
		int	nValue;

		DESC("充能上限值")
		int	nLimitValue;

		DESC("充能一击释放类型")
		CHARGED_STRIKE_TYPE nChargedStrikeType;

		DESC("技能效果释放类型")
		int	nExpendType;

		DESC("消耗值 无用")
		int	nExpendValue;

		DESC("BuffID")
		int nBuffID;

		DESC("BuffLevel")
		int nBuffLevel;

		DESC("时间充能")
		CHARGED_TIME_TYPE nChargedTimeType;

		DESC("时间充能增加值")
		int	nTimeValue;

		DESC("时间间隔")
		int nTimeInterval;

		DESC("1自动0手动消耗")
		int nChooseRelease;

		DESC("自动手动消耗值")
		int nReleaseValue;

		DESC("打断时间充能技能效果类型")
		int	nSpellEffectBreakType;

		DESC("打断时常")
		int	nSpellEffectBreakTime;

		STRUCT_INIT(EffectServer_ChargedStrike)
	};

	DESC("BUFF效果改变")
	STRUCT EffectServer_BuffEffectChange
	{
		int nID;

		DESC("触发频率(毫秒)")
		int nInterval;

		DESC("BUFF标志位匹配")
		unsigned long dwFlag;

		DESC("BUFF效果改变类型")
		BUFF_EFFECT_CHANGE_TYPE nBuffEffectChangeType;

		DESC("固定、百分比")
		MA_VALUE_TYPE nValueType;

		DESC("数值,百分比扩大100倍")
		int nValue;

		STRUCT_INIT(EffectServer_BuffEffectChange)
	};

	DESC("助攻增加Buff")
	STRUCT EffectServer_AssistAddBuff
	{
		int nID;

		DESC("触发频率(毫秒)")
		int nInterval;

		DESC("击杀目标过滤")
		int nKillTargetFilter;

		DESC("死亡目标过滤")
		int nDieTargetFilter;

		DESC("助攻增加BUFF类型")
		ASSIST_ADD_BUFF_TYPE AssistAddBuffType;

		DESC("BuffID")
		int nBuffID;

		DESC("BuffLevel")
		int nBuffLevel;

		STRUCT_INIT(EffectServer_AssistAddBuff)
	};

	DESC("怪物死亡经验、金币加成效果")
	STRUCT EffectServer_DieIncreaseMonneyExp
	{
		int nID;

		DESC("技能ID列表")
		sp_char_array(strComboID,64);

		DESC("金币加成系数")
		int nMonneyCoefficient ;

		DESC("经验加成系数")
		int nExpCoefficient;

		DESC("冷却间隔")
		int nCDTime;

		DESC("死亡怪物周围加效果半径(float)")
		float fDieLocRange;

		DESC("死亡怪物标志位过滤")
		int nKillTargetFilter;

		DESC("周围添加者过滤")
		int nAddTargetFilter;

		DESC("目标添加效果BuffID")
		sp_char_array(nAddTargetBuffID,64);

		DESC("自己添加效果BuffID")
		sp_char_array(nAddSelfBuffID,64);

        DESC("是否为多技能触发加buff效果1是，0否(慎重，填1只是单纯加buff，没有加钱效果)")
        int nIsLotsSpellAttackAddBuffEffect;

        DESC("击中目标过滤")
        int nTargetFilter;

		STRUCT_INIT(EffectServer_DieIncreaseMonneyExp)
	};

	DESC("充能转化为属性")
	STRUCT EffectServer_ChargedToProPerty
	{
		int nID;

        DESC("对应的充能效果ID值(为0不取值)")
        int	nEffectID;

		DESC("充能属性类型")
		CHARGED_PROPERTY_TYPE nChargedPropertyType;

		DESC("充能类型")
		int  nChargedType;

		DESC("攻击到敌人充能增加值")
		int	nPAValue;

		DESC("任何技能充能增加值")
		int	nMAValue;

		DESC("時間充能增加值")
		int	nTimeValue;

		DESC("移動充能增加值")
		int	nMoveValue;

		DESC("时间间隔")
		int nTimeInterval;

		DESC("充能上限值")
		int	nLimitValue;

		DESC("固定、百分比")
		MA_VALUE_TYPE nKillValueType;

		DESC("击杀充能值(float)")
		float fKIllValue;

		DESC("死亡目标过滤")
		int nTargetFilter;

		DESC("死亡扣百分比(float)")
		float fDieValue;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int nSpellEffectType;

        DESC("死亡充能范围")
        float fDieDistance;

        DESC("死亡范围充能值")
        int nDieValue;

        DESC("触发状态阀值(分号分隔)")
		sp_char_array(szActiveValues, 64);

        DESC("状态ID(分号分隔)")
        sp_char_array(szActiveBuffIDs, 64);
      
        DESC("等级(分号分隔)")
        sp_char_array(szActiveBuffLvs, 64);

		STRUCT_INIT(EffectServer_ChargedToProPerty)
	};

	DESC("目标增加位置")
	STRUCT EffectServer_DragToMoveEX
	{
		int nID;

		DESC("延时时间")
		int nTime;

		DESC("拖动方向")
		DIRECTION_TYPE nDirectionType;

		DESC("拖动操作者对象")
		MA_TAGET_TYPE nDragOperatorType;

		DESC("是否瞬移")
		int  nIsTeleport ;

		DESC("移动距离(float)")
		float fDistance;

		DESC("移动速度")
		int nMoveSpeed;

        DESC("是否二段技能回原地1是0否（有时间限定）")
        int  nIsDoubleSpellUseBack ;

        DESC("是否鼠标位移1鼠标位移0固定位移")
        int  nIsUseMouseLocation ;

		STRUCT_INIT(EffectServer_DragToMoveEX)
	};

	DESC("目标添加仇恨")
    STRUCT EffectServer_AddEnmity
	{
		int nID;

		DESC("仇恨数值")
		int nEnmityValue;

		STRUCT_INIT(EffectServer_AddEnmity)
	};

    DESC("设置隐形")
    STRUCT EffectServer_SetStealth
    {
        int nID;

        DESC("对队友显示类型")
        STEALTH_TOFRIENDS_DISP_TYPE nDisp;

        DESC("全部不可见(包括自己)")
        MA_BOOL nAllStealth;

        STRUCT_INIT(EffectServer_SetStealth)
    };

    DESC("给自己加buff,目标死亡移除buff")
    STRUCT EffectServer_AddBuff_And_RmoveOnTargetDie
    {
        int nID;

        DESC("buffID")
        int nBuffID;

        DESC("buffLevel")
        int nBuffLevel;

        STRUCT_INIT(EffectServer_AddBuff_And_RmoveOnTargetDie)
    };

    DESC("反隐形")
    STRUCT EffectServer_AntiStealth
    {
        int nID;

        DESC("暂时无用")
        int nType;

        STRUCT_INIT(EffectServer_AntiStealth)
    };

    DESC("被反隐加buff")
    STRUCT EffectServer_StealthAntiedAddBuff
    {
        int nID;

        DESC("buffID")
        int nBuffID;

        DESC("buffLevel")
        int nBuffLevel;

        STRUCT_INIT(EffectServer_StealthAntiedAddBuff)
    };

    DESC("设置视野距离")
    STRUCT EffectServer_SetSight
    {
        int nID;

        DESC("视野距离(米)(float)")
        float fSight;

        STRUCT_INIT(EffectServer_SetSight)
    };

	DESC("物品使用重新充能")
	STRUCT EffectServer_GoodsRestartCharged
	{
		int nID;

		DESC("恢复充能值默认0恢复满、目前不设置")
		int nRescoveryCount;

		STRUCT_INIT(EffectServer_GoodsRestartCharged)
	};

	DESC("多重冷却效果")
	STRUCT EffectServer_MultipleCoolDown
	{
		int nID;

		DESC("充能上限(次数)")
		int nChargedCount;

		DESC("使用时间间隔")
		int nInterval;

		DESC("回城是否充满次数")
	    int nResetChargeCount;

        DESC("初始化充能次数")
        int nInitalChargeCount;

		STRUCT_INIT(EffectServer_MultipleCoolDown)
	};

    DESC("击退增加Buff")
    STRUCT EffectServer_BeatBackAddBuff
    {
        int nID;

        DESC("增加Buff对象")
        BEAT_BACK_TARGET_TYPE nAddBuffType;

        DESC("Buff添加者类型")
        BEAT_BACK_TARGET_TYPE nAddBuffTargetType;

        DESC("BuffID")
        int nBuffID;

        DESC("BuffLevel")
        int nBuffLevel;

        STRUCT_INIT(EffectServer_BeatBackAddBuff)
    };

    DESC("累积伤害")
    STRUCT EffectServer_AccumulateDamage
    {
        int nID;

        DESC("累积伤害时间")
        int nTime;

        DESC("累积比例(float)")
        float fValue;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("伤害类型")
        int nDamageType;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int	nSpellEffectType;

        STRUCT_INIT(EffectServer_AccumulateDamage)
    };

    DESC("跳跃效果")
    STRUCT EffectServer_Jump
    {
        int nID;

        DESC("暂时无用")
        int nType;

        STRUCT_INIT(EffectServer_Jump)
    };

    DESC("检测目标范围")
    STRUCT EffectServer_CheckTargetRange
    {
        int nID;

        DESC("检测间隔毫秒")
        int nTime;

        DESC("检测距离(float)")
        float fDistance;

        DESC("目标对象")
        MA_TAGET_TYPE nTargetType;

        DESC("检测目标范围类型")
        CHECK_TARGET_RANGE_TYPE nType;

        DESC("BUFF作用对象_作废")
        MA_TAGET_TYPE nBuffType;

        DESC("增加BUFF作用对象_作废")
        MA_TAGET_TYPE nAddBuffType;

        DESC("BuffID")
        int nBuffID;

        DESC("BuffLevel")
        int nBuffLevel;

        DESC("持续时间毫秒")
        int nDuration;

        DESC("持续时间结束给目标增加buff")
        int nBuffID_Final;

        DESC("持续时间结束给目标增加buff等级")
        int nBuffLevel_Final;

        DESC("目标离开是否移除目标的buff")
        MA_BOOL nRemoveBuffOnTargetLeave;

        STRUCT_INIT(EffectServer_CheckTargetRange)
    };


    DESC("定时增加Buff")
    STRUCT EffectServer_TimingAddBuff
    {
        int nID;

        DESC("间隔，毫秒")
        int nInterval;

        DESC("次数")
        int nCount;

        DESC("是否有首次执行")
        int	nFirst;

        DESC("BuffID")
        int nBuffID;

        DESC("BuffLevel")
        int nBuffLevel;

        DESC("目标类型")
        MA_TAGET_TYPE nTargetType;

        DESC("BUFF类型")
        BUFF_TYPE nType;

        DESC("增加目标类型")
        MA_TAGET_TYPE nAddTargetType;

        STRUCT_INIT(EffectServer_TimingAddBuff)
    };

    DESC("显示飘字")
    STRUCT EffectServer_ShowFloatText
    {
        int nID;

        DESC("飘字ID")
        int nTextID;

        STRUCT_INIT(EffectServer_ShowFloatText)
    };

    DESC("寻找目标")
    STRUCT EffectServer_FindTarget
    {
        int nID;

        DESC("寻找范围(float)")
        float fDist;

        DESC("目标过滤")
        int nTargetFilter;

        STRUCT_INIT(EffectServer_FindTarget)
    };

    DESC("设置技能单个数据(int)")
    STRUCT EffectServer_SetSingleSpellData
    {
        int nID;

        DESC("效果结束是否恢复数据")
        MA_BOOL nRecovery;

        DESC("技能ID")
        int nSpellID;

        DESC("技能数值定义枚举")
        SINGLE_SPELL_DATA nIndex;

        DESC("技能数据")
        int nIndexData;

        STRUCT_INIT(EffectServer_SetSingleSpellData)
    };

    // 字符串和int拆出来，不然一个效果占用字段太大
    DESC("设置技能单个字符串数据(str)")
    STRUCT EffectServer_SetSingleStrSpellData
    {
        int nID;

        DESC("效果结束是否恢复数据")
        MA_BOOL nRecovery;

        DESC("技能ID")
        int nSpellID;

        DESC("技能数值定义枚举")
        SINGLE_STR_SPELL_DATA nIndex;

        DESC("技能字符串")
        sp_char_array(strIndex,SINGLE_STR_SPELL_DATA_LEN);

        STRUCT_INIT(EffectServer_SetSingleStrSpellData)
    };

    DESC("持续升级")
    STRUCT EffectServer_Upgrade
    {
        int nID;

        DESC("间隔，毫秒")
        int nInterval;

        DESC("次数")
        int nCount;

        DESC("增加等级")
        int nValue;

        DESC("是否首次执行")
        int	nFirst;

        STRUCT_INIT(EffectServer_Upgrade)
    };

    DESC("设置翅膀")
    STRUCT EffectServer_SetWing
    {
        int nID;

        DESC("翅膀ID")
        int nWingID;

        STRUCT_INIT(EffectServer_SetWing)
    };

    DESC("设置翅膀状态")
    STRUCT EffectServer_SetWingState
    {
        int nID;

        DESC("翅膀状态")
        WING_STATE_TYPE nWingState;

        STRUCT_INIT(EffectServer_SetWingState)
    };

    DESC("设置翅膀属性")
    STRUCT EffectServer_SetWingProperty
    {
        int nID;

        DESC("翅膀属性")
        SET_WING_TYPE nType;

        DESC("数值(float)")
        float fValue;

        STRUCT_INIT(EffectServer_SetWingProperty)
    };

    DESC("随机增加Buff")
    STRUCT EffectServer_RandAddBuff
    {
        int nID;

        DESC("间隔，毫秒")
        int nInterval;

        DESC("距离(float)")
        float fDistance;

        DESC("BuffID列表")
        sp_char_array(strBuffID,32);

        DESC("BuffLevel列表")
        sp_char_array(strBuffLevel,32);

        DESC("BUFF作用对象")
        MA_TAGET_TYPE nBuffType;

        DESC("增加BUFF作用对象")
        MA_TAGET_TYPE nAddBuffType;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("击中增加BuffID")
        int nBuffID;

        DESC("击中增加BuffLevel")
        int nBuffLevel;

        DESC("增加BuffCount")
        int nBuffCount;

        DESC("随机增加BUFF类型")
        RAND_ADD_BUFF_TYPE nRandAddBuffType;

        DESC("选取最近玩家距离(float)")
        float fNearDistance;

        STRUCT_INIT(EffectServer_RandAddBuff)
    };

    DESC("设置翅膀飞行时间")
    STRUCT EffectServer_SetWingFlyTime
    {
        int nID;

        DESC("更改类型")
        MA_CHANGE_TYPE  nChangeType;

        DESC("数值")
        int nValue;

        STRUCT_INIT(EffectServer_SetWingFlyTime)
    };

    DESC("夺取Buff")
    STRUCT EffectServer_CaptureBuff
    {
        int nID;

        DESC("操作者作用对象")
        MA_TAGET_TYPE nOperatorType;

        DESC("目标作用对象")
        MA_TAGET_TYPE nTargetType;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("buff标识")
        int nType;

        DESC("操作者增加BuffID")
        int nOperatorBuffID;

		DESC("操作者增加Buff等级")
		int nOperatorBuffLevel;

        DESC("目标增加BuffID")
        int nTargetBuffID;

		DESC("目标增加Buff等级")
		int nTargetBuffLevel;

        STRUCT_INIT(EffectServer_CaptureBuff)
    };

    DESC("检测属性增加Buff")
    STRUCT EffectServer_CheckPropertyAddBuff
    {
        int nID;

        DESC("间隔，毫秒")
        int nInterval;

        DESC("次数")
        int nCount;

        DESC("范围(float)")
        float fDistance;

        DESC("检测属性增加buff类型")
        CHECK_PROPERTY_ADD_BUFF_TYPE nAddBuffType;

        DESC("数值类型")
        MA_VALUE_TYPE nValueType;

        DESC("低数值(float)")
        float fMinValue;

        DESC("高数值(float)")
        float fMaxValue;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("目标BuffID")
        int nTargetBuffID;

        DESC("目标BuffLevel")
        int nTargetBuffLevel;

        DESC("自己BuffID")
        int nBuffID;

        DESC("BuffLevel")
        int nBuffLevel;

        DESC("最多增加BUFF数量,为0时,默认为8个")
        int nAddBuffCount;

        STRUCT_INIT(EffectServer_CheckPropertyAddBuff)
    };

    DESC("检测魔法或位置增加Buff")
    STRUCT EffectServer_CheckMpOrDistanceAddBuff
    {
        int nID;

        DESC("间隔，毫秒")
        int nInterval;

        DESC("次数")
        int nCount;

        DESC("增加魔法值")
        int nValue;

        DESC("结束魔法值")
        int nEndValue;

        DESC("使用技能点类型")
        USE_SPELL_POS_TYPE nType;

        DESC("距离值,0不检测距离(float)")
        float fDistance;

        DESC("结束增加BuffID")
        int nBuffID;

        DESC("结束增加BuffLevel")
        int nBuffLevel;

		DESC("是否叠加增加魔法")
		MA_BOOL bAccumulateAddMP;

		DESC("叠加增加魔法基础值")
		int nAccumulateBaseMP;

        STRUCT_INIT(EffectServer_CheckMpOrDistanceAddBuff)
    };

    DESC("持续位置增加Buff")
    STRUCT EffectServer_ContinuePositionAddBuff
    {
        int nID;

        DESC("间隔，毫秒")
        int nInterval;

        DESC("次数")
        int nCount;

        DESC("使用技能点类型")
        USE_SPELL_POS_TYPE nType;

        DESC("距离值(float)")
        float fDistance;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("增加BuffID")
        int nBuffID;

        DESC("增加BuffLevel")
        int nBuffLevel;

        DESC("最多增加BUFF数量,为0时,默认为8个")
        int nAddBuffCount;

        STRUCT_INIT(EffectServer_ContinuePositionAddBuff)
    };

    DESC("距离伤害")
    STRUCT EffectServer_DistanceDamage
    {
        int nID;

        DESC("属性目标作用对象")
        MA_TAGET_TYPE nPropertyTargetType;

        DESC("距离伤害属性类型")
        DISTANCE_DAMAGE_PROPERTY_TYPE nPropertyType;

        DESC("距离伤害目标类型")
        DISTANCE_DAMAGE_LOC_TYPE nLocType;

        DESC("基础伤害")
        int nBaseDamageHP;

        DESC("最小伤害")
        int nMinDamageHP;

        DESC("最大伤害")
        int nMaxDamageHP;

        DESC("最小系数(float)")
        float fMinFactor;

        DESC("最大系数(float)")
        float fMaxFactor;

        DESC("伤害类型DAMAGE_TYPE")
        int nDamageType;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int	nSpellEffectType;

        DESC("死亡效果类型")
        DeadEffectType nDeadType;

        DESC("最小伤害等级系数(扩大100倍)")
        int nMinLevelFactor;

        DESC("最大伤害等级系数(扩大100倍)")
        int nMaxLevelFactor;

        STRUCT_INIT(EffectServer_DistanceDamage)
    };

    DESC("累积伤害增加Buff")
    STRUCT EffectServer_AccumulateDamageAddBuff
    {
        int nID;

        DESC("冷却组ID")
        EFREEZE_CLASS nClassID;

        DESC("冷却ID")
        int nCoolDownID;

        DESC("伤害类型")
        int nDamageType;

        DESC("累积时间(毫秒)")
        int nAccumulateTime;

        DESC("固定值")
        int nFixedValue;

        DESC("属性目标作用对象")
        MA_TAGET_TYPE nPropertyTargetType;

        DESC("累积伤害增加Buff类型")
        ACCUMULATE_DAMAGE_ADD_BUFF_TYPE nType;

        DESC("累积伤害系数(float)")
        float fFactor;

        DESC("增加Buff作用对象")
        MA_TAGET_TYPE nAddBuffTargetType;

        DESC("增加BuffID")
        int nBuffID;

        DESC("增加BuffLevel")
        int nBuffLevel;

        DESC("属性目标过滤")
        int nPropertyTargetFilter;

        STRUCT_INIT(EffectServer_AccumulateDamageAddBuff)
    };

    DESC("移动伤害")
    STRUCT EffectServer_MoveDamage
    {
        int nID;

        DESC("间隔，毫秒")
        int nInterval;

        DESC("次数")
        int nCount;

        DESC("影响属性对象")
        MA_TAGET_TYPE nTagetType;

        DESC("附加攻击")
        int	nSpellPower;

        DESC("等级系数(扩大100倍)")
        int	nLevelFactor;

        DESC("影响属性")
        EXTRAATTACK_TYPE nExtraAttackType;

        DESC("攻击百分比，扩大100倍")
        int	nSpellAttackPCT;

        DESC("移动伤害类型")
        MOVE_DAMAGE_TYPE nType;

        DESC("伤害类型DAMAGE_TYPE")
        int nDamageType;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int	nSpellEffectType;

        DESC("死亡效果类型")
        DeadEffectType nDeadType;

        STRUCT_INIT(EffectServer_MoveDamage)
    };


    DESC("击杀召唤怪物")
    STRUCT EffectServer_KillSummonMonster
    {
        int nID;

        DESC("死亡实体类型")
        ENTITY_TYPE nEntityType;

        DESC("击杀目标过滤")
        int nKillTargetFilter;

        DESC("死亡目标过滤")
        int nDieTargetFilter;

        DESC("击杀目标位置类型")
        KILL_TARGET_LOC_TYPE nKillTagetLocType;

        DESC("死亡坐标范围,大于0判断范围,否则不判断范围(float)")
        float fDieLocRange;

        DESC("击杀怪物ID")
        int nMonsterID;

        DESC("召唤怪物ID")
        int nSummonMonsterID;

        DESC("召唤怪物数量")
        int nSummonCount;

        DESC("召唤怪物等级")
        int nSummonLevel;

        DESC("间隔(毫秒)")
        int nInterval;

        STRUCT_INIT(EffectServer_KillSummonMonster)
    };

    DESC("进入草丛增加Buff")
    STRUCT EffectServer_EnterForestAddBuff
    {
        int nID;

        DESC("触发频率(毫秒)")
        int nInterval;

        DESC("获得Buff延迟时间(毫秒)")
        int nDelay;

        DESC("离开草丛移除Buff")
        MA_BOOL nRemoveBuff;

        DESC("增加BuffID")
        int nBuffID;

        DESC("增加BuffLevel")
        int nBuffLevel;

        STRUCT_INIT(EffectServer_EnterForestAddBuff)
    };

    DESC("匹配技能增加Buff")
    STRUCT EffectServer_MatchingSpellAddBuff
    {
        int nID;

        DESC("匹配技能类型")
        MATHING_SPELL_TYPE nType;

        DESC("触发频率(毫秒)")
        int nInterval;

        DESC("匹配SpellID表")
        sp_char_array(strSpellID,64);

        DESC("增加BuffID")
        int nBuffID;

        DESC("增加BuffLevel")
        int nBuffLevel;

        STRUCT_INIT(EffectServer_MatchingSpellAddBuff)
    };

    DESC("检测Buff增加Buff")
    STRUCT EffectServer_CheckBuffAddBuff
    {
        int nID;

        DESC("检测Buff类型")
        CHECK_BUFF_TYPE nType;

        DESC("检测对象")
        MA_TAGET_TYPE nTargetType;

        DESC("检测BuffID,为0不检测")
        int nCheckBuffID;

        DESC("检测BuffLevel")
        int nCheckBuffLevel;

        DESC("触发频率(毫秒)")
        int nInterval;

        DESC("增加BuffID")
        int nBuffID;

        DESC("增加BuffLevel")
        int nBuffLevel;

        DESC("检测Buff标识,为0不检测")
        int nCheckBuffType;

        DESC("增加对象")
        MA_TAGET_TYPE nAddTargetType;

        STRUCT_INIT(EffectServer_CheckBuffAddBuff)
    };

    DESC("攻击实体增加Buff")
    STRUCT EffectServer_AttackEntityAddBuff
    {
        int nID;

        DESC("人物为目标过滤,怪物ID")
        int nValue;

        DESC("触发频率(毫秒)")
        int nInterval;

        DESC("增加BuffID")
        int nBuffID;

        DESC("增加BuffLevel")
        int nBuffLevel;

        STRUCT_INIT(EffectServer_AttackEntityAddBuff)
    };

    DESC("攻击效果增加Buff")
    STRUCT EffectServer_AttackEffectAddBuff
    {
        int nID;

        DESC("攻击效果类型")
        ATTACK_EFFECT_TYPE nType;

        DESC("触发频率(毫秒)")
        int nInterval;

        DESC("增加BuffID")
        int nBuffID;

        DESC("增加BuffLevel")
        int nBuffLevel;

        STRUCT_INIT(EffectServer_AttackEffectAddBuff)
    };

    DESC("检测距离增加不同Buff")
    STRUCT EffectServer_CheckDistanceAddBuff
    {
        int nID;

        DESC("间隔，毫秒")
        int nInterval;

        DESC("次数")
        int nCount;

        DESC("使用技能点类型")
        USE_SPELL_POS_TYPE nType;

        DESC("距离值(float)")
        float fDistance;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("距离列表")
        sp_char_array(strDistance,32);

        DESC("BuffID列表")
        sp_char_array(strBuffID,32);

        DESC("BuffLevel列表")
        sp_char_array(strBuffLevel,32);

        DESC("是否有首次执行")
        int	nFirst;

        DESC("自己增加BuffID列表")
        sp_char_array(strSelfBuffID,32);

        DESC("自己增加BuffLevel列表")
        sp_char_array(strSelfBuffLevel,32);

        DESC("优先英雄")
        MA_BOOL	bPriorityHero;

        DESC("增加Buff数量,(0表示不限制数量)")
        int nAddBuffCount;

        STRUCT_INIT(EffectServer_CheckDistanceAddBuff)
    };

    DESC("内外圈增加Buff")
    STRUCT EffectServer_InsideOuterCircleAddBuff
    {
        int nID;

        DESC("间隔，毫秒")
        int nInterval;

        DESC("次数")
        int nCount;

        DESC("增加时间，毫秒")
        int nAddTime;

        DESC("使用技能点类型")
        USE_SPELL_POS_TYPE nType;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("外圈距离值(float)")
        float fOuterDistance;

        DESC("内圈距离值(float)")
        float fInsideDistance;

        DESC("BuffID")
        int nBuffID;

        DESC("BuffLevel")
        int nBuffLevel;

        DESC("增加Buff数量,(0表示不限制数量)")
        int nAddBuffCount;

        STRUCT_INIT(EffectServer_InsideOuterCircleAddBuff)
    };

    DESC("距离添加不同buff")
    STRUCT EffectServer_DistanceAddBuff
    {
        int nID;

        DESC("距离增加BUFF起点类型")
        DISTANCE_ADD_BUFF_TYPE nStartType;

        DESC("距离增加BUFF终点类型")
        DISTANCE_ADD_BUFF_TYPE nEndType;

        DESC("增加BUFF对象")
        MA_TAGET_TYPE nAddBuffType;

        DESC("距离列表")
        sp_char_array(strDistance,32);

        DESC("BuffID列表")
        sp_char_array(strBuffID,32);

        DESC("BuffLevel列表")
        sp_char_array(strBuffLevel,32);

        STRUCT_INIT(EffectServer_DistanceAddBuff)
    };

    DESC("替换槽位技能")
    STRUCT EffectServer_ReplaceSlotSpell
    {
        int nID;

        DESC("槽位类型")
        SLOT_TYPE nSlotType;

        DESC("槽位索引")
        int nSlotIndex;

        DESC("技能列表")
        sp_char_array(strSpell,32);

        DESC("替换技能ID")
        int nSpellID;

        DESC("是否永久替换")
        MA_BOOL bNeverRestore;

        STRUCT_INIT(EffectServer_ReplaceSlotSpell)
    };

    DESC("替换槽位技能组")
    STRUCT EffectServer_ReplaceSlotSpellGroup
    {
        int nID;

        DESC("槽位类型")
        SLOT_TYPE nSlotType;

        DESC("槽位索引")
        int nSlotIndex;

        DESC("替换技能列表")
        sp_char_array(strReplaceSpell,32);

        DESC("恢复技能列表")
        sp_char_array(strSpell,32);

        DESC("是否永久替换")
        MA_BOOL bNeverRestore;

        STRUCT_INIT(EffectServer_ReplaceSlotSpellGroup)
    };

    DESC("检测范围buff标识增加buff")
    STRUCT EffectServer_CheckRangeBuffAddBuff
    {
        int nID;

        DESC("间隔，毫秒")
        int nInterval;

        DESC("次数")
        int nCount;

        DESC("检测点")
        USE_SPELL_POS_TYPE nType;

        DESC("距离(float)")
        float fDistance;

        DESC("检测目标范围类型")
        CHECK_TARGET_RANGE_TYPE nRangeType;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("检测Buff标识")
        unsigned long dwBuffType;

        DESC("BuffID")
        int nBuffID;

        DESC("Buff等级")
        int nBuffLevel;

        DESC("自身加BuffID")
        int nSelfBuffID;

        DESC("自身加Buff等级")
        int nSelfBuffLevel;

        DESC("是否有首次执行")
        int	nFirst;

        DESC("最多增加BUFF数量,为0时,默认为8个")
        int nAddBuffCount;

        STRUCT_INIT(EffectServer_CheckRangeBuffAddBuff)
    };

    DESC("升级改变属性")
    STRUCT EffectServer_UpgradeChangeProperty
    {
        int nID;

        DESC("属性类型")
        ENTITY_PROPERTY_ nType;

        DESC("属性值(扩大100倍,减少计算转换误差)")
        int nVal;

        STRUCT_INIT(EffectServer_UpgradeChangeProperty)
    };

    DESC("选择怪物传送")
    STRUCT EffectServer_ChooseMonsterTransport
    {
        int nID;

        DESC("怪物ID")
        int nMonsterID;

        DESC("目标过滤")
        int nTargetFilter;

        STRUCT_INIT(EffectServer_ChooseMonsterTransport)
    };

  	DESC("死亡范围buff")
    STRUCT EffectServer_DieAddBuff
    {
        int nID;

        DESC("间隔，毫秒")
        int nInterval;

        DESC("加buff距离(float)")
        float fDistance;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("BuffID")
        int nBuffID;

        DESC("Buff等级")
        int nBuffLevel;

        DESC("延迟时间")
        int nDelay;

        DESC("最多增加BUFF数量,为0时,默认为8个")
        int nAddBuffCount;

        DESC("增加BUFF目标类型")
        MA_TAGET_TYPE nAddTargetType;

        STRUCT_INIT(EffectServer_DieAddBuff)
    };

    DESC("死亡事件增加Buff")
    STRUCT EffectServer_DieEventAddBuff
    {
        int nID;

        DESC("BuffID")
        int nBuffID;

        DESC("Buff等级")
        int nBuffLevel;

        STRUCT_INIT(EffectServer_DieEventAddBuff)
    };
	
    DESC("改变实体属性(可改变值)")
    STRUCT EffectServer_ChangePropertyChangeValue 
    {
        int nID;

        DESC("属性类型")
        ENTITY_PROPERTY_ nType;

        DESC("增减/重设")
        MA_CHANGE_TYPE nChangeType;

        DESC("属性值扩大100倍")
        int nVal;

        DESC("是否有属性改变事件")
        MA_BOOL nEvent;

        STRUCT_INIT(EffectServer_ChangePropertyChangeValue)
    };

    DESC("提升伤害")
    STRUCT EffectServer_PromoteDamage
    {
        int nID;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int nSpellEffectType;

        DESC("数值类型")
        MA_VALUE_TYPE nValueType;

        DESC("提升值(float)")
        float fValue;

        DESC("提升伤害类型")
        PROMOTE_DAMAGE_TYPE nPromoteDamageType;

        STRUCT_INIT(EffectServer_PromoteDamage)
    };

    DESC("提升护盾")
    STRUCT EffectServer_PromoteShield
    {
        int nID;

        DESC("数值类型")
        MA_VALUE_TYPE nValueType;

        DESC("提升值(float)")
        float fValue;

        STRUCT_INIT(EffectServer_PromoteShield)
    };

    DESC("保持可见")
    STRUCT EffectServer_KeepInsight
    {
        int nID;

        DESC("可见阵营")
        MA_CAMP_RELATIONSHIP eCamp;

        STRUCT_INIT(EffectServer_KeepInsight)
    };

    DESC("全图可见")
    STRUCT EffectServer_GlobalVisible
    {
        int nID;

        STRUCT_INIT(EffectServer_GlobalVisible)
    };

    DESC("改变实体标志")
    STRUCT EffectServer_ChangeMaskFlag
    {
        int nID;

        DESC("数值(与原值或)")
        int nValue;

        DESC("是否减少")
        MA_BOOL bReduce;

        STRUCT_INIT(EffectServer_ChangeMaskFlag)
    };

    DESC("布兰登召唤效果")
    STRUCT EffectServer_BLDSummon
    {
        int nID;

        DESC("怪物ID")
        int nMonsterID;

        DESC("狼死亡自己增加BuffID")
        int nBuffID;

        DESC("狼死亡自己增加Buff等级")
        int nBuffLevel;

        DESC("狼死亡凶手增加BuffID")
        int nKillBuffID;

        DESC("狼死亡凶手增加Buff等级")
        int nKillBuffLevel;

        DESC("击杀召唤怪给狼增加BuffID")
        int nKillSummonBuffID;

        DESC("击杀召唤怪给狼增加Buff等级")
        int nKillSummonBuffLevel;

        DESC("间隔(毫秒)")
        int nInterval;

        DESC("距离(float)")
        float fDistance;

        DESC("进入范围增加BuffID")
        int nEnterBuffID;

        DESC("进入范围增加Buff等级")
        int nEnterBuffLevel;

        DESC("离开范围增加BuffID")
        int nLevelBuffID;

        DESC("离开范围增加Buff等级")
        int nLevelBuffLevel;

        DESC("延迟传送间隔(毫秒)")
        int nTransportInterval;

        DESC("检测清除连杀BuffID")
        int nCheckClearBuffID;

        DESC("检测清除连杀Buff等级")
        int nCheckClearBuffLevel;

        STRUCT_INIT(EffectServer_BLDSummon)
    };

    DESC("布兰登伤害")
    STRUCT EffectServer_BLDDamage
    {
        int nID;

        DESC("固定、百分比")
        MA_VALUE_TYPE nValueType;

        DESC("数值(float)")
        float fValue;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int	nSpellEffectType;

        DESC("伤害冷却时间(毫秒)")
        int nCoolTime;

        STRUCT_INIT(EffectServer_BLDDamage)
    };

    DESC("检测目标过滤增加buff")
    STRUCT EffectServer_CheckTargetFilterAddBuff
    {
        int nID;

        DESC("间隔，毫秒")
        int nInterval;

        DESC("次数")
        int nCount;

        DESC("距离(float)")
        float fDistance;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("BuffID")
        int nBuffID;

        DESC("Buff等级")
        int nBuffLevel;

        DESC("是否有首次执行")
        int	nFirst;

        STRUCT_INIT(EffectServer_CheckTargetFilterAddBuff)
    };

    DESC("特定伤害增加Buff")
    STRUCT EffectServer_SpecificDamageAddBuff
    {
        int nID;

        DESC("伤害实体类型")
        ENTITY_TYPE nEntityType;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int nSpellEffectType;

        DESC("操作者目标过滤")
        int nOperatorTargetFilter;

        DESC("目标目标过滤")
        int nTargetTargetFilter;

        DESC("目标类型")
        MA_TAGET_TYPE nTargetType;

        DESC("增加Buff目标对象")
        MA_TAGET_TYPE nAddBuffTagetType;

        DESC("BuffID")
        int nBuffID;

        DESC("Buff等级")
        int nBuffLevel;

        STRUCT_INIT(EffectServer_SpecificDamageAddBuff)
    };

    DESC("分担伤害")
    STRUCT EffectServer_ShareDamage
    {
        int nID;

        DESC("目标类型")
        MA_TAGET_TYPE nTargetType;

        DESC("伤害类型DAMAGE_TYPE")
        int nDamageType;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("固定、百分比")
        MA_VALUE_TYPE nValueType;

        DESC("数值(float)")
        float fValue;

        STRUCT_INIT(EffectServer_ShareDamage)
    };

    DESC("增加属性上限")
    STRUCT EffectServer_AddPropertyLimit
    {
        int nID;

        DESC("增加属性类型")
        ADD_PROPERTY_LIMIT_TYPE nPropertyType;

        DESC("数值")
        int nValue;

        STRUCT_INIT(EffectServer_AddPropertyLimit)
    };

    DESC("持续增加属性")
    STRUCT EffectServer_ContinuedAddProperty
    {
        int nID;

        DESC("间隔(毫秒)")
        int nInterval;

        DESC("次数")
        int nCount;

        DESC("是否首次执行")
        int	nFirst;

        DESC("增加属性类型")
        ADD_PROPERTY_TYPE nType;

        DESC("数值")
        int nValue;

        STRUCT_INIT(EffectServer_ContinuedAddProperty)
    };

    DESC("设置目标位置")
    STRUCT EffectServer_SetTargetLoc
    {
        int nID;

        DESC("使用技能点类型")
        USE_SPELL_POS_TYPE nType;

        DESC("最大距离")
        float fDistance;

        STRUCT_INIT(EffectServer_SetTargetLoc)
    };

    DESC("记录点返回")
    STRUCT EffectServer_RecordPointBack
    {
        int nID;

        DESC("技能列表")
        sp_char_array(strSpellID,32);

        STRUCT_INIT(EffectServer_RecordPointBack)
    };

    DESC("设置槽位等级")
    STRUCT EffectServer_SetSlotLevel
    {
        int nID;

        DESC("槽位类型")
        SLOT_TYPE nSlotType;

        DESC("槽位索引")
        int nSlotIndex;

        DESC("槽位等级")
        int nSlotLevel;

        DESC("技能ID")
        int nSpellID;

        STRUCT_INIT(EffectServer_SetSlotLevel)
    };

    DESC("效果增加物品")
    STRUCT EffectServer_AddGoods
    {
        int nID;

        DESC("增加物品ID")
        int nGoodsID;

        DESC("增加物品数量")
        int nGoodsCount;

        DESC("是否增加,true增加,false移除")
        MA_BOOL nAdd;

        STRUCT_INIT(EffectServer_AddGoods)
    };

    DESC("使用技能增加BUFF")
    STRUCT EffectServer_CastSpellAddBuff
    {
        int nID;

        DESC("匹配技能列表")
        sp_char_array(strSpellID,32);

        DESC("是否匹配技能")
        MA_BOOL bMatching;

        DESC("BUFF类型")
        BUFF_TYPE nType;

        DESC("BuffID")
        int nBuffID;

        DESC("BuffLevel")
        int nBuffLevel;

        STRUCT_INIT(EffectServer_CastSpellAddBuff)
    };

    DESC("施法技能检测BUFF")
    STRUCT EffectServer_CastSpellCheckBuff
    {
        int nID;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int	nSpellEffectType;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("BuffID")
        int nBuffID;

        DESC("BuffLevel")
        int nBuffLevel;

        DESC("是否存在BUFF")
        MA_BOOL bExist;

        STRUCT_INIT(EffectServer_CastSpellCheckBuff)
    };

    DESC("召唤分身怪物")
    STRUCT EffectServer_ReplicationMonster
    {
        int nID;

        DESC("效果结束移除怪物")
        MA_BOOL nRemove;

        DESC("移除怪物时间,0无时间限制(毫秒)")
        int nTime;

        DESC("刷怪ID")
        int nMonsterID;

        DESC("刷怪数量")
        int	nMonsterCount;

        DESC("刷怪位置")
        REPLICATION_MONSTER_POS_TYPE nPosType;

        DESC("方向列表(0前方,1后方,2左方,3右方)")
        sp_char_array(strDir,16);

        DESC("距离列表")
        sp_char_array(strDistance,32);

        DESC("取召唤者名字")
        MA_BOOL bSummonName;

        DESC("怪物附加名字")
        sp_char_array(strAppendName,16);

		DESC("继承召唤者等级比例(float),小于等于0不继承")
		float fLvFactor;

        DESC("继承血量比例(float),小于等于0不继承")
        float fHPFactor;

        DESC("继承魔法比例(float),小于等于0不继承")
        float fMPFactor;

        DESC("继承能量比例(float),小于等于0不继承")
        float fENERGYFactor;

        DESC("继承物理攻击比例(float),小于等于0不继承")
        float fPAFactor;

        DESC("继承法术攻击比例(float),小于等于0不继承")
        float fMAFactor;

        DESC("继承物理防御比例(float),小于等于0不继承")
        float fPDFactor;

        DESC("继承法术防御比例(float),小于等于0不继承")
        float fMDFactor;

        DESC("继承物理穿透比例(float),小于等于0不继承")
        float fPDPFactor;

        DESC("继承法术穿透比例(float),小于等于0不继承")
        float fMDPFactor;

        DESC("继承移动速度比例(float),小于等于0不继承")
        float fSDFactor;

        DESC("继承攻击速度比例(float),小于等于0不继承")
        float fPASDFactor;

        DESC("继承普攻系数比例(float),小于等于0不继承")
        float fAFFactor;

        DESC("继承暴击概率比例(float),小于等于0不继承")
        float fCRCFactor;

        DESC("技能ID召唤怪,0为调用效果技能")
        int nSpellID;

        DESC("召唤怪物上限,不要求创建怪物上限的设置为0")
        int nMaxCount;

        DESC("是否继承主人装备效果")
        MA_BOOL bInheritMasterEquipEffect;

        DESC("怪物方向")
        DIRECTION_TYPE nMonsterDirType;

        STRUCT_INIT(EffectServer_ReplicationMonster)
    };

    DESC("静止增加Buff")
    STRUCT EffectServer_StaticAddBuff
    {
        int nID;

        DESC("触发频率(毫秒)")
        int nInterval;

        DESC("增加BuffID")
        int nBuffID;

        DESC("增加BuffLevel")
        int nBuffLevel;

        DESC("是否排除飞行状态(是:飞行静止不算静止)")
        MA_BOOL bExcludeFlying;

        DESC("是否排除死亡状态(是:死亡不算静止)")
        MA_BOOL bExcludeDead;

        STRUCT_INIT(EffectServer_StaticAddBuff)
    };

    DESC("检测BUFF伤害加成")
    STRUCT EffectServer_CheckBuffDamageBonus
    {
        int nID;

        DESC("BUFF增加对象")
        MA_TAGET_TYPE nTargetType;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int	nSpellEffectType;

        DESC("百分比系数(float)")
        float fValue;

        STRUCT_INIT(EffectServer_CheckBuffDamageBonus)
    };

    DESC("攻击次数增加Buff")
    STRUCT EffectServer_AttackCountAddBuff
    {
        int nID;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int nSpellEffectType;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("攻击增加BUF次数")
        int nCount;

        DESC("触发频率(毫秒)")
        int nInterval;

        DESC("增加BuffID列表")
        sp_char_array(strBuffID, 32);

        DESC("增加BuffLevel列表")
        sp_char_array(strBuffLevel, 32);

        DESC("攻击超时间隔(毫秒)")
        int nValidAttackInterval;

        STRUCT_INIT(EffectServer_AttackCountAddBuff)
    };

    DESC("归属击杀获得BUFF")
    STRUCT EffectServer_BeLongsKillAddBuff
    {
        int nID;

        DESC("检测BuffID")
        int nCheckBuffID;

        DESC("检测BuffLevel")
        int nCheckBuffLevel;

        DESC("死亡目标过滤")
        int nDieTargetFilter;

        DESC("死亡者BuffID")
        int nDieBuffID;

        DESC("死亡者BuffLevel")
        int nDieBuffLevel;

        DESC("击杀目标过滤")
        int nKillTargetFilter;

        STRUCT_INIT(EffectServer_BeLongsKillAddBuff)
    };

    DESC("攻击增加暴击概率")
    STRUCT EffectServer_AttackAddCRD
    {
        int nID;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int	nSpellEffectType;

        DESC("触发频率(毫秒)")
        int nInterval;

        DESC("增加暴击概率")
        int nValue;

        DESC("增加暴击概率上限")
        int nLimitValue;

        STRUCT_INIT(EffectServer_AttackAddCRD)
    };

    DESC("限制技能使用次数")
    STRUCT EffectServer_DisableSpellUseCount
    {
        int nID;

        DESC("限制技能ID")
        int nSpellID;

        DESC("限制技能使用次数")
        int nSpellCount;

        DESC("最大次数后重置时间间隔ms")
        int nResetInterval;

        DESC("使用到上限后提示")
        int nTipID;

        STRUCT_INIT(EffectServer_DisableSpellUseCount)
    };

    DESC("延迟使用技能")
    STRUCT EffectServer_DelayUseSpell
    {
        int nID;

        DESC("间隔，毫秒")
        int nInterval;

        DESC("次数")
        int nCount;

        DESC("技能ID")
        int nSpellID;

        DESC("技能目标对象")
        MA_TAGET_TYPE nTargetType;

        DESC("技能起点类型")
        DELAY_USE_SPELL_POS_TYPE nStartPosType;

        DESC("技能目标点类型")
        DELAY_USE_SPELL_POS_TYPE nTargetPosType;

        STRUCT_INIT(EffectServer_DelayUseSpell)
    };

    DESC("冲锋")
    STRUCT EffectServer_Charge
    {
        int nID;

        DESC("速度(float)")
        float fSpeed;

        DESC("距离(float)")
        float fDistance;

        DESC("高度(float)")
        float fHeight;

        DESC("最大时间(毫秒)")
        int nMaxTime;

        DESC("离目标点距离(float)")
        float fLocationDistance;

        DESC("冲锋打断距离,0不打断(float)")
        float fBreakDistance;

        DESC("锁定目标")
        MA_BOOL nLockTarget;

        DESC("锁定目标时间,超过时间打断(毫秒)")
        int nLockTime;

        DESC("操作者对象")
        MA_TAGET_TYPE nOperatorType;

        DESC("目标对象")
        MA_TAGET_TYPE nTargetType;

        DESC("选择起点")
        CHOOS_POS_TYPE nStartPosType;

        DESC("选择终点")
        CHOOS_POS_TYPE nEndPosType;

        DESC("被动位移类型")
        PASSIVE_MOVE_TYPE nPassiveMoveType;

        DESC("冲锋优先级")
        int nPriority;

        DESC("冲锋最大仰角(度)")
        int nMaxUpAngle;

        DESC("冲锋是否可以跨越障碍")
        MA_BOOL bCanCrossObstacle;

        DESC("冲锋遇到障碍加buffID")
        int nHitObstacleAddBuffID;

        DESC("冲锋遇到障碍加buff等级")
        int nHitObstacleAddBuffLevel;

        DESC("是否是抛物线")
        MA_BOOL bUseHeight;

        DESC("冲锋完加buffID,默认填0")
        int nFinishBuff_ID;

        DESC("冲锋完buffLevel")
        int nFinishBuff_Level;

        DESC("冲锋完加buff目标对象")
        MA_TAGET_TYPE nFinishBuff_Target;

        DESC("冲锋完buff:buff添加者")
        MA_TAGET_TYPE nFinishBuff_Operator;

        STRUCT_INIT(EffectServer_Charge)
    };

    DESC("击退,击飞")
    STRUCT EffectServer_Repulse
    {
        int nID;

        DESC("速度(float)")
        float fSpeed;

        DESC("距离(float)")
        float fDistance;

        DESC("高度(float)")
        float fHeight;

        DESC("最大时间(毫秒)")
        int nMaxTime;

        DESC("离目标点距离(float)")
        float fLocationDistance;

        DESC("目标对象")
        MA_TAGET_TYPE nTargetType;

        DESC("选择起点")
        CHOOS_POS_TYPE nStartPosType;

        DESC("选择终点")
        CHOOS_POS_TYPE nEndPosType;

        DESC("被动位移类型")
        PASSIVE_MOVE_TYPE nPassiveMoveType;

        DESC("冲锋优先级")
        int nPriority;

        DESC("冲锋最大仰角(度)")
        int nMaxUpAngle;

        DESC("冲锋是否可以跨越障碍")
        MA_BOOL bCanCrossObstacle;

        DESC("冲锋遇到障碍加buffID")
        int nHitObstacleAddBuffID;

        DESC("冲锋遇到障碍加buff等级")
        int nHitObstacleAddBuffLevel;

        DESC("是否是抛物线")
        MA_BOOL bUseHeight;

        DESC("冲锋完加buffID,默认填0")
        int nFinishBuff_ID;

        DESC("冲锋完buffLevel")
        int nFinishBuff_Level;

        DESC("冲锋完加buff目标对象")
        MA_TAGET_TYPE nFinishBuff_Target;

        DESC("冲锋完buff:buff添加者")
        MA_TAGET_TYPE nFinishBuff_Operator;

        STRUCT_INIT(EffectServer_Repulse)
    };

    DESC("撞飞")
    STRUCT EffectServer_Strike
    {
        int nID;

        DESC("速度(float)")
        float fSpeed;

        DESC("距离(float)")
        float fDistance;

        DESC("高度(float)")
        float fHeight;

        DESC("最大时间(毫秒)")
        int nMaxTime;

        DESC("离目标点距离(float)")
        float fLocationDistance;

        DESC("操作者对象")
        MA_TAGET_TYPE nOperatorType;

        DESC("目标对象")
        MA_TAGET_TYPE nTargetType;

        DESC("选择起点")
        CHOOS_POS_TYPE nStartPosType;

        DESC("选择终点")
        CHOOS_POS_TYPE nEndPosType;

        DESC("被动位移类型")
        PASSIVE_MOVE_TYPE nPassiveMoveType;

        DESC("冲锋优先级")
        int nPriority;

        DESC("冲锋最大仰角(度)")
        int nMaxUpAngle;

        DESC("冲锋是否可以跨越障碍")
        MA_BOOL bCanCrossObstacle;

        DESC("冲锋遇到障碍加buffID")
        int nHitObstacleAddBuffID;

        DESC("冲锋遇到障碍加buff等级")
        int nHitObstacleAddBuffLevel;

        DESC("是否是抛物线")
        MA_BOOL bUseHeight;

        DESC("冲锋完加buffID,默认填0")
        int nFinishBuff_ID;

        DESC("冲锋完buffLevel")
        int nFinishBuff_Level;

        DESC("冲锋完加buff目标对象")
        MA_TAGET_TYPE nFinishBuff_Target;

        DESC("冲锋完buff:buff添加者")
        MA_TAGET_TYPE nFinishBuff_Operator;

        STRUCT_INIT(EffectServer_Strike)
    };

    DESC("翻滚")
    STRUCT EffectServer_Roll
    {
        int nID;

        DESC("速度(float)")
        float fSpeed;

        DESC("距离(float)")
        float fDistance;

        DESC("高度(float)")
        float fHeight;

        DESC("最大时间(毫秒)")
        int nMaxTime;

        DESC("离目标点距离(float)")
        float fLocationDistance;

        DESC("被动位移类型")
        PASSIVE_MOVE_TYPE nPassiveMoveType;

        DESC("冲锋优先级")
        int nPriority;

        DESC("冲锋最大仰角(度)")
        int nMaxUpAngle;

        DESC("冲锋是否可以跨越障碍")
        MA_BOOL bCanCrossObstacle;

        DESC("冲锋遇到障碍加buffID")
        int nHitObstacleAddBuffID;

        DESC("冲锋遇到障碍加buff等级")
        int nHitObstacleAddBuffLevel;

        DESC("是否是抛物线")
        MA_BOOL bUseHeight;

        DESC("冲锋完加buffID,默认填0")
        int nFinishBuff_ID;

        DESC("冲锋完buffLevel")
        int nFinishBuff_Level;

        DESC("冲锋完加buff目标对象")
        MA_TAGET_TYPE nFinishBuff_Target;

        DESC("冲锋完buff:buff添加者")
        MA_TAGET_TYPE nFinishBuff_Operator;

        STRUCT_INIT(EffectServer_Roll)
    };

    DESC("透视显示")
    STRUCT EffectServer_PerspectiveShow
    {
        int nID;

        DESC("次数，小于等于0无限次")
        int	nCount;

        DESC("间隔时间")
        int	nTime;

        DESC("距离(float)")
        float fDistance;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("最大玩家数量")
        int	nMaxCount;

        STRUCT_INIT(EffectServer_PerspectiveShow)
    };

    DESC("物品限制购买")
    STRUCT EffectServer_GoodBuyLimit
    {
        int nID;

        DESC("限制购买物品ID")
        sp_char_array(strGoodID,32);

        DESC("购买返回码")
        EWarCheckBuyGoodErrorCode nType;

        STRUCT_INIT(EffectServer_GoodBuyLimit)
    };

    DESC("惊险救治")
    STRUCT EffectServer_ThrillingCure
    {
        int nID;

        DESC("范围检测")
        float fCheckDistans;

        DESC("触发事件时间")
        int nSendEventTime;

        STRUCT_INIT(EffectServer_ThrillingCure)
    };

    DESC("夺命控制")
    STRUCT EffectServer_KillingControl
    {
        int nID;

        DESC("触发事件时间")
        int nSendEventTime;

        STRUCT_INIT(EffectServer_KillingControl)
    };

    DESC("团控效果")
    STRUCT EffectServer_GroupControl
    {
        int nID;

        DESC("等级")
        int nLevel;

        STRUCT_INIT(EffectServer_GroupControl)
    };
    
    DESC("死亡召唤怪物")
    STRUCT EffectServer_DieSummonMonster
    {
        int nID;

        DESC("召唤怪物ID")
        int nMonsterID;

        DESC("召唤怪物数量")
        int	nMonsterNum;

        DESC("主人对象")
        MA_TAGET_TYPE nMasterType;

        DESC("选择点")
        REPLICATION_MONSTER_POS_TYPE nPosType;
        
        DESC("基础等级")
        int nBaseLevel;

        DESC("影响属性")
        EXTRAATTACK_TYPE nExtraAttackType;

        DESC("攻击百分比，扩大100倍")
        int	nSpellAttackPCT;

        STRUCT_INIT(EffectServer_DieSummonMonster)
    };

    DESC("受伤累积次数攻击增加Buff")
    STRUCT EffectServer_DamageCountAttackAddBuff
    {
        int nID;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int nSpellEffectType;

        DESC("伤害类型DAMAGE_TYPE")
        int nDamageType;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("累积次数上限")
        int nMaxCount;

        DESC("次数Buff时间")
        int nBuffTime;

        DESC("技能匹配列表")
        sp_char_array(strSpellID, 32);

        DESC("次数列表")
        sp_char_array(strCountID, 32);

        DESC("增加BuffID列表")
        sp_char_array(strBuffID, 32);

        DESC("增加BuffLevel列表")
        sp_char_array(strBuffLevel, 32);

        STRUCT_INIT(EffectServer_DamageCountAttackAddBuff)
    };

    DESC("区域伤害传递效果")
    STRUCT EffectServer_RangeDamageTransmit
    {
        int nID;

        DESC("触发频率(毫秒)")
        int nInterval;

        DESC("目标点点类型")
        TURN_TO_POS_TYPE nType;

        DESC("加buff距离(float)")
        float fDistance;

        DESC("检测BUFFID")
        int nCheckBuffID;

        DESC("检测BUFF等级")
        int nCheckBuffLevel;

        DESC("系数(float)")
        float fValue;

        STRUCT_INIT(EffectServer_RangeDamageTransmit)
    };

    DESC("选择范围增加Buff")
    STRUCT EffectServer_ChooseRangeAddBuff
    {
        int nID;

        DESC("选择范围类型")
        CHOOSE_RANGE_TYPE nChooseRangeType;

        DESC("使用技能点类型")
        USE_SPELL_POS_TYPE nUseSpellPosType;

        DESC("攻击范围")
        float fAttackRange;

        DESC("技能距离")
        float fDistance;

        DESC("技能攻击高度")
        float fAttackHeight;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("最大检测数量(0表示不限制数量)")
        int nMaxCheckCount;

        DESC("次数列表")
        sp_char_array(strCountID, 32);

        DESC("增加BuffID列表")
        sp_char_array(strBuffID, 32);

        DESC("增加BuffLevel列表")
        sp_char_array(strBuffLevel, 32);

        STRUCT_INIT(EffectServer_ChooseRangeAddBuff)
    };

    DESC("战场怪物增加Buff")
    STRUCT EffectServer_WarMonsterAddBuff
    {
        int nID;

        DESC("战场阵营类型")
        EWarCampType nCampType;

        DESC("怪物处理方式")
        EWarMonsterExec nMonsterExec;

        DESC("操作BUFF类型")
        BUFF_TYPE nType;

        DESC("BuffID")
        int nBuffID;

        DESC("BuffLevel")
        int nBuffLevel;

        STRUCT_INIT(EffectServer_WarMonsterAddBuff)
    };

    DESC("地图飞檐走壁")
    STRUCT EffectServer_MapFly
    {
        int nID;

        DESC("最大高度(float)")
        float fWalkHeight;

        STRUCT_INIT(EffectServer_MapFly)
    };

    DESC("禁用学习天赋")
    STRUCT EffectServer_DisableActiveTalent
    {
        int nID;

        DESC("禁用学习天赋,0为禁用所有")
        sp_char_array(strTalentID, 32);

        STRUCT_INIT(EffectServer_DisableActiveTalent)
    };

    DESC("持续攻击(受击)增加buff")
    STRUCT EffectServer_ContinuousAttackBeHitAddBuff
    {
        int nID;

        DESC("攻击or受击")
        ATTACK_OR_BEHIT nAttackOrBeHit;

        DESC("连续攻击/受击计数次数")
        int nTimes;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("技能效果类型SPELL_EFFECT_TYPE")
        int	nSpellEffectType;

        DESC("计数超时时间ms")
        int nCountTimeOut;

        DESC("buffID")
        int nBuffID;

        DESC("buff等级")
        int nBuffLevel;

        STRUCT_INIT(EffectServer_ContinuousAttackBeHitAddBuff)
    };

    DESC("被嘲讽")
    STRUCT EffectServer_BeSneered
    {
        int nID;

        DESC("普攻槽位")
        int nForceAttackSpellSlot;

        DESC("贴近距离(float)")
        float fCloseDistance;

		DESC("超时时间(ms)")
		float fTimeout;

        STRUCT_INIT(EffectServer_BeSneered)
    };


    DESC("数据通知客户端")
    STRUCT EffectServer_NotifyClientData
    {
        int nID;

        DESC("通知数据类型")
        NOTIFY_DATA_TYPE nType;

        DESC("数值")
        int nValue;

        STRUCT_INIT(EffectServer_NotifyClientData)
    };

    DESC("鼠标右键按下时加buff")
    STRUCT EffectServer_AddBuffOnRightButtonDown
    {
        int nID;

        DESC("buffID")
        int nBuffID;

        DESC("buff等级")
        int nBuffLevel;

        STRUCT_INIT(EffectServer_AddBuffOnRightButtonDown)
    };

    DESC("停止当前技能")
    STRUCT EffectServer_StopSpell
    {
        int nID;

        DESC("停止技能ID")
        int nSpellID;

        STRUCT_INIT(EffectServer_StopSpell)
    };

    DESC("打印AI信息")
    STRUCT EffectServer_TraceAI
    {
        int nID;

        STRUCT_INIT(EffectServer_TraceAI)
    };

	DESC("准备子弹")
	STRUCT EffectServer_PrepareBullet
	{
		int nID;

		DESC("子弹数量")
		int nBulletCount;

		STRUCT_INIT(EffectServer_PrepareBullet)
	};

	DESC("减少子弹数量")
	STRUCT EffectServer_DecBulletCount
	{
		int nID;

		DESC("消耗数量")
		int nCount;

		STRUCT_INIT(EffectServer_DecBulletCount)
	};

	DESC("子弹数为0时使用技能")
	STRUCT EffectServer_UseSpellIfNoBullet
	{
		int nID;

		DESC("技能ID")
		int nSpellID;

		STRUCT_INIT(EffectServer_UseSpellIfNoBullet);
	};

    DESC("设置可以通过结界标志")
    STRUCT EffectServer_SetCanPassBarrier
    {
        int nID;

        STRUCT_INIT(EffectServer_SetCanPassBarrier);
    };

    DESC("增加阻挡结界")
    STRUCT EffectServer_CreateMoveBarrier
    {
        int nID;

        DESC("形状")
        BarrierShape eShape;

        DESC("过滤标识")
        int nTargetFilter;

        DESC("圆环:圆心位置")
        CHOOS_POS_TYPE locCircleCenterLoc;
        DESC("圆环:内径")
        float fCircleInnerRadius;
        DESC("圆环:外径")
        float fCircleOutterRadius;
        DESC("圆环:高")
        float fCircelHeight;

        DESC("直线:起点")
        CHOOS_POS_TYPE locLineStart;
        DESC("直线:终点")
        CHOOS_POS_TYPE locLineEnd;
        DESC("直线:宽度")
        float fLineWidth;
        DESC("直线:高度")
        float fLineHeight;

        DESC("圆柱:圆心位置")
        CHOOS_POS_TYPE locCylinderCenterLoc;
        DESC("圆柱:半径")
        float fCylinderRadius;
        DESC("圆柱:高度")
        float fCylinderHeight;

        DESC("最大持续时间ms(-1表示无限制)")
        int nMaxTime;

        STRUCT_INIT(EffectServer_CreateMoveBarrier);
    };

	DESC("检测CD增加效果")
	STRUCT EffectServer_CheckCDAddEffect
	{
		int nID;
		DESC("冷却ID")
		int nFreezeID;
		DESC("效果ID")
		int nEffectID;

		STRUCT_INIT(EffectServer_CheckCDAddEffect)
	};

	DESC("战场改变职业")
	STRUCT EffectServer_WarChangeVoc
	{
		int nID;

		STRUCT_INIT(EffectServer_WarChangeVoc)
	};

	DESC("假死者复活")
	STRUCT EffectServer_FakeDeadRelive
	{
		int nID;

		STRUCT_INIT(EffectServer_FakeDeadRelive)
	};
};