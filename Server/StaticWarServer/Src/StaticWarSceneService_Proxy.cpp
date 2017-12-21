#include "stdafx.h"
#include "StaticWarSceneService_Proxy.h"
#include "OrderDef.h"

#if StaticWarScene_DisableShowDebug
#undef TraceLn
#define TraceLn(x)
#endif

void StaticWarSceneService_Proxy::handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg)
{
    PACKAGE_PTR::T_BAG bag(msg);
    BUILD_MSG_CONTEXT_3( IStaticWarSceneService::handleServerMsg,DWORD ,serverID,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}

void StaticWarSceneService_Proxy::handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg)
{
    PACKAGE_PTR::T_BAG bag(msg);
    BUILD_MSG_CONTEXT_3( IStaticWarSceneService::handleClientMsg,DWORD ,client,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
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
