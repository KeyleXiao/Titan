/*******************************************************************
** �ļ���:	LegendCupClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	������
** ��  ��:	2015/10/19  17:21
** ��  ��:	1.0
** ��  ��:	����ϵͳ�ͻ���
** Ӧ  ��:  	
	
*******************************************************************/

#pragma once
#include "ILegendCupClient.h"
#include "IEntityClient.h"
#include "MatchDef.h"
#include "LegendCupDef.h"
#include "LegendCupMsgDef.h"

class LegendCupClient:public ILegendCupClient, public IGameViewEventHandler, public IMessageHandler
{
public:

	////////////////////////////ILegendCupClient//////////////////////////////////////////////
	virtual void		Release(void);

    ////////////////////////////IGameViewEventHandler////////////////////////////////////////////////////
    virtual bool		onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );

    /////////////////////////////IMessageHandler/////////////////////////////////////////////////////////
    virtual void		onMessage( SNetMsgHead* head, void* data, size_t len);

public:
	// LegendCupClient
	bool				Create(void);
private:
    // �����ĺ�����
    void onMsgSendAllBalcklist( SNetMsgHead* head, void* data, size_t len );

    // ������ĳ������Ĳ�����Ա
    void onMsgSendKinMember( SNetMsgHead* head, void* data, size_t len );

    // ���б�����Ϣ
    void onMsgSendAllLegendcup( SNetMsgHead* head, void* data, size_t len );

    // ����������ս����Ϣ
    void onMsgSendAllKinlist( SNetMsgHead* head, void* data, size_t len );

    // �����ı�����Ϣ
    void onMsgSendAllKinMemberinfo( SNetMsgHead* head, void* data, size_t len );

    // ����ÿ��ʱ�����Ϣ
    void onMsgSendAllRoundTimeinfo( SNetMsgHead* head, void* data, size_t len );

    // ������ÿ����Ϣ
    void onMsgSendAllRoundInfo( SNetMsgHead* head, void* data, size_t len );

    // ��ѯ�����ڵ�ս����Ϣ
    void onMsgSendNodeLookupWarInfo(SNetMsgHead* head, void* data, size_t len);

    // ���µ���������Ϣ
    void onMsgSendUpdateSingelCupInfo(SNetMsgHead* head, void* data, size_t len);

    // ���µ��������ڵ���Ϣ
    void onMsgSendUpdateSingelCupNodeInfo(SNetMsgHead* head, void* data, size_t len);

    // ��������ɹ���Ϣ
    void onMsgSendRegisterSuccess(SNetMsgHead* head, void* data, size_t len);

    // ��������Ӧ���������ɹ�
    void onMsgPublicSuccess(SNetMsgHead* head, void* data, size_t len);

    // �ٰ�����Ӧ���������ɹ�
    void onMsgCreaterPublicSuccess(SNetMsgHead* head, void* data, size_t len);


    // ��ô��������������
    void onMsgGetCreateCupTypeInfo(SNetMsgHead* head, void* data, size_t len);

    // ����ö��������
    void reqJudgeSingalWarResult(SMsgJudgeWarResult *pMsgInfo);

    // �鿴�����ڵ�ս����Ϣ
    void reqLookUpCupNodeWarInfo(SMsgLookUpCupNodeWarInfo *pMsgInfo);

    // ��������������
    void reqJoinLegendCupRoom(SMsgJoinLegendCupRoom *pMsgInfo);

    // ����ս�ӳ�Ա�б�
    void reqKinMemberList(SMsgRequestKinMember *pMsgInfo);

    // ���������
    void reqAllBalcklist(SMsgRequestAllBlackKinList *pMsgInfo);

    // �鿴����
    void reqlookOverLegendCup(SMsgRequestLookOverCup *pMsgInfo);

    // �ƶ���������
    void reqAddToBlacklist(SMsgAddToBlackList *pMsgInfo);

    // ɾ��������
    void reqRemoveBlacklist(SMsgRemoveBlackList *pMsgInfo);

    // ��������
    void reqPublickCompetition(SMsgPublicCompetition *pMsgInfo);
    
    // �˳�����
    void reqExitCup(SMsgExitCup *pMsgInfo);

    // ������뱭��
    void reqJoinCup(SMsgJoinCup *pMsgInfo);

    // ���󴴽�����
    void reqCreateCup(SMsgCreateCup *pMsgInfo);

    // �������б�
    void  requestCupList();

	// ���󴴽�����
	void reqCreateType(SMsgCreateCupType *pMsgInfo);

	// �����˳������۲�
	void reqQuitObserve(SMsgReqQuitObserve *pMsgInfo);

    // ����������±��������׶�
    void  reqOfflinePreEndRegister(SMsgOfflineEndRegister *pMsgInfo);

private:
    
};

extern LegendCupClient* g_pLegendCupClient;