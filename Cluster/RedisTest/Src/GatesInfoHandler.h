#pragma once
#include "Handler.h"
#include "MngConnector.h"
#include "ViewMsgDef_Server.h"
#include "Gates.h"


class GatesInfoHandler : public Handler<MngConnector, SMsgView_MV_GatesInfo>
{
protected:
	virtual void Handle(MngConnector* pSession, SGameMsgHead* head, SMsgView_MV_GatesInfo* pMsg)
	{
		VecGateInfo& vecGates = pMsg->vecGates;
		// �Աȱ��ص�����Gate��Ϣ��û����ȫ������
		gGates.Update(vecGates);

		TraceLn(_GT("�յ����������GateInfo��Ϣ! ") << a2utf8(pSession->ToString().data()));
	}
};