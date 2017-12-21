/*******************************************************************
** 文件名:	EntityServer.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			实体客户端模块: 处理实体相关的客户端逻辑
********************************************************************/

#include "stdafx.h"
#include "EntityServer.h"
#include "IEntity.h"

bool EntityServer::create()
{
	gServerGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_ENTITY, this);
	return true;
}

bool EntityServer::close()
{
	gServerGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_ENTITY,this);
	return true;
}

void EntityServer::onEntityMove(IEntity * pPlayer,ibuffer & in)
{
	msg_creature_rigidbody_sync data;
	in >> data;
	if ( in.fail() )
		return;

	command_entity_move cmd;
	cmd.nTick = data.nTick;
	cmd.move = data.move;
	pPlayer->sendCommand( COMMAND_ENTITY_MOVE,&cmd,sizeof(cmd));
}


void EntityServer::onMessage( IEntity * pPlayer,SNetMsgHead* head, void* data, size_t len)
{
	if ( pPlayer==0 )
		return ;

	ibuffer in(data,len);

	switch( head->byKeyAction )
	{
	case CS_MSG_CREATURE_SYNC:
		onEntityMove(pPlayer,in);
		return;
	case CS_MSG_CREATURE_CTRL:
		pPlayer->sendCommand( COMMAND_ENTITY_CTRL,data,len);
		return;
	}
}


