#include "stdafx.h"
#include "GV_HandshakeResponseHandler.h"
#include "ViewMsgDef_Server.h"
#include "GateConnector.h"



void GV_HandshakeResponseHandler::Handle(GateConnector* pSession, SGameMsgHead* head, SMsgView_GV_HandshakeResponse* pMsg)
{
	if (pSession->GetID() != pMsg->dwID)
	{
		ErrorLn(_GT("网关说他自己的ID=") << pMsg->dwID << _GT("和Mng告诉我的不一样,Mng给他的ID=") << pSession->GetID());
		pSession->Release();// 断开连接

		// 可能向管理服重新申请GatesInfo也无济于事	TODO

		return;
	}

	TraceLn(_GT("收到网关回应握手信息! ") << a2utf8(pSession->ToString().data()));
}
