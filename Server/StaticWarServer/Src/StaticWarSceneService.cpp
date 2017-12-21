#include "stdafx.h"
#include "StaticWarSceneService.h"
#include "OrderDef.h"
#include "ICenterConnectorService.h"
#include "IServerGlobal.h"
#include "EntityHelper.h"
#include "StaticWarSceneHelper.h"
#include "WarStaticMessageDef.h"
#include "SchemeWarDef.h"
#include "IDBEngineService.h"
#include "IShareServer.h"
#include "IKinScene.h"
#include "IKinSceneService.h"
#include "IClanScene.h"
#include "IClanSceneService.h"
#include "ClanHelper.h"

bool SortHeroInfo(const SActorHeroWinRateCache &v1, const SActorHeroWinRateCache &v2)
{
	if (v1.dwMatchCount != v2.dwMatchCount)
		return v1.dwMatchCount > v2.dwMatchCount;
	return v1.dwHeroID < v2.dwHeroID;
}

void StaticWarScenService::release()
{
	TraceLn("StaticWarSceneService::release");

    // 释放申请的数据
    for (unordered_map<PDBID, msg_entity_other_actor_info_all*>::iterator iter= m_mapOtherPlayInfoCache.begin();
            iter != m_mapOtherPlayInfoCache.end(); ++iter)
    {
        if(iter->second != NULL)
        {
            iter->second->heroInfoList.clear();
            delete iter->second;
            iter->second = NULL;
        }

    }
    m_mapOtherPlayInfoCache.clear();

	m_mapOtherPlayerCrownPages.clear();

    for (unordered_map<PDBID, SActorWarStatisticCache*>::iterator iter= m_actorCache.begin();
            iter != m_actorCache.end(); ++iter)
    {
        if(iter->second != NULL)
        {
            iter->second->warInfoList.clear();
            delete iter->second;
            iter->second = NULL;
        }

    }
    m_actorCache.clear();

    for (unordered_map<LONGLONG, SSingleWarStatisticCache*> ::iterator iter= m_warCache.begin();
            iter != m_warCache.end(); ++iter)
    {
        if(iter->second != NULL)
        {
            iter->second->actorinfo.clear();
            delete iter->second;
            iter->second = NULL;
        }

    }
    m_warCache.clear();

    // 假人对应的相关信息
    m_mapFakePlayerInfo.clear();

    // 假人缓存人物数据
    for (vector< msg_entity_player_info_return*> ::iterator iter= m_mapDBFakePlayInfoCache.begin();
            iter != m_mapDBFakePlayInfoCache.end(); ++iter)
    {
        if(*iter != NULL)
        {
            delete *iter;
        }

    }
    m_mapDBFakePlayInfoCache.clear();

    // 假人缓存英雄数据
    for (vector< vector<SHeroInfo>> ::iterator iter= m_mapDBFakeHeroCache.begin();
            iter != m_mapDBFakeHeroCache.end(); ++iter)
    {
        
        iter->clear();
    }
    m_mapDBFakeHeroCache.clear();

    IMessageDispatch* pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (nullptr == pMessageDispatch)
    {
        ErrorLn(__FUNCTION__ ": failed! can not found the IMessageDispatch! ");
        return;
    }

    // 自己需要处理消息
    pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_STATICWAR);

    // 取消注册响应客户端消息
    pMessageDispatch->unregisterMessageHandler( MSG_MODULEID_STATICWAR );

    // 取消注册DB回调
	gServerGlobal->getDBEngineService()->unregisterHandler(this);

	delete this;
}

bool StaticWarScenService::create()
{
	TraceLn("StaticWarSceneService::create...");

    IMessageDispatch* pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (nullptr == pMessageDispatch)
    {
        ErrorLn(__FUNCTION__": failed! can not found the IMessageDispatch! ");
        return false;
    }
    // 自己需要处理消息
    pMessageDispatch->registerTransmitHandler(MSG_MODULEID_STATICWAR, this);

    // 处理客户端发送过来的消息
    pMessageDispatch->registerMessageHandler(MSG_MODULEID_STATICWAR, this);

    // 注册DB回调
	gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);

    // 读取数据库返回的假人列表
    gServerGlobal->getDBEngineService()->executeSP(GAMEDB_REQUEST_FAKE_ACTORLOGIN_LIST, 0, NULL, 0, this);

	return true;
}


void StaticWarScenService::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
{
    StaticWarSceneHelper StaticWarSceneHelper;
    IStaticWarSceneService *pStaticWarSceneService = StaticWarSceneHelper.m_ptr;
    if (pStaticWarSceneService == NULL)
    {
        ErrorLn(__FUNCTION__": server ="<<server<<" uMsgID ="<<uMsgID);
        return;
    }

    PACKAGE_PTR pkg( new string((const char*)data,len));
    pStaticWarSceneService->handleServerMsg( server, *head, pkg );
}

void StaticWarScenService::onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len)
{
    //TraceLn(__FUNCTION__": clientID=" << clientID << " byKeyAction=" << pHead->byKeyAction << " stLen=" << stLen);

    StaticWarSceneHelper StaticWarSceneHelper;
    IStaticWarSceneService *pStaticWarSceneService = StaticWarSceneHelper.m_ptr;
    if (pStaticWarSceneService == NULL)
    {
        ErrorLn(__FUNCTION__": clientID ="<<clientID<<" uMsgID ="<<uMsgID);
        return;
    }

    PACKAGE_PTR pkg( new string((const char*)data,len));
    pStaticWarSceneService->handleClientMsg( clientID, *head, pkg );
}


void StaticWarScenService::handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg)
{
    TraceLn(__FUNCTION__" serverID=" << serverID << " KeyAction=" << head.byKeyAction);
    switch( head.byKeyAction )
    {
    default:
        {
            ErrorLn(__FUNCTION__": unknown KeyAction=" << head.byKeyAction);
        }
    }
}

void StaticWarScenService::handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg)
{
    size_t len = msg->size();
    void *data = (void *)msg->c_str();

    switch(head.byKeyAction)
	{
	case CS_MSG_WAR_GET_WARSTATICS_TOTAL:
		{
			if (len != sizeof(msg_war_get_warstatics_total))
			{
				WAR_WARNINGLN(__FUNCTION__": CS_MSG_WAR_GET_WARSTATICS_TOTAL len error " << len);
				return;
			}

			msg_war_get_warstatics_total *pGetActor = (msg_war_get_warstatics_total*)data;
			PDBID actorID = pGetActor->actorID;
			unordered_map<PDBID, SActorWarStatisticCache*>::iterator iter = m_actorCache.find(actorID);
			if (iter != m_actorCache.end())
			{
                if (time(NULL) - iter->second->dwLatestRequest < 60)
                {
                    if (iter->second->loadState >= 1)
                    {
                        // 取缓存数据
                        sendActorWarStatisticToClient(client, iter->second);
                        return;
                    }
                    else
                    {
                        // 请求数据库等返回中，不处理
                    }
                }
			}
			
            // 大于60秒间隔重新请求一次
            if(iter == m_actorCache.end())
            {
                SActorWarStatisticCache *pNewCache = new SActorWarStatisticCache;
			    pNewCache->nActorID = actorID;
			    m_actorCache[actorID] = pNewCache;
            }

			// 请求数据库
			DBREQ_PARAM_WAR_GET_ACTORGAME actorParam;
			actorParam.dwActorID = actorID;
			gServerGlobal->getDBEngineService()->executeSP(GAMEDB_REQUEST_WAR_GET_ACTORGAME, client, (LPCSTR)&actorParam, sizeof(actorParam), this);
            
		}
		break;
	case CS_MSG_WAR_GET_WARSTATICS_WAR:
		{
			if (len != sizeof(msg_war_get_warstatics_war))
			{
				WAR_WARNINGLN(__FUNCTION__": CS_MSG_WAR_GET_WARSTATICS_WAR len error " << len);
				return;
			}

			msg_war_get_warstatics_war *pGetWar = (msg_war_get_warstatics_war*)data;
			unordered_map<LONGLONG, SSingleWarStatisticCache*>::iterator iter = m_warCache.find(pGetWar->nWarID);
			if (iter != m_warCache.end())
			{
				if (iter->second->loadState >= 1)
				{
					// 取缓存数据
					sendWarStatisticToClient(client, iter->second, pGetWar->byWnd);
				}
				else
				{
					// 请求数据库等返回中，不处理
				}
			}
			else
			{
				SSingleWarStatisticCache *newCache = new SSingleWarStatisticCache;
				m_warCache[pGetWar->nWarID] = newCache;

				// 请求数据库
				DBREQ_PARAM_WAR_GET_WARGAME warParam;
				warParam.nWarID = pGetWar->nWarID;
                warParam.byWnd  = pGetWar->byWnd;
				gServerGlobal->getDBEngineService()->executeSP(GAMEDB_REQUEST_WAR_GET_WARGAME, client, (LPCSTR)&warParam, sizeof(warParam), this);
			}
		}
		break;
	case CS_MSG_WAR_GET_HISTORY_RECORD:
		{
			if (len != sizeof(msg_war_get_history_record))
			{
				WAR_WARNINGLN(__FUNCTION__": CS_MSG_WAR_GET_HISTORY_RECORD len error " << len);
				return;
			}

			msg_war_get_history_record* pData = (msg_war_get_history_record*)data;
			DBREQ_PARAM_QUERY_HISTORY_RECORD param;
			param.dwActorID = pData->actorID;
			if (gServerGlobal->getDBEngineService()->executeSP(GAMEDB_REQUEST_QUERY_HISTORY_RECORD, param.dwActorID, (LPCSTR)&param, sizeof(param), static_cast<IDBRetSink*>(this)) == false)
			{
				ErrorLn(__FUNCTION__": pDBEngine->executeSP() DBREQ_PARAM_QUERY_HISTORY_RECORD failed");
				return;
			}
		}
		break;
	case CS_MSG_WAR_CROWNDOTHER_PAGE:         // 请求别人皇冠页
		{
			onMessageRequestOtherPlayerCrownPages(client, data, len);
		}
		break;
	default:
		{
			WAR_WARNINGLN("CWarStatistic::onMessage unknown pHead.byKeyAction=" << head.byKeyAction);
		}
		break;
	}
}

void StaticWarScenService::onMessageRequestOtherPlayerCrownPages(DWORD clientID, void * data, size_t nLen)
{
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__ << " : failed pDBEngine == NULL");
		return;
	}

	if (data == NULL || nLen < sizeof(Msg_Request_Other_Crown_Points))
	{
		ErrorLn(__FUNCTION__": message data is null or length is invalid! len=" << nLen << ", sizeof=" << sizeof(Msg_Request_Other_Crown_Points));
		return;
	}

	Msg_Request_Other_Crown_Points *pRequestData = (Msg_Request_Other_Crown_Points *)data;

	IShareReceiver * pShareReceiver = gServerGlobal->getShareReceiver();// 在共享服取一下信息
	if (pShareReceiver == NULL || pRequestData->dwFriendID == INVALID_PDBID)
	{
		ErrorLn(__FUNCTION__ << "pRequestPDBID == INVALID_PDBID or failed pShareReceiver == NULL");
		return;
	}
	SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pRequestData->dwFriendID);

	// 如果请求查看玩家在线,直接从服务器内存读取,不在线,若缓存中有，则取缓存中数据，否则从数据库读取
	if (shareProperty.dwPDBID != pRequestData->dwFriendID)
	{
		unordered_map<PDBID, SOtherPlayerCrownpages>::iterator iter = m_mapOtherPlayerCrownPages.find(pRequestData->dwFriendID);
		if (iter == m_mapOtherPlayerCrownPages.end())
		{
			DBREQ_PARAM_QUERY_CROWN_INFO pData;
			pData.dwActorID = pRequestData->dwFriendID;// 请求别人的符文数据到自己part中

			obuf oBuff;
			oBuff.push_back(&pData, sizeof(DBREQ_PARAM_QUERY_CROWN_INFO));

			if (pDBEngine->executeSP(GAMEDB_REQUEST_QUERY_CROWN_INFO, clientID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
			{
				ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_QUERY_CROWN_INFO failed");
				return;
			}

			if (pDBEngine->executeSP(GAMEDB_REQUEST_QUERY_CROWN_GMESTONEITEM, clientID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
			{
				ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_QUERY_CROWN_INFO failed");
				return;
			}
		}
		else
		{
			obuf2048 obData;
			obData.push_back(iter->second.crownPageList, sizeof(iter->second.crownPageList));
			obData.push_back(iter->second.crownGemstoneList, sizeof(iter->second.crownGemstoneList));

			sendOtherPlayerCrownPagesToClient(clientID, obData.data(), obData.size());
		}
	}
	else
	{
		ActorHelper actorHelper(pRequestData->dwFriendID, ACTOR_DBID);
		IActorService *pActorService = actorHelper.m_ptr;
		if (pActorService == NULL)
		{
			ErrorLn(__FUNCTION__ << " : failed pActorService == NULL");
			return;
		}

		string mapCrownData = pActorService->getOtherCrownPageData();
		const char * head = mapCrownData.c_str();
		if (head == NULL)
		{
			ErrorLn(__FUNCTION__ << " : failed pActorService->getOtherCrownData()");
			return;
		}

		string mapGemstoneData = pActorService->getOtherCrownGemstoneData();
		const char * headEx = mapGemstoneData.c_str();
		if (headEx == NULL)
		{
			ErrorLn(__FUNCTION__ << " : failed pActorService->getOtherCrownData()");
			return;
		}

		if (m_mapOtherPlayerCrownPages.find(pRequestData->dwFriendID) != m_mapOtherPlayerCrownPages.end())
		{
			m_mapOtherPlayerCrownPages.erase(pRequestData->dwFriendID);
		}

		obuf2048 obData;
		CROWN_PAGE_ITEM  crownPageList[CROWN_PAGE_MAX_COUNT];

		memcpy(crownPageList, head, sizeof(crownPageList));
		obData.push_back(crownPageList, sizeof(crownPageList));

		GEMSTONE_ITEM crownGemstoneList[CROWN_GEMSTONE_MAX_COUNT];
		memcpy(crownGemstoneList, headEx, sizeof(crownGemstoneList));
		obData.push_back(crownGemstoneList, sizeof(crownGemstoneList));

		sendOtherPlayerCrownPagesToClient(clientID, obData.data(), obData.size());
	}
}

void StaticWarScenService::sendOtherPlayerCrownPagesToClient(DWORD clientID, void *pOutData, int nOutLen)
{
	obuf2048 ob;
	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byKeyModule = MSG_MODULEID_STATICWAR;
	head.byKeyAction = SC_MSG_WAR_OTHER_CROWN_PAGE_INLAY;
	ob << head;
	ob.push_back(pOutData, nOutLen);
	gServerGlobal->getGatewayAcceptorService()->sendData(clientID, PACKAGE_PTR(new string((const char *)ob.data(), ob.size())));
}

void StaticWarScenService::onServerListUpdated()
{

}

void StaticWarScenService::onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{

}

// IDBRetSink
// 数据库请求返回回调方法
void StaticWarScenService::OnReturn(IDBRetSink *pRealDBRetSink, int nCmdID, int nDBRetCode, char *pszDBRetDesc, int nQueueIndex, char *pOutData, int nOutLen)
{
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		WAR_ERRORLN(__FUNCTION__ << " nCmdID=" << nCmdID << ",nDBRetCode=" << nDBRetCode << ",Desc=" << pszDBRetDesc);
		return;
	}

	switch (nCmdID)
	{
	case GAMEDB_REQUEST_WAR_GET_ACTORGAME:
		{
			onDBReturnWarGetActorGame(nQueueIndex, pOutData, nOutLen);
		}
		break;

	case GAMEDB_REQUEST_WAR_GET_WARGAME:
		{
			onDBReturnGetWarGame(nQueueIndex, pOutData, nOutLen);
		}
		break;

	case GAMEDB_REQUEST_QUERYACTORLOGIN:
		{
			OnDBReturnPlayerInfo(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	case GAMEDB_REQUEST_QUERYALLHERO:
		{
			OnDBReturnHeroInfo(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	case GAMEDB_REQUEST_GET_MATCHTYPE_ALL_RANK:         // 获取Rank分值
		{
			OnDBReturnMatchTypeRank(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	case GAMEDB_REQUEST_QUERY_HISTORY_RECORD:
		{
			OnDBReturnHistoryRecord(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
		
	case GAMEDB_REQUEST_FAKE_ACTORLOGIN_LIST:         // 获取假人列表
        {
            OnDBReturnFakeActorList(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
        }
        break;
	case GAMEDB_REQUEST_QUERY_CROWN_INFO:
		{
			onReturnReadOtherPlayerCrownPages(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	case GAMEDB_REQUEST_QUERY_CROWN_GMESTONEITEM:
		{
			onDBReturnOtherPlayerAllGemstoneInfo(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	default:
		break;
	}
}

void StaticWarScenService::onReturnReadOtherPlayerCrownPages(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	ClientID clientID = (DWORD)nQueueIndex;
	if (clientID == INVALID_CLIENT)
		return;

	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__ << " nCmdID=" << nCmdID << ",nDBRetCode=" << nDBRetCode << ",Desc=" << pszDBRetDesc);
		return;
	}

	if (nOutLen < sizeof(DBREQ_RESULT_QUERY_CROWN_PAGE))
	{
		ErrorLn(__FUNCTION__":DataLengthFail! DataLength =" << nOutLen << "< DBREQ_RESULT_QUERY_CROWN_PAGE Size= " << sizeof(DBREQ_RESULT_QUERY_CROWN_PAGE));
		return;
	}

	DBREQ_RESULT_QUERY_CROWN_PAGE * pData = (DBREQ_RESULT_QUERY_CROWN_PAGE *)(pOutData);
	if (nOutLen != sizeof(DBREQ_RESULT_QUERY_CROWN_PAGE) + pData->nPagecount * sizeof(DBREQ_PARAM_CROWN_PAGE_ITEM))
	{
		ErrorLn(__FUNCTION__":Data Length is invalid! nOutLen=" << nOutLen << ", Len=" << (sizeof(DBREQ_RESULT_QUERY_CROWN_PAGE) + pData->nPagecount * sizeof(DBREQ_PARAM_CROWN_PAGE_ITEM)));
		return;
	}

	int nPageNum = pData->nPagecount;
	if (nPageNum < 0)
	{
		ErrorLn(__FUNCTION__": nPageNum is invalid = " << nPageNum);
		return;
	}
	if (nPageNum > CROWN_PAGE_MAX_COUNT)
	{
		ErrorLn(__FUNCTION__": Gemstone Page Count is overflow ! nPageNum=" << nPageNum);
		nPageNum = CROWN_PAGE_MAX_COUNT;
	}

	// 皇冠页详细信息
	unordered_map<PDBID, SOtherPlayerCrownpages>::iterator iter = m_mapOtherPlayerCrownPages.find(pData->dwActorID);
	if (iter == m_mapOtherPlayerCrownPages.end())
	{
		m_mapOtherPlayerCrownPages[pData->dwActorID] = SOtherPlayerCrownpages();
		iter = m_mapOtherPlayerCrownPages.find(pData->dwActorID);
	}

	memset(iter->second.crownPageList, 0, sizeof(iter->second.crownPageList));

	DBREQ_PARAM_CROWN_PAGE_ITEM *pPage = (DBREQ_PARAM_CROWN_PAGE_ITEM*)(pData + 1);
	for (int i = 0; i < nPageNum; ++i)
	{
		iter->second.crownPageList[i] = *pPage;
		++pPage;
	}

	iter->second.loadState |= EOTHER_PLAYER_CROWN_PAGE;
	if (iter->second.loadState | EOTHER_PLAYER_CROWN_PAGE && iter->second.loadState | EOTHER_PLAYER_CROWN_GEMSTONE)
	{
		iter->second.loadState = EOTHER_PLAYER_CROWN_INIT;
		obuf2048 ob;
		ob.push_back(iter->second.crownPageList, sizeof(iter->second.crownPageList));
		ob.push_back(iter->second.crownGemstoneList, sizeof(iter->second.crownGemstoneList));
		sendOtherPlayerCrownPagesToClient(clientID, ob.data(), ob.size());
	}
}

void StaticWarScenService::onDBReturnOtherPlayerAllGemstoneInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	ClientID clientID = (DWORD)nQueueIndex;
	if (clientID == INVALID_CLIENT)
		return;

	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__ << " nCmdID=" << nCmdID << ",nQueueIndex=" << nQueueIndex << ",nDBRetCode=" << nDBRetCode << ",Desc=" << pszDBRetDesc);
		return;
	}

	if (nOutLen < sizeof(DBREQ_RESULT_ALLGEMSTONE_INFO))
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength =" << nOutLen << "< DBREQ_RESULT_ALLGEMSTONE_INFO Size= " << sizeof(DBREQ_RESULT_ALLGEMSTONE_INFO));
		return;
	}

	DBREQ_RESULT_ALLGEMSTONE_INFO * pData = (DBREQ_RESULT_ALLGEMSTONE_INFO *)(pOutData);
	
	// 核晶信息
	unordered_map<PDBID, SOtherPlayerCrownpages>::iterator iter = m_mapOtherPlayerCrownPages.find(pData->dwActorID);
	if (iter == m_mapOtherPlayerCrownPages.end())
	{
		m_mapOtherPlayerCrownPages[pData->dwActorID] = SOtherPlayerCrownpages();
		iter = m_mapOtherPlayerCrownPages.find(pData->dwActorID);
	}

	memset(iter->second.crownGemstoneList, 0, sizeof(iter->second.crownGemstoneList));

	int nGemstoneNum = pData->nGemstoneCount;
	if (nGemstoneNum <= 0)
	{
		if (nGemstoneNum < 0)
		{
			ErrorLn(__FUNCTION__": nGemstoneNum is invalid = " << nGemstoneNum);
		}
		return;
	}

	if (nGemstoneNum > CROWN_GEMSTONE_MAX_COUNT)
	{
		ErrorLn(__FUNCTION__": GemstonePageNum is invalid = " << nGemstoneNum);
		nGemstoneNum = CROWN_GEMSTONE_MAX_COUNT;
	}

	GEMSTONE_ITEM *pGemstoneInfo = (GEMSTONE_ITEM*)(pData + 1);
	for (int i = 0; i < nGemstoneNum; ++i, pGemstoneInfo++)
	{
		iter->second.crownGemstoneList[i] = *pGemstoneInfo;
	}

	iter->second.loadState |= EOTHER_PLAYER_CROWN_GEMSTONE;
	if (iter->second.loadState | EOTHER_PLAYER_CROWN_PAGE && iter->second.loadState | EOTHER_PLAYER_CROWN_GEMSTONE)
	{
		iter->second.loadState = EOTHER_PLAYER_CROWN_INIT;
		obuf2048 ob;
		ob.push_back(iter->second.crownPageList, sizeof(iter->second.crownPageList));
		ob.push_back(iter->second.crownGemstoneList, sizeof(iter->second.crownGemstoneList));
		sendOtherPlayerCrownPagesToClient(clientID, ob.data(), ob.size());
	}
}

// DB请求角色战绩返回
void StaticWarScenService::onDBReturnWarGetActorGame(int nQueueIndex, void *pOutData, int nOutLen)
{
	ClientID clientID = (DWORD)nQueueIndex;
	if (clientID == INVALID_CLIENT)
		return;

	// 校验返回数据
	if (pOutData == NULL || nOutLen < sizeof(DBREQ_RESULT_WAR_GET_ACTORGAME))
	{
		WAR_ERRORLN(__FUNCTION__": pData ==NULL or length is invalid, nOutLen=" << nOutLen << ", sizeof=" << sizeof(DBREQ_RESULT_WAR_GET_ACTORGAME));
		return;
	}

	DBREQ_RESULT_WAR_GET_ACTORGAME *pData = (DBREQ_RESULT_WAR_GET_ACTORGAME*)pOutData;
	if (nOutLen != sizeof(*pData) + pData->dwCount * sizeof(DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO))
	{
		WAR_ERRORLN(__FUNCTION__": pData length error !");
		return;
	}

	unordered_map<PDBID, SActorWarStatisticCache*>::iterator iter = m_actorCache.find(pData->dwActorID);
	if (iter == m_actorCache.end())
	{
		WAR_ERRORLN(__FUNCTION__": can't find actorid in cache! actorid=" << pData->dwActorID);
		return;
	}

	SActorWarStatisticCache *info = iter->second;
	info->dwRecentKillCount = pData->dwRecentKillCount;
	info->dwRecentDieCount = pData->dwRecentDieCount;
	info->dwRecentAssistCount = pData->dwRecentAssistCount;
	info->wRecentWinCount = pData->wRecentWinCount;
	info->wRecentLoseCount = pData->wRecentLoseCount;
	info->dwRecentScore = pData->dwRecentScore;
    info->loadState = 0;
    info->dwLatestRequest = time(NULL);
	DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO *pWarInfo = (DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO*)(pData + 1);
	info->warInfoList.clear();
	for (int i = 0; i < (int)pData->dwCount; ++i)
	{
		info->warInfoList.push_back(*pWarInfo);
		++pWarInfo;
	}

	if (++info->loadState == 1)
	{
		// 载入完成，发给客户端
		sendActorWarStatisticToClient(clientID, info);
	}
}

// DB请求单个战绩返回
void StaticWarScenService::onDBReturnGetWarGame(int nQueueIndex, void *pOutData, int nOutLen)
{
	ClientID clientID = (DWORD)nQueueIndex;
	if (clientID == INVALID_CLIENT)
		return;

	// 校验返回数据
	if (pOutData == NULL || nOutLen < sizeof(DBREQ_RESULT_WAR_GET_WARGAME))
	{
		WAR_ERRORLN(__FUNCTION__": pData ==NULL or length is invalid, nOutLen=" << nOutLen << ", sizeof=" << sizeof(DBREQ_RESULT_WAR_GET_WARGAME));
		return;
	}

	DBREQ_RESULT_WAR_GET_WARGAME *pData = (DBREQ_RESULT_WAR_GET_WARGAME*)pOutData;
	if (nOutLen != sizeof(*pData) + pData->byActorCount * sizeof(DBREQ_PARAM_WAR_ACTOR_INFO))
	{
		WAR_ERRORLN(__FUNCTION__": pData length error !");
		return;
	}

	unordered_map<LONGLONG, SSingleWarStatisticCache*>::iterator iter = m_warCache.find(pData->nWarID);
	if (iter == m_warCache.end())
	{
		WAR_ERRORLN(__FUNCTION__": can't find warid in cache! actorid=" << pData->nWarID);
		return;
	}

	SSingleWarStatisticCache *info = iter->second;
	info->nWarID = pData->nWarID;
	info->dwBeginTime = pData->dwBeginTime;
	info->dwEndTime = pData->dwEndTime;
	info->wWarType = pData->wWarType;
	info->wEndBecause = pData->byEndBecause;
	memcpy_s(info->wBanHeroID1, sizeof(info->wBanHeroID1), pData->wBanHeroID1, sizeof(pData->wBanHeroID1));
	memcpy_s(info->wBanHeroID2, sizeof(info->wBanHeroID2), pData->wBanHeroID2, sizeof(pData->wBanHeroID2));
	DBREQ_PARAM_WAR_ACTOR_INFO *pActorInfo = (DBREQ_PARAM_WAR_ACTOR_INFO*)(pData + 1);
	info->actorinfo.clear();
	for (int i = 0; i < (int)pData->byActorCount; ++i)
	{
		sstrcpyn(pActorInfo->szActorName, pActorInfo->szActorName, sizeof(pActorInfo->szActorName));
		info->actorinfo.push_back(*pActorInfo);
		++pActorInfo;
	}

	if (++info->loadState == 1)
	{
		sendWarStatisticToClient(clientID, info, pData->byWnd);
	}
}

// 发送角色战绩总览到客户端
void StaticWarScenService::sendActorWarStatisticToClient(ClientID clientID, SActorWarStatisticCache *cache)
{
	obuf4096 ob;

	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byKeyModule    = MSG_MODULEID_STATICWAR;
	head.byKeyAction    = SC_MSG_WAR_SEND_WARSTATICS_TOTAL;
	ob << head;

	msg_war_send_warstatics_total data;
	data.nActorID = cache->nActorID;
	data.dwRecentKillCount = cache->dwRecentKillCount;
	data.dwRecentDieCount = cache->dwRecentDieCount;
	data.dwRecentAssistCount = cache->dwRecentAssistCount;
	data.wRecentKDAScore = (WORD)calcKDAScore(data.dwRecentKillCount, data.dwRecentDieCount, data.dwRecentAssistCount);
	data.wRecentWinCount = cache->wRecentWinCount;
	data.wRecentLoseCount = cache->wRecentLoseCount;
	data.dwRecentScore = cache->dwRecentScore;

	data.wWarInfoCount = (WORD)cache->warInfoList.size();
	ob.push_back(&data, sizeof(data));

	if (data.wWarInfoCount > 0)
	{
		deque<DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO>::iterator iterWar = cache->warInfoList.begin();
		deque<DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO>::iterator iterWarEnd = cache->warInfoList.end();
		for ( ; iterWar != iterWarEnd; ++iterWar)
		{
			ob.push_back(&(*iterWar), sizeof(DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO));
		}
	}

	UID uid = gServerGlobal->getActorMgr()->ClientID2UID(clientID);
	send2Client(uid, PACKAGE_PTR(new std::string(ob.data(), ob.size())));
}

// 发送单场战绩到客户端
void StaticWarScenService::sendWarStatisticToClient(ClientID clientID, SSingleWarStatisticCache *cache, BYTE byWnd)
{
	obuf2048 ob;

	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byKeyModule    = MSG_MODULEID_STATICWAR;
	head.byKeyAction    = SC_MSG_WAR_SEND_WARSTATICS_WAR;
	ob << head;

	msg_war_send_warstatics_war data;
	data.nWarID = cache->nWarID;
	data.dwBeginTime = cache->dwBeginTime;
	data.dwEndTime = cache->dwEndTime;
	data.wWarType = cache->wWarType;
	data.wEndBecause = cache->wEndBecause;
	data.dwActorCount = (DWORD)cache->actorinfo.size();
    data.byWnd      = byWnd;
	memcpy_s(data.wBanHeroID1, sizeof(data.wBanHeroID1), cache->wBanHeroID1, sizeof(cache->wBanHeroID1));
	memcpy_s(data.wBanHeroID2, sizeof(data.wBanHeroID2), cache->wBanHeroID2, sizeof(cache->wBanHeroID2));
	ob.push_back(&data, sizeof(data));

	if (data.dwActorCount > 0)
	{
		vector<DBREQ_PARAM_WAR_ACTOR_INFO>::iterator iterActor = cache->actorinfo.begin();
		vector<DBREQ_PARAM_WAR_ACTOR_INFO>::iterator iterActorEnd = cache->actorinfo.end();
		for ( ; iterActor != iterActorEnd; ++iterActor)
		{
			ob.push_back(&(*iterActor), sizeof(DBREQ_PARAM_WAR_ACTOR_INFO));
		}
	}

	UID uid = gServerGlobal->getActorMgr()->ClientID2UID(clientID);
	send2Client(uid, PACKAGE_PTR(new std::string(ob.data(), ob.size())));
}

// 计算KDA分数
int StaticWarScenService::calcKDAScore(int nKillNum, int nDieNum, int nAssistNum)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		WAR_ERRORLN(__FUNCTION__ << ":get SchemeCenter failed!");
		return 0;
	}

	ISchemeWarEvaluate* pSchemeEvaluate = pSchemeCenter->getSchemeWarEvaluate();
	if (pSchemeEvaluate == NULL)
	{
		WAR_ERRORLN(__FUNCTION__<< ": get pSchemeEvaluate failed!");
		return 0;
	}
    
    // 30天KDA 按默认类型为1来获取
	SWarEvaluathInfo* pEvaluathInfo = pSchemeEvaluate->getEvaluathInfo();
	if (pEvaluathInfo == NULL)
	{
		WAR_ERRORLN(__FUNCTION__ << ": get pEvaluathInfo  failed");
		return 0;
	}

	int nDeadNum = max(nDieNum,1);
	int nKdaScore = (int)(((nKillNum+ nAssistNum)/nDeadNum) * pEvaluathInfo->fKDAWR);

	return nKdaScore;
}

// 请求其他玩家基本数据
void StaticWarScenService::reqOtherPlayerInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake)
{
	if (dwSelfID == INVALID_PDBID || dwReqID == INVALID_PDBID)
	{
		ErrorLn(__FUNCTION__"dwSelfID == INVALID_PDBID || dwReqID == INVALID_PDBID");
		return;
	}

    if (bFake)
    {
        // 取假人数据
        reqFakePlayerInfo(dwSelfID, dwReqID);
    }
    else
    {
        // 真人玩家数据
        reqTruePlayerInfo(dwSelfID, dwReqID);
    }
}


void StaticWarScenService::reqFakePlayerInfo(DWORD dwSelfID,DWORD dwReqID)
{
    // 假人对应的相关信息
    map<PDBID, SFakePlayerInfo >::iterator iter = m_mapFakePlayerInfo.find(dwReqID);
    if (iter == m_mapFakePlayerInfo.end())
    {
        // 随机组合假人数据
        SFakePlayerInfo pFakePlayerInfo;
        m_mapDBFakePlayInfoCache.size();
        if (m_mapDBFakePlayInfoCache.size() <= 0)
        {
            ErrorLn(__FUNCTION__": m_mapDBFakePlayInfoCache.size() <= 0");
            return;
        }
        // 随机玩家信息
        int index = (mt_range_rand(1,time(NULL))) % m_mapDBFakePlayInfoCache.size();
        vector< msg_entity_player_info_return*>::iterator iterPlayer = m_mapDBFakePlayInfoCache.begin() + index;

        m_mapDBFakeHeroCache.size();
        if (m_mapDBFakeHeroCache.size() <= 0)
        {
            ErrorLn(__FUNCTION__": m_mapDBFakeHeroCache.size() <= 0");
            return;
        }

        // 随机英雄信息
        index = (mt_range_rand(1,time(NULL))) % m_mapDBFakeHeroCache.size();
        vector< vector<SHeroInfo>>::iterator iterHero = m_mapDBFakeHeroCache.begin() + index;

        pFakePlayerInfo.pPlayerInfo = *iterPlayer;
        pFakePlayerInfo.pPlayerInfo->bFake = true;
        pFakePlayerInfo.pPlayerInfo->pDbid = dwReqID;
        pFakePlayerInfo.pPlayerInfo->nCount = (*iterHero).size();
        pFakePlayerInfo.pHeroInfo = &(*iterHero);

        pair<map<PDBID, SFakePlayerInfo >::iterator, bool> ret = m_mapFakePlayerInfo.insert(pair<PDBID, SFakePlayerInfo>(dwReqID, pFakePlayerInfo));
        if (ret.second)
        {
            iter = ret.first;
        }
        else
        {
            WarningLn(__FUNCTION__": cant insert SFakePlayerInfo PDBID = "<<dwReqID);
            return;
        }
        
    }
    
    //发送数据到客户端请求数据
    msg_entity_player_info_return playerInfo = *(iter->second.pPlayerInfo);
    obuf tmpData;
    int nCount = getFakeStrongHeroInfobBySort(*(iter->second.pHeroInfo), tmpData);
    playerInfo.nCount = nCount;
    obuf obData;
    obData.push_back(&playerInfo, sizeof(playerInfo));
    obData.push_back(tmpData.data(), tmpData.size());
    SendPlayerInfoToClient(dwSelfID, SC_MSG_ENTITY_RETURN_PLAYER_INFO, obData.data(), obData.size());
}


void StaticWarScenService::reqTruePlayerInfo(DWORD dwSelfID,DWORD dwReqID)
{
	ActorHelper actorHelper(dwReqID, ACTOR_DBID);
	IActorService * pActorService = actorHelper.m_ptr;
	// 其他玩家，在线，取服务器内存
	if (pActorService != NULL)
    {
        msg_entity_player_info_return playerInfo;
        SPlayerInfo data = pActorService->getPlayerInfo();
        fillPlayerInfo(playerInfo, data);
        playerInfo.nRankScore = pActorService->getMatchTypeRank(MatchType_Rank);

        string strongHeroInfo = pActorService->getOtherStrongHeroInfo();
		if(strongHeroInfo.empty())
        {
            ErrorLn(__FUNCTION__ << " : strongHeroInfo is empty");
            return;
        }
		const char * head = strongHeroInfo.c_str();
        int * nCount = (int*) head;
        playerInfo.nCount = *nCount;

        obuf obData;
        obData.push_back(&playerInfo, sizeof(msg_entity_player_info_return));
        obData.push_back(head + sizeof(int), strongHeroInfo.length() - sizeof(int));
        SendPlayerInfoToClient(dwSelfID, SC_MSG_ENTITY_RETURN_PLAYER_INFO, obData.data(), obData.size());
    }
    // 其他玩家，不在线，取数据库
    else
    {
		IDBEngineService * pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
		if (pDBEngine == NULL)
		{
			ErrorLn(__FUNCTION__ << " : failed pDBEngine == NULL");
			return;
		}
        unordered_map<PDBID, msg_entity_other_actor_info_all*>::iterator iter = m_mapOtherPlayInfoCache.find(dwReqID);
        // 已请求Player,已请求Hero,无请求
        if (iter != m_mapOtherPlayInfoCache.end())
        {
            if (iter->second->queryType == REQ_PLAYER_INFO)	// 已请求Player
            {
                if (iter->second->loadState == nPlayerInfoDBRequestCount)
                {
                    // 取缓存数据
                    msg_entity_player_info_return playerInfo = iter->second->playerInfo;
                    obuf tmpData;
                    int nCount = getStrongHeroInfobBySort(iter->second->playerInfo.pDbid, tmpData);
                    playerInfo.nCount = nCount;
                    obuf obData;
                    obData.push_back(&playerInfo, sizeof(playerInfo));
                    obData.push_back(tmpData.data(), tmpData.size());
                    SendPlayerInfoToClient(dwSelfID, SC_MSG_ENTITY_RETURN_PLAYER_INFO, obData.data(), obData.size());
                }
                else
                {
                    // 请求数据库等返回中，不处理
                }
            }
            else	// 英雄界面已请求Hero,则只需请求PlayerInfo,请求类型换为PlayerInfo
            {
                iter->second->queryType = REQ_PLAYER_INFO;

                DBREQ_PARAM_QUERYACTORLOGIN paramActor;
                paramActor.dwPDBID = dwReqID;
                obuf oBuffActor;
                oBuffActor.push_back(&paramActor, sizeof(paramActor));
                if (pDBEngine->executeSP(GAMEDB_REQUEST_QUERYACTORLOGIN, dwSelfID, oBuffActor.data(), oBuffActor.size(), static_cast<IDBRetSink *>(this)) == false)
                {
                    ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_QUERYACTORLOGIN failed");
                }

                //DBREQ_GET_MATCHTYPE_ALL_RANK paramRank;
                ////paramRank.dwUserID = pActorService->getUID();
                //paramRank.dwActorID = dwReqID;
                //obuf oBuffRank;
                //oBuffRank.push_back(&paramRank, sizeof(DBREQ_GET_MATCHTYPE_ALL_RANK));
                //if (pDBEngine->executeSP(GAMEDB_REQUEST_GET_MATCHTYPE_ALL_RANK, dwSelfID, oBuffRank.data(), oBuffRank.size(), static_cast<IDBRetSink *>(this)) == false)
                //{
                //    ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_GET_MATCHTYPE_ALL_RANK failed");
                //}
            }
        }
        else	// 无请求
        {
            msg_entity_other_actor_info_all* newCache = new msg_entity_other_actor_info_all();
            newCache->queryType = REQ_PLAYER_INFO;
            m_mapOtherPlayInfoCache[dwReqID] = newCache;

            DBREQ_PARAM_QUERYACTORLOGIN paramActor;
            paramActor.dwPDBID = dwReqID;
            obuf oBuffActor;
            oBuffActor.push_back(&paramActor, sizeof(paramActor));
            if (pDBEngine->executeSP(GAMEDB_REQUEST_QUERYACTORLOGIN, dwSelfID, oBuffActor.data(), oBuffActor.size(), static_cast<IDBRetSink *>(this)) == false)
            {
                ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_QUERYACTORLOGIN failed");
            }

            //DBREQ_GET_MATCHTYPE_ALL_RANK paramRank;
            ////paramRank.dwUserID = pActorService->getUID();
            //paramRank.dwActorID = dwReqID;
            //obuf oBuffRank;
            //oBuffRank.push_back(&paramRank, sizeof(DBREQ_GET_MATCHTYPE_ALL_RANK));
            //if (pDBEngine->executeSP(GAMEDB_REQUEST_GET_MATCHTYPE_ALL_RANK, dwSelfID, oBuffRank.data(), oBuffRank.size(), static_cast<IDBRetSink *>(this)) == false)
            //{
            //    ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_GET_MATCHTYPE_ALL_RANK failed");
            //}

            DBREQ_PARAM_QUERY_ALL_HERO paramHero;
            paramHero.dwActorID = dwReqID;
            obuf oBuffHero;
            oBuffHero.push_back(&paramHero, sizeof(paramHero));
            if (pDBEngine->executeSP(GAMEDB_REQUEST_QUERYALLHERO, dwSelfID, oBuffHero.data(), oBuffHero.size(), static_cast<IDBRetSink *>(this)) == false)
            {
                ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_QUERYALLHERO failed");
            }
        }
    }
}



// 请求其他玩家英雄数据
void StaticWarScenService::reqOtherPlayerHeroInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake)
{
	if (dwSelfID == INVALID_PDBID || dwReqID == INVALID_PDBID)
	{
		ErrorLn(__FUNCTION__"dwSelfID == INVALID_PDBID || dwReqID == INVALID_PDBID");
		return;
	}
    if (bFake)
    {
        // 取假人数据
        reqFakePlayerHeroInfo(dwSelfID, dwReqID);
    }
    else
    {
        // 真人玩家数据
        reqTruePlayerHeroInfo(dwSelfID, dwReqID);
    }
}


void StaticWarScenService::reqFakePlayerHeroInfo(DWORD dwSelfID,DWORD dwReqID)
{
    // 假人对应的相关信息
    map<PDBID, SFakePlayerInfo >::iterator iter = m_mapFakePlayerInfo.find(dwReqID);
    if (iter == m_mapFakePlayerInfo.end())
    {
        // 随机组合假人数据
        SFakePlayerInfo pFakePlayerInfo;
        m_mapDBFakePlayInfoCache.size();
        if (m_mapDBFakePlayInfoCache.size() <= 0)
        {
            ErrorLn(__FUNCTION__": m_mapDBFakePlayInfoCache.size() <= 0");
            return;
        }
        // 随机玩家信息
        int index = (mt_range_rand(1,time(NULL))) % m_mapDBFakePlayInfoCache.size();
        vector< msg_entity_player_info_return*>::iterator iterPlayer = m_mapDBFakePlayInfoCache.begin() + index;

        m_mapDBFakeHeroCache.size();
        if (m_mapDBFakeHeroCache.size() <= 0)
        {
            ErrorLn(__FUNCTION__": m_mapDBFakeHeroCache.size() <= 0");
            return;
        }

        // 随机英雄信息
        index = (mt_range_rand(1,time(NULL))) % m_mapDBFakeHeroCache.size();
        vector< vector<SHeroInfo>>::iterator iterHero = m_mapDBFakeHeroCache.begin() + index;

        pFakePlayerInfo.pPlayerInfo = *iterPlayer;
        pFakePlayerInfo.pPlayerInfo->bFake = true;
        pFakePlayerInfo.pPlayerInfo->pDbid = dwReqID;
        pFakePlayerInfo.pPlayerInfo->nCount = (*iterHero).size();
        pFakePlayerInfo.pHeroInfo = &(*iterHero);

        pair<map<PDBID, SFakePlayerInfo >::iterator, bool> ret = m_mapFakePlayerInfo.insert(pair<PDBID, SFakePlayerInfo>(dwReqID, pFakePlayerInfo));
        if (ret.second)
        {
            iter = ret.first;
        }
        else
        {
            WarningLn(__FUNCTION__": cant insert SFakePlayerInfo PDBID = "<<dwReqID);
            return;
        }
    }

    // 发送数据到客户端
    // 存在nHeroID为0的情况，所以这里加个判定，实际计算nCount
    int nCount = iter->second.pPlayerInfo->nCount;
    // 存在nHeroID为0的情况，所以这里加个判定，实际计算nCount
    int nTrueCount = 0;
    obuf obHeroBuf;
    msg_entity_hero_info msgheroInfo;
    vector<SHeroInfo> & heroInfoList = *iter->second.pHeroInfo;
    for (int i = 0; i < nCount; i++)
    {
        if (heroInfoList[i].nHeroID != 0)
        {
            fillHeroInfoMsg(msgheroInfo, heroInfoList[i]);
            obHeroBuf.push_back(&msgheroInfo, sizeof(msgheroInfo));
            nTrueCount++;
        }
    }

    obuf obData;
    obData << dwReqID << nCount;
    obData.push_back(obHeroBuf.data(), obHeroBuf.size());
    SendPlayerInfoToClient(dwSelfID, SC_MSG_ENTITY_RETURN_HERO_INFO, obData.data(), obData.size());

}

void StaticWarScenService::reqTruePlayerHeroInfo(DWORD dwSelfID,DWORD dwReqID)
{
	ActorHelper actorHelper(dwReqID, ACTOR_DBID);
	IActorService * pActorService = actorHelper.m_ptr;
	// 其他玩家，在线，取服务器内存
	if (pActorService != NULL)
    {
        string strData = pActorService->getHeroArray();
		if (strData.empty())
		{
			ErrorLn(__FUNCTION__ << " : pActorService->getHeroArray() is empty");
			return;
		}
        const char * pHeroData = strData.c_str();
        int * pCount = (int *)pHeroData;
        int nCount = *pCount;
        SHeroInfo *heroInfo = (SHeroInfo *)(pCount + 1);

        // 存在nHeroID为0的情况，所以这里加个判定，实际计算nCount
        int nTrueCount = 0;
        obuf obHeroBuf;
        msg_entity_hero_info msgheroInfo;
        for (int i = 0; i < nCount; ++i, ++heroInfo)
        {
            if (heroInfo->nHeroID != 0)
            {
                fillHeroInfoMsg(msgheroInfo, *heroInfo);
                obHeroBuf.push_back(&msgheroInfo, sizeof(msg_entity_hero_info));
                nTrueCount++;
            }
        }

        obuf obData;
        obData << dwReqID << nTrueCount;
        obData.push_back(obHeroBuf.data(), obHeroBuf.size());
        SendPlayerInfoToClient(dwSelfID, SC_MSG_ENTITY_RETURN_HERO_INFO, obData.data(), obData.size());
    }
    else  // 其他玩家，不在线，取数据库
    {
		IDBEngineService * pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
		if (pDBEngine == NULL)
		{
			ErrorLn(__FUNCTION__ << " : failed pDBEngine == NULL");
			return;
		}
        unordered_map<PDBID, msg_entity_other_actor_info_all*>::iterator iter = m_mapOtherPlayInfoCache.find(dwReqID);
        if (iter != m_mapOtherPlayInfoCache.end()) // PlayerInfo已请求
        {
            int nCount = iter->second->playerInfo.nCount;
            // 存在nHeroID为0的情况，所以这里加个判定，实际计算nCount
            int nTrueCount = 0;
            obuf obHeroBuf;
            msg_entity_hero_info msgheroInfo;
            vector<SHeroInfo> & heroInfoList = iter->second->heroInfoList;
            for (int i = 0; i < nCount; i++)
            {
                if (heroInfoList[i].nHeroID != 0)
                {
                    fillHeroInfoMsg(msgheroInfo, heroInfoList[i]);
                    obHeroBuf.push_back(&msgheroInfo, sizeof(msgheroInfo));
                    nTrueCount++;
                }
            }

            obuf obData;
            obData << dwReqID << nCount;
            obData.push_back(obHeroBuf.data(), obHeroBuf.size());
            SendPlayerInfoToClient(dwSelfID, SC_MSG_ENTITY_RETURN_HERO_INFO, obData.data(), obData.size());
        }
        else// 无请求
        {
            msg_entity_other_actor_info_all* newCache = new msg_entity_other_actor_info_all();
            newCache->queryType = REQ_HERO_INFO;
            m_mapOtherPlayInfoCache[dwReqID] = newCache;

            DBREQ_PARAM_QUERY_ALL_HERO heroParam;
            heroParam.dwActorID = dwReqID;
            obuf oHeroBuff;
            oHeroBuff.push_back(&heroParam, sizeof(heroParam));
            if (pDBEngine->executeSP(GAMEDB_REQUEST_QUERYALLHERO, dwSelfID, oHeroBuff.data(), oHeroBuff.size(), static_cast<IDBRetSink *>(this)) == false)
            {
                ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_QUERYALLHERO failed");
            }
        }
    }
}


void StaticWarScenService::fillFakePlayerInfo(msg_entity_player_info_return *playerInfo, DBREQ_RESULT_QUERYACTORLOGIN* pInfo)
{
    playerInfo->pDbid   = pInfo->dwActorID;
    playerInfo->nLv     = pInfo->nLevel;
    playerInfo->nHeadID = pInfo->wHeadID;
	playerInfo->nHeroID = pInfo->nHeroID;
	playerInfo->nSkinID = pInfo->nSkinID;
    playerInfo->nSupportNum = pInfo->dwSupportNum;

    sstrcpyn( playerInfo->chActorName, pInfo->szActorName, sizeof(playerInfo->chActorName));

    for	(int i = 0; i < ENACHIEVE_MAX; i++)
    {
        memcpy(&playerInfo->dwGlory[i], pInfo->byGlory + i*12, sizeof(DWORD));
        memcpy(&playerInfo->llWarID[i], pInfo->byGlory + i*12 + 4, sizeof(LONGLONG));
    }
    playerInfo->dwCurrentExp = pInfo->dwExp;
    playerInfo->dwFighting = pInfo->dwFightCapacity;
    playerInfo->dwPKWinNum = pInfo->dwPKWinNum;
    playerInfo->dwPKTotalNum = pInfo->dwPKTotalNum;
    
}

void StaticWarScenService::fillPlayerInfo(msg_entity_player_info_return & playerInfo, DBREQ_RESULT_QUERYACTORLOGIN* pInfo)
{
	playerInfo.pDbid   = pInfo->dwActorID;
	playerInfo.nLv     = pInfo->nLevel;
	playerInfo.nHeadID = pInfo->wHeadID;
	playerInfo.nHeroID = pInfo->nHeroID;
	playerInfo.nSkinID = pInfo->nSkinID;
	playerInfo.nSupportNum = pInfo->dwSupportNum;

	sstrcpyn( playerInfo.chActorName, pInfo->szActorName, sizeof(playerInfo.chActorName));

	// 战队信息
	int nKin = pInfo->dwKinID;  
	while(nKin > 0)
	{
		IKinScene *pKinScene = gServerGlobal->getKinScene();
		if (pKinScene == NULL)
			break;
		SERVICE_PTR pKinContainer = pKinScene->getKinSceneService();
		if (pKinContainer == NULL)
			break;
		IKinSceneService *pScene = (IKinSceneService*)pKinContainer ->query_interface();
		if (pScene == NULL)
			break;
		SKinInfo sKin = pScene->getKinInfo(nKin);
		if ( sKin.nKinProp_ID != 0)
		{
			sstrcpyn( playerInfo.chKinName, sKin.szName, sizeof(playerInfo.chKinName));
		}
		break;
	}

	// 帮会信息
	int nClan = pInfo->dwClanID;
	while(nClan > 0)
	{
		ClanHelper clanHelper;
		IClanSceneService * pClanService = clanHelper.m_ptr;
		if( pClanService != NULL )
		{
			sstrcpyn( playerInfo.chClanName, pClanService->getClanName(nClan).c_str(), sizeof(playerInfo.chClanName));
		}        
		break;
	}
	sstrcpyn( playerInfo.chClanPos, "", sizeof(playerInfo.chClanPos));


	for	(int i = 0; i < ENACHIEVE_MAX; i++)
	{
		memcpy(&playerInfo.dwGlory[i], pInfo->byGlory + i*12, sizeof(DWORD));
		memcpy(&playerInfo.llWarID[i], pInfo->byGlory + i*12 + 4, sizeof(LONGLONG));
	}
	playerInfo.dwCurrentExp = pInfo->dwExp;
	playerInfo.dwFighting = pInfo->dwFightCapacity;
	playerInfo.dwPKWinNum = pInfo->dwPKWinNum;
	playerInfo.dwPKTotalNum = pInfo->dwPKTotalNum;
    memcpy(&playerInfo.nRankGrade, pInfo->byRankbuffdata + sizeof(BYTE)+ sizeof(DWORD), sizeof(playerInfo.nRankGrade));
    memcpy(&playerInfo.nRankScore, pInfo->byRankbuffdata + sizeof(BYTE), sizeof(playerInfo.nRankScore));

	// dwGloryScore(荣耀积分)暂未赋值,如何获取？
	// nRankScore（排位得分）函数外另外获取
}

void StaticWarScenService::fillPlayerInfo(msg_entity_player_info_return & playerInfo, SPlayerInfo& info)
{
	playerInfo.pDbid = info.dwActorID;
	playerInfo.nLv = info.nLevel;
	playerInfo.nHeadID = info.wHeadID;
	playerInfo.nHeroID = info.nHeroID;
	playerInfo.nSkinID = info.nSkinID;
	playerInfo.nSupportNum = info.dwSupportNum;

	sstrcpyn(playerInfo.chActorName, info.szActorName, sizeof(playerInfo.chActorName));

	// 战队信息
	int nKin = info.dwKinID;
	while (nKin > 0)
	{
		IKinScene *pKinScene = gServerGlobal->getKinScene();
		if (pKinScene == NULL)
			break;
		SERVICE_PTR pKinContainer = pKinScene->getKinSceneService();
		if (pKinContainer == NULL)
			break;
		IKinSceneService *pScene = (IKinSceneService*)pKinContainer->query_interface();
		if (pScene == NULL)
			break;
		SKinInfo sKin = pScene->getKinInfo(nKin);
		if (sKin.nKinProp_ID != 0)
		{
			sstrcpyn(playerInfo.chKinName, sKin.szName, sizeof(playerInfo.chKinName));
		}
		break;
	}

	// 帮会信息
	int nClan = info.dwClanID;
	while (nClan > 0)
	{
		ClanHelper clanHelper;
		IClanSceneService * pClanService = clanHelper.m_ptr;
		if (pClanService != NULL)
		{
			sstrcpyn(playerInfo.chClanName, pClanService->getClanName(nClan).c_str(), sizeof(playerInfo.chClanName));
		}
		break;
	}
	sstrcpyn(playerInfo.chClanPos, "", sizeof(playerInfo.chClanPos));


	for (int i = 0; i < ENACHIEVE_MAX; i++)
	{
		memcpy(&playerInfo.dwGlory[i], info.byGlory + i * 12, sizeof(DWORD));
		memcpy(&playerInfo.llWarID[i], info.byGlory + i * 12 + 4, sizeof(LONGLONG));
	}
	playerInfo.dwCurrentExp = info.dwExp;
	playerInfo.dwFighting = info.dwFightCapacity;
	playerInfo.dwPKWinNum = info.dwPKWinNum;
	playerInfo.dwPKTotalNum = info.dwPKTotalNum;
    memcpy(&playerInfo.nRankGrade, info.byRankbuffdata + sizeof(BYTE)+ sizeof(DWORD), sizeof(playerInfo.nRankGrade));
    memcpy(&playerInfo.nRankScore, info.byRankbuffdata + sizeof(BYTE), sizeof(playerInfo.nRankScore));
	// dwGloryScore(荣耀积分)暂未赋值,如何获取？
	// nRankScore（排位得分）函数外另外获取
}

void StaticWarScenService::fillHeroInfoMsg(msg_entity_hero_info &sMsg, const SHeroInfo &sHeroInfo)
{
	memset(&sMsg, 0, sizeof(sMsg));
	DWORD nMatchNum = sHeroInfo.dwMatchNum;
	sMsg.dwExp = sHeroInfo.dwHeroExp;
	sMsg.dwGameCount = nMatchNum;
	sMsg.nHeroID = sHeroInfo.nHeroID;
	sMsg.nLv = sHeroInfo.nLevel;
	sMsg.byStarLv = sHeroInfo.nHeroStar;
	if ( nMatchNum == 0 )
	{
		sMsg.nWinPro = 0;
	}
	else
	{
		sMsg.nWinPro = (sHeroInfo.dwWinNum * 100) / nMatchNum;
	}
	sMsg.dwUseTime = sHeroInfo.dwUseTime;
	sMsg.bIsBuy = ((sHeroInfo.dwFlags & HERO_FLAG_NOT_HAVE) != HERO_FLAG_NOT_HAVE);
	sMsg.nUseXPSkillID = sHeroInfo.nUseXPID;
}

bool lessMark(const SHeroInfo& heroInfo1, const SHeroInfo& heroInfo2)
{
	return heroInfo1.dwMatchNum > heroInfo2.dwMatchNum;
}
int StaticWarScenService::getStrongHeroInfobBySort(int nPDBID, obuf &obData)
{
	int nCount = 0;
	unordered_map<PDBID, msg_entity_other_actor_info_all*>::iterator iter = m_mapOtherPlayInfoCache.find(nPDBID);
	if (iter != m_mapOtherPlayInfoCache.end())
	{
		std::vector<SHeroInfo> listHero;
		std::vector<SHeroInfo>::iterator itBegin = iter->second->heroInfoList.begin();
		std::vector<SHeroInfo>::iterator itEnd = iter->second->heroInfoList.end();
		for (; itBegin != itEnd; ++itBegin)
		{
			listHero.push_back(*itBegin);
		}
		std::sort(listHero.begin(), listHero.end(), lessMark);
		std::vector<SHeroInfo>::iterator iterHero = listHero.begin();

		for (; iterHero != listHero.end(); ++iterHero)
		{
			if ( nCount >= STRONGEST_HERO_COUNT)
			{
				break;
			}

			SHeroInfo sHeroInfo = *iterHero;
			int nMatchNum = sHeroInfo.dwMatchNum;
			msg_entity_hero_info heroInfo;
			heroInfo.dwExp = sHeroInfo.dwHeroExp;
			heroInfo.dwGameCount = nMatchNum;
			heroInfo.nHeroID = sHeroInfo.nHeroID;
			heroInfo.nLv = sHeroInfo.nLevel;
			heroInfo.byStarLv = sHeroInfo.nHeroStar;
			heroInfo.dwScore = sHeroInfo.nHeroScore;
			if ( nMatchNum == 0 )
			{
				heroInfo.nWinPro = 0;
			}else
			{
				heroInfo.nWinPro = (sHeroInfo.dwWinNum*100)/(sHeroInfo.dwMatchNum);
			}
			heroInfo.bIsBuy = ((iterHero->dwFlags & HERO_FLAG_NOT_HAVE) != HERO_FLAG_NOT_HAVE);
			obData.push_back( &heroInfo, sizeof(heroInfo) );
			++nCount;
		}
	}
	return nCount;
}


int StaticWarScenService::getFakeStrongHeroInfobBySort(vector<SHeroInfo> &vecHero, obuf &obData)
{
    int nCount = 0;
    if (vecHero.size() > 0)
    {
        std::vector<SHeroInfo> listHero;
        std::vector<SHeroInfo>::iterator itBegin = vecHero.begin();
        std::vector<SHeroInfo>::iterator itEnd = vecHero.end();
        for (; itBegin != itEnd; ++itBegin)
        {
            listHero.push_back(*itBegin);
        }
        std::sort(listHero.begin(), listHero.end(), lessMark);
        std::vector<SHeroInfo>::iterator iterHero = listHero.begin();

        for (; iterHero != listHero.end(); ++iterHero)
        {
            if ( nCount >= STRONGEST_HERO_COUNT)
            {
                break;
            }

            SHeroInfo sHeroInfo = *iterHero;
            int nMatchNum = sHeroInfo.dwMatchNum;
            msg_entity_hero_info heroInfo;
            heroInfo.dwExp = sHeroInfo.dwHeroExp;
            heroInfo.dwGameCount = nMatchNum;
            heroInfo.nHeroID = sHeroInfo.nHeroID;
            heroInfo.nLv = sHeroInfo.nLevel;
            heroInfo.byStarLv = sHeroInfo.nHeroStar;
            heroInfo.dwScore = sHeroInfo.nHeroScore;
            if ( nMatchNum == 0 )
            {
                heroInfo.nWinPro = 0;
            }else
            {
                heroInfo.nWinPro = (sHeroInfo.dwWinNum*100)/(sHeroInfo.dwMatchNum);
            }
            heroInfo.bIsBuy = ((iterHero->dwFlags & HERO_FLAG_NOT_HAVE) != HERO_FLAG_NOT_HAVE);
            obData.push_back( &heroInfo, sizeof(heroInfo) );
            ++nCount;
        }
    }
    return nCount;
}


void StaticWarScenService::SendPlayerInfoToClient(PDBID nPDBID, BYTE byAction, void *pData,int nLen)
{
	if (pData == NULL || nLen <= 0)
	{
		ErrorLn(__FUNCTION__"data is invalid, pData == NULL || nLen <= 0");
		return;
	}

	UID uid = gServerGlobal->getActorMgr()->PDBID2UID(nPDBID);

	// 通知客户端
	obuf256 obufData;

	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
	head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
	head.byKeyModule   = MSG_MODULEID_ENTITY;	
	head.byKeyAction   = byAction;
	obufData << head;

	SMsgEntityHead entityHead;
	entityHead.uidMaster = uid;	
	entityHead.byPartID = PART_BANK;		
	obufData << entityHead;

	obufData.push_back(pData,nLen);

	send2Client(uid, PACKAGE_PTR(new std::string(obufData.data(), obufData.size())));
}



void StaticWarScenService::OnDBReturnPlayerInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if ( nDBRetCode != DBRET_CODE_SUCCEED )
	{
		ErrorLn( __FUNCTION__<<" nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
		return;
	}
	PDBID dwSelfID = (PDBID)nQueueIndex;
	if (dwSelfID == INVALID_PDBID)
	{
		ErrorLn(__FUNCTION__ << " Invilad ActorID !!! ");
		return;
	}

	// 校验返回数据
	if(pOutData == NULL)
	{
		ErrorLn( __FUNCTION__  << "," << __LINE__<<":pData ==  NULL"); 
		return;
	}
	int  nEfficacyLen = sizeof(DBREQ_RESULT_QUERYACTORLOGIN);
	if(nOutLen < nEfficacyLen)
	{			
		ErrorLn( __FUNCTION__  << "," << __LINE__<<":DataLengthFail!  DataLength ="<<nOutLen<<"< DBREQ_RESULT_QUERYACTORLOGIN Size= "<<nEfficacyLen);
		return ;
	}
	DBREQ_RESULT_QUERYACTORLOGIN * select = (DBREQ_RESULT_QUERYACTORLOGIN *)pOutData;
	if(select == NULL)
	{
		ErrorLn( __FUNCTION__  << "," << __LINE__<<":select ==  NULL"); 
		return;
	}
#ifdef FUNCTION_RUNE_OPEN
	nEfficacyLen += sizeof(int);
	if ( nOutLen < nEfficacyLen)
	{
		ErrorLn( __FUNCTION__  << "," << __LINE__<<":DataLengthFail!  DataLength ="<<nOutLen<<"< RUNECOUNT+DBREQ_RESULT_QUERYACTORLOGIN Size= "<<nEfficacyLen);
		return ;
	}

	int nRuneStoreItemLen = sizeof(EntityHeroRuneStore);
	int nRuneStoreCount = (*(int*)(select+1))/nRuneStoreItemLen;
	nEfficacyLen += nRuneStoreCount * nRuneStoreItemLen;
	if ( nOutLen != nEfficacyLen)
	{
		ErrorLn( __FUNCTION__  << "," << __LINE__<<":DataLengthFail!  DataLength ="<<nOutLen<<"< DBREQ_RESULT_QUERYACTORLOGIN+RUNECOUNT+RUNE Size= "<<nEfficacyLen);
		return ;
	}
#endif

    if( select->bFake )
    {
        // 做假人缓存
        msg_entity_player_info_return* pFakeCache = new msg_entity_player_info_return();
        if (pFakeCache == NULL)
        {
            return;
        }

        fillFakePlayerInfo(pFakeCache, select);
        
        m_mapDBFakePlayInfoCache.push_back(pFakeCache);
    }
    else
    {
        unordered_map<PDBID, msg_entity_other_actor_info_all*>::iterator iter = m_mapOtherPlayInfoCache.find(select->dwActorID);
        if (iter == m_mapOtherPlayInfoCache.end())
        {
            ErrorLn(__FUNCTION__": can't find actorid in cache! actorid=" << select->dwActorID);
            return;
        }
        fillPlayerInfo(iter->second->playerInfo, select);

        // 请求PlayerInfo,并且已经load三次,返回数据
        if (++iter->second->loadState == nPlayerInfoDBRequestCount)
        {	
            // 载入完成，发给客户端
            msg_entity_player_info_return playerInfo = iter->second->playerInfo;

            obuf tmpData;
            int nCount = getStrongHeroInfobBySort(playerInfo.pDbid, tmpData);
            playerInfo.nCount = nCount;

            obuf obData;
            obData.push_back(&playerInfo, sizeof(playerInfo));
            obData.push_back(tmpData.data(), tmpData.size());
            SendPlayerInfoToClient(dwSelfID, SC_MSG_ENTITY_RETURN_PLAYER_INFO, obData.data(), obData.size());
        }
    }
}

void StaticWarScenService::OnDBReturnHeroInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if ( nDBRetCode != DBRET_CODE_SUCCEED )
	{
		ErrorLn( __FUNCTION__<<" nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
		return;
	}

	PDBID dwSelfID = (PDBID)nQueueIndex;
	if (dwSelfID == INVALID_PDBID)
	{
		ErrorLn(__FUNCTION__ << " Invilad ActorID !!! ");
		return;
	}

	// 校验返回数据
	if (pOutData == NULL || nOutLen < sizeof(DBREQ_RESULT_QUERY_ALL_HERO))
	{
		ErrorLn(__FUNCTION__": pData ==NULL or length is invalid, nOutLen=" << nOutLen << ", sizeof=" << sizeof(DBREQ_RESULT_QUERY_ALL_HERO));
		return;
	}
	DBREQ_RESULT_QUERY_ALL_HERO *pData = (DBREQ_RESULT_QUERY_ALL_HERO*)pOutData;
	if (nOutLen != sizeof(*pData) + pData->nHeroCount * sizeof(DBREQ_PARAM_HEROINFO))
	{
		ErrorLn(__FUNCTION__": pData length error !");
		return;
	}

    if( pData->bFake )
    {
        // 做假人缓存 
        vector<SHeroInfo> heroInfoList;				// 英雄战绩列表
        DBREQ_PARAM_HEROINFO *pHeroInfo = (DBREQ_PARAM_HEROINFO*)(pData + 1);
        for (int i = 0; i < pData->nHeroCount; ++i)
        {
            heroInfoList.push_back(*pHeroInfo);
            ++pHeroInfo;
        }
        
        m_mapDBFakeHeroCache.push_back(heroInfoList);
    }
    else
    {
        unordered_map<PDBID, msg_entity_other_actor_info_all*>::iterator iter = m_mapOtherPlayInfoCache.find(pData->dwActorID);
        if (iter == m_mapOtherPlayInfoCache.end())
        {
            ErrorLn(__FUNCTION__": can't find actorid in cache! actorid=" << pData->dwActorID);
            return;
        }

        msg_entity_other_actor_info_all * info = iter->second; 
        DBREQ_PARAM_HEROINFO *pHeroInfo = (DBREQ_PARAM_HEROINFO*)(pData + 1);
        for (int i = 0; i < pData->nHeroCount; ++i)
        {
            info->heroInfoList.push_back(*pHeroInfo);
            ++pHeroInfo;
        }
        iter->second->playerInfo.nCount = pData->nHeroCount;

        ++iter->second->loadState;
        // PlayerInfo请求返回，需load三次
        if (iter->second->queryType == REQ_PLAYER_INFO && iter->second->loadState == nPlayerInfoDBRequestCount)
        {	
            // 载入完成，发给客户端,取缓存数据
            msg_entity_player_info_return playerInfo = iter->second->playerInfo;

            obuf tmpData;
            int nCount = getStrongHeroInfobBySort(iter->second->playerInfo.pDbid, tmpData);
            playerInfo.nCount = nCount;

            obuf obData;
            obData.push_back(&playerInfo, sizeof(playerInfo));
            obData.push_back(tmpData.data(), tmpData.size());

            SendPlayerInfoToClient(dwSelfID, SC_MSG_ENTITY_RETURN_PLAYER_INFO, obData.data(), obData.size());
        }

        // HeroInfo请求返回，只需load一次  SendOtherHeroInfoToClient
        if (iter->second->queryType == REQ_HERO_INFO &&iter->second->loadState == nHeroInfoDBRequestCount)
        {
            int dwPDBID = pData->dwActorID;
            int nCount = pData->nHeroCount;
            vector<SHeroInfo> & heroInfoList = iter->second->heroInfoList;

            // 存在nHeroID为0的情况，所以这里加个判定，实际计算nCount
            int nTrueCount = 0;
            obuf obHeroBuf;
            msg_entity_hero_info heroInfo;
            for (int i = 0; i < nCount; i++)
            {
                if (heroInfoList[i].nHeroID != 0)
                {
                    fillHeroInfoMsg(heroInfo, heroInfoList[i]);
                    obHeroBuf.push_back(&heroInfo, sizeof(heroInfo));
                    nTrueCount++;
                }
            }

            obuf obData;
            obData << dwPDBID << nCount;
            obData.push_back(obHeroBuf.data(), obHeroBuf.size());
            SendPlayerInfoToClient(dwSelfID, SC_MSG_ENTITY_RETURN_HERO_INFO, obData.data(), obData.size());
        }
    }
}

void StaticWarScenService::OnDBReturnMatchTypeRank(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	//if ( nDBRetCode != DBRET_CODE_SUCCEED )
	//{
	//	ErrorLn( __FUNCTION__<<" nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
	//	return;
	//}

	//PDBID dwSelfID = (PDBID)nQueueIndex;
	//if (dwSelfID == INVALID_PDBID)
	//{
	//	ErrorLn(__FUNCTION__ << " Invilad ActorID !!! ");
	//	return;
	//}

	//// 校验返回数据
	//if ( pOutData == NULL || nOutLen < sizeof(DBREQ_RESULT_MATCHTYPE_ALL_RANK) )
	//{
	//	ErrorLn(__FUNCTION__": pData ==NULL or length is invalid, nOutLen="<<nOutLen <<", sizeof="<<sizeof(DBREQ_RESULT_MATCHTYPE_ALL_RANK));
	//	return;
	//}
	//DBREQ_RESULT_MATCHTYPE_ALL_RANK * pData = (DBREQ_RESULT_MATCHTYPE_ALL_RANK *)pOutData;
	//if ( nOutLen != sizeof(*pData) + pData->nReCount * sizeof(MatchTypeRankNode))
	//{
	//	ErrorLn(__FUNCTION__": pData length error !");
	//	return ;
	//}

	//unordered_map<PDBID, msg_entity_other_actor_info_all*>::iterator iter = m_mapOtherPlayInfoCache.find(pData->dwActorID);
	//if (iter == m_mapOtherPlayInfoCache.end())
	//{
	//	ErrorLn(__FUNCTION__": can't find actorid in cache! actorid=" << pData->dwActorID);
	//	return;
	//}

	//// 赋值nRankScore
	//MatchTypeRankNode* pOtherDataHead = (MatchTypeRankNode *)(pData +1);
	//for(int i = 0; i < pData->nReCount; ++i)
	//{
	//	if (pOtherDataHead->nMatchType == MatchType_Rank)
	//	{
	//		iter->second->playerInfo.nRankScore = pOtherDataHead->nRankScore;
	//		break;
	//	}

	//	pOtherDataHead++;
	//}

	//// 请求PlayerInfo,并且已经load三次,返回数据
	//if (++iter->second->loadState == nPlayerInfoDBRequestCount)
	//{	
	//	// 载入完成，发给客户端
	//	msg_entity_player_info_return playerInfo = iter->second->playerInfo;

	//	obuf tmpData;
	//	int nCount = getStrongHeroInfobBySort(playerInfo.pDbid, tmpData);
	//	playerInfo.nCount = nCount;

	//	obuf obData;
	//	obData.push_back(&playerInfo, sizeof(playerInfo));
	//	obData.push_back(tmpData.data(), tmpData.size());
	//	SendPlayerInfoToClient(dwSelfID, SC_MSG_ENTITY_RETURN_PLAYER_INFO, obData.data(), obData.size());
	//}
}

void StaticWarScenService::OnDBReturnHistoryRecord(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__ << ",nDBRetCode=" << nDBRetCode << ",Desc=" << pszDBRetDesc << ",nQueueIndex=" << nQueueIndex);
		return;
	}

	if (pOutData == NULL)
	{
		ErrorLn(__FUNCTION__": failed pOutData == NULL");
		return;
	}

	int nPDBID = nQueueIndex;
	if (nPDBID == INVALID_UID)
	{
		ErrorLn(__FUNCTION__ << " Invilad ActorID !!! ");
		return;
	}

	// 校验返回数据
	if (nOutLen < sizeof(int))
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength =" << nOutLen << "< sizeof(int) = " << sizeof(int));
		return;
	}

	int count = *(int*)pOutData;
	// 效验长度值
	int nEfficacyLen = sizeof(DBREQ_RESULT_QUERY_HISTORY_RECORD) * count + sizeof(int);
	if (nOutLen < nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength =" << nOutLen << "< sizeof(DBREQ_RESULT_QUERY_HISTORY_RECORD) * count + sizeof(int) Size= " << nEfficacyLen);
		return;
	}

	// 通知客户端
	obuf1024 obufData;

	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_STATICWAR;
	head.byKeyAction = SC_MSG_WAR_SEND_HISTORY_RECORD;
	obufData << head;

	obufData << count;
	DBREQ_RESULT_QUERY_HISTORY_RECORD* pHistoryRecord = (DBREQ_RESULT_QUERY_HISTORY_RECORD*)((char*)pOutData + sizeof(int));
	for (int i = 0; i < count; ++i, ++pHistoryRecord)
	{
		DBREQ_RESULT_QUERY_HISTORY_RECORD *p = pHistoryRecord;
		msg_war_send_history_record msg;
		msg.wWarType = p->wWarType;
		msg.bIsFull = p->bIsFull;
		msg.nHeroID = p->wHeroID;
		msg.wKillPersonNum = p->wKillPersonNum;
		msg.wAssistCount = p->wAssistCount;
		msg.wDieCount = p->wDieCount;
		obufData.push_back(&msg, sizeof(msg));
	}	

	send2Client(gServerGlobal->getActorMgr()->PDBID2UID(nPDBID), PACKAGE_PTR(new std::string(obufData.data(), obufData.size())));
}

void StaticWarScenService::OnDBReturnFakeActorList(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    IDBEngineService * pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__ << " : failed pDBEngine == NULL");
        return;
    }

    if ( nDBRetCode != DBRET_CODE_SUCCEED )
    {
        ErrorLn( __FUNCTION__<<" nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
        return;
    }

    // 校验返回数据
    if ( pOutData == NULL || nOutLen < sizeof(DBREQ_RESULT_FAKE_ACTORLOGIN_LIST) )
    {
        ErrorLn(__FUNCTION__": pData ==NULL or length is invalid, nOutLen="<<nOutLen <<", sizeof="<<sizeof(DBREQ_RESULT_FAKE_ACTORLOGIN_LIST));
        return;
    }
    DBREQ_RESULT_FAKE_ACTORLOGIN_LIST * pData = (DBREQ_RESULT_FAKE_ACTORLOGIN_LIST *)pOutData;
    if ( nOutLen != sizeof(*pData) + pData->dwCount * sizeof(DWORD))
    {
        ErrorLn(__FUNCTION__": pData length error !");
        return ;
    }

    // 开始读取假人数据
    DWORD *pActorID = (DWORD *)(pData + 1);
    for (int n = 0; n < pData->dwCount; ++n)
    {
        DBREQ_PARAM_QUERYACTORLOGIN paramActor;
        paramActor.dwPDBID = *pActorID;
        paramActor.bFake = true;
        obuf oBuffActor;
        oBuffActor.push_back(&paramActor, sizeof(paramActor));
        if (pDBEngine->executeSP(GAMEDB_REQUEST_QUERYACTORLOGIN, paramActor.dwPDBID, oBuffActor.data(), oBuffActor.size(), static_cast<IDBRetSink *>(this)) == false)
        {
            ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_QUERYACTORLOGIN failed dwPDBID="<<paramActor.dwPDBID<<" bFake = "<<paramActor.bFake);
        }

        DBREQ_PARAM_QUERY_ALL_HERO paramHero;
        paramHero.dwActorID = *pActorID;
        paramHero.bFake = true;
        obuf oBuffHero;
        oBuffHero.push_back(&paramHero, sizeof(paramHero));
        if (pDBEngine->executeSP(GAMEDB_REQUEST_QUERYALLHERO, paramActor.dwPDBID, oBuffHero.data(), oBuffHero.size(), static_cast<IDBRetSink *>(this)) == false)
        {
            ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_QUERYALLHERO failed dwPDBID="<<paramActor.dwPDBID<<" bFake = "<<paramActor.bFake);
        }

        pActorID++;
    }
    
}
