#include "stdafx.h"
#include "LegendCupSceneService_Stub.h"

#if LegendCupScene_DisableShowDebug
#undef TraceLn
#define TraceLn(x)
#endif

bool LegendCupSceneService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	return m_real_service->create();
}

bool LegendCupSceneService_Stub::on_stop()
{
	TraceLn("LegendCupSceneService_Stub::on_stop");
	m_proxy.m_pContainer = SERVICE_PTR(0);
	return true;
}

bool LegendCupSceneService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
    if ( TEST_SAME_FUNCTION(ILegendCupSceneService::handleServerMsg) )
    {
        DWORD serverID;SNetMsgHead head;PACKAGE_PTR::T_BAG bag;
        GET_MSG_PARAM_3(DWORD, serverID,SNetMsgHead ,head,PACKAGE_PTR::T_BAG ,bag );

        m_real_service->handleServerMsg( serverID,head,bag.get() );
        return true;
    }

    if ( TEST_SAME_FUNCTION(ILegendCupSceneService::handleClientMsg) )
    {
        DWORD client;SNetMsgHead head;PACKAGE_PTR::T_BAG bag;
        GET_MSG_PARAM_3(DWORD, client,SNetMsgHead ,head,PACKAGE_PTR::T_BAG ,bag );

        m_real_service->handleClientMsg( client,head,bag.get() );
        return true;
    }

	return false;
}

void LegendCupSceneService_Stub::release()
{
	TraceLn("LegendCupSceneService_Stub::release");

	if (nullptr != m_real_service)
	{
		m_real_service->release();
		m_real_service = nullptr;
	}

	delete this;
}

void * LegendCupSceneService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

LegendCupSceneService_Stub::LegendCupSceneService_Stub( LegendCupScenService * pReal )
	: m_real_service( pReal )
{

}

