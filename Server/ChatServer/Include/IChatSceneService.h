/*******************************************************************
** �ļ���:	E:\speed\Server\ChatServer\Include\IChatSceneService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 
** ������:	����
** ��  ��:	10/16/2015	15:11
** ��  ��:	1.0
** ��  ��:	����ϵͳ������ �����ṩ�Ľӿ�
            
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "PackagePtr.h"
#include "ChatDef.h"
using Gateway::ClientID;

struct IChatSceneService
{
	// ����ϵͳ�㲥(�����÷ֺŷָ�)
	// @param tipID: EMChatTipID(SystemMessage.csv��������Ӧ)
	// @param broadType: EMChatBroadcast
	// @param broadTargetID: ����broadType����������忴EMChatBroadcastType����tType
	// ������SystemMessage.csv�ű�������tips��λ��
	virtual void broadcastSystemMessage(int broadType, int broadTargetID, int tipID, LPCSTR szParam = NULL, int nDelayTime = 0, WORD nReserveData = 0) = 0;

	// �㲥��Ϣ�������(�����÷ֺŷָ�)
	// @param tipID: EMChatTipID(SystemMessage.csv��������Ӧ)
	// @param broadType: EMChatBroadcastType
	// @param broadTargetID: ����broadType����������忴EMChatBroadcastType����
	// ������Ҫ��SystemMessage.csv�ű�������tips��λ��Ϊ�����λ��
	virtual void broadcastSystemMessageToChatBox(int broadType, int broadTargetID, int tipID, int chatChannel, LPCSTR szParam = NULL, int nDelayTime = 0, WORD nReserveData = 0) = 0;

	// ����˵��
	virtual void broadcastMonsterMessage(UID &uid, int msgIndex) const = 0;

	// αװ��ɫ������Ϣ(�����÷ֺŷָ�, ֻ�����ض���ɫʱ����Ŀ��PDBID)
	virtual void broadcastActorTipMessage(PDBID nActorID, LPCSTR szActorName, int nChannel, int nChannelTargetID, EMChatTipID tipID, LPCSTR szParam = NULL, PDBID nTargetID = 0) = 0;

    // �����������������͹�������Ϣ
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, const char* pData, size_t nLen) = 0;

    // ����ͻ��˷��͹�������Ϣ
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, const char* pData, size_t nLen) = 0;

};
