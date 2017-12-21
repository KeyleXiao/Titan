/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\MatchServer\Include\IMatchService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	10/15/2015	17:50
** ��  ��:	1.0
** ��  ��:	����ϵͳ�����ṩ�Ľӿ�

			ʹ�÷�����
            EMChatTipID����EMChatTipIDö���ж�����µ�ö��ֵ����SystemMessage.csv��������Ӧ�Ĳ�������ʾλ�á����������ȣ�
			EMChatBroadcastType: �ò��������˸���Ϣ��㲥�ķ�Χ�����硢���塢ս�ӡ����˵ȣ�
			EMChatChannelType: �ò��������˹㲥���������ĸ�Ƶ��
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "ChatDef.h"
#include "SharePropertyDef.h"
using Gateway::ClientID;

struct IChatService
{
    // �����������������͹�������Ϣ
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg) = 0;

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg) = 0;

	// ����ϵͳ�㲥(�����÷ֺŷָ�)
	// @param tipID: EMChatTipID(SystemMessage.csv��������Ӧ)
	// @param broadType: EMChatBroadcas
	// @param broadTargetID: ����broadType����������忴EMChatBroadcastType����tType
	// ������SystemMessage.csv�ű�������tips��λ��
	virtual void broadcastSystemMessage(int broadType, int broadTargetID, int tipID, LPCSTR szParam = NULL)  = 0;

	// αװ��ɫ������Ϣ(�����÷ֺŷָ�, ֻ�����ض���ɫʱ����Ŀ��PDBID)
	// @param tipID��EMChatTipID����
	virtual void broadcastActorTipMessage(SSharePersonProperty actorInfo, int nChannel, int tipID, LPCSTR szParam = NULL, PDBID nTargetID = 0) = 0;

	// �㲥������Ϣ
	// @param isAdd: true(������,false(����) 
	// @param szParam: �����ַ���
	// @param nCount: �㲥����
	// @param nInternal: �㲥���
	virtual void broadcastADMessage(bool isAdd, LPCSTR szParam = NULL, int nCount = 1, int nInternal = 0) = 0;

	// �㲥��Ϣ�������(�����÷ֺŷָ�)
	// @param tipID: EMChatTipID(SystemMessage.csv��������Ӧ)
	// @param broadType: EMChatBroadcastType
	// @param broadTargetID: ����broadType����������忴EMChatBroadcastType����
	// ������Ҫ��SystemMessage.csv�ű�������tips��λ��Ϊ�����λ��
	virtual void broadcastSystemMessageToChatBox(int broadType, int broadTargetID, int tipID, int chatChannel, LPCSTR szParam = NULL) = 0;
};