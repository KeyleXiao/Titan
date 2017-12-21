#pragma once
#include "AuthNode.h"


enum EnumResultRequestAddPlayer;
struct SMsgView_MG_RequestAddPlayer;
class MngConnector;

class AuthMng : public Singleton<AuthMng>, public TimerHandler
{
	enum
	{
		ETimerEventID_CheckTimeout = 0,
	};
public:
	virtual void OnTimer(unsigned long dwTimerID) override;

	EnumResultRequestAddPlayer Insert(SMsgView_MG_RequestAddPlayer* pMsg, AuthNode*& pAuthNode);

	void Init(TimerAxis* pTimerAxis);

	void Shutdown();

	void SetMng(MngConnector* pSession);

	AuthNode*	Find(const PlayerID& dwPlayerID);

	// 删除此节点并通知Mng
	void Remove(const PlayerID& dwPlayerID, EnumAuthDelReason	eReason);

private:
	// <玩家DBID, 玩家验证信息>
	typedef	std::map<PlayerID, AuthNode>	MapAuth;

	MapAuth			m_MapAuth;	// Mng发来的等待验证登陆的玩家信息。没有这些信息的玩家是不允许登陆的

	TimerAxis*		m_pTimerAxis;
	MngConnector*	m_pMng;
};
extern AuthMng& gAuthMng;