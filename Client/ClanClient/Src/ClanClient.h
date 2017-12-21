/*******************************************************************
** 文件名:	ClanClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/7/2016
** 版  本:	1.0
** 描  述:	帮会客户端
********************************************************************/

#ifndef __CLANCLIENT_H__
#define __CLANCLIENT_H__

#include "IClanClient.h"
#include "Clan.h"
#include "IEntityClient.h"
#include "GameViewDef.h"

// 帮会客户端
class CClanClient : public IClanClient, public IMessageHandler, public IGameViewEventHandler
{
public:
	// 构造函数
	CClanClient();

	// 虚构函数
	virtual ~CClanClient();
	
	// 创建
	bool create(void);

    ////////////////////////IClanClient////////////////////
	// 释放
	virtual void release(void);

    // 能否发布联盟杯赛
    virtual bool canPublicClanLegenCup();

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
	// 请求帮会列表返回处理
	void onClanList(LPCSTR pszMsg, int nLen);
	// 更新成员数据
	void onUpdateSelfClanInfo(LPCSTR pszMsg, int nLen);
	// 退出帮会
	void onQuit(LPCSTR pszMsg, int nLen);
	// 接到申请加入消息
	void onApply(LPCSTR pszMsg, int nLen);
	// 接到回复申请加入消息
	void onAdmit(LPCSTR pszMsg, int nLen);
    // 创建帮会成功
    void onCreateSucceed(LPCSTR pszMsg, int nLen);
	// 接到申请加入返回消息
	void onApplyRet(LPCSTR pszMsg, int nLen);
	// 邀请加入联盟
	void onInviteJoinClan(LPCSTR pszMsg, int nLen);
	// 邀请答复
	void onAnswerInviteJoinClan(LPCSTR pszMsg, int nLen);

	// 显示层消息
	// 请求帮会列表请求
	void clanListRequest(int nPageNum, char* szClanName);
	// 打开帮会界面
	void openFormRequest();
	// 创建帮会请求
	void createClanRequest(cmd_clan_create *pData);
	// 加入帮会
	void joinRequest(DWORD dwClanID);
	// 退出帮会
	void quitRequest();
	// 解散帮会
	void dismissRequest();
	// 任命指挥官
	void setClanLeader(int nDBID);
	// 加入列表请求
	void applyListRequest();
	// 接收成员
	void acceptRequest(DWORD dwKinID, bool bAgree);
	// 请求卡牌列表
	void requestCardList();
	// 请求支援
	void requestCard(int nHeroID);
	// 捐赠卡牌
	void donateCard(int nDBID, int nCardID, int nCardCount);

	// 尚未整理的显示层消息
	// 修改公告
	void motifyRequest(int nPropID, char* szText);
	// 开除成员
	void kickRequest(DWORD dwPDBID);
	// 开除帮会内的战队成员
	void kickKinMemberRequest(DWORD dwPDBID);
	// 获取本帮会事件
	void requestClanEvent();
	// 取消解散帮会
	void cancelDismissRequest();

	// 请求设置自动接受
	void requestSetAutoAcceptFlag(int nAutoAcceptFlag);

private:
	CClan m_Clan;							// 帮会对象
	
	SHeroClanInfo m_HeroClanInfo;			// 玩家帮会信息
	bool m_bMemberVaild;					// 玩家帮会信息是否有效
};

extern CClanClient* g_pClanClient;

#endif // __CLANCLIENT_H__