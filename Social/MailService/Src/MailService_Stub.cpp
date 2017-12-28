#include "stdafx.h"
#include "MailService_Stub.h"


bool MailService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	return m_real_service->create();
}

bool MailService_Stub::on_stop()
{
	TraceLn(__FUNCTION__);
	m_proxy.m_pContainer = SERVICE_PTR(0);
	return true;
}

bool MailService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
    if ( TEST_SAME_FUNCTION(IMailService::handleServerMsg) )
    {
        rkt::ibuffer in(pMsg->context, pMsg->context_len);
        DWORD serverID; SNetMsgHead head; size_t len;
        in >> serverID >> head >> len;

        m_real_service->handleServerMsg(serverID, head, in.current(), len);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IMailService::handleClientMsg) )
    {
        rkt::ibuffer in(pMsg->context, pMsg->context_len);
        DWORD client; SNetMsgHead head; size_t len;
        in >> client >> head >> len;

        m_real_service->handleClientMsg(client, head, in.current(), len);
        return true;
    }

	if ( TEST_SAME_FUNCTION(IMailService::sendMail) )
	{
		ibuffer in(pMsg->context,pMsg->context_len);
		SMailSendData sSendData;
		int nMailFillID;
		string strParam;
		in >> sSendData >> nMailFillID >> strParam;

		bool ret = m_real_service->sendMail(sSendData, nMailFillID, strParam.c_str());
		if ( resultBuf ) *resultBuf<<ret;
		return true;
	}

    if (TEST_SAME_FUNCTION(IMailService::readPlayerMailList))
    {
        ibuffer in(pMsg->context, pMsg->context_len);
        DWORD dwPDBID;

        in >> dwPDBID;

        m_real_service->readPlayerMailList(dwPDBID);
        return true;
    }

	return false;
}

void * MailService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

void MailService_Stub::release()
{
	TraceLn(__FUNCTION__);
	if ( nullptr != m_real_service )
	{
		m_real_service->release();
		m_real_service = nullptr;
	}
	delete this;
}

MailService_Stub::MailService_Stub(MailService* pReal)
	:m_real_service(pReal)
{

}