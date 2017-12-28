/*******************************************************************
** �ļ���:	E:\speed\Social\LegendCupServer\Src\LegendCupService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/15/2015	17��45
** ��  ��:	1.0
** ��  ��:	�������������
********************************************************************/

#pragma once
#include "ILegendCupService.h"
#include "IGameDatabaseDef.h"
#include "IGatewayAcceptorService.h"
#include "IDBEngineService.h"
#include "IDBEngine.h"
#include "MatchDef.h"
#include "LegendCupDef.h"
#include "EventDef.h"
#include "IMessageHandler.h"
#include "IMessageDispatch.h"
#include "IShareServer.h"
#include "LegendCupInfoMgr.h"
#include "LegendCup.h"
#include "DateHandler.h"
#include "IDateTriggerService.h"

class CLegendCupInfoMgr;

class CLegendCup;

class LegendCupService:public ILegendCupService, public IDBRetSink, public IEventExecuteSink, public ITransmitHandler,public IMessageHandler, public ISharePersonHandler, public DateHandler
{
// ��������б�
typedef multimap<DWORD, SLegendCupPlayerInfo>				TMap_LegendCupPlayerList;
// ��������б������
typedef multimap<DWORD, SLegendCupPlayerInfo>::iterator		TMap_LegendCupPlayerListIt;

private:
    enum eDataComeTrigger
    {
        DCT_DataTrigger_LegendCupUpdate,
    };

public:
    LegendCupService();

    ////////////////////////////////ITransmitHandler//////////////////////////////////////////

    // �����������������͹�������Ϣ
    virtual void     handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen);

    // ����ͻ��˷��͹�������Ϣ
    virtual void     handleClientMsg(DWORD client, SNetMsgHead head, void* pData, size_t nLen);

	/** ��Ϣ����
	@param server	Դ������ID
	@param uMsgID	��ϢID
	@param head		��Ϣͷ
	@param data		��Ϣ
	@param len		��Ϣ����
	*/
	virtual void	onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);

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

    // ָ���μ�ĳ������ս���б������׶���Ч��
    virtual void    luaAddCupRegister(LONGLONG llLegendCupID, PACKAGE_PTR pkKinList);

    // �Ƴ��������в���ս���б������׶���Ч��
    virtual void    luaRemoveCupRegister(LONGLONG llLegendCupID);

    // �˳����ڱ����Ľڵ�
    virtual void    exitStartCompetitionNode(LONGLONG llLegendCupID, int nSerchID, DWORD dwNewStartTime);

	// ��Ȩս���ʼ���ʾ
	virtual void	sendAbstentionKinMail(LONGLONG llLegendCupID, DWORD dwFailedKinID);
	////////////////////////////////IMessageHandler//////////////////////////////////////////
	/** ��Ϣ����
	@param msg ������Ϣ
	*/
	virtual void	onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

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
	virtual void    OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

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
	virtual void    OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	/////////////////////////////////////ILegendCupService/////////////////////////////////////

    // _Stub::release() call
    void    Release();
    // _Stub::on_start call
    bool    Create();

    // ��ҵ���
	virtual void    onClientDisconnect(ClientID client,DWORD dwReason);

	// ���ñ����������
	virtual void    setLegendCupMatchResult(SMsgRecordNodeWarResult sNodeWarResult);

    // ���ñ��������ڵ�״̬
    virtual void    setLegendCupNodeState(SMsgSetCompetitionNodeState sNodeState);

    // �õ����˱�����Ϣ
    virtual SCreaterLegendCupInfo getCreaterLegendCupInfo(BYTE byCupCreateType,DWORD dwCreaterID);

	// ��ȡ������˱����Ľ�������
	virtual int		getPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID);

	// ����������˱����Ľ�������
	virtual bool	setPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID, int nRecvPrizeCount);

    // �Ƿ��ڲμӱ���
    virtual bool	isKinInLegendCup(DWORD dwKinID);

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

    ////////////////////////////////DateHandler//////////////////////////////////////////
    virtual void            DateCome(unsigned long nTriggerID);

    virtual void            endCupRemoveCreaterRecInfo(LONGLONG llLegendCupID);

    // �Ƴ��������������Ϣ
    virtual void            removeServiceCupInfo(LONGLONG llLegendCupID);
public:
	// ���汭���������toDB
	void SaveLegendCupPlayerInfoToDB(DWORD dwPDBID);

    // ������Ϣ�����
    void send2Client( ClientID clientID, obuf256& obuf);

private:

    // �����������
    bool canCreateLegendCup(SMsgCreateCup* pCreateCup);

    //��ӱ���������
    bool addLegendCupToMgr(CLegendCup *pLegendCup);

    // ���󴴽�����
    void onMsgCreateLegendCup(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen);


    // ��ȡ�����������ݣ���Ա�б� �ͱ�����Ϣ��
    void readIniCupPlusData(LONGLONG llLegendCupID);

    // ���ұ���
    CLegendCup* findLegendCup(LONGLONG llLegendCupID);

    void addCupAllKinMapedLegendID(CLegendCup *pLegendCup);

    //////////////////////////////////�¼���Ӧ///////////////////////////////////////////

    // �����ݿ��ȡȫ����������
    void readAllCupInfoFromDB();

    // DB��ȡ���
    void readCupInfoDBFinished(LONGLONG llLegendCupID);

    void onEventKinMemberDelte(LPCSTR pszContext, int nLen);

    void onEventKinMemberQuit(LPCSTR pszContext, int nLen);

    void onEventKinMemberLogin(LPCSTR pszContext, int nLen);

    //////////////////////////////////���ݿⷵ��///////////////////////////////////////////
    // ���ر���������Ϣ
    void onReturnReadAllCupDataBaseInfo(char* pOutData, int nOutLen);

    // ���ر���ս����Ϣ
    void onReturnCupKinInfo(char* pOutData, int nOutLen);

    // ���ر��� �������б� �����б� �����б�...
    void onReturnCupEXListDataInfo(char* pOutData, int nOutLen);

    // ���ر���������Ա��Ϣ
    void onReturnCupMemberList(char* pOutData, int nOutLen);

    // ���ر���������Ϣ
    void onReturnCupCompetitionNode(char* pOutData, int nOutLen);

    // ����ÿ��λ����Ϣ
    void onReturnCupRoundKinPos(char* pOutData, int nOutLen);

    // ����ÿ��ʱ����Ϣ
    void onReturnCupRoundTimeInfo(char* pOutData, int nOutLen);

	// ������ұ���������Ϣ
	void onReturnLegendCupPlayerInfo(char* pOutData, int nOutLen);

    /////////////////////////////////////��������Ϣ/////////////////////////////////////
    // �����������
    void onServerMsgCreateCupCheck(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // �������������������
    void onServerMsgCreateCupResult(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen);
    
    // ��������������뱭������
    void onServerMsgReqJoinLegendcupRoom(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen);

	// �������������������
	void onServerMsgCupRecordWarResult(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // ����������뱭���ɹ�
    void onServerMsgReqJoinCupSuccess(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen);

    /////////////////////////////////////�ͻ�����Ϣ/////////////////////////////////////
    // ��Ӧ�ͻ�������ʼ����
    void onClientPublicCompetition(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // ��Ӧ�ͻ����������б�
    void onClientRequestCupList(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // ��Ӧ�ͻ���������뱭��
    void onClientMsgReqJoinCup(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // ��Ӧ�ͻ��������˳�����
    void onClientMsgExitCup(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // ��ս����ӵ�������
    void onClientMsgMoveToBlackList(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // �Ƴ�������
    void onClientMsgRemoveToBlackList(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // ��Ӧ�鿴����
    void onClientMsgLoockOverLegendCup(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);
    
    // ����������б�
    void onClientMsgRequestAllBlackKinlist(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // ����ս�ӳ�Ա�б�
    void onClientMsgRequestKinMemberList(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // �ö��������
    void  onClientMsgRequestJudgeWarResult(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // �鿴�����ڵ�ս����Ϣ
    void onClientMsgRequestLookUpCupNodeWarInfo(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // �رղ鿴����
    void onClientCloseCupView(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // �������½��������׶�
    void onClientOfflineEndRegisterState(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

private:
    map<LONGLONG, bool> m_ReadDBRecord;                 // ���ݿ��ȡ��ɱ�ʾ��¼

    map<LONGLONG, BYTE> m_LegendCupTypeList;            // LegendID ��Ӧ�� ELegendCupType ����

    map<BYTE, CLegendCupInfoMgr*> m_AllLegendCupMgr;    // ELegendCupType ��Ӧ�ı���������

	TMap_LegendCupPlayerList m_LegendCupPlayerList;		// ��ұ��������б�
 
    bool bReadDBCupInfoFinished;                        // ��ȡ��ɱ�ʾ
};