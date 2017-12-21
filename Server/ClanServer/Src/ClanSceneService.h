/*******************************************************************
** 文件名:	ClanSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/6/2016
** 版  本:	1.0
** 描  述:	帮会场景服务
********************************************************************/

#ifndef __CLANSCENESERVICE_H__
#define __CLANSCENESERVICE_H__

#include "IClanSceneService.h"
#include "NetMessageDef.h"
#include "Clan.h"
#include "IDBRetSink.h"
#include "ClanMessageDef.h"
#include "IMessageHandler.h"

class ClanScenService : public IClanSceneService, public ITransmitHandler, public IMessageHandler, public IEventExecuteSink, public IDBRetSink
{
public:
	// 构造函数
	ClanScenService();
	// 析构函数
	virtual ~ClanScenService();

	// _Stub::release() call
	void release();
	// _Stub::on_start call
	bool create();

    void onStop();

	/////////////////////////////////////////IClanSceneService/////////////////////////////////////////
	// 获取帮会名称
	virtual string getClanName(DWORD nClanID);

    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // 处理客户端发送过来的消息
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

    virtual int getNumProp(DWORD dwClaneID, DWORD dwProp);

	virtual bool setNumProp(DWORD dwClanID, DWORD dwProp, int nValue);

	// 检查创建联盟杯赛
	virtual bool checkCreateClanLegendCup(DWORD dwClanID, int LegendCupConfigID);

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
    // 根据帮会ID查找帮会
    CClan* findClan(DWORD dwClanID);
    // 根据战队ID查找帮会成员
    SZoneClanMember* findClanKin(DWORD dwKinID);

    // 调用DB存储过程创建帮会
	void onDBProcCreateClan(msg_clan_create_os *pInfo);
    // 回退帮会操作
    //@ note   : 当通知数据库创建帮会成功返回，场景服发现创建帮会的条件已经不成立，需要取消先前的创建操作
    void backUpCreateClan(DWORD dwClanID, LPCSTR szTrackNote);

    // 往中心服创建帮会			-- 来自数据库
    void onReturnCenterCreateClan(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
    // 创建帮会					-- 来自数据库
	void onReturnCreateClan(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
	// 请求捐赠记录DB返回
	void onReturnGetClanDonateInfo(int nDBRetCode, char *pszDBRetDesc, int nQueueIndex, char *pOutData, int nOutLen);

private:
    // 创建帮会
    void onTransmitCreateClan(LPCSTR pszMsg, int nLen);
    // 删除帮会
    void onTransmitRemoveClan(LPCSTR pszMsg, int nLen);
    // 更新帮会（没有则添加）
    void onTransmitUpdateClan(LPCSTR pszMsg, int nLen);
    // 更新帮会属性
    void onTransmitUpdateClanProp(LPCSTR pszMsg, int nLen);
    // 同步数据
    void onTransmitSynchroFinish(LPCSTR pszMsg, int nLen);
    // 删除成员
    void onTransmitRemoveMember(LPCSTR pszMsg, int nLen);
    // 更新成员（没有则添加）
    void onTransmitUpdateZoneMember(LPCSTR pszMsg, int nLen);
    // 设置玩家的帮会ID
    void onTransmitUpdateClanID(LPCSTR pszMsg, int nLen);
    // 转发消息
    void onTransmitPostMsg(LPCSTR pszMsg, int nLen);
    // 更新主角帮会信息中转
	void onTransmitUpdateSelfClanInfo(DWORD dwMsgID, LPCSTR pszMsg, int nLen);
	// 发送捐卡信息到客户端
	void onTransmitSendRequestCardInfo(LPCSTR pszMsg, int nLen);
    // 请求卡牌支援
    void onTransmitRequestCard(LPCSTR pszMsg, int nLen);
    // 捐赠卡牌
    void onTransmitDonateCard(LPCSTR pszMsg, int nLen);
    // 增加捐赠卡牌
    void onTransmitAddDonateCard(LPCSTR pszMsg, int nLen);
    // 请求卡牌列表
    void onTransmitClanRequestCardList(LPCSTR pszMsg, int nLen);
    // 更新请求卡牌
    void onTransmitClanUpdateRequestCard(LPCSTR pszMsg, int nLen);

	// 辅助函数
	// 发送捐卡信息到客户端
	void SendRequestCardInfo(PDBID nActorID);

private:
    map<DWORD, CClan*>          m_ClanList;				// 帮会列表
    map<DWORD, SZoneClanMember> m_ClanMemberList;		// 帮会成员列表

    // <属性值,帮会ID>
    multimap<int, DWORD, greater<DWORD> > m_mapPropValueClan;	// 某属性帮会排名列表  --仅供GetFirstPlaceActivityClanID使用
    multimap<int, DWORD, greater<DWORD> >::iterator m_mapPropValueClanIter;

	struct SCreateClanInfo
	{
		PDBID nActorID;
		char szClanManifesto[CLAN_MANIFESTO_SIZE];
		int nFlag;

		SCreateClanInfo()
		{
			memset(this, 0, sizeof(*this));
		}
	};
    map<DWORD, SCreateClanInfo>			m_mapCrtClanPlayer;		// key(申请创建帮会的玩家)->value(是否创建为正式帮会)
};
#endif // __CLANSCENESERVICE_H__