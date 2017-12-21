/*******************************************************************
** 文件名:	GameEvent.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	10/27/2014
** 版  本:	1.0
** 描  述:	

			游戏事件定义
********************************************************************/


#pragma once
#include "GameDef.h"


// 01. 实体移动
#define    COMMAND_ENTITY_MOVE       (CMD_OFFSET+1) 

struct command_entity_move
{
	DWORD   nTick;				// 快照时间点
	Move3	move;				// 当前移动信息
};

// 02.  实体操控
#define    COMMAND_ENTITY_CTRL       (CMD_OFFSET+2) 

// 03.  实体销毁
#define    COMMAND_ENTITY_DESTROY       (CMD_OFFSET+3) 

