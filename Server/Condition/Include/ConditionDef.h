/*******************************************************************
** 文件名:	ConditionDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	penglu
** 日  期:	2014/12/25
** 版  本:	1.0
** 描  述:	条件模块相关定义
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "ManagedCode.h"
#include "SpellDef.h"
using namespace SPELL;

namespace CONDITION_SERVER
{
    DESC("操作运算符")
    ENUM OPERATOR
    {
        DESC("小于")
        OPERATOR_LESS_THAN,
        DESC("不大于（小于等于）")
        OPERATOR_LESS_THAN_OR_EQUAL,
        DESC("等于")
        OPERATOR_EQUAL,
        DESC("不等于")
        OPERATOR_NOT_EQUAL,
        DESC("大于")
        OPERATOR_GREATER_THAN,
        DESC("不少于（大于等于）")
        OPERATOR_GREATER_THAN_OR_EQUAL,
    };

	DESC("检查属性")
	STRUCT ConditionServer_CheckProperty
	{
		int nID;

		DESC("目标类型")
		MA_TAGET_TYPE nTagetType;

		DESC("属性类型")
		MA_PROPERTY_TYPE nPropertyType;

        DESC("操作运算符")
        OPERATOR nOperator;

		DESC("值")
		int nValue;

		STRUCT_INIT(ConditionServer_CheckProperty)
	};

	DESC("检查属性范围")
	STRUCT ConditionServer_CheckPropertyRange
	{
		int nID;

		DESC("目标类型")
		MA_TAGET_TYPE nTagetType;

		DESC("属性类型")
		MA_PROPERTY_TYPE nPropertyType;

		DESC("固定值/百分比")
		MA_VALUE_TYPE nValueType;

		DESC("最低值")
		int	nLowValue;

		DESC("最高值")
		int nHighValue;

		STRUCT_INIT(ConditionServer_CheckPropertyRange)
	};

	//=================================================================================
	DESC("检查技能标志类型")
	ENUM CHECK_SPELLFLAG_TYPE
	{
		DESC("判断小于")
		CHECK_SPELLFLAG_LESS = 0,
		DESC("判断等于")
		CHECK_SPELLFLAG_EQUAL,
		DESC("判断大于")
		CHECK_SPELLFLAG_GREATER,
		DESC("判断小于等于")
		CHECK_SPELLFLAG_LESS_EQUAL,
		DESC("判断大于等于")
		CHECK_SPELLFLAG_GREATER_EQUAL,
	};
	DESC("检查技能标志")
	STRUCT ConditionServer_SpellFlag
	{
		int nID;

		DESC("技能标志")
		CHECK_SPELLFLAG_TYPE	nType;

		DESC("技能标志位某位置值")
		int		nIndex;

		DESC("标志值")
		int nValue;

        DESC("检测技能ID,0调用技能,不为0配置技能")
        int nCheckSpellID;

		STRUCT_INIT(ConditionServer_SpellFlag)
	};

	//=================================================================================
	DESC("检查时间")
	STRUCT ConditionServer_Time
	{
		int nID;

		DESC("第几帧触发")
		int nFrameIndex;

		STRUCT_INIT(ConditionServer_Time)
	};

	//=================================================================================
	DESC("检查冷却")
	STRUCT ConditionServer_CoolDown
	{
		int nID;

		DESC("冷却组ID")
		EFREEZE_CLASS nClassID;

		DESC("冷却ID")
		int nCoolDownID;

		STRUCT_INIT(ConditionServer_CoolDown)
	};

	//=================================================================================
	DESC("检查阵营")
	STRUCT ConditionServer_Camp
	{
		int nID;

		DESC("阵营掩码")
		int nCampMask;

		STRUCT_INIT(ConditionServer_Camp)
	};

	//=================================================================================
	DESC("距离类型")
	ENUM DISTANCE_TYPE
	{
		DESC("目标Tile")
		DISTANCE_TILE = 0,
		DESC("目标实体")
		DISTANCE_TARGET,
	};

	DESC("检查距离")
	STRUCT ConditionServer_Distance
	{
		int nID;

        DESC("范围最小检测距离")
        float fMinDistance;

		DESC("范围最大检测距离")
		float fMaxDistance;

		DESC("距离类型")
		DISTANCE_TYPE  nType;

		STRUCT_INIT(ConditionServer_Distance)
	};

	//=================================================================================
	DESC("目标检查")
	STRUCT ConditionServer_Target
	{
		int nID;

		DESC("目标过滤")
		int nTargetFilter;

		STRUCT_INIT(ConditionServer_Target)
	};

	//=================================================================================
	DESC("实体死亡凶手阵营")
	STRUCT ConditionServer_EntityDieKillerCamp
	{
		int nID;

		DESC("目标过滤")
		int nTargetFilter;

		STRUCT_INIT(ConditionServer_EntityDieKillerCamp)
	};

	//=================================================================================
	DESC("死亡实体类型")
	ENUM DIE_ENTITY_TYPE
	{
		DESC("所有类型")
		DIE_ENTITY_ALL			= 0x7FFFFFFF,
		DESC("英雄")
		DIE_ENTITY_PLAYER_ROLE = 0x1,
		DESC("怪物")
		DIE_ENTITY_MONSTER	   = 0x2,
		DESC("载具")
		DIE_ENTITY_TANK		   = 0x4,
	};

	//=================================================================================
	DESC("死亡实体类型")
	STRUCT ConditionServer_DieEntityType
	{
		int nID;

		DESC("实体类型")
		int nType;

		STRUCT_INIT(ConditionServer_DieEntityType)
	};

	//=================================================================================
	DESC("检测BUFF")
	STRUCT ConditionServer_CheckBuff
	{
		int nID;

		DESC("目标类型")
		MA_TAGET_TYPE nTagetType;

		DESC("buffID")
		int nBuffID;

		DESC("buffLevel")
		int nBuffLevel;

		DESC("是否存在BUFF")
		MA_BOOL bExist;

		DESC("查找Buff添加者对象")
		MA_TAGET_TYPE nBuffAddType;

		STRUCT_INIT(ConditionServer_CheckBuff)
	};

    DESC("寻找目标")
    STRUCT ConditionServer_FindTarget
    {
        int nID;

        DESC("寻找范围(float)")
        float fDist;

        DESC("目标过滤")
        int nTargetFilter;

        STRUCT_INIT(ConditionServer_FindTarget)
    };

    DESC("检测蓄力百分比")
    STRUCT ConditionServer_CheckFocoEnergiaPCT
    {
        int nID;

        DESC("最小蓄力百分比(float)")
        float fMinFocoEnergiaPCT;

        DESC("最大蓄力百分比(float)")
        float nMaxFocoEnergiaPCT;

        STRUCT_INIT(ConditionServer_CheckFocoEnergiaPCT)
    };

    DESC("检测改变翅膀状态")
    STRUCT ConditionServer_CheckChangeWingState
    {
        int nID;

        DESC("翅膀状态类型")
        WING_STATE_TYPE nStateType;

        STRUCT_INIT(ConditionServer_CheckChangeWingState)
    };

    DESC("检查BUFF改变目标")
    STRUCT ConditionServer_CheckBuffChangeTarget
    {
        int nID;

        DESC("距离(float)")
        float fDistance;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("buffID")
        int nBuffID;

        DESC("buffLevel")
        int nBuffLevel;

        DESC("是否存在BUFF")
        MA_BOOL bExist;

        DESC("查找Buff添加者对象")
        MA_TAGET_TYPE nBuffAddType;

        DESC("是否保持技能选点")
        MA_BOOL bKeepTile;

        STRUCT_INIT(ConditionServer_CheckBuffChangeTarget)
    };

    DESC("检测BUFF类型")
    STRUCT ConditionServer_CheckBuffType
    {
        int nID;

        DESC("目标类型")
        MA_TAGET_TYPE nTagetType;

        DESC("目标过滤")
        int nTargetFilter;

        DESC("Buff类型")
        int nBuffType;

        DESC("是否存在BUFF")
        MA_BOOL bExist;

        STRUCT_INIT(ConditionServer_CheckBuffType)
    };

    DESC("检查地图飞檐走壁")
    STRUCT ConditionServer_CheckMapFly
    {
        int nID;

        DESC("类型,随意填")
        int nType;

        STRUCT_INIT(ConditionServer_CheckMapFly)
    };


    DESC("检查充能")
    STRUCT ConditionServer_CheckCharge
    {
        int nID;

        DESC("操作运算符")
        OPERATOR nOperator;

        DESC("充能数值")
        int nValue;

        STRUCT_INIT(ConditionServer_CheckCharge)
    };


    DESC("检测战斗状态")
    STRUCT ConditionServer_CheckFightState
    {
        int nID;

        DESC("是否战斗状态")
        MA_BOOL bIsFight;

        DESC("检测失败时提示TipID")
        int nTipID;

        STRUCT_INIT(ConditionServer_CheckFightState)
    };

	DESC("检测子弹数量")
	STRUCT ConditionServer_CheckBulletCount
	{
		int nID;

        DESC("比较")
        OPERATOR eCompare;

		DESC("数量")
		int nCount;

		STRUCT_INIT(ConditionServer_CheckBulletCount)
	};


    DESC("检查技能槽位等级")
    STRUCT ConditionServer_CheckSpellSlotLevel
    {
        int nID;

        DESC("技能槽位")
        int nSlotIndex;

        DESC("操作运算符")
        OPERATOR nOperator;

        DESC("槽位等级")
        int nValue;

        STRUCT_INIT(ConditionServer_CheckSpellSlotLevel)
    };


}