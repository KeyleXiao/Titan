#pragma once
#include "GateConnector.h"


struct ClientUser
{
	GateID			m_wGateID;		// ��������ڵ�GateID
	PlayerID		m_dwPlayerID;	// ���ΨһID

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
		TBuildObufMsg(obufData, header, m_dwPlayerID, msg);

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