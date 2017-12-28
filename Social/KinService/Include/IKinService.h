/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\MatchServer\Include\IKinService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/15/2015	17:50
** ��  ��:	1.0
** ��  ��:	
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "KinDef.h"
using Gateway::ClientID;
struct IKin;

struct IKinService
{

    // �����������������͹�������Ϣ
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void *data, size_t len) = 0;

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void *data, size_t len) = 0;

    /** ����ս��ID��ȡKin�ӿ�
	@param dwKinID		:ս��ID
	*/
	virtual IKin*	getKin(DWORD dwKinID) = 0;

    /** �ⲿ�ӿ�ɾ��ս�ӳ�Ա
	@param dwKinID		:ս��ID 
	*/
    virtual bool    deleteKinMember(DWORD dwKinID, DWORD ActorID) =0;

    /** ����ս��ID��ȡKin����
	@param dwKinID		:ս��ID
	*/
	virtual SKinInfo getKinInfo(DWORD dwKinID) = 0;

    /** ����ս��ID��ȡ��Ա���ݽӿ�
	@param dwKinID		:ս��ID
	*/
	virtual SKinMember getKinMemberInfo(DWORD dwPBID) = 0;

    /** ����ս��ID��ȡ��Ա�б�ӿ�
	@param dwKinID		:ս��ID 
    @return obuf (�����е�һ��DWORD �������Ա���� >= 0)
	*/
	virtual DWORD   getKinMemberList(DWORD dwKinID,PDBID* pReturnArray, DWORD dwArrayMaxSize) = 0;

    /** ��ȡս�ӳ�Ա����
	@param dwKinID		:ս��ID 
	*/
	virtual DWORD   getKinMemberCount(DWORD dwKinID) = 0;

    /// purpose: ����ս�ӹ��׶�
    virtual bool    addClanCtrb(DWORD dwPDBID, int nClanCtrb) = 0;

    // ��ȡ��ս��
    virtual DWORD   getTotalFightScore(DWORD dwKinID) = 0;

	/// purpose: ����ս�ӱ�������
	virtual void addKinLegendAward(SKinCupAwardNode sAwardNode) = 0;

	/// purpose: ����ս�ӱ���ս��
	virtual void addKinLegendWarResult(SKinWarRecordNode sRecordNode) = 0;

	// purpose:  ս�ӱ�����������
	virtual void addKinLegendGlory(int nKinID, int nNum, int nGloryType) = 0;

	// purpose: ȡս�ӱ�������
	virtual int getKinLegendGlory(int nKinID, int nGloryType) = 0;

	/// purpose: ��ȡ�ܻ�Ծ��
	virtual int getWeekActivity(DWORD dwKinID) = 0;
};