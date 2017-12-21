#include "stdafx.h"
#include "ClientSession.h"
#include "ViewMsgDef_Client.h"
#include "MsgFactory.h"


ClientSession::ClientSession(IConnection * conn, IUserList& pUserList, Processer<ClientSession>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord)
	:SessionUser(conn, pUserList, processer, timerAxis, pByteRecord)
{
	m_tConnectTime = (DWORD)time(NULL);

	const size_t size = MAC_ADDRESS_SIZE * sizeof(m_szMacAddress[0]);
	memset(m_szMacAddress, 0, size);
}


ClientSession::~ClientSession()
{}

std::string ClientSession::ToString()
{
	// TODO
	std::string str;
	return str;
}

WORD ClientSession::GetKeepAliveID()
{
	static MsgKey msgKey(MSG_MODULEID_VIEW, ENUM_MSG_VIEW_KEEPALIVE);
	return msgKey.key;
}

const obuf&	ClientSession::GetAnswerKeepAliveMsg()
{
	static bool bFlag = true;
	static obuf obufData;
	if (bFlag)
	{
		const SGameMsgHead& header = gMsg.BuildHead_GC(ENUM_MSG_VIEW_ANSWER_KEEPALIVE);
		SMsgView_GC_Answer_KeepAlive msg;

		obufData.push_back(&header, sizeof(SGameMsgHead));
		obufData.push_back(&msg, sizeof(SMsgView_GC_Answer_KeepAlive));
		bFlag = false;
	}

	return obufData;
}

void ClientSession::SetMacAddress(char szMacAddress[])
{
	const size_t size = MAC_ADDRESS_SIZE * sizeof(m_szMacAddress[0]);
	memcpy_s(m_szMacAddress, size, szMacAddress, size);
}
