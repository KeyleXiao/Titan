/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\MatchServer\Include\IMatchService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	10/15/2015	17:50
** ��  ��:	1.0
** ��  ��:	
            ����ϵͳ�����ṩ�Ľӿ�
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
using Gateway::ClientID;

/*******************************************************************
** �ļ���:	e:\Rocket\SocialServer\TeamService\Include\ITeamService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	¡����(Forlion Long)��Forlion@126.com��
** ��  ��:	2008/6/11  21:15
** ��  ��:	1.0
** ��  ��:	��ӷ���	
** Ӧ  ��:	

**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once

#include "TeamDef.h"
#include "ActorDef.h"


/*! \addtogroup TeamService
*  ��ӷ���
*  @{
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// ��
struct IGroup
{

	//////////////////////// IGroup ////////////////////////////////////

	/**  ָ���б���ǰ��
	@return  
	*/
	virtual void						MoveHead(void) = NULL;

	/**  ָ���б�����
	@return  
	*/
	virtual void						MoveEnd(void) = NULL;

	/**  ָ���б���һ��
	@return  @return true: û����β false:����β
	*/
	virtual bool						MoveNext(void) = NULL;

	/**  ȡ���б�ǰָ�������
	@return  
	*/
	virtual const SGroupMemberInfo *	GetListData(void) = NULL;

	/**  ָ���� ָ���б���ǰ��
	@param   DWORD dwIndex	:   ����� 1~GROUP_MAX_GROUP_INDEX
	@return  
	*/
	virtual void						GroupMoveHead(DWORD dwIndex) = NULL;

	/**  ָ���� ָ���б�����
	@param   DWORD dwIndex	:   ����� 1~GROUP_MAX_GROUP_INDEX
	@return  
	*/
	virtual void						GroupMoveEnd(DWORD dwIndex) = NULL;

	/**  ָ���� ָ���б���һ��
	@param   DWORD dwIndex	:   ����� 1~GROUP_MAX_GROUP_INDEX
	@return  @return true: û����β false:����β
	*/
	virtual bool						GroupMoveNext(DWORD dwIndex) = NULL;

	/**  ָ���� ȡ���б�ǰָ�������
	@param   DWORD dwIndex	:   ����� 1~GROUP_MAX_GROUP_INDEX
	@return  
	*/
	virtual const SGroupMemberInfo *	GroupGetListData(DWORD dwIndex) = NULL;

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

	/** �Ƿ�ͳ��
	@return 
	*/
	virtual bool						IsTotal(void) = NULL;

	/** �趨�Ƿ�ͳ��
	@return 
	*/
	virtual bool						SetTotal(bool bFlag) = NULL;

	/** ���ͳ������
	@return 
	*/
	virtual void						ClearTotal(void) = NULL;

	/** ����ͳ������
	@param   DWORD dwPlayerID	: ���ID
	@param  BYTE nType	: ����	EMGroupTotalType
	@param  DWORD dwNumber : ����
	@return 
	*/
	virtual bool						AddTotalInfo(DWORD dwPlayerID,BYTE nType,DWORD dwNumber) = NULL;

	/** ����ͳ�������������״̬
	@param  DWORD dwPlayerID		: ���ID
	@param  BYTE nFlagType			: ָ��λ��ʶ
	@param  bool bFlag				: Ҫ���ñ�ʶ״̬,true���ǣ�false:��
	@return �ɹ���
	*/
	virtual bool						UpdateTotalOnlineState(DWORD dwPlayerID,BYTE nFlagType,bool bFlag) = NULL;

	////// ��Ա�¼� ////////////////////////////////////////////////////////////////////

	/**  ��Ҽ����¼�
	@param   int nTeamID		: ����ID
	@param   DWORD dwPlayerID	: ���ID
	@return
	*/
	virtual void						OnPlayerJoin(int nTeamID,DWORD dwPlayerID) = NULL;

	/**  ����뿪�¼�
	@param   int nTeamID		: ����ID
	@param   DWORD dwPlayerID	: ���ID
	@return
	*/
	virtual void						OnPlayerLeave(int nTeamID,DWORD dwPlayerID) = NULL;

	/**  ���¶ӳ��¼��¼�
	@param   int nTeamID		: ����ID
	@param   DWORD dwPlayerID	: ���ID(ԭ�ӳ�)
	@return
	*/
	virtual void						OnTeamCaptain(int nTeamID,DWORD dwPlayerID) = NULL;

	/**   �����ɢ�¼�
	@param   int nTeamID		: ����ID
	@return
	*/
	virtual void						OnTeamDisband(int nTeamID) = NULL;

	/////////////// �Ź��� ///////////////////////////////

	/** �������
	@param   int nTeamID	:	����ID
	@param   bool bSync		:	�Ƿ�ͬ������
	@return  �ɹ���
	*/
	virtual bool						Add(int nTeamID, bool bSync = true) = NULL;

	/** �Ƴ�����
	@param   int nTeamID	:	����ID
	@param   bool bSync		:	�Ƿ�ͬ������
	@return  �ɹ���
	*/
	virtual bool						Remove(int nTeamID, bool bSync = true) = NULL;

	/** ������
	@param   DWORD dwIndex	:   ����� 1~GROUP_MAX_GROUP_INDEX
	@param   int nTeamID	:	����ID
	@param   bool bSync		:	�Ƿ�ͬ������
	@return  �ɹ���
	*/
	virtual bool						GroupAdd(DWORD dwIndex,int nTeamID) = NULL;

	/** �������Ƴ�����
	@param   DWORD dwIndex	:   ����� 1~GROUP_MAX_GROUP_INDEX
	@param   int nTeamID	:	����ID
	@param   bool bSync		:	�Ƿ�ͬ������
	@return  �ɹ���
	*/
	virtual bool						GroupRemove(DWORD dwIndex,int nTeamID) = NULL;

	/** ��ɢ
	@return  
	*/
	virtual void						Clear(void) = NULL;

	/** �����ų�
	@param   DWORD dwPlayerID : Ҫ�������ų���ɫID
	@param    int nReason	  : ԭ������
	@param    bool bSync	  : �Ƿ�ͬ������
	@return   �ɹ���
	*/
	virtual bool						SetCaptain(DWORD dwPlayerID, int nReason=0, bool bSync=true) = NULL;


	/** ȡ�������г�ԱClientID�б�
	@param   DWORD * pClientArray :ClientID�б�����
	@param   int wClientNum	:	�б������С
	@param   int nTeamID	:	����ID,0Ϊȫ������
	@param   bool bIncludeCaptain	:	�Ƿ�����ų�
	@return  ȡ�õĸ���
	*/
	virtual int							GetMemberClientIDList(DWORD * pClientArray,int wClientNum,int nTeamID=0,bool bIncludeCaptain=true) = NULL;

	/** �����Ż�����Ϣ���ͻ���
	@param	pClientArray: Ҫ���͵Ŀͻ���ID����
	@param	wClientNum  : �ͻ�����Ŀ
	@return  
	*/
	virtual void						SendBaseInfoToClient(ClientID * pClientArray,int wClientNum) = NULL;

	/** �����ŵĶ����Ա��Ϣ���ͻ���
	@param	pClientArray: Ҫ���͵Ŀͻ���ID����
	@param	wClientNum  : �ͻ�����Ŀ
	@return  
	*/
	virtual void						SendTeamInfoToClient(ClientID * pClientArray,int wClientNum) = NULL;

	/** �����ŵ�����Ϣ���ͻ���
	@param	pClientArray: Ҫ���͵Ŀͻ���ID����
	@param	wClientNum  : �ͻ�����Ŀ
	@return  
	*/
	virtual void						SendGroupInfoToClient(ClientID * pClientArray,int wClientNum) = NULL;

	/** �����ŵ�ͳ����Ϣ���ͻ���
	@param	pClientArray: Ҫ���͵Ŀͻ���ID����
	@param	wClientNum  : �ͻ�����Ŀ
	@return  
	*/
	virtual void						SendTotalInfoToClient(ClientID * pClientArray,int wClientNum) = NULL;

	/** �������ж���Ͷ�Ա��Ϣ���ͻ���
	@param	pClientArray: Ҫ���͵Ŀͻ���ID����
	@param	wClientNum  : �ͻ�����Ŀ
	@return  
	*/
	virtual void						SendTeamMemberListToClient(ClientID * pClientArray,int wClientNum) = NULL;

	/** ����ʰȡģʽ
	@param   
	@param   
	@return  
	*/
	virtual void						SetPickMode(DWORD dwPickMode) = NULL;

	/** ��������ģʽ
	@param   
	@param   
	@return  
	*/
	virtual void						SetActionMode(DWORD dwActionMode) = NULL;


	/** �������ģʽ
	@param   
	@param   
	@return  
	*/
	virtual DWORD						GetActionMode() = NULL;

	/** ���õȼ�����
	@param   
	@param   
	@return  
	*/
	virtual void						SetLevelLimit(DWORD dwLevelLimit) = NULL;

	/** ��ȡ�ȼ�����
	@param   
	@param   
	@return  
	*/
	virtual DWORD						GetLevelLimit() = NULL;

	/** ���һ���������г�Ա�ĵȼ��Ƿ������Ŷӵȼ�����
	@param   
	@param   
	@return  ���г�Ա�����㷵��true�����򷵻�false
	*/
	//virtual bool						IsAllTeamMemberLevelValid(int nTeamID, ostrbuf & strBuffOut) = NULL;

	///////////////////////////////���ټ�///////////////////////////////////////////
	/** �������ټ���Ӧ
	@param  dwPlayerID	 �� ���ID
	@param  nConveneType �� ���ټ������� EMGroupConveneType
	@param  nLevel �� �ټ��ȼ�����
	@param  nCostType �� �ټ���������
	@param  szDescr �� ������
	@return  
	*/
	virtual bool						OpenGroupConvene(DWORD dwPlayerID, BYTE nConveneType, int nLevel, int nCostType, const char * szDescr) = NULL;

	/** �ر����ټ�
	@return  
	*/
	virtual bool						CloseGroupConvene(void) = NULL;

	/** �����Ӧ���ټ�
	@param  dwPlayerID	�� ���ID
	@return  
	*/
	virtual void						ResponseConvene(DWORD dwPlayerID) = NULL;

	/** �Զ�������ټ���Ӧ��ҵ�����
	@return  
	*/
	virtual bool						AutoConveneMatchTeam(void) = NULL;

	/** �㲥�ټ���Ϣ
	@return  
	*/
	virtual bool						BroadcastConveneMessage(void) = NULL;

	/** ��Ӧ�������ټ��¼�
	@return  
	*/
	virtual bool						OnEventAddGroupConvene(void) = NULL;

	/** ���������ټ��ټ��¼�
	@return  
	*/
	virtual bool						OpenFreeGroupConvene(void) = NULL;

	/** ��ȡ�Ƿ������ټ�
	@return  
	*/
	virtual bool						IsOpenConvene(void) = NULL;


	/** ��ȡ��������
	@return  
	*/
	virtual int							GetMemberTotal(void) = NULL;

	/** ��ȡ���ټ�����
	@return  
	*/
	virtual const char *				GetConveneDesc(void) = NULL;
};


/// ��
struct IGroupManager
{

	/** ������
	@param   DWORD dwCaptainDBID: �ų���ɫID
	@param   bool bSync		:	�Ƿ�ͬ������
	@param   
	@return   IGroup * �Žӿ�ָ��
	*/
	virtual IGroup *				CreateNewGroupByDBID(DWORD dwCaptainDBID,bool bSync=true) = NULL;

	/** ���б���ɾ����
	@param   DWORD dwGroupID	: ��ID
	@param   
	@return  �ɹ���
	*/
	virtual bool					RemoveGroup(DWORD dwGroupID) = NULL;

	/** ����һ�����鵽��
	@param   IGroup * pGroup :ָ����
	@param   int nTeamID	: ����ID
	@param   
	@return  �ɹ���
	*/
	virtual bool					AddTeam(IGroup * pGroup,int nTeamID) = NULL;

	/** ������ɾ��һ������
	@param   IGroup * pGroup :ָ����
	@param   int nTeamID	: ����ID
	@param   
	@return  �ɹ���
	*/
	virtual bool					RemoveTeam(IGroup * pGroup,int nTeamID) = NULL;

	/** �����ų���ɫID
	@param   DWORD dwGroupID		: ��ID
	@param   DWORD dwNewCaptainDBID: ���ų���ɫID,Ϊ0���Զ��Ӷӳ���ѡһ��
	@param   
	@return  �ɹ���
	*/
	virtual bool					ChangeGroupCaptain(DWORD dwGroupID,DWORD dwNewCaptainDBID) = NULL;

	/**  ��������
	@param   DWORD dwPlayerID :��ҽ�ɫID
	@param   DWORD	dwTeamID;	�����ɶӵ�TeamID
	@param   DWORD	dwOpType;	�������� 0:�����,1:����,2:���,3:���ӳ�
	@param   char * pResultMsg;	�����ִ�
	@param   int nMsgLen;	�����ִ�����
	@return  bool �ɹ���
	*/
	virtual bool					ChangePlayerTeam(DWORD dwPlayerID,DWORD dwTeamID,DWORD dwOpType,char * pResultMsg,int nMsgLen) = NULL;

	/** �������������
	@return  
	*/
	virtual void					Clear(void) = NULL;

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

/// ��ӷ���
struct ITeam
{
	/** ����ID
	@param   
	@param   
	@return  
	*/
	virtual int					GetTeamID(void) = NULL;

	/** �ӳ�PDBID
	@param   
	@param   
	@return  
	*/
	virtual DWORD				GetCaptain(void) = NULL;



	/** �Ƿ�����
	@param   
	@param   
	@return  
	*/
	virtual bool				IsMemberFull(void) = NULL;

	/** ��Ա����
	@param   
	@param   
	@return  
	*/
	virtual int					GetMemberCount(void) = NULL;

	/** �����б�
	@param   
	@param   
	@return  
	*/
	virtual TLIST_MEMBERINFO *	GetMemberList(void) = NULL;	

	/** ��Ա��Ϣ
	@param   
	@param   
	@return  
	*/
	virtual STeamMemberInfo *	GetMemberInfo(DWORD dwPDBID) = NULL;

	/** �Ƿ�ͳ��
	@return 
	*/
	virtual bool						IsTotal(void) = NULL;

	/** �趨�Ƿ�ͳ��
	@return 
	*/
	virtual bool						SetTotal(bool bFlag) = NULL;

	/** ���ͳ������
	@return 
	*/
	virtual void						ClearTotal(void) = NULL;

	/** ����ͳ������
	@param   DWORD dwPlayerID	: ���ID
	@param  BYTE nType	: ����	EMGroupTotalType
	@param  DWORD dwNumber : ����
	@return 
	*/
	virtual bool						AddTotalInfo(DWORD dwPlayerID,BYTE nType,DWORD dwNumber) = NULL;

	/** ����ͳ�������������״̬
	@param  DWORD dwPlayerID		: ���ID
	@param  BYTE nFlagType			: ָ��λ��ʶ
	@param  bool bFlag				: Ҫ���ñ�ʶ״̬,true���ǣ�false:��
	@return �ɹ���
	*/
	virtual bool						UpdateTotalOnlineState(DWORD dwPlayerID,BYTE nFlagType,bool bFlag) = NULL;

	/** ���Ͷ���Ļ�����Ϣ���ͻ���
	@param	pClientArray: Ҫ���͵Ŀͻ���ID����
	@param	wClientNum  : �ͻ�����Ŀ
	@return  
	*/
	virtual void						SendBaseInfoToClient(ClientID * pClientArray,int wClientNum) = NULL;

	/** �����ŵ�ͳ����Ϣ���ͻ���
	@param	pClientArray: Ҫ���͵Ŀͻ���ID����
	@param	wClientNum  : �ͻ�����Ŀ
	@return  
	*/
	virtual void						SendTotalInfoToClient(ClientID * pClientArray,int wClientNum) = NULL;

	/** ��Ա����
	@param   
	@param   
	@return  
	*/
	virtual bool						JoinMember(STeamMemberInfo * pMember, bool bSync) = NULL;

	/** ��Ա�뿪
	@param   
	@param   
	@return  
	*/
	virtual bool						LeaveMember(DWORD dwPDBID, int nReason, bool bSync) = NULL;


	/** ��ԭ��������
	@param   
	@param   
	@return  
	*/
	virtual bool						RestoreMember(STeamMemberInfo * pMember, int nCount) = NULL;

	/** ��ԭ������������
	@param   
	@param   
	@return  
	*/
	virtual bool						RestoreDataToZoneSvr(DWORD ServerID) = NULL;

	/** ���������ˣ���Ҫ���¶���״̬
	@param   
	@param   
	@return  
	*/
	virtual void						ZoneSvrDisconnected(DWORD ServerID) = NULL;

	/** �뿪����
	@param   
	@param   
	@return  
	*/
	virtual bool						DoLeave(DWORD dwPDBID) = NULL;

	/** �����ɢ
	@param   
	@param   
	@return  
	*/
	virtual void						DoDisband(void) = NULL;

    /** ����������Ա�����䣨��ȥ�ӳ���
	@param   
	@param   
	@return  
	*/
	virtual void						inviteMemberToRoom(DWORD dwRoomID) = NULL;
};

struct IStateHandler
{
	/** 
	@param   
	@param   
	@return  
	*/
	virtual void				OnLogin(DWORD dwPDBID, DWORD clientID, ELoginMode eMode){ };

	/** 
	@param   
	@param   
	@return  
	*/
	virtual void				OnLogout(DWORD dwPDBID, DWORD clientID, ELogoutMode eMode, bool bActiveExit){ };
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ���ݾ������
/// �ܶ�ʱ��������Ҫ���ʳ����������ݣ������ɫ���ݣ����Ҫ�õ�ʱ���첽ȥ��ȡ��
/// ������д����������ǳ��鷳�����Ա���Ҫ��һ�׻������ó�������������ͬ����������
/// ��������еĽ�ɫ���Զ�ͬ����������������ǳ��󣬲��һ�Ӱ���������Ч�ʣ����Ի�
/// �ǲ�ȡ�۲��ģʽ��ֻ����Ҫ�۲�Ľ�ɫͬ������

// ������������
struct IMirrorPerson
{
	/** ����
	@param   
	@param   
	@return  
	*/ 
	virtual void				Release(void) = NULL;

	/** ȡ��UID
	@param   
	@param   
	@return  
	*/
	virtual LONGLONG &			GetUID(void) = NULL;

	/** ȡ�ó���ID
	@param   
	@param   
	@return  
	*/
	virtual DWORD				GetZoneID(void) = NULL;

	/** ȡ�����ڵĵ�ͼλ��
	@param   
	@param   
	@return  
	*/
	virtual POINT &				GetMapLoc(void) = NULL;

	/** ȡ���������������
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR				GetName(void) = NULL;

	/** ȡ����ֵ����
	@param   
	@param   
	@return  
	*/
	virtual int					GetNumProp(DWORD dwPropID) = NULL;

	/** ȡ����Դ����Ϸ����ID���������ͨ������Ϊ��ǰ��Ϸ����ID
	@param   
	@param   
	@return  
	*/
	virtual int					GetFromGameWorldID(void) = NULL;
};

// ��������ص�
struct IMirrorPersonSink
{
	/** ��������ǰ�ص�����ǰ����Ҫ����Ӧ�ò�֪�������ĵ������Ƿ�����
	@param   
	@param   
	@return  
	*/
	virtual	void				OnPre_Update(DWORD dwPDBID) = NULL;

	/** �������ݺ�ص�
	@param   
	@param   
	@return  
	*/
	virtual void				OnPost_Update(DWORD dwPDBID) = NULL;

	/** ��½���ߣ���ʱͨ��dwPDBIDȡ��IMirrorPerson
	@param   bOffline ��true=�µ�½��false=�л�������½
	@param   
	@return  
	*/
	virtual void				OnLogin(DWORD dwPDBID, bool bOffline) = NULL;

	/** �ǳ�����
	@param   bOffline ��true=�뿪��Ϸ��false=�л�������½
	@param   
	@return  
	���bOfflineΪtrue�����ô˷������޷�ͨ��dwPDBIDȡ��IMirrorPerson
	*/
	virtual void				OnLogout(DWORD dwPDBID, bool bOffline) = NULL;
};


struct IBuildTeamFlow
{
	/** ����
	@param   
	@param   
	@return  
	*/ 
	virtual void				Release(void) = NULL;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual	int					GetJoinFlowType(void) = NULL;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual void				OnMessage(DWORD dwPDBID, DWORD dwMsgID, LPCSTR pData, int nLen) = NULL;
};


struct ITeamService
{
    // �����������������͹�������Ϣ
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len) = 0;

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void * data, size_t len) = 0;

	// �Ͽ�����
	virtual void onClientDisconnect(ClientID client,DWORD dwReason) = 0;

    /** 
	@param   
	@param   
	@return  
	*/
	virtual DWORD				getTeamIDByPlayer( PDBID PlayerID) = NULL;

    /** ����TeamID��ȡ��Ա�б�ӿ�
	@param          dwTeamID	:ս��ID
    @param return Team��Ա��	:��Ա����
	*/
	virtual DWORD               getTeamMemberList(DWORD dwTeamID, STeamMemberInfo* pReturnArray, DWORD dwArrayMaxSize) = NULL;


	/** �������������飨д�����﷽���ģ����ã�
	@param	dwPDBID		  �����ID
	@param	nTeamID		  ������ID
	@param  bIsCaptain	  ���Ƿ�Ϊ�ӳ�
	@param  nState		  ����������
	@param  pPersonContext�������Ϣ
	@return  
	*/
	virtual void				SysJoinTeam(DWORD dwUserID, DWORD dwPDBID, DWORD dwClientID, int nTeamID, bool bIsCaptain, BYTE nState, STeamSyncPersonContext* pPersonContext) = NULL;

	/** ����������,��ͨ��������ȡ����Ϣ��������
	@param	dwPDBID		  �����ID
	@param	nTeamID		  ������ID
	@param  bIsCaptain	  ���Ƿ�Ϊ�ӳ�
	@return  
	*/
	virtual void				RequestJoinTeam(DWORD dwPDBID,int nTeamID, bool bIsCaptain) = NULL;

    /** 
	@param	dwPDBID		  �����ID
	@return  
	*/
	virtual bool				isTeamCaptain(DWORD dwPDBID) = NULL;

    /** 
    @param	dwTeamID		  �����ID
	@param	dwRoomID		  ������ID
	@return  
	*/
	virtual void				inviteMemberToRoom(DWORD dwTeamID, DWORD dwRoomID) = NULL;
};