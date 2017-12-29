#pragma once
#include "GateConnector.h"


struct ClientUser
{
	GateID			m_wGateID;		// 此玩家所在的GateID
	PlayerID		m_dwPlayerID;	// 玩家唯一ID
	ReplayID		m_rID;			// 正在观看的战报ID(目前，每个玩家同一时间只能看一场)

	ClientUser() : m_wGateID(0), m_dwPlayerID(0), m_rID(0)
	{}

	bool IsViewing() { return m_rID != 0; }

	void StopView();

	// 发送消息
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

	// 发送消息
	inline bool SendData(const char * pData, DWORD dwLen)
	{
		GateConnector* pConnection = GetGate();
		return pConnection->SendData(pData, dwLen);
	}

private:
	void OnSendData(DWORD dwDataLen);
	GateConnector*	GetGate();

};