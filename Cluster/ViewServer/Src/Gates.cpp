#include "stdafx.h"
#include "Gates.h"
#include "GateConnector.h"
#include "GateMsgRegister.h"


Gates& gGates = Gates::getInstance();

Gates::Gates()
{
	GateMsgRegister::RegisterMsgs(m_processer);
}

Gates::~Gates()
{
}

GateConnector*	Gates::Get(WORD wID)
{
	auto it = m_map.find(wID);
	if (it!=m_map.end())
		return it->second;

	return nullptr;
}
GateConnector*	Gates::Get(const std::string& ip, const WORD& port)
{
	for each (auto tmpPair in m_map)
	{
		auto pConn = tmpPair.second;
		if (ip.compare(pConn->GetRemoteIP())==0
			&& port== pConn->GetRemotePort())
		{
			return pConn;
		}
	}
	return nullptr;
}

void Gates::Update(VecGateInfo& vecGates)
{
	auto count = vecGates.size();
	for (int i = 0; i < count; ++i)
	{
		SGateInfo*& pInfo = vecGates.at(i);
		const WORD& wID = pInfo->wID;
		GateConnector* pConn = Get(wID);
		if (pConn)
		{
			if (pInfo->strIp.compare(pConn->GetRemoteIP()) == 0
				&& pInfo->wPort == pConn->GetRemotePort())
			{
				// 连接完好，啥都不用做
				continue;
			} 
			else
			{
				// 该ID的网关连接还在，但IP或端口换了。
				// 则断开连接，重新连
				m_map.erase(wID);
				pConn->Release();
				NewConnect(pInfo);
			}
		}
		else
		{
			pConn = Get(pInfo->strIp, pInfo->wPort);
			if (pConn)
			{
				// 存在相同IP和端口的Gate连接。应该是Mng和该Gate的连接中断过，而后又连上了，因此Mng会重新赋予该Gate新的ID.
				// 那么将其ID更新即可
				m_map.erase(pConn->GetID());
				pConn->SetID(wID);
				m_map[wID] = pConn;
			} 
			else
			{
				// 走到这里，真的是新连接了
				NewConnect(pInfo);
			}
		}
	}
}

void Gates::NewConnect(SGateInfo*& pInfo)
{
	const WORD& wID = pInfo->wID;
	TimerAxis* pTimerAxis = gGlobalServer->getTimerAxis();
	GateConnector* pNewConn = new GateConnector(m_processer, m_br, *pTimerAxis);
	pNewConn->SetID(wID);
	pNewConn->Connect(pInfo->strIp.c_str(), pInfo->wPort);

	m_map[wID] = pNewConn;
}