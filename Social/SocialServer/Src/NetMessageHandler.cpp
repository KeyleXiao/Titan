/*******************************************************************
** 文件名:	NetMessageHandler.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-10-10
** 版  本:	1.0
** 描  述:	网络消息分发器

		
********************************************************************/

#include "stdafx.h"
#include "NetMessageHandler.h"
#include "ISocialGlobal.h"
#include "ICenterConnectorService.h"
#include "IMessageDispatch.h"
#include "SocialGlobalHelper.h"


// 取得消息分发器
inline IMessageDispatch* CNetMessageHandler::getMessageDispatch()
{
	return gSocialGlobal->getMessageDispatch();
}


///////ICenterServerMessageHandler///////////////////////////////////////////////////////////////////
/**
@name         : 通知服务器列表已经更新
@brief        : 服务器列表更新只会在有服务器进入退出时触发
@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
*/
void CNetMessageHandler::handleServerListUpdated()
{
#ifdef SERVICE_HANDLE_MESSAGE
    ISocialGlobalService * pGlobalService = SocialGlobalHelper().m_ptr;
    if (pGlobalService)
    {
        pGlobalService->handleServerListUpdated();
    }
#else
	getMessageDispatch()->dispatchServerListUpdated();
#endif
}

void CNetMessageHandler::handleServerInfoUpdated(CSID ServerID,SERVER_STATE nState,ServerData * pInfo)
{
#ifdef SERVICE_HANDLE_MESSAGE
    ISocialGlobalService * pGlobalService = SocialGlobalHelper().m_ptr;
    if (pGlobalService)
    {
        pGlobalService->handleServerInfoUpdated(ServerID, nState, pInfo, sizeof(ServerData));
    }
#else
	getMessageDispatch()->dispatchServerInfoUpdated(ServerID, nState, pInfo);
#endif
}

void CNetMessageHandler::handlePostMessage(CGID server,const char * pData,WORD wDataLen)
{
#ifdef SERVICE_HANDLE_MESSAGE
    ISocialGlobalService * pGlobalService = SocialGlobalHelper().m_ptr;
    if (pGlobalService)
    {
        pGlobalService->handlePostMessage(server, pData, wDataLen);
    }
#else
	getMessageDispatch()->dispatchTransmit(server, (void*)pData, wDataLen);
#endif
}

void CNetMessageHandler::handleCommandRequest(CGID server,const char * szCommand)
{

}


/**
@name          : 处理命令执行的结果
@brief         :
@param szCommand:执行的命令字符串
@param bSuccess: 命令是否执行成功
@param szOutput: 输出的提示信息
@return        :
*/
void CNetMessageHandler::handleCommandResponse(const char * szCommand,bool bSuccess,const char * szOutput )
{

}

///////IGatewayMessageHandler///////////////////////////////////////////////////////////////////
/**
@name         : 网关服务器通知游戏服务器有一个新的客户端连进来
@param  client: 该客户端在网关服务器上的ID
@return       :
*/
void CNetMessageHandler::handleClientIncoming(ClientID client)
{

}

// GM Chat Client
void CNetMessageHandler::HandleClientIncoming(SMsgGatewayHandshakeGMChatClient& mp)
{

}

/**
@name         : 网关服务器通知游戏服务器有新的数据包到达
@brief        : 该数据包是客户端发给该游戏服务器的,通过数据包包头的wKeyServer字节指定
@param client : 发送数据的客户端ID
@param pData  : 数据缓冲区位置
@param wDataLen:数据长度
*/
void CNetMessageHandler::handleReceivedData(ClientID client,LPVOID pData,DWORD wDataLen)
{
#ifdef SERVICE_HANDLE_MESSAGE
    ISocialGlobalService * pGlobalService = SocialGlobalHelper().m_ptr;
    if (pGlobalService)
    {
        pGlobalService->handleGateWayReceivedData(client, pData, wDataLen);
    }
#else
	if (pData != NULL && wDataLen >= sizeof(SGameMsgHead))
	{
		SGameMsgHead* head = (SGameMsgHead*)pData;
		if (head->SrcEndPoint == MSG_ENDPOINT_CLIENT)
		{
			getMessageDispatch()->dispatchMessage(client, pData, (size_t)wDataLen);
		}
	}
#endif
}

/**
@name         : 网关服务器通知游戏服务器有连接断开
@param client : 断开连接的客户端ID
@param dwReason:断开的原因或错误码
*/
void CNetMessageHandler::handleClientDisconnect(ClientID client,DWORD dwReason)
{
	ISocialGlobalService * pGlobalService = SocialGlobalHelper().m_ptr;
	if (pGlobalService)
	{
		pGlobalService->handleGateWayClientDisconnect(client, dwReason);
	}
}

/** 
@name         : 收到设置游戏ID
*/
void CNetMessageHandler::handleManagerSetGameWorldID()
{
    ISocialGlobalService * pGlobalService = SocialGlobalHelper().m_ptr;
    if (pGlobalService)
    {
        pGlobalService->handleManagerSetGameWorldID();
    }
}

/** 
@name         : 收到管理器下发的资源数据
*/
void CNetMessageHandler::handleManagerResourceData(LPVOID pData,DWORD dwDataLen)
{
    ISocialGlobalService * pGlobalService = SocialGlobalHelper().m_ptr;
    if (pGlobalService)
    {
        pGlobalService->handleManagerResourceData(pData, dwDataLen);
    }
}


/** 
@name         : 收到管理器下发的资源数据结果
*/
void CNetMessageHandler::handleManagerResourceResult(LPVOID pData,DWORD dwDataLen)
{
    ISocialGlobalService * pGlobalService = SocialGlobalHelper().m_ptr;
    if (pGlobalService)
    {
        pGlobalService->handleManagerResourceResult(pData, dwDataLen);
    }
}