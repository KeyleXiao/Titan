#include "stdafx.h"
#include "ClientUser.h"
#include "Gates.h"


void ClientUser::OnSendData(DWORD dwDataLen)
{
	//if (m_pByteRecord)
	//	m_pByteRecord->OnSendData(dwDataLen);
}

GateConnector*	ClientUser::GetGate()
{
	return gGates.Get(m_wGateID);
}