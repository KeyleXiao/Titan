/*******************************************************************
** �ļ���:	E:\speed\Server\KinServer\Include\IKinSceneService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	����ϵͳ������ �����ṩ�Ľӿ�
            
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "PackagePtr.h"
#include "IKinScene.h"
#include "KinDef.h"
using Gateway::ClientID;

struct  IKinSceneService
{
	//virtual void start() = 0;

    // �����������������͹�������Ϣ
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len) = 0;

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void * data, size_t len) = 0;

	// purpose: ����������Ϣ
	virtual void sendToSocial(BYTE nMsgID, void * data, size_t len) = 0;

    // purpose: ���ݳ�Ա��PDBID����ս�ӳ�Ա
    virtual SSceneKinMember getKinMember(DWORD dwPDBID) = 0;

	///** ����DB�洢���̴���ս��
	//@param	pdbid		: ��ҽ�ɫID
	//@param	szKinName	: ս����
	//@param	bCrtAsFormal: �Ƿ񴴽�Ϊ��ʽս��
	//@return
	//*/
	//virtual void DBProcCreateKin(DWORD pdbid, const char* szKinName, bool bCrtAsFormal = false) = 0;

    /** ����ս��ID��ȡKin����
	@param dwKinID		:ս��ID
	*/
	virtual SKinInfo getKinInfo(DWORD dwKinID) = 0;

    /** ����ս��ID��ȡ��Ա�б�ӿ�
	@param dwKinID		:ս��ID 
    @return obuf (�����е�һ��DWORD �������Ա���� >= 0)
	*/
	virtual DWORD   getKinMemberList(DWORD dwKinID,PDBID* pReturnArray, DWORD dwArrayMaxSize) = 0;

    /** ��ȡս�ӳ�Ա����
	@param dwKinID		:ս��ID 
	*/
	virtual DWORD   getKinMemberCount(DWORD dwKinID) = 0;

	/** �����ܻ�Ծ��
	* @param dwKinID		:ս��ID
	* @param nValue		    :�ܻ�Ծ��ֵ
	*/
	virtual bool setWeekActivity(DWORD dwKinID, int nValue) = 0;

	/** ��ȡ�ܻ�Ծ��
	* @param dwKinID		:ս��ID
	*/
	virtual int getWeekActivity(DWORD dwKinID) = 0;
};
