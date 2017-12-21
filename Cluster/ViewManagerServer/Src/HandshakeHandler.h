#pragma once
#include "Handler.h"
#include "ViewMsgDef_Server.h"

class ViewUser;

// ViewServer������������Ϣ
class HandshakeHandler : public Handler<ViewUser, SMsgView_VM_Handshake>
{
protected:
	virtual void Handle(ViewUser* pSession, SGameMsgHead* head, SMsgView_VM_Handshake* pMsg);
};