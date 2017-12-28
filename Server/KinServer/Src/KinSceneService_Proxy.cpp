#include "stdafx.h"
#include "KinSceneService_Proxy.h"
#include "KinDef.h"

#if KinScene_DisableShowDebug
#undef TraceLn
#define TraceLn(x)
#endif



//void KinSceneService_Proxy::start()
//{
//	BUILD_MSG_CONTEXT_VOID( IKinSceneService::start );
//	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
//}
//
//
//void KinSceneService_Proxy::handleMessage( DWORD serverID, PACKAGE_PTR msg )
//{ 
//	BUILD_MSG_CONTEXT_2( IKinSceneService::handleMessage,DWORD, serverID, PACKAGE_PTR, msg );
//	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
//}
//
//void KinSceneService_Proxy::onClientMsg( ClientID clientID, PACKAGE_PTR msg )
//{
//	BUILD_MSG_CONTEXT_2( IKinSceneService::onClientMsg,ClientID ,clientID, PACKAGE_PTR, msg );
//	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
//}

// 发送社会服消息
void KinSceneService_Proxy::sendToSocial(BYTE nMsgID, void * data, size_t len)
{
	obuf128 t_data;
	t_data << nMsgID;
	t_data << len;
	t_data.push_back(data, len);

	BUILD_MSG_BUFFER( IKinSceneService::sendToSocial,t_data );
	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}


SSceneKinMember KinSceneService_Proxy::getKinMember(DWORD dwPDBID)
{
	BUILD_MSG_CONTEXT_1(IKinSceneService::getKinMember,DWORD, dwPDBID);

	rkt::obuf resultBuf;
	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(SSceneKinMember);

    return SSceneKinMember();
}

SKinInfo KinSceneService_Proxy::getKinInfo(DWORD dwKinID)
{
    BUILD_MSG_CONTEXT_1(IKinSceneService::getKinInfo,DWORD, dwKinID);

	rkt::obuf resultBuf;
	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(SKinInfo);

    return SKinInfo();
}

DWORD KinSceneService_Proxy::getKinMemberList(DWORD dwKinID,PDBID* pReturnArray, DWORD dwArrayMaxSize)
{
    BUILD_MSG_CONTEXT_3(IKinSceneService::getKinMemberList,DWORD, dwKinID,PDBID*, pReturnArray, DWORD ,dwArrayMaxSize);

	rkt::obuf resultBuf;
	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(DWORD);
	return 0;
}

DWORD KinSceneService_Proxy::getKinMemberCount(DWORD dwKinID)
{
    BUILD_MSG_CONTEXT_1(IKinSceneService::getKinMemberCount,DWORD ,dwKinID);

	rkt::obuf resultBuf;
	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(DWORD);

    return 0;
}

bool KinSceneService_Proxy::setWeekActivity(DWORD dwKinID, int nValue)
{
	BUILD_MSG_CONTEXT_2(IKinSceneService::setWeekActivity, DWORD, dwKinID, int, nValue);

	m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);

	return true;
}

int KinSceneService_Proxy::getWeekActivity(DWORD dwKinID)
{
	BUILD_MSG_CONTEXT_1(IKinSceneService::getWeekActivity, DWORD, dwKinID);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
	RETURN_FROM_MSG_BUF(int);

	return 0;
}

void KinSceneService_Proxy::handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len)
{
    obuf256 t_data;
    t_data << serverID << head << len;
    t_data.push_back(data, len);

    BUILD_MSG_BUFFER(IKinSceneService::handleServerMsg, t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}

void KinSceneService_Proxy::handleClientMsg(DWORD client, SNetMsgHead head, void * data, size_t len)
{
    obuf256 t_data;
    t_data << client << head << len;
    t_data.push_back(data, len);

    BUILD_MSG_BUFFER(IKinSceneService::handleClientMsg, t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}

