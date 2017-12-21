/*******************************************************************
** 文件名: CWarBase.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/1
** 版  本: 1.0
** 描  述: 战场基类-实现最基本的战场逻辑
** 应  用:
**************************** 修改记录 ******************************
** 修改人:  李有红
** 日  期:  2017/11/5
** 描  述:  该类只实现基本的战场逻辑，方便以后改新的玩法
********************************************************************/

#include "StdAfx.h"
#include "WarBase.h"
#include "GUID.h"
#include "PathFindHelper.h"
#include "IPathFindService.h"
#include "WarAddBuff.h"
#include "WarDrop.h"
#include "WarLogRecord.h"
#include "MonsterExec.h"
#include "MonsBaseExec.h"
#include "MonsSoliderExec.h"
#include "MonsTowerExec.h"
#include "DamageDef.h"
#include "ExternalHelper.h"
#include "MatchSchemeDef.h"
#include "ClanHelper.h"
#include "KinHelper.h"
#include "MatchSceneHelper.h"
#include <sstream>

CWarBase::CWarBase()
	: m_pWarService(NULL)
	, m_lMutex(EWP_Ready)
{
	memset(m_WarMgrList, 0, sizeof(m_WarMgrList));
}

CWarBase::~CWarBase()
{
}

bool CWarBase::isEnd()
{
	return m_WarInfo.bIsFinish;
}

EWarEndReason CWarBase::getWarEndReason()
{
	return m_WarInfo.EndWarReason;
}

void CWarBase::setWarEndReason(EWarEndReason eReason)
{
	m_WarInfo.EndWarReason = eReason;
}

void CWarBase::onEntityRelive(UID uid)
{
	SWPerson* pPerson = getPlayerInfo(uid);
	if (pPerson != NULL)
	{
		pPerson->lifeState = ELS_Alive;
	}
}

__IWarMgr * CWarBase::getWarMgr(int nWarMgrID)
{
	if (nWarMgrID < 0 || nWarMgrID >= Mgr_Max)
	{
		return NULL;
	}

	return m_WarMgrList[nWarMgrID];
}

int CWarBase::getWarSceneID()
{
	return m_WarInfo.nSceneID;
}

int CWarBase::getWarTypeId()
{
	return m_context.nConfigWarID;
}

bool CWarBase::Init()
{
	// 获得配置信息
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return false;
	}

	ISchemeEntity *pSchemeEntity = pSchemeCenter->getSchemeEntity();
	if (pSchemeEntity == NULL)
	{
		return false;
	}

	// 取得是否显示打印添加天赋经验信息
	m_WarInfo.bIsWarShowAddEXPAndMoney = pSchemeEntity->getEntityXml()->nWarShowAddEXPAndMoney == 0 ? false : true;

	// 战场自动添加Buff
	int nMatchType = m_context.dwMatchTypeID;
	//m_pWarAddBuff = new CWarAddBuff(this);
	//if (m_pWarAddBuff->Init(nMatchType) == false)
	//{
	//	ErrorLn(__FUNCTION__ " Init WarAddBuff failed.");
	//	m_pWarAddBuff->Release();
	//	m_pWarAddBuff = NULL;
	//	return false;
	//}

	//// 战场掉落
	//m_pWarDrop = new CWarDrop(this);
	//if (m_pWarDrop->Init(nMatchType) == false)
	//{
	//	ErrorLn(__FUNCTION__ " Init WarDrop failed.");
	//	m_pWarDrop->Release();
	//	m_pWarDrop = NULL;
	//	return false;
	//}

	//m_pWarLogRecord = new CWarLogRecord(this);
	//if (m_pWarLogRecord->Init(nMatchType) == false)
	//{
	//	ErrorLn(__FUNCTION__ " Init WarLogRecord failed.");
	//	m_pWarLogRecord->Release();
	//	m_pWarLogRecord = NULL;
	//	return false;
	//}

	if (InitSubFunc() == false)
	{
		ErrorLn("Init sub manager failed.");
		return false;
	}

	return true;
}

void CWarBase::UnInit()
{
	// 通知大厅结束
	MatchSceneHelper matchHelper;
	if (matchHelper.m_ptr != NULL)
	{
		matchHelper.m_ptr->warEnd(m_context.nWarID, getWarDBKey());
	}
}

void CWarBase::Start()
{
	if (m_WarInfo.bIsStart)
	{
		return;
	}

	m_WarInfo.BeginTime = (DWORD)time(NULL);
	m_WarInfo.bIsStart = true;

	TraceLn(__FUNCTION__ ": Start() nSceneID =" << m_WarInfo.nSceneID << ", nWarID = " << m_context.nWarID << ", dwRoomID = " << m_context.dwRoomID);

	// 生成唯一id
	m_WarInfo.dwDBWarID = createDBWarId(gServerGlobal->getServerID());

	TraceLn(__FUNCTION__"DBWarID=" << m_WarInfo.dwDBWarID);

	PathFindHelper helper(m_WarInfo.nSceneID);
	IPathFindService *pPathService = helper.m_ptr;
	if (pPathService != NULL)
	{
		if (!pPathService->startDynamicObstacleProxy())
		{
			ErrorLn("Start Dynamic Obstacle Service Failed !!!");
		}
	}
}

void CWarBase::End()
{
	TraceLn(__FUNCTION__":dbWarId=" << m_WarInfo.dwDBWarID << ", MatchType=" << m_context.dwMatchTypeID << " warid=" << m_context.nWarID);

}

DWORD CWarBase::getWarContinueTime()
{
	if (m_WarInfo.BeginTime == 0)
	{
		return 0;
	}

	return (DWORD)time(NULL) - m_WarInfo.BeginTime;
}

bool CWarBase::broadCastDataToAllPlayer(BYTE byKeyAction, LPCSTR pszData, int nDataLen, int nCamp, bool bToView)
{
	if (pszData == NULL || nDataLen == 0)
	{
		return false;
	}

	SERVICE_PTR pContainer = gServerGlobal->getSceneManager()->getScene(m_WarInfo.nSceneID);
	if (pContainer == NULL)
	{
		return false;
	}

	obuf o;
	if (bToView)
	{
		g_EHelper.fillDataToClientView(o, byKeyAction, pszData, nDataLen);
	}
	else
	{
		SNetMsgHead head;
		head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
		head.byKeyModule = MSG_MODULEID_WAR;
		head.byKeyAction = byKeyAction;

		o << head;
		o.push_back(pszData, nDataLen);
	}
	

	IGameSceneService * pScene = (IGameSceneService*)pContainer->query_interface();
	if (pScene != NULL)
	{
		if (nCamp == BCASTALLCAMP)
		{
			pScene->broadcast_all(BROADCAST_SCENE, std::string(o.data(), o.size()));
		}
		else /*if (nCamp >= CAMP_NEUTRAL && nCamp < CAMP_MAX)*/
		{
			pScene->broadcast_all(BROADCAST_SCENE, std::string(o.data(), o.size()), CAMP_MASK(nCamp) | CAMP_MASK(m_WarInfo.nTotalCampCount));
		}
	}
	return true;
}

bool CWarBase::broadCastDataToAllOtherCamp(BYTE byKeyAction, LPCSTR pszData, int nDataLen, int nCamp, bool bToView)
{
	if (pszData == NULL || nDataLen == 0)
	{
		return false;
	}

	SERVICE_PTR pContainer = gServerGlobal->getSceneManager()->getScene(m_WarInfo.nSceneID);
	if (pContainer == NULL)
	{
		return false;
	}

	obuf o;
	if (bToView)
	{
		g_EHelper.fillDataToClientView(o, byKeyAction, pszData, nDataLen);
	}
	else
	{
		SNetMsgHead head;
		head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
		head.byKeyModule = MSG_MODULEID_WAR;
		head.byKeyAction = byKeyAction;

		o << head;
		o.push_back(pszData, nDataLen);
	}

	IGameSceneService * pScene = (IGameSceneService*)pContainer->query_interface();
	if (pScene != NULL)
	{
		if (nCamp == BCASTALLCAMP)
		{
			pScene->broadcast_all(BROADCAST_SCENE, std::string(o.data(), o.size()));
		}
		else/* if (nCamp >= CAMP_NEUTRAL && nCamp < CAMP_MAX)*/
		{
			pScene->broadcast_all(BROADCAST_SCENE, std::string(o.data(), o.size()), CAMP_EXCEPT_MASK(nCamp));
		}
	}

	return true;
}

bool CWarBase::broadCastDataToHaveSightPlayer(Vector3 vec, BYTE byKeyAction, LPCSTR pszData, int nDataLen, bool bToView)
{
	if (pszData == NULL || nDataLen == 0)
	{
		return false;
	}

	SERVICE_PTR pContainer = gServerGlobal->getSceneManager()->getScene(m_WarInfo.nSceneID);
	if (pContainer == NULL)
	{
		return false;
	}

	obuf o;

	if (bToView)
	{
		g_EHelper.fillDataToClientView(o, byKeyAction, pszData, nDataLen);
	}
	else
	{
		SNetMsgHead head;
		head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
		head.byKeyModule = MSG_MODULEID_WAR;
		head.byKeyAction = byKeyAction;

		o << head;
		o.push_back(pszData, nDataLen);
	}

	IGameSceneService * pScene = (IGameSceneService*)pContainer->query_interface();
	if (pScene != NULL)
	{
		pScene->broadcast(vec, 0, BROADCAST_SCENE, std::string(o.data(), o.size()), AOI_PROXY_PTR(0), false);
	}

	return true;
}

int CWarBase::getStaticID()
{
	return m_context.nConfigWarID;
}

int CWarBase::getMapID()
{

	return m_context.dwMapID;
}

int CWarBase::getWarID()
{
	return m_context.nWarID;
}

DWORD CWarBase::getMatchTypeID()
{
	return m_context.dwMatchTypeID;
}

LONGLONG CWarBase::getWarDBKey()
{
	return m_WarInfo.dwDBWarID;
}

DWORD CWarBase::getWarPlayTime()
{
	if (m_WarInfo.nStartTimeTick == 0)
	{
		return 0;
	}

	DWORD dwCurTick = gServerGlobal->getTimeSyncService()->GetTick();
	if (dwCurTick < m_WarInfo.nStartTimeTick)
	{
		return 0;
	}

	return dwCurTick - m_WarInfo.nStartTimeTick;
}

SWPerson* CWarBase::getPlayerInfo(UID uid)
{
	std::map<UID, WarPersonVector::iterator>::iterator iter = m_mapUID2IterList.find(uid);
	return  (iter == m_mapUID2IterList.end() ? NULL : &(*(iter->second)));
}


LONGLONG CWarBase::createDBWarId(int nSceneId)
{
	return SCENE_GUID(nSceneId);
}


// 发送数据到客户端
void CWarBase::sendAllPersonInfoToAllClient()
{
	if (getWarPhase() == EWP_End)
	{
		return;
	}

	int nPersonCount = 0;
	// 组织数据发送到客户端
	obuf oAllPersonInfo;
	obuf oAllPerson;
	WarCampMap::iterator itCamp = m_mapWarCamp.begin();
	for (; itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
		for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{
			ActorHelper actorHelper(itPerson->uID);
			IActorService *pActorService = actorHelper.m_ptr;
			if (pActorService != NULL && itPerson->nLevel <= 0)
			{
				itPerson->nLevel = pActorService->getProperty_Integer(PROPERTY_LEVEL);
			}
			msg_war_person_info tempInfo;
			tempInfo.uPDBID = itPerson->uPDBID;
			tempInfo.nCamp = itCamp->first;
			tempInfo.nIndex = itPerson->nIndex;
			tempInfo.uID = itPerson->uID;

			tempInfo.nLevel = itPerson->nLevel;
			tempInfo.nKillNum = itPerson->nBaseRecord[EDT_KillCount];
			tempInfo.nKillMonsterNum = itPerson->nBaseRecord[EDT_Money]; // 连杀，后续处理
			tempInfo.nHeroID = itPerson->nHeroID;
			tempInfo.nSkinID = itPerson->nHeroSkinID;
			tempInfo.nEXP = itPerson->nExperien;
			tempInfo.nDeadNum = itPerson->nDeadNum;
			tempInfo.nContKill = itPerson->deadContKillList.back();
			tempInfo.nAssists = itPerson->nBaseRecord[EDT_AssistCount];
			tempInfo.bIsOnGame = itPerson->bIsOnGame;
			tempInfo.nSummerID = itPerson->nSummerID;
			sstrcpyn(tempInfo.gcPlayerName, itPerson->szName, sizeof(tempInfo.gcPlayerName));
			tempInfo.bIsAI = getProperty_Integer(itPerson->uID, PROPERTY_AI_SCHEME_ID);
			tempInfo.nAliveContAssist = itPerson->nAliveContAssist;
			tempInfo.bySex = itPerson->byActorSex;
			tempInfo.byPlayerType = itPerson->byPlayerType;
			oAllPerson << tempInfo;
			nPersonCount++;

		}

	}

	msg_war_person_info_list PersonList;
	PersonList.nNum = nPersonCount;
	oAllPersonInfo << PersonList;
	oAllPersonInfo.push_back(oAllPerson.data(), oAllPerson.size());
	broadCastDataToAllPlayer(SC_MSG_WAR_PERSONINFO, oAllPersonInfo.data(), oAllPersonInfo.size());
}

void CWarBase::sendToOpenTabInfoClient(PDBID uNeedUpdateActor, SWPerson & sWPerson)
{
	if (getWarPhase() == EWP_End)
	{
		return;
	}

	int nPersonCount = 0;
	// 组织数据发送到客户端
	obuf oAllPersonInfo;
	obuf oAllPerson;

	ActorHelper actorHelper(sWPerson.uID);
	IActorService *pActorService = actorHelper.m_ptr;
	if (pActorService != NULL && sWPerson.nLevel <= 0)
	{
		sWPerson.nLevel = pActorService->getProperty_Integer(PROPERTY_LEVEL);
	}
	msg_war_update_persontab tempInfo;
	tempInfo.uPDBID = sWPerson.uPDBID;
	tempInfo.uID = sWPerson.uID;
	tempInfo.nLevel = sWPerson.nLevel;
	tempInfo.nKillNum = sWPerson.nBaseRecord[EDT_KillCount];
	tempInfo.nKillMonsterNum = sWPerson.nBaseRecord[EDT_Money];
	tempInfo.nAssists = sWPerson.nBaseRecord[EDT_AssistCount];
	tempInfo.nDeadNum = sWPerson.nDeadNum;
	tempInfo.nContKill = sWPerson.deadContKillList.back();
	tempInfo.bIsOnGame = sWPerson.bIsOnGame;
    tempInfo.nDestroyTower = sWPerson.nBaseRecord[EDT_DestoryTower];
	oAllPerson << tempInfo;

	nPersonCount++;

	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_WAR;
	head.byKeyAction = SC_MSG_WAR_UPDATE_PERSONTAB;

	msg_war_person_info_list PersonList;
	PersonList.nNum = nPersonCount;

	oAllPersonInfo << PersonList;
	oAllPersonInfo.push_back(oAllPerson.data(), oAllPerson.size());

	obuf oSendData;
	oSendData << head;
	oSendData.push_back(oAllPersonInfo.data(), oAllPersonInfo.size());

	map<PDBID, sTabRecordInfo>::iterator bIter = m_mapTableRequest.begin();
	for (; bIter != m_mapTableRequest.end(); ++bIter)
	{
		if (bIter->second.bOpen)
		{
			ActorHelper actorHelper(bIter->first, ACTOR_DBID);
			IActorService *pActorService = actorHelper.m_ptr;
			if (pActorService == NULL)
			{
				continue;
			}
			send2Client(pActorService->getUID(), PACKAGE_PTR(new std::string(oSendData.data(), oSendData.size())));
		}
	}
}


SWarCamp * CWarBase::getWarCamp(int nCamp)
{
	WarCampMap::iterator iter = m_mapWarCamp.find(nCamp);
	if (iter == m_mapWarCamp.end())
		return NULL;

	return &(iter->second);
}

vector<int> CWarBase::getCampList()
{
	vector<int> campList;
	WarCampMap::iterator iter = m_mapWarCamp.begin();
	for (; iter != m_mapWarCamp.end(); ++iter)
	{
		campList.push_back(iter->first);
	}

	return campList;
}

void CWarBase::setWinCamp(int nWinCamp)
{
	if (m_mapWarCamp.find(nWinCamp) == m_mapWarCamp.end())
	{
		ErrorLn("m_mapWarCamp.find(nWinCamp) == m_mapWarCamp.end(), nWinCamp=" << nWinCamp);
		return;
	}

	m_WarInfo.nWinCamp = nWinCamp;
}

bool CWarBase::broadCastDataToSameCamp(BYTE byKeyAction, LPCSTR pszData, int nDataLen, int nCamp, bool bToView )
{
	return broadCastDataToAllPlayer(byKeyAction, pszData, nDataLen, nCamp, bToView);
}

void CWarBase::sendDataToClient(UID uid, BYTE byKeyAction, void* data, size_t len, bool bToView)
{
	obuf ob;
	if (bToView)
	{
		g_EHelper.fillDataToClientView(ob, byKeyAction, (LPCSTR)data, len);
	}
	else
	{
		SNetMsgHead head;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
		head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
		head.byKeyModule = MSG_MODULEID_WAR;
		head.byKeyAction = byKeyAction;

		ob << head;
		ob.push_back(data, len);
	}

	send2Client(uid, PACKAGE_PTR(new std::string(ob.data(), ob.size())));

	// 同时发给OB
	for (auto iter = m_OBPlayerBaseInfo.begin(); iter != m_OBPlayerBaseInfo.end(); ++iter)
	{
		send2Client(iter->second.uidActor, PACKAGE_PTR(new std::string(ob.data(), ob.size())));
	}
}

void CWarBase::sendMsgAllUidToClient()
{
	if (m_WarInfo.nPersonNum > m_mapRealPlayerUid.size())
	{
		return;
	}

	// 发送数据
	int nCount = m_mapRealPlayerUid.size();
	obuf obuffData;
	obuffData << nCount;

	map<PDBID, UID>::iterator itBegin = m_mapRealPlayerUid.begin();
	for (; itBegin != m_mapRealPlayerUid.end(); ++itBegin)
	{
		msg_war_all_players_created msgInfo;
		UID uID = itBegin->second;
		msgInfo.uUID = uID;
		msgInfo.uPDBID = itBegin->first;
		obuffData << msgInfo;
	}

	broadCastDataToAllPlayer(SC_MSG_WAR_ENTITY_ALL_PLAYER_CREATED, obuffData.data(), obuffData.size());
}

SWPerson* CWarBase::getPlayerInfoByPDBIU(PDBID uPBBID)
{
	std::map<UID, WarPersonVector::iterator>::iterator iter = m_mapPDBID2IterList.find(uPBBID);
	return  (iter == m_mapPDBID2IterList.end() ? NULL : &(*(iter->second)));
}

void CWarBase::sendWarBaseInfoToClient(UID uid)
{
	if (getWarPhase() == EWP_End)
	{
		return;
	}

	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_WAR;
	head.byKeyAction = SC_MSG_WAR_NOTIFY_WAR_INFO;


	msg_war_rsp_war_info msg;
	msg.nWarStartTick = m_WarInfo.nStartTimeTick;
	msg.lDBWarID = m_WarInfo.dwDBWarID;
	msg.nConfigWarID = getStaticID();
	msg.isFull = m_WarInfo.isPlayerFull;
	msg.nMatchType = getMatchTypeID();

	obuf obuffData;
	obuffData << head << msg;

	send2Client(uid, PACKAGE_PTR(new std::string(obuffData.data(), obuffData.size())));
}

// 发送玩家基本信息到客户端
void CWarBase::sendPlayerBaseInfoToClient(UID uId)
{
	if (getWarPhase() == EWP_End)
	{
		return;
	}



	// 战场记录信息
	sendWarRecordInfo(uId);

	sendMsgAllUidToClient();
}

void CWarBase::sendWarRecordInfo(UID uid)
{
	int nPersonCount = 0;
	// 组织数据发送到客户端
	obuf oAllPersonInfo;
	obuf oAllPerson;
	WarCampMap::iterator itCamp = m_mapWarCamp.begin();
	for (; itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
		for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{
			ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
			IActorService *pActorService = actorHelper.m_ptr;
			if (pActorService != NULL && itPerson->nLevel <= 0)
			{
				itPerson->nLevel = pActorService->getProperty_Integer(PROPERTY_LEVEL);
			}
			msg_war_person_info tempInfo;
			tempInfo.uPDBID = itPerson->uPDBID;
			tempInfo.nCamp = itCamp->first;
			tempInfo.nIndex = itPerson->nIndex;
			tempInfo.uID = itPerson->uID;

			tempInfo.nLevel = itPerson->nLevel;
			tempInfo.nKillNum = itPerson->nBaseRecord[EDT_KillCount];
			tempInfo.nKillMonsterNum = itPerson->nBaseRecord[EDT_Money]; // 连杀，后续处理
			tempInfo.nHeroID = itPerson->nHeroID;
			tempInfo.nSkinID = itPerson->nHeroSkinID;
			tempInfo.nEXP = itPerson->nExperien;
			//tempInfo.nEXP = pActorService->getProperty_Integer(PROPERTY_EXP);
			tempInfo.nDeadNum = itPerson->nDeadNum;
			tempInfo.nContKill = itPerson->deadContKillList.back();
			tempInfo.nAssists = itPerson->nBaseRecord[EDT_AssistCount];
			tempInfo.bIsOnGame = itPerson->bIsOnGame;
			tempInfo.nSummerID = itPerson->nSummerID;
			sstrcpyn(tempInfo.gcPlayerName, itPerson->szName, sizeof(tempInfo.gcPlayerName));
			tempInfo.bIsAI = getProperty_Integer(itPerson->uID, PROPERTY_AI_SCHEME_ID);
			tempInfo.nAliveContAssist = itPerson->nAliveContAssist;
			tempInfo.bySex = itPerson->byActorSex;
			tempInfo.byPlayerType = itPerson->byPlayerType;
			oAllPerson << tempInfo;
			nPersonCount++;
		}
	}

	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_ENTITY;
	head.byKeyAction = SC_MSG_WAR_PERSONINFO_INITAL;

	SMsgEntityHead entityHead;
	entityHead.uidMaster = uid;
	entityHead.byPartID = PART_WAR;

	obuf obuffData;
	obuffData << head << entityHead;

	msg_war_person_info_list PersonList;
	PersonList.nNum = nPersonCount;
	obuffData << PersonList;
	obuffData.push_back(oAllPerson.data(), oAllPerson.size());

	send2Client(uid, PACKAGE_PTR(new std::string(obuffData.data(), obuffData.size())));
}


const void CWarBase::getCampPdbidString(int nCamp, char* resultStr, size_t nLen)
{
	if (nCamp != CAMP_BLUE && nCamp != CAMP_RED)
		return;

	string& pdbidStr = m_CampPdbidStr[nCamp];

	if (pdbidStr.empty())
	{
		SWarCamp* pWarCamp = getWarCamp(nCamp);
		if (pWarCamp == NULL)
			return;

		WarPersonVector& personList = pWarCamp->warPersVect;

		return;

		stringstream ss;
		for (WarPersonVector::iterator iter = personList.begin(); iter != personList.end(); ++iter)
		{
			if (iter->bIsComputer)
				continue;

			ss << iter->uPDBID;
			ss << ';';
		}

		pdbidStr = ss.str();
	}

	if (pdbidStr.size() > nLen)
	{
		WarningLn(__FUNCTION__",pdbidStr.size() > nLen, pdbidStr.size=" << pdbidStr.size());
	}

	sstrcpyn(resultStr, pdbidStr.c_str(), nLen);
}

int CWarBase::getPlayTimeMin()
{
	DWORD dwWarContinueTime = getWarPlayTime();

	// 战场打了多少分钟
	DWORD nPlayTime = dwWarContinueTime / (1000 * 60);

	return nPlayTime;
}

void CWarBase::stopComputerAI()
{
	for (WarCampMap::iterator itCamp = m_mapWarCamp.begin(); itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		WarPersonVector &personList = itCamp->second.warPersVect;
		for (WarPersonVector::iterator itPerson = personList.begin(); itPerson != personList.end(); ++itPerson)
		{
			if (itPerson->bIsComputer)
			{
				UID uid = itPerson->uID;

				AIServiceHelper aiHelper(UID_2_SCENE(uid));
				if (aiHelper.m_ptr)
				{
					aiHelper.m_ptr->destroyAI(uid);
				}
			}
		}
	}
}

void CWarBase::bcastPersonInfo(UID uid, EWarPersonInfoUpdateType nUpdateType)
{
	SWPerson* pPerson = getPlayerInfo(uid);
	if (pPerson == NULL)
		return;

	SWar2ViewMsgPersonInfoUpdate msg;
	msg.nUpdateType = nUpdateType;
	msg.pdbid = pPerson->uPDBID;

	obuf ob;
	ob << msg;

	switch (nUpdateType)
	{
	case EWPTUT_VOCATION:
		{
			ob << pPerson->nHeroID;
			ob << pPerson->nHeroSkinID;
		}
		break;
	default:break;
	}

	broadCastDataToAllPlayer(WAR2VIEW_MSG_WAR_PERSON_INFO_UPDATE, ob.data(), ob.size(), BCASTALLCAMP, true);
}

void CWarBase::onClientReqWarBaseInfo(UID uid, void * pData, size_t stLen)
{
	ActorHelper actorHelper(uid);
	IActorService* pActorService = actorHelper.m_ptr;
	if (pActorService == NULL)
		return;

	PDBID pdbid = pActorService->getProperty_Integer(PROPERTY_ID);
	if (pdbid == INVALID_PDBID)
		return;

	// 存储已经创建好的uid；
	m_mapRealPlayerUid[pdbid] = uid;

	// 发送战场信息到客户端
	sendWarBaseInfoToClient(uid);

	// 发送战场基本信息到客户端
	sendPlayerBaseInfoToClient(uid);
}

int CWarBase::getAllPerson(UID *PersonArray, int nArraySize)
{
	if (PersonArray == NULL || nArraySize <= 0)
		return 0;

	int nCount = 0;
	for (WarCampMap::iterator itCamp = m_mapWarCamp.begin(); itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		WarPersonVector & personList = itCamp->second.warPersVect;
		for (WarPersonVector::iterator itPerson = personList.begin(); itPerson != personList.end(); ++itPerson)
		{
			if (nCount >= nArraySize)
			{
				return nCount;
			}

			PersonArray[nCount++] = itPerson->uID;
		}
	}
	return nCount;
}