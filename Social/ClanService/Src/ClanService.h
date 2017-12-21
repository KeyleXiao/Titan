/*******************************************************************
** �ļ���:	ClanService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	3/31/2016	17:19
** ��  ��:	1.0
** ��  ��:	������
********************************************************************/

#ifndef __CLANSERVICE_H__
#define __CLANSERVICE_H__

#include "IGameDatabaseDef.h"
#include "IGatewayAcceptorService.h"
#include "IDBEngineService.h"
#include "IDBEngine.h"
#include "IMessageHandler.h"
#include "ICenterConnectorService.h"
#include "IShareServer.h"
#include "IClanService.h"
#include "Clan.h"
#include "KinDef.h"
#include "DateHandler.h"
#include "DateHandler.h"
#include "IDateTriggerService.h"

// ��½��һ������ݽṹ,����ṹ��������ҵ�½������һ����ң������������ȫ�������ڼ䣬��½������б�
struct SClanUserLogin
{
    DWORD dwClanID;			// ���ID
    list<DWORD> userList;	// ���PDBID�б�
    bool  bIsRequest;		// �ύ״̬��δ�ύ���ύ��

    SClanUserLogin()
    {
        dwClanID = 0;
        userList.clear();
        bIsRequest = false;
    }
};

class CClanService : public IClanService,
    public ITransmitHandler,
    public IMessageHandler,
    public IDBRetSink,
    public IEventExecuteSink,
    public TimerHandler,
    public ISharePersonHandler,
	public DateHandler
{
private:
	enum EMTimerID
	{
		TimerID_Clan_Clear_User_Login_List,       // �����һ����б�ʱ��
	};

	enum eDataComeTrigger
	{
		DCT_DataTrigger_ClanUpdate,
		DCT_DataTrigger_WeekActivityReset,
        DCT_DataTrigger_AutoSetShaikhState,
        DCT_DataTrigger_LegendCupDidaUpdate
	};

public:
	CClanService();
    ~CClanService();

    // _Stub::on_start call
    bool create();
public:
    /////////////////////////////////////IClanService/////////////////////////////////////
    // _Stub::release() call

    // �����������������͹�������Ϣ
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

    virtual void release();

	// ����ս���������ID
	virtual DWORD findKinClan(DWORD dwKinID);
    // �Ƴ����ڶ�ȡ��ս��
	virtual void removeReadingKin(DWORD dwKinID);
	// ��ȡ����Ա�б�
	virtual int getClanKinList(DWORD dwClanID, DWORD *pKinList, int nLen);
    // ȡ�ð����ֵ
    virtual int getClanNumProp(DWORD dwClanID, int nNumIndex);
	// ��ȡ�������
	virtual string getClanName(DWORD dwClanID);
	// ���˱�������
	virtual void notifyCreateClanLegendCup(DWORD dwClanID, LONGLONG llLegendCupID, int nMaxKinCount, LPCSTR pCupName);

	////////////////////////////////ITransmitHandler//////////////////////////////////////////
	// ��Ϣ����
	virtual void	        onTransmit(DWORD server, ulong uMsgID, SNetMsgHead *head, void *data, size_t len);
	// ֪ͨ�������б��Ѿ�����
	virtual void	        onServerListUpdated();
	// ֪ͨ��������Ϣ����
	virtual void	        onServerInfoUpdated(DWORD ServerID, BYTE nState, void *pServerData);

	////////////////////////////////IMessageHandler//////////////////////////////////////////
	// ��Ϣ����
	virtual void	        onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead *head, void *data, size_t len);

    /////////////////////////////////////IDBRetSink/////////////////////////////////////
    // ���ݿ����󷵻ػص�����
	virtual void			OnReturn(IDBRetSink *pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char *pOutData, int nOutLen);

    ////////////////////////////////IEventExecuteSink//////////////////////////////////////////
	virtual void            OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ////////////////////////////////TimerHandler//////////////////////////////////////////
    virtual void            OnTimer( unsigned long dwTimerID );

    ////////////////////////////////////ISharePersonHandler////////////////////////////////////
    // ���߻��߿�����л���ͼ��Ҳ��������õ�֪ͨ
	virtual void			OnLogin(ISharePerson *pSharePerson, ELoginMode nLineType);
	// ���߻��߿�����л���ͼ��Ҳ��������õ�֪ͨ���������Ժ󣬾��޷���ȡ��
	virtual void			OnLogout(ISharePerson *pSharePerson, ELogoutMode nLineType);
	// ��������ǰ���˴���һǰһ������Ӧ�ò�֪��ʲô���ݸı��ˣ���nUpdateFlag��ʾ����Ϊʲôԭ��ı���
	virtual void			OnPre_Update(ISharePerson *pSharePerson, SHAREUPDATE_REASON nUpdateReason);
	// �������ݺ󣬴˴���һǰһ������Ӧ�ò�֪��ʲô���ݸı��ˣ���nUpdateFlag��ʾ����Ϊʲôԭ��ı���
	virtual void			OnPost_Update(ISharePerson *pSharePerson, SHAREUPDATE_REASON nUpdateReason);

	////////////////////////////////DateHandler//////////////////////////////////////////
	virtual void            DateCome(unsigned long nTriggerID);

	////////////////////////////////CClanService//////////////////////////////////////////

    // ���Ұ��
    CClan* findClan(DWORD dwClanID);
    // ���Ұ��
    CClan* findClanByName(string szClanName);
    // ��ð��ս��
    DWORD findKinClanByPDBID(DWORD dwPDBID);

    // �Ƴ����
    void removeClan(DWORD dwClanID);

    // ��Ӱ���Ա��������£�
    void addClanMember(DWORD dwClanID, DWORD dwKinID, bool bNeedSaveDB);
    // ɾ������Ա
    void removeClanMember(DWORD dwKinID, bool bNeedSaveDB);

    // ���³�Ա���ݵ����ݿ�
	void updateMemberToDB(DWORD dwClanID, DWORD dwKinID);
	// ����ȫ���������ݵ�������
	bool refreshClanInfoToZone(CSID dwServerID = INVALID_SERVER_ID);
    // ���³�Ա���ݵ�������
    void updateMemberToZone(DWORD dwClanID, DWORD dwKinID, CSID ServerID = INVALID_SERVER_ID);
    // ���³�Ա���ݵ��ͻ���
    void updateMemberToClient_OZC(DWORD dwClanID, SKinMember* pKinMember);
    // ���³�Ա���ݵ��ͻ���
    void updateMemberToClient_OC(DWORD dwClanID, SKinMember* pKinMember);

    // ��Ա���ߣ������ͳ���������Ҳ��֪ͨ��
    void onMemberLogin(DWORD dwClanID, DWORD dwMemberPDBID);
    // ���Ұ��ĵ�½�����Ա
    SClanUserLogin *findLoginMember(DWORD dwClanID);
    // ������ڶ�ȡ��ս��
    bool addReadingKin(DWORD dwClanID, DWORD dwKinID);
    // �Ƿ�Ϊ��
    bool isReadingKinEmpty(DWORD dwClanID);
    // ����ȫ�����ݶ�ȡ���
    void onReadDBFinish(DWORD dwClanID);

	// �����ܻ�Ծ�ȼ���Ƿ��Զ���ɢ���
	void autoDismissLowerActivity();

    // ����Ƿ���Ҫ��ɢ�İ��
    bool checkDBDismiss(DBREQ_TABLE_CLAN* pClanContext);
    // �ύ��ɢ�������
    void onDismissClan(DWORD dwClanID);

    /// ִ�д洢����
    bool onExecuteSP(DWORD dwCmdID, DWORD dwQueueIndex, LPCSTR data, int nLen);

private:
    // ��ȡ����б�         					-- �������ݿ�
    void onReturnReadClanList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
    // ��ȡ����Ա�б�       				-- �������ݿ�
    void onReturnReadMemberList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
    // ����/��ӳ�Ա��Ϣ       				-- �������ݿ�
    void onReturnUpdateMember(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
    // ɾ����Ա��Ϣ			  				-- �������ݿ�
    void onReturnRemoveMember(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
    // ���°���������			  			-- �������ݿ�
    void onReturnUpdateClan(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
    // ��ɢ���			  					-- �������ݿ�
    void onReturnDismissClan(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

    // ��������ģ����Ϣ��
    void onModuleHandleMessage(DWORD dwServerID, DWORD dwMsgID, LPCSTR pszData, size_t nLen);
    // ���������Ϣ����					-- ���Գ�����
    void onSceneCreateClan(DWORD dwServerID, LPCSTR pszData, size_t nLen);
    
    // ���ͻ���ģ����Ϣ��
    void onModuleClientMsg(DWORD dwClientID, DWORD dwMsgID, LPCSTR pszData, size_t nLen);
    // ���������Ϣ����						-- ���Կͻ���
    void onClientCreateClan(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);
    // �������б�							-- ���Կͻ���
    void onClientClanList(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);
	// �����������							-- ���Կͻ���
	void onClientClanInvite(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);
	// �����							-- ���Կͻ���
	void onClientAnswerInvite(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);

    // ��ȡ����б���Ϣ
    bool readClanList();
    // ��ȡ��ḽ�����ݣ���Ա�б�
	void readClanPlusData(DWORD dwClanID);

	// ֪ͨ���������Ľ�ɫ���ϵ�����ID
	void setActorClanID(DWORD dwPDBID, DWORD dwClanID, DWORD dwServerID);

	// �޸��������˱�������
	void UpdateAllClanLegendCupProp();

    // ���±���dida��ʾ
    void updateClanLegendDida();

	// �����ܻ�Ծ��
	void ResetWeekActivity();

    // �Զ���������״̬,��̨�Զ���������
    void AutoSetShaikhState();

private:
    // ����б�
    typedef stdext::hash_map<DWORD, CClan*>                   TMap_Clan;
    // ����б������
    typedef stdext::hash_map<DWORD, CClan*>::iterator         TMap_ClanIt;
    TMap_Clan       m_ClanList;								// ����б�
    list<CClan*>    m_ClanArray;							// ����б�(��ʾ��)

    // ����б�����������
    typedef stdext::hash_map<std::string, CClan*>             TMap_NameClan;
    // ����б�����������������
    typedef stdext::hash_map<std::string, CClan*>::iterator   TMap_NameClanIt;
    TMap_NameClan	m_NameClanList;							// ����б�����������

    // ����Ա�б�
    typedef stdext::hash_map<DWORD, DWORD>				   TMap_KinClan;
    // ����Ա�б������
    typedef stdext::hash_map<DWORD, DWORD>::iterator       TMap_KinClanIt;
    TMap_KinClan	m_KinClanList;							// ����Ա�б�

private:
    // ��½��һ����б�
    typedef map<DWORD, SClanUserLogin>                        TMap_ClanUserLogin;
    // ��½��һ����б������
    typedef map<DWORD, SClanUserLogin>::iterator              TMap_ClanUserLoginIt;
    // ������ҵ�½������һ����ң������������ȫ�������ڼ䣬��½������б�
    TMap_ClanUserLogin m_ClanUserLoginList;
    // ս�����ݶ�ȡ�����б�
    map<DWORD, list<DWORD>> m_ReadingKinList;

};

#endif // __CLANSERVICE_H__