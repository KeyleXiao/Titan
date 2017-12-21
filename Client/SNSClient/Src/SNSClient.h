/*******************************************************************
** 文件名:	SNSClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李有红
** 日  期:	12/7/2016
** 版  本:	1.0
** 描  述:	SNS系统客户端
** 应  用:  	
	
*******************************************************************/

#pragma once
#include "ISNSClient.h"
#include "IEntityClient.h"

class SNSClient : public ISNSClient, public IMessageHandler, public IGameViewEventHandler
{
public:
	// 构造函数
	SNSClient();
	// 析构函数
	virtual ~SNSClient();

	// ISNSClient
	// 释放
	virtual void release(void);

	// IMessageHandler
	virtual void onMessage(SNetMsgHead* head, void* data, size_t len);

	// IGameViewEventHandler
	virtual bool onViewEvent( int eventID, int nParam, const char *strParam, void *ptrParam );

public:
	// SNSClient
	bool Create(void);

protected:
	// 填充信息头
	void fillSNSData(obuf& ob, int destEndPoint, BYTE msgId);

	void sendMsgToSocial(BYTE byActionID, const char *strParam = NULL, int strLen = 0, void* pData = NULL, int nLen = 0);

private:
	// 获取排位赛段位图标
	int getRankGradeIcon(int nRankGrade);
	// 获取排位赛段位名称
	string getRankGradeName(int nRankGrade);
private:

};

extern SNSClient* g_pSNSClient;