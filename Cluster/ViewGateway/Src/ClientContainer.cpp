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

DWORD ClientContainer::GetMaxID()
{
	return gSetting.m_dwMaxConnection;
}

ClientSession* ClientContainer::GetByPlayerID(PlayerID dwPlayerID)
{
	// TODO

	throw std::logic_error("The method or operation is not implemented.");
}

//注册这里要处理的所有消息，到m_Processer中
void ClientContainer::RegisterHandlers()
{
	ClientMsgRegister::RegisterMsgs(m_Processer);
}