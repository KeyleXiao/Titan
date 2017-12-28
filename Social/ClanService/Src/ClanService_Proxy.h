/*******************************************************************
** �ļ���:	ClanService_Proxy.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	3/31/2016	17:19
** ��  ��:	1.0
** ��  ��:	���������
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "IClanService.h"

using namespace rkt;
using namespace std;

class ClanService_Proxy : public IClanService
{
public:
	////////////////// ClanService_Proxy ////////////////////////////////////////////////////////

	ClanService_Proxy() :m_pContainer(0) {};
	~ClanService_Proxy() {};
public:
	///////////////////////////////////IClanService////////////////////////////
    virtual void release();

    // �����������������͹�������Ϣ
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void *data, size_t len);

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void *data, size_t len);

	// ����ս���������ID
	virtual DWORD findKinClan(DWORD dwKinID);

    // �Ƴ����ڶ�ȡ��ս��
	virtual void removeReadingKin(DWORD dwKinID);

	// ��ȡ����Ա�б�
	virtual int getClanKinList(DWORD dwClanID, DWORD *pKinList, int nLen);

    // ȡ�ð����ֵ
    virtual int getClanNumProp(DWORD dwClanID, int nNumIndex);

	// ��ȡ�������
	virtual string getClanName(DWORD dwClanID);

	// ���˱�������
	virtual void notifyCreateClanLegendCup(DWORD dwClanID, LONGLONG llLegendCupID, int nMaxKinCount, LPCSTR pCupName);
public:
	SERVICE_PTR m_pContainer;
};