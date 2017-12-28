/*******************************************************************
** 文件名:	speed\Server\MailServer\Include\IMailService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/6/2017
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "MailDef.h"
#include "Mail_ManagedDef.h"
using Gateway::ClientID;
using namespace Mail_ManagerDef;

struct IMailService
{

    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len) = 0;

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void * data, size_t len) = 0;

	// 发送邮件（参数用分号隔开）
	virtual bool sendMail(SMailSendData sSendData, int nMailFillID = emMailFill_Begin, LPCSTR szParam = NULL) = 0;

    // 指定玩家读取邮件列表
    virtual bool readPlayerMailList(DWORD dwPDBID) = 0;
};