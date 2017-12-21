//==========================================================================
/**
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/15/2015	17：45
** 版  本:	1.0
** 描  述:	排行榜消息定义
*/
//==========================================================================

#ifndef __ORDERMSGDEF_H__
#define __ORDERMSGDEF_H__

#include "GatewayServiceDef.h"
#include "MatchDef.h"
#include "NetMessageDef.h"
#include "IGameDatabaseDef.h"
#include "Match_Manageddef.h"
using namespace Match_ManagerDef;

#include "Order_ManagedDef.h"
using namespace Order_ManagedDef;

using namespace Gateway;
// 消息ID定义
//////////////////////////////////////////////////////////////////////////
enum ORDER_NET_MESSAGE_KEYACTION
{
    /////////////////////////////////////客户端到场景服/////////////////////////////////////

	/////////////////////////////////////客户端到场景服/////////////////////////////////////
    CO_REQUEST_ORDER_CLASS_INFO,                            // 请求排行类型的数据信息

    CO_REQUEST_SET_ORDER_SETTINGINFO,                       // 设置个人排行榜显示

	/////////////////////////////////////场景服到社会服/////////////////////////////////////
    

    SO_UPDATE_PERSON_MATCHTYPE_ORDERINFO,                   // 场景服请求更新到社会服排行榜数据


    /////////////////////////////////////社会服到客户端/////////////////////////////////////

    OC_MSG_ORDER_RESPON_SETINFO,                             // 排行榜个人设置

    OC_REQUEST_PLAYER_ORDER_INFO,                        // 角色键值排行榜

    OS_MSG_KING_RANK_ORDER,                             // 排位王者榜排名

};

//OS_MSG_KING_RANK_ORDER
struct SMsgKingRankOrder
{
    DWORD dwActorID;				// 玩家ID
    EMMatchType eMatchType;         // 比赛类型
    int nOrderID;                   // 玩家王者排名
    int nGradeID;                   // 王者排位等级ID
};

// CO_REQUEST_ORDER_CLASS_INFO
struct SMsgOrderRequest_CO
{
    BYTE byOrderClass;				// 请求的排行类别 EOrderClass
    int nKeyID;                     // 请求类型ID
    int nDataEx;                    // 附加数据
};

// 通用比赛类型排行榜头信息
struct SMsg_OrderInfoHead
{
    BYTE byOrderClass;				// 请求的排行类别 EOrderClass
    int  nKeyID;                    // 请求类型ID
    int  nDataEx;                   // 附加数据
    int  nCount;

    SMsg_OrderInfoHead()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsg_RequestHeroTypeOrderInfo
{
    ClientID        client;
    EMMatchType	    eMatchType;

    SMsg_RequestHeroTypeOrderInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};



// 更新社会服排行榜数据 SO_UPDATE_PERSON_MATCHTYPE_ORDERINFO
struct SMsg_UpdatePersonMatchTypeRank
{
    PDBID       idActor;
    EMMatchType	eMatchType;
    char        szActorName[ACTORNAME_MAXSIZE];
    MatchTypeRankNode RankNodeInfo;

    SMsg_UpdatePersonMatchTypeRank()
    {
        memset(this, 0, sizeof(*this));
    }
};


struct SMsgOrderResponSetInfo_OC
{
    ulong pdbid;				// 玩家dbid
    uchar type;					// 类型
    uchar personSetInfo[EOS_Max_Set_Class];	// 排榜设置的数据

    SMsgOrderResponSetInfo_OC()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgOrderRequestSetInfo_CO
{
	ulong pdbid;
	uchar personSetInfo[EOS_Max_Set_Class];
    SMsgOrderRequestSetInfo_CO()
    {
        memset(this, 0, sizeof(*this));
    }
};



#endif // __ORDERMSGDEF_H__