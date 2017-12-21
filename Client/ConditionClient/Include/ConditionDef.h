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

namespace CONDITION_CLIENT
{
	//=================================================================================
	DESC("检查生命值")
	STRUCT ConditionClient_HP
	{
		int nID;

		DESC("目标类型")
		MA_TAGET_TYPE nMethod;

		DESC("生命固定值/百分比")
		MA_VALUE_TYPE nValueType;

		DESC("最低值")
		int	nLowVal;

		DESC("最高值")
		int nHighVal;

		DESC("校验失败飘字ID")
        int nFailFloatText;

		STRUCT_INIT(ConditionClient_HP)
	};

	//=================================================================================
	DESC("检查时间")
	STRUCT ConditionClient_Time
	{
		int nID;

		DESC("第几帧触发")
		int nFrameIndex;

		STRUCT_INIT(ConditionClient_Time)
	};

	//=================================================================================
	DESC("检查冷却")
	STRUCT ConditionClient_CoolDown
	{
		int nID;

		DESC("冷却组ID")
		EFREEZE_CLASS nClassID;

		DESC("冷却ID")
		int nCoolDownID;

		STRUCT_INIT(ConditionClient_CoolDown)
	};

	//=================================================================================
	DESC("检查法力")
	STRUCT ConditionClient_MP
	{
		int nID;

		DESC("法力值")
		int nValue;

		STRUCT_INIT(ConditionClient_MP)
	};

	//=================================================================================
	DESC("检查阵营")
	STRUCT ConditionClient_Camp
	{
		int nID;

		DESC("阵营掩码")
		int nCampMask;

		STRUCT_INIT(ConditionClient_Camp)
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

	//=================================================================================
	DESC("检查距离")
	STRUCT ConditionClient_Distance
	{
		int nID;

		DESC("要求距离")
		float fDistance;
		DESC("距离类型")
		DISTANCE_TYPE  nType;

		STRUCT_INIT(ConditionClient_Distance)
	};

	//=================================================================================
	DESC("检查体力")
	STRUCT ConditionClient_Stamina
	{
		int nID;

		DESC("体力值")
		int nValue;

		STRUCT_INIT(ConditionClient_Stamina)
	};

	DESC("检查能量")
	STRUCT ConditionClient_Energy
	{
		int nID;

		DESC("能量值")
		int nValue;

		STRUCT_INIT(ConditionClient_Energy)
	};



    DESC("检测战斗状态")
    STRUCT ConditionClient_CheckFightState
    {
        int nID;

        DESC("是否战斗状态")
        MA_BOOL bIsFight;

        DESC("检测失败时提示TipID")
        int nTipID;

        STRUCT_INIT(ConditionClient_CheckFightState)
    };

    DESC("目标检查")
    STRUCT ConditionClient_CheckTarget
    {
        int nID;

        DESC("目标过滤")
        int nTargetFilter;

        STRUCT_INIT(ConditionClient_CheckTarget)
    };

    DESC("检测BUFF")
    STRUCT ConditionClient_CheckBuff
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

        STRUCT_INIT(ConditionClient_CheckBuff)
    };

}