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

		// 1.0 先验证
		// 1.1 查找预置的登陆验证信息
		AuthNode* pNode = gAuthMng.Find(dwPlayerID);
		if (pNode==nullptr)
		{
			// 通知客户端：这里没有你的信息，不允许登陆
			NotifyFailAndDisconnect(pClient, E_HANDSHAKE_FAIL_NO_ID);
			return;
		}

		// 1.2 查找指定的ViewServer
		auto pView = gViewContainer.Get(pNode->m_info.wViewID);
		if (pView==nullptr)
		{
			gAuthMng.Remove(dwPlayerID, E_AuthDelR_NOT_REACH_VIEW);
			NotifyFailAndDisconnect(pClient, E_HANDSHAKE_FAIL_NOT_REACH_VIEW);
			return;
		}

		// 1.3 验证key
		if (!CheckKeyData(pNode->m_info.szKeyData, pMsg->szKey))
		{
			NotifyFailAndDisconnect(pClient, E_HANDSHAKE_FAIL_ERR_KEY);
			return;
		}

		// 2.0 尝试加入正式的容器中
		if (!gClientContainer.OnSessionRcvID(pClient, dwPlayerID))
		{
			NotifyFailAndDisconnect(pClient, E_HANDSHAKE_FAIL_IS_FULL);
			return;
		}
		pClient->SetMacAddress(pMsg->szMacAddress);// 保存mac地址
		pClient->SetViewID(pView->GetID());

		// 3.0 回复握手消息
		pClient->SendMsg(SMsgView_GC_HandshakeResponse());

		// 4.0 通知View，某Client要观看某战报，以某种权限
		{
			SMsgView_GV_CRequestReplay msg;
			msg.reqMsg = pNode->m_info.reqMsg;
			pView->SendMsg(msg);
		}

		// 5.0 通知Mng，玩家登陆成功（此时需要将玩家从Auth列表中移除，加入到登陆成功列表中）
		gAuthMng.Remove(dwPlayerID, E_AuthDelR_HANDSHAKE);// 这里执行完，pNode就被析构了
	}

private:
	// 通知客户端失败原因，并断开连接
	void NotifyFailAndDisconnect(ClientSession* pSession, EnumHandshakeFail eFailReason)
	{
		SMsgView_GC_HandshakeFail msg;
		msg.eFailReason = eFailReason;
		pSession->SendMsg(msg);

		// 断开连接
		pSession->Disconnect();
	}

	// 校验key
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