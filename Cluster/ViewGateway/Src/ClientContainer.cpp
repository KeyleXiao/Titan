#include "stdafx.h"
#include "ClientContainer.h"
#include "Setting.h"
#include "ClientMsgRegister.h"
#include "ViewMsgDef_Server.h"
#include "IGlobalViewGateway.h"
#include "MngConnector.h"
#include "ViewContainer.h"
#include "ViewSession.h"


ClientContainer& gClientContainer = ClientContainer::getInstance();

ClientContainer::ClientContainer()
{
	RegisterHandlers();
}

ClientContainer::~ClientContainer()
{}

PlayerID ClientContainer::GetMaxID()
{
	return gSetting.m_dwMaxConnection;
}

//注册这里要处理的所有消息，到m_Processer中
void ClientContainer::RegisterHandlers()
{
	ClientMsgRegister::RegisterMsgs(m_Processer);
}

void ClientContainer::OnDelUser(ISessionUser<PlayerID>* pUser)
{
	// 需要通知Mng和View
	PlayerID dwPlayerID = pUser->GetID();

	// 通知Mng
	{
		SMsgView_GM_PlayerDel msg;
		msg.dwPlayerID = dwPlayerID;

		MngConnector& mng = gGlobalServer->GetMngConnector();
		mng.SendMsg(msg);
	}

	// 通知View
	{
		SMsgView_GV_PlayerDel msg;
		msg.dwPlayerID = dwPlayerID;

		auto pClient = (ClientSession*)pUser;
		auto viewID = pClient->GetViewID();
		auto pView = gViewContainer.Get(viewID);
		if (pView)
			pView->SendMsg(msg);
	}
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