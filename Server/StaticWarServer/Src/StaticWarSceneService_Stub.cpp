#include "stdafx.h"
#include "StaticWarSceneService_Stub.h"

#if StaticWarScene_DisableShowDebug
#undef TraceLn
#define TraceLn(x)
#endif

bool StaticWarSceneService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	return m_real_service->create();
}

bool StaticWarSceneService_Stub::on_stop()
{
	TraceLn("StaticWarSceneService_Stub::on_stop");
	m_proxy.m_pContainer = SERVICE_PTR(0);
	return true;
}

bool StaticWarSceneService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
    if ( TEST_SAME_FUNCTION(IStaticWarSceneService::handleServerMsg) )
    {
        DWORD serverID;SNetMsgHead head;PACKAGE_PTR::T_BAG bag;
        GET_MSG_PARAM_3(DWORD, serverID,SNetMsgHead ,head,PACKAGE_PTR::T_BAG ,bag );

        m_real_service->handleServerMsg( serverID,head,bag.get() );
        return true;
    }

    if ( TEST_SAME_FUNCTION(IStaticWarSceneService::handleClientMsg) )
    {
        DWORD client;SNetMsgHead head;PACKAGE_PTR::T_BAG bag;
        GET_MSG_PARAM_3(DWORD, client,SNetMsgHead ,head,PACKAGE_PTR::T_BAG ,bag );

        m_real_service->handleClientMsg( client,head,bag.get() );
        return true;
    }

	if ( TEST_SAME_FUNCTION(IStaticWarSceneService::reqOtherPlayerInfo) )
	{
		PDBID dwSelfID;
		PDBID dwReqID;
        bool bFake;
		GET_MSG_PARAM_3(PDBID , dwSelfID, PDBID, dwReqID, bool, bFake);

		m_real_service->reqOtherPlayerInfo( dwSelfID, dwReqID,bFake );
		return true;
	}

	if ( TEST_SAME_FUNCTION(IStaticWarSceneService::reqOtherPlayerHeroInfo) )
	{
		PDBID dwSelfID;
		PDBID dwReqID;
        bool bFake;
		GET_MSG_PARAM_3(PDBID , dwSelfID, PDBID, dwReqID, bool, bFake);

		m_real_service->reqOtherPlayerHeroInfo( dwSelfID, dwReqID, bFake );
		return true;
	}

	return false;
}

void StaticWarSceneService_Stub::release()
{
	TraceLn("StaticWarSceneService_Stub::release");

	if (nullptr != m_real_service)
	{
		m_real_service->release();
		m_real_service = nullptr;
	}

	delete this;
}

void * StaticWarSceneService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

StaticWarSceneService_Stub::StaticWarSceneService_Stub( StaticWarScenService * pReal )
	: m_real_service( pReal )
{

}

