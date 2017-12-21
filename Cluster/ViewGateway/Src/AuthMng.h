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

	// ɾ���˽ڵ㲢֪ͨMng
	void Remove(const PlayerID& dwPlayerID, EnumAuthDelReason	eReason);

private:
	// <���DBID, �����֤��Ϣ>
	typedef	std::map<PlayerID, AuthNode>	MapAuth;

	MapAuth			m_MapAuth;	// Mng�����ĵȴ���֤��½�������Ϣ��û����Щ��Ϣ������ǲ������½��

	TimerAxis*		m_pTimerAxis;
	MngConnector*	m_pMng;
};
extern AuthMng& gAuthMng;