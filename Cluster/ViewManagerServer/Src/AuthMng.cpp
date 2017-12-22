#include "stdafx.h"
#include "AuthMng.h"


AuthMng& gAuthMng = AuthMng::getInstance();

AuthNode* AuthMng::Get(PlayerID dwPlayerID)
{
	auto it = m_MapAuth.find(dwPlayerID);
	if (it!= m_MapAuth.end())
		return &it->second;

	return nullptr;
}
