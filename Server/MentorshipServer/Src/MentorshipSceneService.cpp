#include "stdafx.h"
#include "MentorshipSceneService.h"
#include "ICenterConnectorService.h"
#include "IServerGlobal.h"
#include "EntityHelper.h"
#include "IDBEngineService.h"
#include "ExternalHelper.h"
#include "EventDef.h"
#include "IManagerConnectorService.h"
#include "Chat_ManagedDef.h"
#include "TaskStorageDef.h"
#include "IMessageDispatch.h"
#include "ChatHelper.h"
#include "OssLogDef.h"
#include "TitleDef.h"
#include "MentorshipHelper.h"
#include "Mentorship_ManagedDef.h"

using namespace Mentorship_ManagerDef;

// 构造函数
MentorshipSceneService::MentorshipSceneService()
{
	m_nMentorshipIndex = 0;

	m_MentorshipList.clear();
	m_MasterList.clear();
	m_PrenticeList.clear();
}

// 析构函数
MentorshipSceneService::~MentorshipSceneService()
{
	
}

void MentorshipSceneService::onStop()
{
    IEventEngine *pEventEngine = gServerGlobal->getEventEngine();
    if (pEventEngine != NULL)
    {
        // 取消订阅角色事件
        pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_LOGIN, SOURCE_TYPE_PERSON, 0);
    }

    IMessageDispatch *pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (pMessageDispatch != NULL)
    {
        // 自己需要处理消息
        pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_MENTORSHIP);
        // 取消注册响应客户端消息
        pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_MENTORSHIP);
    }

    IDBEngineService *pDBEngine = gServerGlobal->getDBEngineService();
    if (pDBEngine != NULL)
    {
        pDBEngine->unregisterHandler(this);
    }
}

void MentorshipSceneService::release()
{
	TraceLn(__FUNCTION__);
    delete this;
}

bool MentorshipSceneService::create()
{
    TraceLn(__FUNCTION__);

	gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);

	IMessageDispatch *pMessageDispatch = gServerGlobal->getMessageDispatch();
	if (pMessageDispatch == NULL)
		return false;
	// 自己需要处理消息
	pMessageDispatch->registerTransmitHandler(MSG_MODULEID_MENTORSHIP, this);
	// 处理客户端发送过来的消息
	pMessageDispatch->registerMessageHandler(MSG_MODULEID_MENTORSHIP, this);

	IEventEngine *pEventEngine = gServerGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return false;
	// 订阅角色事件
	pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_LOGIN, SOURCE_TYPE_PERSON, 0, __FUNCTION__);

    return true;
}

// 通知服务器列表已经更新
void MentorshipSceneService::onServerListUpdated()
{

}

// 通知服务器信息更新
void MentorshipSceneService::onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{

}

void MentorshipSceneService::handleServerMsg(DWORD serverID, SNetMsgHead head, void *data, size_t len)
{
    int nLen = (int)len;

    switch (head.byKeyAction)
    {
    case MSG_MENTORSHIP_UPDATE_MENTORSHIP_OS:
        {
            onTransmitUpdateMentorship((LPCSTR)data, nLen);
        }
        break;
	case MSG_MENTORSHIP_DISMISS_OS:
		{
			onTransmitDismissMentorship((LPCSTR)data, nLen);
		}
		break;
	case MSG_MENTORSHIP_GIVE_FIRSTEWIN_OS:
		{
			onTransmitGiveFirstWinAward((LPCSTR)data, nLen);
		}
		break;
	case MSG_MENTORSHIP_REMOVEALLSHIP_OS:
		{
			onTransmitRemoveAllMentorship((LPCSTR)data, nLen);
		}
		break;
    default:
        break;
    }
}

void MentorshipSceneService::handleClientMsg(DWORD client, SNetMsgHead head, void *data, size_t len)
{
    if (isPublicGameWorld() == true)
        return;
}

// 发送战斗结果
void MentorshipSceneService::sendFightResult(DWORD dwPDBID, bool bFightResult, bool bFirstWin, BYTE nMentorshipPost)
{
	SMsgMentorship_Add_Member_ShipVal_SO sendData;
	sendData.dwPDBID = dwPDBID;
	sendData.bFightResult = bFightResult;
	sendData.bFirstWin = bFirstWin;
	sendData.nPost = nMentorshipPost;

	g_EHelper.sendMentorshipSocial(emMsgMentorship_Module, MSG_MENTORSHIP_ADD_MEMBER_SHIPVAL_SO, &sendData, sizeof(sendData));
}

// 师傅存在师徒关系
BYTE MentorshipSceneService::getMyMentorshipPost(DWORD dwSrcPDBID, DWORD dwDesPDBID)
{
	TMap_MemMentorshipIt it = m_MasterList.find(dwSrcPDBID);
	if (it != m_MasterList.end())
	{
		size_t nLen = (*it).second.size();
		for (size_t i=0; i < nLen; ++i)
		{
			CMentorship *pMentorship = findMentorship((*it).second[i]);
			if (pMentorship != NULL && pMentorship->getPrenticePDBID() == dwDesPDBID)
			{
				return emMentorshipPost_Master;
			}
		}
	}

	it = m_PrenticeList.find(dwSrcPDBID);
	if (it != m_PrenticeList.end())
	{
		size_t nLen = (*it).second.size();
		for (size_t i=0; i < nLen; ++i)
		{
			CMentorship *pMentorship = findMentorship((*it).second[i]);
			if (pMentorship != NULL && pMentorship->getMasterPDBID() == dwDesPDBID)
			{
				return emMentorshipPost_Prentice;
			}
		}
	}

	return emMentorshipPost_Normal;
}

// 获取师徒奖励
void MentorshipSceneService::getMentorshipAward(DWORD dwPDBID, int* nExpPercent, int* nHeroExpPercent, int* nGoldPercent)
{
	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig != NULL)
	{
		if (pConfig->nExpPercent > 0)
		{
			*nExpPercent = pConfig->nExpPercent;
		}

		if (pConfig->nHeroExpPercent > 0)
		{
			*nHeroExpPercent = pConfig->nHeroExpPercent;
		}

		if (pConfig->nGoldPercent > 0)
		{
			*nGoldPercent = pConfig->nGoldPercent;
		}
	}	
}

// 获取师傅个数
int MentorshipSceneService::getPlayerMasterCount(DWORD dwDBID)
{
	TMap_MemMentorshipIt it = m_MasterList.find(dwDBID);
	if (it == m_MasterList.end())
		return 0;

	return (*it).second.size();
}

// 获取学徒个数
int MentorshipSceneService::getPlayerPrenticeCount(DWORD dwDBID)
{
	TMap_MemMentorshipIt it = m_PrenticeList.find(dwDBID);
	if (it == m_PrenticeList.end())
		return 0;

	return (*it).second.size();
}

// 检测目标是否为玩家徒弟
int MentorshipSceneService::checkIsPlayerPrentice(DWORD dwDBID, DWORD dwTargetDBID)
{
	CMentorship *pMentorship = findMentorshipByPDBID(dwDBID, dwTargetDBID);
	if (pMentorship == NULL)
	{
		return 0;
	}

	return 1;
}

////////////////////////////////////////ITransmitHandler//////////////////////////////////////////
// 消息处理
void MentorshipSceneService::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void *data, size_t len)
{
    
    MentorshipHelper mentorshipSceneHelper;
    IMentorshipSceneService *pMentorshipSceneService = mentorshipSceneHelper.m_ptr;
    if (pMentorshipSceneService == NULL)
    {
        ErrorLn(__FUNCTION__": server ="<<server<<" uMsgID ="<<uMsgID);
        return;
    }

    pMentorshipSceneService->handleServerMsg( server, *head, data, len);
}


///////////////////////////////////////IMessageHandler/////////////////////////////////////////
// 消息处理
void MentorshipSceneService::onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead *head, void *data, size_t len)
{
    MentorshipHelper mentorshipSceneHelper;
    IMentorshipSceneService *pMentorshipSceneService = mentorshipSceneHelper.m_ptr;
    if (pMentorshipSceneService == NULL)
    {
        ErrorLn(__FUNCTION__": clientID ="<<clientID<<" uMsgID ="<<uMsgID);
        return;
    }

    pMentorshipSceneService->handleClientMsg( clientID, *head, data, len);
}


/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
void MentorshipSceneService::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	if (bSrcType == SOURCE_TYPE_PERSON)
	{
		switch (wEventID)
		{

		default:
			break;
		}
	}
}

void MentorshipSceneService::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{

}

// 获取师徒索引
int MentorshipSceneService::getMentorshipIndex()
{
	m_nMentorshipIndex++;
	return m_nMentorshipIndex;
}

// 查找师徒数据
CMentorship* MentorshipSceneService::findMentorship(DWORD dwMentorshipID)
{
	TMap_MentorshipIt it = m_MentorshipList.find(dwMentorshipID);
	if (it == m_MentorshipList.end())
		return NULL;

	return (*it).second;
}

// 查找师徒数据
CMentorship* MentorshipSceneService::findMentorshipByPDBID(DWORD dwMasterPDBID, DWORD dwPrenticePDBID)
{
	TMap_MentorshipIt it = m_MentorshipList.begin();
	for (; it != m_MentorshipList.end(); ++it)
	{
		if ((*it).second->getMasterPDBID() == dwMasterPDBID && (*it).second->getPrenticePDBID() == dwPrenticePDBID)
		{
			return (*it).second;
		}
	}

	return NULL;
}

// 通知任务
void MentorshipSceneService::sendPlayerTaskEvent(DWORD dwPDBID, DWORD dwDesPDBID)
{
	ActorHelper actorHelper(dwPDBID, ACTOR_DBID);
	IActorService * pActorService = actorHelper.m_ptr;
	if( pActorService == NULL ) 
		return;
	
	pActorService->buildMentorship(dwDesPDBID);
}

// 更新师徒属性
void MentorshipSceneService::onTransmitUpdateMentorship(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen != sizeof(SMsgMentorship_UpdateMentorship_OS))
        return;

    SMsgMentorship_UpdateMentorship_OS *pRecvData = (SMsgMentorship_UpdateMentorship_OS*)pszMsg;
    CMentorship *pMentorship = findMentorshipByPDBID(pRecvData->dwMasterPDBID, pRecvData->dwPrenticePDBID);
    if (pMentorship == NULL)
	{
		pMentorship = new CMentorship();
		if (pMentorship == NULL)
		{
			WarningLn(__FUNCTION__": new CMentorship() faild MasterID" << pRecvData->dwMasterPDBID << ",PrenticeID" << pRecvData->dwPrenticePDBID);
			return;
		}

		DWORD dwMentorshipID = (DWORD)getMentorshipIndex();

		if (pMentorship->create(dwMentorshipID, pRecvData) == false)
		{
			WarningLn(__FUNCTION__": pMentorship->Create() faild MasterPDBID" << pRecvData->dwMasterPDBID << ",PrenticeID" << pRecvData->dwPrenticePDBID);
			delete pMentorship;
		}

		m_MentorshipList[dwMentorshipID] = pMentorship;

		TMap_MemMentorshipIt itMaster = m_MasterList.find(pRecvData->dwMasterPDBID);
		if (itMaster != m_MasterList.end())
		{
			vector<DWORD>::iterator tmpIt = find((*itMaster).second.begin(), (*itMaster).second.end(), dwMentorshipID);
			if (tmpIt == (*itMaster).second.end())
			{
				(*itMaster).second.push_back(dwMentorshipID);
			}
		} 
		else
		{
			vector<DWORD> tempList;
			tempList.clear();
			tempList.push_back(dwMentorshipID);
			m_MasterList[pRecvData->dwMasterPDBID] = tempList;
		}

		TMap_MemMentorshipIt itPrentice = m_PrenticeList.find(pRecvData->dwPrenticePDBID);
		if (itPrentice != m_PrenticeList.end())
		{
			vector<DWORD>::iterator tmpIt = find((*itPrentice).second.begin(), (*itPrentice).second.end(), dwMentorshipID);
			if (tmpIt == (*itPrentice).second.end())
			{
				(*itPrentice).second.push_back(dwMentorshipID);
			}
		} 
		else
		{
			vector<DWORD> tempList;
			tempList.clear();
			tempList.push_back(dwMentorshipID);
			m_PrenticeList[pRecvData->dwPrenticePDBID] = tempList;
		}

		sendPlayerTaskEvent(pRecvData->dwMasterPDBID, pRecvData->dwPrenticePDBID);
		sendPlayerTaskEvent(pRecvData->dwPrenticePDBID, pRecvData->dwMasterPDBID);
	}
	else
	{
		pMentorship->updateData(pRecvData);
	}
}

// 解散师徒数据
void MentorshipSceneService::onTransmitDismissMentorship(LPCSTR pszMsg, int nLen)
{
	if (nLen != sizeof(SMsgMentorship_Dismiss_OS))
	{
		return;
	}

	SMsgMentorship_Dismiss_OS* pRecvData = (SMsgMentorship_Dismiss_OS*)pszMsg;

	CMentorship *pMentorship = findMentorshipByPDBID(pRecvData->dwMasterPDBID, pRecvData->dwPrenticePDBID);
	if (NULL == pMentorship)
	{
		return;
	}

	DWORD dwMentorshipID = pMentorship->getMentorshipID();

	// 移除师傅列表
	TMap_MemMentorshipIt itMaster = m_MasterList.find(pMentorship->getMasterPDBID());
	if (itMaster != m_MasterList.end())
	{
		vector<DWORD>::iterator tmpIt = (*itMaster).second.begin();
		while (tmpIt != (*itMaster).second.end())
		{
			if ((*tmpIt) == dwMentorshipID)
			{
				(*itMaster).second.erase(tmpIt);
				break;
			}
			else
			{
				++tmpIt;
			}
		}

		if ((*itMaster).second.size() == 0)
		{
			m_MasterList.erase(itMaster);
		}
	}

	// 移除学徒列表
	TMap_MemMentorshipIt itPrentice = m_PrenticeList.find(pMentorship->getPrenticePDBID());
	if (itPrentice != m_PrenticeList.end())
	{
		vector<DWORD>::iterator tmpIt = (*itPrentice).second.begin();
		while (tmpIt != (*itPrentice).second.end())
		{
			if ((*tmpIt) == dwMentorshipID)
			{
				(*itPrentice).second.erase(tmpIt);
				break;
			}
			else
			{
				++tmpIt;
			}
		}

		if ((*itPrentice).second.size() == 0)
		{
			m_PrenticeList.erase(itPrentice);
		}
	}

	// 释放战队对象
	pMentorship->release();

	// 移除记录
	m_MentorshipList.erase(dwMentorshipID);
}

// 发放首胜奖励
void MentorshipSceneService::onTransmitGiveFirstWinAward(LPCSTR pszMsg, int nLen)
{
	if (nLen != sizeof(SMsgMentorship_GiveFirstWin_SO))
	{
		return;
	}

	SMsgMentorship_GiveFirstWin_SO* pRecvData = (SMsgMentorship_GiveFirstWin_SO*)pszMsg;

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
	{
		return;
	}

	ActorHelper helper(pRecvData->dwPDBID, ACTOR_DBID);
	IActorService* pActor = helper.m_ptr;
	if( pActor == NULL )
	{
		ErrorLn(__FUNCTION__": can't find actorService uID = " << pRecvData->dwPDBID);
		return;
	}

	if (pRecvData->nMentorhsipPost == emMentorshipPost_Master && pConfig->nPFWinPrizeID > 0)
	{
		pActor->prizeActor(pConfig->nPFWinPrizeID, 1, OSSRES_ADD_MENTORSHIP_TASK, 0, a2utf8("战场学徒首胜奖励"));
	}
	else if (pRecvData->nMentorhsipPost == emMentorshipPost_Prentice && pConfig->nMFWinPrizeID > 0)
	{
		pActor->prizeActor(pConfig->nMFWinPrizeID, 1, OSSRES_ADD_MENTORSHIP_TASK, 0, a2utf8("战场导师首胜奖励"));
	}
}

// 移除所有师徒关系
void MentorshipSceneService::onTransmitRemoveAllMentorship(LPCSTR pszMsg, int nLen)
{
	if (nLen != sizeof(SMsgMentorship_RemoveAllShip_OS))
	{
		return;
	}

	SMsgMentorship_RemoveAllShip_OS* pRecvData = (SMsgMentorship_RemoveAllShip_OS*)pszMsg;

	TMap_MentorshipIt it;
	for (it = m_MentorshipList.begin(); it != m_MentorshipList.end(); ++it)
	{
		SAFE_RELEASE((*it).second);
	}

	m_MentorshipList.clear();
	m_MasterList.clear();
	m_PrenticeList.clear();
}



