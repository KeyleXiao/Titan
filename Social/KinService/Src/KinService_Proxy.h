/*******************************************************************
** �ļ���:	E:\speed\Social\KinServer\Src\KinService_Proxy.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/16/2015	10:50
** ��  ��:	1.0
** ��  ��:	ս�Ӵ���
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "IKinService.h"


using namespace rkt;
using namespace std;

class KinService_Proxy:public IKinService
{
public:
	///////////////////////////////IKinService///////////////////////////////////////////

    // �����������������͹�������Ϣ
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len);

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void * data, size_t len);


     /** ����ս��ID��ȡKin�ӿ�
	@param dwKinID		:ս��ID
	*/
    virtual IKin*   getKin(DWORD dwKinID);

        
    /** ����ս��ID��ȡKin����
	@param dwKinID		:ս��ID
	*/
	virtual SKinInfo getKinInfo(DWORD dwKinID);

    /** ��ȡս�ӳ�Ա���ݽӿ�
	@param dwKinID		:ս��ID
	*/ 
    virtual SKinMember getKinMemberInfo(DWORD dwKinID);

     /** ����ս��ID��ȡ��Ա�б�ӿ�
	@param          dwKinID		:ս��ID
    @param return ս�ӳ�Ա��	:��Ա����
	*/
	virtual DWORD   getKinMemberList(DWORD dwKinID, PDBID* pReturnArray, DWORD dwArrayMaxSize);

    /** �ⲿ�ӿ� ɾ��ս�ӳ�Ա
	@param          dwKinID		:ս��ID
    @param return ս�ӳ�Ա��	:��Ա����
	*/
    virtual bool deleteKinMember(DWORD dwKinID, DWORD ActorID);

     /** ��ȡս�ӳ�Ա����
	@param dwKinID		:ս��ID 
	*/
    virtual DWORD getKinMemberCount(DWORD dwKinID);

    /** ��ȡս�ӳ�Ա����
	@param dwKinID		:ս��ID 
	*/
    virtual DWORD getTotalFightScore(DWORD dwKinID);

    /// purpose: ����ս�ӹ��׶�
    virtual bool  addClanCtrb(DWORD dwPDBID, int nClanCtrb);

	/// purpose: ����ս�ӱ�������
	virtual void addKinLegendAward(SKinCupAwardNode sAwardNode);

	/// purpose: ����ս�ӱ���ս��
	virtual void addKinLegendWarResult(SKinWarRecordNode sRecordNode);

	// purpose:  ս�ӱ�����������
	virtual void addKinLegendGlory(int nKinID, int nNum, int nGloryType);

	// purpose: ȡս�ӱ�������
	virtual int getKinLegendGlory(int nKinID, int nGloryType);

	/// purpose: ��ȡ�ܻ�Ծ��
	virtual int getWeekActivity(DWORD dwKinID);

public:
	SERVICE_PTR m_pContainer;

private:
	void PostMsg(obuf256& oMsg);
};