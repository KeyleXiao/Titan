/*******************************************************************
** 文件名:	BuffMessageDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/22/2015
** 版  本:	1.0
** 描  述:	Buff消息定义
********************************************************************/

#ifndef __BUFF_MESSAGE_DEF_H__
#define __BUFF_MESSAGE_DEF_H__

#include "NetMessageDef.h"

#pragma pack(1)

/////////////////////////// Buff模块消息  ////////////////////////////
// 消息码定义键值
enum BUFF_NET_MESSAGE
{
	BUFF_MSG_ADD_BUFF = 0,		        // 增加Buff
	BUFF_MSG_REMOVE_BUFF,				// 移除Buff
	BUFF_MSG_CHARGE_COUNT_CHANGE,       // BUFF上充能层数改变
    BUFF_MSG_REMOVE_INDEX,              // BUFF索引移除
};


// 场景服-客户端 增加buff           BUFF_MSG_ADD_BUFF
struct SBuffMsgAddBuff_SC
{
	DWORD		dwIndex;		// 序号
	DWORD		dwBuffID;		// BUFF ID
	WORD		dwBuffLevel;	// BUFF 等级
	DWORD		dwTime;			// 时间长
	UID			uidAdd;			// 增加buff者
	Vector3		targetTile;		// 加buff时候鼠标位置
	int			nSpellID;		// 技能ID

	SBuffMsgAddBuff_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-客户端 移除buff               BUFF_MSG_REMOVE_BUFF
struct SBuffMsgRemoveBuff_SC
{
	DWORD		dwIndex;		// 序号

	SBuffMsgRemoveBuff_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-场景服 增加buff                BUFF_MSG_ADD_BUFF
struct SBuffMsgAddBuff_SS
{
	UID			uidAddBuff;		// 增加buff人
	int			dwBuffID;		// BUFF ID
	int			dwBuffLevel;	// BUFF 等级

	SBuffMsgAddBuff_SS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-场景 移除buff                    BUFF_MSG_REMOVE_BUFF
struct SBuffMsgRemoveBuff_SS
{
	DWORD		dwIndex;		// 序号

	SBuffMsgRemoveBuff_SS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
// 场景服-客户端 buff充能层数改变           BUFF_MSG_CHARGE_COUNT_CHANGE
struct SBuffMsgBuffChargeCountChange_SC
{
	int			nChargeCount;		  // 当前充能层数
	int         nChargeValue;         // 当前该效果的充能量
	DWORD		dwIndex;		      // 序号
	SBuffMsgBuffChargeCountChange_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 客户端-场景服 buff索引移除                BUFF_MSG_REMOVE_INDEX
struct SBuffMsgRemoveIndex_CS
{
    int         nCount;

    SBuffMsgRemoveIndex_CS(void)
    {
        memset(this, 0, sizeof(*this));
    }
};
#pragma pack()
#endif	// __BUFF_MESSAGE_DEF_H__