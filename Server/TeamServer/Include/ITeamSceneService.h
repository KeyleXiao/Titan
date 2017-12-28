/*******************************************************************
** �ļ���:	E:\speed\Server\TeamServer\Include\ITeamSceneService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	����ϵͳ������ �����ṩ�Ľӿ�
            
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "PackagePtr.h"
#include "TeamDef.h"
using Gateway::ClientID;



/// ��
struct IGroup
{
	//////////////////////// IGroup ////////////////////////////////////
	/** ȡ����ID
	@return  ��ID
	*/	
	virtual DWORD						GetGroupID(void) = NULL;

	/** ȡ���ų�PDBID
	@return  �ų�PDBID
	*/
	virtual DWORD						GetCaptain(void) = NULL;

	/** ȡ��������
	@return  ������
	*/
	virtual const char *				GetName(void) = NULL;

	/** �Ƿ�����
	@return  true:����,false:û����
	*/
	virtual bool						IsFull(void) = NULL;

	/** ȡ�ö������
	@return  �������
	*/
	virtual int							GetCount(void) = NULL;

	/**  ָ������ID�Ƿ�������
	@param   int nTeamID	: ����ID
	@return  @return true: ���� false:������
	*/
	virtual bool						ExistTeam(int nTeamID) = NULL;

	/////////////// �Ź��� ///////////////////////////////

	/** �������
	@param   int nTeamID	:	����ID
	@return  �ɹ���
	*/
	virtual bool						Add(int nTeamID) = NULL;

	/** �Ƴ�����
	@param   int nTeamID	:	����ID
	@return  �ɹ���
	*/
	virtual bool						Remove(int nTeamID) = NULL;

	/** ��ɢ
	@return  
	*/
	virtual void						Clear(void) = NULL;

	/** �����ų�
	@param   DWORD dwPlayerID : Ҫ�������ų���ɫID
	@return   �ɹ���
	*/
	virtual bool						SetCaptain(DWORD dwPlayerID) = NULL;

	/** ����ʰȡģʽ
	@param   
	@param   
	@return  
	*/
	virtual void						SetPickMode(DWORD dwPickMode) = NULL;

	/** ��ȡʰȡģʽ
	@param   
	@param   
	@return  
	*/
	virtual DWORD						GetPickMode() = NULL;

};


/// ��
struct IGroupManager
{

	/** ��IDȡ���Žӿ�ָ��
	@param   DWORD dwGroupID	: ��ID
	@param   
	@return  IGroup * �Žӿ�ָ��
	*/
	virtual IGroup *				GetGroupByID(DWORD dwGroupID) = NULL;

	/** ���ų���ɫIDȡ���Žӿ�ָ��
	@param   DWORD dwPDBID		: �ų���ɫID
	@param   
	@return  IGroup * �Žӿ�ָ��
	*/
	virtual IGroup *				GetGroupByCaptain(DWORD dwPDBID) = NULL;

	/** �ý�ɫIDȡ���Žӿ�ָ��
	@param   DWORD dwPDBID		: ��ɫID
	@param   
	@return  IGroup * �Žӿ�ָ��
	*/
	virtual IGroup *				GetGroupByPerson(DWORD dwPDBID) = NULL;

	/** �ö���IDȡ���Žӿ�ָ��
	@param   DWORD dwTeamID		: ����ID
	@param   
	@return  IGroup * �Žӿ�ָ��
	*/
	virtual IGroup *				GetGroupByTeam(DWORD dwTeamID) = NULL;
};

//////////////////////////////////////////////////////////////////////////

/// ��ӷ�����
struct ITeam
{
	/** ����ID
	@param   
	@param   
	@return  
	*/
	virtual int						GetTeamID(void) = NULL;

	/** �ӳ�PDBID
	@param   
	@param   
	@return  
	*/
	virtual DWORD					GetCaptain(void) = NULL;

	/** ʰȡģʽ
	@param   
	@param   
	@return  
	*/
	virtual int						GetPickupMode(void) = NULL;

	/** diceģʽ��Ϣ
	@param   
	@param   
	@return  
	*/
	virtual SDiceModeInfo *			GetDiceModeInfo(void) = NULL;

	/** �Ƿ�����
	@param   
	@param   
	@return  
	*/
	virtual bool					IsMemberFull(void) = NULL;

	/** ��Ա����
	@param   
	@param   
	@return  
	*/
	virtual int						GetMemberCount(void) = NULL;

	/** �����б�
	@param   
	@param   
	@return  
	*/
	virtual TLIST_MEMBERINFO *		GetMemberList(void) = NULL;	


	/** ��Ա��Ϣ
	@param   
	@param   
	@return  
	*/
	virtual STeamMemberInfo *		GetMemberInfo(DWORD dwPDBID) = NULL;

};

struct ITeamServer
{	
	/** �ͷ�
	@param   
	@param   
	@param   
	@return  
	@note     �ͷ�������Դ���������ٴ˶���
	@warning 
	@retval buffer 
	*/
	virtual void					Release(void) = NULL;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ITeam *					GetTeamByTeamID(int nTeamID) = NULL;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ITeam *					GetTeamByPerson(DWORD dwPDBID) = NULL;


	/** 
	@param   
	@param   
	@return  
	*/
	virtual IGroupManager *			GetGroupManagerInterface(void) = NULL;

	/** �������������飨д�����﷽���ģ����ã�
	@param   
	@param   
	@return  
	*/
	virtual void					SysJoinTeam(int nTeamID, bool bIsCaptain, DWORD dwPlayer) = NULL;

	/** ����ͳ�����ݵ�����
	@param	nTeamID		: ����ID
	@param	bIsAdd		: true����ӣ�false�����
	@param  wFlagStatus	: Flagֵ
	*/
	virtual void					UpdateTeamFlagToSocial(int nTeamID, bool bIsAdd, WORD wFlagStatus ) = NULL;
};


struct  ITeamSceneService
{
    // �����������������͹�������Ϣ
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, void *data, size_t len) = 0;

    // ����ͻ��˷��͹�������Ϣ
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, void *data, size_t len) = 0;

	// ����������Ϣ
	virtual void    sendToSocial(BYTE nMsgID, void * data, size_t len) = 0;

    /** ��ȡ���TeamID
	@param   
	@param   
	@return  
	*/
	virtual DWORD   getTeamIDByPlayer( PDBID PlayerID) = NULL;

    /** ����TeamID��ȡ��Ա�б�ӿ�
	@param          dwTeamID	:ս��ID
    @param return Team��Ա��	:��Ա����
	*/
	virtual DWORD   getTeamMemberList(DWORD dwTeamID, STeamMemberInfo* pReturnArray, DWORD dwArrayMaxSize) = NULL;

	/** �������������飨д�����﷽���ģ����ã�
	@param   
	@param   
	@return  
	*/
	virtual void    SysJoinTeam(int nTeamID, bool bIsCaptain, DWORD dwPlayer) = NULL;

    /** �ⲿ�ӿڣ��˳����飩
    @param   
	@param   
	@return  
	*/
	virtual void    leaveTeam(PDBID dwActorID) = NULL;

};