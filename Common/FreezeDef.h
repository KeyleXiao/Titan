/*******************************************************************
** 文件名:	FreezeDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/13/2014
** 版  本:	1.0
** 描  述:	冷却功能模块定义头文件

			
********************************************************************/


#ifndef __COMMON_FREEZE_DEF_H__
#define __COMMON_FREEZE_DEF_H__


#include "NetMessageDef.h"
#include "SpellDef.h"
using namespace SPELL;

#pragma pack(1)

// 冷却缩减最大比例
#define  FREEZE_MAX_SUB_RATE	50

/////////////////////////// 冷却模块消息  ////////////////////////////

// 消息码定义键值
enum FREEZE_NET_MESSAGE_KEYACTION
{
	SC_MSG_ACTION_INITFREEZE = SC_MSG_FREEZE_START,	// 服务端-客户端 冷却初始化
	SC_MSG_ACTION_FREEZEWORK,						// 服务端-客户端 执行冷却
	SC_MSG_ACTION_SETFREEZETIME,						// 服务端-客户端 执行冷却
	SC_MSG_ACTION_CHANGEFREEZETIME,					// 服务端-客户端 修改某类冷却
	SC_MSG_ACTION_CANCELFREEZE,						// 服务端-客户端 取消冷却
	SC_MSG_ACTION_BREAK_SPELL_CHANGEFREEZETIME,		// 服务端-客户端 修改技能打断冷却
};

// 冷却初始化数据	SC_MSG_ACTION_INITFREEZE
struct msg_entity_action_init_freeze
{
	struct SNodeContext
	{
		BYTE		byClassID;		// 冷却类ID	
		int			nFreezeID;		// 冷却ID
		int			nFreezeTime;	// 冷却剩余时间（毫秒）
	};

	// .................			// 多个SNodeContext
};

// 服务器执行某冷却	SC_MSG_ACTION_FREEZEWORK
struct msg_entity_action_work_freezetime
{
	DWORD dwClassID;
	DWORD dwFreezeID;

	msg_entity_action_work_freezetime(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

// 服务器修改客户端某类的冷却	SC_MSG_ACTION_CHANGEFREEZETIME
struct msg_entity_action_change_freezetime
{
	DWORD dwClassID;
	DWORD dwFreezeID;
	int	  nTime;

	msg_entity_action_change_freezetime(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

// 服务器修改客户端某类的冷却	SC_MSG_ACTION_SETFREEZETIME
struct msg_entity_action_set_freezetime
{
	DWORD dwClassID;
	DWORD dwFreezeID;
	int	  nTime;
    bool  nIsCleanInterferedFreeze;
    bool  bBreak;
	msg_entity_action_set_freezetime(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

// 服务器取消客户端冷却	SC_MSG_ACTION_CANCELFREEZE
struct msg_entity_action_cancel_freezetime
{
	DWORD dwClassID;
	DWORD dwFreezeID;

	msg_entity_action_cancel_freezetime(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

#pragma pack()




#endif	// __COMMON_FREEZE_DEF_H__