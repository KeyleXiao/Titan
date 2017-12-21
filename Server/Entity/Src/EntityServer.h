/*******************************************************************
** 文件名:	EntityServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			实体客户端模块: 处理实体相关的客户端逻辑
********************************************************************/

#pragma once

#include "IMessageDispatch.h"
#include "IServerGlobal.h"
#include "buffer.h"
#include "CommandDef.h"
#include "EntityMsgDef.h"

/**
	负责实体相关的通讯消息解码,然后分给各个模块进行处理
	各个实体部件不需要关注通讯协议
*/
class EntityServer : public IMessageHandler
{
public:
	bool create();

	bool close();

	void onEntityMove(IEntity * pPlayer,ibuffer & in);

	virtual void onMessage( IEntity * pPlayer,SNetMsgHead* head, void* data, size_t len);
};