#include "stdafx.h"
#include "ViewList.h"
#include "HandshakeHandler.h"
#include "ViewUser.h"
#include "GateList.h"
#include "ViewMsgDef_Server.h"


ViewList& gViewList = ViewList::getInstance();

ViewList::ViewList()
{
	RegisterHandlers();
}

ViewList::~ViewList()
{
}

//ע������Ҫ�����������Ϣ����m_Processer��
void ViewList::RegisterHandlers()
{
	m_Processer.Register(new HandshakeHandler);
}

int ViewList::GetReadyCount()
{
	int nCount = 0;
	for (auto it = m_UserMap.begin(); it != m_UserMap.end(); ++it)
	{
		auto pUser = it->second;
		if (pUser->IsReady())
			nCount += 1;
	}
	return nCount;
}

void ViewList::BroadcastGatesInfoToView()
{
	const SGameMsgHead& header = gMsg.BuildHead_MV(ENUM_MSG_VIEW_GATES_INFO);
	SMsgView_MV_GatesInfo sendData;
	gGateList.BuildGatesInfo(sendData);

	obuf obufData;
	TBuildObufMsg(obufData, header, sendData);

	for each (auto tmpPair in m_UserMap)
	{
		ViewUser*& pUser = tmpPair.second;
		if (pUser->IsReady())
		{
			pUser->SendData(obufData.data(), obufData.size());
		}
	}
	TraceLn(_GT("����GatesInfo������View! ") );
}
