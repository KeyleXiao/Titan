#include "stdafx.h"
#include "GateList.h"
#include "GateUser.h"
#include "GateMsgRegister.h"
#include "ViewMsgDef_Server.h"


GateList& gGateList = GateList::getInstance();

GateList::GateList()
{
	RegisterHandlers();
}

//注册这里要处理的所有消息，到m_Processer中
void GateList::RegisterHandlers()
{
	GateMsgRegister::RegisterMsgs(m_Processer);
}

void GateList::BuildGatesInfo(SMsgView_MV_GatesInfo& gatesInfo)
{
	for (auto it = m_UserMap.begin(); it != m_UserMap.end(); ++it)
	{
		GateUser*& pUser = it->second;
		if (pUser->IsReady())
		{
			SGateInfo* pInfo = new SGateInfo;
			pInfo->strIp = pUser->GetRemoteIP();
			pInfo->wPort = pUser->GetPortForView();
			pInfo->wID = (WORD)pUser->GetID();
			gatesInfo.vecGates.push_back(pInfo);
		}
	}
}

GateID GateList::GetIdleGate()
{
	GateID wRet = 0;
	DWORD remainCountMax = 0;
	for (auto it = m_UserMap.begin(); it != m_UserMap.end(); ++it)
	{
		GateUser*& pUser = it->second;
		DWORD remainCount = pUser->GetMaxCount() - pUser->GetCurrCount();
		if (remainCount > remainCountMax)
		{
			remainCountMax = remainCount;
			wRet = (GateID)it->first;
			Assert(it->first == wRet);
		}
	}

	return wRet;
}

int GateList::GetReadyCount()
{
	int nCount = 0;
	for (auto it = m_UserMap.begin(); it != m_UserMap.end(); ++it)
	{
		GateUser*& pUser = it->second;
		if (pUser->IsReady())
			nCount += 1;
	}
	return nCount;
}
