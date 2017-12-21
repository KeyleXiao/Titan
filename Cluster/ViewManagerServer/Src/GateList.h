#pragma once
#include "UserList.h"

class GateUser;
struct SMsgView_MV_GatesInfo;

class GateList : public UserList<GateUser>, public Singleton<GateList>
{
public:
	GateList();
	virtual ~GateList() {};

	int		GetReadyCount();

	void	BuildGatesInfo(SMsgView_MV_GatesInfo& gatesInfo);

	// ���Ҹ��������Gate��������ID������Ҳ������򷵻�0 (���ա�ʣ��������������Ƚ�)
	GateID	GetIdleGate();

protected:
	virtual void onDelUser(ISessionUser* pUser) {};

	//ע������Ҫ�����������Ϣ����m_Processer��
	virtual void RegisterHandlers();

	virtual DWORD GetMaxID() { return MAX_GATEWAY_COUNT; }

};
extern GateList& gGateList;