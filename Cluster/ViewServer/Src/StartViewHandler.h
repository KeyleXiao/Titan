#pragma once
#include "Handler.h"
#include "MngConnector.h"
#include "ViewMsgDef_Server.h"
#include "ReplayContainer.h"
#include "Replay.h"


class StartViewHandler : public Handler<MngConnector, SMsgView_MV_StartView>
{
protected:
	virtual void Handle(MngConnector* pMng, SGameMsgHead* head, SMsgView_MV_StartView* pMsg)
	{
		const ReplayID&	rID = pMsg->rID;// ս��ID
		Replay* pReplay = gReplays.Get(rID);
		if (pReplay!=nullptr)
		{
			Assert(false);
			return;
		}

		// �����ڵ�
		pReplay = gReplays.Insert(rID);

		pReplay->Start();
	}
};