#include "stdafx.h"
#include "ClientUser.h"
#include "Gates.h"
#include "ReplayContainer.h"
#include "Replay.h"


void ClientUser::OnSendData(DWORD dwDataLen)
{
	//if (m_pByteRecord)
	//	m_pByteRecord->OnSendData(dwDataLen);
}

GateConnector*	ClientUser::GetGate()
{
	return gGates.Get(m_wGateID);
}

void ClientUser::StopView()
{
	if (m_rID == 0)
		return;

	auto pReplay = gReplays.Get(m_rID);
	if (pReplay == nullptr)
		return;

	pReplay->Remove(m_dwPlayerID);
}
