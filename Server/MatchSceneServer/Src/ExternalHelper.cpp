/*******************************************************************
** 文件名:	e:\Rocket\Server\KinServer\Src\ExternalHelper.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	xiaobao
** 日  期:	2009/3/9
** 描  述:	辅助功能类
** 应  用:  	
********************************************************************/
#include "stdafx.h"
#include "ExternalHelper.h"
#include "RPC/IGameRPC.h"
#include "ICenterConnectorService.h"
#include "KinMsgDef.h"
#include "IServerGlobal.h"
#include "IActorMgr.h"
#include "ChatHelper.h"

using namespace rkt;

CExternalHelper	g_EHelper;

/// purpose: 构造函数
CExternalHelper::CExternalHelper(void)
{

}

/// purpose: 析构函数
CExternalHelper::~CExternalHelper(void)
{

}


/// purpose: 发送消息
//@ param  : nSubModule 战队子模块消息码，参考emMsgKin_
//@ param  : dwKinID 战队ID，非emMsgKin_Kin的消息可以填0
//@ param  : nActionID 消息码
//@ param  : pData 消息内容
//@ param  : nLen 消息内容长度
void CExternalHelper::SendKinSocial(BYTE byKeyAction, int nActionID, DWORD dwKinID,void* pData, int nLen)
{
    // 社会服数据
	ICenterConnectorService *pCenterConnector = gServerGlobal->getCenterConnectorService();
	if (nullptr == pCenterConnector)
	{
		ErrorLn(__FUNCTION__" failed( void * buf, size_t len )! nullptr == pCenterConnector");
		return;
	}

	obuf256 obMsg;

    SMsgKinDataSubMsg subMsg;
    subMsg.dwMsgCode = nActionID;
    subMsg.dwKinID = dwKinID;

	obMsg.push_back(&subMsg,sizeof(SMsgKinDataSubMsg));
	obMsg.push_back(pData, nLen);
	
	pCenterConnector->sendToSocial(MSG_MODULEID_KIN, byKeyAction, obMsg.data(), obMsg.size());
}

IActorService * CExternalHelper::getActorByPDBID( PDBID pdbid )
{
    SERVICE_PTR pContainer = gServerGlobal->getActorMgr()->getActorByPDBID(pdbid);
    if ( pContainer==0 )
    {
    //    WarningLn(__FUNCTION__ ": pContainer==0, pdbid="<< pdbid);         
        return NULL;
    }

    return (IActorService*)pContainer->query_interface();
}

void CExternalHelper::showSystemMessage(PDBID dwPDBID, EMChatTipID nTipID, LPCSTR szParam /*= NULL*/)
{
    ChatHelper chatHelper;
    IChatSceneService *pChatSceneService = chatHelper.m_ptr;
    if (pChatSceneService == NULL)
    {
        return;
    }

    pChatSceneService->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, dwPDBID, nTipID, szParam);
}
