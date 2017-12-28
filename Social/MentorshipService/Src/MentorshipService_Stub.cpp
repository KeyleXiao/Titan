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
        rkt::ibuffer in(pMsg->context, pMsg->context_len);
        DWORD serverID; SNetMsgHead head; size_t len;
        in >> serverID >> head >> len;

        m_real_service->handleServerMsg(serverID, head, in.current(), len);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IMentorshipService::handleClientMsg) )
    {
        rkt::ibuffer in(pMsg->context, pMsg->context_len);
        DWORD client; SNetMsgHead head; size_t len;
        in >> client >> head >> len;

        m_real_service->handleClientMsg(client, head, in.current(), len);
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