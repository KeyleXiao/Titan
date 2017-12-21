#include "stdafx.h"
#include "Clan.h"
#include "ClanService.h"
#include "ExternalHelper.h"

// ================================帮会状态基类=============================
/// purpose: 构造函数
CClanState::CClanState()
{
	m_pClan = NULL;
}

/// purpose: 虚构函数
CClanState::~CClanState()
{

}

// ================================正式状态=================================
/// purpose: 构造函数
CClanNormalState::CClanNormalState()
{

}

/// purpose: 虚构函数
CClanNormalState::~CClanNormalState()
{

}

// 开始状态
void CClanNormalState::start(CClan* pClan)
{
	m_pClan = pClan;
	
}

// 结束状态
void CClanNormalState::stop()
{
	m_pClan = NULL;
}

// 对帮会对象消息的过滤
bool CClanNormalState::voteMessage(DWORD dwMsgID)
{
	return true;
}

// ================================解散状态=================================
/// purpose: 构造函数
CClanDismissingState::CClanDismissingState()
{

}

/// purpose: 虚构函数
CClanDismissingState::~CClanDismissingState()
{

}

// 开始状态
void CClanDismissingState::start(CClan* pClan)
{
	m_pClan = pClan;
	SClanConfig* pConfig = g_EHelper.getConfig();
	if (m_pClan == NULL || pConfig == NULL)
	{
		return;
	}

	int	nDismissApplyTime = m_pClan->getNumProp(emClanProp_DismissApplyTime);
	int nCurTime = (int)time(NULL);
	// 如果离创建时间已经超过了1小时，则解散帮会（其实不可能小于10分钟，读取列表的时候已经做了处理）
	int nInterval = pConfig->nDismissingLife - (nCurTime - nDismissApplyTime);
    if (nInterval < 0)
    {
        nInterval = 1;
    }

    // 启动服务器
    g_EHelper.setTimer(0, nInterval * 1000, this, "CClanDismissingState::Start");
}

// 结束状态
void CClanDismissingState::stop()
{
    g_EHelper.killTimer(0, this);
}

// 对帮会对象消息的过滤
bool CClanDismissingState::voteMessage(DWORD dwMsgID)
{
	return true;
}

//////////////////////////TimerHandler///////////////////////////
/// purpose: 定时器触发后回调,您可以在这里编写处理代码
void CClanDismissingState::OnTimer(unsigned long dwTimerID)
{
	// 时间到了还没成为正式帮会，解散帮会对象
	if (m_pClan != NULL)
	{
        g_EHelper.killTimer(0, this);
		m_pClan->dismiss(emClanDismiss_Dismiss);
		return;
	}
}