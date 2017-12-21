/*******************************************************************
** �ļ���:	E:\speed\Social\KinServer\Src\KinService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/15/2015	17��45
** ��  ��:	1.0
** ��  ��:	ս�ӷ���
********************************************************************/

#pragma once

class CKin;
#include "Kin.h"
#include "IKinService.h"
#include "IGameDatabaseDef.h"
#include "IGatewayAcceptorService.h"
#include "IDBEngineService.h"
#include "IDBEngine.h"
#include "MatchDef.h"
#include "KinDef.h"
#include "KinEvent.h"
#include "Event_ManagedDef.h"
#include "KinMember.h"
#include "ICenterConnectorService.h"
#include "IShareServer.h"
#include "IMessageHandler.h"
#include "DateHandler.h"
#include "IDateTriggerService.h"

// ����������������
// �������DBID list
typedef std::vector<const SKinMember *>  TVEC_KinMember;

// ��½��һ������ݽṹ,����ṹ��������ҵ�½��ս�ӵ�һ����ң���ս��������ȫ�������ڼ䣬��½������б�
struct SKinUserLogin
{
    DWORD dwKinID;			// ս��ID
    list<DWORD> userList;	// ���PDBID�б�
    bool  bIsRequest;		// �ύ״̬��δ�ύ���ύ��

    SKinUserLogin()
    {
        dwKinID = 0;
        userList.clear();
        bIsRequest = false;
    }
};


class KinService: public IKinService,
	public IEventExecuteSink,
	public IDBRetSink,
	public ISharePersonHandler,
	public ITransmitHandler,
	public IMessageHandler,
	public DateHandler
{
private:
	enum eDataComeTrigger
	{
		DCT_DataTrigger_WeekActivityReset,
        DCT_DataTrigger_AutoSetShaikhState,
	};

// ��½��һ����б�
typedef map<DWORD, SKinUserLogin>                        TMap_KinUserLogin;
// ��½��һ����б������
typedef map<DWORD, SKinUserLogin>::iterator              TMap_KinUserLoginIt;

// ս���б�
typedef stdext::hash_map<DWORD, CKin*>                   TMap_Kin;
// ս���б������
typedef stdext::hash_map<DWORD, CKin*>::iterator         TMap_KinIt;

// ս���б�����������
typedef stdext::hash_map<std::string, CKin*>             TMap_NameKin;
// ս���б�����������������
typedef stdext::hash_map<std::string, CKin*>::iterator   TMap_NameKinIt;

// ս�ӳ�Ա�б�
typedef stdext::hash_map<DWORD, CKinMember>              TMap_KinMember;
// ս�ӳ�Ա�б������
typedef stdext::hash_map<DWORD, CKinMember>::iterator    TMap_KinMemberIt;

typedef map<DWORD, CKin*> TMap_FormalKin;

typedef TMap_FormalKin TMap_InFormalKin;



public:
    /// purpose: ���캯��
    KinService();

    /// purpose: �鹹����
    virtual ~KinService();

public:
    /////////////////////////////////////IDBRetSink/////////////////////////////////////
    /** ���ݿ����󷵻ػص�����
	@param   pDBRetSink		��������ؽӿ�	
	@param   nCmdID ������ɣ�
	@param   nDBRetCode�����ݿ����󷵻�ֵ���ο����涨��
	@param   pszDBRetDesc�����ݿ����󷵻��������ɣӣз���
	@param   nQueueIndex�����ж���
	@param   pOutData���������
	@param   nOutLen��������ݳ���
	@return  
	@note     
	@warning �˶���ǧ���������ݿⷵ��ǰ�ͷţ������Ƿ���
	@retval buffer 
	*/	
	virtual void			OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

    ////////////////////////////////IEventExecuteSink//////////////////////////////////////////
	/** 
	@param   wEventID ���¼�ID
	@param   bSrcType ������Դ����
	@param   dwSrcID : ����Դ��ʶ��ʵ��ΪUID��"���к�"���ݣ���ʵ���Ϊ0��
	@param   pszContext : ������
	@param   nLen : �����ĳ���
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void            OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ////////////////////////////////ISharePersonHandler//////////////////////////////////////////
    /** ���߻��߿�����л���ͼ��Ҳ��������õ�֪ͨ
	@param   SHARELINE_TYPE �����߻ص�����
	@param   
	@return  
	*/
	virtual void			OnLogin(ISharePerson * pSharePerson, ELoginMode nLineType);

	/** ���߻��߿�����л���ͼ��Ҳ��������õ�֪ͨ���������Ժ󣬾��޷���ȡ��
	@param   SHARELINE_TYPE �����߻ص�����
	@param   
	@return  
	*/
	virtual void			OnLogout(ISharePerson * pSharePerson, ELogoutMode nLineType);

	/** ��������ǰ���˴���һǰһ������Ӧ�ò�֪��ʲô���ݸı��ˣ���nUpdateFlag��ʾ����Ϊʲôԭ��ı���
	@param   pSharePerson ������ǰ�Ķ���
	@param   nUpdateFlag  ���ı�ԭ��
	@return  
	*/
	virtual void			OnPre_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason);

	/** �������ݺ󣬴˴���һǰһ������Ӧ�ò�֪��ʲô���ݸı��ˣ���nUpdateFlag��ʾ����Ϊʲôԭ��ı���
	@param   pSharePerson �����º�Ķ���
	@param   nUpdateFlag  ���ı�ԭ��
	@return  
	*/
	virtual void			OnPost_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason);

    ////////////////////////////////IMessageHandler//////////////////////////////////////////
	/** ��Ϣ����
	@param msg ������Ϣ
	*/
	virtual void	        onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

	////////////////////////////////ITransmitHandler//////////////////////////////////////////
	/** ��Ϣ����
	@param server	Դ������ID
	@param uMsgID	��ϢID
	@param head		��Ϣͷ
	@param data		��Ϣ
	@param len		��Ϣ����
	*/
	virtual void	        onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);

    /**
	@name         : ֪ͨ�������б��Ѿ�����
	@brief        : �������б����ֻ�����з����������˳�ʱ����
	@note         : ������б����ICenterServerConnector::GetServerList���
	*/
	virtual void	        onServerListUpdated();

	/** ֪ͨ��������Ϣ����
	@param nState		: SERVER_STATE
	@param pServerData	: ServerData
	@brief        : ��������������仯�ȵ�
	*/
	virtual void	        onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData);
	
	// ���ڴ�����
	virtual void            DateCome(unsigned long nTriggerID);
	
    //////////////////////////////////IKinService////////////////////////////////////////

    // �����������������͹�������Ϣ
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // ����ͻ��˷��͹�������Ϣ
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

    /// purpose: ����ս�ӹ��׶�
    virtual bool    addClanCtrb(DWORD dwPDBID, int nClanCtrb);

    /** ����ս��ID��ȡKin�ӿ�
	@param dwKinID		:ս��ID
	*/
	virtual IKin*	getKin(DWORD dwKinID);

    /** ����ս��ID��ȡKinInfo
	@param dwKinID		:ս��ID
	*/
    virtual SKinInfo getKinInfo(DWORD dwKinID);

    /** ����ս��ID��ȡ��Ա���ݽӿ�
	@param dwKinID		:ս��ID
	*/
	virtual SKinMember getKinMemberInfo(DWORD dwPBID);

    /** ����ս��ID��ȡ��Ա�ӿ�
	@param dwKinID		:ս��ID
	*/
	virtual SKinMember*	getKinMember(DWORD dwPBID);

    /** ����ս��ID��ȡ��Ա�б�ӿ�
	@param dwKinID		:ս��ID
	*/
	virtual DWORD getKinMemberList(DWORD dwKinID, PDBID* pReturnArray,DWORD dwArrayMaxSize);

    /** ��ȡս�ӳ�Ա����
	@param dwKinID		:ս��ID 
	*/
    virtual DWORD getKinMemberCount(DWORD dwKinID);

    /** ��ȡս����ս��
	@param dwKinID		:ս��ID 
	*/
    virtual DWORD getTotalFightScore(DWORD dwKinID);

    /** �ⲿ�ӿ�ɾ��ս�ӳ�Ա
	@param dwKinID		:ս��ID 
	*/
    virtual bool deleteKinMember(DWORD dwKinID, DWORD ActorID);

	/// purpose: _Stub::release() call
	void release();
	/// purpose: _Stub::on_start call
	bool create();

	/// purpose: ������Ϣ�����
	void send2Client( ClientID clientID, obuf256& obuf);

    /// purpose: ���ս�ӳ�Ա��������£�
    virtual void addKinMember(SKinMember* pMember, bool bNeedSaveDB);

    /// purpose: ���³�Ա���ݵ��ͻ���
    virtual void updateMemberToClient(SKinMember* pMember);

    /// purpose: ���³�Ա���ݵ����ݿ�
    virtual void updateMemberToScene(SKinMember* pMember, CSID ServerID = INVALID_SERVER_ID);

    virtual CKin* findKin(DWORD dwKinID);

    /// purpose: ����ս�ӳ�Ա
    virtual SKinMember* findKinMember(DWORD dwPDBID);

    /// purpose: ���³�Ա���ݵ����ݿ�
    virtual void updateMemberToDB(SKinMember* pMember);

    /// purpose: ɾ��ս�ӳ�Ա
    virtual void removeKinMember(DWORD dwMemberPDBID, bool bNeedSaveDB);

    /// purpose: ȡ������뿪ս�ӵ�ʱ��
    int getQuitKinTime(DWORD dwPDBID);

    /// purpose: �����뿪ս��ʱ��
    void setQuitKinTime(DWORD dwPDBID, int nTime);

    /// purpose: �ύ��ɢս������
    void DBDismissKin(DWORD dwKinID);

    /// purpose: ����ս��
    void removeKin(DWORD dwKinID);

    // purpose: ִ�д洢����
    void ExecSP(DWORD cmdID,LPCSTR pData, int len);

	// purpose: ����ս�ӱ�������
	virtual void addKinLegendAward(SKinCupAwardNode sAwardNode);

	// purpose: ����ս�ӱ���ս��
	virtual void addKinLegendWarResult(SKinWarRecordNode sRecordNode);

    // purpose: ȡ��ĳ����ݵ�����
    virtual LPCSTR getIdentityName(int nIdentity);

	// purpose:  ս�ӱ�����������
	virtual void addKinLegendGlory(int nKinID, int nNum, int nGloryType);

	// purpose: ȡս�ӱ�������
	virtual int getKinLegendGlory(int nKinID, int nGloryType);

	/// purpose: ��ȡ�ܻ�Ծ��
	virtual int getWeekActivity(DWORD dwKinID);

	// purpose: ����ս���б�ڵ�����״̬			
	void updateKinList(int nKinID, int nClientID);					

private:


    // ��ȡ��ʼ������
    void readInitalKinData();

    /// purpose: ��ȡս�Ӹ������ݣ���Ա�б�������Ϣ���ƺ����ݣ�
    void readIniKinPlusData(DWORD dwKinID, bool bAddReadFlag);

    // ����ս���б����
    void onReadKinListFinish();

    // ��ȡս���б���Ϣ
    bool readKinList();

    /// purpose: ս�ӵ�ȫ�����ݶ�ȡ���
    void onReadDBFinish(DWORD dwKinID);

    /// purpose: ����ģ����Ϣ
	void onModuleMessage(ulong nEndpointId, DWORD dwMsgID, SNetMsgHead* pHead, LPCSTR pszData, size_t nLen);

    /// purpose: ս�Ӻϲ���Ϣ
	void OnMergeMessage(ulong nEndpointId, DWORD dwMsgID, SNetMsgHead* pHead, LPCSTR pszData, size_t nLen);

	/// purpose: ����ս����Ϣ����						-- ���Կͻ���
	void onClientCreateKin(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: ���������Ϣ����						-- ���Կͻ���
	void onClientInvite(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: �������Ϣ����						-- ���Կͻ���
	void onClientAnswerInvite(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: ����ս���б�						-- ���Կͻ���
    void onClientKinList(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: ս�Ӹ�����Ϣ����						-- ���Կͻ���
	void onClientKinRename(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: ս�Ӻϲ�������Ϣ����					-- ���Կͻ���
	void onClientRequestMerge(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: ս�Ӻϲ���Ӧ��Ϣ����					-- ���Կͻ���
	void onClientMergeResponse(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: ��Ӧ�˳��۲�ս���б�					-- ���Կͻ���
    void onClientRemoveKinListOb(ClientID clientID, LPCSTR pszData, size_t nLen);
    
	/// purpose: �޸�ս��������Ϣ����					-- ���Գ�����
	void onSceneKinRename(DWORD dwSceneID, LPCSTR pszData, size_t nLen);

    /// purpose: �޸��Ա�					            -- ���Գ�����
    void onSceneActorChangeSex(DWORD dwSceneID, LPCSTR pszData, size_t nLen);

    /// purpose: �޸�ս��������Ϣ����					-- ���Գ�����
    void onSceneActorRename(DWORD dwSceneID, LPCSTR pszData, size_t nLen);

    /// purpose: ����ս����������                       -- ���Գ�����
    void onSceneKinActorPropertyUpdate(DWORD dwSceneID, LPCSTR pszData, size_t nLen);
    
	/// purpose: ����ս����Ϣ����						-- ���Գ�����
	void onSceneCreateKin(DWORD dwSceneID, LPCSTR pszData, size_t nLen);

	/// purpose: ��ȡս���б�         					-- �������ݿ�
	void onReturnReadKinList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: ��ȡս�ӻ�������     					-- �������ݿ�
	void onReturnReadKinBase(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: ��ȡս�ӳ�Ա�б�       				-- �������ݿ�
	void onReturnReadMemberList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: ��ȡս�ӱ��������б�       			-- �������ݿ�
	void onReturnReadLegendAwardList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: ��ȡս�ӱ���ս���б�       			-- �������ݿ�
	void onReturnReadLegendWarList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: ����/��ӳ�Ա��Ϣ       				-- �������ݿ�
	void onReturnUpdateMember(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: ɾ����Ա��Ϣ			  				-- �������ݿ�
	void onReturnRemoveMember(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: ����ս�ӻ�������			  			-- �������ݿ�
	void onReturnUpdateKin(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: ��ɢս��			  					-- �������ݿ�
	void onReturnDismissKin(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: ��ӱ���������Ϣ       				-- �������ݿ�
	void onReturnAddLegendAward(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: ��ӱ���ս����Ϣ       				-- �������ݿ�
	void onReturnAddLegendWar(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

    /// purpose: ��Ա���ߣ������ͳ���������Ҳ��֪ͨ��
    void onMemberLogin(DWORD dwKinID, DWORD dwMemberPDBID);

    /// purpose: ��Ա����
    void onMemberLogout(DWORD dwKinID, SKinMember* pMember);

    /// purpose: ��������ˢ��ս�ӵ���Ϣ
    bool refreshKinInfoToScene(CSID dwServerID = INVALID_SERVER_ID);

    // ���¸���Rank��Ϣ
    void UpdatePersonRank(void* pEventData);

    
    // ���¸���Rank��Ϣ
    void UpdatePersonUpdateClanID(void* pEventData);
  

    // ���¸���״̬��ս�ӹ۲���
    void UpdatePersonInfoToKinOb(SKinMember* pMember);

    /// purpose: ���ս���б�۲���
    bool addObserve(DWORD dwPDBID);

    /// purpose: ɾ��ս���б�۲���
    void removeObserve(DWORD dwPDBID, bool bCoerceRemove);

    /// purpose: ����ս���б�۲���
    SKinListObserve* findObserve(DWORD dwPDBID);

    // ���ȫ��DB���ݶ���
    void checkAllDBDataRead();

	// �����ܻ�Ծ��
	void ResetWeekActivity();

	// �Զ���ɢ�ͻ�Ծ�ȵ�ս��
	void AutoDismissLowerActivity();

    // �Զ����öӳ�״̬,��̨�Զ������ӳ�
    void AutoSetShaikhState();

private:
    // ����뿪ս��ʱ��
    map<DWORD, int> m_UserQuitTimeTable;

    // ������ҵ�½��ս�ӵ�һ����ң���ս��������ȫ�������ڼ䣬��½������б�
    TMap_KinUserLogin m_KinUserLoginList;

    // �Ƿ����һҳ����ս���б�
    bool m_bFinalList;

    // �������ݶ���
    bool m_bDBKinAllDataFinal;

    // ������Ҫɾ������ʱս���б�
    list<DWORD> m_DeleteKinList;

    map<DWORD, SKinListObserve> m_mapObserveList;               // ս���б�۲��ߣ����ڴ���ɾ��ս�ӹ㲥���۲���Ա��

    TMap_Kin			m_KinList;								// ս���б�
    TMap_InFormalKin	m_FormalKinArray;	                    // ��ʽս���б�ÿ�����Ҷ�Ӧһ��list�б�,��ΪID��1��ʼ�ģ�0��λ�ò���
    TMap_NameKin		m_NameKinList;							// ս���б�����������
    TMap_KinMember		m_KinMemberList;						// ս�ӳ�Ա�б�

    volatile	LONG    m_mutex;
};