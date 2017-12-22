#pragma once
#include "ViewMsgDef_Server.h"


struct AuthNode
{
	SMsgView_MG_RequestAddPlayer	m_info;			// ����ľ�����Ϣ
	GameID							m_reqServerID;	// ���������ServerID
	GateID							m_wGateID;		// Ϊ����������ID

	void GetKeyData(BYTE szKeyData[])
	{
		for (size_t i = 0; i < VIEW_SECRET_KEY_SIZE; i++)
		{
			szKeyData[i] = m_info.szKeyData[i];
		}
	}
};