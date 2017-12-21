#include "StdAfx.h"
#include "Mentorship.h"
#include "ExternalHelper.h"
#include "MentorshipClient.h"
#include "MentorshipMsgDef.h"
#include "Event_ManagedDef.h"
#include "IBankPart.h"
#include "IClientEntity.h"
#include "ISchemeCenter.h"
#include "MatchSchemeDef.h"
#include "Mentorship_ManagedDef.h"

using namespace Mentorship_ManagerDef;

CMentorship* g_pMentorship = NULL;

bool Less_Mentorship(const SClientMentorShip& mentorship1, const SClientMentorShip& mentorship2)
{
	if (mentorship1.dwBuildTime != mentorship2.dwBuildTime)
	{
		return (mentorship1.dwBuildTime < mentorship2.dwBuildTime);
	}

	return false;
}

// 构造函数
CMentorship::CMentorship()
{
	reset();

	memset(m_propAddress, 0, sizeof(m_propAddress));

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

// 析构函数
CMentorship::~CMentorship()
{

}

// 创建
bool CMentorship::create(void)
{
	return true;
}

// 释放
void CMentorship::release(void)
{
}

// 清空所有数据
void CMentorship::reset(void)
{
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

	m_bLoad = false;

	m_MentorshipVec.clear();
}

// 是否有效
bool CMentorship::isVaild(void)
{
	return m_bLoad;
}

// 取得帮会的整型数据
int CMentorship::getNumProp(DWORD dwPropID)
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

// 设置帮会的整型数据
bool CMentorship::setNumProp(DWORD dwPropID, int nValue)
{
	if (dwPropID < em_MshipPersonProp_Post && dwPropID >= emMshipPersonProp_NumPropCount)
	{
		return false;
	}

	if (m_propAddress[dwPropID] == 0)
	{
		return false;
	}

	*(m_propAddress[dwPropID]) = nValue;

	return true;
}

// 获取师徒职务
BYTE CMentorship::getMentorshipPost()
{
	return m_nMetorshipPost;
}

// 获取正式个数
int CMentorship::getFormalCount(BYTE nPost)
{
	int nFormalCount = 0;

	size_t nLen = m_MentorshipVec.size();
	for (size_t i=0; i < nLen; ++i)
	{
		if (m_MentorshipVec[i].nRelationType == emMentorshipType_Formal && m_MentorshipVec[i].nPost == nPost)
		{
			++nFormalCount;
		}
	}

	return nFormalCount;
}

// 获取见习个数
int CMentorship::getInternCount(BYTE nPost)
{
	int nInternCount = 0;

	size_t nLen = m_MentorshipVec.size();
	for (size_t i=0; i < nLen; ++i)
	{
		if (m_MentorshipVec[i].nRelationType == emMentorshipType_Intern && m_MentorshipVec[i].nPost == nPost)
		{
			++nInternCount;
		}
	}

	return nInternCount;
}

////////////////////////IMessageHandler////////////////////
// 消息处理
void CMentorship::onMessage(SNetMsgHead *pHead, void *pData, size_t nLen)
{
	// 客户端消息case宏
#define MSG_CASE(proc, msgID) \
		case msgID:\
		{ proc((LPCSTR)pData, nLen);	break;}

	switch (pHead->byKeyAction)
	{
		// 请求师徒个人数据
		MSG_CASE(onMentorshipPersonData, MSG_MENTORSHIP_PERSONDATA)

		// 师徒个人基础数据
		MSG_CASE(onMentorshipPersonBaseData, MSG_MENTORSHIP_UPDATE_BASE_PERSONDATA)

		// 更新师徒关系数据
		MSG_CASE(onUpdateMentorshipData, MSG_MENTORSHIP_UPDATE_MENTORSHIP)

		// 添加师徒关系数据
		MSG_CASE(onAddMentorshipData, MSG_MENTORSHIP_ADD_MENTORSHIP)

		// 移除师徒关系数据
		MSG_CASE(onRemoveMentorshipData, MSG_MENTORSHIP_DISMISS)

		// 师徒成员上下线
		MSG_CASE(onMemberLoginAndLogout, MSG_MENTORSHIP_MEMBER_LOGIN_LOGOUT)

	default:
		break;
	}
}

// 更新数据到显示
void CMentorship::updatePersonDataToView()
{
	updatePersonBaseDataToView();
	updatePersonShipToView();
}

// 更新基础数据到显示
void CMentorship::updatePersonBaseDataToView()
{
	obuf obData;

	cmd_mentorship_person_baseinfo baseData;
	baseData.nPDBID = m_dwPDBID;
	baseData.nAutoIssueMFlag = m_nAutoIssueMFlag;
	baseData.nAutoIssuePFlag = m_nAutoIssuePFlag;
	baseData.nMSumShipVal = m_nMSumShipVal;
	baseData.nMWeekShipVal = m_nMWeekShipVal;
	baseData.nMLastWeekShipVal = m_nMLastWeekShipVal;
	baseData.nMLastSVOrder = m_nMLastSVOrder;
	baseData.nMRecordTime = m_nMRecordTime;
	baseData.nPSumShipVal = m_nPSumShipVal;
	baseData.nPWeekShipVal = m_nPWeekShipVal;
	baseData.nPLastWeekShipVal = m_nPLastWeekShipVal;
	baseData.nPLastSVOrder = m_nPLastSVOrder;
	baseData.nPRecordTime = m_nPRecordTime;
	baseData.nMFWinAwardPreCount = m_nMFWinAwardPreCount;
	baseData.nMFWinAwardCount = m_nMFWinAwardCount;
	baseData.nMFWinAwardTime = m_nMFWinAwardTime;
	baseData.nPFWinAwardPreCount = m_nPFWinAwardPreCount;
	baseData.nPFWinAwardCount = m_nPFWinAwardCount;
	baseData.nPFWinAwardTime = m_nPFWinAwardTime;

	obData.push_back(&baseData, sizeof(baseData));
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MENTORSHIP_UPDATEPERSONBASE, obData.size(), 0, obData.data(), obData.size());
}

// 更新师徒数据到显示
void CMentorship::updatePersonShipToView()
{
	obuf4096 obData;
	cmd_mentorship_mentorshiplist_head headData;
	headData.nCount = 0;

	obuf4096 obNodeData;
	vector<SClientMentorShip>::iterator it = m_MentorshipVec.begin();
	for (; it != m_MentorshipVec.end(); ++it)
	{
		cmd_mentorship_personshipinfo sendNode;
		sendNode.nMentorshipID = (*it).dwMentorshipID;
		sendNode.nPDBID = (*it).dwPDBID;
		sendNode.nFaceID = (*it).nFaceID;
		sendNode.nIsOnline = ((*it).bIsOnline == true) ? 1 : 0;
		sendNode.nPost = (*it).nPost;
		sstrcpyn(sendNode.szName, (*it).szName, sizeof(sendNode.szName));
		sendNode.nRelationType = (*it).nRelationType;
		sendNode.nState = (*it).nState;
		sendNode.nBuildTime = (*it).dwBuildTime;
		sendNode.nSeverPDBID = (*it).dwSeverPDBID;
		sendNode.nSeverTime = (*it).dwSeverTime;

		obNodeData.push_back(&sendNode, sizeof(sendNode));

		headData.nCount += 1;
	}

	obData.push_back(&headData, sizeof(headData));
	obData.push_back(obNodeData.data(), obNodeData.size());
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MENTORSHIP_UPDATEMENTORSHIPLIST, obData.size(), 0, obData.data(), obData.size());
}

// 移除师徒数据到显示
void CMentorship::removePersonShipToView(DWORD dwMentorshipID)
{
	obuf obData;
	cmd_mentorship_request_dismiss sendData;
	sendData.nMentorshipID = dwMentorshipID;
	obData.push_back(&sendData, sizeof(sendData));
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MENTORSHIP_REQUESTDISMISS, obData.size(), 0, obData.data(), obData.size());
}

// 请求师徒数据返回处理
void CMentorship::onMentorshipPersonData(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen < sizeof(SMsgMentorship_PersonData_OC))
		return;
	SMsgMentorship_PersonData_OC *pRecvData = (SMsgMentorship_PersonData_OC*)pszMsg;
	int offSet = sizeof(SMsgMentorship_PersonData_OC);
	if (nLen != offSet + sizeof(SMentorshipPerson) + pRecvData->nCount * sizeof(SClientMentorShip))
	{
		return;
	}

	SMentorshipPerson *pPerson = (SMentorshipPerson*)(pszMsg + offSet);

	m_dwPDBID = pPerson->dwPDBID;
	m_nAutoIssueMFlag = pPerson->nAutoIssueMFlag;
	m_nAutoIssuePFlag = pPerson->nAutoIssuePFlag;
	m_nMSumShipVal = pPerson->nMSumShipVal;
	m_nMWeekShipVal = pPerson->nMWeekShipVal;
	m_nMLastWeekShipVal = pPerson->nMLastWeekShipVal;
	m_nMLastSVOrder = pPerson->nMLastSVOrder;
	m_nMRecordTime = pPerson->nMRecordTime;
	m_nPSumShipVal = pPerson->nPSumShipVal;
	m_nPWeekShipVal = pPerson->nPWeekShipVal;
	m_nPLastWeekShipVal = pPerson->nPLastWeekShipVal;
	m_nPLastSVOrder = pPerson->nPLastSVOrder;
	m_nPRecordTime = pPerson->nPRecordTime;
	m_nMFWinAwardPreCount = pPerson->nMFWinAwardPreCount;
	m_nMFWinAwardCount = pPerson->nMFWinAwardCount;
	m_nMFWinAwardTime = pPerson->nMFWinAwardTime;
	m_nPFWinAwardPreCount = pPerson->nPFWinAwardPreCount;
	m_nPFWinAwardCount = pPerson->nPFWinAwardCount;
	m_nPFWinAwardTime = pPerson->nPFWinAwardTime;

	offSet += sizeof(SMentorshipPerson);
	
	m_MentorshipVec.clear();
	for (int i=0; i < pRecvData->nCount; ++i)
	{
		SClientMentorShip *pMentorshipNode = (SClientMentorShip*)(pszMsg + offSet);
		SClientMentorShip clientMentorship;
		memcpy(&clientMentorship, pMentorshipNode, sizeof(SClientMentorShip));
		m_MentorshipVec.push_back(clientMentorship);

		offSet += sizeof(SClientMentorShip);
	}

	m_bLoad = true;

	updatePersonDataToView();
}

// 师徒个人基础数据
void CMentorship::onMentorshipPersonBaseData(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(SMentorshipPerson))
		return;

	SMentorshipPerson *pPerson = (SMentorshipPerson*)pszMsg;

	m_nAutoIssueMFlag = pPerson->nAutoIssueMFlag;
	m_nAutoIssuePFlag = pPerson->nAutoIssuePFlag;
	m_nMSumShipVal = pPerson->nMSumShipVal;
	m_nMWeekShipVal = pPerson->nMWeekShipVal;
	m_nMLastWeekShipVal = pPerson->nMLastWeekShipVal;
	m_nMLastSVOrder = pPerson->nMLastSVOrder;
	m_nMRecordTime = pPerson->nMRecordTime;
	m_nPSumShipVal = pPerson->nPSumShipVal;
	m_nPWeekShipVal = pPerson->nPWeekShipVal;
	m_nPLastWeekShipVal = pPerson->nPLastWeekShipVal;
	m_nPLastSVOrder = pPerson->nPLastSVOrder;
	m_nPRecordTime = pPerson->nPRecordTime;
	m_nMFWinAwardPreCount = pPerson->nMFWinAwardPreCount;
	m_nMFWinAwardCount = pPerson->nMFWinAwardCount;
	m_nMFWinAwardTime = pPerson->nMFWinAwardTime;
	m_nPFWinAwardPreCount = pPerson->nPFWinAwardPreCount;
	m_nPFWinAwardCount = pPerson->nPFWinAwardCount;
	m_nPFWinAwardTime = pPerson->nPFWinAwardTime;

	updatePersonBaseDataToView();
}

// 更新师徒关系数据
void CMentorship::onUpdateMentorshipData(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(SMsgMentorship_UpdateMentorship_OC))
		return;
	SMsgMentorship_UpdateMentorship_OC *pRecvData = (SMsgMentorship_UpdateMentorship_OC*)pszMsg;

	vector<SClientMentorShip>::iterator it = m_MentorshipVec.begin();
	for (; it != m_MentorshipVec.end(); ++it)
	{
		if ((*it).dwMentorshipID == pRecvData->dwMentorshipID)
		{
			sstrcpyn((*it).szName, pRecvData->szName, sizeof((*it).szName));
			(*it).nRelationType = pRecvData->nRelationType;
			(*it).nState = pRecvData->nState;
			(*it).dwSeverPDBID = pRecvData->dwSeverPDBID;
			(*it).dwSeverTime = pRecvData->dwSeverTime;
			break;
		}
	}

	obuf obData;
	cmd_mentorship_updateshipinfo sendData;
	sendData.nMentorshipID = pRecvData->dwMentorshipID;
	sendData.nRelationType = pRecvData->nRelationType;
	sendData.nState = pRecvData->nState;
	sendData.nSeverPDBID = pRecvData->dwSeverPDBID;
	sendData.nSeverTime = pRecvData->dwSeverTime;
	sstrcpyn(sendData.szName, pRecvData->szName, sizeof(sendData.szName));
	obData.push_back(&sendData, sizeof(sendData));
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MENTORSHIP_UPDATESHIPINFO, obData.size(), 0, obData.data(), obData.size());
}

// 添加师徒关系数据
void CMentorship::onAddMentorshipData(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(SClientMentorShip))
		return;
	SClientMentorShip *pMentorship = (SClientMentorShip*)pszMsg;

	SClientMentorShip clientMentorship;

	memcpy(&clientMentorship, pMentorship, sizeof(SClientMentorShip));


	m_MentorshipVec.push_back(clientMentorship);

	updatePersonShipToView();
}

// 删除师徒关系数据
void CMentorship::onRemoveMentorshipData(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(SMsgMentorship_Dismiss))
		return;
	SMsgMentorship_Dismiss *pRecvData = (SMsgMentorship_Dismiss*)pszMsg;

	bool bRemoveFlag = false;
	vector<SClientMentorShip>::iterator it = m_MentorshipVec.begin();
	for (; it != m_MentorshipVec.end(); ++it)
	{
		if ((*it).dwMentorshipID == pRecvData->dwMentorshipID)
		{
			m_MentorshipVec.erase(it);
			bRemoveFlag = true;
			break;
		}
	}

	if (bRemoveFlag == true)
	{
		removePersonShipToView(pRecvData->dwMentorshipID);
	}
}

// 师徒成员上下线
void CMentorship::onMemberLoginAndLogout(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(SMsgMentorship_Member_Login_Logout))
		return;
	SMsgMentorship_Member_Login_Logout *pRecvData = (SMsgMentorship_Member_Login_Logout*)pszMsg;

	bool bUpdateFlag = false;
	vector<SClientMentorShip>::iterator it = m_MentorshipVec.begin();
	for (; it != m_MentorshipVec.end(); ++it)
	{
		if ((*it).dwPDBID == pRecvData->dwPDBID)
		{
			(*it).bIsOnline = pRecvData->bFlag;
			bUpdateFlag = true;
		}
	}

	if (bUpdateFlag)
	{
		updatePersonShipToView();
	}
}