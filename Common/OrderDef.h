/*******************************************************************
** 文件名:	E:\speed\Common\OrderDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/15/2015	15：50
** 版  本:	1.0
** 描  述:	
            排行榜相关定义
********************************************************************/

#pragma once
#include "PackagePtr.h"
#include "IGameDatabaseDef.h"
#include "Order_ManagedDef.h"
using namespace Order_ManagedDef;

// 输出DEBUG信息
#ifdef VERSEION_DEBUG
#   define ORDER_DEBUG_OUTPUT
#endif
#ifdef ORDER_DEBUG_OUTPUT
#	define	ORDER_TRACELN(x)			TraceLn(x)
#	define	ORDER_WARNINGLN(x)			WarningLn(x)
#	define	ORDER_ERRORLN(x)			ErrorLn(x)
#	define	ORDER_EMPHASISLN(x)			EmphasisLn(x)
#else
#	define	ORDER_TRACELN(x)
#	define	ORDER_WARNINGLN(x)
#	define	ORDER_ERRORLN(x)
#	define	ORDER_EMPHASISLN(x)
#endif



#define ORDER_MAX_ITEM  200


typedef struct DBREQ_RESULT_MATCHTYPE_ORDERINFO_PERSON      OrderMatchtype_Item;

typedef struct DBREQ_RESULT_HEROORDER_DETAIL_INFO           OrderHero_Item;     // 英雄养成榜数据

typedef struct DBREQ_RESULT_HEROTYPE_ORDER_DETAIL_INFO      OrderHeroType_Item; // 英雄类型榜数据

typedef struct DBREQ_PARAM_TABLE_ORDERINFO_PERSON           OrderComPersonKey_Item;// 角色键值通用排行榜


struct PersonKeyOrderItem_OC
{
    int		nOrder;  								// 排行名次
    WORD    wHeadID;                                // 头像Id
    DWORD   dwUserID;                               // 账号ID
    int		nLastOrder;								// 上次排名
    DWORD	dwActorID;								// 玩家DBID
    char	szActorName[DBDEF_ACTORNAME_MAXSIZE];	// 角色名称
    long	lValue1;								// 具体排行值1
    long	lValue2;								// 具体排行值2
    DWORD   dwKinID;                                // 家族ID
    char    szKinName[DBDEF_KINNAME_MAXSIZE];		// 战队名称
    DWORD   dwClanID;                               // 帮会ID
    char    szClanName[DBDEF_CLANNAME_MAXSIZE];		// 帮会名称
    BYTE    byMatchType;                             // 排位相关信息
    DWORD   nRankScore;
    DWORD   nGrade;
    

    PersonKeyOrderItem_OC(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

struct OrderKin_Item
{
    int     nOrder;
    DWORD   dwKinID;			                        // 战队ID
    char    szKinName[DBDEF_KINNAME_MAXSIZE];			// 战队名称
    DWORD   dwTotalFightCapacity;                       // 战力总和
    DWORD   dwShaikhID;							        // 族长ID
    char    szShaikhName[DBDEF_ACTORNAME_MAXSIZE];	    // 族长名字
    DWORD   dwPlayerCount;                              // 战队人数
    OrderKin_Item(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

struct OrderClan_Item
{
    int     nOrder;
    DWORD   dwClanID;			                        // 帮会ID
    char    szClanName[DBDEF_CLANNAME_MAXSIZE];			// 战队名称
    DWORD   dwTotalFightCapacity;                       // 战力总和
    DWORD   dwShaikhID;							        // 帮主ID
    char    szShaikhName[DBDEF_ACTORNAME_MAXSIZE];	    // 帮主名字
    DWORD   dwPlayerCount;                              // 帮会人数

    OrderClan_Item(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// 排行榜返回数据类型
enum OrderSetType
{
	ORDER_PERSON_SET_NOTIFY = 0,			 //	服务器通知客户端
	ORDER_PERSON_SET_FAILED,				 //	设置排行返回失败
	ORDER_PERSON_SET_SUCESS,				 // 设置排行榜成功
};


// 排行榜显示设置
struct Order_Set_Info 
{
    char personSetInfo[EOS_Max_Set_Class];
    Order_Set_Info()
    {
        memset(personSetInfo,0,sizeof(personSetInfo));
    }
};

// 公共结构体定义

template<typename TMsg>
bool getOrderMsg(TMsg*& pMsg, void*& pData, size_t& stLen)
{
	if (nullptr == pData)
	{
		WarningLn("getMsg nullptr == pData");
		return false;
	}
	if ( stLen < sizeof(TMsg) )
	{
		WarningLn("getMsg stLen < sizeof(TMsg) stLen=" << stLen << " sizeof(TMsg)=" << sizeof(TMsg));
		return false;
	}

	pMsg	= (TMsg *)pData;
	pData	= (void*)(pMsg+1);
	stLen	-= sizeof(TMsg);

	return true;
}


static SNetMsgHead*	getOrderMsgHead( PACKAGE_PTR msg, void*& pData, size_t& stLen )
{
	pData	= (void*)msg->c_str();
	stLen	= msg->size();

	Trace("SNetMsgHead*	getOrderMsgHead" << msg->size() );

	SNetMsgHead* pMsg;
	if ( !getOrderMsg( pMsg, pData, stLen ) )
	{
		WarningLn("getMsgHead failed! !getMsg");
		return nullptr;
	}
	return pMsg;
}

// 消息
//////////////////////////////////////////////////////////////////////////
// CHSS	ORDER_SCENE_SERVER		场景服上消息
// C	CLIENT					客户端
// CHS	ORDER_SERVER				聊天服

#include "OrderMsgDef.h"

