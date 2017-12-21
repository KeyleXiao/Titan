/*******************************************************************
** 文件名:	OrderClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/10/19  17:21
** 版  本:	1.0
** 描  述:	排行榜系统客户端
** 应  用:  	
	
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

    // 响应公共键值排行榜信息
    void onRequestPlayerOrderInfo( SNetMsgHead* head, void* data, size_t len );
    //  响应请求排行榜信息
    void onRequestMatchTypeOrderInfo( SNetMsgHead* head, void* data, size_t len );

    //  响应请求排行榜信息
    void onRequestRankKingOrderInfo( SNetMsgHead* head, void* data, size_t len );

    //  角色键值榜
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

    // 显示层到逻辑层
    void onViewCmdOpenOrderHighLadder( int nParam, const char * strParam, void * ptrParam );

    // 显示层到逻辑层
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
    // 排名顺序 排行榜数据
    typedef map<DWORD, OrderMatchtype_Item> MAP_PERSON_ORDER_ITEM;
    typedef map<EMMatchType, MAP_PERSON_ORDER_ITEM> MAP_MATCHTYPE_PERSON_ORDER;

    MAP_MATCHTYPE_PERSON_ORDER mapMatchTypePersonOrder;
};

extern OrderClient* g_pOrderClient;