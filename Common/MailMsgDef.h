//==========================================================================
/**
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/6/2017
** 版  本:	1.0
** 描  述:	邮件消息定义
*/
//==========================================================================

#ifndef __MAILMSGDEF_H__
#define __MAILMSGDEF_H__

#include "GatewayServiceDef.h"
#include "NetMessageDef.h"
#include "IGameDatabaseDef.h"
#include "NameLenDef.h"
#include "MailDef.h"


using namespace Gateway;

#pragma once
#pragma pack(1)

/******************************************************************/
// 上层的消息码必定是MSG_MODULEID_MAIL
/******************************************************************/
///////////////////////////////////////////////////////////////////


/***************************************************************/
//////////////////////// 服务器层的全局定义 /////////////////////
/***************************************************************/

// 消息ID定义
//////////////////////////////////////////////////////////////////////////
enum MAIL_NET_MESSAGE_KEYACTION
{
	MSG_MAIL_REQUESTMAILLIST,					// 请求邮件列表
	MSG_MAIL_REQUESTREADMAIL,					// 请求阅读邮件
	MSG_MAIL_REQUESTFASTRECV,					// 请求快速领取
	MSG_MAIL_RESPONSEREADMAIL,					// 响应阅读邮件
	MSG_MAIL_NEWMAILCOUNT,						// 玩家未读邮件总数
	MSG_MAIL_MAILRECVSUCCESS,					// 邮件收件成功
	MSG_MAIL_ADDMAIL_SO,						// 添加邮件
	MSG_MAIL_DELMAIL_SO,						// 删除邮件
	MSG_MAIL_SETMAILREAD_SO,					// 设置邮件已读
	MSG_MAIL_REFRESHMAILLIST,                   // 刷新邮件列表
	MSG_MAIL_MAXID,
};

//////////////////////////////////////////////////////////////////////////
// 消息体 MSG_MAIL_REQUESTMAILLIST			-- 请求邮件列表
struct SMsgMail_MailList
{
	DWORD dwPDBID;
	int nCount;

	// nCount * SMailBaseData
	SMsgMail_MailList()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体 MSG_MAIL_REQUESTREADMAIL				-- 请求阅读邮件
struct SMsgMail_ReadMail
{
	LONGLONG llMailID;

	SMsgMail_ReadMail()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体  MSG_MAIL_REQUESTFASTRECV					-- 请求快速领取
struct SMsgMail_FastRecv
{
	LONGLONG llMailID;

	SMsgMail_FastRecv()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体  MSG_MAIL_RESPONSEREADMAIL				-- 响应阅读邮件
// struct SMailDetailData;

// 消息体  MSG_MAIL_NEWMAILCOUNT					// 玩家未读邮件总数
struct SMsgMail_NewMailCount
{
	int nCount;
	int nPlusCount;

	SMsgMail_NewMailCount()
	{
		memset(this, 0, sizeof(*this));
	}
};

//消息体 MSG_MAIL_MAILRECVSUCCESS
struct SMsgMail_RecvSuccess							// 收取成功 
{
	DWORD dwPDBID;
	LONGLONG llMailID;

	SMsgMail_RecvSuccess()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体  MSG_MAIL_ADDMAIL_SO						// 添加邮件
struct SMsgMail_AddMail_SO
{
	int nMailFillID;
	SMailSendData sMailData;
	char szParam[MAIL_CONTENT_MAXSIZE];				// 邮件参数

	SMsgMail_AddMail_SO()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体  MSG_MAIL_DELMAIL_SO						// 删除邮件
struct SMsgMail_DelMail_SO
{
	DWORD dwPDBID;
	LONGLONG llMailID;

	SMsgMail_DelMail_SO()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 设置邮件已读 MSG_MAIL_SETMAILREAD_SO
struct SMsgMail_SetMailRead_SO
{
	DWORD dwPDBID;
	LONGLONG llMailID;

	SMsgMail_SetMailRead_SO()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体 MSG_MAIL_REFRESHMAILLIST			-- 从数据库刷新邮件列表
struct SMsgMail_RefreshMailList
{
	DWORD dwPDBID;
};

#pragma pack()

#endif // __MAILMSGDEF_H__

