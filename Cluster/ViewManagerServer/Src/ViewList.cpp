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

//注册这里要处理的所有消息，到m_Processer中
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

void ViewList::BroadcastGatesInfoToViews()
{
	SMsgView_MV_GatesInfo sendData;
	gGateList.BuildGatesInfo(sendData);

	for each (auto tmpPair in m_UserMap)
	{
		ViewUser*& pUser = tmpPair.second;
		if (pUser->IsReady())
		{
			pUser->SendMsg(sendData);
		}
	}
	TraceLn(_GT("发送GatesInfo给所有View! ") );
}
