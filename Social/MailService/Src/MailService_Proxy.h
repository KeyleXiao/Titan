/*******************************************************************
** 文件名:	speed\Social\MailServer\Src\MailService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/6/2017
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "IMailService.h"


using namespace rkt;
using namespace std;

class MailService_Proxy:public IMailService
{
public:
	///////////////////////////////IMailService///////////////////////////////////////////

    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg) override;

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg) override;

	/// 发送邮件
	virtual bool sendMail(SMailSendData sSendData, int nMailFillID = emMailFill_Begin, LPCSTR szParam = NULL) override;

    // 指定玩家读取邮件列表
    virtual bool readPlayerMailList(DWORD dwPDBID) override;
public:
	SERVICE_PTR m_pContainer;

private:
	void PostMsg(obuf256& oMsg);
};