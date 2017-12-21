#include "stdafx.h"
#include "MailSceneService_Proxy.h"
#include "MailDef.h"

void MailSceneService_Proxy::handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg)
{
    PACKAGE_PTR::T_BAG bag(msg);
    BUILD_MSG_CONTEXT_3( IMailSceneService::handleServerMsg,DWORD ,serverID,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}

//void MailSceneService_Proxy::handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg)
//{
//    PACKAGE_PTR::T_BAG bag(msg);
//    BUILD_MSG_CONTEXT_3( IMailSceneService::handleClientMsg,DWORD ,client,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );
//
//    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
//}
//
//// 删除邮件
//bool MailSceneService_Proxy::delMail(DWORD dwPDBID, LONGLONG llMailID)
//{
//	BUILD_MSG_CONTEXT_2( IMailSceneService::delMail,DWORD, dwPDBID,LONGLONG, llMailID );
//
//	rkt::obuf resultBuf;
//	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
//	RETURN_FROM_MSG_BUF(bool);
//	return 0;
//}

// 发送邮件
bool MailSceneService_Proxy::sendMail(SMailSendData sMailSendData, int nMailFillID, LPCSTR szParam)
{
	string strParam = (szParam == NULL ? "" : szParam);
	obuf1024 t_data;
	t_data << sMailSendData << nMailFillID << strParam;
	BUILD_MSG_BUFFER( IMailSceneService::sendMail,t_data );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf,MSG_FLAG_NO_BLOCK);
	RETURN_FROM_MSG_BUF(bool);
	return 0;
}

// 发送填充邮件(数据由配置表里面填充)
bool MailSceneService_Proxy::sendFillMail(DWORD dwPDBID, int nMailFillID, LPCSTR szParam)
{
	string strParam = (szParam == NULL ? "" : szParam);
	obuf t_data;
	t_data << dwPDBID << nMailFillID << strParam;
	BUILD_MSG_BUFFER( IMailSceneService::sendFillMail,t_data );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf,MSG_FLAG_NO_BLOCK);
	RETURN_FROM_MSG_BUF(bool);
	return 0;
}
