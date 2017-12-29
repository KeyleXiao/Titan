/*******************************************************************
** 文件名:	ClientList.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/28
** 版  本:	1.0
** 描  述:	在本服的所有Client
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "ClientUser.h"
#include "GateConnector.h"
#include "Gates.h"


class ClientList : public Singleton<ClientList>
{
public:
	// 如果没有就创建；如果有，就返回其指针。所以本方法绝不会返回nullptr
	ClientUser* InsertIfAbsent(PlayerID dwPlayerID, GateConnector* pGateConnector);

	ClientUser*	Get(PlayerID dwPlayerID);

	void Del(const PlayerID& dwPlayerID);

	template<typename TMsg>
	void BroadCastData(TMsg& msg, const VecPlayerID& vec)
	{
		std::map<GateID, VecPlayerID>	_map;
		
		for (size_t i = 0; i < vec.size(); i++)
		{
			const PlayerID& dwPlayerID = vec.at(i);
			auto it = m_ClientMap.find(dwPlayerID);
			if (it == m_ClientMap.end())
				continue;
			
			const GateID& wGateID = it->second.m_wGateID;
			_map[wGateID].push_back(dwPlayerID);
		}

		for each (auto entry in _map)
		{
			const VecPlayerID& vecTmp = entry.second;
			if (vecTmp.empty())
				continue;

			const GateID& wGateID = entry.first;
			GateConnector* pGate = gGates.Get(wGateID);
			if (pGate == nullptr)
				continue;
			
			pGate->SendMsg(msg, (void*)vecTmp.data(), vecTmp.size()*sizeof(PlayerID));
		}
	}

private:
	typedef std::map<PlayerID, ClientUser>	ClientMap;

	ClientMap	m_ClientMap;

};
extern ClientList& gClientList;