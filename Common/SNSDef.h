/*******************************************************************
** 文件名:	SNSDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	11/24/2016
** 版  本:	1.0
** 描  述:	社交服（Social Network Service Server)模块定义	
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __SNS_DEF_H__
#define __SNS_DEF_H__

#pragma pack(1)
#pragma warning(push) // warning C4200: 使用了非标准扩展 : 结构/联合中的零大小数组
#pragma warning(disable : 4200)

#include "NameLenDef.h"
#include "SNS_ManagedDef.h"

/************************************************************************/
/* 调试信息输出宏定义                                                   */
/************************************************************************/
// 输出DEBUG信息
#ifdef VERSEION_DEBUG
#   define SNS_DEBUG_OUTPUT
#endif
#ifdef SNS_DEBUG_OUTPUT
#	define SNS_TRACELN(x)			TraceLn(x)
#	define SNS_EMPHASISLN(x)		EmphasisLn(x)
#	define SNS_ERRORLN(x)			ErrorLn(x)
#	define SNS_WARNINGLN(x)			WarningLn(x)
#else
#	define SNS_TRACELN(x)
#	define SNS_EMPHASISLN(x)
#	define SNS_ERRORLN(x)
#	define SNS_WARNINGLN(x)
#endif


#define		SNS_CHAT_MSG_MAXSIZE			512			// 聊天信息长度
#define		ACCOUNT_NAME_MAXSIZE			32			// 账号名字长度
#define		ACTOR_NAME_MAXSIZE				32			// 角色名字长度
#define		BUDDYGROUP_NAME_MAXSIZE			32			// 好友分组名称长度
#define     SERVER_NAME_MAXSIZE             32
#define     FONT_NAME_MAXSIZE				32			// 字体名字长度
#define     REMARK_NAME_MAXSIZE				32			// 备注名字长度
#define     MOOD_CONTENT_MAXSIZE			128			// 备注名字长度
#define     SNS_DEFAULT_LISTEN_PORT         8600		// SNS服务器默认监听端口
#define		STATUS_REVERSE_STR_MAXSIZE		32			// 默认字符串最大长度
#define		GAME_WORLD_NAME_MAXSIZE			32			// 游戏世界名称最大值
#define		SNS_NEARBY__MAXSIZE				15			// 附近人最大个数

/***************************************************************/
// 上层的消息码必定是 MSG_MODULEID_SNS
/***************************************************************/

///////////////////////////////////////////////////////////////////
// 消息码枚举定义: ENUM + 消息码名
enum EM_MSG_SNS
{
    MSG_SNS_INVALID = 0,
    MSG_SNS_HANDSHAKE,						// 握手消息（社会服发过来）
    MSG_SNS_HANDSHAKE_RESPONSE,				// 回应握手消息
    MSG_SNS_CONNECTED,						// 与SNS服务器建立了连接
    MSG_SNS_DISCONNECTED,					// 与SNS服务器断开连接
    MSG_SNS_KEEPALIVE,						// 心跳检查（社会服发过来）
    MSG_SNS_ANSWER_KEEPALIVE,				// 回应心跳检查
	MSG_SNS_TRANSMIT2CLIENT,				// 把消息发给社会服，通过它转发到客户端
	MSG_SNS_TRANSMIT2SNS,					// 把消息发给社会服，通过它转发到客户端
	MSG_SNS_USER_LOGIN,						// 玩家登陆
	MSG_SNS_USER_LOGOUT,					// 玩家登出

	MSG_SNS_UPDATE_BUDDY,					// 添加、删除好友（客户端发送）
	MSG_SNS_REQ_RECOMMAND_BUDDY,			// 请求推荐好友（客户端发送）
	MSG_SNS_REQ_BUDDY_INFO,					// 请求好友信息（客户端发送）
	MSG_SNS_REQ_BUDDYRELATION,				// 请求好友关系（客户端发送）
	MSG_SNS_SEND_MSG_DATA,					// 发送聊听消息（客户端发送）
	MSG_SNS_UPDATE_REMARK,					// 修改好友备注（客户端发送）
	MSG_SNS_RSP_ADD_BUDDY,					// 回应添加好友请求（客户端发送）
	MSG_SNS_UPDATE_BUDDYGROUP,				// 更新好友分组信息（客户端发送）
	MSG_SNS_CHANGE_BUDDYGROUP,				// 改变好友分组（客户端发送）
	MSG_SNS_REQ_ADD_BUDDY_BY_NAME,			// 通过名字添加好友（客户端发送）
	MSG_SNS_REQ_USERINFO,					// 请求用户信息（客户端发送）
	MSG_SNS_REQ_CHANGE_PERMISSION,			// 请求修改权限（客户端发送）
	MSG_SNS_REQ_CHANGE_MOOD,				// 请求修改心情（客户端发送）
	MSG_SNS_REQ_UPDATE_BUDDY_STATUS,		// 请求更新在线状态（客户端发送）
	MSG_SNS_REQ_INVITE_PLAYGAME,			// 请求邀请一起游戏（客户端发送）
	MSG_SNS_SEND_DATA_TO_SOCIAL,			// 客户端C#直接发数据到社会服（客户端发送）

	MSG_SNS_MSG_DATA,						// 发送聊听消息（发给客户端）
	MSG_SNS_UPDATE_RELATIONSHIP,			// 好友关系状态更新（发给客户端）
	MSG_SNS_BUDDY_STATUS,					// 好友在线状态（发给客户端）
	MSG_SNS_BUDDY_LIST,						// 所有好友信息（发给客户端）
	MSG_SNS_BUDDY_GROUP_LIST,				// 所有好友分组信息（发给客户端）
	MSG_SNS_BUDDY_RELATION_LIST,			// 所有好友关系列表（发给客户端）
	MSG_SNS_SYSTEM_MESSAGE,					// 发提示消息（发给客户端）
	MSG_SNS_BUDDY_INFO_SELF,				// 自己的角色信息（发给客户端）
	MSG_SNS_BUDDY_INFO,						// 返回好友信息（发给客户端）
	MSG_SNS_BUDDY_RELATIONSHIP,				// 返回好友关系信息（发给客户端）
	MSG_SNS_BUDDY_REMARK,					// 返回好友备注（发给客户端）
	MSG_SNS_USERINFO_ACK,					// 返回用户信息（发给客户端）
	MSG_SNS_BUDDYINFO_CHANGE_NOTIFY,		// 通知用户某个好友的信息发生改变（发给客户端）
	MSG_SNS_USERSTATUS_NOTIFY,				// 通知用户在线状态（发给客户端）
	MSG_SNS_INVITE_PLAY_GAME_NOTIFY,		// 通知邀请一起游戏（发给客户端）
	MSG_SNS_UPDATE_SCENE_NOTIFY,			// 更新玩家的场景信息(发给所有好友客户端）
	MSG_SNS_RELATION_CHANGE_NOTIFY,			// 好友关系改变通知(发给客户端）

	MSG_SNS_REQ_DIRECT_ADD_BUDDY,			// 直接添加两个玩家为好友（社会服到SNS服）
	MSG_SNS_UPDATE_SCENE,					// 更新玩家的场景信息（社会服到SNS服）
	MSG_SNS_UPDATE_ACTOR_INFO,				// 更新玩家的角色信息（社会服到SNS服）

	MSG_SNS_REQ_INVITE_MATCH_TEAM,			// 请求邀请匹配组队（客户端发送）
	MSG_SNS_INVITE_MATCH_TEAM_NOTIFY,		// 通知邀请匹配组队（发给客户端）

    MSG_SNS_MAXID,							// 最大消息码
};



// DB操作状态
enum EMSNS_DB_OPT_TYPE_UPDATE_BUDDY
{
	EMSNS_DB_STATUS_NONE,						
	EMSNS_DB_STATUS_ADD_BLACKLIST,				// 加入黑名单
	EMSNS_DB_STATUS_ADD_BUDDY,					// 添加好友
	EMSNS_DB_STATUS_DEL_BUDDY,					// 删除好友
	EMSNS_DB_STATUS_AGREE_ADDBUDDY,				// 同意添加好友
	EMSNS_DB_STATUS_REFUSE_ADDBUDDY,			// 拒绝添加好友
	EMSNS_DB_STATUS_ADD_BUDDY_DIRECT,			// 直接添加好友
};

// DB操作状态：查询好友信息
enum EMSNS_DB_OPT_TYPE_READ_BUDDYINFO
{
	EMSNS_DB_OPT_READ_BUDDYINFO_NORMAL,			// 正常查询好友信息
	EMSNS_DB_OPT_READ_BUDDYINFO_ADDBUDDY,		// 查找并添加好友
};

// DB操作状态：查询玩家信息
enum EMSNS_DB_OPT_TYPE_READ_PLAYERINFO
{
	EMSNS_DB_OPT_TYPE_READ_PLAYERINFO_NORMAL,		// 正常查询玩家信息
	EMSNS_DB_OPT_TYPE_READ_PLAYERINFO_ADDBUDDY,		// 查找并添加好友
};
enum EMSNS_GAME_ID
{
	EMSNS_GAME_ID_NONE,
	EMSNS_GAME_ID_TITAN,
};

// 角色信息更新类型
enum ESNSActorInfoUpdateType
{
	ESNSActorInfoUpdateType_Unkown,
	ESNSActorInfoUpdateType_Name,
	ESNSActorInfoUpdateType_GameState,
	ESNSActorInfoUpdateType_ClanName,
	ESNSActorInfoUpdateType_MatchRankInfo,
};

struct SSNSGameWorldInfo
{
	DWORD	dwGameID;							// 游戏ID，属于哪款游戏；如TITAN，TITAN手游等
	DWORD	dwWorldID;							// 服务器所在游戏世界ID
	WORD	nServerType;						// 服务器类型
	WORD	wSubID;								// 第n号服务器
	char	szName[SERVER_NAME_MAXSIZE];		// 服务器名


	SSNSGameWorldInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	bool operator ==(const SSNSGameWorldInfo& info) const
	{
		return info.dwGameID == dwGameID && info.dwWorldID == dwWorldID;
	}
};


// 角色信息
struct SSNSActorInfo
{
	DWORD	dwUserID;						// 账号ID
	DWORD	dwGameID;						// 游戏ID，属于哪个游戏
	DWORD	dwWorldID;						// 世界ID
	DWORD	dwServerID;						// 场景服ID
	DWORD	dwSceneID;						// 场景ID
	DWORD	dwPdbID;						// pdbid，玩家所在服的角色id
	DWORD	dwHeadID;						// 头像ID
	char	szName[ACTOR_NAME_MAXSIZE];		// 角色名字
	int		nLevel;							// 角色等级
	DWORD	dwLastLogout;					// 最后下线时间
	int		nMatchNum;						// 总场数
	int		nWinNum;						// 胜场数
	char	szClan[CLAN_NAME_SIZE];			// 联盟名称
	char	szTitle[TITLE_NAME_SIZE];		// 称号
    int     nRankGrade;                      // 排位等级
	int		nRankScore;						// 排位分
	char	szRankName[RANK_NAME_MAXSIZE];	// 段位描述
	BYTE    nSex;									// 性别 0男 1女 2不区分
	char    szWorldName[GAME_WORLD_NAME_MAXSIZE];	// 世界名字
	DWORD	dwUpdateTime;							// 角色信息更新时间

	SSNSActorInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 好友关系
struct SSNSRelationship
{
	DWORD	dwUserID1;									// 用户1的账号id
	DWORD	dwUserID2;									// 用户2的账号id
	BYTE	nStatus;									// 关系状态
	DWORD	dwBuddyGroupID;								// 好友分组ID
	BYTE	nIntimacy;									// 亲密度
	char	szRemark[REMARK_NAME_MAXSIZE];				// 用户2在用户1中的备注名
	DWORD	nUpdated;									// 更新时间
	
	SSNSRelationship()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 用户信息
struct SSNSUserInfo
{
	DWORD dwUserID;							// 用户ID
	int nPermission;						// 权限
	char szMood[MOOD_CONTENT_MAXSIZE];		// 心情
	DWORD dwUpdateTime;						// 最后更新时间

	SSNSUserInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 好友在线状态信息
struct SSNSBuddyStatusInfo
{
	BYTE nStatus;							// 状态：离线、游戏中、撮合中
	char szReverse[STATUS_REVERSE_STR_MAXSIZE];	// 保留字符串

	SSNSBuddyStatusInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 好友信息
struct SSNSBuddyInfo
{
	SSNSBuddyStatusInfo statusInfo;
	SSNSActorInfo actorInfo;			// 角色信息
	SSNSUserInfo userInfo;				// 用户信息

	SSNSBuddyInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SSNSBuddyGroupInfo
{
	BYTE nRelationStatus;						// 好友分组关系状态，EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP
	DWORD dwBuddyGroupID;						// 好友分组ID
	char szName[BUDDYGROUP_NAME_MAXSIZE];		// 好友分组名称

	SSNSBuddyGroupInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SSNSBuddyRemark
{
	DWORD	dwUserID1;							// 玩家1ID
	DWORD	dwUserID2;							// 玩家2ID
	char	szRemark[REMARK_NAME_MAXSIZE];		// 玩家2的备注名

	SSNSBuddyRemark()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 游戏世界发给SNS服务器时候携带的头部信息
struct SONMsgSNSHead
{
	DWORD dwGameID;			// 游戏ID
	DWORD dwGameWorldID;	// 世界ID
	DWORD dwUserID;			// 账号ID

	SONMsgSNSHead()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SNS服务器向游戏世界发数据时候带的头部信息
struct SNOMsgSNSHead
{
	DWORD dwUserID;		// 账号ID
	DWORD dwPdbID;			// 角色ID
	SNOMsgSNSHead()
	{
		memset(this, 0, sizeof(*this));
	}
};

//  SNS服务器发给客户端消息时候携带的头部信息（通过社会服转发）
struct SNCMsgSNSHead
{
	DWORD dwSNSMsgID;
	SNCMsgSNSHead()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////游戏世界发给SNS服务器///////////////////////////
// MSG_SNS_HANDSHAKE		            连接器与服务器握手消息数据结构
struct SMsgSNSHandshake
{
	DWORD	dwGameID;							// 游戏ID，属于哪个游戏
    DWORD	dwWorldID;							// 服务器所在游戏世界ID
    WORD	nServerType;						// 服务器类型
    WORD	wSubID;								// 第n号服务器
    char	szName[SERVER_NAME_MAXSIZE];		// 服务器名

    SMsgSNSHandshake(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// MSG_SNS_USER_LOGIN		登陆消息
struct SMsgSNSUserLogin
{
	SSNSGameWorldInfo gameWorldInfo;			// 游戏世界信息
	SSNSActorInfo actorInfo;					// 角色信息
	char clientIP[16];								// 客户端IP

    SMsgSNSUserLogin(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// MSG_SNS_UPDATE_BUDDY	更新好友操作
struct SMsgSNSUpdateBuddy
{
	BYTE nOptType;									// 操作类型，EMSNS_BUDDY_UPDATE_TYPE
	DWORD dwOtherAccountID;							// 被添加好友的账号ID
	char szActorName[ACTOR_NAME_MAXSIZE];			// 角色名

	SMsgSNSUpdateBuddy(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_RSP_ADD_BUDDY 回应添加好友请求
struct SMsgSNSRspAddBuddy
{
	DWORD dwOtherAccountID;							// 被添加好友ID
	BYTE nFlag;										// 是否同意：EMSNS_RSP_ADDBUDDY_TYPEE_AGGRE
	char szActorName[ACTOR_NAME_MAXSIZE];			// 角色名

	SMsgSNSRspAddBuddy()
	{
		memset(this, 0, sizeof(*this));
	}
};


// ON_MSG_SNS_USER_LOGOUT 注销
struct SONMsgSNSLogout
{
	SSNSActorInfo actorInfo;					// 角色信息

	SONMsgSNSLogout()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_UPDATE_BUDDYGROUP 更新好友分组信息
struct SMsgSNSUpdateBuddyGroup
{
	BYTE	nOptType;			// 操作类型，EMSNS_BUDDYGROUP_UPDATE_TYPE
	DWORD	dwBuddyGroupID;		// 分组ID
	char	szName[32];			// 新的分组名字
	SMsgSNSUpdateBuddyGroup()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_RECOMMAND_BUDDY 请求推荐好友
struct SMsgSNSReqRecommandBuddy
{
	
	SMsgSNSReqRecommandBuddy()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_UPDATE_REMARK 修改好友备注
struct SMsgSNSUpdateRemark
{
	DWORD dwUserID;			// 好友账号ID
	char szActorName[32];	// 备注名
	char szName[32];		// 备注名
	SMsgSNSUpdateRemark()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_BUDDY_INFO // 获取好友信息
struct SMsgSNSReqBuddyInfo
{
	DWORD dwUserID;		// 好友账号ID
	SMsgSNSReqBuddyInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_BUDDYRELATION 获取好友关系
struct SMsgSNSReqBuddyRelation
{
	DWORD dwUserID;		// 好友账号ID
	
	SMsgSNSReqBuddyRelation()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_CHANGE_BUDDYGROUP    改变好友分组
struct SMsgSNSReqChangeBuddyGroup	
{
	DWORD dwUserID;				// 好友用户ID
	DWORD dwBuddyGroupID;		// 新的好友分组ID

	SMsgSNSReqChangeBuddyGroup()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_ADD_BUDDY_BY_NAME  通过名字添加好友
struct SMsgSNSReqAddBuddyByName
{
	char szActorName[32];		// 角色名

	SMsgSNSReqAddBuddyByName()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_USERINFO 请求用户信息
struct SMsgSNSReqUserInfo
{
	DWORD dwUserID;			// 用户ID

	SMsgSNSReqUserInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_CHANGE_PERMISSION 请求修改权限
struct SMsgSNSReqChangePermission
{
	int nPermission;		// 权限

	SMsgSNSReqChangePermission()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_CHANGE_MOOD 请求修改心情
struct SMsgSNSReqChangeMood
{
	char szMood[MOOD_CONTENT_MAXSIZE];	// 心情

	SMsgSNSReqChangeMood()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_UPDATE_BUDDY_STATUS 请求更新在线状态
struct SMsgSNSReqUpdateBuddyStatus
{
	BYTE nStatus;									// 好友状态（在线、离线、游戏中等等）
	char szReverse[STATUS_REVERSE_STR_MAXSIZE];			// 保留字段

	SMsgSNSReqUpdateBuddyStatus()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_INVITE_PLAYGAME 请求邀请好友一起游戏
struct SMsgSNSReqInvitePlayGame
{
	int nUserID;
	int nRoomID;
	char szInviterName[ACTORNAME_MAXSIZE];

	SMsgSNSReqInvitePlayGame()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_INVITE_PLAYGAME 请求邀请好友一起游戏
struct SMsgSNSReqInviteMatchTeam
{
	int nUserID;
	char szInviterName[ACTORNAME_MAXSIZE];

	SMsgSNSReqInviteMatchTeam()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_SEND_DATA_TO_SOCIAL C#直接发数据到社会服
struct SMsgSNSSendDataToSocial
{
	int nMsgType;

	// data...

	SMsgSNSSendDataToSocial()
	{
		memset(this, 0, sizeof(*this));
	}
};
///////////////////////////SNS服务器发给游戏世界/////////////////////////////////////////////
// MSG_SNS_HANDSHAKE_RESPONSE		 服务器回应握手消息数据结构
struct SMsgSNSHandshakeResponse	
{
	DWORD	dwID;								// 连接ID
	DWORD	dwServerTime;						// 服务器时间

	SMsgSNSHandshakeResponse(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_MSG_DATA		聊听消息
struct SMsgSNSMsgData
{
	DWORD dwMsgID;					// 消息ID（服务器填写）
	DWORD dwTime;					// 发送时间（服务器填写）
	DWORD dwFromAccountID;			// 发送者账号ID
	char szFromName[32];			// 发送者名字（服务器填写）
	DWORD dwToAccountID;			// 目标账号ID
	char szFont[FONT_NAME_MAXSIZE];	// 字体
	int nFontSize;					// 字体大小
	int nFontColor;					// 字体颜色
	int nContentLen;				// 聊听内容大小

	SMsgSNSMsgData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_RELATIONSHIP_STATUS	// 更新好友关系状态
struct SMsgSNSUpdateRelationship
{
	DWORD	dwUserID;		// 玩家ID
	BYTE	nStatus;			// 关系状态
	char	szActorName[32];		// 玩家的备注名

	SMsgSNSUpdateRelationship(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_BUDDY_STATUS		// 好友在线状态
struct SMsgSNSBuddyStatus
{
	DWORD dwUserID;							// 好友账号ID
	BYTE nStatus;							// 好友状态（在线、离线、游戏中等等）
	char szReverse[STATUS_REVERSE_STR_MAXSIZE];	// 好友状态保留字符串
	DWORD dwLastLogoutTime;					// 最后一次下线时间

	SMsgSNSBuddyStatus()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_SYSTEM_MESSAGE  // 发提示消息给客户端
struct SMsgSNSSystemMsg
{
	int nTipID;				// 提示ID
	int nLen;				// 提示消息长度
	
	SMsgSNSSystemMsg()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_BUDDY_INFO_LIST
struct SMsgSNSBuddyList
{
	int nBuddyCount;		// 好友个数
	
	// 后面有 nBuddyCount * SSNSBuddyInfo

	SMsgSNSBuddyList()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_BUDDY_GROUP_LIST
struct SMsgSNSBuddyGroupList
{
	int nBuddyGroupCount;		// 好友分组个数

	// 后面有 nBuddyGroupCount * SSNSBuddyGroupInfo
	

	SMsgSNSBuddyGroupList()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_BUDDY_RELATION_LIST
struct SMsgSNSBuddyRelationList
{
	int nBuddyRelationCount;		// 好友分组个数

	// 后面有 nBuddyRelationCount * SSNSRelationship

	SMsgSNSBuddyRelationList()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_BUDDY_INFO
struct SMsgSNSBuddyInfo
{
	SSNSBuddyInfo buddyInfo;		// 好友信息

	SMsgSNSBuddyInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_BUDDY_RELATIONSHIP
struct SMsgSNSBuddyRelationship
{
	SSNSRelationship buddyRelation;
};

// MSG_SNS_BUDDY_REMARK
struct SMsgSNSBuddyRemark
{
	SSNSBuddyRemark remark;
};

// MSG_SNS_BUDDY_INFO_SELF
struct SMsgSNSBuddyInfoSelf
{
	SSNSBuddyInfo buddyInfo;		// 自己的信息

	SMsgSNSBuddyInfoSelf()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_USERINFO_ACK
struct SMsgSNSUserInfo_Ack
{
	SSNSUserInfo userInfo;
	SMsgSNSUserInfo_Ack()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_ADD_BUDDY
struct SONMsgSNSReqDirectAddBudy
{
	DWORD dwUserID1;
	DWORD dwUserID2;
	char szActorName1[ACTOR_NAME_MAXSIZE];
	char szActorName2[ACTOR_NAME_MAXSIZE];

	SONMsgSNSReqDirectAddBudy()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_BUDDYINFO_CHANGE_NOTIFY
struct SMsgSNSBuddyInfoChange_Notify
{
	DWORD dwBuddyUserID;		// 好友的用户ID
	int type;					// 好友信息更新类型

	SMsgSNSBuddyInfoChange_Notify()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_USERSTATUS_NOTIFY
struct SMsgSNSUserStatus_Notify
{
	DWORD dwBuddyUserID;		// 好友的用户ID
	int nStatus;				// 好友在线状态

	SMsgSNSUserStatus_Notify()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_INVITE_PLAY_GAME_NOTIFY
struct SMsgSNSInvitePlayGame_Notify
{
	int nRoomID;
    DWORD dwInviteActorID;
	char szInviterName[ACTORNAME_MAXSIZE];

	SMsgSNSInvitePlayGame_Notify()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_INVITE_MATCH_TEAM_NOTIFY,		// 通知邀请匹配组队（发给客户端）
struct SMsgSNSInviteMatchTeam_Notify
{
	DWORD dwTargetActorID;

	SMsgSNSInviteMatchTeam_Notify()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_UPDATE_SCENE			玩家切换了场景
struct SMsgSNSUpdateScene
{
	DWORD dwUserID;
	DWORD dwWorldID;
	DWORD dwSceneID;

	SMsgSNSUpdateScene()
	{
		memset(this, 0, sizeof(*this));
	}
};


// MSG_SNS_UPDATE_ACTOR_INFO  玩家角色信息发生改变
struct SMsgSNSUpdateActorInfo
{
	int nUpdateReson;		// 更新原因(ESNSActorInfoUpdateType)
	// buff...

	SMsgSNSUpdateActorInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_RELATION_CHANGE_NOTIFY 好友关系改变通知
struct SMsgSNSRelationChangeNotify
{
	int nRelation;
	DWORD	dwUserID1;		// 玩家1ID
	DWORD	dwUserID2;		// 玩家2ID
	BYTE	nStatus;			// 关系状态
	char	szRemark[32];		// 玩家2的备注名
};
///////////////////////////////////////////////////////////////////////////

#pragma warning(pop)
#pragma pack()

#endif //__SNS_DEF_H__
