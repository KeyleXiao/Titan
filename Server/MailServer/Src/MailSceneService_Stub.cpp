#include "stdafx.h"
#include "MailSceneService_Stub.h"

bool MailSceneService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	return m_real_service->create();
}

bool MailSceneService_Stub::on_stop()
{
    m_real_service->onStop();
	m_proxy.m_pContainer = SERVICE_PTR(0);
	return true;
}

bool MailSceneService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
    if ( TEST_SAME_FUNCTION(IMailSceneService::handleServerMsg) )
    {
        DWORD serverID;SNetMsgHead head;PACKAGE_PTR::T_BAG bag;
        GET_MSG_PARAM_3(DWORD, serverID,SNetMsgHead ,head,PACKAGE_PTR::T_BAG ,bag );

        m_real_service->handleServerMsg( serverID,head,bag.get() );
        return true;
    }

 //   if ( TEST_SAME_FUNCTION(IMailSceneService::handleClientMsg) )
 //   {
 //       DWORD client;SNetMsgHead head;PACKAGE_PTR::T_BAG bag;
 //       GET_MSG_PARAM_3(DWORD, client,SNetMsgHead ,head,PACKAGE_PTR::T_BAG ,bag );

 //       m_real_service->handleClientMsg( client,head,bag.get() );
 //       return true;
 //   }

	//if ( TEST_SAME_FUNCTION(IMailSceneService::delMail) )
	//{
	//	DWORD dwPDBID;
	//	LONGLONG llMailID;
	//	GET_MSG_PARAM_2(DWORD, dwPDBID,LONGLONG, llMailID);

	//	bool ret = m_real_service->delMail(dwPDBID, llMailID);
	//	if ( resultBuf ) *resultBuf<<ret;
	//	return true;
	//}

	if ( TEST_SAME_FUNCTION(IMailSceneService::sendMail) )
	{
		ibuffer in(pMsg->context,pMsg->context_len);
		SMailSendData sMailSendData;
		int nMailFillID;
		string strParam;
		in >> sMailSendData >> nMailFillID >> strParam; 

		bool ret = m_real_service->sendMail(sMailSendData, nMailFillID, strParam.c_str());
		if ( resultBuf ) *resultBuf<<ret;
		return true;
	}

	if ( TEST_SAME_FUNCTION(IMailSceneService::sendFillMail) )
	{
		ibuffer in(pMsg->context,pMsg->context_len);
		DWORD dwPDBID;
		int nMailFillID;
		string strParam;
		in >> dwPDBID >> nMailFillID >> strParam; 

		bool ret = m_real_service->sendFillMail(dwPDBID, nMailFillID, strParam.c_str());
		if ( resultBuf ) *resultBuf<<ret;
		return true;
	}

	return false;
}

void MailSceneService_Stub::release()
{
	if (nullptr != m_real_service)
	{
		m_real_service->release();
		m_real_service = nullptr;
	}

	delete this;
}

void * MailSceneService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

MailSceneService_Stub::MailSceneService_Stub( MailSceneService * pReal )
	: m_real_service( pReal )
{
}