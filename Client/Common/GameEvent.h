/*******************************************************************
** 文件名:	GameEvent.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/6/2014
** 版  本:	1.0
** 描  述:	

			游戏事件定义
********************************************************************/


#pragma once

#include "GameDef.h"


#define    EVENT_ENTITY_CREATE     (EVENT_OFFSET+1)               // 自定义事件: 实体创建

#define    EVENT_ENTITY_DESTROY    (EVENT_OFFSET+2)               // 自定义事件: 实体销毁

#define    EVENT_ENTITY_MOVE       (EVENT_OFFSET+3)               // 自定义事件: 实体移动

#define    EVENT_ENTITY_ENTER_VIEWPORT     (EVENT_OFFSET+4)               // 自定义事件: 进入视野

#define    EVENT_ENTITY_LEAVE_VIEWPORT     (EVENT_OFFSET+5)               // 自定义事件: 离开视野