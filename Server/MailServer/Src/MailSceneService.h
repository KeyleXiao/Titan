/*******************************************************************
** 文件名:	MailSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/7/2017
** 版  本:	1.0
** 描  述:	邮件场景服务
********************************************************************/

#ifndef __MAILSCENESERVICE_H__
#define __MAILSCENESERVICE_H__

#include "IMailSceneService.h"
#include "NetMessageDef.h"
#include "IDBRetSink.h"
#include "MailMsgDef.h"
#include "IMessageHandler.h"

class MailSceneService : public IMailSceneService, public ITransmitHandler, public IMessageHandler/*, public IEventExecuteSink, public IDBRetSink*/
{
public:
	//// 构造函数
	//MailSceneService();
	//// 析构函数
	//virtual ~MailSceneService();

	// _Stub::release() call
	void release();
	// _Stub::on_start call
	bool create();

    void onStop();

	/////////////////////////////////////////IMailSceneService/////////////////////////////////////////
    //// 处理其它服务器发送过来的消息
    //virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len);

 	// 发送邮件
	virtual bool	sendMail(SMailSendData sMailSendData, int nMailFillID = emMailFill_Begin, LPCSTR szParam = NULL);

	// 发送填充邮件(数据由配置表里面填充)
	virtual bool	sendFillMail(DWORD dwPDBID, int nMailFillID, LPCSTR szParam = NULL);

	////////////////////////////////////////ITransmitHandler//////////////////////////////////////////
	// 消息处理
	virtual void	onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);
	// 通知服务器列表已经更新
	virtual void	onServerListUpdated();
	// 通知服务器信息更新
	virtual void	onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData);

	///////////////////////////////////////IMessageHandler/////////////////////////////////////////
	// 消息处理
	virtual void	onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

private:

	/// purpose: 发送邮件
	bool sendMailToSocial(SMailSendData *pSendData, int nMailFillID = 0, LPCSTR szParam = NULL);
};
#endif // __MAILSCENESERVICE_H__