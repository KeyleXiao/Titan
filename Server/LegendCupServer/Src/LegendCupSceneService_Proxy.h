/*******************************************************************
** 文件名:	E:\speed\Server\LegendCupServer\Src\LegendCupSceneService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	排行榜 代理
********************************************************************/

#pragma once


#include "IServiceContainer.h"
#include "buffer.h"
#include "ILegendCupSceneService.h"
#include "LegendCupSceneService.h"

using namespace rkt;


class LegendCupSceneService_Proxy : public ILegendCupSceneService
{
public:

    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen);

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void* pData, size_t nLen);

    LegendCupSceneService_Proxy() : m_pContainer(0)
    {

    }

public:
	SERVICE_PTR    m_pContainer;


private:
	void PostMsg(obuf256& oMsg);
};
