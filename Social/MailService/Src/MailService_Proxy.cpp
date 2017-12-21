#include "stdafx.h"
#include "MailService_Proxy.h"
#include "StubMacroDef.h"

 //////////////////////////////////////////////////////////////////////////////////
 void MailService_Proxy::handleServerMsg( DWORD serverID,SNetMsgHead head,PACKAGE_PTR msg )
 {
     PACKAGE_PTR::T_BAG bag(msg);
     BUILD_MSG_CONTEXT_3( IMailService::handleServerMsg,DWORD ,serverID,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

     m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
 };

 //////////////////////////////////////////////////////////////////////////////////
 void MailService_Proxy::handleClientMsg( DWORD client,SNetMsgHead head,PACKAGE_PTR msg )
 {
     PACKAGE_PTR::T_BAG bag(msg);
     BUILD_MSG_CONTEXT_3( IMailService::handleClientMsg,DWORD ,client,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

     m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
 };

 /// 发送邮件
bool MailService_Proxy::sendMail(SMailSendData sSendData, int nMailFillID, LPCSTR szParam)
{
	string strParam = (szParam == NULL ? "" : szParam);
	obuf1024 t_data;
	t_data << sSendData << nMailFillID << strParam;

	BUILD_MSG_BUFFER( IMailService::sendMail,t_data );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf,MSG_FLAG_NO_BLOCK);
	RETURN_FROM_MSG_BUF(bool);
	return 0;
}

bool MailService_Proxy::readPlayerMailList(DWORD dwPDBID)
{
    BUILD_MSG_CONTEXT_1(MailService_Proxy::readPlayerMailList, DWORD, dwPDBID);
    return m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
}



 

