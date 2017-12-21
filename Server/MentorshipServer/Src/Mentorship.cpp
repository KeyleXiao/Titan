#include "StdAfx.h"
#include "Mentorship.h"
#include "ExternalHelper.h"
#include "MentorshipScene.h"
#include "MentorshipMsgDef.h"
#include "Mentorship_ManagedDef.h"

using namespace Mentorship_ManagerDef;

// 构造函数
CMentorship::CMentorship(void)
{
	m_dwMentorshipID = 0;
	m_dwMasterPDBID = 0;
	m_dwPrenticePDBID = 0;
	m_nRelationType = emMentorshipType_Invalid;
	m_nState = emMentorshipState_Normal;
	m_bPromoteFlag = false;
	m_dwBuildTime = 0;
	m_dwSeverPDBID = 0;
	m_dwSeverTime = 0;
}

// 析构函数
CMentorship::~CMentorship(void)
{

}

// 创建
bool CMentorship::create(DWORD dwMentorshipID, SMsgMentorship_UpdateMentorship_OS *pContext)
{
	if (NULL == pContext)
	{
		return false;
	}
	// 数据
	m_dwMentorshipID = dwMentorshipID;
	m_dwMasterPDBID = pContext->dwMasterPDBID;
	m_dwPrenticePDBID = pContext->dwPrenticePDBID;
	m_nRelationType = pContext->nRelationType;
	m_nState = pContext->nState;
	m_bPromoteFlag = pContext->bPromoteFlag;
	m_dwBuildTime = pContext->dwBuildTime;
	m_dwSeverPDBID = pContext->dwSeverPDBID;
	m_dwSeverTime = pContext->dwSeverTime;

	return true;
}

// 关闭
void CMentorship::release(void)
{
	delete this;
}

// 更新数据
bool CMentorship::updateData(SMsgMentorship_UpdateMentorship_OS *pContext)
{
	if (pContext == NULL)
		return false;

	m_nRelationType = pContext->nRelationType;
	m_nState = pContext->nState;
	m_bPromoteFlag = pContext->bPromoteFlag;
	m_dwBuildTime = pContext->dwBuildTime;
	m_dwSeverPDBID = pContext->dwSeverPDBID;
	m_dwSeverTime = pContext->dwSeverTime;

	return true;
}

// 获取ID
DWORD CMentorship::getMentorshipID()
{
	return m_dwMentorshipID;
}

// 获取导师PDBID
DWORD CMentorship::getMasterPDBID()
{
	return m_dwMasterPDBID;
}

// 获取导师PDBID
DWORD CMentorship::getPrenticePDBID()
{
	return m_dwPrenticePDBID;
}
