#pragma once
#include "Handler.h"
#include "ViewMsgDef.h"


class ViewSession;
struct SMsgView_VG_Handshake;

class HandshakeHandler : public Handler<ViewSession, SMsgView_VG_Handshake>
{
protected:
	virtual void Handle(ViewSession* pSession, SGameMsgHead* head, SMsgView_VG_Handshake* pMsg);

};