/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\MatchServer\Include\ILegendCupService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	10/15/2015	17:50
** ��  ��:	1.0
** ��  ��:	
            ����ϵͳ�����ṩ�Ľӿ�
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "LegendCupMsgDef.h"
#include "LegendCupDef.h"
using Gateway::ClientID;

struct ILegendCupService
{
    // �����������������͹�������Ϣ
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg) = 0;

    // ����ͻ��˷��͹�������Ϣ
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg) = 0;

	// ���ñ����������
	virtual void    setLegendCupMatchResult(SMsgRecordNodeWarResult sNodeWarResult)= 0;

    // ���ñ��������ڵ�״̬
    virtual void    setLegendCupNodeState(SMsgSetCompetitionNodeState sNodeState)= 0;

    // �˳����ڱ����Ľڵ�
    virtual void    exitStartCompetitionNode(LONGLONG llLegendCupID, int nSerchID, DWORD dwNewStartTime)= 0;

    // ָ���μ�ĳ������ս���б������׶���Ч��
    virtual void    luaAddCupRegister(LONGLONG llLegendCupID, PACKAGE_PTR pkKinList) = 0;

    // �Ƴ��������в���ս���б������׶���Ч��
    virtual void    luaRemoveCupRegister(LONGLONG llLegendCupID) = 0;

    // �Ƴ����������¼����Ϣ
    virtual void    endCupRemoveCreaterRecInfo(LONGLONG llLegendCupID) = 0;

    // ͨ�������߻�ȡ����������Ϣ
    virtual SCreaterLegendCupInfo getCreaterLegendCupInfo(BYTE byCupCreateType,DWORD dwCreaterID) = 0;

	// ��ȡ������˱����Ľ�������
	virtual int		getPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID) = 0;

	// ����������˱����Ľ�������
	virtual bool	setPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID, int nRecvPrizeCount) = 0;

    // �Ƿ����ڲμӱ���
    virtual bool	isKinInLegendCup(DWORD dwKinID) = 0;

    // �Ƴ��������������Ϣ
    virtual void    removeServiceCupInfo(LONGLONG llLegendCupID) = 0;
};