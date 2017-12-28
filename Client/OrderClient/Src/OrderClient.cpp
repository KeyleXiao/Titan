#include "stdafx.h"
#include "OrderClient.h"
#include "IClientEntity.h"
#include "GameViewDef.h"
#include "OrderMsgDef.h"
#include "ISchemeCenter.h"
#include "MatchSchemeDef.h"
#include "SchemeDef.h"
#include "IChatClient.h"


OrderClient* g_pOrderClient = NULL;
void OrderClient::release(void)
{
    gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_ORDER, this);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_ORDER_HIGH_LADDER);

    // 打开战队排行榜
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_ORDER_KIN);

    // 打开帮会排行榜
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_ORDER_CLAN);

    // 打开击杀排行榜
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_ORDER_PLAYERKILL);

    // 打开捐赠排行榜
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_ORDER_DONATE);

    // 打开英雄排行榜
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_ORDER_HERO);

    // 打开英雄类型排行榜
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_ORDER_HEROTYPE);

	// 打开本周导师排行榜
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_ORDER_CURWEEK_MASTER);

	// 打开本周学徒排行榜
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_ORDER_CURWEEK_PRENTICE);

	// 打开上周导师排行榜
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_ORDER_LASTWEEK_MASTER);

	// 打开上周学徒排行榜
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_ORDER_LASTWEEK_PRENTICE);

	// 打开宝石排行榜
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_ORDER_GEMSTONE);

	// 打开皮肤排行榜
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_ORDER_SKIN);

	// 打开英雄数量排行榜
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_ORDER_HEROCOUNT);

    //王者榜数据请求
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_OPEN_KING_OF_HERO_ORDER);
    
    // 个人排行榜设置
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SET_ORDER_PERSONINFO);


	// 全局指针NULL
	g_pOrderClient = NULL;

	delete this;
}


bool OrderClient::Create(void)
{
    IMessageDispatch* pMessageDispatch = gClientGlobal->getMessageDispatch();
    if (pMessageDispatch == NULL)
    {
        return false;
    }

    pMessageDispatch->registerMessageHandler(MSG_MODULEID_ORDER, this);	

    // 请求打开排行榜
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_OPEN_ORDER_HIGH_LADDER,this );

    // 打开战队排行榜
    gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_OPEN_ORDER_KIN,this );

    // 打开帮会排行榜
    gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_OPEN_ORDER_CLAN,this );

    // 打开击杀排行榜
    gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_OPEN_ORDER_PLAYERKILL,this );

    // 打开捐赠排行榜
    gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_OPEN_ORDER_DONATE,this );

    // 打开英雄排行榜
    gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_OPEN_ORDER_HERO,this );

    // 打开英雄类型排行榜
    gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_OPEN_ORDER_HEROTYPE,this );

	// 打开本周导师排行榜
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_OPEN_ORDER_CURWEEK_MASTER, this);

	// 打开本周学徒排行榜
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_OPEN_ORDER_CURWEEK_PRENTICE, this);

	// 打开上周导师排行榜
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_OPEN_ORDER_LASTWEEK_MASTER, this);

	// 打开上周学徒排行榜
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_OPEN_ORDER_LASTWEEK_PRENTICE, this);

	// 打开宝石排行榜
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_OPEN_ORDER_GEMSTONE, this);	

	// 打开皮肤排行榜
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_OPEN_ORDER_SKIN, this);

	// 打开英雄数量排行榜
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_OPEN_ORDER_HEROCOUNT, this);

    // 王者榜
    gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_OPEN_KING_OF_HERO_ORDER, this);

    // 个人排行榜设置
    gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SET_ORDER_PERSONINFO,this );
    

	return true;
}

bool OrderClient::onViewEvent(int32 eventID,int nParam,const char * strParam,void * ptrParam)
{
    switch ( eventID )
    {
    case GVIEWCMD_OPEN_ORDER_HIGH_LADDER:     // 打开天梯排行榜
        {
            onViewCmdOpenOrderHighLadder(nParam, strParam, ptrParam);
        }
        break;
    case GVIEWCMD_OPEN_ORDER_KIN:              // 打开战队排行榜
        {
            onViewCmdOpenOrderKin(nParam, strParam, ptrParam);
        }
        break;
    case GVIEWCMD_OPEN_ORDER_CLAN:             // 打开帮会排行榜
        {
            onViewCmdOpenOrderClan(nParam, strParam, ptrParam);
        }
        break;

    case GVIEWCMD_OPEN_ORDER_PLAYERKILL:        // 打开击杀排行榜
        {
            onViewCmdOpenOrderPlayerKill(nParam, strParam, ptrParam);
        }
        break;
    case GVIEWCMD_OPEN_ORDER_DONATE:            // 打开捐赠排行榜
        {
            onViewCmdOpenOrderDonate(nParam, strParam, ptrParam);
        }
        break;
    case GVIEWCMD_OPEN_ORDER_HERO:              // 打开英雄排行榜
        {
            onViewCmdOpenOrderHero(nParam, strParam, ptrParam);
        }
        break;
    case GVIEWCMD_OPEN_ORDER_HEROTYPE:          // 打开英雄类型排行榜
        {
            onViewCmdOpenOrderHeroType(nParam, strParam, ptrParam);
        }
        break;
	case GVIEWCMD_OPEN_ORDER_CURWEEK_MASTER:	// 打开本周导师排行榜
		{
			onViewCmdOpenOrderCurWeekMaster(nParam, strParam, ptrParam);
		}
		break;
	case GVIEWCMD_OPEN_ORDER_CURWEEK_PRENTICE:	// 打开本周学徒排行榜
		{
			onViewCmdOpenOrderCurWeekPrentice(nParam, strParam, ptrParam);
		}
		break;
	case GVIEWCMD_OPEN_ORDER_LASTWEEK_MASTER:	// 打开上周导师排行榜
		{
			onViewCmdOpenOrderLastWeekMaster(nParam, strParam, ptrParam);
		}
		break;
	case GVIEWCMD_OPEN_ORDER_LASTWEEK_PRENTICE:	// 打开上周学徒排行榜
		{
			onViewCmdOpenOrderLastWeekPrentice(nParam, strParam, ptrParam);
		}
		break;
	case GVIEWCMD_OPEN_ORDER_GEMSTONE:			// 打开宝石排行榜
		{
			onViewCmdOpenOrderGemStone(nParam, strParam, ptrParam);
		}
		break;
	case GVIEWCMD_OPEN_ORDER_SKIN:			// 打开皮肤排行榜
		{
			onViewCmdOpenOrderSkin(nParam, strParam, ptrParam);
		}
		break;
	case GVIEWCMD_OPEN_ORDER_HEROCOUNT:			// 打开英雄数量排行榜
		{
			onViewCmdOpenOrderHeroCount(nParam, strParam, ptrParam);
		}
		break;

    case GVIEWCMD_OPEN_KING_OF_HERO_ORDER:          // 打开英雄王者榜
        {
            onViewCmdOpenOrderKingOfHero(nParam, strParam, ptrParam);
        }
        break;
	
    case GVIEWCMD_SET_ORDER_PERSONINFO:          // 设置排行榜数据
        {
            onViewCmdSetOrderPersonInfo(nParam, strParam, ptrParam);
        }
        break;

    default:
        ErrorLn( __FUNCTION__": unhandled eventID=" << eventID );
        return false;
    }
        
    return true;
}

void OrderClient::onRequestPlayerOrderInfo( SNetMsgHead* head, void* data, size_t len )
{

    if ( sizeof(SMsg_OrderInfoHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_OrderInfoHead) > len" );
        return;
    }

    SMsg_OrderInfoHead* msg = (SMsg_OrderInfoHead*)data;
    if ( len < sizeof(SMsg_OrderInfoHead))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        return;
    }

    switch(msg->byOrderClass)
    {

    case EOC_Person_Order_Class:        // 个人排行
        {
            switch(msg->nKeyID)
            {
            case Player_MatchType_Key:          // 撮合类型榜 单独处理
                {
                    onRequestMatchTypeOrderInfo(head, data, len);
                }
                break;

            case Player_KingRank_Key:          // 王者排位榜单
                {
                    onRequestRankKingOrderInfo(head, data, len);
                }
                break;
                
            default:
                {
                    // 个人信息榜 键值榜
                    onRequestPersonKeyOrder(head, data, len);
                }
                break;
            }
        }
        break;

    case EOC_Hero_Order_Class:          // 英雄排行
        {
            switch(msg->nKeyID)
            {
            case Order_Hero_Develop_Key: 
                {
                    onRequestHeroOrderInfo(head, data, len);
                }
                break;

            case Order_Hero_Type_Key:
                {
                    
                    onRequestHeroTypeOrderInfo(head, data, len);
                }
                break;

            default:
                break;
            }

        }
        break;
    case EOC_Clan_Order_Class:          // 联盟排行
        {
            switch(msg->nKeyID)
            {
            case Order_Clan_Key:        
                {
                    onRequestClanOrderInfo(head, data, len);
                }
                break;


            default:
                break;
            }
        }
        break;

    case EOC_Kin_Order_Class:           // 战队排行
        {
            switch(msg->nKeyID)
            {
            case Order_Kin_Key:
                {
                    onRequestKinOrderInfo(head, data, len);
                }
                break;

            default:
                break;
            }

        }
        break;

    case EOC_Mentorship_Order_Class:    // 师徒排行
        {
            onRequestMentorshipOrder(head, data, len);
        }
        break;
    case EOC_King_Of_Hero_Class:    // 英雄王者榜
        {
            onRequestKingOfHeroOrder(head, data, len);
        }
        break;

    default:
        break;
    }
}

void OrderClient::onRequestPersonKeyOrder(SNetMsgHead* head, void* data, size_t len)
{
    // 响应请求排行榜数据信息
    if ( sizeof(SMsg_OrderInfoHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_OrderInfoHead) > len" );
        return;
    }
    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (NULL == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": NULL == pSchemeCenter");
        return;
    }
    ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
    if (NULL == pMatchRankConfigInfo)
    {
        ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
        return;
    }

    SMsg_OrderInfoHead* msg = (SMsg_OrderInfoHead*)data;
    if ( len != sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(PersonKeyOrderItem_OC))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__":  right len ="<<sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(PersonKeyOrderItem_OC));
        return;
    }

    switch(msg->nKeyID)
    {
    case Player_Kill_Key:       // 角色击杀榜
        {
            onRequestPlayerKillOrderInfo(head, data, len);
        }
        break;
    case Player_Donate_Key:     // 捐赠榜
        {
            onRequestDonateOrderInfo(head, data, len);
        }
        break;
    case Player_Gemstone_Key:   // 宝石榜
        {
            onRequestGemstoneOrderInfo(head, data, len);
        }
        break;
    case Player_Skin_Key:       // 皮肤榜
        {
            onRequestSkinOrderInfo(head, data, len);
        }
        break;
    case Player_Herocount_Key:  // 英雄数量榜
        {
            onRequestHeroCountOrderInfo(head, data, len);
        }
        break;

    }
}

void OrderClient::onMessage(SNetMsgHead* head, void* data, size_t len)
{
    TraceLn(__FUNCTION__": len= " << len);

    switch ( head->byKeyAction )
    {
       
    case OC_REQUEST_PLAYER_ORDER_INFO:
        {
            onRequestPlayerOrderInfo(head, data, len);
        }
        break;

    case OC_MSG_ORDER_RESPON_SETINFO:                          // 个人排行榜设置
        {
            onOrderResponSetInfo(head, data, len);
        }
        break;

    default:
        {
            ErrorLn(__FUNCTION__": unknown head->byKeyAction= " << head->byKeyAction);
        }
    }
}


void OrderClient::onRequestRankKingOrderInfo(SNetMsgHead* head, void* data, size_t len)
{
    // 响应请求排行榜数据信息
    if ( sizeof(SMsg_OrderInfoHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_MatchTypeOrderInfoHead) > len" );
        return;
    }

    SMsg_OrderInfoHead* msg = (SMsg_OrderInfoHead*)data;
    if ( len != sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(OrderMatchtype_Item))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__": right len ="<<sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(OrderMatchtype_Item));
        return;
    }

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (NULL == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": NULL == pSchemeCenter");
        return;
    }
    ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
    if (NULL == pMatchRankConfigInfo)
    {
        ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
        return;
    }

    // 清除内存数据
    EMMatchType eMatchType = (EMMatchType)msg->nDataEx;
    MAP_MATCHTYPE_PERSON_ORDER::iterator iter = mapMatchTypePersonOrder.find(eMatchType);
    if (iter != mapMatchTypePersonOrder.end())
    {
        iter->second.clear();
    }

    // 客户端显示层数据
    obuf obViewdata;
    cmd_Entity_Rank_Info_head cmdData;
    cmdData.nCount = msg->nCount;
    obViewdata.push_back(&cmdData, sizeof(cmd_Entity_Rank_Info_head));

    // 个人排行条目数据结构
    OrderMatchtype_Item tmpItem;
    OrderMatchtype_Item *pData = (OrderMatchtype_Item *)(msg+1);
    OrderMatchtype_Item* PersonItem;
    for (int i = 0; i< msg->nCount;  i++)
    {
        // 逻辑层数据 按排名顺序作为键值存储
        mapMatchTypePersonOrder[eMatchType][tmpItem.nOrder] = tmpItem;
        PersonItem = (OrderMatchtype_Item*)pData;
        memcpy( &tmpItem, PersonItem,sizeof(OrderMatchtype_Item));
        mapMatchTypePersonOrder[eMatchType][tmpItem.nOrder] = tmpItem;

        // 显示层数据
        cmd_Entity_HighLadder_Rank_Info cmdRankInfo;
		cmdRankInfo.bySex = PersonItem->bySex;			// 性别
        cmdRankInfo.dwUserID = PersonItem->dwUserID;
        cmdRankInfo.dwHeadID = PersonItem->wHeadID;     // 头像ID
        cmdRankInfo.nOrderID = PersonItem->nOrder;      // 排名ID
        cmdRankInfo.idActor = PersonItem->dwActorID;      // 角色ID 
        cmdRankInfo.nType = PersonItem->byMatchType;    // 天梯类型
        cmdRankInfo.nRankGrade = PersonItem->nGrade;	// 段位
        cmdRankInfo.nRankScore = PersonItem->nRankScore;// 天梯分数
        cmdRankInfo.dwClanID = PersonItem->dwClanID;// 帮会ID

        // 获得段位信息
        const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(cmdRankInfo.nType, cmdRankInfo.nRankGrade);
        if (pSchemeInfo != NULL)
        {
            cmdRankInfo.nRankIconID = pSchemeInfo->nIconID;
            sstrcpyn( cmdRankInfo.szGradeName,(char *)pSchemeInfo->szDes, sizeof(cmdRankInfo.szGradeName));
        }

        sstrcpyn( cmdRankInfo.szClanName, (char *)PersonItem->szClanName, sizeof(cmdRankInfo.szClanName));
        sstrcpyn( cmdRankInfo.szPlayerName, (char *)PersonItem->szActorName, sizeof(cmdRankInfo.szPlayerName));
        obViewdata.push_back(&cmdRankInfo, sizeof(cmd_Entity_HighLadder_Rank_Info));

        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_RANK_KING_ORDER_INFO, 0, 0, obViewdata.data(), obViewdata.size());
}


void OrderClient::onRequestMatchTypeOrderInfo(SNetMsgHead* head, void* data, size_t len)
{
    // 响应请求排行榜数据信息
    if ( sizeof(SMsg_OrderInfoHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_MatchTypeOrderInfoHead) > len" );
        return;
    }

    SMsg_OrderInfoHead* msg = (SMsg_OrderInfoHead*)data;
    if ( len != sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(OrderMatchtype_Item))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__": right len ="<<sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(OrderMatchtype_Item));
        return;
    }

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (NULL == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": NULL == pSchemeCenter");
        return;
    }
    ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
    if (NULL == pMatchRankConfigInfo)
    {
        ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
        return;
    }

    // 清除内存数据
    EMMatchType eMatchType = (EMMatchType)msg->nDataEx;
    MAP_MATCHTYPE_PERSON_ORDER::iterator iter = mapMatchTypePersonOrder.find(eMatchType);
    if (iter != mapMatchTypePersonOrder.end())
    {
        iter->second.clear();
    }
    
    // 客户端显示层数据
    obuf obViewdata;
    cmd_Entity_Rank_Info_head cmdData;
    cmdData.nCount = msg->nCount;
    obViewdata.push_back(&cmdData, sizeof(cmd_Entity_Rank_Info_head));

    // 个人排行条目数据结构
    OrderMatchtype_Item tmpItem;
    OrderMatchtype_Item *pData = (OrderMatchtype_Item *)(msg+1);
    OrderMatchtype_Item* PersonItem;
    for (int i = 0; i< msg->nCount;  i++)
    {
        // 逻辑层数据 按排名顺序作为键值存储
        mapMatchTypePersonOrder[eMatchType][tmpItem.nOrder] = tmpItem;
        PersonItem = (OrderMatchtype_Item*)pData;
        memcpy( &tmpItem, PersonItem,sizeof(OrderMatchtype_Item));
        mapMatchTypePersonOrder[eMatchType][tmpItem.nOrder] = tmpItem;

        // 显示层数据
        cmd_Entity_HighLadder_Rank_Info cmdRankInfo;
		cmdRankInfo.bySex = PersonItem->bySex;			// 性别
        cmdRankInfo.dwUserID = PersonItem->dwUserID;
        cmdRankInfo.dwHeadID = PersonItem->wHeadID;     // 头像ID
        cmdRankInfo.nOrderID = PersonItem->nOrder;      // 排名ID
        cmdRankInfo.idActor = PersonItem->dwActorID;      // 角色ID 
        cmdRankInfo.nType = PersonItem->byMatchType;    // 天梯类型
        cmdRankInfo.nRankGrade = PersonItem->nGrade;	// 段位
        cmdRankInfo.nRankScore = PersonItem->nRankScore;// 天梯分数
        cmdRankInfo.dwClanID = PersonItem->dwClanID;// 帮会ID
        
        // 获得段位信息
        const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(cmdRankInfo.nType, cmdRankInfo.nRankGrade);
        if (pSchemeInfo != NULL)
        {
            cmdRankInfo.nRankIconID = pSchemeInfo->nIconID;
            sstrcpyn( cmdRankInfo.szGradeName,(char *)pSchemeInfo->szDes, sizeof(cmdRankInfo.szGradeName));
        }
        
        sstrcpyn( cmdRankInfo.szClanName, (char *)PersonItem->szClanName, sizeof(cmdRankInfo.szClanName));
        sstrcpyn( cmdRankInfo.szPlayerName, (char *)PersonItem->szActorName, sizeof(cmdRankInfo.szPlayerName));
        obViewdata.push_back(&cmdRankInfo, sizeof(cmd_Entity_HighLadder_Rank_Info));

        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_MATCHTYPE_ORDER_RANK_INFO, 0, 0, obViewdata.data(), obViewdata.size());
}

void OrderClient::onViewCmdOpenOrderHighLadder(int nParam, const char * strParam, void * ptrParam)
{
    // 请求打开天梯排行榜
    int nRequestMatchType = nParam;
    nRequestMatchType = MatchType_Rank;

    obuf256 ob;
    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
    MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
    MsgHead.byKeyAction    = CO_REQUEST_ORDER_CLASS_INFO;
    ob <<MsgHead;

    SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Person_Order_Class;
    RequestInfo.nDataEx = nRequestMatchType;
    RequestInfo.nKeyID = Player_MatchType_Key;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenRankKingHighLadder(int nParam, const char * strParam, void * ptrParam)
{
    // 请求打开天梯排行榜
    int nRequestMatchType = nParam;
    nRequestMatchType = MatchType_Rank;

    obuf256 ob;
    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
    MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
    MsgHead.byKeyAction    = CO_REQUEST_ORDER_CLASS_INFO;
    ob <<MsgHead;

    SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Person_Order_Class;
    RequestInfo.nDataEx = nRequestMatchType;
    RequestInfo.nKeyID = Player_KingRank_Key;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenOrderKin(int nParam, const char * strParam, void * ptrParam)
{
    // 请求打开战队排行榜

    obuf256 ob;
    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
    MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
    MsgHead.byKeyAction    = CO_REQUEST_ORDER_CLASS_INFO;
    ob <<MsgHead;

    SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Kin_Order_Class;
    RequestInfo.nKeyID = Order_Kin_Key;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenOrderClan(int nParam, const char * strParam, void * ptrParam)
{
    // 客户端请求联盟榜
    obuf256 ob;
    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
    MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
    MsgHead.byKeyAction    = CO_REQUEST_ORDER_CLASS_INFO;
    ob <<MsgHead;

    SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Clan_Order_Class;
    RequestInfo.nKeyID = Order_Clan_Key;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenOrderPlayerKill(int nParam, const char * strParam, void * ptrParam)
{
    // 击杀榜
    obuf256 ob;
    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
    MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
    MsgHead.byKeyAction    = CO_REQUEST_ORDER_CLASS_INFO;
    ob <<MsgHead;

    SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Person_Order_Class;
    RequestInfo.nKeyID = Player_Kill_Key;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenOrderDonate(int nParam, const char * strParam, void * ptrParam)
{
    // 捐赠榜
    obuf256 ob;
    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
    MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
    MsgHead.byKeyAction    = CO_REQUEST_ORDER_CLASS_INFO;
    ob <<MsgHead;

    SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Person_Order_Class;
    RequestInfo.nKeyID = Player_Donate_Key;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenOrderHero(int nParam, const char * strParam, void * ptrParam)
{
    // 英雄养成榜
    obuf256 ob;
    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
    MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
    MsgHead.byKeyAction    = CO_REQUEST_ORDER_CLASS_INFO;
    ob <<MsgHead;

    SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Hero_Order_Class;
    RequestInfo.nKeyID = Order_Hero_Develop_Key;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenOrderHeroType(int nParam, const char * strParam, void * ptrParam)
{
    // 英雄类型榜
    if (ptrParam == NULL)
    {
        ErrorLn(__FUNCTION__": ptrParam == NULL");
        return;
    }
    cmd_order_herotype_request_info* pCmd = (cmd_order_herotype_request_info*)ptrParam;

    int nRequestMatchType = pCmd->nMatchType;
    obuf256 ob;
    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
    MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
    MsgHead.byKeyAction    = CO_REQUEST_ORDER_CLASS_INFO;
    ob <<MsgHead;

    SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Hero_Order_Class;
    RequestInfo.nKeyID = Order_Hero_Type_Key;
    RequestInfo.nDataEx = nRequestMatchType;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenOrderKingOfHero( int nParam, const char * strParam, void * ptrParam )
{
    int nHeroID = nParam;
	obuf256 ob;
	SNetMsgHead MsgHead;
	MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
	MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
	MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
	MsgHead.byKeyAction    = CO_REQUEST_ORDER_CLASS_INFO;
	ob <<MsgHead;

	SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_King_Of_Hero_Class;
    RequestInfo.nKeyID = nHeroID;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenOrderCurWeekPrentice( int nParam, const char * strParam, void * ptrParam )
{
	obuf256 ob;
	SNetMsgHead MsgHead;
	MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
	MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
	MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
	MsgHead.byKeyAction    = CO_REQUEST_ORDER_CLASS_INFO;
	ob <<MsgHead;

	SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Mentorship_Order_Class;
    RequestInfo.nKeyID = Player_CurWeek_Prentice_Key;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenOrderLastWeekMaster( int nParam, const char * strParam, void * ptrParam )
{
	obuf256 ob;
	SNetMsgHead MsgHead;
	MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
	MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
	MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
	MsgHead.byKeyAction    = CO_REQUEST_ORDER_CLASS_INFO;
	ob <<MsgHead;

	SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Mentorship_Order_Class;
    RequestInfo.nKeyID = Player_LastWeek_Master_Key;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenOrderLastWeekPrentice( int nParam, const char * strParam, void * ptrParam )
{
	obuf256 ob;
	SNetMsgHead MsgHead;
	MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
	MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
	MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
	MsgHead.byKeyAction    = CO_REQUEST_ORDER_CLASS_INFO;
	ob <<MsgHead;

	SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Mentorship_Order_Class;
    RequestInfo.nKeyID = Player_LastWeek_Prentice_Key;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenOrderCurWeekMaster( int nParam, const char * strParam, void * ptrParam )
{
    obuf256 ob;
    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
    MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
    MsgHead.byKeyAction    = CO_REQUEST_ORDER_CLASS_INFO;
    ob <<MsgHead;

    SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Mentorship_Order_Class;
    RequestInfo.nKeyID = Player_CurWeek_Master_Key;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenOrderGemStone( int nParam, const char * strParam, void * ptrParam )
{
	obuf256 ob;
	SNetMsgHead MsgHead;
	MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
	MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
	MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
	MsgHead.byKeyAction    = CO_REQUEST_ORDER_CLASS_INFO;
	ob <<MsgHead;

	SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Person_Order_Class;
    RequestInfo.nKeyID = Player_Gemstone_Key;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenOrderSkin(int nParam, const char * strParam, void * ptrParam)
{
	obuf256 ob;
	
	SNetMsgHead MsgHead;
	MsgHead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
	MsgHead.byKeyModule = MSG_MODULEID_ORDER;
	MsgHead.byKeyAction = CO_REQUEST_ORDER_CLASS_INFO;
	ob << MsgHead;

	SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Person_Order_Class;
    RequestInfo.nKeyID = Player_Skin_Key;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onViewCmdOpenOrderHeroCount(int nParam, const char * strParam, void * ptrParam)
{
	obuf256 ob;

	SNetMsgHead MsgHead;
	MsgHead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
	MsgHead.byKeyModule = MSG_MODULEID_ORDER;
	MsgHead.byKeyAction = CO_REQUEST_ORDER_CLASS_INFO;
	ob << MsgHead;

	SMsgOrderRequest_CO RequestInfo;
    RequestInfo.byOrderClass = EOC_Person_Order_Class;
    RequestInfo.nKeyID = Player_Herocount_Key;
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequest_CO));

    net_senddata( ob.data(), ob.size() );
}

void OrderClient::onRequestKinOrderInfo(SNetMsgHead* head, void* data, size_t len)
{
    // 响应请求排行榜数据信息
    if ( sizeof(SMsg_OrderInfoHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_MatchTypeOrderInfoHead) > len" );
        return;
    }

    SMsg_OrderInfoHead* msg = (SMsg_OrderInfoHead*)data;
    if ( len != sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(OrderKin_Item))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__":  right len ="<<sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(OrderKin_Item));
        return;
    }

    // 客户端显示层数据
    obuf obViewdata;
    cmd_Entity_Rank_Info_head cmdData;
    cmdData.nCount = msg->nCount;
    obViewdata.push_back(&cmdData, sizeof(cmd_Entity_Rank_Info_head));

    // 排行条目数据结构
    OrderKin_Item *pData = (OrderKin_Item *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
        // 显示层数据
        cmd_entity_kin_order_info cmdInfo;
        cmdInfo.nOrderID                = pData->nOrder;                                // 排名ID
        cmdInfo.dwKinID                 = pData->dwKinID;			                    // 战队ID
        cmdInfo.dwTotalFightCapacity    = pData->dwTotalFightCapacity;                   // 战力总和
        cmdInfo.dwShaikhID              = pData->dwShaikhID;							    // 族长ID
        cmdInfo.dwPlayerCount           = pData->dwPlayerCount;                          // 战队人数
        sstrcpyn( cmdInfo.szKinName, pData->szKinName, sizeof(cmdInfo.szKinName));
        sstrcpyn( cmdInfo.szShaikhName, pData->szShaikhName, sizeof(cmdInfo.szShaikhName));
        obViewdata.push_back(&cmdInfo, sizeof(cmdInfo));

        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_KIN_ORDER_RANK_INFO, 0, 0, obViewdata.data(), obViewdata.size());
}

void OrderClient::onRequestClanOrderInfo(SNetMsgHead* head, void* data, size_t len)
{
    // 响应请求排行榜数据信息
    if ( sizeof(SMsg_OrderInfoHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_OrderInfoHead) > len" );
        return;
    }

    SMsg_OrderInfoHead* msg = (SMsg_OrderInfoHead*)data;
    if ( len != sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(OrderClan_Item))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__":  right len ="<<sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(OrderClan_Item));
        return;
    }

    // 客户端显示层数据
    obuf obViewdata;
    cmd_Entity_Rank_Info_head cmdData;
    cmdData.nCount = msg->nCount;
    obViewdata.push_back(&cmdData, sizeof(cmd_Entity_Rank_Info_head));

    // 排行条目数据结构
    OrderClan_Item *pData = (OrderClan_Item *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
        // 逻辑层数据 按排名顺序作为键值存储

        // 显示层数据
        cmd_entity_clan_order_info cmdInfo;
        cmdInfo.nOrderID                = pData->nOrder;                                // 排名ID
        cmdInfo.dwClanID                 = pData->dwClanID;			                    // 战队ID
        cmdInfo.dwTotalFightCapacity    = pData->dwTotalFightCapacity;                  // 战力总和
        cmdInfo.dwShaikhID              = pData->dwShaikhID;							// 族长ID
        cmdInfo.dwPlayerCount           = pData->dwPlayerCount;                         // 战队人数
        sstrcpyn( cmdInfo.szKinName, pData->szClanName, sizeof(cmdInfo.szKinName));
        sstrcpyn( cmdInfo.szShaikhName, pData->szShaikhName, sizeof(cmdInfo.szShaikhName));
        obViewdata.push_back(&cmdInfo, sizeof(cmdInfo));

        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_CLAN_ORDER_RANK_INFO, 0, 0, obViewdata.data(), obViewdata.size());
}

void OrderClient::onRequestPlayerKillOrderInfo(SNetMsgHead* head, void* data, size_t len)
{
    // 响应请求排行榜数据信息
    if ( sizeof(SMsg_OrderInfoHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_OrderInfoHead) > len" );
        return;
    }
    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (NULL == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": NULL == pSchemeCenter");
        return;
    }
    ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
    if (NULL == pMatchRankConfigInfo)
    {
        ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
        return;
    }

    SMsg_OrderInfoHead* msg = (SMsg_OrderInfoHead*)data;
    if ( len != sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(PersonKeyOrderItem_OC))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__":  right len ="<<sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(PersonKeyOrderItem_OC));
        return;
    }

    // 客户端显示层数据
    obuf obViewdata;
    cmd_Entity_Rank_Info_head cmdData;
    cmdData.nCount = msg->nCount;
    obViewdata.push_back(&cmdData, sizeof(cmd_Entity_Rank_Info_head));

    // 排行条目数据结构
    PersonKeyOrderItem_OC *pData = (PersonKeyOrderItem_OC *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
        // 逻辑层数据 按排名顺序作为键值存储

        // 显示层数据
        cmd_entity_kill_order_info cmdInfo;
		cmdInfo.bySex	= pData->bySex;										// 性别
        cmdInfo.dwUserID = pData->dwUserID;
        cmdInfo.nPDBID  = pData->dwActorID;
        cmdInfo.nOrderID        = pData->nOrder;                                    // 排名 
        cmdInfo.dwHeadID        = pData->wHeadID ;                                  // 头像ID
        cmdInfo.n5KillNum       = pData->lValue1;                                   // 五杀次数
        cmdInfo.nSuperGodNum    = pData->lValue2;                                   // 超神次数
        cmdInfo.nMatchTypeID    = pData->byMatchType;                               // 段位类型
        cmdInfo.nMatchTypeRankGrad= pData->nGrade;                                  // 段位
        cmdInfo.dwClanID        = pData->dwClanID;			                        // 帮会ID
        sstrcpyn( cmdInfo.szClanName, pData->szClanName, sizeof(cmdInfo.szClanName));
        sstrcpyn( cmdInfo.szName, pData->szActorName, sizeof(cmdInfo.szName));
        

        // 获得段位信息
        const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(cmdInfo.nMatchTypeID, cmdInfo.nMatchTypeRankGrad);
        if (pSchemeInfo != NULL)
        {
            cmdInfo.nRankIconID = pSchemeInfo->nIconID;
            sstrcpyn( cmdInfo.szGradeName,(char *)pSchemeInfo->szDes, sizeof(cmdInfo.szGradeName));
        }
        obViewdata.push_back(&cmdInfo, sizeof(cmdInfo));
        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_KILL_ORDER_RANK_INFO, 0, 0, obViewdata.data(), obViewdata.size());
}

void OrderClient::onRequestDonateOrderInfo(SNetMsgHead* head, void* data, size_t len)
{
    // 响应请求排行榜数据信息
    if ( sizeof(SMsg_OrderInfoHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_OrderInfoHead) > len" );
        return;
    }

    SMsg_OrderInfoHead* msg = (SMsg_OrderInfoHead*)data;
    if ( len != sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(PersonKeyOrderItem_OC))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__":  right len ="<<sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(PersonKeyOrderItem_OC));
        return;
    }

    // 客户端显示层数据
    obuf obViewdata;
    cmd_Entity_Rank_Info_head cmdData;
    cmdData.nCount = msg->nCount;
    obViewdata.push_back(&cmdData, sizeof(cmd_Entity_Rank_Info_head));

    // 排行条目数据结构
    PersonKeyOrderItem_OC *pData = (PersonKeyOrderItem_OC *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
        // 逻辑层数据 按排名顺序作为键值存储
        // 显示层数据
        cmd_entity_donate_order_info cmdInfo;
		cmdInfo.bySex			= pData->bySex;										// 性别
        cmdInfo.dwUserID        = pData->dwUserID;
        cmdInfo.nPDBID          = pData->dwActorID;
        cmdInfo.dwHeadID        = pData->wHeadID ;                                   // 头像ID
        cmdInfo.nOrder          = pData->nOrder;                                    // 排名 
        cmdInfo.nClanCtrb       = pData->lValue2;							        // 帮会贡献度 2009.07.29     -- 捐赠
        cmdInfo.nTotalClanCtrb  = pData->lValue1;					                // 累计帮会贡献度 2009.07.29 -- 总捐赠
        cmdInfo.dwClanID        = pData->dwClanID;	                                // 帮会ID
        sstrcpyn( cmdInfo.szClanName, pData->szClanName, sizeof(cmdInfo.szClanName));
        sstrcpyn( cmdInfo.szName, pData->szActorName, sizeof(cmdInfo.szName));
        obViewdata.push_back(&cmdInfo, sizeof(cmdInfo));

        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_DONATE_ORDER_RANK_INFO, 0, 0, obViewdata.data(), obViewdata.size());
}

void OrderClient::onRequestHeroOrderInfo(SNetMsgHead* head, void* data, size_t len)
{
    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
        return;

    ISchemePersonModel* pModel = pSchemeCenter->getSchemePersonModel();
    if (pModel == NULL)
    {
        return;
    }
    ISchemeHeroStarData* pSchemeHeroStar = pSchemeCenter->getSchemeHeroStarInfo();

    // 响应请求排行榜数据信息
    if ( sizeof(SMsg_OrderInfoHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_OrderInfoHead) > len" );
        return;
    }

    

    SMsg_OrderInfoHead* msg = (SMsg_OrderInfoHead*)data;
    if ( len != sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(OrderHero_Item))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__":  right len ="<<sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(OrderHero_Item));
        return;
    }

    // 客户端显示层数据
    obuf obViewdata;
    cmd_Entity_Rank_Info_head cmdData;
    cmdData.nCount = msg->nCount;
    obViewdata.push_back(&cmdData, sizeof(cmd_Entity_Rank_Info_head));

    // 排行条目数据结构
    OrderHero_Item *pData = (OrderHero_Item *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
        // 逻辑层数据 按排名顺序作为键值存储

        // 显示层数据
        cmd_entity_hero_order_info cmdInfo;
		cmdInfo.bySex			= pData->bySex;
        cmdInfo.nOrder          = pData->nOrder;                                     // 排名
        cmdInfo.dwHeadID        = pData->wHeadID ;                                   // 头像ID
        cmdInfo.dwUserID        = pData->dwUserID ;                                   // 账号ID
        cmdInfo.dwPlayerID      = pData->dwPlayerID;                                 // 玩家ID
        cmdInfo.dwHeroID        = pData->dwHeroID;                                   // 英雄ID
        cmdInfo.nHeroScore      = pData->nHeroScore;                                 // 战绩
        cmdInfo.nHeroGrade      = pData->nHeroGrade;                                 // 熟练等级
        cmdInfo.nHeroStar       = pData->nHeroStar;                                  // 星级
        cmdInfo.dwWinNum        = pData->dwWinNum;                                   // 比赛胜利场次
        cmdInfo.dwMatchNum      = pData->dwMatchNum;                                 // 比赛场次
        cmdInfo.dwClanID        = pData->dwClanID;			                         // 帮会ID
        sstrcpyn( cmdInfo.szClanName, pData->szClanName, sizeof(cmdInfo.szClanName));
        sstrcpyn( cmdInfo.szName, pData->szName, sizeof(cmdInfo.szName));
		if(pData->dwHeroID > VOCATION_INVALID)
        {
			sstrcpyn( cmdInfo.szHeroName, pModel->getHeroName(pData->dwHeroID), sizeof(cmdInfo.szHeroName) );
		}
       
        // 填充星级相关附加显示信息
        if(pSchemeHeroStar)
        {
            SHeroStarSchemeInfo* pSchemeInfo = pSchemeHeroStar->getHeroStarData(cmdInfo.dwHeroID, cmdInfo.nHeroStar);
            if(pSchemeInfo)
            {
                cmdInfo.nStarIcon = pSchemeInfo->nStarIconID;       // 星级图标
    			sstrcpyn( cmdInfo.szStarDesc, pSchemeInfo->chStarDesc, sizeof(cmdInfo.szStarDesc) );    // 星级描述
            }
        }

        obViewdata.push_back(&cmdInfo, sizeof(cmdInfo));

        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_HERO_ORDER_RANK_INFO, 0, 0, obViewdata.data(), obViewdata.size());
}


void OrderClient::onRequestHeroTypeOrderInfo(SNetMsgHead* head, void* data, size_t len)
{
    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
        return;

    ISchemePersonModel* pModel = pSchemeCenter->getSchemePersonModel();
    if (pModel == NULL)
    {
        return;
    }

    if ( sizeof(SMsg_OrderInfoHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_OrderHeroTypeInfoHead) > len" );
        return;
    }

    SMsg_OrderInfoHead* msg = (SMsg_OrderInfoHead*)data;
    if ( len != sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(OrderHeroType_Item))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__":  right len ="<<sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(OrderHeroType_Item));
        return;
    }

    // 客户端显示层数据
    obuf obViewdata;
    cmd_Entity_Rank_Info_head cmdData;
    cmdData.nCount = msg->nCount;
    cmdData.nOtherCondition = msg->nDataEx; 
    obViewdata.push_back(&cmdData, sizeof(cmd_Entity_Rank_Info_head));

    // 排行条目数据结构
    OrderHeroType_Item *pData = (OrderHeroType_Item *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
        // 显示层数据
        cmd_entity_herotype_order_info cmdInfo;
        cmdInfo.byStaMatchType = pData->byMatchType;      // 0为综合类型 然后为统计某个比赛类型的
        cmdInfo.byHeroType = pData->byHeroType;             // 英雄的类型（战士 法师 刺客 ADC 辅助 ）

        cmdInfo.dwWinHeroID = pData->dwWinHeroID;           // 胜率最高英雄ID
        cmdInfo.WinNum = pData->WinNum;
        // 胜利率
        if(pData->dwWinHeroID > VOCATION_INVALID)
        {
            sstrcpyn( cmdInfo.szWinHeroName, pModel->getHeroName(pData->dwWinHeroID), sizeof(cmdInfo.szWinHeroName) );
        }

        cmdInfo.dwOnStageHeroID = pData->dwOnStageHeroID;    // 上场率最高英雄ID
        cmdInfo.OnStageNum = pData->OnStageNum;              // 上场率
        if(pData->dwOnStageHeroID > VOCATION_INVALID)
        {
            sstrcpyn( cmdInfo.szOnStageHeroName, pModel->getHeroName(pData->dwOnStageHeroID), sizeof(cmdInfo.szOnStageHeroName) );
        }
        
        cmdInfo.dwUpperFastestHeroID = pData->dwUpperFastestHeroID; // 最快提升英雄ID
        cmdInfo.dwUpperFastestNum = pData->dwUpperFastestNum;       // 最快提升
        if(pData->dwOnStageHeroID > VOCATION_INVALID)
        {
            sstrcpyn( cmdInfo.szUpperFastHeroName, pModel->getHeroName(pData->dwUpperFastestHeroID), sizeof(cmdInfo.szUpperFastHeroName) );
        }

        obViewdata.push_back(&cmdInfo, sizeof(cmd_entity_herotype_order_info));
        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_HEROTYPE_ORDER_RANK_INFO, 0, 0, obViewdata.data(), obViewdata.size());
}

void OrderClient::onOrderResponSetInfo(SNetMsgHead* head, void* data, size_t len)
{
    if ( sizeof(SMsgOrderResponSetInfo_OC) != len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsgOrderResponSetInfo_OC) != len" );
        return;
    }
    IChatClient *pChatClient = gClientGlobal->getChatClient();
    if (pChatClient == NULL)
    {
        ErrorLn(__FUNCTION__ << "pChatClient == NULL" );
        return;
    }

    bool bNeedUpdateView = false;
    SMsgOrderResponSetInfo_OC* pInfo =(SMsgOrderResponSetInfo_OC *) data;
    switch (pInfo->type)
    {
    case ORDER_PERSON_SET_NOTIFY:
        {
            // 更新到显示层数据中心
            bNeedUpdateView = true;
        }
        break;
    case ORDER_PERSON_SET_FAILED:
        {
            bNeedUpdateView = false;
            // 提示失败信息
            pChatClient->showSystemMessage(CHAT_TIP_ORDER_SAVE_PERSONSET_FAILED);
        }
        break;
    case ORDER_PERSON_SET_SUCESS:
        {
            // 提示成功信息并更新到显示层数据中心
            bNeedUpdateView = true;
            pChatClient->showSystemMessage(CHAT_TIP_ORDER_SAVE_PERSONSET_SUCESS);
        }
        break;
    default:
        break;
    }

    if (bNeedUpdateView =true)
    {
        
        cmd_entity_set_orderinfo cmdInfo;
        int tempSetInfo[GEOS_MAX] = {0};
        for (int i = 0; i < GEOS_MAX; ++i)
        {
            cmdInfo.szInfo[i] = (int)pInfo->personSetInfo[i];
        }

        gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_PERSON_ORDER_SET_INFO, 0, 0, &cmdInfo, sizeof(cmdInfo));
    }
    
}

void OrderClient::onRequestKingOfHeroOrder(SNetMsgHead* head, void* data, size_t len)
{
    // 响应请求排行榜数据信息
    if ( sizeof(SMsg_OrderInfoHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_OrderInfoHead) > len" );
        return;
    }
    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (NULL == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": NULL == pSchemeCenter");
        return;
    }
    ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
    if (NULL == pMatchRankConfigInfo)
    {
        ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
        return;
    }

    // 响应请求排行榜数据信息
    if ( sizeof(SMsg_OrderInfoHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_OrderInfoHead) > len" );
        return;
    }

    SMsg_OrderInfoHead* msg = (SMsg_OrderInfoHead*)data;
    if ( len != sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(PersonKeyOrderItem_OC))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__":  right len ="<<sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(PersonKeyOrderItem_OC));
        return;
    }

    // 客户端显示层数据
    obuf obViewdata;
    cmd_Entity_Rank_Info_head cmdData;
    cmdData.nOtherCondition = msg->nKeyID;
    cmdData.nCount = msg->nCount;
    obViewdata.push_back(&cmdData, sizeof(cmd_Entity_Rank_Info_head));

    // 排行条目数据结构
    PersonKeyOrderItem_OC *pData = (PersonKeyOrderItem_OC *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
        // 逻辑层数据 按排名顺序作为键值存储

        // 显示层数据
        cmd_entity_king_of_hero_order_info cmdInfo;
		cmdInfo.bySex			= pData->bySex;										// 性别
        cmdInfo.dwUserID		= pData->dwUserID;
        cmdInfo.nPDBID			= pData->dwActorID;
        cmdInfo.nOrderID        = pData->nOrder;                                    // 排名 
        cmdInfo.dwHeadID        = pData->wHeadID ;                                  // 头像ID
        cmdInfo.nWinMatch       = pData->lValue1;                                   // 胜利场次
        cmdInfo.nTotalMatch     = pData->lValue2;                                   // 比赛场次
        cmdInfo.nMatchTypeID    = pData->byMatchType;                               // 段位类型
        cmdInfo.nMatchTypeRankGrad= pData->nGrade;                                  // 段位
        cmdInfo.dwClanID        = pData->dwClanID;			                        // 帮会ID
        sstrcpyn( cmdInfo.szClanName, pData->szClanName, sizeof(cmdInfo.szClanName));
        sstrcpyn( cmdInfo.szName, pData->szActorName, sizeof(cmdInfo.szName));

        // 获得段位信息
        const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(cmdInfo.nMatchTypeID, cmdInfo.nMatchTypeRankGrad);
        if (pSchemeInfo != NULL)
        {
            cmdInfo.nRankIconID = pSchemeInfo->nIconID;
            sstrcpyn( cmdInfo.szGradeName,(char *)pSchemeInfo->szDes, sizeof(cmdInfo.szGradeName));
        }
        obViewdata.push_back(&cmdInfo, sizeof(cmdInfo));
        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_KING_OF_HERO_RANK_INFO, 0, 0, obViewdata.data(), obViewdata.size());
}

void OrderClient::onRequestMentorshipOrder( SNetMsgHead* head, void* data, size_t len )
{
	// 响应请求排行榜数据信息
	if ( sizeof(SMsg_OrderInfoHead) > len )
	{
		ErrorLn(__FUNCTION__ << "sizeof(SMsg_OrderInfoHead) > len" );
		return;
	}

	SMsg_OrderInfoHead* msg = (SMsg_OrderInfoHead*)data;
	if ( len != sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(OrderComPersonKey_Item))
	{
		ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
		ErrorLn(__FUNCTION__":  right len ="<<sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(OrderComPersonKey_Item));
		return;
	}

	// 客户端显示层数据
	obuf obViewdata;
	cmd_Entity_Rank_Info_head cmdData;
	cmdData.nCount = msg->nCount;
	obViewdata.push_back(&cmdData, sizeof(cmd_Entity_Rank_Info_head));

	// 排行条目数据结构
	OrderComPersonKey_Item *pData = (OrderComPersonKey_Item *)(msg+1);
	for (int i = 0; i< msg->nCount;  i++)
	{
		// 逻辑层数据 按排名顺序作为键值存储
		// 显示层数据
		cmd_entity_mentorship_order_info cmdInfo;
		cmdInfo.bySex			= pData->bySex;
		cmdInfo.nOrder          = pData->nOrder;                                    // 排名 
		cmdInfo.dwPDBID         = pData->dwActorID;                                   // 玩家ID
		cmdInfo.dwUserID        = pData->dwUserID;
		cmdInfo.dwShipVal       = pData->lValue1;							// 师徒值
		sstrcpyn( cmdInfo.szName, pData->szActorName, sizeof(cmdInfo.szName));
		obViewdata.push_back(&cmdInfo, sizeof(cmdInfo));

		pData++; 
	}

    switch(msg->nKeyID)
    {
    case Player_CurWeek_Master_Key:
        {
            gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_CURWEEK_MASTER_ORDER_RANK_INFO, 0, 0, obViewdata.data(), obViewdata.size());   
        }
        break;
    case Player_CurWeek_Prentice_Key:
        {
             gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_CURWEEK_PRENTICE_ORDER_RANK_INFO, 0, 0, obViewdata.data(), obViewdata.size());    
        }
        break;
    case Player_LastWeek_Master_Key:
        {
             gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LASTWEEK_MASTER_ORDER_RANK_INFO, 0, 0, obViewdata.data(), obViewdata.size());
        }
        break;
    case Player_LastWeek_Prentice_Key:
        {
             gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_LASTWEEK_PRENTICE_ORDER_RANK_INFO, 0, 0, obViewdata.data(), obViewdata.size());   
        }
        break;
    default:
        break;
    }
    
}


void OrderClient::onRequestGemstoneOrderInfo( SNetMsgHead* head, void* data, size_t len )
{
	// 响应请求宝石榜数据信息
	if ( sizeof(SMsg_OrderInfoHead) > len )
	{
		ErrorLn(__FUNCTION__ << "sizeof(SMsg_OrderInfoHead) > len" );
		return;
	}

	SMsg_OrderInfoHead* msg = (SMsg_OrderInfoHead*)data;
	if ( len != sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(PersonKeyOrderItem_OC))
	{
		ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
		ErrorLn(__FUNCTION__":  right len ="<<sizeof(SMsg_OrderInfoHead) + msg->nCount * sizeof(PersonKeyOrderItem_OC));
		return;
	}

	// 客户端显示层数据
	obuf obViewdata;
	cmd_Entity_Rank_Info_head cmdData;
	cmdData.nCount = msg->nCount;
	obViewdata.push_back(&cmdData, sizeof(cmd_Entity_Rank_Info_head));

	// 排行条目数据结构
	PersonKeyOrderItem_OC *pData = (PersonKeyOrderItem_OC *)(msg+1);
	for (int i = 0; i< msg->nCount;  i++)
	{
		// 逻辑层数据 按排名顺序作为键值存储
		// 显示层数据
		cmd_entity_gemstone_order_info cmdInfo;
		cmdInfo.bySex			= pData->bySex;										// 性别
		cmdInfo.nOrder          = pData->nOrder;                                    // 排名 
		cmdInfo.dwUserID        = pData->dwUserID;
        cmdInfo.dwPDBID         = pData->dwActorID;
		cmdInfo.dwFaceScore     = pData->lValue1;								// 颜值
		cmdInfo.dwPropVal		= pData->lValue2;								// 威力
        cmdInfo.dwClanID        = pData->dwClanID;
		sstrcpyn(cmdInfo.szName, pData->szActorName, sizeof(cmdInfo.szName));
		obViewdata.push_back(&cmdInfo, sizeof(cmdInfo));

		pData++; 
	}

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_GEMSTONE_ORDER_RANK_INFO, 0, 0, obViewdata.data(), obViewdata.size());
}

void OrderClient::onRequestSkinOrderInfo(SNetMsgHead*head, void*data, size_t len)
{
	if (data == NULL || len < sizeof(SMsg_OrderInfoHead))
	{
		ErrorLn(__FUNCTION__": message data is invalid, length = " << len);
		return;
	}

	SMsg_OrderInfoHead * pMsgHead = (SMsg_OrderInfoHead *)data;
	if (len != sizeof(SMsg_OrderInfoHead) + pMsgHead->nCount * sizeof(PersonKeyOrderItem_OC))
	{
		ErrorLn(__FUNCTION__": message len is invalid, message len = " << len << "right len = " << sizeof(SMsg_OrderInfoHead) + pMsgHead->nCount * sizeof(PersonKeyOrderItem_OC));
		return;
	}

	obuf obViewData;

	cmd_Entity_Rank_Info_head cmdHead;
	cmdHead.nCount = pMsgHead->nCount;
	obViewData.push_back(&cmdHead, sizeof(cmd_Entity_Rank_Info_head));

	PersonKeyOrderItem_OC * pMsgNode = (PersonKeyOrderItem_OC *)(pMsgHead + 1);
	for (int i = 0; i < pMsgHead->nCount; ++i)
	{
		cmd_entity_skin_order_info cmdNode;
		cmdNode.bySex = pMsgNode->bySex;										// 性别
		cmdNode.nOrder = pMsgNode->nOrder;
		cmdNode.dwPDBID = pMsgNode->dwActorID;
		cmdNode.dwUserID = pMsgNode->dwUserID;
		cmdNode.dwSkinCount = pMsgNode->lValue1;
        cmdNode.dwClanID = pMsgNode->dwClanID;
		//cmdNode.dwLimitedSkinCount = pMsgNode->dwLimitedSkinCount;
		//cmdNode.dwLegendSkinCount = pMsgNode->dwLegendSkinCount;
		//cmdNode.dwEpicSkinCount = pMsgNode->dwEpicSkinCount;
		//cmdNode.dwNormalSkinCount = pMsgNode->dwNormalSkinCount;
		sstrcpyn(cmdNode.szName, pMsgNode->szActorName, sizeof(cmdNode.szName));

		obViewData.push_back(&cmdNode, sizeof(cmd_entity_skin_order_info));
		++pMsgNode;
	}

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_SKIN_ORDER_RANK_INFO, 0 , 0, obViewData.data(), obViewData.size());
}

void OrderClient::onRequestHeroCountOrderInfo(SNetMsgHead*head, void*data, size_t len)
{
	if (data == NULL || len < sizeof(SMsg_OrderInfoHead))
	{
		ErrorLn(__FUNCTION__": message data is invalid, length = " << len);
		return;
	}

	SMsg_OrderInfoHead * pMsgHead = (SMsg_OrderInfoHead *)data;

	if (len != sizeof(SMsg_OrderInfoHead) + pMsgHead->nCount * sizeof(PersonKeyOrderItem_OC))
	{
		ErrorLn(__FUNCTION__": message length is invalid, message len = " << len << "correct len = " << sizeof(SMsg_OrderInfoHead) + pMsgHead->nCount * sizeof(PersonKeyOrderItem_OC));
		return;
	}

	obuf obViewData;

	cmd_Entity_Rank_Info_head cmdHead;
	cmdHead.nCount = pMsgHead->nCount;
	obViewData.push_back(&cmdHead, sizeof(cmd_Entity_Rank_Info_head));

	PersonKeyOrderItem_OC * pMsgNode = (PersonKeyOrderItem_OC *)(pMsgHead + 1);
	for (int i = 0; i < pMsgHead->nCount; ++i)
	{
		cmd_entity_herocount_order_info cmdNode;
		cmdNode.bySex = pMsgNode->bySex;										// 性别
		cmdNode.nOrder = pMsgNode->nOrder;
        cmdNode.dwPDBID = pMsgNode->dwActorID;
		cmdNode.dwUserID = pMsgNode->dwUserID;
		cmdNode.dwHeroCount = pMsgNode->lValue1;
        cmdNode.dwClanID  = pMsgNode->dwClanID;
		sstrcpyn(cmdNode.szName, pMsgNode->szActorName, sizeof(cmdNode.szName));

		obViewData.push_back(&cmdNode, sizeof(cmd_entity_herocount_order_info));
		++pMsgNode;
	}

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_HEROCOUNT_ORDER_RANK_INFO, 0, 0, obViewData.data(), obViewData.size());
}


void OrderClient::onViewCmdSetOrderPersonInfo(int nParam, const char * strParam, void * ptrParam)
{
    if (nullptr == ptrParam)
    {
        ErrorLn(__FUNCTION__": nullptr == ptrParam " );
        return;
    }

    cmd_entity_set_orderinfo *pCmd = (cmd_entity_set_orderinfo*) ptrParam;
    obuf256 ob;
    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
    MsgHead.byKeyModule    = MSG_MODULEID_ORDER;
    MsgHead.byKeyAction    = CO_REQUEST_SET_ORDER_SETTINGINFO;
    ob <<MsgHead;


    SMsgOrderRequestSetInfo_CO RequestInfo;
    for (int i = 0; i < GEOS_MAX; ++i)
    {
        RequestInfo.personSetInfo[i] = (uchar)(pCmd->szInfo[i]);
    }
    ob.push_back(&RequestInfo, sizeof(SMsgOrderRequestSetInfo_CO));

    net_senddata( ob.data(), ob.size() );
}


/** 输出函数
@param   
@param   
@return  
*/
API_EXPORT IOrderClient * CreateOrderClient(void)
{
	// 如果创建了，则直接返回
	if(g_pOrderClient != NULL)
	{
		return g_pOrderClient;
	}

	// 实例化BUFF客户端
	OrderClient * pOrderClient = new OrderClient();
	if(pOrderClient == NULL || !pOrderClient->Create())
	{
		safeRelease(pOrderClient);
		return NULL;
	}

	g_pOrderClient = pOrderClient;
	return g_pOrderClient;
}