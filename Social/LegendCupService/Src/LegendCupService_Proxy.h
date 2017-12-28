/*******************************************************************
** �ļ���:	E:\speed\Social\LegendCupServer\Src\LegendCupService_Proxy.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/16/2015	10:50
** ��  ��:	1.0
** ��  ��:	��������
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "ILegendCupService.h"


using namespace rkt;
using namespace std;

class LegendCupService_Proxy:public ILegendCupService
{
public:
	// ILegendCupService
	//////////////////////////////////////////////////////////////////////////
    // �����������������͹�������Ϣ
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen);

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void* pData, size_t nLen);

	// ���ñ����������
	virtual void setLegendCupMatchResult(SMsgRecordNodeWarResult sNodeWarResult);

    // ���ñ����ڵ�״̬
    virtual void setLegendCupNodeState(SMsgSetCompetitionNodeState sNodeState);

    // ָ���μ�ĳ������ս���б������׶���Ч��
    virtual void luaAddCupRegister(LONGLONG llLegendCupID, PACKAGE_PTR pkKinList);

    // �Ƴ��������в���ս���б������׶���Ч��
    virtual void luaRemoveCupRegister(LONGLONG llLegendCupID);

    // �˳����ڱ����Ľڵ�
    virtual void exitStartCompetitionNode(LONGLONG llLegendCupID, int nSerchID, DWORD dwNewStartTime);

	// ��Ȩս���ʼ���ʾ
	virtual void sendAbstentionKinMail(LONGLONG llLegendCupID, DWORD dwFailedKinID);

    // �Ƴ����������¼����Ϣ
    virtual void endCupRemoveCreaterRecInfo(LONGLONG llLegendCupID);

    // �Ƴ��������������Ϣ
    virtual void    removeServiceCupInfo(LONGLONG llLegendCupID);

    // �õ����˱�����Ϣ
    virtual SCreaterLegendCupInfo getCreaterLegendCupInfo(BYTE byCupCreateType, DWORD dwCreaterID);

	// ��ȡ������˱����Ľ�������
	virtual int		getPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID);

	// ����������˱����Ľ�������
	virtual bool	setPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID, int nRecvPrizeCount);

    // �Ƿ��ڲμӱ���
    virtual bool	isKinInLegendCup(DWORD dwKinID);

public:
	SERVICE_PTR m_pContainer;

private:
	void PostMsg(obuf256& oMsg);
};