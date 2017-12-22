#pragma once
#include "AuthNode.h"


class AuthMng : public Singleton<AuthMng>
{
public:
	AuthNode*		Get(PlayerID dwPlayerID);

	inline	void	Remove(PlayerID dwPlayerID) { m_MapAuth.erase(dwPlayerID); }

private:
	// <���DBID, �����֤��Ϣ>
	typedef	std::map<PlayerID, AuthNode>	MapAuth;

	MapAuth		m_MapAuth;
};
extern AuthMng& gAuthMng;