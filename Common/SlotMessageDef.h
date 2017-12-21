/*******************************************************************
** 文件名:	SlotMessageDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/23/2015
** 版  本:	1.0
** 描  述:	Slot消息定义
********************************************************************/

#ifndef __SLOT_MESSAGE_DEF_H__
#define __SLOT_MESSAGE_DEF_H__

#include "NetMessageDef.h"
#include "Slot_ManagedDef.h"
#include "SlotDef.h"

#pragma pack(1)

/////////////////////////// Slot模块消息  ////////////////////////////
// 消息码定义键值
enum SC_SLOT_NET_MESSAGE
{
	SC_NET_MSG_SLOT = SC_MSG_SLOT_START,		// 槽位消息
	SC_MSG_UPDATE_SKILL_SLOT,
	SC_MSG_UPDATE_GOODS_SLOT,
	SC_MSG_UPDATE_CHANGED_SKIN
	
};

//SC_MSG_UPDATE_SKILL_SLOT
struct msg_slot_update_skill_slot
{
	bool			bIsAdd;
	int				nSlotIndex;				// 槽索引
	SLOT_TYPE		nSlotType;				// 槽位类型	(1、装备 2、技能)			
	int				nSkillID;				// 技能ID
	int				nSlotLevel;				// 槽位等级

	msg_slot_update_skill_slot(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//SC_MSG_UPDATE_SKILL_SLOT
struct msg_slot_update_skin_changed
{
	UID	uUID;

	msg_slot_update_skin_changed(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

#pragma pack()
#endif	// __SLOT_MESSAGE_DEF_H__