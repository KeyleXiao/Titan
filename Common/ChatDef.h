/*******************************************************************
** 文件名:	E:\speed\Common\ChatDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	10/15/2015	15：50
** 版  本:	1.0
** 描  述:	
            聊天系统相关公共定义
********************************************************************/

#pragma once
#include "GatewayServiceDef.h"
#include "PackagePtr.h"

//输出DEBUG信息
#ifdef VERSEION_DEBUG
#define CHAT_DEBUG_OUTPUT
#endif

#ifdef CHAT_DEBUG_OUTPUT
# define CHAT_TRACELN(x)			TraceLn(x)
# define CHAT_WARNINGLN(x)			WarningLn(x)
# define CHAT_ERRORLN(x)			ErrorLn(x)
# define CHAT_EMPHASISLN(x)			EmphasisLn(x)
#else
# define CHAT_TRACELN(x)
# define CHAT_WARNINGLN(x)
# define CHAT_ERRORLN(x)
# define CHAT_EMPHASISLN(x)
#endif

using Gateway::ClientID;

#define MAX_LOUDSPEAKER_MSG_LEN			512	   		// 喇叭消息最大长度
#define MAX_CHAT_MSG_LENGTH				256			// 聊天信息长度
#define SYSTEMMESSAGE_MAX_LEN			512			// 系统提示最大长度
#define CHAT_MSG_ITEM_FLAG				(char)21	// 聊天物件头标记
#define FLOAT_TEXT_DEFAULT_TIME			1000		// 飘字默认时间
#define CHAT_NEARBY_DISTANCE			24			// 附近聊听消息接收范围
//#define MAX_REPORT_MSG_LENGTH			256			// 举报附加信息
//#define REPORT_COLD_TICKCOUNT			180000		// 举报的冷却时间间隔
#define SYSTEMMESSAGE_SCHEME_FILENAME	"Scp\\SystemMessage.csv"
#define CHATCD_SCHEME_FILENAME	"Scp\\ChatCD.csv"

#define LOUDSPEAKER_CALLTIMER_HUGE      25000    // 喇叭的广播间隔时间

// 公共枚举定义
#include "Chat_ManagedDef.h"

// 系统消息广播类型定义
enum EMChatBroadcastType
{
	CHAT_BROADCASTTYPE_SINGLE,		// 对单人(传PDBID)
	CHAT_BROADCASTTYPE_TEAMMATE,	// 对全队(传PDBID)
	CHAT_BROADCASTTYPE_WARSCENE,	// 对全场(传场景ID)
	CHAT_BROADCASTTYPE_SERVER,		// 对全场景服(传场景服ID)
	CHAT_BROADCASTTYPE_KIN,			// 对战队(传战队ID)
	CHAT_BROADCASTTYPE_CLAN,		// 对帮会(传帮会ID)
	CHAT_BROADCASTTYPE_ROOM,		// 对房间(传房间ID)
	CHAT_BROADCASTTYPE_WORLD,		// 对世界(传PDBID)
	CHAT_BROADCASTTYPE_LOUDSPEAKER,	// 对世界(传0)
	CHAT_BROADCASTTYPE_ZONE,		// 对区域(传场景ID)
	CHAT_BROADCASTTYPE_CAMP,		// 对阵营(战场内传场景ID，房间内传PDBID)

	CHAT_BROADCASTTYPE_MAX			// 最大类型数
};

// SystemMessage.csv脚本记录使用标识
enum EMChatTextUseFlag
{
	CHAT_TEXTUSEFLAG_CLIENT    = 0x01,     // 客户端使用 1
	CHAT_TEXTUSEFLAG_SCENE     = 0x02,     // 场景服使用 2
	CHAT_TEXTUSEFLAG_SOCIAL    = 0x04,     // 社会服使用 4
	CHAT_TEXTUSEFLAG_LOGIN     = 0x08,     // 登陆服使用 8
	CHAT_TEXTUSEFLAG_GATEWAY   = 0x10,     // 网关服使用 16
};

// 玩家举报信息给GM类型定义
enum EMChatReportToGMType
{
    // 未定义
    CHAT_REPORTTOGMTYPE_NULL = 0,		// 未知
    CHAT_REPORTTOGMTYPE_REPORT,			// 玩家举报信息
    CHAT_REPORTTOGMTYPE_REPORTCHAT,		// 玩家与回复Gm聊天 dwUserData1 回传举报信息ID,聊天信息放在用户说明szMsg中
    CHAT_REPORTTOGMTYPE_REPLYFAIL,		// GM回复失败 dwUserData1 回传举报信息ID

    CHAT_REPORTTOGMTYPE_MAX				// 最大类型数
};

// 玩家举报信息给GM原因标识,位标识
enum EMChatReportToGMReasonFlag
{
    CHAT_REPORTTOGMREASON_NULL			= 0x0,			// 0	:无
    CHAT_REPORTTOGMREASON_OTHER			= 0x1,			// 1	:其他
    CHAT_REPORTTOGMREASON_BILK			= 0x2,			// 2	:诈骗
    CHAT_REPORTTOGMREASON_HOSTILITY		= 0x4,			// 4	:恶意
    CHAT_REPORTTOGMREASON_BLACKMAIL		= 0x8,			// 8	:举报恶意网址

    CHAT_REPORTTOGMREASON_MAX			= 0x10000		// 最大
};

// 喇叭类型
enum EMLoudspeakerType
{
	LOUDSPEAKERTYPE_NULL = 0,
	LOUDSPEAKERTYPE_HUGE,					// 大喇叭
	LOUDSPEAKERTYPE_MAX					
};

// 权限
enum EMPrivilegeFlag
{
	PrivilegeFlag_None              = 0,
	PrivilegeFlag_Notice            = 1 << 0,	// 公告
	PrivilegeFlag_Stat_Online       = 1 << 1,	// 统计在线人数
	PrivilegeFlag_Kickout           = 1 << 2,	// 踢人
    PrivilegeFlag_HideSelf          = 1 << 3,   // 自我隐身
    PrivilegeFlag_VipViewBattle     = 1 << 4,   // VIP观战 
};

// 广播消息
struct SBroacastSysMessage
{
	int broadType;				// EMChatBroadcastType
	int broadTargetID;
	int tipID;					// EMChatTipID
	string strMsg;
	DWORD dwSendTime;

	SBroacastSysMessage()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 尚未移植的功能的定义，等功能移植后再开放
/*
// 免登入打开官方的某些链接定义
enum EMOpenUrlType
{
	OPENURLTYPE_FORUM,			// 论坛
};


// 禁言类型
enum EMChatMuteType
{
	CHAT_MUTETYPE_GENERAL,	// 普通禁言
	CHAT_MUTETYPE_COMPUTER,	// 禁言电脑
	CHAT_MUTETYPE_AUTO,		// 自动禁言
	CHAT_MUTETYPE_OFFLINE,	// 离线禁言

	CHAT_MUTETYPE_MAX		// 禁言类型最大值
};

// 禁言规则
enum EMChatMuteRule
{
	CHAT_MUTERULE_NULL = 0,					// 不做任何处理
	CHAT_MUTERULE_NORMAL,					// 提示玩家被禁言，玩家无法发送消息，这是以前的禁言处理机制
	CHAT_MUTERULE_NOPROMPT,					// 不提示玩家被禁言，玩家可以发消息，但不会广播，仅他可见，这是新增加的机制
	CHAT_MUTERULE_EXCLUDEPRIVATECHANNELS,	// 战队，队伍，私聊，帮会，团队,不受限制，其他的不许发言
	
	CHAT_MUTERULE_MAX
};

// 非文明处罚方式
enum EMChatRudeFine
{
	CHAT_RUDEFINE_CHECK,	// 查询非文明值
	CHAT_RUDEFINE_ADD,		// 添加非文明值

	CHAT_RUDEFINE_MAX
};
*/

// 公共结构体定义
// SystemMessage脚本信息结构
struct SSchemeSystemMessage
{
	int             nId;              // 飘字id
	std::string     strDesc;          // 文字描述
	int             nType;            // 消息类型
	int             nPos;             // 消息位置
	int             nShowTime;        // 显示时间
	int             nFlag;            // 使用标识 EMTEXT_USE_FLAG
	std::string		strTitle;		  // 弹窗标题（适用于弹窗消息）

	SSchemeSystemMessage()
	{
		nId = 0;
		nType = 0;
		nPos = 0;
		nShowTime = 0;
		nFlag = 0;
	}
};

// ChatCD脚本信息结构
struct SSchemeChatCD
{
	int nChannel;		// 频道ID
	int nLimitLevel;	// 等级限制
	int nCDTime;		// 冷却时间

	SSchemeChatCD()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 以下为待整理结构体,需要整理时再整理

//  玩家举报信息给GM消息数据结构
struct SMsgReportToGMInfo
{
	DWORD dwMsgType;							// 消息类型  EMChatReportToGMMsgType
	DWORD dwReasonFlag;							// 原因标识  ChatReportToGMReasonFlag
	BOOL  bIsVIP;								// 是否是VIP
	DWORD dwReporterID;							// 玩家角色ID
	DWORD dwCountryID;							// 玩家角色国家ID
	string strReporterName;						// 角色名
	DWORD dwCheaterID;							// 举报对象玩家角色ID
	string strCheaterName;						// 举报对象角色名
	string strTitle;							// 举报证据（最大长度和聊天内容长短一致）
	string strMsg;								// 附加说明
};
/*
struct SMsgChat_KickoutToPublicWorld
{
	char szKickPersonName[ACTORNAME_MAXSIZE];	// 被踢者
	char szSenderName[ACTORNAME_MAXSIZE];		// 踢人者
	char szKickReason[512];						// 踢人原因

	SMsgChat_KickoutToPublicWorld(void)
	{
		memset(this, 0, sizeof(*this));
	};
};
*/

#include "ChatMessageDef.h"

