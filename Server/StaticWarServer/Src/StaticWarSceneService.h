/*******************************************************************
** 文件名:	E:\speed\Server\OrderServer\Src\StaticWarSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	所有战场数据服务
********************************************************************/

#pragma once
#include "IStaticWarSceneService.h"
#include "NetMessageDef.h"
#include "IMessageHandler.h"
#include "IMessageDispatch.h"
#include "IDBRetSink.h"
#include <unordered_map>
#include "IGameDatabaseDef.h"
#include "IDBEngine.h"
#include "EntityDef.h"

// 角色英雄胜率缓存
struct SActorHeroWinRateCache
{
	DWORD	dwHeroID;				// 英雄ID
	DWORD   dwWinNum;               // 比赛胜利场次
	DWORD   dwMatchCount;           // 比赛场次

	SActorHeroWinRateCache()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 角色战绩总览缓存
struct SActorWarStatisticCache
{
	int loadState;

	PDBID	nActorID;				// 角色ID
	DWORD	dwRecentKillCount;		// 近30天杀敌数
	DWORD	dwRecentDieCount;		// 近30天死亡数
	DWORD	dwRecentAssistCount;	// 近30天助攻数
	DWORD	dwRecentScore;			// 近30天战绩评分
	WORD	wRecentWinCount;		// 近30天胜场
	WORD	wRecentLoseCount;		// 近30天负场
    DWORD   dwLatestRequest;        // 最近一次请求数据时间

	deque<DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO> warInfoList;	// 历史战绩列表

	SActorWarStatisticCache()
	{
		loadState = 0;
		nActorID = 0;
		dwRecentKillCount = 0;
		dwRecentDieCount = 0;
		dwRecentAssistCount = 0;
		wRecentWinCount = 0;
		wRecentLoseCount = 0;
	}
};

// 其他角色战绩总览缓存
struct SOtherActorWarStatisticCache
{
	PDBID	nActorID;				// 角色ID
	DWORD	dwRecentKillCount;		// 近30天杀敌数
	DWORD	dwRecentDieCount;		// 近30天死亡数
	DWORD	dwRecentAssistCount;	// 近30天助攻数
	WORD	wRecentWinCount;		// 近30天胜场
	WORD	wRecentLoseCount;		// 近30天负场

	deque<DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO> warInfoList;	// 历史战绩列表

	SOtherActorWarStatisticCache()
	{
		nActorID = 0;
		dwRecentKillCount = 0;
		dwRecentDieCount = 0;
		dwRecentAssistCount = 0;
		wRecentWinCount = 0;
		wRecentLoseCount = 0;
	}
};

// 单场战绩缓存
struct SSingleWarStatisticCache
{
	int loadState;

	LONGLONG nWarID;							// 战场ID
	DWORD	dwBeginTime;                        // 战场开始时间
	DWORD	dwEndTime;                          // 战场结束时间
	WORD	wWarType;                           // 战场类型
	WORD	wEndBecause;                        // 结束原因 1、代表阵营1胜利 2、代表阵营2胜利 3、代表阵营1投降 4、代表阵营2投降
	WORD	wBanHeroID1[DBDEF_BAN_HERO_SIZE];	// 阵营1禁用英雄列表
	WORD	wBanHeroID2[DBDEF_BAN_HERO_SIZE];	// 阵营2禁用英雄列表

	vector<DBREQ_PARAM_WAR_ACTOR_INFO> actorinfo;	// 参与角色信息

	SSingleWarStatisticCache()
	{
		loadState = 0;
		nWarID = 0;
		dwBeginTime = 0;
		dwEndTime = 0;
		wWarType = 0;
		wEndBecause = 0;
		memset(&wBanHeroID1, 0, sizeof(wBanHeroID1));
		memset(&wBanHeroID2, 0, sizeof(wBanHeroID2));
	}
};

// 
struct SFakePlayerInfo
{
    msg_entity_player_info_return* pPlayerInfo;
    vector<SHeroInfo>* pHeroInfo; 

    SFakePlayerInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};

enum
{
	EOTHER_PLAYER_CROWN_INIT = 0,
	EOTHER_PLAYER_CROWN_PAGE = 1,
	EOTHER_PLAYER_CROWN_GEMSTONE = (1 << 1),
};
struct SOtherPlayerCrownpages
{
	int loadState;
	CROWN_PAGE_ITEM  crownPageList[CROWN_PAGE_MAX_COUNT];
	GEMSTONE_ITEM    crownGemstoneList[CROWN_GEMSTONE_MAX_COUNT];
	SOtherPlayerCrownpages()
	{
		memset(this, 0, sizeof(*this));
	}
};

class StaticWarScenService:public IStaticWarSceneService, public ITransmitHandler,public IMessageHandler,public IDBRetSink
{
	enum
	{
		REQ_PLAYER_INFO = 1,						// 请求玩家信息
		REQ_HERO_INFO,						    	// 请求英雄信息
	};

public:
     //
	////////////////////////////////////////ITransmitHandler//////////////////////////////////
	/** 消息处理
	@param server	源服务器ID
	@param uMsgID	消息ID
	@param head		消息头
	@param data		消息
	@param len		消息长度
	*/
	virtual void	onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);

	/**
	@name         : 通知服务器列表已经更新
	@brief        : 服务器列表更新只会在有服务器进入退出时触发
	@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
	*/
	virtual void	onServerListUpdated();

	/** 通知服务器信息更新
	@param nState		: SERVER_STATE
	@param pServerData	: ServerData
	@brief        : 例如服务器人数变化等等
	*/
	virtual void	onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData);

	// 
	///////////////////////////////////IMessageHandler///////////////////////////////////////
	/** 消息处理
	@param msg 网络消息
	*/
	virtual void	onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

    ///////////////////////////////////IStaticWarSceneService///////////////////////////////////////
    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // 处理客户端发送过来的消息
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);


	// StaticWarSceneServer
	//////////////////////////////////////////////////////////////////////////
	// _Stub::release() call
	void release();
	// _Stub::on_start call
	bool create();

	// 请求其他玩家基本数据
	virtual void reqOtherPlayerInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake = false);

	// 请求其他玩家英雄数据
	virtual void reqOtherPlayerHeroInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake = false);

	// IDBRetSink
	// 数据库请求返回回调方法
	virtual void OnReturn(IDBRetSink *pRealDBRetSink, int nCmdID, int nDBRetCode, char *pszDBRetDesc, int nQueueIndex, char *pOutData, int nOutLen);

private:
    // DB请求角色战绩返回
	void onDBReturnWarGetActorGame(int nQueueIndex, void *pOutData, int nOutLen);

	// DB请求角色英雄信息返回
	void onDBReturnQueryAllHero(int nQueueIndex, void *pOutData, int nOutLen);

	// DB请求角色地图战绩返回
	void onDBReturnGetPlayerMap(int nQueueIndex, void *pOutData, int nOutLen);

	// DB请求单个战绩返回
	void onDBReturnGetWarGame(int nQueueIndex, void *pOutData, int nOutLen);

	// 发送角色战绩总览到客户端
	void sendActorWarStatisticToClient(ClientID clientID, SActorWarStatisticCache *cache);

	// 发送单场战绩到客户端
	void sendWarStatisticToClient(ClientID clientID, SSingleWarStatisticCache *cache, BYTE byWnd = 1);

	// 计算KDA分数
	int calcKDAScore(int nKillNum, int nDieNum, int nAssistNum);

	// 赋值玩家结构信息
	void fillPlayerInfo(msg_entity_player_info_return & playerInfo, SPlayerInfo & info);
	void fillPlayerInfo(msg_entity_player_info_return & playerInfo, DBREQ_RESULT_QUERYACTORLOGIN* pInfo);
    // 赋值假人玩家数据
    void fillFakePlayerInfo(msg_entity_player_info_return *playerInfo, DBREQ_RESULT_QUERYACTORLOGIN* pInfo);
	// 赋值玩家英雄结构信息
	void fillHeroInfoMsg(msg_entity_hero_info &sMsg, const SHeroInfo &sHeroInfo);
	// 获得常用英雄信息
	int getStrongHeroInfobBySort(int nPDBID, obuf &obData);

    // 获取假人常用英雄信息
	int getFakeStrongHeroInfobBySort(vector<SHeroInfo> &vecHero, obuf &obData);

	// 给客户端发送玩家信息
	void SendPlayerInfoToClient(PDBID nPDBID, BYTE byAction, void *pData,int nLen);

	// 数据库返回玩家信息
	void OnDBReturnPlayerInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// 数据库返回英雄信息
	void OnDBReturnHeroInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// 数据库返回匹配撮合数据
	void OnDBReturnMatchTypeRank(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// 数据库返回玩家历史战绩
	void OnDBReturnHistoryRecord(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// 数据库返回其他玩家的核晶页数据
	void onReturnReadOtherPlayerCrownPages(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// 数据库返回其他玩家的核晶数据
	void onDBReturnOtherPlayerAllGemstoneInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// 返回要读取的假人列表
    void OnDBReturnFakeActorList(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

    // 取假人数据
    void reqFakePlayerInfo(DWORD dwSelfID,DWORD dwReqID);

    // 取真人数据
    void reqTruePlayerInfo(DWORD dwSelfID,DWORD dwReqID);

    // 取假人英雄数据
    void reqFakePlayerHeroInfo(DWORD dwSelfID,DWORD dwReqID);

    // 取真人英雄数据
    void reqTruePlayerHeroInfo(DWORD dwSelfID,DWORD dwReqID);

	// 客户端请求别的玩家的核晶数据
	void onMessageRequestOtherPlayerCrownPages(DWORD client, void * data, size_t nLen);
	// 发送其他玩家的核晶数据到客户端
	void sendOtherPlayerCrownPagesToClient(DWORD clientID, void *pOutData, int nOutLen);
private:

    unordered_map<PDBID, SActorWarStatisticCache*> m_actorCache;

	unordered_map<LONGLONG, SSingleWarStatisticCache*> m_warCache;

	//请求其他玩家信息缓存  
	static const int nHeroInfoDBRequestCount = 1;
	static const int nPlayerInfoDBRequestCount = 2;
	unordered_map<PDBID, msg_entity_other_actor_info_all*> m_mapOtherPlayInfoCache;

    // 假人缓存数据
    vector< msg_entity_player_info_return*> m_mapDBFakePlayInfoCache;
    vector< vector<SHeroInfo>> m_mapDBFakeHeroCache;
    // 假人对应的相关信息
    map<PDBID, SFakePlayerInfo > m_mapFakePlayerInfo;

	unordered_map<PDBID, SOtherPlayerCrownpages> m_mapOtherPlayerCrownPages;
};