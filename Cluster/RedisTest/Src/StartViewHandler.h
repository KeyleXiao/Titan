#pragma once
#include "Handler.h"
#include "MngConnector.h"
#include "ViewMsgDef_Server.h"


class StartViewHandler : public Handler<MngConnector, SMsgView_MV_StartView>
{
protected:
	virtual void Handle(MngConnector* pSession, SGameMsgHead* head, SMsgView_MV_StartView* pMsg)
	{
		//VecGateInfo& vecGates = pMsg->vecGates;
		//// �Աȱ��ص�����Gate��Ϣ��û����ȫ������
		//gGates.Update(vecGates);

		//TraceLn(_GT("�յ����������GateInfo��Ϣ! ") << a2utf8(pSession->ToString().data()));
	}
};