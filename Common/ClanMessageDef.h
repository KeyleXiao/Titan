/*******************************************************************
** 文件名:	ClanMessageDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/1/2016	10:39
** 版  本:	1.0
** 描  述:	帮会消息码和结构定义
********************************************************************/

#ifndef __CLANMESSAGEDEF_H__
#define __CLANMESSAGEDEF_H__

#include "ClanDef.h"
#include "NameLenDef.h"

// 上层的消息码必定是MSG_MODULEID_CLAN
// 服务器层的全局定义
enum EMMSG_CLAN
{
    MSG_CLAN_MODULE = 1,							// 模块消息码
    MSG_CLAN_CLAN,									// 帮会消息码
};

// 消息码定义键值
enum CLAN_NET_MESSAGE
{
    MSG_CLAN_CREATE = 0,            // 建立帮会
    MSG_CLAN_DISMISS,               // 解散帮会
    MSG_CLAN_CANEL_DISMISS,         // 取消解散
    MSG_CLAN_APPLY,                 // 申请加入
    MSG_CLAN_ADMIT,                 // 接收申请者
    MSG_CLAN_KICK,                  // 剔除成员
    MSG_CLAN_QUIT,                  // 退出帮会
    MSG_CLAN_EXITOBSERVE,           // 取消观察帮会数据
    MSG_CLAN_LIST,                  // 获取帮会列表
    MSG_CLAN_UPDATESELFCLANINFO,    // 更新自身帮会数据
	MSG_CLAN_UPDATEMEMBERINFO,      // 更新成员数据
	MSG_CLAN_REMOVEMEMBERINFO,		// 移除成员数据
    MSG_CLAN_UPDATECLANINTPROP,     // 更新帮会数字属性
	MSG_CLAN_UPDATECLANSTRPROP,     // 更新帮会字符串属性
    MSG_CLAN_CLANBASEDATA,          // 申请帮会基本数据
    MSG_CLAN_SETSTRINGPROP,         // 修改宣言，公告
    MSG_CLAN_UPDATEID,              // 更改ID
    MSG_CLAN_REMOVECLAN_ZONE,       // 社会服向场景服删除帮会
    MSG_CLAN_UPDATECLAN_ZONE,       // 社会服向场景服更新帮会（全部）
    MSG_CLAN_UPDATECLANPROP_ZONE,   // 社会服向场景服更新帮会属性
    MSG_CLAN_REMOVEMEMBER_ZONE,     // 社会服向场景服删除成员
    MSG_CLAN_UPDATEMEMBER_ZONE,     // 社会服向场景服更新成员
    MSG_CLAN_UPDATECLIENT,          // 更新客户端
    MSG_CLAN_ZONEPOSTMSG,           // 场景服消息转发
    MSG_CLAN_KICKKINMEMBER,         // 踢掉成员
    MSG_CLAN_UPDATEEVENT,           // 帮会事件
    MSG_CLAN_CREATE_SUCCEED,        // 创建帮会成功
    MSG_CLAN_REQUEST_CARD_LIST,     // 请求卡牌列表
    MSG_CLAN_REQUEST_CARD,          // 请求卡牌支援
    MSG_CLAN_DONATE_CARD,           // 捐赠卡牌
    MSG_CLAN_ADD_DONATE_CARD,       // 增加捐赠卡牌
    MSG_CLAN_UPDATE_REQUEST_CARD,   // 更新请求卡牌
    MSG_CLAN_REMOVE_REQUEST_CARD,   // 移除请求卡牌
	MSG_CLAN_REQUEST_DONATE_LOG,	// 请求捐赠记录
	MSG_CLAN_UPDATE_DONATE_LOG,		// 更新捐赠记录
	MSG_CLAN_ADD_REQUEST_TIP,		// 增加请求提示
	MSG_CLAN_SENDREQUESTCARDINFO,	// 发送请求捐卡信息
    MSG_CLAN_APPOINT,               // 任命职位
    MSG_CLAN_SET_CLANBASEDATA,      // 设置帮会基础数据(比如更换帮主时,要把基础数据发下去)
	MSG_CLAN_APPLY_RET,             // 申请加入返回
	MSG_CLAN_SETINTPROP,			// 设置数字属性
	MSG_CLAN_INVITE,			    // 邀请加入联盟
	MSG_CLAN_ANSWER_INVITE,         // 邀请答复
    MSG_CLAN_LEGEND_CUP_DIDA,       // 联盟杯赛创建DIDA
    MSG_CLAN_MAXID,
};

#pragma pack(1)
struct msg_clan_sub_head
{
    DWORD   dwMsgCode;
    DWORD	dwClanID;

    msg_clan_sub_head(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_CREATE///////////////////////////////
// 消息体
struct msg_clan_create_co
{
	char szName[CLAN_NAME_SIZE];					// 帮会名
	char szManifesto[CLAN_MANIFESTO_SIZE];			// 帮会宣言
	int nClanFlag;									// 帮会旗帜

    msg_clan_create_co()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct msg_clan_create_os
{
	char szName[CLAN_NAME_SIZE];						// 帮会名
	char szManifesto[CLAN_MANIFESTO_SIZE];				// 帮会宣言
	int nClanFlag;										// 帮会旗帜
	DWORD dwPDBID;										// 创建者PDBID
	DWORD dwKinID;										// 战队ID
	DWORD dwClanID;										// 帮会ID

    msg_clan_create_os()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct msg_clan_create_so
{
	DWORD dwPDBID;										// 创建者PDBID
	DWORD dwKinID;										// 战队ID
	char  szShaikhName[ACTORNAME_MAXSIZE];				// 角色名字
	DWORD dwClanID;										// 帮会ID
	char  szName[CLAN_NAME_SIZE];						// 帮会名
	char  szManifesto[CLAN_MANIFESTO_SIZE];				// 帮会宣言
	int   nClanFlag;									// 帮会旗帜
	int   nEstablishTime;								// 创建时间

    msg_clan_create_so()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_APPLY///////////////////////////////
// 消息体
struct msg_clan_apply_oc
{
	DWORD dwClanID;								// 现在的帮会ID
	DWORD dwKinID;								// 战队ID
	char  szName[KIN_NAME_SIZE];				// 战队名字
	char  szShaikhName[ACTORNAME_MAXSIZE];		// 族长名字
    int   nMemberCount;                         // 战队成员数量

	msg_clan_apply_oc()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////MSG_CLAN_APPLY_RET///////////////////////////////
// 消息体
struct msg_clan_apply_ret_oc
{
	DWORD dwOldClanID;
	DWORD dwClanID;								// 帮会ID
	WORD nApplystate;

	msg_clan_apply_ret_oc()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////MSG_CLAN_ADMIT///////////////////////////////
// 消息体
struct msg_clan_admit_co
{
	DWORD dwKinID;										// 申请战队ID
	bool  bAgree;										// 同意/拒绝

    msg_clan_admit_co()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct msg_clan_admit_oc
{
	DWORD dwClanID;										// 现在的帮会ID
	DWORD dwKinID;										// 战队ID

	msg_clan_admit_oc()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////MSG_CLAN_KICK///////////////////////////////
// 消息体
struct msg_clan_kick_co
{
	DWORD dwMemberPDBID;								// 成员ID

    msg_clan_kick_co()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_LIST///////////////////////////////
// 消息体
struct msg_clan_clanlist_co
{
	int nPageNum;										// 页码
	char szClanName[CLAN_NAME_SIZE];					// 帮会名

    msg_clan_clanlist_co()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct msg_clan_clanlist_node_oc
{
	DWORD dwID;											// 帮会ID
	char  szName[CLAN_NAME_SIZE];						// 帮会名
	char  szShaikhName[ACTORNAME_MAXSIZE];				// 帮主名字
	WORD  nFlag;										// 帮会旗帜
	BYTE  nLevel;										// 帮会等级
	BYTE  nKinCount;									// 帮会战队数
	char  szManifesto[CLAN_MANIFESTO_SIZE];				// 帮会宣言
	WORD  nMemberCount;									// 帮会人数
	WORD  nState;										// 帮会状态
    int   nClanTotalFight;                              // 帮会总战力
	int   nWeekActivity;                                // 周活跃度

    msg_clan_clanlist_node_oc()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct msg_clan_clanlist_oc
{
	int nClanCount;										// 帮会个数
	int nPageNum;										// 返回页码
	int nTotalClanCount;								// 总帮会个数
	int nApplyClanID;									// 自己申请的帮会

	// nClanCount * msg_clan_clanlist_node_oc

    msg_clan_clanlist_oc()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_UPDATESELFCLANINFO///////////////////////////////
// 消息体
// SHeroClanInfo

///////////////////////////////MSG_CLAN_UPDATEMEMBERINFO///////////////////////////////
// 消息体
// SClientClanMember

///////////////////////////////MSG_CLAN_REMOVEMEMBERINFO///////////////////////////////
// 消息体
struct msg_clan_removemember_oc
{
	DWORD dwPDBID;				// 玩家ID

	msg_clan_removemember_oc()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////MSG_CLAN_UPDATECLANINTPROP///////////////////////////////
// 消息体
struct msg_clan_updateintprop_oc
{
	int nPropID;
	int nValue;

	msg_clan_updateintprop_oc()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////MSG_CLAN_UPDATECLANSTRPROP///////////////////////////////
// 消息体
struct msg_clan_updatestringprop_oc
{
	int nPropID;
	char szValue[CLAN_MAX_PROPLEN];

	msg_clan_updatestringprop_oc()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////MSG_CLAN_CLANBASEDATA///////////////////////////////
// 消息体
struct msg_clan_clanbasedata_oc
{
	char szClanName[CLAN_NAME_SIZE];				// 帮会名
	DWORD dwShaikhID;								// 族长
	char szShaikhName[ACTORNAME_MAXSIZE];			// 族长名
	BYTE nClanLevel;								// 帮会等级
	char szManifesto[CLAN_MANIFESTO_SIZE];			// 帮会宣言
	char szNotice[CLAN_NOTICE_SIZE];			    // 帮会公告
	int nMemberCount;								// 成员个数
	int nConstructDegree;							// 建设度
	int nGold;										// 帮会金钱
	int nActivity;									// 活跃度
	int nWeekActivity;                              // 周活跃度
	int nPlace;										// 排名
    BYTE nApplyMemberNum;							// 申请中成员的个数
	int nKickTime;								    // 上一次踢人的时间
	int nKickNum;									// 踢人的次数 每日踢人次数速度配置表
    int nTotalFight;                                // 总战力值
    int nKinCount;                                  // 战队数量
    int nMaxKinCount;                               // 最大战队数量
	int nLegendCupCount;							// 举办杯赛次数
	LONGLONG llLegendCupID;							// 举办联盟杯赛ID
	int nAutoAcceptFlag;							// 自动接受标示
	int nCurTime;									// 当前时间
    int nDismissApplyTime;                          // 联盟申请解散时间
    int nState;                                     // 联盟解散状态
	int nShaikhState;                               // 盟主状态

	// 成员列表信息	nMemberCount * SClientClanMember
    msg_clan_clanbasedata_oc()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_SETSTRINGPROP//////////////////////////
struct msg_clan_setstringprop_co
{
	int nPropID;										// 属性ID
	char szText[CLAN_MANIFESTO_SIZE];					// 属性内容

    msg_clan_setstringprop_co()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_SETINTPROP//////////////////////////
struct msg_clan_setintprop_co
{
	int nPropID;										// 属性ID
	int nValue;											// 值

	msg_clan_setintprop_co()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////MSG_CLAN_UPDATEID///////////////////////////////
struct msg_clan_setclanid_os
{
	DWORD dwPDBID;										// 玩家ID
	DWORD dwClanID;										// 帮会ID

    msg_clan_setclanid_os()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_REMOVECLAN_ZONE///////////////////////////////
struct msg_clan_removeclan_os
{
	DWORD dwClanID;											// 帮会ID

    msg_clan_removeclan_os()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_UPDATECLAN_ZONE///////////////////////////////
struct msg_clan_updateclan_os
{
	DWORD dwID;											// 帮会ID
	char  szName[CLAN_NAME_SIZE];						// 帮会名
	int   nState;										// 帮会的状态：临时，正式，解散中
	int   nLevel;										// 帮会等级
	int   nConstructDegree;								// 建设度
	int   nGold;										// 黄金
	int   nShaikhID;									// 帮主PDBID
	char  szShaikhName[ACTORNAME_MAXSIZE];				// 帮主名字
	int   nActivity;									// 活跃度
	int   nWeekActivity;                                // 周活跃度
	int   nPlace;										// 排名
	int   nWorldID;										// 游戏世界ID
	int	  nKickTime;									// 踢人时间
	int	  nKickNum;										// 踢人的次数
	int	  nMana;										// 帮会威望
	int	  nLegendCupCount;								// 可举行杯赛次数

	msg_clan_updateclan_os()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////MSG_CLAN_UPDATECLANPROP_ZONE///////////////////////////////
// 消息体
struct msg_clan_updateintprop_os
{
	DWORD dwClanID;										// 帮会ID
	BYTE nPropID;
	int nValue;

    msg_clan_updateintprop_os()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_REMOVEMEMBER_ZONE///////////////////////////////
struct msg_clan_removekin_os
{
	DWORD dwKinID;										// 战队ID
	DWORD dwClanID;										// 帮会ID

    msg_clan_removekin_os()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_UPDATEMEMBER_ZONE///////////////////////////////
// 消息体
//...SZoneClanMember

///////////////////////////////MSG_CLAN_UPDATECLIENT///////////////////////////////
// 消息体
struct msg_clan_updateclaninfo_oc
{
	DWORD dwID;								// 帮会ID
	BYTE  nState;							// 帮会状态
	char  szName[CLAN_NAME_SIZE];			// 帮会名字
	DWORD dwShaikhID;						// 帮主
	char  szShaikhName[ACTORNAME_MAXSIZE];	// 帮主名字

    msg_clan_updateclaninfo_oc()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_ZONEPOSTMSG///////////////////////////////
struct msg_clan_zonepostmsg_os
{
	DWORD dwPDBID;							// 玩家ID
	DWORD dwMsgID;							// 真正消息码ID
	//...后面跟消息的数据

    msg_clan_zonepostmsg_os()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_KICKKINMEMBER///////////////////////////////
struct msg_clan_kickkinmember_co	
{
	DWORD dwMemberPDBID;						// 成员ID

    msg_clan_kickkinmember_co()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_UPDATEEVENT///////////////////////////////
// 帮会事件消息结构
struct msg_clan_updateclanlog_oc
{
	WORD wCount;										// 事件个数
	DWORD wLen;											// 事件总长度
	// msg_clan_updateclanlog_single * wCount
	msg_clan_updateclanlog_oc()
	{
		memset(this, 0, sizeof(*this));
	}
};
// 帮会事件消息结构
struct msg_clan_updateclanlog_single
{
	int nTime;										// 发生时间
	WORD wLen;										// 内容长度
	// char *
	msg_clan_updateclanlog_single()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////MSG_CLAN_CREATE_SUCCEED///////////////////////////////
// 创建帮会成功
struct msg_clan_oncreatesucceed_oc
{
	bool bSucceed;

	msg_clan_oncreatesucceed_oc()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////MSG_CLAN_REQUEST_CARD_LIST///////////////////////////////
// 请求卡牌列表消息子结构
struct msg_clan_requestcardlist_single
{
	DWORD dwPDBID;			            // 请求支援玩家
	char szName[ACTORNAME_MAXSIZE];     // 请求支援玩家名字
	int nCardID;                        // 卡牌ID
	int nCurCount;                      // 当前卡牌数量
	int nMaxCount;                      // 最大卡牌数量
    int nDonateCardCount;               // 可捐赠卡牌次数
    int nDayDonateCardCount;            // 一天可捐赠卡牌次数

	msg_clan_requestcardlist_single()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 请求卡牌列表消息结构       社会服-场景服
struct msg_clan_requestcardlist_os
{
    DWORD dwPDBID;
    WORD  nCount;
    // nCount * msg_clan_requestcardlist_single

    msg_clan_requestcardlist_os()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 请求卡牌列表消息结构       场景服-客户端
struct msg_clan_requestcardlist_sc
{
    WORD  nCount;
    // nCount * msg_clan_requestcardlist_single

    msg_clan_requestcardlist_sc()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_REQUEST_CARD///////////////////////////////
// 请求卡牌消息结构
struct msg_clan_requestcard_co
{
    int nCardID;            // 卡牌ID

    msg_clan_requestcard_co()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 请求卡牌消息结构
struct msg_clan_requestcard_os
{
    DWORD dwPDBID;          // 玩家PDBID
    int nCardID;            // 卡牌ID

    msg_clan_requestcard_os()
    {
        memset(this, 0, sizeof(*this));
    }
};


// 请求卡牌消息结构
struct msg_clan_requestcard_so
{
    DWORD dwPDBID;          // 玩家PDBID
    int nCardID;            // 卡牌ID
	int nLevel;				// 玩家等级
	int nDayGetCount;		// 玩家当天已受捐数量

    msg_clan_requestcard_so()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_DONATE_CARD///////////////////////////////
// 捐赠卡牌消息结构
struct msg_clan_donatecard_co
{
    DWORD dwByDBID;         // 被捐赠DBID
    int nCardID;            // 卡牌ID
    int nCardCount;         // 卡牌数量

    msg_clan_donatecard_co()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 捐赠卡牌消息结构
struct msg_clan_donatecard_os
{
    DWORD dwPDBID;          // 捐赠DBID
    DWORD dwByDBID;         // 被捐赠DBID
	int nDonateLevel;		// 捐赠者等级
	int nTargetLevel;		// 被捐赠者等级
    int nCardID;            // 卡牌ID
    int nCardCount;         // 卡牌数量

    msg_clan_donatecard_os()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 捐赠卡牌消息结构
struct msg_clan_donatecard_so
{
	DWORD dwPDBID;          // 捐赠DBID
	char szName[ACTORNAME_MAXSIZE];     // 捐赠者名称
    DWORD dwByDBID;         // 被捐赠DBID
    int nCardID;            // 卡牌ID
    int nCardCount;         // 卡牌数量

    msg_clan_donatecard_so()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_ADD_DONATE_CARD///////////////////////////////
// 增加捐赠卡牌消息结构
struct msg_clan_adddonatecard_os
{
	DWORD dwPDBID;          // 捐赠DBID
	char szName[ACTORNAME_MAXSIZE];     // 捐赠者名称
    DWORD dwByDBID;         // 被捐赠DBID
    int nCardID;            // 卡牌ID
    int nCardCount;         // 卡牌数量

    msg_clan_adddonatecard_os()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_UPDATE_REQUEST_CARD///////////////////////////////
// 更新请求卡牌
struct msg_clan_updaterequestcard_os
{
    DWORD dwSendPDBID;                  // 发送玩家PDBID
    DWORD dwPDBID;			            // 请求支援玩家
    char szName[ACTORNAME_MAXSIZE];     // 请求支援玩家名字
    int nCardID;                        // 卡牌ID
    int nCurCount;                      // 当前卡牌数量
    int nMaxCount;                      // 最大卡牌数量
    int nDonateCardCount;               // 可捐赠卡牌次数

    msg_clan_updaterequestcard_os()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 更新请求卡牌
struct msg_clan_updaterequestcard_sc
{
    DWORD dwPDBID;			            // 请求支援玩家
    char szName[ACTORNAME_MAXSIZE];     // 请求支援玩家名字
    int nCardID;                        // 卡牌ID
    int nCurCount;                      // 当前卡牌数量
    int nMaxCount;                      // 最大卡牌数量
    int nDonateCardCount;               // 可捐赠卡牌次数
    int nDayDonateCardCount;            // 一天可捐赠卡牌次数

    msg_clan_updaterequestcard_sc()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_REMOVE_REQUEST_CARD///////////////////////////////
// 移除请求卡牌
struct msg_clan_removerequestcard_oc
{
    DWORD dwPDBID;			            // 移除请求支援玩家

    msg_clan_removerequestcard_oc()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_REQUEST_DONATE_LOG///////////////////////////////
// 请求捐赠记录
struct msg_clan_requestdonatelog_oc
{
	int   nCount;
	// dwCount*msg_clan_updatedonatelog_oc

    msg_clan_requestdonatelog_oc()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_UPDATE_DONATE_LOG///////////////////////////////
// 更新捐赠记录
struct msg_clan_updatedonatelog_oc
{
	PDBID nGaveActorID;							// 赠与玩家ID
	char szGaveActorName[ACTORNAME_MAXSIZE];	// 赠与玩家名字
	PDBID nTakeActorID;							// 被赠与玩家ID
	char szTakeActorName[ACTORNAME_MAXSIZE];	// 被赠与玩家名字
	int nCardID;								// 卡牌ID
	int nCardNum;								// 卡牌数量

    msg_clan_updatedonatelog_oc()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_ADD_REQUEST_TIP///////////////////////////////
// 增加请求提示
struct msg_clan_addrequesttip_oc
{
	WORD nIsInit;
	WORD nCount;

    msg_clan_addrequesttip_oc()
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////MSG_CLAN_SENDREQUESTCARDINFO///////////////////////////////
// 发送请求捐卡信息
struct msg_clan_sendrequestcardinfo_os
{
	DWORD dwPDBID;

	msg_clan_sendrequestcardinfo_os()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct msg_clan_sendrequestcardinfo_sc
{
	DWORD nCooldownTime;
	DWORD nLeftRequestNum;

	msg_clan_sendrequestcardinfo_sc()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////MSG_CLAN_APPOINT///////////////////////////////
struct msg_clan_appoint_co
{
    DWORD dwMemberPDBID;								// 成员ID
    
    msg_clan_appoint_co()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 邀请加入 MSG_CLAN_INVITE
struct msg_clan_invite_co
{
	DWORD dwInviteePDBID;			// 被邀请者ID
	msg_clan_invite_co()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 邀请加入 MSG_CLAN_INVITE
struct msg_clan_invite_oc
{
	int nInviterPDBID;                                   // 邀请者角色ID
	char szInviterName[GAME_NAME_MAXSIZE];              // 角色名
	char szClanName[CLAN_NAME_SIZE];					// 联盟名
	msg_clan_invite_oc()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 答复邀请 MSG_CLAN_ANSWER_INVITE
struct msg_clan_answer_invite_co
{
	DWORD dwInviterPDBID;								// 邀请者ID
	bool bIsAgree;										// 是否同意

	msg_clan_answer_invite_co()
	{
		memset(this, 0, sizeof(*this));
	}
};


#pragma pack()

#endif  // __CLANMESSAGEDEF_H__