/*******************************************************************
** 文件名:	MailSceneService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/7/2017
** 版  本:	1.0
** 描  述:	邮件系统场景代理
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "buffer.h"
#include "IMailSceneService.h"
#include "MailSceneService.h"

using namespace rkt;

class MailSceneService_Proxy : public IMailSceneService
{
public:
	//IMailSceneService///////////////////////////////////////////////////////////////////////
	
    // 处理其它服务器发送过来的消息
    //virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len);

	// 发送邮件
	virtual bool	sendMail(SMailSendData sMailSendData, int nMailFillID = emMailFill_Begin, LPCSTR szParam = NULL);

	// 发送填充邮件(数据由配置表里面填充)
	virtual bool	sendFillMail(DWORD dwPDBID, int nMailFillID, LPCSTR szParam = NULL);

public:
	SERVICE_PTR    m_pContainer;

};
