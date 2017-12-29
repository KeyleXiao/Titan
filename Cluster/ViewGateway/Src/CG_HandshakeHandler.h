#pragma once
#include "Handler.h"
#include "ViewMsgDef_Client.h"
#include "ClientSession.h"
#include "ClientContainer.h"
#include "MsgFactory.h"
#include "AuthMng.h"
#include "ViewContainer.h"
#include "ViewSession.h"


class ClientSession;

class CG_HandshakeHandler : public Handler<ClientSession, SMsgView_CG_Handshake>
{
protected:
	virtual void Handle(ClientSession* pClient, SGameMsgHead* head, SMsgView_CG_Handshake* pMsg)
	{
		PlayerID& dwPlayerID = pMsg->dwPlayerID;

		// 1.0 ����֤
		// 1.1 ����Ԥ�õĵ�½��֤��Ϣ
		AuthNode* pNode = gAuthMng.Find(dwPlayerID);
		if (pNode==nullptr)
		{
			// ֪ͨ�ͻ��ˣ�����û�������Ϣ���������½
			NotifyFailAndDisconnect(pClient, E_HANDSHAKE_FAIL_NO_ID);
			return;
		}

		// 1.2 ����ָ����ViewServer
		auto pView = gViewContainer.Get(pNode->m_info.wViewID);
		if (pView==nullptr)
		{
			gAuthMng.Remove(dwPlayerID, E_AuthDelR_NOT_REACH_VIEW);
			NotifyFailAndDisconnect(pClient, E_HANDSHAKE_FAIL_NOT_REACH_VIEW);
			return;
		}

		// 1.3 ��֤key
		if (!CheckKeyData(pNode->m_info.szKeyData, pMsg->szKey))
		{
			NotifyFailAndDisconnect(pClient, E_HANDSHAKE_FAIL_ERR_KEY);
			return;
		}

		// 2.0 ���Լ�����ʽ��������
		if (!gClientContainer.OnSessionRcvID(pClient, dwPlayerID))
		{
			NotifyFailAndDisconnect(pClient, E_HANDSHAKE_FAIL_IS_FULL);
			return;
		}
		pClient->SetMacAddress(pMsg->szMacAddress);// ����mac��ַ
		pClient->SetViewID(pView->GetID());

		// 3.0 �ظ�������Ϣ
		pClient->SendMsg(SMsgView_GC_HandshakeResponse());

		// 4.0 ֪ͨView��ĳClientҪ�ۿ�ĳս������ĳ��Ȩ��
		{
			SMsgView_GV_CRequestReplay msg;
			msg.reqMsg = pNode->m_info.reqMsg;
			pView->SendMsg(msg);
		}

		// 5.0 ֪ͨMng����ҵ�½�ɹ�����ʱ��Ҫ����Ҵ�Auth�б����Ƴ������뵽��½�ɹ��б��У�
		gAuthMng.Remove(dwPlayerID, E_AuthDelR_HANDSHAKE);// ����ִ���꣬pNode�ͱ�������
	}

private:
	// ֪ͨ�ͻ���ʧ��ԭ�򣬲��Ͽ�����
	void NotifyFailAndDisconnect(ClientSession* pSession, EnumHandshakeFail eFailReason)
	{
		SMsgView_GC_HandshakeFail msg;
		msg.eFailReason = eFailReason;
		pSession->SendMsg(msg);

		// �Ͽ�����
		pSession->Disconnect();
	}

	// У��key
	bool CheckKeyData(BYTE szKeyData[], BYTE szKey[])
	{
		for (size_t i = 0; i < VIEW_SECRET_KEY_SIZE; i++)
		{
			if (szKeyData[i] != szKey[i])
				return false;
		}
		return true;
	}

};