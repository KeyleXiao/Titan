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
				// ������ã�ɶ��������
				continue;
			} 
			else
			{
				// ��ID���������ӻ��ڣ���IP��˿ڻ��ˡ�
				// ��Ͽ����ӣ�������
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
				// ������ͬIP�Ͷ˿ڵ�Gate���ӡ�Ӧ����Mng�͸�Gate�������жϹ��������������ˣ����Mng�����¸����Gate�µ�ID.
				// ��ô����ID���¼���
				m_map.erase(pConn->GetID());
				pConn->SetID(wID);
				m_map[wID] = pConn;
			} 
			else
			{
				// �ߵ�����������������
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