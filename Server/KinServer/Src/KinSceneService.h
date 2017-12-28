/*******************************************************************
** �ļ���:	E:\speed\Server\KinServer\Src\KinSceneService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	ս�ӷ���
********************************************************************/

#pragma once
#include "IKinSceneService.h"
#include "NetMessageDef.h"
#include "Kin.h"
#include "KinMember.h"
#include "IDBEngine.h"
#include "IMessageHandler.h"
#include "Kin_ManagedDef.h"
#include "Simple_Atom_Lock.h"
using namespace Kin_ManagerDef;

class KinScenService:public IKinSceneService, public IDBRetSink,  public IEventExecuteSink, public ITransmitHandler, public IMessageHandler
{
public:
	////////////////////////////////IKinSceneService//////////////////////////////////////////

    // �����������������͹�������Ϣ
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len);

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void * data, size_t len);

	// purpose:����������Ϣ
	virtual void sendToSocial(BYTE nMsgID, void * data, size_t len);
	/////////////////////////////////KinSceneServer/////////////////////////////////////////
	// purpose:  _Stub::release() call
	void release();

	// purpose:  _Stub::on_start call
	bool create();

    //  purpose: ȡ��ְλ������
	void getIdentityName(int nIdentity, char* szName, int nLen);

	/// purpose: ���ݳ�Ա��PDBID����ս�ӳ�Ա
	CKinMember* findKinMemberEx(DWORD dwPDBID);

	/// purpose: ɾ��һ����Ա
	void removeKinMember(DWORD dwPDBID);

	/// purpose:����DB�洢���̴���ս��
	virtual void DBProcCreateKin(DWORD pdbid, const char* szKinName, const char* szKinLabel, bool bCrtAsFormal = false);


	///// purpose: ���ݳ�Ա��PDBID����ս�ӳ�Ա
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
	* @param dwKinID		 :ս��ID
	*/
	virtual int getWeekActivity(DWORD dwKinID);

    /////////////////////////////ITransmitHandler/////////////////////////////////////////////
    /// purpose: ��Ϣ����
    virtual void	onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);

    ////////////////////////////IMessageHandler//////////////////////////////////////////////
    /// purpose: ��Ϣ����
    virtual void	onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

    /**
	@name         : ֪ͨ�������б��Ѿ�����
	@brief        : �������б����ֻ�����з����������˳�ʱ����
	@note         : ������б����ICenterServerConnector::GetServerList���
	*/
	virtual void	onServerListUpdated();

	/** ֪ͨ��������Ϣ����
	@param nState		: SERVER_STATE
	@param pServerData	: ServerData
	@brief        : ��������������仯�ȵ�
	*/
	virtual void	onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData);

public:

	//////////////////////////////////IDBRetSink///////////////////////
	/// purpose: ���ݿ����󷵻ػص�����
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	////////////////// IEventExecuteSink ////////////////////////////////////////////////////////
	/// purpose: ִ���¼��ӿ�ʵ��
    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

private:

	/// purpose: ����ս��					-- ��������
	void onTransmitCreateKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

	/// purpose: ս�Ӹ���					-- ��������
	void onTransmitKinRename(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

	/// purpose: ˢ��ս�����ƺ�				-- ��������
	void onTransmitNameTitle(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);
	
	/// purpose: ������ҵ�ս��ID			-- ��������
	void onTransmitUpdateKinID(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

	/// purpose: ת����Ϣ					-- ��������
	void onTransmitPostMsg(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

    /// purpose: ɾ����Ա				-- ��������
    void onTransmitRemoveMember(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

    /// purpose: ���³�Ա��û������ӣ�	-- ��������
    void onTransmitUpdateSceneMember(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

    /// purpose: ɾ��ս��				-- ��������
    void onTransmitRemoveKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

    /// purpose: ����ս�ӣ�û������ӣ�	-- ��������
    void onTransmitUpdateKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

    /// purpose: ����ս������			-- ��������
    void onTransmitUpdateKinProp(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

    /// purpose: ���ս��				-- ��������
    void onTransmitClearKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

    /// purpose: ���ս��				-- ��������
    void OnTransmitClearKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

	/// purpose: ���ķ����ս������			-- �������ݿ�
	void onReturnCenterKinNameCheck(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: ���ķ�����ս��				-- �������ݿ�
	void onReturnCenterCreateKin(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: ����ս��					-- �������ݿ�
	void onReturnCreateKin(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: ����ս�Ӳ���
	//@ note   : ��֪ͨ���ݿⴴ��ս�ӳɹ����أ����������ִ���ս�ӵ������Ѿ�����������Ҫȡ����ǰ�Ĵ�������
	void backUpCreateKin(DWORD dwKinID, LPCSTR szTrackNote);

    virtual CKin* findKin(DWORD dwKinID);

    ///// purpose: ����ս��ID����ս��
    virtual IKin* getKin(DWORD dwKinID);

    // ��Ϸ�޸������Ϣ����
    //bool AddService_ResponseUpdateUserInfoFromGameDB(const DBREQ_RESULT_ADDSERVICES_USER_UPDATE & param);

    /// purpose: ����ս��ID
    void updateKinIDInfo(DWORD dwActorID, DWORD dwKinID);

private:
	map<DWORD, CKin*>			m_KinList;					// ս���б�
	map<DWORD, CKinMember*>		m_KinMemberList;			// ս�ӳ�Ա�б�

    volatile	LONG    m_mutex;

};