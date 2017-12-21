/*******************************************************************
** �ļ���:	E:\speed\Server\OrderServer\Src\StaticWarSceneService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	����ս�����ݷ���
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

// ��ɫӢ��ʤ�ʻ���
struct SActorHeroWinRateCache
{
	DWORD	dwHeroID;				// Ӣ��ID
	DWORD   dwWinNum;               // ����ʤ������
	DWORD   dwMatchCount;           // ��������

	SActorHeroWinRateCache()
	{
		memset(this, 0, sizeof(*this));
	}
};

// ��ɫս����������
struct SActorWarStatisticCache
{
	int loadState;

	PDBID	nActorID;				// ��ɫID
	DWORD	dwRecentKillCount;		// ��30��ɱ����
	DWORD	dwRecentDieCount;		// ��30��������
	DWORD	dwRecentAssistCount;	// ��30��������
	DWORD	dwRecentScore;			// ��30��ս������
	WORD	wRecentWinCount;		// ��30��ʤ��
	WORD	wRecentLoseCount;		// ��30�츺��
    DWORD   dwLatestRequest;        // ���һ����������ʱ��

	deque<DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO> warInfoList;	// ��ʷս���б�

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

// ������ɫս����������
struct SOtherActorWarStatisticCache
{
	PDBID	nActorID;				// ��ɫID
	DWORD	dwRecentKillCount;		// ��30��ɱ����
	DWORD	dwRecentDieCount;		// ��30��������
	DWORD	dwRecentAssistCount;	// ��30��������
	WORD	wRecentWinCount;		// ��30��ʤ��
	WORD	wRecentLoseCount;		// ��30�츺��

	deque<DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO> warInfoList;	// ��ʷս���б�

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

// ����ս������
struct SSingleWarStatisticCache
{
	int loadState;

	LONGLONG nWarID;							// ս��ID
	DWORD	dwBeginTime;                        // ս����ʼʱ��
	DWORD	dwEndTime;                          // ս������ʱ��
	WORD	wWarType;                           // ս������
	WORD	wEndBecause;                        // ����ԭ�� 1��������Ӫ1ʤ�� 2��������Ӫ2ʤ�� 3��������Ӫ1Ͷ�� 4��������Ӫ2Ͷ��
	WORD	wBanHeroID1[DBDEF_BAN_HERO_SIZE];	// ��Ӫ1����Ӣ���б�
	WORD	wBanHeroID2[DBDEF_BAN_HERO_SIZE];	// ��Ӫ2����Ӣ���б�

	vector<DBREQ_PARAM_WAR_ACTOR_INFO> actorinfo;	// �����ɫ��Ϣ

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
		REQ_PLAYER_INFO = 1,						// ���������Ϣ
		REQ_HERO_INFO,						    	// ����Ӣ����Ϣ
	};

public:
     //
	////////////////////////////////////////ITransmitHandler//////////////////////////////////
	/** ��Ϣ����
	@param server	Դ������ID
	@param uMsgID	��ϢID
	@param head		��Ϣͷ
	@param data		��Ϣ
	@param len		��Ϣ����
	*/
	virtual void	onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);

	/**
	@name         : ֪ͨ�������б��Ѿ�����
	@brief        : �������б����ֻ�����з����������˳�ʱ����
	@note         : ������б����ICenterServerConnector::GetServerList���
	*/
	virtual void	onServerListUpdated();

	/** ֪ͨ��������Ϣ����
	@param nState		: SERVER_STATE
	@param pServerData	: ServerData
	@brief        : ��������������仯�ȵ�
	*/
	virtual void	onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData);

	// 
	///////////////////////////////////IMessageHandler///////////////////////////////////////
	/** ��Ϣ����
	@param msg ������Ϣ
	*/
	virtual void	onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

    ///////////////////////////////////IStaticWarSceneService///////////////////////////////////////
    // �����������������͹�������Ϣ
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // ����ͻ��˷��͹�������Ϣ
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);


	// StaticWarSceneServer
	//////////////////////////////////////////////////////////////////////////
	// _Stub::release() call
	void release();
	// _Stub::on_start call
	bool create();

	// ����������һ�������
	virtual void reqOtherPlayerInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake = false);

	// �����������Ӣ������
	virtual void reqOtherPlayerHeroInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake = false);

	// IDBRetSink
	// ���ݿ����󷵻ػص�����
	virtual void OnReturn(IDBRetSink *pRealDBRetSink, int nCmdID, int nDBRetCode, char *pszDBRetDesc, int nQueueIndex, char *pOutData, int nOutLen);

private:
    // DB�����ɫս������
	void onDBReturnWarGetActorGame(int nQueueIndex, void *pOutData, int nOutLen);

	// DB�����ɫӢ����Ϣ����
	void onDBReturnQueryAllHero(int nQueueIndex, void *pOutData, int nOutLen);

	// DB�����ɫ��ͼս������
	void onDBReturnGetPlayerMap(int nQueueIndex, void *pOutData, int nOutLen);

	// DB���󵥸�ս������
	void onDBReturnGetWarGame(int nQueueIndex, void *pOutData, int nOutLen);

	// ���ͽ�ɫս���������ͻ���
	void sendActorWarStatisticToClient(ClientID clientID, SActorWarStatisticCache *cache);

	// ���͵���ս�����ͻ���
	void sendWarStatisticToClient(ClientID clientID, SSingleWarStatisticCache *cache, BYTE byWnd = 1);

	// ����KDA����
	int calcKDAScore(int nKillNum, int nDieNum, int nAssistNum);

	// ��ֵ��ҽṹ��Ϣ
	void fillPlayerInfo(msg_entity_player_info_return & playerInfo, SPlayerInfo & info);
	void fillPlayerInfo(msg_entity_player_info_return & playerInfo, DBREQ_RESULT_QUERYACTORLOGIN* pInfo);
    // ��ֵ�����������
    void fillFakePlayerInfo(msg_entity_player_info_return *playerInfo, DBREQ_RESULT_QUERYACTORLOGIN* pInfo);
	// ��ֵ���Ӣ�۽ṹ��Ϣ
	void fillHeroInfoMsg(msg_entity_hero_info &sMsg, const SHeroInfo &sHeroInfo);
	// ��ó���Ӣ����Ϣ
	int getStrongHeroInfobBySort(int nPDBID, obuf &obData);

    // ��ȡ���˳���Ӣ����Ϣ
	int getFakeStrongHeroInfobBySort(vector<SHeroInfo> &vecHero, obuf &obData);

	// ���ͻ��˷��������Ϣ
	void SendPlayerInfoToClient(PDBID nPDBID, BYTE byAction, void *pData,int nLen);

	// ���ݿⷵ�������Ϣ
	void OnDBReturnPlayerInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// ���ݿⷵ��Ӣ����Ϣ
	void OnDBReturnHeroInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// ���ݿⷵ��ƥ��������
	void OnDBReturnMatchTypeRank(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// ���ݿⷵ�������ʷս��
	void OnDBReturnHistoryRecord(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// ���ݿⷵ��������ҵĺ˾�ҳ����
	void onReturnReadOtherPlayerCrownPages(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// ���ݿⷵ��������ҵĺ˾�����
	void onDBReturnOtherPlayerAllGemstoneInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// ����Ҫ��ȡ�ļ����б�
    void OnDBReturnFakeActorList(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

    // ȡ��������
    void reqFakePlayerInfo(DWORD dwSelfID,DWORD dwReqID);

    // ȡ��������
    void reqTruePlayerInfo(DWORD dwSelfID,DWORD dwReqID);

    // ȡ����Ӣ������
    void reqFakePlayerHeroInfo(DWORD dwSelfID,DWORD dwReqID);

    // ȡ����Ӣ������
    void reqTruePlayerHeroInfo(DWORD dwSelfID,DWORD dwReqID);

	// �ͻ�����������ҵĺ˾�����
	void onMessageRequestOtherPlayerCrownPages(DWORD client, void * data, size_t nLen);
	// ����������ҵĺ˾����ݵ��ͻ���
	void sendOtherPlayerCrownPagesToClient(DWORD clientID, void *pOutData, int nOutLen);
private:

    unordered_map<PDBID, SActorWarStatisticCache*> m_actorCache;

	unordered_map<LONGLONG, SSingleWarStatisticCache*> m_warCache;

	//�������������Ϣ����  
	static const int nHeroInfoDBRequestCount = 1;
	static const int nPlayerInfoDBRequestCount = 2;
	unordered_map<PDBID, msg_entity_other_actor_info_all*> m_mapOtherPlayInfoCache;

    // ���˻�������
    vector< msg_entity_player_info_return*> m_mapDBFakePlayInfoCache;
    vector< vector<SHeroInfo>> m_mapDBFakeHeroCache;
    // ���˶�Ӧ�������Ϣ
    map<PDBID, SFakePlayerInfo > m_mapFakePlayerInfo;

	unordered_map<PDBID, SOtherPlayerCrownpages> m_mapOtherPlayerCrownPages;
};