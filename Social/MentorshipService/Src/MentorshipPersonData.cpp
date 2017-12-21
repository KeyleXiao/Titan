#include "StdAfx.h"
#include "MentorshipService.h"
#include "MentorshipPersonData.h"
#include "ExternalHelper.h"
#include "ISocialGlobal.h"
#include "IOSSLogService.h"
#include "Mentorship_ManagedDef.h"
#include "OssLogDef.h"

using namespace Mentorship_ManagerDef;

CMentorshipPersonData::CMentorshipPersonData()
{
	m_pMentorshipService = NULL;
	m_dwPDBID = 0;
	m_nMetorshipPost = emMentorshipPost_Normal;
	m_nAutoIssueMFlag = emMentorshipAutoIssueFlag_Close;
	m_nAutoIssuePFlag = emMentorshipAutoIssueFlag_Close;
	
	m_nMSumShipVal = 0;
	m_nMWeekShipVal = 0;
	m_nMLastWeekShipVal = 0;
	m_nMLastSVOrder = 0;
	m_nMRecordTime = (int)time(NULL);

	m_nPSumShipVal = 0;
	m_nPWeekShipVal = 0;
	m_nPLastWeekShipVal = 0;
	m_nPLastSVOrder = 0;
	m_nPRecordTime = (int)time(NULL);

	m_nMFWinAwardPreCount = 0;
	m_nMFWinAwardCount = 0;
	m_nMFWinAwardTime = (int)time(NULL);

	m_nPFWinAwardPreCount = 0;
	m_nPFWinAwardCount = 0;
	m_nPFWinAwardTime = (int)time(NULL);

	m_bDBPsersonDataFinal = false;

	memset(m_propAddress, 0, sizeof(m_propAddress));

	// 成员地址初始化
	m_propAddress[em_MshipPersonProp_Post] = (int*)&m_nMetorshipPost;
	m_propAddress[em_MshipPersonProp_AutoIssueMFlag] = (int*)&m_nAutoIssueMFlag;
	m_propAddress[em_MshipPersonProp_AutoIssuePFlag] = (int*)&m_nAutoIssuePFlag;
	m_propAddress[em_MshipPersonProp_MSumShipVal] = &m_nMSumShipVal;
	m_propAddress[em_MshipPersonProp_MWeekShipVal] = &m_nMWeekShipVal;
	m_propAddress[em_MshipPersonProp_MLastWeekShipVal] = &m_nMLastWeekShipVal;
	m_propAddress[em_MshipPersonProp_MLastSVOrder] = &m_nMLastSVOrder;
	m_propAddress[em_MshipPersonProp_MRecordTime] = &m_nMRecordTime;
	m_propAddress[em_MshipPersonProp_PSumShipVal] = &m_nPSumShipVal;
	m_propAddress[em_MshipPersonProp_PWeekShipVal] = &m_nPWeekShipVal;
	m_propAddress[em_MshipPersonProp_PLastWeekShipVal] = &m_nPLastWeekShipVal;
	m_propAddress[em_MshipPersonProp_PLastSVOrder] = &m_nPLastSVOrder;
	m_propAddress[em_MshipPersonProp_PRecordTime] = &m_nPRecordTime;
	m_propAddress[em_MshipPersonProp_MFWinAwardPreCount] = (int*)&m_nMFWinAwardPreCount;
	m_propAddress[em_MshipPersonProp_MFWinAwardCount] = (int*)&m_nMFWinAwardCount;
	m_propAddress[em_MshipPersonProp_MFWinAwardTime] = &m_nMFWinAwardTime;
	m_propAddress[em_MshipPersonProp_PFWinAwardPreCount] = (int*)&m_nPFWinAwardPreCount;
	m_propAddress[em_MshipPersonProp_PFWinAwardCount] = (int*)&m_nPFWinAwardCount;
	m_propAddress[em_MshipPersonProp_PFWinAwardTime] = &m_nPFWinAwardTime;

}

/// purpose: 析构函数
CMentorshipPersonData::~CMentorshipPersonData(void)
{

}

//
void CMentorshipPersonData::release()
{
	delete this;
}

 /// purpose: 获取PDBID
 DWORD CMentorshipPersonData::getPDBID()
 {
	 return m_dwPDBID;
 }

/// purpose: 取得师徒个人的整型数据
int CMentorshipPersonData::getNumProp(DWORD dwPropID)
{
	if (dwPropID < em_MshipPersonProp_Post && dwPropID >= emMshipPersonProp_NumPropCount)
	{
		return -1;
	}

	if (m_propAddress[dwPropID] == 0)
	{
		return -1;
	}

	return *(m_propAddress[dwPropID]);
}

/// purpose: 设置师徒个人的整型数据
typedef void * (CMentorshipPersonData::* LEECHDOM_CHANGE_PROC)(DWORD dwPropID, int nValue);
bool CMentorshipPersonData::setNumProp(DWORD dwPropID, int nValue, bool bSaveDB /*= false*/)
{
	static LEECHDOM_CHANGE_PROC changeproc[emMshipPersonProp_NumPropCount] =
	{
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::cover_NoNegative,		// 师徒职位
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::cover_NoNegative,		// 自动发布师傅信息标识
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::cover_NoNegative,		// 自动发布学徒信息标识
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::increment_NoNegative,	// 导师关系总值
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::increment_NoNegative,	// 导师本周关系值
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::cover_NoNegative,		// 导师上周关系值
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::cover_NoNegative,		// 导师上周关系排名
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::cover_NoNegative,		// 导师最后战斗时间
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::increment_NoNegative,	// 学徒关系总值
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::increment_NoNegative,	// 学徒本周关系值
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::cover_NoNegative,		// 学徒上周关系值
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::cover_NoNegative,		// 学徒上周关系排名
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::cover_NoNegative,		// 学徒最后战斗时间
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::cover_NoNegative,		// 导师首胜奖励可领取次数
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::increment_NoNegative,	// 导师首胜奖励次数
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::cover_NoNegative,		// 导师首胜奖励时间
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::cover_NoNegative,		// 学徒首胜奖励可领取次数
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::increment_NoNegative,	// 学徒首胜奖励次数
		(LEECHDOM_CHANGE_PROC)&CMentorshipPersonData::cover_NoNegative,		// 学徒首胜奖励时间
	};

	if (dwPropID < em_MshipPersonProp_Post || dwPropID >= emMshipPersonProp_NumPropCount || changeproc[dwPropID] == NULL)
	{
		return false;
	}

	if (m_propAddress[dwPropID] == 0)
	{
		return false;
	}
	(this->*changeproc[dwPropID])(dwPropID, nValue);

	if (bSaveDB == true && m_bDBPsersonDataFinal == true)
	{
		updateDataToDB();
	}

	return true;
}

/// purpose: 覆盖修改 非负数
void CMentorshipPersonData::cover_NoNegative(DWORD dwPropID, int nValue)
{
	nValue < 0 ? nValue = 0 : 0;
	*(m_propAddress[dwPropID]) = nValue;
}

/// purpose: 增量修改 非负数
void CMentorshipPersonData::increment_NoNegative(DWORD dwPropID, int nValue)
{
	LONGLONG old_value = *(m_propAddress[dwPropID]);
	if (old_value + (LONGLONG)nValue > MAX_MENTORSHIP_INT_VALUE)
		return;

	*(m_propAddress[dwPropID]) += nValue;
	*(m_propAddress[dwPropID]) < 0 ? *(m_propAddress[dwPropID]) = 0 : 0;
}

/// purpose: 设置DB数据信息
bool CMentorshipPersonData::setDBDataContext(MentorshipService *pMentorshipService, DBREQ_RESULT_READMENTORSHIPPERSON *pRetData)
{
	if (pMentorshipService == NULL)
	{
		ErrorLn(__FUNCTION__": setDBDataContext pMentorshipService == NULL");
		return false;
	}

	m_pMentorshipService = pMentorshipService;

	m_dwPDBID = pRetData->dwPDBID;
	m_nAutoIssueMFlag = pRetData->nAutoIssueMFlag;
	m_nAutoIssuePFlag = pRetData->nAutoIssuePFlag;
	m_nMSumShipVal = pRetData->nMSumShipVal;
	m_nMWeekShipVal = pRetData->nMWeekShipVal;
	m_nMLastWeekShipVal = pRetData->nMLastWeekShipVal;
	m_nMLastSVOrder = pRetData->nMLastSVOrder;
	m_nMRecordTime = (pRetData->nMRecordTime == 0) ? (int)time(NULL) : pRetData->nMRecordTime;
	m_nPSumShipVal = pRetData->nPSumShipVal;
	m_nPWeekShipVal = pRetData->nPWeekShipVal;
	m_nPLastWeekShipVal = pRetData->nPLastWeekShipVal;
	m_nPLastSVOrder = pRetData->nPLastSVOrder;
	m_nPRecordTime = (pRetData->nPRecordTime == 0) ? (int)time(NULL) : pRetData->nPRecordTime;
	m_nMFWinAwardPreCount = pRetData->nMFWinAwardPreCount;
	m_nMFWinAwardCount = pRetData->nMFWinAwardCount;
	m_nMFWinAwardTime = (pRetData->nMFWinAwardTime == 0) ? (int)time(NULL) : pRetData->nMFWinAwardTime;
	m_nPFWinAwardPreCount = pRetData->nPFWinAwardPreCount;
	m_nPFWinAwardCount = pRetData->nPFWinAwardCount;
	m_nPFWinAwardTime = (pRetData->nPFWinAwardTime == 0) ? (int)time(NULL) : pRetData->nPFWinAwardTime;

	m_bDBPsersonDataFinal = true;
	
	return true;
}

/// purpose: 更新数据通过不同的周
bool CMentorshipPersonData::updateDataByDiffWeek()
{
	bool bFlag = false;

	int nWeek = g_EHelper.DiffWeek((DWORD)time(NULL), (DWORD)m_nMRecordTime);
	if (nWeek > 0)
	{
		if (nWeek == 1)
		{
			bFlag = true;
			m_nMLastWeekShipVal = m_nMWeekShipVal;
			m_nMWeekShipVal = 0;
			m_nMLastSVOrder = 0; // 去排行榜取值
			m_nMRecordTime = (int)time(NULL);
		}
		else
		{
			bFlag = true;
			m_nMWeekShipVal = 0;
			m_nMLastWeekShipVal = 0;
			m_nMLastSVOrder = 0;
			m_nMRecordTime = (int)time(NULL);
		}	
	}

	nWeek = g_EHelper.DiffWeek((DWORD)time(NULL), (DWORD)m_nPRecordTime);
	if (nWeek > 0)
	{
		if (nWeek == 1)
		{
			bFlag = true;
			m_nPLastWeekShipVal = m_nPWeekShipVal;
			m_nPWeekShipVal = 0;
			m_nPLastSVOrder = 0; // 去排行榜取值
			m_nPRecordTime = (int)time(NULL);
		}
		else
		{
			bFlag = true;
			m_nPWeekShipVal = 0;
			m_nPLastWeekShipVal = 0;
			m_nPLastSVOrder = 0;
			m_nPRecordTime = (int)time(NULL);
		}	
	}

	nWeek = g_EHelper.DiffWeek((DWORD)time(NULL), (DWORD)m_nMFWinAwardTime);
	if (nWeek > 0)
	{
		bFlag = true;
		m_nMFWinAwardPreCount = 0;
		m_nMFWinAwardCount = 0;
		m_nMFWinAwardTime = (int)time(NULL);
	}
	else
	{
		SMentorshipConfig *pConfig = g_EHelper.getConfig();
		if (pConfig != NULL)
		{
			if (m_nMFWinAwardPreCount + m_nMFWinAwardCount > pConfig->nMFWinAwardCount)
			{
				bFlag = true;
				m_nMFWinAwardPreCount = pConfig->nMFWinAwardCount - m_nMFWinAwardCount;
				m_nMFWinAwardPreCount = (m_nMFWinAwardPreCount < 0 ? 0 : m_nMFWinAwardPreCount);
			}
		}
	}

	nWeek = g_EHelper.DiffWeek((DWORD)time(NULL), (DWORD)m_nPFWinAwardTime);
	if (nWeek > 0)
	{
		bFlag = true;
		m_nPFWinAwardPreCount = 0;
		m_nPFWinAwardCount = 0;
		m_nPFWinAwardTime = (int)time(NULL);
	}
	else
	{
		SMentorshipConfig *pConfig = g_EHelper.getConfig();
		if (pConfig != NULL)
		{
			if (m_nPFWinAwardPreCount + m_nPFWinAwardCount > pConfig->nPFWinAwardCount)
			{
				bFlag = true;
				m_nPFWinAwardPreCount = pConfig->nPFWinAwardCount - m_nPFWinAwardCount;
				m_nPFWinAwardPreCount = (m_nPFWinAwardPreCount < 0 ? 0 : m_nPFWinAwardPreCount);
			}
		}
	}

	return bFlag;
}

/// purpose: 更新个人师徒数据存数据库
void CMentorshipPersonData::updateDataToDB()
{
	IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
	if (pBEngineService == NULL)
	{
		return;
	}

	if (m_pMentorshipService == NULL)
		return;

	DBREQ_PARAM_UPDATEMENTORSHIPPERSON referData;
	referData.dwPDBID = m_dwPDBID;
	referData.nAutoIssueMFlag = m_nAutoIssueMFlag;
	referData.nAutoIssuePFlag = m_nAutoIssuePFlag;
	referData.nMSumShipVal = m_nMSumShipVal;
	referData.nMWeekShipVal = m_nMWeekShipVal;
	referData.nMLastWeekShipVal = m_nMLastWeekShipVal;
	referData.nMLastSVOrder = m_nMLastSVOrder;
	referData.nMRecordTime = m_nMRecordTime;
	referData.nPSumShipVal = m_nPSumShipVal;
	referData.nPWeekShipVal = m_nPWeekShipVal;
	referData.nPLastWeekShipVal = m_nPLastWeekShipVal;
	referData.nPLastSVOrder = m_nPLastSVOrder;
	referData.nPRecordTime = m_nPRecordTime;
	referData.nMFWinAwardPreCount = m_nMFWinAwardPreCount;
	referData.nMFWinAwardCount = m_nMFWinAwardCount;
	referData.nMFWinAwardTime = m_nMFWinAwardTime;
	referData.nPFWinAwardPreCount = m_nPFWinAwardPreCount;
	referData.nPFWinAwardCount = m_nPFWinAwardCount;
	referData.nPFWinAwardTime = m_nPFWinAwardTime;
	// 更新到数据库
	m_pMentorshipService->ExecSP(GAMEDB_REQUEST_UPDATE_MENTORSHIPPERSON, (LPCSTR)&referData, sizeof(DBREQ_PARAM_UPDATEMENTORSHIPPERSON));
}

/// purpose: 添加师徒值
void CMentorshipPersonData::addMentorshipValue(SMsgMentorship_Add_Member_ShipVal_SO *pRecvData)
{
	if (pRecvData == NULL)
	{
		return;
	}

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
	{
		return;
	}

	if (pRecvData->nPost == emMentorshipPost_Master)
	{
		updateDataByDiffWeek();

		if (m_nMWeekShipVal < pConfig->nWeekShipVal)
		{
			int oldValue = m_nMWeekShipVal;
			int nAddValue = (pRecvData->bFightResult == true) ? pConfig->nWinShipVal : pConfig->nFailShipVal;
			int nCurValue = oldValue + nAddValue;
			m_nMWeekShipVal = (nCurValue <= pConfig->nWeekShipVal) ? nCurValue : pConfig->nWeekShipVal;
			nAddValue = m_nMWeekShipVal - oldValue;
			m_nMSumShipVal += nAddValue;
			m_nMRecordTime = (int)time(NULL);

			updateDataToDB();

			if (m_pMentorshipService != NULL)
			{
				m_pMentorshipService->updateMentorshipPersonBaseToClient(this);
			}

			// 记录日志
			IOSSLogService* pOSSLogService = gSocialGlobal->getOSSLogService();
			if (pOSSLogService != NULL)
			{
				pOSSLogService->addNumericLog(getThisGameWorldID(), OSS_OPT_TYPE_PERSON, m_dwPDBID, m_nMetorshipPost, nAddValue, OSS_NUMERIC_MENTORSHIP_VALUE, OSS_RESOURCE_OPT_ADD, OSSRES_ADD_MENTORSHIP_INC_VAL, m_nMRecordTime, a2utf8("增加导师值"));
			}
		}
	}
	else if (pRecvData->nPost == emMentorshipPost_Prentice)
	{
		updateDataByDiffWeek();

		if (m_nPWeekShipVal < pConfig->nWeekShipVal)
		{
			int oldValue = m_nPWeekShipVal;
			int nAddValue = (pRecvData->bFightResult == true) ? pConfig->nWinShipVal : pConfig->nFailShipVal;
			int nCurValue = oldValue + nAddValue;
			m_nPWeekShipVal = (nCurValue <= pConfig->nWeekShipVal) ? nCurValue : pConfig->nWeekShipVal;
			nAddValue = m_nPWeekShipVal - oldValue;
			m_nPSumShipVal += nAddValue;
			m_nPRecordTime = (int)time(NULL);

			updateDataToDB();

			if (m_pMentorshipService != NULL)
			{
				m_pMentorshipService->updateMentorshipPersonBaseToClient(this);
			}

			// 记录日志
			IOSSLogService* pOSSLogService = gSocialGlobal->getOSSLogService();
			if (pOSSLogService != NULL)
			{
				pOSSLogService->addNumericLog(getThisGameWorldID(), OSS_OPT_TYPE_PERSON, m_dwPDBID, m_nMetorshipPost, nAddValue, OSS_NUMERIC_MENTORSHIP_VALUE, OSS_RESOURCE_OPT_ADD, OSSRES_ADD_MENTORSHIP_INC_VAL, m_nPRecordTime, a2utf8("增加学徒值"));
			}
		}
	}
}

// 获取属性对应时间属性
int CMentorshipPersonData::getPropToTimeProp(int nProp)
{
	switch (nProp)
	{
	case em_MshipPersonProp_MFWinAwardCount:
		{
			return em_MshipPersonProp_MFWinAwardTime;
		}
		break;
	case em_MshipPersonProp_PFWinAwardCount:
		{
			return em_MshipPersonProp_PFWinAwardTime;
		}
		break;
	default:
		{
			return -1;
		}
		break;
	}

	return -1;
}

