/*******************************************************************
** �ļ���:	ISNSService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���к�
** ��  ��:	12/6/2016
** ��  ��:	1.0
** ��  ��:	SNS����
********************************************************************/

#pragma once

struct ISNSService
{
	// ��SNSServer������Ϣ
	virtual void recv_message(int msgID, const char* data, int nLen) = 0;

	// ֱ�ӼӺ���
	virtual void addBuddy(DWORD dwUserID1, DWORD dwUserID2, const char* szActorName1, const char* szActorName2) = 0;

	// ����һ����Ϸ
	// @param dwTargetActorID : ��������ActorID
	// @param dwRoomID : ����ID
	// @param szInviterName : ����������
	virtual void invitePlayGame(DWORD dwInviterActorID, DWORD dwTargetActorID, DWORD dwRoomID) = 0;
};