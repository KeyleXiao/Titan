/*******************************************************************
** 文件名:	MailClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	12/2/2017
** 版  本:	1.0
** 描  述:	邮件客户端
********************************************************************/

#ifndef __MAILCLIENT_H__
#define __MAILCLIENT_H__

#include "IMailClient.h"
#include "IEntityClient.h"
#include "GameViewDef.h"

// 邮件客户端
class CMailClient : public IMailClient, public IMessageHandler, public IGameViewEventHandler
{
public:
	// 构造函数
	CMailClient();

	// 虚构函数
	virtual ~CMailClient();
	
	// 创建
	bool create(void);

    ////////////////////////IMailClient////////////////////
	// 释放
	virtual void release(void);

	////////////////////////IMessageHandler////////////////////
	/** 消息处理
	@param head : 消息头，通常为一个消息包的前4个字节
    @param data : 消息的实际内容，除去了消息头的
	@param len  : 内容长度
	*/
	virtual void onMessage(SNetMsgHead *pHead, void *pData, size_t nLen);

    ////////////////////////IGameViewEventHandler////////////////////////
    virtual bool onViewEvent( int32 eventID,int nParam,const char *strParam,void *ptrParam );

private:
	// 注册显示层事件
	void registerGameViewEvent(void);
	// 注销显示层事件
	void unregisterGameViewEvent(void);

	// 服务器消息
	// 邮件基础信息列表
	void onMailBaseDataList(LPCSTR pszMsg, int nLen);
	// 邮件详细信息
	void onMailDetailData(LPCSTR pszMsg, int nLen);
	// 玩家未读邮件总数
	void onNewMailCountData(LPCSTR pszMsg, int nLen);
	// 快速收件信息
	void onMailFastRecvPlusData(LPCSTR pszMsg, int nLen);
	// 收件成功
	void onMailRecvPlusSuccess(LPCSTR pszMsg, int nLen);

	// 显示层消息
	// 请求邮件基础数据列表
	void reqMailBaseDataList();
	// 请求读取邮件
	void reqReadMailData(LONGLONG llMailID);
	// 请求快速领取
	void reqFastRecvMailPlus(gameview_mail_req_fastrecvplus *pReqData);
};

extern CMailClient* g_pMailClient;

#endif // __MAILCLIENT_H__