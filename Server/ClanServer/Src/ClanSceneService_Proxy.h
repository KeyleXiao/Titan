/*******************************************************************
** �ļ���:	ClanSceneService_Proxy.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	4/6/2016
** ��  ��:	1.0
** ��  ��:	��᳡������
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "buffer.h"
#include "IClanSceneService.h"
#include "ClanSceneService.h"

using namespace rkt;

class ClanSceneService_Proxy : public IClanSceneService
{
public:
	//IClanSceneService////////////////////////////////////////////////////////////////////////
	// ��ȡ�������
	virtual string getClanName(DWORD nClanID);

    // �����������������͹�������Ϣ
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // ����ͻ��˷��͹�������Ϣ
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

    virtual int getNumProp(DWORD dwClaneID, DWORD dwProp);

	virtual bool setNumProp(DWORD dwClanID, DWORD dwProp, int nValue);

	// ��鴴�����˱���
	virtual bool checkCreateClanLegendCup(DWORD dwClanID,int LegendCupConfigID);

public:
	SERVICE_PTR    m_pContainer;

};
