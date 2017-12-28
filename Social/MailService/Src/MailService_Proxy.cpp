#include "stdafx.h"
#include "MailService_Proxy.h"
#include "StubMacroDef.h"

 //////////////////////////////////////////////////////////////////////////////////
 void MailService_Proxy::handleServerMsg( DWORD serverID,SNetMsgHead head,void * data, size_t len )
 {
     obuf256 t_data;
     t_data << serverID << head << len;
     t_data.push_back(data, len);

     BUILD_MSG_BUFFER(IMailService::handleServerMsg, t_data);

     m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
 };

 //////////////////////////////////////////////////////////////////////////////////
 void MailService_Proxy::handleClientMsg( DWORD client,SNetMsgHead head,void * data, size_t len )
 {
     obuf256 t_data;
     t_data << client << head << len;
     t_data.push_back(data, len);

     BUILD_MSG_BUFFER(IMailService::handleClientMsg, t_data);

     m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
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



 

