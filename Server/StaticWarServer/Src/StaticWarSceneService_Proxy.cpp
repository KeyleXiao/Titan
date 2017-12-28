#include "stdafx.h"
#include "StaticWarSceneService_Proxy.h"
#include "OrderDef.h"

#if StaticWarScene_DisableShowDebug
#undef TraceLn
#define TraceLn(x)
#endif

void StaticWarSceneService_Proxy::handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen)
{
	obuf256 t_data;
	t_data << serverID << head << nLen;
	t_data.push_back(pData, nLen);

	BUILD_MSG_BUFFER(IStaticWarSceneService::handleServerMsg, t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}

void StaticWarSceneService_Proxy::handleClientMsg(DWORD client, SNetMsgHead head, void* pData, size_t nLen)
{
	obuf256 t_data;
	t_data << client << head << nLen;
	t_data.push_back(pData, nLen);
	BUILD_MSG_BUFFER(IStaticWarSceneService::handleClientMsg, t_data);

	m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
}

void StaticWarSceneService_Proxy::reqOtherPlayerInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake)
{
	BUILD_MSG_CONTEXT_3( IStaticWarSceneService::reqOtherPlayerInfo, PDBID, dwSelfID, PDBID, dwReqID,bool, bFake);

	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}

void StaticWarSceneService_Proxy::reqOtherPlayerHeroInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake)
{
	BUILD_MSG_CONTEXT_3( IStaticWarSceneService::reqOtherPlayerHeroInfo, PDBID, dwSelfID, PDBID, dwReqID, bool, bFake);

	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}
