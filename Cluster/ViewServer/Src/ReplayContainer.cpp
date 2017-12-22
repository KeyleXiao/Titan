#include "stdafx.h"
#include "ReplayContainer.h"
#include "Replay.h"


ReplayContainer& gReplays = ReplayContainer::getInstance();

Replay* ReplayContainer::Get(const ReplayID& rID)
{
	auto it = m_ReplayMap.find(rID);
	if (it != m_ReplayMap.end())
		return &it->second;
	else
		return nullptr;
}

Replay* ReplayContainer::Insert(const ReplayID& rID)
{
	m_ReplayMap[rID].m_rID = rID;
	return &m_ReplayMap[rID];
}