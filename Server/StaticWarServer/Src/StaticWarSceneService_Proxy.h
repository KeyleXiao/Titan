/*******************************************************************
** 文件名:	E:\speed\Server\OrderServer\Src\StaticWarSceneService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	所有战场数据服务 代理
********************************************************************/

#pragma once


#include "IServiceContainer.h"
#include "buffer.h"
#include "IStaticWarSceneService.h"
#include "StaticWarSceneService.h"

using namespace rkt;


class StaticWarSceneService_Proxy : public IStaticWarSceneService
{
public:

    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

	// 请求其他玩家基本数据
	virtual void reqOtherPlayerInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake);

	// 请求其他玩家英雄数据
	virtual void reqOtherPlayerHeroInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake);

public:
	SERVICE_PTR    m_pContainer;


private:
	void PostMsg(obuf256& oMsg);
};
