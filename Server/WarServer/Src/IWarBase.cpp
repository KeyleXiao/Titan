/*******************************************************************
** 文件名: IWarBase.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/1
** 版  本: 1.0
** 描  述: 战场基类-实现基本的功能函数
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/

#include "StdAfx.h"
#include "IWarBase.h"
#include "GUID.h"
#include "PathFindHelper.h"
#include "IPathFindService.h"

IWarBase::IWarBase()
	: m_pWarService(NULL)
	, m_nSceneID(INVALID_SCENE_ID)
	, m_nStartTimeTick(0)
	, m_dwDBWarID(0)
	, m_lMutex(EWP_Ready)
	, m_bIsStart(false)                  // 战场是否已开启
	, m_WarPhase(EWP_Ready)
	, m_nPersonNum(0)                    // 进入的玩家个数
	, m_BeginTime(0)                     // 战场开始时间
	, m_continueTime(0)                  // 总的比赛时间
	, m_bIsFinish(false)                 // 比赛是否结束
	, m_EndWarReason(CAMP_WIN_INVALID)
	, m_bNormalEnd(true)
	, m_bIsWarShowAddEXPAndMoney(false)

{
}

IWarBase::~IWarBase()
{
}

bool IWarBase::isEnd()
{
	return m_bIsFinish;
}

EWarEndReason IWarBase::getWarEndReason()
{
	return m_EndWarReason;
}

void IWarBase::setWarEndReason(EWarEndReason eReason)
{
	m_EndWarReason = eReason;
}

int IWarBase::getWarSceneID()
{
	return m_nSceneID;
}

int IWarBase::getWarTypeId()
{
	return m_context.nConfigWarID;
}

bool IWarBase::Init()
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
	m_bIsWarShowAddEXPAndMoney = pSchemeEntity->getEntityXml()->nWarShowAddEXPAndMoney == 0 ? false : true;

	return true;
}

void IWarBase::UnInit()
{
}

void IWarBase::Start()
{
	if (m_bIsStart)
	{
		return;
	}

	m_BeginTime = (DWORD)time(NULL);
	m_bIsStart = true;

	TraceLn(__FUNCTION__ ": Start() nSceneID =" << m_nSceneID << ", nWarID = " << m_context.nWarID << ", dwRoomID = " << m_context.dwRoomID);

	// 生成唯一id
	m_dwDBWarID = createDBWarId(gServerGlobal->getServerID());

	TraceLn(__FUNCTION__"DBWarID=" << m_dwDBWarID);

	PathFindHelper helper(m_nSceneID);
	IPathFindService *pPathService = helper.m_ptr;
	if (pPathService != NULL)
	{
		if (!pPathService->startDynamicObstacleProxy())
		{
			ErrorLn("Start Dynamic Obstacle Service Failed !!!");
		}
	}
}

DWORD IWarBase::getWarContinueTime()
{
	if (m_BeginTime == 0)
	{
		return 0;
	}

	return (DWORD)time(NULL) - m_BeginTime;
}

bool IWarBase::broadCastDataToAllPlayer(BYTE byKeyAction, LPCSTR pszData, int nDataLen, int nCamp)
{
	if (pszData == NULL || nDataLen == 0)
	{
		return false;
	}

	SERVICE_PTR pContainer = gServerGlobal->getSceneManager()->getScene(m_nSceneID);
	if (pContainer == NULL)
	{
		return false;
	}
	obuf o;
	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byKeyModule = MSG_MODULEID_WAR;
	head.byKeyAction = byKeyAction;


	o << head;
	o.push_back(pszData, nDataLen);

	IGameSceneService * pScene = (IGameSceneService*)pContainer->query_interface();
	if (pScene != NULL)
	{
		if (nCamp == CAMP_MAX)
		{
			pScene->broadcast_all(BROADCAST_SCENE, std::string(o.data(), o.size()));
		}
		else if (nCamp >= CAMP_NEUTRAL && nCamp < CAMP_MAX)
		{
			pScene->broadcast_all(BROADCAST_SCENE, std::string(o.data(), o.size()), CAMP_MASK(nCamp));
		}
	}
	return true;
}

bool IWarBase::broadCastDataToAllOtherCamp(BYTE byKeyAction, LPCSTR pszData, int nDataLen, int nCamp)
{
	if (pszData == NULL || nDataLen == 0)
	{
		return false;
	}

	SERVICE_PTR pContainer = gServerGlobal->getSceneManager()->getScene(m_nSceneID);
	if (pContainer == NULL)
	{
		return false;
	}
	obuf o;
	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byKeyModule = MSG_MODULEID_WAR;
	head.byKeyAction = byKeyAction;

	o << head;
	o.push_back(pszData, nDataLen);

	IGameSceneService * pScene = (IGameSceneService*)pContainer->query_interface();
	if (pScene != NULL)
	{
		if (nCamp == CAMP_MAX)
		{
			pScene->broadcast_all(BROADCAST_SCENE, std::string(o.data(), o.size()));
		}
		else if (nCamp >= CAMP_NEUTRAL && nCamp < CAMP_MAX)
		{
			pScene->broadcast_all(BROADCAST_SCENE, std::string(o.data(), o.size()), CAMP_EXCEPT_MASK(nCamp));
		}
	}

	return true;
}

bool IWarBase::broadCastDataToHaveSightPlayer(Vector3 vec, BYTE byKeyAction, LPCSTR pszData, int nDataLen)
{
	if (pszData == NULL || nDataLen == 0)
	{
		return false;
	}

	SERVICE_PTR pContainer = gServerGlobal->getSceneManager()->getScene(m_nSceneID);
	if (pContainer == NULL)
	{
		return false;
	}

	obuf o;
	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byKeyModule = MSG_MODULEID_WAR;
	head.byKeyAction = byKeyAction;

	o << head;
	o.push_back(pszData, nDataLen);

	IGameSceneService * pScene = (IGameSceneService*)pContainer->query_interface();
	if (pScene != NULL)
	{
		pScene->broadcast(vec, 0, BROADCAST_SCENE, std::string(o.data(), o.size()), AOI_PROXY_PTR(0), false);
	}

	return true;
}

const SchemeWarPersonConfigInfo * IWarBase::getPersonConfigInfo()
{
	// 获得配置信息
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return NULL;
	}
	ISchemeWarPersonConfigInfo *pSchemeWarPersonConfigInfo = pSchemeCenter->getSchemeWarPersonConfigInfo();
	return pSchemeWarPersonConfigInfo ? pSchemeWarPersonConfigInfo->GetSchemeWarPersonConfigInfo(getStaticID()) : NULL;
}

int IWarBase::getStaticID()
{
	return m_context.nConfigWarID;
}

int IWarBase::getSceneID()
{
	return m_nSceneID;
}

int IWarBase::getMapID()
{

	return m_context.dwMapID;
}

int IWarBase::getWarID()
{
	return m_context.nWarID;
}

DWORD IWarBase::getMatchTypeID()
{
	return m_context.dwMatchTypeID;
}

LONGLONG IWarBase::getWarDBKey()
{
	return m_dwDBWarID;
}

DWORD IWarBase::getWarPlayTime()
{
	if (m_nStartTimeTick == 0)
	{
		return 0;
	}

	DWORD dwCurTick = gServerGlobal->getTimeSyncService()->GetTick();
	if (dwCurTick < m_nStartTimeTick)
	{
		return 0;
	}

	return dwCurTick - m_nStartTimeTick;
}

void IWarBase::setNormalEnd(bool bNormalEnd)
{
	m_bNormalEnd = bNormalEnd;
}

SWPerson* IWarBase::getPlayerInfo(UID uid)
{
	std::map<UID, WarPersonVector::iterator>::iterator iter = m_mapUID2IterList.find(uid);
	return  (iter == m_mapUID2IterList.end() ? NULL : &(*(iter->second)));
}

bool IWarBase::isHaveRealPlayer(int nCamp)
{
	WarCampMap::iterator itCamp = m_mapWarCamp.find(nCamp);
	if (itCamp != m_mapWarCamp.end())
	{
		WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
		for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{
			if (!itPerson->bIsComputer)
				return true;
		}
	}

	return false;
}

LONGLONG IWarBase::createDBWarId(int nSceneId)
{
	return SCENE_GUID(nSceneId);
}
