#pragma once
#include "GateConnector.h"


struct ClientUser
{
	GateID			m_wGateID;		// ��������ڵ�GateID
	PlayerID		m_dwPlayerID;	// ���ΨһID
	ReplayID		m_rID;			// ���ڹۿ���ս��ID(Ŀǰ��ÿ�����ͬһʱ��ֻ�ܿ�һ��)

	ClientUser() : m_wGateID(0), m_dwPlayerID(0), m_rID(0)
	{}

	bool IsViewing() { return m_rID != 0; }

	void StopView();

	// ������Ϣ
	template<typename TMsg>
	bool SendMsg(TMsg& msg)
	{
		GateConnector* pConnection = GetGate();
		if (pConnection == NULL)
			return false;

		SGameMsgHead header;
		header.SrcEndPoint = msg.GetSrcEndPoint();
		header.DestEndPoint = msg.GetDestEndPoint();
		header.byKeyModule = msg.GetModuleId();
		header.byKeyAction = msg.GetActionId();

		obuf obufData;
		TBuildObufMsg(obufData, header, msg, &m_dwPlayerID, sizeof(m_dwPlayerID));

		const DWORD dwLen = (DWORD)obufData.size();
		Assert(dwLen == obufData.size());
		if (!pConnection->SendData(obufData.data(), dwLen))
			return false;

		OnSendData(dwLen);

		return true;
	}

	// ������Ϣ
	inline bool SendData(const char * pData, DWORD dwLen)
	{
		GateConnector* pConnection = GetGate();
		return pConnection->SendData(pData, dwLen);
	}

private:
	void OnSendData(DWORD dwDataLen);
	GateConnector*	GetGate();

};