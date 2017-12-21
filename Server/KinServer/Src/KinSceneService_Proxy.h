/*******************************************************************
** �ļ���:	E:\speed\Server\KinServer\Src\KinSceneService_Proxy.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	ս�� ����
********************************************************************/

#pragma once


#include "IServiceContainer.h"
#include "buffer.h"
#include "IKinSceneService.h"
#include "KinSceneService.h"


using namespace rkt;


class KinSceneService_Proxy : public IKinSceneService
{
public:
	/////////////////////////////IKinSceneService/////////////////////////////////////////////
	//virtual void start();
    // �����������������͹�������Ϣ
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

	// purpose: ����������Ϣ
	virtual void sendToSocial(BYTE nMsgID, void * data, size_t len);

    // purpose: ���ݳ�Ա��PDBID����ս�ӳ�Ա
    virtual SSceneKinMember getKinMember(DWORD dwPDBID);

    /** ����ս��ID��ȡKin����
	@param dwKinID		:ս��ID
	*/
	virtual SKinInfo getKinInfo(DWORD dwKinID);

    /** ����ս��ID��ȡ��Ա�б�ӿ�
	@param dwKinID		:ս��ID 
    @return obuf (�����е�һ��DWORD �������Ա���� >= 0)
	*/
	virtual DWORD   getKinMemberList(DWORD dwKinID,PDBID* pReturnArray, DWORD dwArrayMaxSize);

    /** ��ȡս�ӳ�Ա����
	@param dwKinID		:ս��ID 
	*/
	virtual DWORD   getKinMemberCount(DWORD dwKinID);

	/** �����ܻ�Ծ��
	* @param dwKinID  		:ս��ID
	* @param nValue		    :�ܻ�Ծ��ֵ
	*/
	virtual bool setWeekActivity(DWORD dwKinID, int nValue);

	/** ��ȡ�ܻ�Ծ��
	* @param dwKinID		:ս��ID
	*/
	virtual int getWeekActivity(DWORD dwKinID);

public:
	SERVICE_PTR    m_pContainer;
};
