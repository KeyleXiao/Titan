#include "stdafx.h"
#include "SNSService_Stub.h"

bool SNSService_Stub::on_start( SERVICE_PTR pContainer, void *data, int len )
{
	m_proxy.m_pContainer = pContainer;
	return m_real_service->create();
}

bool SNSService_Stub::on_stop()
{
	TraceLn(__FUNCTION__);
	m_proxy.m_pContainer = SERVICE_PTR(0);
	return true;
}

bool SNSService_Stub::handle_message( SERVICE_MESSAGE *pMsg, rkt::obuf *resultBuf )
{
	rkt::ibuffer in(pMsg->context,pMsg->context_len);

	if ( TEST_SAME_FUNCTION(ISNSService::recv_message) )
	{
		rkt::ibuffer in(pMsg->context,pMsg->context_len );
		int msgID = 0; int nLen = 0;
		in >> msgID >> nLen;

		m_real_service->recv_message(msgID, in.current(), nLen);
		return true;
	}

	if ( TEST_SAME_FUNCTION(ISNSService::addBuddy) )
	{
		SNSService_Proxy::addBuddy_Param* pParam;
		GET_MSG_PARAM(SNSService_Proxy::addBuddy_Param,pParam);

		m_real_service->addBuddy(pParam->dwUserID1, pParam->dwUserID2, pParam->szActorName1, pParam->szActorName2);
		return true;
	}

	if (TEST_SAME_FUNCTION(ISNSService::invitePlayGame))
	{
		DWORD dwInviterActorID, dwTargetActorID, dwRoomID;
		GET_MSG_PARAM_3(DWORD, dwInviterActorID, DWORD, dwTargetActorID, DWORD, dwRoomID);

		m_real_service->invitePlayGame(dwInviterActorID, dwTargetActorID, dwRoomID);

		return true;
	}
   	return false;
}

void * SNSService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

void SNSService_Stub::release()
{
	TraceLn(__FUNCTION__);
	if ( nullptr != m_real_service )
	{
		m_real_service->release();
		m_real_service = nullptr;
	}
	delete this;
}

SNSService_Stub::SNSService_Stub(SNSService *pReal)
	:m_real_service(pReal)
{

}