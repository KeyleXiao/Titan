/*******************************************************************
** �ļ���:	IClanService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	3/31/2016	16:57
** ��  ��:	1.0
** ��  ��:	������ӿ�
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
using Gateway::ClientID;

struct IClanService
{

    virtual void release() = 0;

    // �����������������͹�������Ϣ
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg) = 0;

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg) = 0;

    // ����ս���������ID
    virtual DWORD findKinClan(DWORD dwKinID) = 0;

    // �Ƴ����ڶ�ȡ��ս��
	virtual void removeReadingKin(DWORD dwKinID) = 0;

	// ��ȡ����Ա�б�
	virtual int getClanKinList(DWORD dwClanID, DWORD *pKinList, int nLen) = 0;

    // ȡ�ð����ֵ
    virtual int getClanNumProp(DWORD dwClanID, int nNumIndex) = 0;

	// ��ȡ�������
	virtual string getClanName(DWORD dwClanID) = 0;

	// ���˱�������
	virtual void notifyCreateClanLegendCup(DWORD dwClanID, LONGLONG llLegendCupID, int nMaxKinCount, LPCSTR pCupName) = 0;

};