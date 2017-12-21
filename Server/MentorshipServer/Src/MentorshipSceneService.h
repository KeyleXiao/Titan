/*******************************************************************
** 文件名:	MentorshipSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/29/2016
** 版  本:	1.0
** 描  述:	师徒场景服务
********************************************************************/

#ifndef __MENTORSHIPSCENESERVICE_H__
#define __MENTORSHIPSCENESERVICE_H__

#include "IMentorshipSceneService.h"
#include "NetMessageDef.h"
#include "Mentorship.h"
#include "IDBRetSink.h"
#include "MentorshipMsgDef.h"
#include "IMessageHandler.h"

class MentorshipSceneService : public IMentorshipSceneService, public ITransmitHandler, public IMessageHandler, public IEventExecuteSink, public IDBRetSink
{
// 师徒列表
typedef stdext::hash_map<DWORD, CMentorship*>						TMap_Mentorship;
// 师徒列表迭代器
typedef stdext::hash_map<DWORD, CMentorship*>::iterator				TMap_MentorshipIt;

// 成员师徒列表
typedef stdext::hash_map<DWORD, vector<DWORD>>						TMap_MemMentorship;
// 成员师徒列表迭代器
typedef stdext::hash_map<DWORD, vector<DWORD>>::iterator			TMap_MemMentorshipIt;

public:
	// 构造函数
	MentorshipSceneService();
	// 析构函数
	virtual ~MentorshipSceneService();

	// _Stub::release() call
	void release();
	// _Stub::on_start call
	bool create();

    void onStop();

	/////////////////////////////////////////IMentorshipSceneService/////////////////////////////////////////
    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // 处理客户端发送过来的消息
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

	// 发送战斗结果
	virtual void	sendFightResult(DWORD dwPDBID, bool bFightResult, bool bFirstWin, BYTE nMentorshipPost);

	// 师傅存在师徒关系
	virtual BYTE	getMyMentorshipPost(DWORD dwSrcPDBID, DWORD dwDesPDBID);

	// 获取师徒奖励
	virtual void	getMentorshipAward(DWORD dwPDBID, int* nExpPercent, int* nHeroExpPercent, int* nGoldPercent);

	// 获取师傅个数
	virtual int getPlayerMasterCount(DWORD dwDBID);

	// 获取学徒个数
	virtual int getPlayerPrenticeCount(DWORD dwDBID);

	// 检测目标是否为玩家徒弟
	virtual int checkIsPlayerPrentice(DWORD dwDBID, DWORD dwTargetDBID);
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

    /////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    //////////////////////////////////IDBRetSink///////////////////////
    // 数据库请求返回回调方法
    virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

private:
	// 获取师徒索引
	int getMentorshipIndex();
	// 查找师徒数据
	CMentorship* findMentorship(DWORD dwMentorshipID);
	// 查找师徒数据
	CMentorship* findMentorshipByPDBID(DWORD dwMasterPDBID, DWORD dwPrenticePDBID);
	// 通知任务
	void sendPlayerTaskEvent(DWORD dwPDBID, DWORD dwDesPDBID);
private:
    // 更新师徒数据
    void onTransmitUpdateMentorship(LPCSTR pszMsg, int nLen);
	// 解散师徒数据
	void onTransmitDismissMentorship(LPCSTR pszMsg, int nLen);
	// 发放首胜奖励
	void onTransmitGiveFirstWinAward(LPCSTR pszMsg, int nLen);
	// 移除所有师徒关系
	void onTransmitRemoveAllMentorship(LPCSTR pszMsg, int nLen);

private:
	int					     m_nMentorshipIndex;

    TMap_Mentorship          m_MentorshipList;				// 师徒列表
	TMap_MemMentorship		 m_MasterList;
	TMap_MemMentorship		 m_PrenticeList;
};
#endif // __MENTORSHIPSCENESERVICE_H__