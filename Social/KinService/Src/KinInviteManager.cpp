//==========================================================================
/**
* @file	  : KinInviteManager.cpp
* @author : xiaobao
* created : 2009-3-7
* purpose : 
*/
//==========================================================================

#include "stdafx.h"
#include "KinInviteManager.h"
#include "Kin.h"
#include "KinService.h"

// 全局战队数据中心对象
CKinInviteManager g_KinInviteManager;

/// purpose: 构造函数
CKinInviteManager::CKinInviteManager()
{

}

/// purpose: 虚构函数
CKinInviteManager::~CKinInviteManager()
{

}

/// purpose: 玩家下线处理
//@ note  : 被邀请者下线，删除邀请关系，通知客户端删除dida和窗口
void CKinInviteManager::OnUserLogout(DWORD uPDBID)
{
	// 删除被邀请者的相关邀请对象，并提示客户端关闭窗口
	list<SKinInvite>::iterator it = m_InviteList.begin();
	while (it != m_InviteList.end())
	{
		SKinInvite* pInvite = &(*it);
		if (pInvite->dwAnswerPDBID == uPDBID)
		{
			it = m_InviteList.erase(it);
		}
		else
		{
			++ it;
		}
	}
}

/// purpose: 添加一个邀请对象
//@ param  : dwInvitePDBID 邀请者ID
//@ param  : dwAnswerPDBID 被邀请者ID
void CKinInviteManager::AddInvite(DWORD dwInvitePDBID, DWORD dwInviteKinID, DWORD dwAnswerPDBID)
{
	// 添加一个邀请对象
	SKinInvite invite;
	invite.dwInvitePDBID = dwInvitePDBID;
	invite.dwInviteKinID = dwInviteKinID;
	invite.dwAnswerPDBID = dwAnswerPDBID;

	RemoveInvite(dwInvitePDBID, dwAnswerPDBID);
	m_InviteList.push_back(invite);
}

/// purpose: 删除一个邀请对象
//@ param  : dwInvitePDBID 邀请者ID
//@ param  : dwAnswerPDBID 被邀请者ID
void CKinInviteManager::RemoveInvite(DWORD dwInvitePDBID, DWORD dwAnswerPDBID)
{
	//  删除一个邀请对象
	list<SKinInvite>::iterator it;
	for (it = m_InviteList.begin(); it != m_InviteList.end(); ++ it)
	{
		SKinInvite* pInvite = &(*it);
		if (pInvite->dwInvitePDBID == dwInvitePDBID && pInvite->dwAnswerPDBID == dwAnswerPDBID)
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

/// purpose: 删除被邀请者ID为dwAnswerPDBID的所有邀请对象
//@ param  : dwAnswerPDBID 被邀请者ID
SKinInvite* CKinInviteManager::FindInvite(DWORD dwInvitePDBID, DWORD dwAnswerPDBID)
{
	// 删除被邀请者的相关邀请对象，并提示客户端关闭窗口
	list<SKinInvite>::iterator it;
	for (it = m_InviteList.begin(); it != m_InviteList.end(); ++ it)
	{
		SKinInvite* pInvite = &(*it);
		if (pInvite->dwInvitePDBID == dwInvitePDBID && pInvite->dwAnswerPDBID == dwAnswerPDBID)
		{
			return pInvite;
		}
	}

	return NULL;
}

/// purpose: 添加一个申请对象
void CKinInviteManager::AddApply(SKinApply* pKinApply)
{
	// 添加一个申请对象
	if (pKinApply == NULL)
	{
		return;
	}

	list<SKinApply>::iterator it;
	for (it = m_ApplyList.begin(); it != m_ApplyList.end(); ++ it)
	{
		SKinApply* pApply = &(*it);
		if (pApply->dwApplyPDBID == pKinApply->dwApplyPDBID && pApply->dwApplyKinID == pKinApply->dwApplyKinID)
		{
			return;
		}
	}

	m_ApplyList.push_back(*pKinApply);
}

/// purpose: 删除一个申请对象
//@ param  : dwApplyPDBID 申请者ID
void CKinInviteManager::RemoveApply(DWORD dwApplyPDBID)
{
	// 删除一个申请对象
    list<SKinApply>::iterator it = m_ApplyList.begin();
    while (it != m_ApplyList.end())
    {
        SKinApply* pKinApply = &(*it);
        if (pKinApply->dwApplyPDBID == dwApplyPDBID)
        {
            // 先从该战队的申请成员列表中删除
            CKin* pKin = (CKin*)m_pKinService->findKin(pKinApply->dwApplyKinID);
            if (pKin != NULL)
            {
                pKin->removeApplyMember(pKinApply->dwApplyPDBID);
            }

            it = m_ApplyList.erase(it);
        }
        else
        {
            ++ it;
        }
    }
}

/// purpose: 删除一个申请对象
//@ param  : dwApplyPDBID 申请者ID
void CKinInviteManager::RemoveApply(DWORD dwApplyPDBID, DWORD dwKinID)
{
	// 删除一个申请对象
    list<SKinApply>::iterator it = m_ApplyList.begin();
    while (it != m_ApplyList.end())
    {
        SKinApply* pKinApply = &(*it);
        if (pKinApply->dwApplyPDBID == dwApplyPDBID && pKinApply->dwApplyKinID == dwKinID)
        {
            // 先从该战队的申请成员列表中删除
            CKin* pKin = (CKin*)m_pKinService->findKin(pKinApply->dwApplyKinID);
            if (pKin != NULL)
            {
                pKin->removeApplyMember(pKinApply->dwApplyPDBID);
            }

            it = m_ApplyList.erase(it);
        }
        else
        {
            ++ it;
        }
    }
}

/// purpose: 查找一个申请对象
//@ param  : dwApplyPDBID 角色ID
int CKinInviteManager::GetApplyKinCount(DWORD dwApplyPDBID)
{
	// 查找一个申请对象
	int nCount = 0;
	list<SKinApply>::iterator it;
	for (it = m_ApplyList.begin(); it != m_ApplyList.end(); ++ it)
	{
		SKinApply* pKinApply = &(*it);
		if (pKinApply->dwApplyPDBID == dwApplyPDBID)
		{
			nCount ++;
		}
	}

	return nCount;
}

/// purpose: 查找一个申请对象
//@ param  : dwApplyPDBID 申请者ID
//@ param  : dwKinID 战队ID
SKinApply* CKinInviteManager::FindApply(DWORD dwApplyPDBID, DWORD dwKinID)
{
	// 查找一个申请对象
	list<SKinApply>::iterator it;
	for (it = m_ApplyList.begin(); it != m_ApplyList.end(); ++ it)
	{
		SKinApply* pKinApply = &(*it);
		if (pKinApply->dwApplyPDBID == dwApplyPDBID && pKinApply->dwApplyKinID == dwKinID)
		{
			return pKinApply;

		}
	}

	return NULL;
}

void CKinInviteManager::setKinService(KinService *pKinService)
{
    if (pKinService == NULL)
    {
        return;
    }
    m_pKinService = pKinService;
}
