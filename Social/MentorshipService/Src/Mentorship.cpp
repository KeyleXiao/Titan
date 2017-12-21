#include "stdafx.h"
#include "Mentorship.h"
#include "MentorshipService.h"
#include "ISocialGlobal.h"
#include "ExternalHelper.h"
#include "IShareServer.h"
#include "ISchemeCenter.h"
#include "IOSSLogService.h"
#include "EntityDef.h"
#include "ChatHelper.h"
#include "OrderHelper.h"
#include "Mentorship_ManagedDef.h"
#include "OssLogDef.h"

using namespace Mentorship_ManagerDef;

/// purpose: 构造函数
CMentorship::CMentorship()
{
    m_pMentorshipService = NULL;
	m_dwID = 0;
	m_dwMasterPDBID = 0;
	memset(m_szMasterName, 0, sizeof(m_szMasterName));
	m_dwPrenticePDBID = 0;
	memset(m_szPrenticeName, 0, sizeof(m_szPrenticeName));
	m_nFaceID = 0;
	m_nRelationType = emMentorshipType_Invalid;
	m_nState = emMentorshipState_Normal;
	m_bPromoteFlag = false;
	m_dwBuildTime = 0;
	m_dwSeverPDBID = 0;
	m_dwSeverTime = 0;
}

/// purpose: 虚构函数
CMentorship::~CMentorship()
{

}


void CMentorship::release()
{
    delete this;
}

/// purpose: 师徒数据更新数据库
void CMentorship::updateDataToDB()
{
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService == NULL)
    {
        return;
    }

	if (m_pMentorshipService == NULL)
		return;

    DBREQ_PARAM_UPDATEMENTORSHIP referData;
	referData.dwMasterPDBID = m_dwMasterPDBID;
	sstrcpyn(referData.szMasterName, m_szMasterName, sizeof(referData.szMasterName));
	referData.dwPrenticePDBID = m_dwPrenticePDBID;
	sstrcpyn(referData.szPrenticeName, m_szPrenticeName, sizeof(referData.szPrenticeName));
	referData.nRelationType = m_nRelationType;
	referData.nState = m_nState;
	referData.nPromoteFlag = (m_bPromoteFlag == true) ? 1 : 0;
	referData.dwSeverPDBID = m_dwSeverPDBID;
	referData.dwSeverTime = m_dwSeverTime;
 
    // 更新到数据库
    m_pMentorshipService->ExecSP(GAMEDB_REQUEST_UPDATE_MENTORSHIP,(LPCSTR)&referData,sizeof(DBREQ_PARAM_UPDATEMENTORSHIP));
}

bool CMentorship::createFromDB(MentorshipService *pMentorshipService, DWORD dwMentorshipID, DBREQ_TABLE_MENTORSHIP *pDBContext)
{
    if (pMentorshipService == NULL)
    {
        ErrorLn(__FUNCTION__": pMentorshipService == NULL");
        return false;
    }

    // 设置数据
    m_pMentorshipService = pMentorshipService;
    m_dwID = dwMentorshipID;
	m_dwMasterPDBID = pDBContext->dwMasterPDBID;
    sstrcpyn(m_szMasterName, pDBContext->szMasterName, sizeof(m_szMasterName));
	m_dwPrenticePDBID = pDBContext->dwPrenticePDBID;
	sstrcpyn(m_szPrenticeName, pDBContext->szPrenticeName, sizeof(m_szPrenticeName));
	m_nFaceID = pDBContext->nPrenticeFaceID;
	m_nRelationType = pDBContext->nRelationType;
	m_nState = pDBContext->nState;
	m_bPromoteFlag = (pDBContext->nPromoteFlag == 1) ? true : false;
	m_dwBuildTime = pDBContext->dwBuildTime;
	m_dwSeverPDBID = pDBContext->dwSeverPDBID;
	m_dwSeverTime = pDBContext->dwSeverTime;

	updateMentorshipToScene();

    return true;
}

/// purpose: 获取师徒关系信息
SMentorshipInfo CMentorship::getMentorshipInfo()
{
	SMentorshipInfo mentorshipInfo;
	mentorshipInfo.dwMentorshipID = m_dwID;
	mentorshipInfo.dwMasterPDBID = m_dwMasterPDBID;
	sstrcpyn(mentorshipInfo.szMasterName, m_szMasterName, sizeof(mentorshipInfo.szMasterName));
	mentorshipInfo.dwPrenticePDBID = m_dwPrenticePDBID;
	sstrcpyn(mentorshipInfo.szPrenticeName, m_szPrenticeName, sizeof(mentorshipInfo.szPrenticeName));
	mentorshipInfo.nFaceID = m_nFaceID;
	mentorshipInfo.nRelationType = m_nRelationType;
	mentorshipInfo.nState = m_nState;
	mentorshipInfo.bPromoteFlag = m_bPromoteFlag;
	mentorshipInfo.dwBuildTime = m_dwBuildTime;
	mentorshipInfo.dwSeverPDBID = m_dwSeverPDBID;
	mentorshipInfo.dwSeverTime = m_dwSeverTime;

	return mentorshipInfo;
}

/// purpose: 获取师徒关系类型
BYTE CMentorship::getMentorshipRelationType()
{
	return m_nRelationType;
}

/// purpose: 获取师徒关系状态
BYTE CMentorship::getMentorshipState()
{
	return m_nState;
}

/// purpose: 获取师徒关系提升标识
bool CMentorship::getMentorshipPromoteFlag()
{
	return m_bPromoteFlag;
}

/// purpose: 获取师徒师傅PDBID
DWORD CMentorship::getMasterPDBID()
{
	return m_dwMasterPDBID;
}

/// purpose: 获取师徒学徒PDBID
DWORD CMentorship::getPrenticePDBID()
{
	return m_dwPrenticePDBID;
}

/// purpose: 获取师徒师傅名字
LPCSTR CMentorship::getMasterName()
{
	return m_szMasterName;
}

/// purpose: 获取师徒学徒名字
LPCSTR CMentorship::getPrenticeName()
{
	return m_szPrenticeName;
}

/// purpose: 获取师徒学徒建立时间
DWORD CMentorship::getMentorshipBuildTime()
{
	return m_dwBuildTime;
}

/// purpose: 获取解散时间
DWORD CMentorship::getMentorshipSeverTime()
{
	return m_dwSeverTime;
}

void CMentorship::onClientDismiss(DWORD dwPDBID)
{
	if (m_nState == emMentorshipState_Dismissing && dwPDBID == m_dwSeverPDBID)
	{
		return;
	}
	
	if (m_nState == emMentorshipState_Normal)
	{
		m_nState = emMentorshipState_Dismissing;
		m_dwSeverPDBID = dwPDBID;
		m_dwSeverTime = (DWORD)time(NULL);
		// 更新到数据库
		updateDataToDB();
		// 更新到各自场景服
		updateMentorshipToScene();
		// 更新到各自客户端
		updateMentorshipToClient(m_dwMasterPDBID);
		updateMentorshipToClient(m_dwPrenticePDBID);

		SMentorshipConfig *pConfig = g_EHelper.getConfig();
		if (pConfig != NULL)
		{
			char szText[256] = {0};
			ssprintf_s(szText, sizeof(szText), "%s;%d", (dwPDBID == m_dwMasterPDBID ? m_szPrenticeName : m_szMasterName), pConfig->nServerTimeLen / 3600);
			g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_MENTORSHIP_APPLY_DISMISS, szText);

			ssprintf_s(szText, sizeof(szText), "%s;%d", (dwPDBID == m_dwMasterPDBID ? m_szMasterName : m_szPrenticeName), pConfig->nServerTimeLen / 3600);
			g_EHelper.showSystemMessage((dwPDBID == m_dwMasterPDBID ? m_dwPrenticePDBID : m_dwMasterPDBID), CHAT_TIP_MENTORSHIP_OTHER_APPLY_DISMISS, szText);
		}
	}
	else if (m_nState == emMentorshipState_Dismissing)
	{
		dismiss();
	}
}

/// purpose: 更新师徒关系到场景
void CMentorship::updateMentorshipToScene(CSID dwServerID)
{
	obuf256 ob;
	SMsgMentorship_UpdateMentorship_OS sendData;
	sendData.dwMasterPDBID = m_dwMasterPDBID;
	sendData.dwPrenticePDBID = m_dwPrenticePDBID;
	sendData.nRelationType = m_nRelationType;
	sendData.nState = m_nState;
	sendData.bPromoteFlag = m_bPromoteFlag;
	sendData.dwBuildTime = m_dwBuildTime;
	sendData.dwSeverPDBID = m_dwSeverTime;
	fillMentorshipData(ob, MSG_ENDPOINT_SCENE, MSG_MENTORSHIP_UPDATE_MENTORSHIP_OS);
	ob.push_back(&sendData, sizeof(sendData));
	g_EHelper.SendDataToSceneSvr(ob.data(), ob.size(), dwServerID);
}

/// purpose: 更新师徒关系到客户端
void CMentorship::updateMentorshipToClient(DWORD dwPDBID)
{
	obuf256 ob;
	SMsgMentorship_UpdateMentorship_OC sendData;
	sendData.dwMentorshipID = m_dwID;
	sendData.nRelationType = m_nRelationType;
	sendData.nState = m_nState;
	sendData.dwSeverPDBID = m_dwSeverPDBID;
	sendData.dwSeverTime = m_dwSeverTime;

	LPCSTR pTmpName = (dwPDBID == m_dwMasterPDBID ? m_szPrenticeName : m_szMasterName);
	sstrcpyn(sendData.szName, pTmpName, sizeof(sendData.szName));

	fillMentorshipData(ob, MSG_ENDPOINT_CLIENT, MSG_MENTORSHIP_UPDATE_MENTORSHIP);
	ob.push_back(&sendData, sizeof(sendData));
	g_EHelper.SendDataToPerson(dwPDBID, ob.data(), ob.size());
}

/// purpose: 移除师徒关系到场景
void CMentorship::removeMentorshipToScene()
{
	obuf256 ob;
	SMsgMentorship_Dismiss_OS sendData;
	sendData.dwMasterPDBID = m_dwMasterPDBID;
	sendData.dwPrenticePDBID = m_dwPrenticePDBID;
	fillMentorshipData(ob, MSG_ENDPOINT_SCENE, MSG_MENTORSHIP_DISMISS_OS);
	ob.push_back(&sendData, sizeof(sendData));
	g_EHelper.SendDataToSceneSvr(ob.data(), ob.size());
}

/// purpose: 移除师徒关系到客户端
void CMentorship::removeMentorshipToClient(DWORD dwPDBID)
{
	obuf256 ob;
	SMsgMentorship_Dismiss sendData;
	sendData.dwMentorshipID = m_dwID;
	fillMentorshipData(ob, MSG_ENDPOINT_CLIENT, MSG_MENTORSHIP_DISMISS);
	ob.push_back(&sendData, sizeof(sendData));
	g_EHelper.SendDataToPerson(dwPDBID, ob.data(), ob.size());
}

/// purpose: 提升见习学徒为正式
void CMentorship::promoteInternToFormal()
{
	// 设置关系类型
	m_nRelationType = emMentorshipType_Formal;
	m_bPromoteFlag = true;
	
	// 更新到数据库
	updateDataToDB();
	// 更新到各自场景服
	updateMentorshipToScene();
	// 更新到各自客户端
	updateMentorshipToClient(m_dwMasterPDBID);
	updateMentorshipToClient(m_dwPrenticePDBID);

	g_EHelper.showSystemMessage(m_dwMasterPDBID, CHAT_TIP_MENTORSHIP_PROMOTE_PRENTICE, m_szPrenticeName);
	g_EHelper.showSystemMessage(m_dwPrenticePDBID, CHAT_TIP_MENTORSHIP_PROMOTE_PRENTICE, m_szMasterName);

	// 记录日志
	IOSSLogService* pOSSLogService = gSocialGlobal->getOSSLogService();
	if (pOSSLogService != NULL)
	{
		char szText[256] = {0};
		ssprintf_s(szText, sizeof(szText), a2utf8("%s拜%s[%u]为正式师傅"), m_szPrenticeName , m_szMasterName, m_dwMasterPDBID);
		pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_MENTORSHIP, m_dwPrenticePDBID, szText);
	}
}

void CMentorship::dismiss()
{
    // 通知数据库解散师徒关系
	IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
	if (pBEngineService != NULL)
	{
		DBREQ_PARAM_DELMENTORSHIP referData;
		referData.dwMasterPDBID = m_dwMasterPDBID;
		referData.dwPrenticePDBID = m_dwPrenticePDBID;
		m_pMentorshipService->ExecSP(GAMEDB_REQUEST_DEL_MENTORSHIP,(LPCSTR)&referData,sizeof(DBREQ_PARAM_DELMENTORSHIP));
	}

    // 从删除全局战队对象
    m_pMentorshipService->removeMentorship(m_dwID);

    // 通知客户端删除师徒相关数据
    removeMentorshipToClient(m_dwMasterPDBID);

	// 通知学徒删除师徒相关数据
	removeMentorshipToClient(m_dwPrenticePDBID);

	// 通知场景
	removeMentorshipToScene();

	// 记录日志
	IOSSLogService* pOSSLogService = gSocialGlobal->getOSSLogService();
	if (pOSSLogService != NULL)
	{
		char szText[256] = {0};
		ssprintf_s(szText, sizeof(szText), a2utf8("%s和%s[%u]解除师徒关系"), m_szPrenticeName , m_szMasterName, m_dwMasterPDBID);
		pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_MENTORSHIP, m_dwPrenticePDBID, szText);
	}

	g_EHelper.showSystemMessage(m_dwMasterPDBID, CHAT_TIP_MENTORSHIP_DISMISS_SUCCESS, m_szPrenticeName);
	g_EHelper.showSystemMessage(m_dwPrenticePDBID, CHAT_TIP_MENTORSHIP_DISMISS_SUCCESS, m_szMasterName);

    // 释放自己，注意
    release();
}

/// purpose: 更新师傅名字
void CMentorship::changeMasterName(LPCSTR szName)
{
	if (strcmp(m_szMasterName, szName) == 0)
		return;

	sstrcpyn(m_szMasterName, szName, sizeof(m_szMasterName));
	updateDataToDB();
	updateMentorshipToClient(m_dwPrenticePDBID);
}

/// purpose: 更新学徒名字
void CMentorship::changePrenticeName(LPCSTR szName)
{
	if (strcmp(m_szPrenticeName, szName) == 0)
		return;

	sstrcpyn(m_szPrenticeName, szName, sizeof(m_szPrenticeName));
	updateDataToDB();
	updateMentorshipToClient(m_dwMasterPDBID);
}



