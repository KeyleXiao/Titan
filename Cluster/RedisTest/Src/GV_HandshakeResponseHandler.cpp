#include "stdafx.h"
#include "GV_HandshakeResponseHandler.h"
#include "ViewMsgDef_Server.h"
#include "GateConnector.h"



void GV_HandshakeResponseHandler::Handle(GateConnector* pSession, SGameMsgHead* head, SMsgView_GV_HandshakeResponse* pMsg)
{
	if (pSession->GetID() != pMsg->dwID)
	{
		ErrorLn(_GT("����˵���Լ���ID=") << pMsg->dwID << _GT("��Mng�����ҵĲ�һ��,Mng������ID=") << pSession->GetID());
		pSession->Release();// �Ͽ�����

		// ������������������GatesInfoҲ�޼�����	TODO

		return;
	}

	TraceLn(_GT("�յ����ػ�Ӧ������Ϣ! ") << a2utf8(pSession->ToString().data()));
}
