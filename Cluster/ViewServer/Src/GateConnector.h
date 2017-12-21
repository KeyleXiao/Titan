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

	// MngΪ��Gate���ɵ�����ID
	inline	WORD	GetID() { return m_wID; }
	inline	void	SetID(WORD wID) { m_wID = wID; }

protected:
	//ȡ��Ŀ��ڵ������
	virtual	const char*		GetDestPointName();
	//����������Ϣ
	virtual	bool			SendHandshake();

	virtual	const obuf&		GetKeepAliveMsg();
	virtual WORD			GetAnswerKeepAliveID();

protected:
	WORD 		m_wID;			// MngΪ��Gate���ɵ�����ID
};

