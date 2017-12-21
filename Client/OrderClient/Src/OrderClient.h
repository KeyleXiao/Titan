/*******************************************************************
** �ļ���:	OrderClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	������
** ��  ��:	2015/10/19  17:21
** ��  ��:	1.0
** ��  ��:	���а�ϵͳ�ͻ���
** Ӧ  ��:  	
	
*******************************************************************/

#pragma once
#include "IOrderClient.h"
#include "IEntityClient.h"
#include "MatchDef.h"
#include "OrderDef.h"

class OrderClient:public IOrderClient, public IGameViewEventHandler, public IMessageHandler
{
public:

	////////////////////////////IOrderClient//////////////////////////////////////////////
	virtual void		release(void);

    ////////////////////////////IGameViewEventHandler////////////////////////////////////////////////////
    virtual bool		onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );

    /////////////////////////////IMessageHandler/////////////////////////////////////////////////////////
    virtual void		onMessage( SNetMsgHead* head, void* data, size_t len);

public:
	// OrderClient
	bool				Create(void);
private:

    // ��Ӧ������ֵ���а���Ϣ
    void onRequestPlayerOrderInfo( SNetMsgHead* head, void* data, size_t len );
    //  ��Ӧ�������а���Ϣ
    void onRequestMatchTypeOrderInfo( SNetMsgHead* head, void* data, size_t len );

    //  ��Ӧ�������а���Ϣ
    void onRequestRankKingOrderInfo( SNetMsgHead* head, void* data, size_t len );

    //  ��ɫ��ֵ��
    void onRequestPersonKeyOrder( SNetMsgHead* head, void* data, size_t len );

    void onRequestKinOrderInfo( SNetMsgHead* head, void* data, size_t len );

    void onRequestClanOrderInfo( SNetMsgHead* head, void* data, size_t len );

    void onRequestPlayerKillOrderInfo( SNetMsgHead* head, void* data, size_t len );

    void onRequestDonateOrderInfo( SNetMsgHead* head, void* data, size_t len );

    void onRequestHeroOrderInfo( SNetMsgHead* head, void* data, size_t len );

    void onRequestHeroTypeOrderInfo( SNetMsgHead* head, void* data, size_t len );

    void onOrderResponSetInfo( SNetMsgHead* head, void* data, size_t len );

	void onRequestMentorshipOrder( SNetMsgHead* head, void* data, size_t len );

    void onRequestKingOfHeroOrder( SNetMsgHead* head, void* data, size_t len );

	void onRequestGemstoneOrderInfo( SNetMsgHead* head, void* data, size_t len );

    void onRequestSkinOrderInfo(SNetMsgHead*head, void*data, size_t len);

	void onRequestHeroCountOrderInfo(SNetMsgHead*head, void*data, size_t len);

    // ��ʾ�㵽�߼���
    void onViewCmdOpenOrderHighLadder( int nParam, const char * strParam, void * ptrParam );

    // ��ʾ�㵽�߼���
    void onViewCmdOpenRankKingHighLadder( int nParam, const char * strParam, void * ptrParam );

    void onViewCmdOpenOrderKin( int nParam, const char * strParam, void * ptrParam );

    void onViewCmdOpenOrderClan( int nParam, const char * strParam, void * ptrParam );

    void onViewCmdOpenOrderPlayerKill( int nParam, const char * strParam, void * ptrParam );

    void onViewCmdOpenOrderDonate( int nParam, const char * strParam, void * ptrParam );

    void onViewCmdOpenOrderHero( int nParam, const char * strParam, void * ptrParam );

    void onViewCmdOpenOrderHeroType( int nParam, const char * strParam, void * ptrParam );

	void onViewCmdOpenOrderCurWeekMaster( int nParam, const char * strParam, void * ptrParam );

	void onViewCmdOpenOrderCurWeekPrentice( int nParam, const char * strParam, void * ptrParam );

	void onViewCmdOpenOrderLastWeekMaster( int nParam, const char * strParam, void * ptrParam );

	void onViewCmdOpenOrderLastWeekPrentice( int nParam, const char * strParam, void * ptrParam );

	void onViewCmdOpenOrderGemStone( int nParam, const char * strParam, void * ptrParam );

	void onViewCmdOpenOrderSkin(int nParam, const char * strParam, void * ptrParam);

	void onViewCmdOpenOrderHeroCount(int nParam, const char * strParam, void * ptrParam);

    void onViewCmdSetOrderPersonInfo(int nParam, const char * strParam, void * ptrParam );

    void onViewCmdOpenOrderKingOfHero(int nParam, const char * strParam, void * ptrParam );
    

private:
    // ����˳�� ���а�����
    typedef map<DWORD, OrderMatchtype_Item> MAP_PERSON_ORDER_ITEM;
    typedef map<EMMatchType, MAP_PERSON_ORDER_ITEM> MAP_MATCHTYPE_PERSON_ORDER;

    MAP_MATCHTYPE_PERSON_ORDER mapMatchTypePersonOrder;
};

extern OrderClient* g_pOrderClient;