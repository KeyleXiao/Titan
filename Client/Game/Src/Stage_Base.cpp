/*******************************************************************
** 文件名:	Stage_Base.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/24/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/

#include "stdafx.h"
#include "Stage_Base.h"
#include "IMessageDispatch.h"
#include "IClientGlobal.h"
#include "GatewayClient.h"
#include "MigrateDef.h"
#include "Stage_Manager.h"
#include "IChatClient.h"

void Stage_Base::OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler )
{
	// 不会调到这里来
	ErrorLn("Stage_Base::OnAccept assert error!");
}

void Stage_Base::OnConnected( IConnection * conn )
{
	// 通知界面显示服务器已连接
    TraceLn("Socket connect successful.");
}

void Stage_Base::OnDisconnected( IConnection * conn,DWORD reason )
{
    WarningLn("Client Socket is disconnected! reason="<<reason);
	// 通知界面服务器已断开
	//GatewayClient::getInstance().Stop();
    if (gClientGlobal->getStageManager()->getCurState() != ESS_LOGIN)
    {
        gClientGlobal->getChatClient()->showSystemMessage(CHAT_TIP_SYSTEM_DISCONNECT);
    }
}

void Stage_Base::OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
{

}

void Stage_Base::OnError( IConnection * conn,DWORD dwErrorCode )
{
    string strError = getSysErrorMessage(dwErrorCode);
    WarningLn("Client Socket is error! code="<<dwErrorCode<<" errordes ="<<strError);
	// 通知界面服务器已断开
	//GatewayClient::getInstance().Stop();
    //gClientGlobal->getChatClient()->showSystemMessage(CHAT_TIP_SYSTEM_DISCONNECT);
    gClientGlobal->getChatClient()->showSystemMessage(CHAT_TIP_CUSTOMER_TIP, strError.data());
}