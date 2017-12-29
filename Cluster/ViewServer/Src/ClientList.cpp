#include "stdafx.h"
#include "ClientList.h"
#include "GateConnector.h"


ClientList& gClientList = ClientList::getInstance();

ClientUser* ClientList::InsertIfAbsent(PlayerID dwPlayerID, GateConnector* pGateConnector)
{
	auto it = m_ClientMap.find(dwPlayerID);
	if (it != m_ClientMap.end())
		return &it->second;
	else
	{
		m_ClientMap[dwPlayerID].m_dwPlayerID = dwPlayerID;
		m_ClientMap[dwPlayerID].m_wGateID = pGateConnector->GetID();
		return &m_ClientMap[dwPlayerID];
	}
}

ClientUser*	ClientList::Get(PlayerID dwPlayerID)
{
	auto it = m_ClientMap.find(dwPlayerID);
	if (it != m_ClientMap.end())
		return &it->second;
	else
		return nullptr;
}

void ClientList::Del(const PlayerID& dwPlayerID)
{
	m_ClientMap.erase(dwPlayerID);
}
