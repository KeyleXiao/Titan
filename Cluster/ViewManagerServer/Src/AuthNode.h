#pragma once
#include "ViewMsgDef_Server.h"


struct AuthNode
{
	SMsgView_MG_RequestAddPlayer	m_info;			// 请求的具体信息
	GameID							m_reqServerID;	// 发来请求的ServerID
	GateID							m_wGateID;		// 为其分配的网关ID

	void GetKeyData(BYTE szKeyData[])
	{
		for (size_t i = 0; i < VIEW_SECRET_KEY_SIZE; i++)
		{
			szKeyData[i] = m_info.szKeyData[i];
		}
	}
};