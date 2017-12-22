#pragma once
#include "MngConnector.h"
#include "Handler.h"
#include "ViewMsgDef_Server.h"
#include "AuthMng.h"
#include "MsgFactory.h"


class MG_RequestAddPlayerHandler : public Handler<MngConnector, SMsgView_MG_RequestAddPlayer>
{
protected:
	virtual void Handle(MngConnector* pSession, SGameMsgHead* head, SMsgView_MG_RequestAddPlayer* pMsg)
	{
		AuthNode* pAuthNode = nullptr;
		EnumResultRequestAddPlayer eResult = gAuthMng.Insert(pMsg, pAuthNode);

		SMsgView_GM_RequestAddPlayerReply msg;
		msg.eResult = eResult;
		msg.dwPlayerID = pMsg->reqMsg.dwPlayerID;

		pSession->SendMsg(msg);
	}
};