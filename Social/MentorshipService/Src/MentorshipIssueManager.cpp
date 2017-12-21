#include "stdafx.h"
#include "MentorshipIssueManager.h"
#include "ExternalHelper.h"
#include "MentorshipDef.h"
#include "ISocialGlobal.h"
#include "IShareServer.h"
#include "ClanHelper.h"
#include "KinHelper.h"
#include "Mentorship_ManagedDef.h"

using namespace Mentorship_ManagerDef;

// 全局战队数据中心对象
CMentorshipIssueManager g_MentorshipIssueManager;

/// purpose: 构造函数
CMentorshipIssueManager::CMentorshipIssueManager()
{

}

/// purpose: 虚构函数
CMentorshipIssueManager::~CMentorshipIssueManager()
{

}

/// purpose: 添加一个师傅发布信息对象
void CMentorshipIssueManager::addMasterIssue(SMentorshipIssue* pMasterIssue)
{
	if (NULL == pMasterIssue)
	{
		return;
	}
	TMap_MentorshipIssueIt it = m_MasterIssueList.find(pMasterIssue->dwPDBID);
	if (it != m_MasterIssueList.end())
	{
		return;
	}

	m_MasterIssueList[pMasterIssue->dwPDBID] = *pMasterIssue;
}

/// purpose: 添加一个学徒发布信息对象
void CMentorshipIssueManager::addPrenticeIssue(SMentorshipIssue* pPrenticeIssue)
{
	if (NULL == pPrenticeIssue)
	{
		return;
	}
	TMap_MentorshipIssueIt it = m_PrenticeIssueList.find(pPrenticeIssue->dwPDBID);
	if (it != m_PrenticeIssueList.end())
	{
		return;
	}

	m_PrenticeIssueList[pPrenticeIssue->dwPDBID] = *pPrenticeIssue;
}

/// purpose: 删除一个师傅发布信息对象
//@ param  : dwPDBID 玩家ID
void CMentorshipIssueManager::removeMasterIssue(DWORD dwPDBID)
{
	TMap_MentorshipIssueIt it = m_MasterIssueList.find(dwPDBID);
	if (it != m_MasterIssueList.end())
	{
		m_MasterIssueList.erase(it);

		removeMIssueOnlineMember(dwPDBID);
	}
}

/// purpose: 删除一个学徒发布信息对象
//@ param  : dwPDBID 玩家ID
void CMentorshipIssueManager::removePrenticeIssue(DWORD dwPDBID)
{
	TMap_MentorshipIssueIt it = m_PrenticeIssueList.find(dwPDBID);
	if (it != m_PrenticeIssueList.end())
	{
		m_PrenticeIssueList.erase(it);

		removePIssueOnlineMember(dwPDBID);
	}
}

/// purpose: 查找一个师傅发布信息对象
//@ param  : dwPDBID 玩家ID
SMentorshipIssue* CMentorshipIssueManager::findMasterIssue(DWORD dwPDBID)
{
	TMap_MentorshipIssueIt it = m_MasterIssueList.find(dwPDBID);
	if (it != m_MasterIssueList.end())
	{
		return &((*it).second);
	}

	return NULL;
}

/// purpose: 查找一个学徒发布信息对象
//@ param  : dwPDBID 玩家ID
SMentorshipIssue* CMentorshipIssueManager::findPrenticeIssue(DWORD dwPDBID)
{
	TMap_MentorshipIssueIt it = m_PrenticeIssueList.find(dwPDBID);
	if (it != m_PrenticeIssueList.end())
	{
		return &((*it).second);
	}

	return NULL;
}

/// purpose: 添加一个上线对象
//@ param  : dwPDBID 玩家ID
void CMentorshipIssueManager::addIssueOnlineMember(DWORD dwPDBID)
{
	addMIssueOnlineMember(dwPDBID);

	addPIssueOnlineMember(dwPDBID);
}

/// purpose: 添加师傅上线对象
//@ param  : dwPDBID 玩家ID
void CMentorshipIssueManager::addMIssueOnlineMember(DWORD dwPDBID)
{
	SMentorshipIssue *pMasterIssue = findMasterIssue(dwPDBID);
	if (pMasterIssue != NULL)
	{
		vector<DWORD>::iterator it = find(m_MIssueOnlineList.begin(), m_MIssueOnlineList.end(), dwPDBID);
		if (it == m_MIssueOnlineList.end())
		{
			m_MIssueOnlineList.push_back(dwPDBID);
		}
	}
}

/// purpose: 添加学徒上线对象
//@ param  : dwPDBID 玩家ID
void CMentorshipIssueManager::addPIssueOnlineMember(DWORD dwPDBID)
{
	SMentorshipIssue *pPrenticeIssue = findPrenticeIssue(dwPDBID);
	if (pPrenticeIssue != NULL)
	{
		vector<DWORD>::iterator it = find(m_PIssueOnlineList.begin(), m_PIssueOnlineList.end(), dwPDBID);
		if (it == m_PIssueOnlineList.end())
		{
			m_PIssueOnlineList.push_back(dwPDBID);
		}
	}
}

/// purpose: 移除一个上线对象
//@ param  : dwPDBID 玩家ID
void CMentorshipIssueManager::removeIssueOnlineMember(DWORD dwPDBID)
{
	removeMIssueOnlineMember(dwPDBID);
	//
	removePIssueOnlineMember(dwPDBID);
}

/// purpose: 移除师傅上线对象
//@ param  : dwPDBID 玩家ID
void CMentorshipIssueManager::removeMIssueOnlineMember(DWORD dwPDBID)
{
	size_t nLen = m_MIssueOnlineList.size();
	for (size_t i=0; i < nLen; ++i)
	{
		if (m_MIssueOnlineList[i] == dwPDBID)
		{
			m_MIssueOnlineList[i] = m_MIssueOnlineList[nLen-1];
			m_MIssueOnlineList.pop_back();
			return;
		}
	}
}

/// purpose: 移除学徒上线对象
//@ param  : dwPDBID 玩家ID
void CMentorshipIssueManager::removePIssueOnlineMember(DWORD dwPDBID)
{
	size_t nLen = m_PIssueOnlineList.size();
	for (size_t i=0; i < nLen; ++i)
	{
		if (m_PIssueOnlineList[i] == dwPDBID)
		{
			m_PIssueOnlineList[i] = m_PIssueOnlineList[nLen-1];
			m_PIssueOnlineList.pop_back();
			return;
		}
	}
}

/// purpose: 发送发布信息列表To客户端
//@ param  : dwPDBID 玩家ID
void CMentorshipIssueManager::sendIssueListToClient(DWORD dwPDBID, BYTE nIssueType)
{
	if (nIssueType != emMentorshipIssueType_Master && nIssueType != emMentorshipIssueType_Prentice)
	{
		return;
	}

	obuf2048 ob;
	SMsgMentorshipIssueList sendData;
	sendData.nIssueType = nIssueType;
	sendData.nCount = 0;
	fillMentorshipData(ob, MSG_ENDPOINT_CLIENT, MSG_MENTORSHIP_ISSUE_LIST);
	
	obuf2048 obNode;
	if (nIssueType == emMentorshipIssueType_Master)
	{
		int i = 0;
		TMap_MentorshipIssueIt itSelfIssue = m_MasterIssueList.find(dwPDBID);
		if (itSelfIssue != m_MasterIssueList.end())
		{
			SMsgMentorship_Issue_Node issueNode;
			if (fillMentorshipIssueNode(dwPDBID, &issueNode) == true)
			{
				sstrcpyn(issueNode.szIssueInfo, (*itSelfIssue).second.szIssueInfo, sizeof(issueNode.szIssueInfo));
				++sendData.nCount;
				++i;
				obNode.push_back(&issueNode, sizeof(issueNode));
			}
		}

		int nCurRandSize = (int)m_MIssueOnlineList.size();
		if (nCurRandSize > i)
		{
			int nRandMax = (nCurRandSize > MENTORSHIP_ISSUE_PAGE_SIZE ? (nCurRandSize - MENTORSHIP_ISSUE_PAGE_SIZE) : 0);
			int nRandIndex = mt_range_rand(0, nRandMax);
			while (i < MENTORSHIP_ISSUE_PAGE_SIZE)
			{
				if (nRandIndex >= nCurRandSize)
					break;

				DWORD dwRandPDBID = m_MIssueOnlineList[nRandIndex];
				if (dwRandPDBID != dwPDBID)
				{
					TMap_MentorshipIssueIt itIssue = m_MasterIssueList.find(dwRandPDBID);
					if (itIssue != m_MasterIssueList.end())
					{
						SMsgMentorship_Issue_Node issueNode;
						if (fillMentorshipIssueNode(dwRandPDBID, &issueNode) == true)
						{
							sstrcpyn(issueNode.szIssueInfo, (*itIssue).second.szIssueInfo, sizeof(issueNode.szIssueInfo));
							++i;
							++sendData.nCount;
							obNode.push_back(&issueNode, sizeof(issueNode));
						}
					}
				}

				++nRandIndex;
			}
		}
	}
	else
	{
		int i = 0;
		TMap_MentorshipIssueIt itSelfIssue = m_PrenticeIssueList.find(dwPDBID);
		if (itSelfIssue != m_PrenticeIssueList.end())
		{
			SMsgMentorship_Issue_Node issueNode;
			if (fillMentorshipIssueNode(dwPDBID, &issueNode) == true)
			{
				sstrcpyn(issueNode.szIssueInfo, (*itSelfIssue).second.szIssueInfo, sizeof(issueNode.szIssueInfo));
				++sendData.nCount;
				++i;
				obNode.push_back(&issueNode, sizeof(issueNode));
			}
		}

		int nCurRandSize = (int)m_PIssueOnlineList.size();
		if (nCurRandSize > i)
		{
			int nRandMax = (nCurRandSize > MENTORSHIP_ISSUE_PAGE_SIZE ? (nCurRandSize - MENTORSHIP_ISSUE_PAGE_SIZE) : 0);
			int nRandIndex = mt_range_rand(0, nRandMax);
			while (i < MENTORSHIP_ISSUE_PAGE_SIZE)
			{
				if (nRandIndex >= nCurRandSize)
					break;

				DWORD dwRandPDBID = m_PIssueOnlineList[nRandIndex];
				if (dwRandPDBID != dwPDBID)
				{
					TMap_MentorshipIssueIt itIssue = m_PrenticeIssueList.find(dwRandPDBID);
					if (itIssue != m_PrenticeIssueList.end())
					{
						SMsgMentorship_Issue_Node issueNode;
						if (fillMentorshipIssueNode(dwRandPDBID, &issueNode) == true)
						{
							sstrcpyn(issueNode.szIssueInfo, (*itIssue).second.szIssueInfo, sizeof(issueNode.szIssueInfo));
							++i;
							++sendData.nCount;
							obNode.push_back(&issueNode, sizeof(issueNode));
						}
					}
				}

				++nRandIndex;
			}
		}
	}

	ob.push_back(&sendData, sizeof(sendData));
	ob.push_back(obNode.data(), obNode.size());
	g_EHelper.SendDataToPerson(dwPDBID, ob.data(), ob.size());
}

/// purpose: 填充发布节点数据
//@ param  : 节点
bool CMentorshipIssueManager::fillMentorshipIssueNode(DWORD dwPDBID, SMsgMentorship_Issue_Node* pIssueNode)
{
	if (dwPDBID == INVALID_PDBID)
	{
		return false;
	}

	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver==NULL)
	{
		return false;
	}

	SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
	if (shareProperty.dwPDBID != dwPDBID)
	{
		return false;
	}

	pIssueNode->dwPDBID = dwPDBID;
	pIssueNode->nLevel = shareProperty.wLevel;
	pIssueNode->bySex = shareProperty.bySex;
	sstrcpyn(pIssueNode->szName, shareProperty.szName, sizeof(pIssueNode->szName));
	KinHelper kinHelper;
	IKinService *pKinService = kinHelper.m_ptr;
	if (pKinService != NULL)
	{
		SKinInfo kinInfo = pKinService->getKinInfo(shareProperty.dwKinID);
		if (kinInfo.nKinProp_ID != 0)
		{
			sstrcpyn(pIssueNode->szCorpsName, kinInfo.szName, sizeof(pIssueNode->szCorpsName));

			ClanHelper helper;
			IClanService *pClanService = helper.m_ptr;
			if (pClanService != NULL)
			{
				string clanName = pClanService->getClanName(shareProperty.dwClanID);
				if (clanName.size() != 0)
				{
					sstrcpyn(pIssueNode->szClanName, clanName.c_str(), sizeof(pIssueNode->szClanName));
				}
			}
		}
	}

	return true;
}
