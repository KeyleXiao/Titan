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

//ע������Ҫ�����������Ϣ����m_Processer��
void ViewContainer::RegisterHandlers()
{
	ViewMsgRegister::RegisterMsgs(m_Processer);
}