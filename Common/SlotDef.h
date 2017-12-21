/*******************************************************************
** 文件名:	SlotDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/23/2015
** 版  本:	1.0
** 描  述:	战场公用定义

			
********************************************************************/


#ifndef __COMMON_SLOT_DEF_H__
#define __COMMON_SLOT_DEF_H__

#pragma pack(1)

#include "Slot_ManagedDef.h"

/////////////////////////槽位配置/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

struct SSlotSkillInfo
{
	int				nSlotIndex;				// 槽索引
	SLOT_TYPE		nSlotType;				// 槽位类型	(1、装备 2、技能)			
	int				nSkillID;				// 技能ID
	int				nSlotLevel;

	SSlotSkillInfo()
	{
		nSkillID = nSlotIndex = 0;
		nSlotType = SLOT_TYPE_SKILL;		// 默认使用技能槽位
		nSlotLevel = 0;
	}
};

struct SSlotGoodsInfo
{
	int nSlotIndex;
	SLOT_TYPE nSlotType;
	int nGoodID;
	bool bCanUse;
	int nCanUseTimes;
	int nFreezeID;
	int nSkillID;
	int nBuffID;
	bool bIsNewAdd;

	SSlotGoodsInfo()
	{
		nBuffID = nSkillID = nFreezeID = nCanUseTimes = nGoodID = nSkillID = nSlotIndex = 0;
		nSlotType = SLOT_TYPE::SLOT_TYPE_GOODS;		// 默认使用物品槽位
		bCanUse = false;
	}
};

#pragma pack()
#endif	// __COMMON_SLOT_DEF_H__