/*******************************************************************
** 文件名:	MovePartDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	rjc
** 日  期:	5/26/2015
** 版  本:	1.0
** 描  述:	

			移动部件定义文件
********************************************************************/

#pragma once

#include "NetMessageDef.h"


/////////////////////////// 移动模块消息  ////////////////////////////
// 消息码定义键值
enum MOVE_NET_MESSAGE
{
	MOVE_MSG_STOP	 = SC_MSG_MOVE_START,		    // 移动停止
};

struct msg_creatrue_move_stop
{
	UID	 uidOperater;				// 动作发起者
	UID	 uidTarget;					// 动作被击者
	Vector3 ptNewLoc;				// 被击退者停止点

	msg_creatrue_move_stop()
	{
		memset(this, 0, sizeof(*this));
	}
};