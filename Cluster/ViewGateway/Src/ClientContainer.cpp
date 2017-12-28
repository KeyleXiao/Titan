#include "stdafx.h"
#include "ClientContainer.h"
#include "Setting.h"
#include "ClientMsgRegister.h"


ClientContainer& gClientContainer = ClientContainer::getInstance();

ClientContainer::ClientContainer()
{
	RegisterHandlers();
}

ClientContainer::~ClientContainer()
{
}

PlayerID ClientContainer::GetMaxID()
{
	return gSetting.m_dwMaxConnection;
}

//ע������Ҫ�����������Ϣ����m_Processer��
void ClientContainer::RegisterHandlers()
{
	ClientMsgRegister::RegisterMsgs(m_Processer);
}

void ClientContainer::OnDelUser(ISessionUser<PlayerID>* pUser)
{
}

void ClientContainer::BroadCastData(const obuf& obufData, const VecPlayerID& vec)
{
	for (size_t i = 0; i < vec.size(); i++)
	{
		const PlayerID& dwPlayerID = vec.at(i);
		auto pClient = this->Get(dwPlayerID);
		if (pClient == nullptr)
			continue;

		pClient->SendData(obufData.data(), obufData.size());
	}
}