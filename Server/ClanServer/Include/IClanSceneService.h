/*******************************************************************
** �ļ���:	IClanSceneService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	4/6/2016
** ��  ��:	1.0
** ��  ��:	��᳡������ӿ�
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "PackagePtr.h"
#include "IClanScene.h"
#include "NetMessageDef.h"
using Gateway::ClientID;

struct IClanSceneService
{
	// ��ȡ�������
	virtual string getClanName(DWORD nClanID) = 0;

    // �����������������͹�������Ϣ
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, void *data, size_t len) = 0;

    //// ����ͻ��˷��͹�������Ϣ
    //virtual void    handleClientMsg(DWORD client, SNetMsgHead head, void *data, size_t len) = 0;

    virtual int getNumProp(DWORD dwClaneID, DWORD dwProp) = 0;

	virtual bool setNumProp(DWORD dwClanID, DWORD dwProp, int nValue) = 0;

	// ��鴴�����˱���
	virtual bool checkCreateClanLegendCup(DWORD dwClanID, int LegendCupConfigID) = 0;
};
