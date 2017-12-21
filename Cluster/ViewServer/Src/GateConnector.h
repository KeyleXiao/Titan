#pragma once
#include "ConnectorWithProcess.h"


class GateConnector : public ConnectorWithProcess<GateConnector>
{
public:
	GateConnector(Processer<GateConnector>& processer, ByteRecord& br, TimerAxis& timerAxis);
	virtual ~GateConnector();

	virtual std::string ToString()
	{
		ostrstream buffer;
		buffer << "GateConnector[m_wID=" << m_wID << "], IP[" << m_szRemoteIP << ":" << m_wRemotePort << "]" << std::ends;
		return buffer.str();
	}

	// Mng为该Gate生成的自增ID
	inline	WORD	GetID() { return m_wID; }
	inline	void	SetID(WORD wID) { m_wID = wID; }

protected:
	//取得目标节点的名称
	virtual	const char*		GetDestPointName();
	//发送握手消息
	virtual	bool			SendHandshake();

	virtual	const obuf&		GetKeepAliveMsg();
	virtual WORD			GetAnswerKeepAliveID();

protected:
	WORD 		m_wID;			// Mng为该Gate生成的自增ID
};

