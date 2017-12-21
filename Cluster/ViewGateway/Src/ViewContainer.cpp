#include "stdafx.h"
#include "ViewContainer.h"
#include "ViewSession.h"
#include "HandshakeHandler.h"
#include "ViewMsgDef_Server.h"
#include "ViewMsgRegister.h"


ViewContainer& gViewContainer = ViewContainer::getInstance();

ViewContainer::ViewContainer()
{
	RegisterHandlers();
}

ViewContainer::~ViewContainer()
{
}

//注册这里要处理的所有消息，到m_Processer中
void ViewContainer::RegisterHandlers()
{
	ViewMsgRegister::RegisterMsgs(m_Processer);
}