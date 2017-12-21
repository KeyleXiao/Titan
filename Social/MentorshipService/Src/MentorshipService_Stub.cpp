#include "stdafx.h"
#include "MentorshipService_Stub.h"


bool MentorshipService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	return m_real_service->create();
}
bool MentorshipService_Stub::on_stop()
{
	TraceLn(__FUNCTION__);
	m_proxy.m_pContainer = SERVICE_PTR(0);
	return true;
}
bool MentorshipService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
    if ( TEST_SAME_FUNCTION(IMentorshipService::handleServerMsg) )
    {
        DWORD serverID;SNetMsgHead head;PACKAGE_PTR::T_BAG bag;
        GET_MSG_PARAM_3(DWORD, serverID, SNetMsgHead, head, PACKAGE_PTR::T_BAG, bag);

        m_real_service->handleServerMsg( serverID,head,bag.get() );
        return true;
    }

    if ( TEST_SAME_FUNCTION(IMentorshipService::handleClientMsg) )
    {
        DWORD client;SNetMsgHead head;PACKAGE_PTR::T_BAG bag;
        GET_MSG_PARAM_3(DWORD, client,SNetMsgHead ,head,PACKAGE_PTR::T_BAG ,bag );

        m_real_service->handleClientMsg( client,head,bag.get() );
        return true;
    }

	return false;
}
void * MentorshipService_Stub::query_interface( int iid )
{
	return &m_proxy;
}
void MentorshipService_Stub::release()
{
	TraceLn(__FUNCTION__);
	if ( nullptr != m_real_service )
	{
		m_real_service->release();
		m_real_service = nullptr;
	}
	delete this;
}

MentorshipService_Stub::MentorshipService_Stub(MentorshipService* pReal)
	:m_real_service(pReal)
{

}