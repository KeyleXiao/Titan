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
	// 构造函数
	MailSceneService();
	// 析构函数
	virtual ~MailSceneService();

	// _Stub::release() call
	void release();
	// _Stub::on_start call
	bool create();

    void onStop();

	/////////////////////////////////////////IMailSceneService/////////////////////////////////////////
    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // 处理客户端发送过来的消息
    //virtual void    handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

	//// 删除邮件
	//virtual bool    delMail(DWORD dwPDBID, LONGLONG llMailID);

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

 //   /////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	//virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ////////////////////////////////////IDBRetSink///////////////////////
    //// 数据库请求返回回调方法
    //virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	//////////////////////////////////MailSceneService///////////////////////

private:

	//// purpose: 读取邮件详细数据				-- 来自数据库
	//void onReturnReadMailData(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
	//// purpose: 删除邮件						-- 来自数据库
	//void onReturnDelMail(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
	
	//// 请求玩家邮件列表
	//void onMsgRequestMailList(ClientID clientID, void* pData, size_t stLen);
	// 请求阅读邮件
	//void onMsgRequestReadMail(ClientID clientID, void* pData, size_t stLen);
	// 请求快速领取
	//void onMsgRequestFastRecv(ClientID clientID, void* pData, size_t stLen);

	/// purpose: 发送邮件
	bool sendMailToSocial(SMailSendData *pSendData, int nMailFillID = 0, LPCSTR szParam = NULL);

	//// 更新邮件
	//void setMailReadToSocial(DWORD dwPDBID, LONGLONG llMailID);
};
#endif // __MAILSCENESERVICE_H__