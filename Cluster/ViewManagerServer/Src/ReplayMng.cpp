#include "stdafx.h"
#include "ReplayMng.h"
#include "ViewList.h"
#include "ViewUser.h"
#include "ViewMsgDef_Server.h"


ReplayMng& gReplayMng = ReplayMng::getInstance();

// ��������
ReplayMng::~ReplayMng()
{
	// 1.0
	for (ReplayMap::iterator it = m_map.begin(); it!= m_map.end(); ++it)
	{
		OneReplayMap*& pMap = it->second;
		for each (auto entry in *pMap)
		{
			ReplayNode*& pNode = entry.second;
			safeDelete(pNode);
		}
		pMap->clear();
		safeDelete(pMap);
	}
	m_map.clear();

	// 2.0
	m_ViewMap.clear();
}
EnumResultRequestReplay ReplayMng::Find(ReplayID rID, ReplayNode*& pNode)
{
	if (gViewList.Count() <= 0)
		return E_RESULT_RR_ERR_NO_VIEW_SERVER;

	OneReplayMap* pOneMap = nullptr;
	auto it = m_map.find(rID);
	if (it != m_map.end())
		pOneMap = it->second;
	else
	{
		// �Դ�ս������û�й�սMap
		pOneMap = new OneReplayMap;
		m_map[rID] = pOneMap;
	}

	// �����нڵ��У�ѡ��һ�����������Ľڵ�
	if (!pOneMap->empty())
	{
		for (auto iter = pOneMap->begin(); iter != pOneMap->end(); ++iter)
		{
			ReplayNode*& pEachNode = iter->second;
			if (pEachNode->IsFull())
				continue;

			if (pNode == nullptr)
			{
				pNode = pEachNode;
				continue;
			}

			if (pEachNode->RemainCount() < pNode->RemainCount())
			{
				pNode = pEachNode;
			}
		}
	}

	// û�й�ս�ڵ� �� ���нڵ�����
	if (pNode==nullptr)
	{
		// ��һ�����ؽ����ViewServer
		ViewID dwViewID = GetIdleView();
		if (dwViewID == 0)
			return E_RESULT_RR_ERR_VIEW_IS_FULL;

		// ����һ���µĹ�ս�ڵ�
		pNode = new ReplayNode(dwViewID, rID);
		(*pOneMap)[dwViewID] = pNode;
		pNode->NotifyView();	// ֪ͨ��ViewServer
		return E_RESULT_RR_SUCCESS;
	}
	return E_RESULT_RR_SUCCESS;
}

ViewID ReplayMng::GetIdleView(ViewID exceptID)
{
	std::pair<const ViewID, DWORD>* _pair = nullptr;
	for each (auto entry in m_ViewMap)
	{
		if (entry.first == exceptID)
			continue;
		auto pView = gViewList.Get(entry.first);
		if (pView == nullptr)
			continue;
		if (pView->IsFull())
			continue;

		if (_pair == nullptr)
		{
			_pair = &entry;
			continue;
		}
		else
		{
			const DWORD& count = entry.second;
			if (_pair->second > count)
			{
				_pair = &entry;
			}
		}
	}

	if (_pair == nullptr)
		return 0;

	return _pair->first;
}

