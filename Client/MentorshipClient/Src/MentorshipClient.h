/*******************************************************************
** 文件名:	MentorshipClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	12/2/2016
** 版  本:	1.0
** 描  述:	师徒客户端
********************************************************************/

#ifndef __MENTORSHIPCLIENT_H__
#define __MENTORSHIPCLIENT_H__

#include "IMentorshipClient.h"
#include "Mentorship.h"
#include "IEntityClient.h"
#include "GameViewDef.h"

// 师徒客户端
class CMentorshipClient : public IMentorshipClient, public IMessageHandler, public IGameViewEventHandler
{
public:
	// 构造函数
	CMentorshipClient();

	// 虚构函数
	virtual ~CMentorshipClient();
	
	// 创建
	bool create(void);

    ////////////////////////IMentorshipClient////////////////////
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
	// 师徒关系申请应答
	void onApplyAnswerData(LPCSTR pszMsg, int nLen);
	// 拜师收徒发布信息列表
	void onIssueListData(LPCSTR pszMsg, int nLen);
	// 拜师收徒信息发布自动设置
	void onIssueAutoSetData(LPCSTR pszMsg, int nLen);
	// 导师的学徒列表
	void onPrenticeListData(LPCSTR pszMsg, int nLen);

	// 显示层消息
	// 请求师徒数据请求
	void mentorshipPersonDataRequest();
	// 导师学徒列表请求
	void prenticeListRequest(cmd_mentorship_prenticelist_head *pData);
	// 发布列表请求
	void issueListRequest(cmd_mentorship_issuelist_head *pData);
	// 发布消息请求
	void issueInfoRequest(cmd_mentorship_request_issue_info *pData);
	// 拜师或收徒信息请求
	void applyMentorshipRequest(cmd_mentorship_request_apply *pData);
	// 拜师或收徒应答信息请求
	void applyAnswerRequest(cmd_mentorship_request_applyanswer *pData);
	// 提示学徒信息请求
	void promotePrenticeRequest(cmd_mentorship_request_promote *pData);
	// 自动发布信息请求
	void issueAutoSetRequest(cmd_mentorship_request_issueautoset *pData);
	// 解散师徒
	void dismissRequest(cmd_mentorship_request_dismiss *pData);
	// 领取师徒首胜奖励
	void recvFirstAwardRequest(cmd_mentorship_request_firstaward *pData);

private:
	CMentorship m_Mentorship;				// 师徒对象
};

extern CMentorshipClient* g_pMentorshipClient;

#endif // __MENTORSHIPCLIENT_H__