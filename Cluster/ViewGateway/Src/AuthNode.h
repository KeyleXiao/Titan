#pragma once
#include "ViewMsgDef_Server.h"


struct AuthNode
{
	SMsgView_MG_RequestAddPlayer	m_info;
	time_t							m_tExpiry;	// ����ʱ��

	// ����Ƿ����
	bool IsExpiry(time_t now) { return this->m_tExpiry <= now; }
};