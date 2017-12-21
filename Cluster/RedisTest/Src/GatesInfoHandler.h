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
		// 对比本地的所有Gate信息，没连的全都连上
		gGates.Update(vecGates);

		TraceLn(_GT("收到管理服所有GateInfo信息! ") << a2utf8(pSession->ToString().data()));
	}
};