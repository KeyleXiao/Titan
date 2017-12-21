//==========================================================================
/**
* @file	  : Kin.cpp
* @author : xiaobao
* created : 2009-3-7
* purpose : 
*/
//==========================================================================

#include "stdafx.h"
#include "Kin.h"
#include "KinService.h"
#include "ITimerService.h"

// ================================家族状态基类=============================
/// purpose: 构造函数
CKinState::CKinState()
{
	m_pKin = NULL;
}

/// purpose: 虚构函数
CKinState::~CKinState()
{

}


// ================================非正式状态===============================
/// purpose: 构造函数
CKinInformalState::CKinInformalState()
{

}

/// purpose: 虚构函数
CKinInformalState::~CKinInformalState()
{

}

// 开始状态
void CKinInformalState::Start(CKin* pKin)
{
	m_pKin = pKin;
	TimerAxis * pTimerAxis = gGlobalSocialServer->getTimerAxis();
	SKinConfig* pConfig = g_KinDataCenter.GetConfig();
	if (m_pKin == NULL || pTimerAxis == NULL || pConfig == NULL)
	{
		return;
	}

	int	nEstablishTime = m_pKin->GetNumProp(emKinProp_EstablishTime);
	int nCurTime = (int)time(NULL);
	// 如果离创建时间已经超过了1小时，则解散家族（其实不可能小于10分钟，读取列表的时候已经做了处理）
	int nInterval = nCurTime + pConfig->nInformalLife - nEstablishTime ;
	if (nInterval < 0)
	{
		nInterval = 0;
	}

	pTimerAxis->SetTimer(0, nInterval * 1000, this, INFINITY_CALL, "CKinInformalState::Start");
}

// 结束状态
void CKinInformalState::Stop()
{
	TimerAxis* pTimerAxis = gGlobalSocialServer->getTimerAxis();
	if (pTimerAxis != NULL)
	{
		pTimerAxis->KillTimer(0, this);
	}

	m_pKin = NULL;
}

// 对家族对象消息的过滤
bool CKinInformalState::VoteMessage(DWORD dwMsgID, DWORD dwSrcPointID)
{
	if (m_pKin == NULL)
	{
		return true;
	}

	// 只对响应家族进行响应
	if (dwMsgID == MSG_KIN_RESPOND || dwMsgID == MSG_KIN_QUIT)
	{
		return true;
	}

	return false;
}

/// purpose: 定时器触发后回调,您可以在这里编写处理代码
void CKinInformalState::OnTimer(unsigned long dwTimerID)
{
	// 时间到了还没成为正式家族，解散家族对象
	TimerAxis* pTimerAxis = gGlobalSocialServer->getTimerAxis();
	if (m_pKin != NULL && pTimerAxis != NULL)
	{
		pTimerAxis->KillTimer(0, this);
		// 这里家族的对象和家族的状态对象都已经被释放，小心
		m_pKin->Dismiss(emKinDismiss_Informal, "非正式家族到期");
		return;
	}
}


// ================================正式状态=================================
/// purpose: 构造函数
CKinFormalState::CKinFormalState()
{

}

/// purpose: 虚构函数
CKinFormalState::~CKinFormalState()
{

}

// 开始状态
void CKinFormalState::Start(CKin* pKin)
{
	m_pKin = pKin;
}

// 结束状态
void CKinFormalState::Stop()
{
	m_pKin = NULL;
}

// 对家族对象消息的过滤
bool CKinFormalState::VoteMessage(DWORD dwMsgID, DWORD dwSrcPointID)
{
	switch (dwMsgID)
	{
	case MSG_KIN_RESPOND:
		return false;

	case MSG_KIN_APPOINT:
	case MSG_KIN_KICK:
	case MSG_KIN_QUIT:
	case MSG_KIN_ADMIT:
		// 家族移民中予以否决
		if (m_pKin != 0 && m_pKin->IsChangeLand())
		{
			UserInfo* pUserInfo = gGlobalSocialServer->getUserManager()->findUserByClientID(dwSrcPointID);
			if (pUserInfo != NULL)
			{
				gGlobalSocialServer->getChatService()->sendSystemTips( TipType_Operate, pUserInfo->id, 
																	   InfoPos_ActorUnder,
																	   "您正在家族移民中，无法执行此操作！" );
				return false;
			}
		}
		// else 不在家族移民中执行default:

	default:
		return true;
	}
}


// ================================解散状态=================================
/// purpose: 构造函数
CKinDismissingState::CKinDismissingState()
{

}

/// purpose: 虚构函数
CKinDismissingState::~CKinDismissingState()
{

}

// 开始状态
void CKinDismissingState::Start(CKin* pKin)
{
	
}

// 结束状态
void CKinDismissingState::Stop()
{
	
}

// 对家族对象消息的过滤
bool CKinDismissingState::VoteMessage(DWORD dwMsgID, DWORD dwSrcPointID)
{
	// 只对响应家族进行不响应
	if (dwMsgID == MSG_KIN_RESPOND)
	{
		return false;
	}

	return true;
}

//////////////////////////TimerHandler///////////////////////////
/// purpose: 定时器触发后回调,您可以在这里编写处理代码
void CKinDismissingState::OnTimer(unsigned long dwTimerID)
{
	// 时间到了还没成为正式家族，解散家族对象
   
}
