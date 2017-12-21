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

		const SGameMsgHead& header = gMsg.BuildHead_GM(ENUM_MSG_VIEW_REQUEST_ADD_PLAYER_REPLY);
		TSendMsg(pSession, header, msg);
	}
};