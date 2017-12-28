#include "stdafx.h"
#include "MentorshipService.h"
//#include "ICenterConnectorService.h"
#include "ISocialGlobal.h"
#include "Event_ManagedDef.h"
#include "IShareServer.h"
#include "ExternalHelper.h"
#include "ISchemeCenter.h"
#include "IMessageDispatch.h"
#include "ISocialGlobal.h"
#include "MentorshipHelper.h"
#include "ClanHelper.h"
#include "KinHelper.h"
#include "ClanDef.h"
#include "OrderEvent.h"
#include "EventDef.h"
#include "Chat_ManagedDef.h"
#include "Simple_Atom_Lock.h"
#include "Mentorship_ManagedDef.h"
#include "OssLogDef.h"
#include "IOSSLogService.h"
#include "ISNSService.h"
#include "SNSHelper.h"
#include "ChatDef.h"

using namespace Mentorship_ManagerDef;

struct IDBEngineService;

MentorshipService::MentorshipService()
{
	m_nMentorshipIndex = 0;
	m_MentorshipList.clear();
	m_MasterList.clear();
	m_PrenticeList.clear();
	m_PersonDataList.clear();
	m_DeleteMentorshipList.clear();
}


MentorshipService::~MentorshipService()
{

}



void MentorshipService::release()
{
	TraceLn(__FUNCTION__);

    //取消注册
    gSocialGlobal->getEventEngine()->UnSubscibe(this, EVENT_ORDER_UPDATE_RANK, SOURCE_TYPE_ODER, 0);
 
    // 数据库接口反注册
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService != NULL)
    {
        pBEngineService->unregisterHandler(this);
    }

    // 共享信息注册
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver != NULL)
    {
        pShareReceiver->UnregisterHandler(this);
    }

    IMessageDispatch* pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        // 服务器消息分发反注册
        pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_MENTORSHIP);
        // 客户端消息分发反注册
        pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_MENTORSHIP);
    }

	g_EHelper.KillTimer(MENTORSHIP_CLEAR_MENTORSHIP_LIST_TIMER, this);

	delete this;
}

bool MentorshipService::create()
{
	TraceLn(__FUNCTION__);
    // 战队事件注册
	IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return false;

    pEventEngine->Subscibe(this, EVENT_ORDER_UPDATE_RANK, SOURCE_TYPE_ODER, 0, __FUNCTION__);

    // 数据库接口注册
    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if (pDBEngineService != NULL)
    {
        pDBEngineService->registerHandler(this);
    }

    // 共享信息注册
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver != NULL)
    {
        pShareReceiver->RegisterHandler(this, __FUNCTION__);
    }

    IMessageDispatch* pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        // 服务器消息分发注册
        pMessageDispatch->registerTransmitHandler(MSG_MODULEID_MENTORSHIP,this);
        // 客户端消息分发注册
        pMessageDispatch->registerMessageHandler(MSG_MODULEID_MENTORSHIP, this);
    }

	// 启动服务器
	g_EHelper.SetTimer(MENTORSHIP_CLEAR_MENTORSHIP_LIST_TIMER, 10*60*1000, this, "MentorshipService::create");

	return true;
}

int MentorshipService::getMentorshipIndex()
{
	m_nMentorshipIndex++;

	return m_nMentorshipIndex;
}

void MentorshipService::send2Client( ClientID clientID, obuf256& obuf)
{
    IGatewayAcceptorService* pGatewayAcceptorService = gSocialGlobal->getGatewayAcceptorService();
	if (nullptr == pGatewayAcceptorService)
	{
		ErrorLn(__FUNCTION__": failed!  PDBID=" << clientID);
		return;
	}

    if(clientID == INVALID_CLIENT)
	{
		ErrorLn(__FUNCTION__": failed! clientID == INVALID_CLIENT !");
		return;
	}

	pGatewayAcceptorService->sendData(clientID, PACKAGE_PTR(new std::string((char const *)obuf.data(), obuf.size())));
}

/// purpose: 更新基础数据to客户端
void MentorshipService::updateMentorshipPersonBaseToClient(CMentorshipPersonData* pMentorshipPerson)
{
	if (pMentorshipPerson == NULL)
		return;

	obuf1024 obuf;
	SMentorshipPerson mentorshipPerson = getMentorshipPersonInfo(pMentorshipPerson);
	fillMentorshipData(obuf, MSG_ENDPOINT_CLIENT, MSG_MENTORSHIP_UPDATE_BASE_PERSONDATA);
	obuf.push_back(&mentorshipPerson, sizeof(SMentorshipPerson));
	g_EHelper.SendDataToPerson(mentorshipPerson.dwPDBID, obuf.data(), obuf.size());
}

/// purpose: 更新师徒数据到客户端
void MentorshipService::updateMentorshipPersonToClient(CMentorshipPersonData* pMentorshipPerson)
{
	if (pMentorshipPerson == NULL)
		return;

	SMentorshipPerson mentorshipPerson = getMentorshipPersonInfo(pMentorshipPerson);
	SMsgMentorship_PersonData_OC sendData;
	sendData.nCount = 0;

	obuf2048 obNode;
	TMap_PersonMentorshipIt it = m_MasterList.find(mentorshipPerson.dwPDBID);
	if (it != m_MasterList.end())
	{
		list<DWORD>::iterator tmpIt = (*it).second.begin();
		for (; tmpIt != (*it).second.end(); ++tmpIt)
		{
			CMentorship *pMentorship = findMentorship((*tmpIt));
			if (pMentorship == NULL)
			{
				continue;
			}
			SMentorshipInfo mentorshipInfo = pMentorship->getMentorshipInfo();
			if (mentorshipInfo.dwMasterPDBID == mentorshipPerson.dwPDBID)
			{
				SClientMentorShip mentorshipNode;
				mentorshipNode.dwMentorshipID = mentorshipInfo.dwMentorshipID;
				mentorshipNode.dwPDBID = mentorshipInfo.dwPrenticePDBID;
				sstrcpyn(mentorshipNode.szName, mentorshipInfo.szPrenticeName, sizeof(mentorshipNode.szName));
				mentorshipNode.nFaceID = mentorshipInfo.nFaceID;
				mentorshipNode.bIsOnline = isOnline(mentorshipInfo.dwPrenticePDBID);
				mentorshipNode.nPost = emMentorshipPost_Prentice;
				mentorshipNode.nRelationType = mentorshipInfo.nRelationType;
				mentorshipNode.nState = mentorshipInfo.nState;
				mentorshipNode.dwBuildTime = mentorshipInfo.dwBuildTime;
				mentorshipNode.dwSeverPDBID = mentorshipInfo.dwSeverPDBID;
				mentorshipNode.dwSeverTime = mentorshipInfo.dwSeverTime;

				sendData.nCount += 1;
				obNode.push_back(&mentorshipNode, sizeof(SClientMentorShip));
			}
		}
	}

	TMap_PersonMentorshipIt itPrentice = m_PrenticeList.find(mentorshipPerson.dwPDBID);
	if (itPrentice != m_PrenticeList.end())
	{
		list<DWORD>::iterator tmpIt = (*itPrentice).second.begin();
		for (; tmpIt != (*itPrentice).second.end(); ++tmpIt)
		{
			CMentorship *pMentorship = findMentorship((*tmpIt));
			if (pMentorship == NULL)
			{
				continue;
			}
			SMentorshipInfo mentorshipInfo = pMentorship->getMentorshipInfo();
			if (mentorshipInfo.dwPrenticePDBID == mentorshipPerson.dwPDBID)
			{
				SClientMentorShip mentorshipNode;
				mentorshipNode.dwMentorshipID = mentorshipInfo.dwMentorshipID;
				mentorshipNode.dwPDBID = mentorshipInfo.dwMasterPDBID;
				sstrcpyn(mentorshipNode.szName, mentorshipInfo.szMasterName, sizeof(mentorshipNode.szName));
				mentorshipNode.bIsOnline = isOnline(mentorshipInfo.dwMasterPDBID);
				mentorshipNode.nPost = emMentorshipPost_Master;
				mentorshipNode.nRelationType = mentorshipInfo.nRelationType;
				mentorshipNode.nState = mentorshipInfo.nState;
				mentorshipNode.dwBuildTime = mentorshipInfo.dwBuildTime;
				mentorshipNode.dwSeverPDBID = mentorshipInfo.dwSeverPDBID;
				mentorshipNode.dwSeverTime = mentorshipInfo.dwSeverTime;

				sendData.nCount += 1;
				obNode.push_back(&mentorshipNode, sizeof(SClientMentorShip));
			}
		}
	}

	obuf2048 obuf;
	fillMentorshipData(obuf, MSG_ENDPOINT_CLIENT, MSG_MENTORSHIP_PERSONDATA);
	obuf.push_back(&sendData, sizeof(SMsgMentorship_PersonData_OC));
	obuf.push_back(&mentorshipPerson, sizeof(SMentorshipPerson));
	obuf.push_back(obNode.data(), obNode.size());
	g_EHelper.SendDataToPerson(mentorshipPerson.dwPDBID, obuf.data(), obuf.size());
}

/// purpose: 添加师徒关系数据到客户端
void MentorshipService::addMentorshipToClient(CMentorship* pMentorship, SSharePersonProperty &shareProperty)
{
	if (pMentorship == NULL || shareProperty.dwPDBID == INVALID_PDBID)
	{
		return;
	}

	SClientMentorShip clientMentorship;
	SMentorshipInfo mentorshipInfo = pMentorship->getMentorshipInfo();

	clientMentorship.dwMentorshipID = mentorshipInfo.dwMentorshipID;
	clientMentorship.nRelationType = mentorshipInfo.nRelationType;
	clientMentorship.nState = mentorshipInfo.nState;
	clientMentorship.dwBuildTime = mentorshipInfo.dwBuildTime;

	if (shareProperty.dwPDBID == mentorshipInfo.dwMasterPDBID)
	{
		clientMentorship.dwPDBID = mentorshipInfo.dwPrenticePDBID;
		sstrcpyn(clientMentorship.szName, mentorshipInfo.szPrenticeName, sizeof(clientMentorship.szName));
		clientMentorship.nFaceID = mentorshipInfo.nFaceID;
		clientMentorship.bIsOnline = isOnline(mentorshipInfo.dwPrenticePDBID);
		clientMentorship.nPost = emMentorshipPost_Prentice;
	} 
	else
	{
		clientMentorship.dwPDBID = mentorshipInfo.dwMasterPDBID;
		sstrcpyn(clientMentorship.szName, mentorshipInfo.szMasterName, sizeof(clientMentorship.szName));
		clientMentorship.bIsOnline = isOnline(mentorshipInfo.dwMasterPDBID);
		clientMentorship.nPost = emMentorshipPost_Master;
	}

	obuf512 obuf;
	fillMentorshipData(obuf, MSG_ENDPOINT_CLIENT, MSG_MENTORSHIP_ADD_MENTORSHIP);
	obuf.push_back(&clientMentorship, sizeof(SClientMentorShip));

	g_EHelper.SendDataToPerson(shareProperty.dwPDBID, obuf.data(), obuf.size());
}

void MentorshipService::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    if (nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__":error return! nDBRetCode = " << nDBRetCode << "  reason：" << pszDBRetDesc);
		return;
	}

	switch (nCmdID)
	{ 
		// 读取师徒关系列表                 -- 数据库返回处理
	case GAMEDB_REQUEST_READ_MENTORSHIP:
		{
			onReturnReadMentorshipList(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}

		// 添加师徒关系数据
	case GAMEDB_REQUEST_ADD_MENTORSHIP:
		{
			onReturnAddMentorship(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}

		// 更新师徒关系数据             -- 数据库返回处理
	case GAMEDB_REQUEST_UPDATE_MENTORSHIP:
		{
			onReturnUpdateMentorship(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}

		// 删除师徒关系数据       -- 数据库返回处理
	case GAMEDB_REQUEST_DEL_MENTORSHIP:
		{
			onReturnDelMentorship(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}

		// 读取师徒个人数据       -- 数据库返回处理
	case GAMEDB_REQUEST_READ_MENTORSHIPPERSON:
		{
			onReturnReadMentorshipPersonData(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}
		// 更新师徒个人数据(没有则添加)       -- 数据库返回处理
	case GAMEDB_REQUEST_UPDATE_MENTORSHIPPERSON:
		{
			onReturnUpdateMentorshipPersonData(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}
	}
}

/// purpose: 读取师徒关系列表         					-- 来自数据库
void MentorshipService::onReturnReadMentorshipList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
	IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		return;
	}

	// 验证数据库返回结果
	if(nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn("[onReturnReadMentorshipList] read MentorshipList error！ pszDBRetDesc = " << a2utf8(pszDBRetDesc));
		return;
	}

	SMentorshipConfig *pMentorshipConfig = g_EHelper.getConfig();
	if (pMentorshipConfig == NULL)
	{
		ErrorLn(_GT("[onReturnReadMentorshipList] get SMentorshipConfig error！"));
		return;
	}

	// 检查数据返回长度
	if (nOutLen < sizeof(DBREQ_RESULT_READMENTORSHIPLIST))
	{
		int tempLen = sizeof(DBREQ_RESULT_READMENTORSHIPLIST);
		ErrorLn("[onReturnReadMentorshipList] error length outlength = " << nOutLen << " sizeof(DBREQ_RESULT_READMENTORSHIPLIST) = " << tempLen);
		return;
	}
	
	DBREQ_RESULT_READMENTORSHIPLIST* pRetData = (DBREQ_RESULT_READMENTORSHIPLIST*)pOutData;
	EmphasisLn("return Mentorship count =" << pRetData->nCurCount);
	if (nOutLen != sizeof(DBREQ_RESULT_READMENTORSHIPLIST) + pRetData->nCurCount * sizeof(DBREQ_TABLE_MENTORSHIP))
	{
		int tempLen = sizeof(DBREQ_RESULT_READMENTORSHIPLIST) + pRetData->nCurCount * sizeof(DBREQ_TABLE_MENTORSHIP);
		ErrorLn("[onReturnReadMentorshipList] error lengthlist outlength = " << nOutLen << " sizeof(DBREQ_RESULT_READMENTORSHIPLIST) = " << tempLen);
		return;
	}
	
	int offset = sizeof(DBREQ_RESULT_READMENTORSHIPLIST);
	for (int i=0; i < pRetData->nCurCount; ++i)
	{
		DBREQ_TABLE_MENTORSHIP* pMentorshipData = (DBREQ_TABLE_MENTORSHIP*)(pOutData + offset);

		if (pMentorshipData->nState == emMentorshipState_Dismissing && (DWORD)time(NULL) - pMentorshipData->dwSeverTime >= pMentorshipConfig->nServerTimeLen)
		{
			SMentorshipDelInfo delInfo;
			delInfo.dwMasterPDBID = pMentorshipData->dwMasterPDBID;
			delInfo.dwPrenticePDBID = pMentorshipData->dwPrenticePDBID;
			m_DeleteMentorshipList.push_back(delInfo);
			offset += sizeof(DBREQ_TABLE_MENTORSHIP);
			continue;
		}

		CMentorship* pMentorship = findMentorshipByPDBID(pMentorshipData->dwMasterPDBID, pMentorshipData->dwPrenticePDBID);
		if (pMentorship != NULL)
		{
			offset += sizeof(DBREQ_TABLE_MENTORSHIP);
			continue;
		}

		pMentorship = new CMentorship();
		if (NULL == pMentorship)
		{
			ErrorLn(__FUNCTION__": pMentorship == NULL" << ",MasterID" << pMentorshipData->dwMasterPDBID << ", PrenticeID" << pMentorshipData->dwPrenticePDBID);
			offset += sizeof(DBREQ_TABLE_MENTORSHIP);
			continue;
		}

		DWORD dwMentorshipID = (DWORD)getMentorshipIndex();
		if (pMentorship->createFromDB(this, dwMentorshipID, pMentorshipData) == false)
		{
			ErrorLn(__FUNCTION__": pMentorship createFromDB failed" << ",MasterID" << pMentorshipData->dwMasterPDBID << ", PrenticeID" << pMentorshipData->dwPrenticePDBID);
			delete pMentorship;
			offset += sizeof(DBREQ_TABLE_MENTORSHIP);
			continue;
		}

		m_MentorshipList[dwMentorshipID] = pMentorship;
		offset += sizeof(DBREQ_TABLE_MENTORSHIP);

		TMap_PersonMentorshipIt itMaster = m_MasterList.find(pMentorshipData->dwMasterPDBID);
		if (itMaster != m_MasterList.end())
		{
			(*itMaster).second.push_back(dwMentorshipID);
		} 
		else
		{
			list<DWORD> tempList;
			tempList.clear();
			tempList.push_back(dwMentorshipID);
			m_MasterList[pMentorshipData->dwMasterPDBID] = tempList;
		}

		TMap_PersonMentorshipIt itPrentice = m_PrenticeList.find(pMentorshipData->dwPrenticePDBID);
		if (itPrentice != m_PrenticeList.end())
		{
			(*itPrentice).second.push_back(dwMentorshipID);
		} 
		else
		{
			list<DWORD> tempList;
			tempList.clear();
			tempList.push_back(dwMentorshipID);
			m_PrenticeList[pMentorshipData->dwPrenticePDBID] = tempList;
		}
	}

	delMentorshipFromDB();
}

/// purpose: 添加师徒关系数据         					-- 来自数据库
void MentorshipService::onReturnAddMentorship(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
	// 验证数据库返回结果
	if(nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn("[onReturnReadMentorshipList] Add Mentorship error！ pszDBRetDesc = " << a2utf8(pszDBRetDesc));
		return;
	}
}

/// purpose: 更新师徒关系数据         					-- 来自数据库
void MentorshipService::onReturnUpdateMentorship(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
	// 验证数据库返回结果
	if(nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn("[onReturnUpdateMentorship] Update Mentorship error！ pszDBRetDesc = " << a2utf8(pszDBRetDesc));
		return;
	}
}

/// purpose: 删除师徒关系数据         					-- 来自数据库
void MentorshipService::onReturnDelMentorship(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
	// 验证数据库返回结果
	if(nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn("[onReturnDelMentorship] Del Mentorship error！ pszDBRetDesc = " << a2utf8(pszDBRetDesc));
		return;
	}
}

/// purpose: 读取师徒个人数据         					-- 来自数据库
void MentorshipService::onReturnReadMentorshipPersonData(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
	IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		return;
	}

	// 验证数据库返回结果
	if(nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn("[onReturnReadMentorshipPersonData] read MentorshipPersonData error！ pszDBRetDesc = " << a2utf8(pszDBRetDesc));
		return;
	}

	if (nOutLen != sizeof(DBREQ_RESULT_READMENTORSHIPPERSON))
	{
		int tempLen = sizeof(DBREQ_RESULT_READMENTORSHIPPERSON);
		ErrorLn("[onReturnReadMentorshipPersonData] error lengthlist outlength = " << nOutLen << " sizeof(DBREQ_RESULT_READMENTORSHIPPERSON) = " << tempLen);
		return;
	}

	DBREQ_RESULT_READMENTORSHIPPERSON* pRetData = (DBREQ_RESULT_READMENTORSHIPPERSON*)pOutData;
	// 共享信息
	IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
	{
		return;
	}
	SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pRetData->dwPDBID);
	if ( shareProperty.dwPDBID != pRetData->dwPDBID)
	{
		return;
	}
	
	CMentorshipPersonData *pPersonData = findMentorshipPersonData(pRetData->dwPDBID);
	if (pPersonData != NULL)
	{
		return;
	}

	CMentorshipPersonData* pMentorshipPerson = new CMentorshipPersonData();
	if (pMentorshipPerson == NULL)
		return;

	if (pMentorshipPerson->setDBDataContext(this, pRetData) == true)
	{
		m_PersonDataList[pRetData->dwPDBID] = pMentorshipPerson;

		if (pMentorshipPerson->updateDataByDiffWeek() == true)
		{
			pMentorshipPerson->updateDataToDB();
		}

		updateMentorshipPersonToClient(pMentorshipPerson);

		// 自动添加到发布消息
		autoAddMentorshipIssue(shareProperty, pMentorshipPerson);
	}
	else
	{
		delete pMentorshipPerson;
	}
}

/// purpose: 更新师徒个人数据(没有则添加)         		-- 来自数据库
void MentorshipService::onReturnUpdateMentorshipPersonData(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
	// 验证数据库返回结果
	if(nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn("[onReturnReadKinList] Update MentorshipPersonData error！ pszDBRetDesc = " << a2utf8(pszDBRetDesc));
		return;
	}
}

/// purpose: 添加师徒首胜奖励
void MentorshipService::processAddFirstWinPrize(CMentorship *pMentorship, BYTE nAddPost, SMentorshipConfig *pConfig)
{
	if (pMentorship == NULL || pConfig == NULL)
		return;

	if (pMentorship->getMentorshipRelationType() != emMentorshipType_Formal)
		return;

	int nFWinAwardTimeLen = pConfig->nMFWinAwardTimeLen;
	DWORD dwPDBID = pMentorship->getPrenticePDBID();
	int nMaxCount = pConfig->nMFWinAwardCount;

	if (nAddPost == emMentorshipPost_Master)
	{
		nFWinAwardTimeLen = pConfig->nPFWinAwardTimeLen;
		dwPDBID = pMentorship->getMasterPDBID();
		nMaxCount = pConfig->nPFWinAwardCount;
	}

	if (pMentorship->getMentorshipBuildTime() + nFWinAwardTimeLen > (DWORD)time(NULL))
		return;

	CMentorshipPersonData *pPersonData = findMentorshipPersonData(dwPDBID);
	if (pPersonData == NULL)
	{
		IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
		if (pDBEngine != NULL)
		{
			DBREQ_PARAM_UPDATEMFWINAWARDPRECOUNT referData;
			referData.dwPDBID = dwPDBID;
			referData.nAddCount = 1;
			referData.nMaxCount = nMaxCount;
			if (pDBEngine->executeSP(GAMEDB_REQUEST_ADD_MFWINAWARDPRECOUNT, 0, (LPCSTR)&referData, sizeof(referData), this) == false)
			{
				ErrorLn(__FUNCTION__": GAMEDB_REQUEST_ADD_MFWINAWARDPRECOUNT failed!! PDBID =" << dwPDBID);
			}
		}
	}
	else
	{
		pPersonData->updateDataByDiffWeek();
		DWORD dwPreDropID = (nAddPost == emMentorshipPost_Master ? em_MshipPersonProp_PFWinAwardPreCount : em_MshipPersonProp_MFWinAwardPreCount);
		DWORD dwDropID = (nAddPost == emMentorshipPost_Master ? em_MshipPersonProp_PFWinAwardCount : em_MshipPersonProp_MFWinAwardCount);
		int nPreCount = pPersonData->getNumProp(dwPreDropID);
		int nCurCount = pPersonData->getNumProp(dwDropID);

		if (nCurCount < 0 || nPreCount < 0 || (nCurCount + nPreCount) >= nMaxCount)
			return;

		pPersonData->setNumProp(dwPreDropID, nPreCount + 1, true);
		updateMentorshipPersonBaseToClient(pPersonData);
	}
}

/// purpose: 更改师徒玩家属性
void MentorshipService::changeMentorshipPlayerName(DWORD dwPDBID, SHAREUPDATE_REASON nUpdateReason, ISharePerson * pSharePerson)
{
	if (pSharePerson == NULL)
		return;

	TMap_PersonMentorshipIt it = m_MasterList.find(dwPDBID);
	if (it != m_MasterList.end())
	{	
		for (list<DWORD>::iterator tmpIt=(*it).second.begin(); tmpIt != (*it).second.end(); ++tmpIt)
		{
			CMentorship *pMentorship = findMentorship((*tmpIt));
			if (pMentorship == NULL)
				continue;

			pMentorship->changeMasterName(pSharePerson->GetName());
		}
	}

	TMap_PersonMentorshipIt itPrentice = m_PrenticeList.find(dwPDBID);
	if (itPrentice != m_PrenticeList.end())
	{
		for (list<DWORD>::iterator tmpIt=(*itPrentice).second.begin(); tmpIt != (*itPrentice).second.end(); tmpIt++)
		{
			CMentorship *pMentorship = findMentorship((*tmpIt));
			if (pMentorship == NULL)
				continue;

			pMentorship->changePrenticeName(pSharePerson->GetName());
		}
	}
}

/// purpose: 添加师徒成员师徒值消息处理					-- 来自场景服
void MentorshipService::onSceneAddMentorshipValue(DWORD dwServerID, LPCSTR pszData, size_t nLen)
{
	if (pszData == NULL || nLen != sizeof(SMsgMentorship_Add_Member_ShipVal_SO))
	{
		return;
	}

	SMsgMentorship_Add_Member_ShipVal_SO *pRecvData = (SMsgMentorship_Add_Member_ShipVal_SO*)pszData;

	CMentorshipPersonData *pPersonData = findMentorshipPersonData(pRecvData->dwPDBID);
	if (pPersonData == NULL)
	{
		return;
	}

	pPersonData->addMentorshipValue(pRecvData);

	if (pRecvData->bFirstWin == false)
	{
		return;
	}

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
	{
		return;
	}

	TMap_PersonMentorshipIt it = m_MasterList.find(pRecvData->dwPDBID);
	if (it != m_MasterList.end())
	{	
		for (list<DWORD>::iterator tmpIt=(*it).second.begin(); tmpIt != (*it).second.end(); ++tmpIt)
		{
			CMentorship *pMentorship = findMentorship((*tmpIt));
			processAddFirstWinPrize(pMentorship, emMentorshipPost_Prentice, pConfig);
		}
	}

	TMap_PersonMentorshipIt itPrentice = m_PrenticeList.find(pRecvData->dwPDBID);
	if (itPrentice != m_PrenticeList.end())
	{
		for (list<DWORD>::iterator tmpIt=(*itPrentice).second.begin(); tmpIt != (*itPrentice).second.end(); tmpIt++)
		{
			CMentorship *pMentorship = findMentorship((*tmpIt));
			processAddFirstWinPrize(pMentorship, emMentorshipPost_Master, pConfig);
		}
	}
}

/// purpose: 师徒数据						-- 来自客户端
void MentorshipService::onClientMentorshipPersonData(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	if (pszData == NULL || nLen != sizeof(SMsgMentorship_PersonData_OC))
		return;

	CMentorshipPersonData *pPersonData = findMentorshipPersonData(shareProperty.dwPDBID);
	if (pPersonData == NULL)
	{
		return;
	}

	updateMentorshipPersonToClient(pPersonData);
}

/// purpose: 							-- 来自客户端
void MentorshipService::onClientMentorshipIssue(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	if (pszData == NULL || nLen != sizeof(SMsgMentorship_Issue))
		return;

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
	{
		return;
	}

	DWORD dwPDBID = shareProperty.dwPDBID;
	SMsgMentorship_Issue *pRecvData = (SMsgMentorship_Issue*)pszData;

	if (pRecvData->nIssueType == emMentorshipIssueType_Master)
	{
		if (shareProperty.wLevel >= pConfig->nMasterNeedLevel)
		{
			int nFormalCount = getMentorshipFormalCount(dwPDBID, emMentorshipPost_Master);
			int nInternCount = getMentorshipInternCount(dwPDBID, emMentorshipPost_Master);
			if (nFormalCount < pConfig->nFormalPrenticeCount || nInternCount < pConfig->nInternPrenticeCount)
			{
				SMentorshipIssue *pIssue = g_MentorshipIssueManager.findMasterIssue(dwPDBID);
				if (pIssue != NULL)
				{
					sstrcpyn(pIssue->szIssueInfo, pRecvData->szIssueInfo, sizeof(pIssue->szIssueInfo));
				}
				else
				{
					SMentorshipIssue mentorshipIssue;
					mentorshipIssue.dwPDBID = dwPDBID;
					mentorshipIssue.nIssueType = emMentorshipIssueType_Master;
					sstrcpyn(mentorshipIssue.szIssueInfo, pRecvData->szIssueInfo, sizeof(mentorshipIssue.szIssueInfo));
					g_MentorshipIssueManager.addMasterIssue(&mentorshipIssue);
					g_MentorshipIssueManager.addMIssueOnlineMember(dwPDBID);
				}
				
				g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_MENTORSHIP_MASTER_ISSUE_SUCESS, "");

				g_MentorshipIssueManager.sendIssueListToClient(dwPDBID, emMentorshipIssueType_Master);
			}
		}
	}
	else if (pRecvData->nIssueType == emMentorshipIssueType_Prentice)
	{
		if (shareProperty.wLevel >= pConfig->nPrenticeNeedLevel)
		{
			int nFormalCount = getMentorshipFormalCount(dwPDBID, emMentorshipPost_Prentice);
			int nInternCount = getMentorshipInternCount(dwPDBID, emMentorshipPost_Prentice);
			if (nFormalCount < pConfig->nFormalMasterCount || nInternCount < pConfig->nInternMasterCount)
			{
				SMentorshipIssue *pIssue = g_MentorshipIssueManager.findPrenticeIssue(dwPDBID);
				if (pIssue != NULL)
				{
					sstrcpyn(pIssue->szIssueInfo, pRecvData->szIssueInfo, sizeof(pIssue->szIssueInfo));
				}
				else
				{
					SMentorshipIssue mentorshipIssue;
					mentorshipIssue.dwPDBID = dwPDBID;
					mentorshipIssue.nIssueType = emMentorshipIssueType_Prentice;
					sstrcpyn(mentorshipIssue.szIssueInfo, pRecvData->szIssueInfo, sizeof(mentorshipIssue.szIssueInfo));
					g_MentorshipIssueManager.addPrenticeIssue(&mentorshipIssue);
					g_MentorshipIssueManager.addPIssueOnlineMember(dwPDBID);
				}

				g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_MENTORSHIP_PRENTICE_ISSUE_SUCESS, "");

				g_MentorshipIssueManager.sendIssueListToClient(dwPDBID, emMentorshipIssueType_Prentice);
			}
		}
	}
}

/// purpose: 师徒发布消息列表					-- 来自客户端
void MentorshipService::onClientMentorshipIssueList(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	if (pszData == NULL || nLen != sizeof(SMsgMentorshipIssueList))
		return;

	DWORD dwPDBID = shareProperty.dwPDBID;
	SMsgMentorshipIssueList *pRecvData = (SMsgMentorshipIssueList*)pszData;

	g_MentorshipIssueManager.sendIssueListToClient(dwPDBID, pRecvData->nIssueType);
}

/// purpose: 师徒关系申请					-- 来自客户端
void MentorshipService::onClientMentorshipApply(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	if (pszData == NULL || nLen != sizeof(SMsgMentorship_Apply))
		return;

	SMsgMentorship_Apply *pRecvData = (SMsgMentorship_Apply*)pszData;

	if (pRecvData->dwDesPDBID == shareProperty.dwPDBID)
	{
		return;
	}

	if (pRecvData->nApplyPost == emMentorshipPost_Master)
	{
		applyMentorshipMaster(shareProperty, pRecvData->dwDesPDBID, pRecvData->nApplyPost, pRecvData->nRelationType);
	}
	else if (pRecvData->nApplyPost == emMentorshipPost_Prentice)
	{
		applyMentorshipPrentice(shareProperty, pRecvData->dwDesPDBID, pRecvData->nApplyPost, pRecvData->nRelationType);
	}
}

/// purpose: 师徒关系申请应答				-- 来自客户端
void MentorshipService::onClientMentorshipApplyAnswer(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	if (pszData == NULL || nLen != sizeof(SMsgMentorship_Apply_Anwser))
		return;

	SMsgMentorship_Apply_Anwser *pRecvData = (SMsgMentorship_Apply_Anwser*)pszData;

	if (pRecvData->dwSrcPDBID == shareProperty.dwPDBID)
	{
		return;
	}

	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

	SSharePersonProperty srcShareProperty = pShareReceiver->GetSharePerson(pRecvData->dwSrcPDBID);
	if (srcShareProperty.dwPDBID != pRecvData->dwSrcPDBID)
	{
		if (pRecvData->bAnwser == true)
		{
			g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_APPLY_ANSWER_LEAVELINE, pRecvData->szSrcName);
		}
		return;
	}

	if (pRecvData->bAnwser == false)
	{
		if (pRecvData->nApplyPost == emMentorshipPost_Master)
		{
			g_EHelper.showSystemMessage(srcShareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_APPLY_M_ANSWER_REFUSE, shareProperty.szName);
		}
		else if (pRecvData->nApplyPost == emMentorshipPost_Prentice)
		{
			g_EHelper.showSystemMessage(srcShareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_APPLY_P_ANSWER_REFUSE, shareProperty.szName);
		}
		return;
	}

	if (pRecvData->nApplyPost == emMentorshipPost_Master)
	{
		applyMentorshipMasterAnswer(shareProperty, srcShareProperty, pRecvData->nApplyPost, pRecvData->nRelationType);
	}
	else if (pRecvData->nApplyPost == emMentorshipPost_Prentice)
	{
		applyMentorshipPrenticeAnswer(shareProperty, srcShareProperty, pRecvData->nApplyPost, pRecvData->nRelationType);
	}
}

/// purpose: 师徒发布消息自动发布设置					-- 来自客户端
void MentorshipService::onClientMentorshipIssueAutoSet(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	if (pszData == NULL || nLen != sizeof(SMsgMentorshipIssue_AutoSet))
		return;

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
		return;

	SMsgMentorshipIssue_AutoSet *pRecvData = (SMsgMentorshipIssue_AutoSet*)pszData;

	CMentorshipPersonData *pPersonData = findMentorshipPersonData(shareProperty.dwPDBID);
	if (pPersonData == NULL)
	{
		return;
	}

	BYTE nIssueType = pRecvData->nIssueType;
	if (nIssueType != emMentorshipIssueType_Master || nIssueType != emMentorshipIssueType_Prentice)
	{
		return;
	}

	if (nIssueType == emMentorshipIssueType_Master && shareProperty.wLevel < pConfig->nMasterNeedLevel)
	{
		return;
	}

	if (nIssueType == emMentorshipIssueType_Prentice && shareProperty.wLevel < pConfig->nPrenticeNeedLevel)
	{
		return;
	}

	DWORD dwPropID = (nIssueType == emMentorshipIssueType_Master) ? em_MshipPersonProp_AutoIssueMFlag : em_MshipPersonProp_AutoIssuePFlag;

	pPersonData->setNumProp(dwPropID, pRecvData->bAutoSet, true);

	obuf256 ob;
	SMsgMentorshipIssue_AutoSet sendData;
	sendData.nIssueType = nIssueType;
	sendData.bAutoSet = pPersonData->getNumProp(dwPropID);
	fillMentorshipData(ob, MSG_ENDPOINT_CLIENT, MSG_MENTORSHIP_ISSUE_AUTOSET);
	ob.push_back(&sendData, sizeof(sendData));
	g_EHelper.SendDataToPerson(shareProperty.dwPDBID, ob.data(), ob.size());
}

/// purpose: 师徒关系解散					-- 来自客户端
void MentorshipService::onClientMentorshipDismiss(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	if (pszData == NULL || nLen != sizeof(SMsgMentorship_Dismiss))
		return;

	SMsgMentorship_Dismiss *pRecvData = (SMsgMentorship_Dismiss*)pszData;

	CMentorship *pMentorship = findMentorship(pRecvData->dwMentorshipID);
	if (pMentorship == NULL)
		return;

	if (shareProperty.dwPDBID != pMentorship->getMasterPDBID() && shareProperty.dwPDBID != pMentorship->getPrenticePDBID())
	{
		return;
	}

	pMentorship->onClientDismiss(shareProperty.dwPDBID);
}

/// purpose: 师徒关系提升学徒				-- 来自客户端
void MentorshipService::onClientMentorshipPromote(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	if (pszData == NULL || nLen != sizeof(SMsgMentorship_Promote))
		return;

	SMsgMentorship_Promote *pRecvData = (SMsgMentorship_Promote*)pszData;

	if (shareProperty.dwPDBID == pRecvData->dwDesPDBID)
		return;

	applyMentorshipMaster(shareProperty, pRecvData->dwDesPDBID, emMentorshipPost_Master, emMentorshipType_Formal);
}

/// purpose: 请求领取首胜奖励				-- 来自客户端
void MentorshipService::onClientMentorshipReqFirstAward(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	if (pszData == NULL || nLen != sizeof(SMsgMentorship_Req_FirstAward))
		return;

	SMsgMentorship_Req_FirstAward *pRecvData = (SMsgMentorship_Req_FirstAward*)pszData;

	CMentorshipPersonData *pPersonData = findMentorshipPersonData(shareProperty.dwPDBID);
	if (pPersonData == NULL)
		return;

	giveMentorshipFirstWinAward(shareProperty, pPersonData, pRecvData->nPost);
}

/// purpose: 师徒关系导师的学徒列表					-- 来自客户端
void MentorshipService::onClientMentorshipMasterPrenticeList(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	if (pszData == NULL || nLen != sizeof(SMsgMentorshipMaster_PrenticeList))
		return;

	SMsgMentorshipMaster_PrenticeList *pRecvData = (SMsgMentorshipMaster_PrenticeList*)pszData;

	SMsgMentorshipMaster_PrenticeList sendData;
	sendData.nCount = 0;

	obuf2048 obNode;
	TMap_PersonMentorshipIt it = m_MasterList.find(pRecvData->dwMasterPDBID);
	if (it != m_MasterList.end())
	{
		list<DWORD>::iterator itTmp = (*it).second.begin();
		for (; itTmp != (*it).second.end(); ++itTmp)
		{
			CMentorship *pMentorship = findMentorship((*itTmp));
			if (pMentorship != NULL)
			{
				SMentorshipInfo mentorshipInfo = pMentorship->getMentorshipInfo();

				SMsgMentorshipMaster_PrenticeNode sendNode;
				sendNode.dwMasterPDBID = mentorshipInfo.dwMasterPDBID;
				sendNode.dwPDBID = mentorshipInfo.dwPrenticePDBID;
				sstrcpyn(sendNode.szName, mentorshipInfo.szPrenticeName, sizeof(sendNode.szName));
				sendNode.nFaceID = mentorshipInfo.nFaceID;
				sendNode.bIsOnline = isOnline(mentorshipInfo.dwPrenticePDBID);
				sendNode.nRelationType = mentorshipInfo.nRelationType;
				sendNode.nState = mentorshipInfo.nState;

				sendData.nCount += 1;
				obNode.push_back(&sendNode, sizeof(sendNode));
			}
		}
	}

	obuf2048 ob;
	fillMentorshipData(ob, MSG_ENDPOINT_CLIENT, MSG_MENTORSHIP_MASTER_PRENTICELIST);
	ob.push_back(&sendData, sizeof(sendData));
	ob.push_back(obNode.data(), obNode.size());
	g_EHelper.SendDataToPerson(shareProperty.dwPDBID, ob.data(), ob.size());
}

/// purpose: 申请导师
void MentorshipService::applyMentorshipMaster(SSharePersonProperty &shareProperty, DWORD dwDesPDBID, BYTE nApplyPost, BYTE nRelationType)
{
	if (nRelationType != emMentorshipType_Formal && nRelationType != emMentorshipType_Intern)
		return;

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
		return;

	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

	SSharePersonProperty desShareProperty = pShareReceiver->GetSharePerson(dwDesPDBID);
	if (desShareProperty.dwPDBID != dwDesPDBID)
		return;

	if (nRelationType == emMentorshipType_Formal)
	{
		CMentorship *pMentorship = findMentorshipByPDBID(shareProperty.dwPDBID, desShareProperty.dwPDBID);
		if (pMentorship == NULL || pMentorship->getMentorshipRelationType() == emMentorshipType_Formal)
			return;

		if (pMentorship->getMentorshipState() == emMentorshipState_Dismissing)
		{
			g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_DISMISS_STATE, "");
			return;
		}

		int nFormalCount = getMentorshipFormalCount(desShareProperty.dwPDBID, emMentorshipPost_Prentice);
		if (nFormalCount >= pConfig->nFormalMasterCount)
		{
			g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_DES_FORMAL_MASTER_FULL, desShareProperty.szName);
			return;
		}

		int nDesFormalCount = getMentorshipFormalCount(shareProperty.dwPDBID, emMentorshipPost_Master);
		if (nDesFormalCount >= pConfig->nFormalPrenticeCount)
		{
			return;
		}
	}
	else
	{
		if (shareProperty.wLevel < pConfig->nMasterNeedLevel)
			return;

		if (desShareProperty.wLevel < pConfig->nPrenticeNeedLevel)
			return;

		if (isHaveMentorship(shareProperty.dwPDBID, dwDesPDBID) == true)
			return;

		int nInternCount = getMentorshipInternCount(shareProperty.dwPDBID, emMentorshipPost_Master);
		if (nInternCount >= pConfig->nInternPrenticeCount)
			return;

		int nDesInternCount = getMentorshipInternCount(desShareProperty.dwPDBID, emMentorshipPost_Prentice);
		if (nDesInternCount >= pConfig->nInternMasterCount)
		{
			g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_DES_FORMAL_PRENTICE_FULL, desShareProperty.szName);
			return;
		}
	}

	obuf512 ob;
	SMsgMentorship_Apply_Anwser sendData;
	sendData.dwSrcPDBID = shareProperty.dwPDBID;
	sendData.nRelationType = nRelationType;
	sendData.nApplyPost = nApplyPost;
	sstrcpyn(sendData.szSrcName, shareProperty.szName, sizeof(sendData.szSrcName));
	fillMentorshipData(ob, MSG_ENDPOINT_CLIENT, MSG_MENTORSHIP_APPLY_ANSWER);
	ob.push_back(&sendData, sizeof(sendData));
	g_EHelper.SendDataToPerson(desShareProperty.dwPDBID, ob.data(), ob.size());

	g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_MASTER_APPLY, desShareProperty.szName);
}

/// purpose: 申请学徒					
void MentorshipService::applyMentorshipPrentice(SSharePersonProperty &shareProperty, DWORD dwDesPDBID, BYTE nApplyPost, BYTE nRelationType)
{
	if (nRelationType != emMentorshipType_Formal && nRelationType != emMentorshipType_Intern)
		return;

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
		return;

	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

	SSharePersonProperty desShareProperty = pShareReceiver->GetSharePerson(dwDesPDBID);
	if (desShareProperty.dwPDBID != dwDesPDBID)
		return;

	if (nRelationType == emMentorshipType_Formal)
	{
		CMentorship *pMentorship = findMentorshipByPDBID(desShareProperty.dwPDBID, shareProperty.dwPDBID);
		if (pMentorship == NULL || pMentorship->getMentorshipRelationType() == emMentorshipType_Formal)
			return;

		if (pMentorship->getMentorshipState() == emMentorshipState_Dismissing)
		{
			g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_DISMISS_STATE, "");
			return;
		}

		int nFormalCount = getMentorshipFormalCount(shareProperty.dwPDBID, emMentorshipPost_Prentice);
		if (nFormalCount >= pConfig->nFormalMasterCount)
		{
			return;
		}

		int nDesFormalCount = getMentorshipFormalCount(desShareProperty.dwPDBID, emMentorshipPost_Master);
		if (nDesFormalCount >= pConfig->nFormalPrenticeCount)
		{
			g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_DES_FORMAL_PRENTICE_FULL, desShareProperty.szName);
			return;
		}
	}
	else
	{
		if (shareProperty.wLevel < pConfig->nPrenticeNeedLevel)
			return;

		if (desShareProperty.wLevel < pConfig->nMasterNeedLevel)
			return;

		if (isHaveMentorship(shareProperty.dwPDBID, dwDesPDBID) == true)
			return;

		int nInternCount = getMentorshipInternCount(shareProperty.dwPDBID, emMentorshipPost_Prentice);
		if (nInternCount >= pConfig->nInternMasterCount)
		{
			return;
		}

		int nDesInternCount = getMentorshipInternCount(desShareProperty.dwPDBID, emMentorshipPost_Master);
		if (nDesInternCount >= pConfig->nInternPrenticeCount)
		{
			g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_DES_INTERN_PRENTICE_FULL, desShareProperty.szName);
			return;
		}
	}

	obuf512 ob;
	SMsgMentorship_Apply_Anwser sendData;
	sendData.dwSrcPDBID = shareProperty.dwPDBID;
	sendData.nRelationType = nRelationType;
	sendData.nApplyPost = nApplyPost;
	sstrcpyn(sendData.szSrcName, shareProperty.szName, sizeof(sendData.szSrcName));
	fillMentorshipData(ob, MSG_ENDPOINT_CLIENT, MSG_MENTORSHIP_APPLY_ANSWER);
	ob.push_back(&sendData, sizeof(sendData));
	g_EHelper.SendDataToPerson(desShareProperty.dwPDBID, ob.data(), ob.size());

	g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_PRENTICE_APPLY, desShareProperty.szName);
}

/// purpose: 申请导师回答					
void MentorshipService::applyMentorshipMasterAnswer(SSharePersonProperty &shareProperty, SSharePersonProperty &srcShareProperty, BYTE nApplyPost, BYTE nRelationType)
{
	if (nRelationType != emMentorshipType_Formal && nRelationType != emMentorshipType_Intern)
		return;

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
		return;

	if (nRelationType == emMentorshipType_Formal)
	{
		CMentorship *pMentorship = findMentorshipByPDBID(srcShareProperty.dwPDBID, shareProperty.dwPDBID);
		if (pMentorship == NULL || pMentorship->getMentorshipRelationType() == emMentorshipType_Formal)
			return;

		int nFormalCount = getMentorshipFormalCount(srcShareProperty.dwPDBID, emMentorshipPost_Master);
		if (nFormalCount >= pConfig->nFormalPrenticeCount)
		{
			g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_DES_FORMAL_PRENTICE_FULL, srcShareProperty.szName);
			return;
		}

		int nSrcFormalCount = getMentorshipFormalCount(shareProperty.dwPDBID, emMentorshipPost_Prentice);
		if (nSrcFormalCount >= pConfig->nFormalMasterCount)
			return;

		pMentorship->promoteInternToFormal();
	}
	else
	{
		if (shareProperty.wLevel < pConfig->nPrenticeNeedLevel)
			return;

		if (srcShareProperty.wLevel < pConfig->nMasterNeedLevel)
			return;

		if (isHaveMentorship(shareProperty.dwPDBID, srcShareProperty.dwPDBID) == true)
			return;

		int nInternCount = getMentorshipInternCount(shareProperty.dwPDBID, emMentorshipPost_Prentice);
		if (nInternCount >= pConfig->nInternMasterCount)
			return;

		int nDesInternCount = getMentorshipInternCount(srcShareProperty.dwPDBID, emMentorshipPost_Master);
		if (nDesInternCount >= pConfig->nInternPrenticeCount)
		{
			g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_DES_INTERN_PRENTICE_FULL, srcShareProperty.szName);
			return;
		}

		// 添加到
		addMentorship(srcShareProperty, shareProperty, nRelationType);
	}
}

/// purpose: 申请学徒回答				
void MentorshipService::applyMentorshipPrenticeAnswer(SSharePersonProperty &shareProperty, SSharePersonProperty &srcShareProperty, BYTE nApplyPost, BYTE nRelationType)
{
	if (nRelationType != emMentorshipType_Formal && nRelationType != emMentorshipType_Intern)
		return;

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
		return;

	if (nRelationType == emMentorshipType_Formal)
	{
		CMentorship *pMentorship = findMentorshipByPDBID(shareProperty.dwPDBID, srcShareProperty.dwPDBID);
		if (pMentorship == NULL || pMentorship->getMentorshipRelationType() == emMentorshipType_Formal)
		{
			return;
		}

		int nFormalCount = getMentorshipFormalCount(shareProperty.dwPDBID, emMentorshipPost_Master);
		if (nFormalCount >= pConfig->nFormalPrenticeCount)
		{
			return;
		}

		int nSrcFormalCount = getMentorshipFormalCount(srcShareProperty.dwPDBID, emMentorshipPost_Prentice);
		if (nSrcFormalCount >= pConfig->nFormalMasterCount)
		{
			g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_DES_FORMAL_MASTER_FULL, srcShareProperty.szName);
			return;
		}

		pMentorship->promoteInternToFormal();
	}
	else
	{
		if (shareProperty.wLevel < pConfig->nMasterNeedLevel)
			return;

		if (srcShareProperty.wLevel < pConfig->nPrenticeNeedLevel)
			return;

		if (isHaveMentorship(shareProperty.dwPDBID, srcShareProperty.dwPDBID) == true)
			return;

		int nInternCount = getMentorshipInternCount(shareProperty.dwPDBID, emMentorshipPost_Master);
		if (nInternCount >= pConfig->nInternPrenticeCount)
			return;

		int nDesInternCount = getMentorshipInternCount(srcShareProperty.dwPDBID, emMentorshipPost_Prentice);
		if (nDesInternCount >= pConfig->nInternMasterCount)
		{
			g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_DES_INTERN_MASTER_FULL, srcShareProperty.szName);
			return;
		}

		// 添加师徒
		addMentorship(shareProperty, srcShareProperty, nRelationType);
	}
}

/// purpose: 添加新的师徒关系
void MentorshipService::addMentorship(SSharePersonProperty &masterShareProperty, SSharePersonProperty &prenticeShareProperty, BYTE nRelationType)
{
	if (masterShareProperty.dwPDBID == INVALID_PDBID || prenticeShareProperty.dwPDBID == INVALID_PDBID)
	{
		return;
	}

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
	{
		return;
	}

	IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService == NULL)
    {
        return;
    }

	// 通知数据库存盘成员数据
	DBREQ_PARAM_ADDMENTORSHIP referData;
	referData.dwMasterPDBID = masterShareProperty.dwPDBID;
	sstrcpyn(referData.szMasterName, masterShareProperty.szName, sizeof(referData.szMasterName));
	referData.dwPrenticePDBID = prenticeShareProperty.dwPDBID;
	sstrcpyn(referData.szPrenticeName, prenticeShareProperty.szName, sizeof(referData.szPrenticeName));
	referData.nPrenticeFaceID = prenticeShareProperty.nFaceID;
	referData.nRelationType = nRelationType,
	referData.nPromoteFlag = 0;
	referData.nState = emMentorshipState_Normal;
	referData.dwBuildTime = (DWORD)time(NULL);

	if (pBEngineService->executeSP(GAMEDB_REQUEST_ADD_MENTORSHIP, 0, (LPCSTR)(&referData), sizeof(referData), this) == false)
	{
		ErrorLn(__FUNCTION__": GAMEDB_REQUEST_ADD_MENTORSHIP failed! ");
		return;
	}

	DBREQ_TABLE_MENTORSHIP mentorship;
	mentorship.dwMasterPDBID = masterShareProperty.dwPDBID;
	sstrcpyn(mentorship.szMasterName, masterShareProperty.szName, sizeof(mentorship.szMasterName));
	mentorship.dwPrenticePDBID = prenticeShareProperty.dwPDBID;
	sstrcpyn(mentorship.szPrenticeName, prenticeShareProperty.szName, sizeof(mentorship.szPrenticeName));
	mentorship.nPrenticeFaceID = prenticeShareProperty.nFaceID;
	mentorship.nRelationType = nRelationType,
	mentorship.nPromoteFlag = 0;
	mentorship.nState = emMentorshipState_Normal;
	mentorship.dwBuildTime = (DWORD)time(NULL);
	mentorship.dwSeverPDBID = 0;
	mentorship.dwSeverTime = 0;

	DWORD dwMentorshipID = (DWORD)getMentorshipIndex();
	CMentorship* pMentorship = new CMentorship();
	if (NULL == pMentorship)
	{
		ErrorLn(__FUNCTION__": AddMentorship pMentorship == NULL" << ",MasterID" << mentorship.dwMasterPDBID << ", PrenticeID" << mentorship.dwPrenticePDBID);
		return;
	}

	if (pMentorship->createFromDB(this, dwMentorshipID, &mentorship) == false)
	{
		ErrorLn(__FUNCTION__": AddMentorship pMentorship createFromDB failed" << ",MasterID" << mentorship.dwMasterPDBID << ", PrenticeID" << mentorship.dwPrenticePDBID);
		delete pMentorship;
		return;
	}

	m_MentorshipList[dwMentorshipID] = pMentorship;

	TMap_PersonMentorshipIt itMaster = m_MasterList.find(mentorship.dwMasterPDBID);
	if (itMaster != m_MasterList.end())
	{
		(*itMaster).second.push_back(dwMentorshipID);

		int nPFormalCount = getMentorshipFormalCount(mentorship.dwMasterPDBID, emMentorshipPost_Master);
		int nPInternCount = getMentorshipInternCount(mentorship.dwMasterPDBID, emMentorshipPost_Master);
		if (nPFormalCount >= pConfig->nFormalPrenticeCount && nPInternCount >= pConfig->nInternPrenticeCount)
		{
			g_MentorshipIssueManager.removeMasterIssue(mentorship.dwMasterPDBID);
		}
	} 
	else
	{
		list<DWORD> tempList;
		tempList.clear();
		tempList.push_back(dwMentorshipID);
		m_MasterList[mentorship.dwMasterPDBID] = tempList;

		CMentorshipPersonData *pMentorshipPerson = findMentorshipPersonData(mentorship.dwMasterPDBID);
		if (pMentorshipPerson != NULL)
		{
			pMentorshipPerson->updateDataToDB();
			updateMentorshipPersonBaseToClient(pMentorshipPerson);
		}
	}

	TMap_PersonMentorshipIt itPrentice = m_PrenticeList.find(mentorship.dwPrenticePDBID);
	if (itPrentice != m_PrenticeList.end())
	{
		(*itPrentice).second.push_back(dwMentorshipID);

		int nMFormalCount = getMentorshipFormalCount(mentorship.dwPrenticePDBID, emMentorshipPost_Prentice);
		int nMInternCount = getMentorshipInternCount(mentorship.dwPrenticePDBID, emMentorshipPost_Prentice);
		if (nMFormalCount >= pConfig->nFormalMasterCount && nMInternCount >= pConfig->nInternMasterCount)
		{
			g_MentorshipIssueManager.removePrenticeIssue(mentorship.dwPrenticePDBID);
		}
	} 
	else
	{
		list<DWORD> tempList;
		tempList.clear();
		tempList.push_back(dwMentorshipID);
		m_PrenticeList[mentorship.dwPrenticePDBID] = tempList;

		CMentorshipPersonData *pMentorshipPerson = findMentorshipPersonData(mentorship.dwPrenticePDBID);
		if (pMentorshipPerson != NULL)
		{
			pMentorshipPerson->updateDataToDB();
			updateMentorshipPersonBaseToClient(pMentorshipPerson);
		}
	}

	addMentorshipToClient(pMentorship, masterShareProperty);

	addMentorshipToClient(pMentorship, prenticeShareProperty);

	SNSHelper tmpSNSHelper;
	ISNSService *pSNSService = tmpSNSHelper.m_ptr;
	if (pSNSService != NULL)
	{
		pSNSService->addBuddy(masterShareProperty.dwUDBID, prenticeShareProperty.dwUDBID, masterShareProperty.szName, prenticeShareProperty.szName);
	}

	char szText[256] = { 0 };
	if (IsSameScene(masterShareProperty, prenticeShareProperty))
	{
		ssprintf_s(szText, sizeof(szText), "%d;%s", CHAT_CHANNEL_SYSTEM, prenticeShareProperty.szName);
		g_EHelper.showSystemMessage(masterShareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_BLUID_SUCCESS, szText);

		ssprintf_s(szText, sizeof(szText), "%d;%s", CHAT_CHANNEL_SYSTEM, masterShareProperty.szName);
		g_EHelper.showSystemMessage(prenticeShareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_BLUID_SUCCESS, szText);
	}
	else
	{
		ssprintf_s(szText, sizeof(szText), "%d;%s;%d;%d;%d", CHAT_CHANNEL_SYSTEM, prenticeShareProperty.szName, 
			prenticeShareProperty.dwWorldID, prenticeShareProperty.serverID, prenticeShareProperty.dwSceneID);
		g_EHelper.showSystemMessage(masterShareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_BLUID_SUCCESS_EX, szText);

		ssprintf_s(szText, sizeof(szText), "%d;%s;%d;%d;%d", CHAT_CHANNEL_SYSTEM, masterShareProperty.szName,
			masterShareProperty.dwWorldID, masterShareProperty.serverID, masterShareProperty.dwSceneID);
		g_EHelper.showSystemMessage(prenticeShareProperty.dwPDBID, CHAT_TIP_MENTORSHIP_BLUID_SUCCESS_EX, szText);
	}


	// 记录日志
	IOSSLogService* pOSSLogService = gSocialGlobal->getOSSLogService();
	if (pOSSLogService != NULL)
	{
		ssprintf_s(szText, sizeof(szText), a2utf8("%s拜%s[%u]为见习师傅"), mentorship.szPrenticeName , mentorship.szMasterName, mentorship.dwMasterPDBID);
		pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_MENTORSHIP, mentorship.dwPrenticePDBID, szText);
	}
}

void MentorshipService::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch(wEventID)
    {
        
    case EVENT_ORDER_UPDATE_RANK:
        {
            // 更新师徒值Rank数据
            if(pszContext == NULL || nLen != sizeof(SEventOrder_UpdateRank))
            {
                return;
            }
            updateMentorshipPersonRank((void*)pszContext);
        }
        break;

    default:
        break;
    }
}

/// purpose: 读取师徒关系列表信息
bool MentorshipService::readMentorshipList(DWORD dwPDBID)
{
	TMap_PersonMentorshipIt it = m_MasterList.find(dwPDBID);
	if (it != m_MasterList.end())
	{
		return false;
	}

	it = m_PrenticeList.find(dwPDBID);
	if (it != m_PrenticeList.end())
	{
		return false;
	}

    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        return false;
    }

    DBREQ_PARAM_READMENTORSHIPLIST refer;
    refer.dwPDBID = dwPDBID;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_MENTORSHIP, 0, (LPCSTR)&refer, sizeof(refer), this) == false)
    {
        ErrorLn(__FUNCTION__": GAMEDB_REQUEST_READ_MENTORSHIP failed!!");
        return false;
    }
    return true;
}

/// purpose: 删除师徒数据从DB
void MentorshipService::delMentorshipFromDB()
{
	IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		return;
	}
	
	list<SMentorshipDelInfo>::iterator it = m_DeleteMentorshipList.begin();
	while (it != m_DeleteMentorshipList.end())
	{
		DBREQ_PARAM_DELMENTORSHIP refer;
		refer.dwMasterPDBID = (*it).dwMasterPDBID;
		refer.dwPrenticePDBID = (*it).dwPrenticePDBID;

		if (pDBEngine->executeSP(GAMEDB_REQUEST_DEL_MENTORSHIP, 0, (LPCSTR)&refer, sizeof(refer), this) == true)
		{
			it = m_DeleteMentorshipList.erase(it);
		}
		else
		{
			++it;
		}
	}
}

/// purpose: 处理模块服务器消息
void MentorshipService::onModuleHandleMsg(DWORD dwServerID, DWORD dwMsgID, LPCSTR pszData, size_t nLen)
{
	// 服务器发过来的消息
	switch ( dwMsgID )
	{
	case MSG_MENTORSHIP_ADD_MEMBER_SHIPVAL_SO:
		{
			onSceneAddMentorshipValue(dwServerID, pszData, nLen);
		}
		break;
	default:
		{
			// 其他场景服消息
			ErrorLn(__FUNCTION__": unknown dwMsgID= " << dwMsgID);
		}
		break;
	}
}

/// purpose: 处理模块Client消息
void MentorshipService::onModuleClientMsg(DWORD dwClientID, DWORD dwMsgID, LPCSTR pszData, size_t nLen)
{
	// 在共享服取一下信息
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

	SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(dwClientID);
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	// 客户端发过来的消息
	switch ( dwMsgID )
	{
	case MSG_MENTORSHIP_PERSONDATA:
		{
			onClientMentorshipPersonData(shareProperty, pszData, nLen);
		}
		break;
	case MSG_MENTORSHIP_ISSUE:
		{
			onClientMentorshipIssue(shareProperty, pszData, nLen);
		}
		break;
	case MSG_MENTORSHIP_ISSUE_LIST:
		{
			onClientMentorshipIssueList(shareProperty, pszData, nLen);
		}
		break;
	case MSG_MENTORSHIP_APPLY:
		{
			onClientMentorshipApply(shareProperty, pszData, nLen);
		}
		break;
	case MSG_MENTORSHIP_APPLY_ANSWER:
		{
			onClientMentorshipApplyAnswer(shareProperty, pszData, nLen);
		}
		break;
	case MSG_MENTORSHIP_ISSUE_AUTOSET:
		{
			onClientMentorshipIssueAutoSet(shareProperty, pszData, nLen);
		}
		break;
	case MSG_MENTORSHIP_DISMISS:
		{
			onClientMentorshipDismiss(shareProperty, pszData, nLen);
		}
		break;
	case MSG_MENTORSHIP_MASTER_PRENTICELIST:
		{
			onClientMentorshipMasterPrenticeList(shareProperty, pszData, nLen);
		}
		break;
	case MSG_MENTORSHIP_PROMOTE:
		{

			onClientMentorshipPromote(shareProperty, pszData, nLen);
		}
		break;
	case MSG_MENTORSHIP_REQUEST_FIRST_AWARD:
		{
			onClientMentorshipReqFirstAward(shareProperty, pszData, nLen);
		}
		break;
	default:
		{
			// 其他消息
			ErrorLn(__FUNCTION__": unknown dwMsgID= " << dwMsgID);
		}
		break;
	}
}

/// purpose: 获取师徒个人数据
SMentorshipPerson MentorshipService::getMentorshipPersonInfo(CMentorshipPersonData* pMentorshipPerson)
{
	SMentorshipPerson sMentorshipPerson;

	sMentorshipPerson.dwPDBID = pMentorshipPerson->getPDBID();
	sMentorshipPerson.nAutoIssueMFlag = pMentorshipPerson->getNumProp(em_MshipPersonProp_AutoIssueMFlag);
	sMentorshipPerson.nAutoIssuePFlag = pMentorshipPerson->getNumProp(em_MshipPersonProp_AutoIssuePFlag);
	sMentorshipPerson.nMSumShipVal = pMentorshipPerson->getNumProp(em_MshipPersonProp_MSumShipVal);
	sMentorshipPerson.nMWeekShipVal = pMentorshipPerson->getNumProp(em_MshipPersonProp_MWeekShipVal);
	sMentorshipPerson.nMLastWeekShipVal = pMentorshipPerson->getNumProp(em_MshipPersonProp_MLastWeekShipVal);
	sMentorshipPerson.nMLastSVOrder = pMentorshipPerson->getNumProp(em_MshipPersonProp_MLastSVOrder);
	sMentorshipPerson.nMRecordTime = pMentorshipPerson->getNumProp(em_MshipPersonProp_MRecordTime);

	sMentorshipPerson.nPSumShipVal = pMentorshipPerson->getNumProp(em_MshipPersonProp_PSumShipVal);
	sMentorshipPerson.nPWeekShipVal = pMentorshipPerson->getNumProp(em_MshipPersonProp_PWeekShipVal);
	sMentorshipPerson.nPLastWeekShipVal = pMentorshipPerson->getNumProp(em_MshipPersonProp_PLastWeekShipVal);
	sMentorshipPerson.nPLastSVOrder = pMentorshipPerson->getNumProp(em_MshipPersonProp_PLastSVOrder);
	sMentorshipPerson.nPRecordTime = pMentorshipPerson->getNumProp(em_MshipPersonProp_PRecordTime);

	sMentorshipPerson.nMFWinAwardPreCount = pMentorshipPerson->getNumProp(em_MshipPersonProp_MFWinAwardPreCount);
	sMentorshipPerson.nMFWinAwardCount = pMentorshipPerson->getNumProp(em_MshipPersonProp_MFWinAwardCount);
	sMentorshipPerson.nMFWinAwardTime = pMentorshipPerson->getNumProp(em_MshipPersonProp_MFWinAwardTime);

	sMentorshipPerson.nPFWinAwardPreCount = pMentorshipPerson->getNumProp(em_MshipPersonProp_PFWinAwardPreCount);
	sMentorshipPerson.nPFWinAwardCount = pMentorshipPerson->getNumProp(em_MshipPersonProp_PFWinAwardCount);
	sMentorshipPerson.nPFWinAwardTime = pMentorshipPerson->getNumProp(em_MshipPersonProp_PFWinAwardTime);	

	return sMentorshipPerson;
}

/// purpose: 查找师徒
CMentorship* MentorshipService::findMentorship(DWORD dwMentorshipID)
{
    TMap_MentorshipIt it = m_MentorshipList.find(dwMentorshipID);
    if (it == m_MentorshipList.end())
    {
        return NULL;
    }

    return (*it).second;
}

/// purpose: 查找师徒数据
CMentorship* MentorshipService::findMentorshipByPDBID(DWORD dwMasterPDBID, DWORD dwPrenticePDBID)
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

CMentorshipPersonData* MentorshipService::findMentorshipPersonData(DWORD dwPDBID)
{
    TMap_MentorshipPersonDataIt it = m_PersonDataList.find(dwPDBID);
    if (it == m_PersonDataList.end())
    {
        return NULL;
    }

    return (*it).second;
}

/// purpose: 清除个人师徒数据
void MentorshipService::removeMentorshipPersonData(DWORD dwPDBID)
{
	TMap_MentorshipPersonDataIt it = m_PersonDataList.find(dwPDBID);
	if ( it == m_PersonDataList.end() )
	{
		return;
	}

	(*it).second->release();

	m_PersonDataList.erase(it);
}

/// purpose: 清除师徒数据
void MentorshipService::removeMentorship(DWORD dwMenorshipID)
{
	TMap_MentorshipIt it = m_MentorshipList.find(dwMenorshipID);
	if (it == m_MentorshipList.end())
	{
		return;
	}

	// 移除师傅列表
	CMentorship *pMentorship = (*it).second;
	TMap_PersonMentorshipIt itMaster = m_MasterList.find(pMentorship->getMasterPDBID());
	if (itMaster != m_MasterList.end())
	{
		list<DWORD>::iterator tmpIt = (*itMaster).second.begin();
		while (tmpIt != (*itMaster).second.end())
		{
			if ((*tmpIt) == dwMenorshipID)
			{
				tmpIt = (*itMaster).second.erase(tmpIt);
			}
			else
			{
				++tmpIt;
			}
		}

		if ((*itMaster).second.size() == 0)
		{
			m_MasterList.erase(itMaster);
			CMentorshipPersonData *pMentorshipPerson = findMentorshipPersonData(pMentorship->getMasterPDBID());
			if (pMentorshipPerson != NULL)
			{
				updateMentorshipPersonBaseToClient(pMentorshipPerson);
			}
		}
	}

	// 移除学徒列表
	TMap_PersonMentorshipIt itPrentice = m_PrenticeList.find(pMentorship->getPrenticePDBID());
	if (itPrentice != m_PrenticeList.end())
	{
		list<DWORD>::iterator tmpIt = (*itPrentice).second.begin();
		while (tmpIt != (*itPrentice).second.end())
		{
			if ((*tmpIt) == dwMenorshipID)
			{
				tmpIt = (*itPrentice).second.erase(tmpIt);
			}
			else
			{
				++tmpIt;
			}
		}

		if ((*itPrentice).second.size() == 0)
		{
			m_PrenticeList.erase(itPrentice);
			CMentorshipPersonData *pMentorshipPerson = findMentorshipPersonData(pMentorship->getPrenticePDBID());
			if (pMentorshipPerson != NULL)
			{
				updateMentorshipPersonBaseToClient(pMentorshipPerson);
			}
		}
	}

	m_MentorshipList.erase(it);
}

/// purpose: 更新全部师徒数据到场景服
bool MentorshipService::refreshMentorshipToScene(CSID dwServerID)
{
	// 通知该场景服删除原有师徒数据
	obuf512 obuf;
	SMsgMentorship_RemoveAllShip_OS sendData;
	sendData.bFlag = true;
	fillMentorshipData(obuf, MSG_ENDPOINT_SCENE, MSG_MENTORSHIP_REMOVEALLSHIP_OS);
	obuf.push_back(&sendData, sizeof(sendData));
	g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size(), dwServerID);

	// 把师徒的所有数据发到场景服
	TMap_MentorshipIt it;
	for (it = m_MentorshipList.begin(); it != m_MentorshipList.end(); ++it)
	{
		CMentorship *pMentorship = (*it).second;
		pMentorship->updateMentorshipToScene(dwServerID);
	}

	return true;
}

/// purpose: 获取师徒个人数据接口
IMentorshipPerson* MentorshipService::getMentorshipPerson(DWORD dwPDBID)
{
    return (IMentorshipPerson*)findMentorshipPersonData(dwPDBID);
}

/// purpose: 是否需要加载师徒关系fromDB
bool MentorshipService::IsNeedLoadMentorshipFromDB(DWORD dwPDBID)
{
	TMap_PersonMentorshipIt itMaster = m_MasterList.find(dwPDBID);
	if (itMaster == m_MasterList.end() || (*itMaster).second.size() == 0)
	{
		return true;
	}

	TMap_PersonMentorshipIt itPrentice = m_PrenticeList.find(dwPDBID);
	if (itPrentice == m_PrenticeList.end() || (*itPrentice).second.size() == 0)
	{
		return true;
	}

	return false;
}

bool MentorshipService::isSameScene(CMentorship * pMentorship)
{
	IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
	if (!pShareReceiver)
		return false;

	PDBID mPdbid = pMentorship->getMasterPDBID();
	SSharePersonProperty mProperty = pShareReceiver->GetSharePerson(mPdbid);

	PDBID pPdbid = pMentorship->getPrenticePDBID();
	SSharePersonProperty pProperty = pShareReceiver->GetSharePerson(pPdbid);

	
	return IsSameScene(mProperty, pProperty);
}

void MentorshipService::OnLogin(ISharePerson * pSharePerson, ELoginMode nLineType)
{
    if (pSharePerson == NULL)
    {
        return;
    }
    PDBID PersonDBID = pSharePerson->GetNumProp(PROPERTY_ID);

	if (g_EHelper.IsComputer(PersonDBID) == true)
	{
		return;
	}

	g_MentorshipIssueManager.addIssueOnlineMember(PersonDBID);

	CMentorshipPersonData *pMentorshipPerson = findMentorshipPersonData(PersonDBID);
	if (pMentorshipPerson != NULL)
	{
		updateMentorshipPersonToClient(pMentorshipPerson);
		return;
	}

	if (IsNeedLoadMentorshipFromDB(PersonDBID) == true)
	{
		readMentorshipList(PersonDBID);
	}	

	IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		return;
	}

	DBREQ_PARAM_READMENTORSHIPPERSON refer;
	refer.dwPDBID = PersonDBID;

	if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_MENTORSHIPPERSON, 0, (LPCSTR)&refer, sizeof(refer), this) == false)
	{
		ErrorLn(__FUNCTION__": GAMEDB_REQUEST_READ_MENTORSHIPPERSON failed!! PDBID =" << PersonDBID);
	}

	if (nLineType == elogin_online)
	{
		processMemberLoginAndLogout(pSharePerson->GetUDBID(), PersonDBID, true);
	}
}

void MentorshipService::OnLogout(ISharePerson * pSharePerson, ELogoutMode nLineType)
{
	if (nLineType != elogout_offline)
	{
		return;
	}

	if (pSharePerson == NULL)
	{
		return;
	}

	PDBID PersonDBID = pSharePerson->GetNumProp(PROPERTY_ID);

	if (g_EHelper.IsComputer(PersonDBID) == true)
	{
		return;
	}

	g_MentorshipIssueManager.removeIssueOnlineMember(PersonDBID);

	removeMentorshipPersonData(PersonDBID);

	processMemberLoginAndLogout(pSharePerson->GetUDBID(), PersonDBID, false);
}

void MentorshipService::OnPre_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason)
{
    return;
}

void MentorshipService::OnPost_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason)
{
    PDBID PersonDBID = pSharePerson->GetNumProp(PROPERTY_ID);
    if (PersonDBID == INVALID_PDBID)
    {
        return;
    }

    // 属性更新
    switch(nUpdateReason)
    {
	case SHAREUPDATE_REASON_CHANGE_FACEID:
		{

		}
		break;
	case SHAREUPDATE_REASON_CHANGE_ACTORNAME:
		{
			changeMentorshipPlayerName(PersonDBID, nUpdateReason, pSharePerson);
		}
		break;
    default:
        break;
    }

    return;
}

////////////////////////////////ITransmitHandler//////////////////////////////////////////
/** 消息处理
@param server	源服务器ID
@param uMsgID	消息ID
@param head		消息头
@param data		消息
@param len		消息长度
*/
void MentorshipService::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
{
    MentorshipHelper mentorshipHelper;
    IMentorshipService *pMentorshipService = mentorshipHelper.m_ptr;
    if (pMentorshipService == NULL)
    {
        ErrorLn(__FUNCTION__": server ="<<server<<" uMsgID ="<<uMsgID);
        return;
    }

	pMentorshipService->handleServerMsg( server, *head, data, len );
}

////////////////////////////////IMessageHandler//////////////////////////////////////////
/** 消息处理
@param msg 网络消息
*/
void MentorshipService::onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len)
{
    MentorshipHelper mentorshipHelper;
    IMentorshipService *pMentorshipService = mentorshipHelper.m_ptr;
    if (pMentorshipService == NULL)
    {
        ErrorLn(__FUNCTION__": clientID ="<<clientID<<" uMsgID ="<<uMsgID);
        return;
    }

    pMentorshipService->handleClientMsg( clientID, *head, data, len );
}


void MentorshipService::handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len)
{
    // 服务器转发过来的消息
    if (data == NULL || len < sizeof(SMsgMentorshipSubMsg))
    {
        return;
    }
    SMsgMentorshipSubMsg * pMsg = (SMsgMentorshipSubMsg *)(data);

    LPSTR pMsgData = (LPSTR)data + sizeof(SMsgMentorshipSubMsg);
    size_t nNewlen = len - sizeof(SMsgMentorshipSubMsg);    

    // 服务器发过来的消息
    switch ( head.byKeyAction )
    {
    case emMsgMentorship_Module:
        {
            onModuleHandleMsg(serverID, pMsg->dwMsgCode, pMsgData, nNewlen);
        }
        break;
    default:
        {
            // 其他场景服消息
            ErrorLn(__FUNCTION__": unknown byKeyAction="<< head.byKeyAction);
        }
    }
}

void MentorshipService::handleClientMsg(DWORD client, SNetMsgHead head, void * data, size_t len)
{
    if (data == NULL || len < sizeof(SMsgMentorshipSubMsg))
    {
        return;
    }
    SMsgMentorshipSubMsg * pMsg = (SMsgMentorshipSubMsg *)(data);

    LPSTR pMsgData = (LPSTR)data + sizeof(SMsgMentorshipSubMsg);
    size_t nNewlen = len - sizeof(SMsgMentorshipSubMsg);


    // 服务器发过来的消息
    switch ( head.byKeyAction )
    {
    case emMsgMentorship_Module:
        {
            onModuleClientMsg(client, pMsg->dwMsgCode, pMsgData, nNewlen);
        }
        break;
    default:
        {
            // 其他场景服消息
            ErrorLn(__FUNCTION__": unknown byKeyAction= "<< head.byKeyAction);
        }
    }
}

/**
@name         : 通知服务器列表已经更新
@brief        : 服务器列表更新只会在有服务器进入退出时触发
@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
*/
void MentorshipService::onServerListUpdated()
{

}

/** 通知服务器信息更新
@param nState		: SERVER_STATE
@param pServerData	: ServerData
@brief        : 例如服务器人数变化等等
*/
void MentorshipService::onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{
    ServerData *pInfo = (ServerData *)pServerData;
    if (nState == SERVER_CONNECTED && pInfo->wServerType == MSG_ENDPOINT_SCENE)
    {
        refreshMentorshipToScene(ServerID);
    }
    else if (nState == SERVER_CONNECTED && pInfo->wServerType == MSG_ENDPOINT_SOCIAL)
    {
        refreshMentorshipToScene();
    }
    return;
}

void MentorshipService::updateMentorshipPersonRank(void* pEventData)
{
    SEventOrder_UpdateRank* pEvent = (SEventOrder_UpdateRank*)pEventData;
    if (pEvent->ActorID == INVALID_PDBID)
    {
        return;
    }
}

/// purpose: 是否有该学徒
bool MentorshipService::isHaveMentorshipPrentice(DWORD dwPDBID, DWORD dwPrenticePDBID)
{
	TMap_PersonMentorshipIt it = m_MasterList.find(dwPDBID);
	if (it != m_MasterList.end())
	{
		for (list<DWORD>::iterator itTmp = (*it).second.begin(); itTmp != (*it).second.end(); ++itTmp)
		{
			CMentorship *pMentorship = findMentorship((*itTmp));
			if (pMentorship != NULL && pMentorship->getPrenticePDBID() == dwPrenticePDBID)
			{
				return true;
			}
		}
	}

	return false;
}

/// purpose: 是否有该导师
bool MentorshipService::isHaveMentorshipMaster(DWORD dwPDBID, DWORD dwMasterPDBID)
{
	TMap_PersonMentorshipIt it = m_PrenticeList.find(dwPDBID);
	if (it != m_PrenticeList.end())
	{
		for (list<DWORD>::iterator itTmp = (*it).second.begin(); itTmp != (*it).second.end(); ++itTmp)
		{
			CMentorship *pMentorship = findMentorship((*itTmp));
			if (pMentorship != NULL && pMentorship->getMasterPDBID() == dwMasterPDBID)
			{
				return true;
			}
		}
	}

	return false;
}

// 师傅存在师徒关系
bool MentorshipService::isHaveMentorship(DWORD dwSrcPDBID, DWORD dwDesPDBID)
{
	if ( isHaveMentorshipPrentice(dwSrcPDBID, dwDesPDBID) )
		return true;

	if ( isHaveMentorshipMaster(dwSrcPDBID, dwDesPDBID) )
		return true;

	return false;
}

/// purpose: 玩家是否在线
bool MentorshipService::isOnline(DWORD dwPDBID)
{
	// 在共享服取一下信息
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver==NULL)
	{
		return false;
	}

	SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
	if ( shareProperty.dwPDBID == dwPDBID)
	{
		return true;
	}

	return false;
}

/// purpose: 获取师徒正式关系个数
int MentorshipService::getMentorshipFormalCount(DWORD dwPDBID, BYTE nMentorshipPost)
{	
	if (nMentorshipPost == emMentorshipPost_Master)
	{
		TMap_PersonMentorshipIt it = m_MasterList.find(dwPDBID);
		return getMentorshipCount(it, m_MasterList.end(), dwPDBID, emMentorshipType_Formal);
	}
	else if (nMentorshipPost == emMentorshipPost_Prentice)
	{
		TMap_PersonMentorshipIt it = m_PrenticeList.find(dwPDBID);
		return getMentorshipCount(it, m_PrenticeList.end(), dwPDBID, emMentorshipType_Formal);
	}

	return 0;
}

/// purpose: 获取师徒见习关系个数
int MentorshipService::getMentorshipInternCount(DWORD dwPDBID, BYTE nMentorshipPost)
{
	if (nMentorshipPost == emMentorshipPost_Master)
	{
		TMap_PersonMentorshipIt it = m_MasterList.find(dwPDBID);
		return getMentorshipCount(it, m_MasterList.end(), dwPDBID, emMentorshipType_Intern);
	}
	else if (nMentorshipPost == emMentorshipPost_Prentice)
	{
		TMap_PersonMentorshipIt it = m_PrenticeList.find(dwPDBID);
		return getMentorshipCount(it, m_PrenticeList.end(), dwPDBID, emMentorshipType_Intern);
	}

	return 0;
}

/// purpose: 获取师徒关系个数
int MentorshipService::getMentorshipCount(TMap_PersonMentorshipIt it, TMap_PersonMentorshipIt itEnd, DWORD dwPDBID, BYTE nRelationType)
{
	if (it == itEnd)
	{
		return 0;
	}

	int nCount = 0;
	list<DWORD>::iterator tmpIt = (*it).second.begin();
	for (; tmpIt != (*it).second.end(); ++tmpIt)
	{
		CMentorship *pMentorship = findMentorship((*tmpIt));
		if (pMentorship == NULL)
		{
			continue;
		}

		if (pMentorship->getMentorshipRelationType() == nRelationType && (pMentorship->getMasterPDBID() == dwPDBID || pMentorship->getPrenticePDBID() == dwPDBID))
		{
			nCount += 1;
		}
	}

	return nCount;
}

// 自动添加到发布消息
void MentorshipService::autoAddMentorshipIssue(SSharePersonProperty &shareProperty, CMentorshipPersonData *pMentorshipPerson)
{
	if (pMentorshipPerson == NULL)
	{
		return;
	}

	if (shareProperty.dwPDBID != pMentorshipPerson->getPDBID())
	{
		return;
	}

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
	{
		return;
	}

	SMentorshipIssue mentorshipIssue;
	mentorshipIssue.dwPDBID = shareProperty.dwPDBID;

	string sTmp = a2utf8("");
	sstrcpyn(mentorshipIssue.szIssueInfo, sTmp.c_str(), sizeof(mentorshipIssue.szIssueInfo));

	if (shareProperty.wLevel >= pConfig->nMasterNeedLevel)
	{
		int nFormalCount = getMentorshipFormalCount(shareProperty.dwPDBID, emMentorshipPost_Master);
		int nInternCount = getMentorshipInternCount(shareProperty.dwPDBID, emMentorshipPost_Master);
		if (nFormalCount < pConfig->nFormalPrenticeCount || nInternCount < pConfig->nInternPrenticeCount)
		{
			mentorshipIssue.nIssueType = emMentorshipIssueType_Master;
			g_MentorshipIssueManager.addMasterIssue(&mentorshipIssue);
			g_MentorshipIssueManager.addMIssueOnlineMember(shareProperty.dwPDBID);
		}
	}
	
	if(shareProperty.wLevel >= pConfig->nPrenticeNeedLevel)
	{
		int nFormalCount = getMentorshipFormalCount(shareProperty.dwPDBID, emMentorshipPost_Prentice);
		int nInternCount = getMentorshipInternCount(shareProperty.dwPDBID, emMentorshipPost_Prentice);
		if (nFormalCount < pConfig->nFormalMasterCount || nInternCount < pConfig->nInternMasterCount)
		{
			mentorshipIssue.nIssueType = emMentorshipIssueType_Prentice;
			g_MentorshipIssueManager.addPrenticeIssue(&mentorshipIssue);
			g_MentorshipIssueManager.addPIssueOnlineMember(shareProperty.dwPDBID);
		}
	}
}

/// purpose: 发放师徒首胜奖励
void MentorshipService::giveMentorshipFirstWinAward(SSharePersonProperty &shareProperty, CMentorshipPersonData *pMentorshipPerson, BYTE nMentorshipPost)
{
	if (pMentorshipPerson == NULL)
	{
		return;
	}

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
	{
		return;
	}

	if (nMentorshipPost != emMentorshipPost_Master && nMentorshipPost != emMentorshipPost_Prentice)
		return;

	DWORD dwPrePropID = em_MshipPersonProp_MFWinAwardPreCount;
	DWORD dwPropID = em_MshipPersonProp_MFWinAwardCount;
	int nMaxCount = pConfig->nMFWinAwardCount;

	if (nMentorshipPost == emMentorshipPost_Master)
	{
		dwPrePropID = em_MshipPersonProp_PFWinAwardPreCount;
		dwPropID = em_MshipPersonProp_PFWinAwardCount;
		nMaxCount = pConfig->nPFWinAwardCount;
	}

	// 领取首胜奖励
	int nFWinPreCount = pMentorshipPerson->getNumProp(dwPrePropID);
	if (nFWinPreCount <= 0)
		return;

	int nFWinAwardCount = pMentorshipPerson->getNumProp(dwPropID);
	if (nFWinAwardCount < 0 || nFWinAwardCount >= nMaxCount)
		return;

	int nTmpCount = nMaxCount - nFWinAwardCount;
	nTmpCount = (nFWinPreCount > nTmpCount) ? nTmpCount : nFWinPreCount;
	if (pMentorshipPerson->setNumProp(dwPrePropID, 0, false) == true)
	{
		if (pMentorshipPerson->setNumProp(dwPropID, nTmpCount, true) == true)
		{
			for (int i=0; i < nTmpCount; ++i)
			{
				sendMentorshipFirstWinAward(shareProperty, nMentorshipPost);
			}
			updateMentorshipPersonBaseToClient(pMentorshipPerson);
		}
	}
}

/// purpose: 发放师徒首胜奖励
void MentorshipService::sendMentorshipFirstWinAward(SSharePersonProperty &shareProperty, BYTE nMentorshipPost)
{
	obuf ob;
	SMsgMentorship_GiveFirstWin_SO sendData;
	sendData.dwPDBID = shareProperty.dwPDBID;
	sendData.nMentorhsipPost = nMentorshipPost;
	fillMentorshipData(ob, MSG_ENDPOINT_SCENE, MSG_MENTORSHIP_GIVE_FIRSTEWIN_OS);
	ob.push_back(&sendData, sizeof(sendData));
	g_EHelper.SendDataToSceneSvr(ob.data(), ob.size(), shareProperty.serverID);
}

/// purpose: 处理玩家上下线通知
void MentorshipService::processMemberLoginAndLogout(DWORD dwUDBID, DWORD dwPDBID, bool bFlag)
{
	// 通知徒弟
	TMap_PersonMentorshipIt it = m_MasterList.find(dwPDBID);
	if (it != m_MasterList.end())
	{
		for (list<DWORD>::iterator tmpIt = (*it).second.begin(); tmpIt != (*it).second.end(); ++tmpIt)
		{
			CMentorship *pMentorship = findMentorship((*tmpIt));
			if (pMentorship != NULL)
			{
				sendMemberLoginAndLogout(pMentorship->getPrenticePDBID(), dwPDBID, bFlag);
				if (bFlag)
				{
					char szText[256] = {0};
					LPCSTR pMasterName = pMentorship->getMasterName();

					if (isSameScene(pMentorship))
					{
						ssprintf_s(szText, sizeof(szText), "%d;%s;%lu", CHAT_CHANNEL_SYSTEM, pMasterName, dwUDBID);
						g_EHelper.showSystemMessage(pMentorship->getPrenticePDBID(), CHAT_TIP_MENTORSHIP_MASTER_LOGIN, szText);
					}
					else
					{
						// 不在一个场景,发送切换线路提示
						IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
						if (pShareReceiver)
						{
							PDBID mPdbid = pMentorship->getMasterPDBID();
							SSharePersonProperty mProperty = pShareReceiver->GetSharePerson(mPdbid);

							ssprintf_s(szText, sizeof(szText), "%d;%s;%lu;%d;%d;%d", CHAT_CHANNEL_SYSTEM, pMasterName, dwUDBID,
								mProperty.dwWorldID, mProperty.serverID, mProperty.dwSceneID);
							g_EHelper.showSystemMessage(pMentorship->getPrenticePDBID(), CHAT_TIP_MENTORSHIP_MASTER_LOGIN_EX, szText);
						}
					}
				}
			}
		}
	}

	// 通知师傅
	it = m_PrenticeList.find(dwPDBID);
	if (it != m_PrenticeList.end())
	{
		for (list<DWORD>::iterator tmpIt = (*it).second.begin(); tmpIt != (*it).second.end(); ++tmpIt)
		{
			CMentorship *pMentorship = findMentorship((*tmpIt));
			if (pMentorship != NULL)
			{
				sendMemberLoginAndLogout(pMentorship->getMasterPDBID(), dwPDBID, bFlag);
				if (bFlag)
				{
					char szText[256] = {0};
					LPCSTR pPrenticeName = pMentorship->getPrenticeName();

					if (isSameScene(pMentorship))
					{
						ssprintf_s(szText, sizeof(szText), "%d;%s;%lu", CHAT_CHANNEL_SYSTEM, pPrenticeName, dwUDBID);
						g_EHelper.showSystemMessage(pMentorship->getMasterPDBID(), CHAT_TIP_MENTORSHIP_PRENTICE_LOGIN, szText);
					}
					else
					{
						// 不在一个场景,发送切换线路提示
						IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
						if (pShareReceiver)
						{
							SSharePersonProperty pProperty = pShareReceiver->GetSharePerson(pMentorship->getPrenticePDBID());

							ssprintf_s(szText, sizeof(szText), "%d;%s;%lu;%d;%d;%d", CHAT_CHANNEL_SYSTEM, pPrenticeName, dwUDBID,
								pProperty.dwWorldID, pProperty.serverID, pProperty.dwSceneID);
							g_EHelper.showSystemMessage(pMentorship->getMasterPDBID(), CHAT_TIP_MENTORSHIP_PRENTICE_LOGIN_EX, szText);
						}
					}
				}
			}
		}
	}
}

/// purpose: 处理玩家上下线通知
void MentorshipService::sendMemberLoginAndLogout(DWORD dwPDBID, DWORD dwDesPDBID, bool bFlag)
{
	obuf ob;
	SMsgMentorship_Member_Login_Logout sendData;
	sendData.dwPDBID = dwDesPDBID;
	sendData.bFlag = bFlag;
	fillMentorshipData(ob, MSG_ENDPOINT_CLIENT, MSG_MENTORSHIP_MEMBER_LOGIN_LOGOUT);
	ob.push_back(&sendData, sizeof(sendData));
	g_EHelper.SendDataToPerson(dwPDBID, ob.data(), ob.size());
}

void MentorshipService::ExecSP(DWORD cmdID,LPCSTR pData, int len)
{
    if (pData == NULL || len <= 0)
    {
        return;
    }
    IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return;
    }

    obuf oBuff;
    oBuff.push_back(pData, len);

    if (pDBEngine->executeSP(cmdID, 0, oBuff.data(), SIZE_INT(oBuff.size()), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->ExecuteSP()  cmdID = "<<cmdID<<" failed");
    }
}

void MentorshipService::OnTimer(unsigned long dwTimerID)
{
	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
	{
		return;
	}

	if (dwTimerID == MENTORSHIP_CLEAR_MENTORSHIP_LIST_TIMER)
	{
		vector<CMentorship*> tmpDelList;
		tmpDelList.clear();

		TMap_MentorshipIt it = m_MentorshipList.begin();
		for(; it != m_MentorshipList.end(); ++it)
		{
			if ((*it).second->getMentorshipState() == emMentorshipState_Dismissing && (*it).second->getMentorshipSeverTime() + pConfig->nServerTimeLen <= (DWORD)time(NULL))
			{
				tmpDelList.push_back((*it).second);
			}
		}

		size_t nLen = tmpDelList.size();
		for (size_t i=0; i < nLen; ++i)
		{
			tmpDelList[i]->dismiss();
		}
	}
}



