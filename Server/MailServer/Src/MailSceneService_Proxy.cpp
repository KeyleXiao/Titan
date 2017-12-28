#include "stdafx.h"
#include "MailSceneService_Proxy.h"
#include "MailDef.h"

//void MailSceneService_Proxy::handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len)
//{
//    obuf256 t_data;
//    t_data << serverID << head << len;
//    t_data.push_back(data, len);
//
//    BUILD_MSG_BUFFER(IMailSceneService::handleServerMsg, t_data);
//
//    m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
//}


// 发送邮件
bool MailSceneService_Proxy::sendMail(SMailSendData sMailSendData, int nMailFillID, LPCSTR szParam)
{
    if (m_pContainer == 0)
        return bool();

	string strParam = (szParam == NULL ? "" : szParam);
	obuf1024 t_data;
	t_data << sMailSendData << nMailFillID << strParam;
	BUILD_MSG_BUFFER( IMailSceneService::sendMail,t_data );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf,MSG_FLAG_NO_BLOCK);
	RETURN_FROM_MSG_BUF(bool);

	return bool();
}

// 发送填充邮件(数据由配置表里面填充)
bool MailSceneService_Proxy::sendFillMail(DWORD dwPDBID, int nMailFillID, LPCSTR szParam)
{
    if (m_pContainer == 0)
        return bool();

	string strParam = (szParam == NULL ? "" : szParam);
	obuf t_data;
	t_data << dwPDBID << nMailFillID << strParam;
	BUILD_MSG_BUFFER( IMailSceneService::sendFillMail,t_data );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf,MSG_FLAG_NO_BLOCK);
	RETURN_FROM_MSG_BUF(bool);

	return bool();
}
