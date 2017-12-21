/*******************************************************************
** �ļ���:	KinClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	������
** ��  ��:	2015/10/19  17:21
** ��  ��:	1.0
** ��  ��:	ս��ϵͳ�ͻ���
** Ӧ  ��:  	
	
*******************************************************************/

#pragma once
#include "IKinClient.h"
#include "IEntityClient.h"
#include "MatchDef.h"
#include "Kin.h"

class KinClient:public IKinClient, public IGameViewEventHandler, public IMessageHandler
{
public:

    /// purpose: ���캯��
    KinClient();

    /// purpose: �鹹����
    virtual ~KinClient();

	////////////////////////////IKinClient//////////////////////////////////////////////
	virtual void		release(void);

     /** ����ս��
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallCreateKin(const char* szName);

    /** �������ս��
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallJoinKin(DWORD dwKinID, string strLeaveWord);

     /** �˳�ս��
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallQuitKin();

    /** ��ɢս��
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallDissmissKin();

    ////////////////////////////IGameViewEventHandler////////////////////////////////////////////////////
    virtual bool		onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );

    /////////////////////////////IMessageHandler/////////////////////////////////////////////////////////
    virtual void		onMessage( SNetMsgHead* head, void* data, size_t len);

    /// purpose: �ͻ��˽���۲�
    virtual void        enterObserveRequest();

    /// purpose: �ͻ����˳��۲죨�رմ��ڣ�
    virtual void        exitObserveRequest();

public:
	// KinClient
	bool				Create(void);

private:
    /// purpose: ����ս�Ӹ���
    void                requestKinRename(char * szNewName);

    // purpose: �ӵ�������Ϣ
    void                onClientInvite(LPCSTR pszMsg, int nLen);

    // purpose: �����Ӧ
    void                answerInvite(DWORD dwPDBID, bool bIsAgree);

    /// purpose: ��������ְλ
    void                appointRequest(DWORD dwPDBID, int nIdentity);

    // purpose: �����������ս��
    void                inviteRequest(DWORD dwKinID, string strLeaveWord);

    // purpose: �����������ս��
    void                joinRequest(DWORD dwKinID, string strLeaveWord);

    // purpose: �����˳�ս��
    void                quitRequest();

    /// purpose: ���󿪳���Ա
    void                kickRequest(DWORD dwPDBID);

    /// purpose: ���󴴽�ս��
    void                createKinRequest(const char* szName, const char* szLabel);

    // purpose: ���³�Ա����
    void                onUpdateMember(LPCSTR pszMsg, int nLen);

    // purpose: �˳�ս��
    void                onQuit(LPCSTR pszMsg, int nLen);

    // purpose: ���³�Ա����
    void                onMsgKinList(LPCSTR pszMsg, int nLen);

    // purpose: �ͻ����¼�
    void                onClientEvent(LPCSTR pszMsg, int nLen);

    /// purpose: ��ɢս��
    void                dismissRequest();

    /// purpose: �޸Ĺ���
    void                MotifyRequest(int nPropID, char* szText);

	/// purpose: �鿴����ս����Ϣ
	void				onViewLegendCupKinInfo(LPCSTR pszMsg, int nLen);

	/// purpose: �鿴����ս������
	void				onViewLegendCupKinGlory(LPCSTR pszMsg, int nLen);

	// purpose: ����ս���б�
	void                onMsgUpdateKinList(LPCSTR pszMsg, int nLen);

private:
    CKin m_Kin;								// ս�Ӷ���

    SKinMember m_MemberInfo;				// ���ս����Ϣ
    bool m_bMemberVaild;					// ���ս����Ϣ�Ƿ���Ч
    int  m_myApplyKinID;					// ��ǰ���������еİ��
    int	 m_nCoolingTime;					// ������ʼʱ��
   
};

extern KinClient* g_pKinClient;