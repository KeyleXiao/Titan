#include "stdafx.h"
#include "ClanInviteManager.h"
#include "Clan.h"
#include "ClanService.h"
#include "ClanHelper.h"

// 全局帮会数据中心对象
CClanInviteManager g_ClanInviteManager;

// 构造函数
CClanInviteManager::CClanInviteManager()
{

}

// 析构函数
CClanInviteManager::~CClanInviteManager()
{

}

// 玩家下线处理
void CClanInviteManager::onUserLogout(DWORD dwPDBID)
{
	// 删除被邀请者的相关邀请对象，并提示客户端关闭窗口
	list<SInvite>::iterator it = m_InviteList.begin();
	while (it != m_InviteList.end())
	{
		SInvite* pInvite = &(*it);
		if (pInvite->dwAnswerPDBID == dwPDBID)
		{
			m_InviteList.erase(it++);
		}
		else
		{
			++ it;
		}
	}
}

// 添加一个邀请对象
void CClanInviteManager::addInvite(DWORD dwInvitePDBID, DWORD dwInviteClanID, DWORD dwAnswerPDBID)
{
	// 添加一个邀请对象
	SInvite invite;
	invite.dwInvitePDBID = dwInvitePDBID;
	invite.dwInviteClanID = dwInviteClanID;
	invite.dwAnswerPDBID = dwAnswerPDBID;

	m_InviteList.push_back(invite);
}

// 删除一个邀请对象
void CClanInviteManager::removeInvite(DWORD dwInvitePDBID, DWORD dwAnswerPDBID)
{
	//  删除一个邀请对象
	list<SInvite>::iterator it;
	for (it = m_InviteList.begin(); it != m_InviteList.end(); ++ it)
	{
		SInvite* pInvite = &(*it);
		if (pInvite->dwInviteClanID == dwInvitePDBID && pInvite->dwAnswerPDBID == dwAnswerPDBID)
		{
			break;
		}
	}
	
	if (it == m_InviteList.end())
	{
		return;
	}

	m_InviteList.erase(it);
}

// 删除被邀请者ID为dwAnswerPDBID的所有邀请对象
SInvite* CClanInviteManager::findInvite(DWORD dwInvitePDBID, DWORD dwAnswerPDBID)
{
	// 删除被邀请者的相关邀请对象，并提示客户端关闭窗口
	list<SInvite>::iterator it;
	for (it = m_InviteList.begin(); it != m_InviteList.end(); ++ it)
	{
		SInvite* pInvite = &(*it);
		if (pInvite->dwInvitePDBID == dwInvitePDBID && pInvite->dwAnswerPDBID == dwAnswerPDBID)
		{
			return pInvite;
		}
	}

	return NULL;
}

// 添加一个申请对象
void CClanInviteManager::addApply(CClan *pClan, SClanApply* pClanApply)
{
	if (pClan == NULL || pClanApply == NULL)
		return;
	// 看看表里面有没有
	list<SClanApply>::iterator it;
	for (it = m_ApplyList.begin(); it != m_ApplyList.end(); ++ it)
	{
		if ((*it).dwKinID == pClanApply->dwKinID)
		{
			return;
		}
	}

	m_ApplyList.push_back(*pClanApply);

	pClan->addApplyMember(pClanApply->dwKinID);
}

// 删除一个申请对象
void CClanInviteManager::removeApply(CClan *pClan, DWORD dwKinID)
{
	if (pClan == NULL || dwKinID == 0)
		return;
	// 看看表里面有没有
	list<SClanApply>::iterator it;
	for (it = m_ApplyList.begin(); it != m_ApplyList.end(); ++ it)
	{
		SClanApply* pClanApply = &(*it);
		if (pClanApply->dwKinID == dwKinID)
		{
			pClan->removeApplyMember(pClanApply->dwKinID);

			m_ApplyList.erase(it);
			return;
		}
	}
}

// 查找一个申请对象
SClanApply* CClanInviteManager::findApply(DWORD dwKinID)
{
	// 看看表里面有没有
	list<SClanApply>::iterator it;
	for (it = m_ApplyList.begin(); it != m_ApplyList.end(); ++ it)
	{
		SClanApply* pClanApply = &(*it);
		if (dwKinID == pClanApply->dwKinID)
		{
			return pClanApply;
		}
	}

	return NULL;
}