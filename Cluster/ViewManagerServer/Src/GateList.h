#pragma once
#include "UserList.h"

class GateUser;
struct SMsgView_MV_GatesInfo;

class GateList : public UserList<GateUser, GateID>, public Singleton<GateList>
{
public:
	GateList();
	virtual ~GateList() {};

	int		GetReadyCount();

	void	BuildGatesInfo(SMsgView_MV_GatesInfo& gatesInfo);

	//************************************
	// Returns:   GateID
	// Qualifier: ���Ҹ��������Gate��������ID������Ҳ������򷵻�0 (���ա�ʣ��������������Ƚ�)
	// Parameter: GateID wExceptID	Ҫ�ų���GateID
	//************************************
	GateID	GetIdleGate(GateID wExceptID = 0);

protected:
	virtual void OnDelUser(ISessionUser<GateID>* pUser) {};

	//ע������Ҫ�����������Ϣ����m_Processer��
	virtual void RegisterHandlers();

	virtual GateID GetMaxID() { return MAX_GATEWAY_COUNT; }

};
extern GateList& gGateList;