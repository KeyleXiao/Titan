#pragma once
#include "ViewMsgDef_Server.h"


struct AuthNode
{
	SMsgView_MG_RequestAddPlayer	m_info;
	time_t							m_tExpiry;	// 到期时间

	// 检测是否过期
	bool IsExpiry(time_t now) { return this->m_tExpiry <= now; }
};