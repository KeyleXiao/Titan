/*******************************************************************
** 文件名:	E:\speed\Common\TeamDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/15/2015	15：50
** 版  本:	1.0
** 描  述:	组队相关定义
         
********************************************************************/

#pragma once
#include "PackagePtr.h"
#include "DMirrorServer.h"
#include "NameLenDef.h"

// 输出DEBUG信息
#ifdef VERSEION_DEBUG
#   define TEAM_DEBUG_OUTPUT
#endif
#ifdef TEAM_DEBUG_OUTPUT
#	define	TEAM_TRACELN(x)			TraceLn(x)
#	define	TEAM_WARNINGLN(x)			WarningLn(x)
#	define	TEAM_ERRORLN(x)			ErrorLn(x)
#	define	TEAM_EMPHASISLN(x)			EmphasisLn(x)
#else
#	define	TEAM_TRACELN(x)
#	define	TEAM_WARNINGLN(x)
#	define	TEAM_ERRORLN(x)
#	define	TEAM_EMPHASISLN(x)
#endif

// 打印调试信息,并设为测试模式
//#define TEAM_PRINT_DEBUG_INF



// 组队同步队友位置的定时器间隔
#define TEAM_POSTPLACETIMER_INTERVAL			5000

// tolua_begin

// 组队分成的最大有效距离
#define TEAM_DROPPRIZE_VALIDDISTANCE			128

// 如果创建后，队伍不足两人，则会解散队伍
#define TEAMCREATE_ENDLIFE_INTERVAL				300000

#pragma pack(1)



/******************************************************************/
/////////////////// 团的公共结构体定义 ////////////////////////////
/******************************************************************/


// 团的公用定义 ( 打头: GROUP )

#define GROUP_MAX_TEAM_COUNTS			100			// 团最大容量数
#define GROUP_MAX_GROUP_INDEX			8			// 最大分组序号
#define GROUP_SELF_TEAM_INDEX			9			// 自己队的序号
#define GROUP_CUSTOM_TEAM_INDEX			10			// 自定义队的序号
#define GROUP_MEMBER_TEAM_INDEX			11			// 自定义队员的序号
#define GROUP_ALL_TEAM_INDEX			0			// 所有队的序号
#define GROUP_REQUEST_TIMEOUT			60000		// 如果超过 60000 毫秒，也认为没有重复申请，防止程序bug
#define GROUP_COMMAND_MAX_ID			255			// 命令最大ID
#define GROUP_COMMANDLIST_MAX			8			// 命令列表最多保留个数
#define GROUP_EFFECTLIST_MAX			8			// 显示列表最多保留个数
#define GROUP_SENDLIST_MAXNUM			1024		// 所有接收命令玩家列表数组最大数
#define GROUP_CMD_COOL_TIMES			1000		// 使用命令的冷却 1000 毫秒
#define GROUP_REQUEST_COOL_TIMES		5000		// 请求冷却 5000 毫秒
#define GROUP_CHECK_LIFETIME			5000		// 查命令效果过期 5000 毫秒
#define GROUP_MAX_CMDLIFETIMES			1800		// 命令最长过期 1800 秒,30分钟,过期为0时用
#define GROUP_TARGET_RESID				1709		// 位置实体视图对象资源ID
#define GROUP_TARGETFLAG_COUNT			10			// 位置实体视图对象个数
#define GROUP_FOLLOW_CHECK_TIMES		1000		// 跟随定时器 1000 毫秒
#define GROUP_DPSSEND_COOL_TIMES		10000		// 发送DPS数据冷却 10000 毫秒
#define GROUP_DEFAUT_ARROW_CMDID		13			// 默认箭头命令ID

// 称号特效类型
#define GROUP_TEAMCAP_TEFFECTID			6			// 队长图标,称号特效类型ID
#define GROUP_TEAMOTHERCAP_TEFFECTID	40			// 其它队长图标,称号特效类型ID
#define GROUP_TEAMGROUPCAP_TEFFECTID	41			// 团中其它队长图标,称号特效类型ID
#define GROUP_GROUPTEAMCAP_TEFFECTID	7			// 团长队长图标,称号特效类型ID
#define GROUP_GROUPCAP_TEFFECTID		7			// 团长图标,称号特效类型ID
#define GROUP_TEAMER_TEFFECTID			8			// 队伍成员图标,称号特效类型ID
#define GROUP_GROUPER_TEFFECTID			9			// 团成员图标,称号特效类型ID
#define GROUP_NOTEAMER_TEFFECTID		21			// 不能组队图标,称号特效类型ID
#define GROUP_KILLCOOL_TEFFECTID		22			// 杀人冷却,称号特效类型ID
#define GROUP_PROTECT1_TEFFECTID		23			// 低级保护,称号特效类型ID
#define GROUP_PROTECT2_TEFFECTID		24			// 高级警戒,称号特效类型ID
#define GROUP_SPEAKING_TEFFECTID		42			// 正在语音,称号特效类型ID


#define GROUP_FOLLOW_EFFECTID			4005		// 跟随光效特效类型ID





// 无效的队伍ID
#define INVALID_TEAM_ID						0

// 无效的团ID
#define INVALID_GROUP_ID					0

// 队伍成员最多个数
#define TEAM_MEMBER_MAXCOUNT                5

#define TEAM_MAX_ITEM                       200

// 运行参数信息定义,为int数组,0表示无,1表示有
enum EMGroupConfigType
{
    // 类型分类ID定义规则（GROUP_CONFIG + 功能涵义）
    GROUP_CONFIG_INVALID = 0,			// 无效ID
    GROUP_CONFIG_POPUP,					// 在收到命令时是否弹出
    GROUP_CONFIG_AUTOMOVE,				// 在收到命令时是自动走向目标位置
    GROUP_CONFIG_EFFECT,				// 显示命令效果
    GROUP_CONFIG_LEFTMENU,				// 队伍队员头像命令菜单
    GROUP_CONFIG_MAXID,					// 最大分类ID
};

// 团数据分类
enum EMGroupRequestDataType
{
    // 团数据分类ID定义规则（GROUP_REQUEST + 功能涵义）
    GROUP_REQUESTDATA_BASE = 0,		// 团基本信息
    GROUP_REQUESTDATA_TEAM,			// 团的成员队伍信息
    GROUP_REQUESTDATA_TEAMMEMBER,	// 团中队伍和队员信息
    GROUP_REQUESTDATA_GROUP,		// 团的组信息
    GROUP_REQUESTDATA_TOTAL,		// 团的统计信息
    GROUP_REQUESTDATA_TEAMTOTAL,	// 队伍的统计信息
    GROUP_REQUESTDATA_MAXID,		// 最大分类ID
};


// 权限标识定义
enum EMGroupCommandRightFlag
{
    // 权限标识ID定义规则（GROUP_RIGHT + 功能涵义）
    GROUP_RIGHT_NONE		= 0x0,			// 0	:无
    GROUP_RIGHT_MEMBER		= 0x1,			// 1	:成员有权限
    GROUP_RIGHT_TEAM		= 0x2,			// 2	:队长有权限
    GROUP_RIGHT_GROUP		= 0x4,			// 4	:团长有权限
};


// 发送者类型分类
enum EMGroupSenderType
{
    // 发送者类型分类ID定义规则（GROUP_SENDTYPE + 功能涵义）
    GROUP_SENDTYPE_INVALID = 0,		// 无效ID
    GROUP_SENDTYPE_MEMBER,			// 成员
    GROUP_SENDTYPE_TEAM,			// 队长
    GROUP_SENDTYPE_GROUP,			// 团长
    GROUP_SENDTYPE_MAXID,			// 最大目标分类ID
};


// 玩家流程请求目标分类
enum EMGroupRequestType
{
    // 玩家请求目标分类ID定义规则（GROUP_REQUEST + 功能涵义）
    GROUP_REQUEST_PERSON = 0,		// 玩家ID
    GROUP_REQUEST_GROUP,			// 团ID
    GROUP_REQUEST_TEAM,				// 队伍ID
    GROUP_REQUEST_MAXID,			// 最大目标分类ID
};

// 选择类型,目标类型,0.不选目标与目标点,1.选目标,2.选目标点,3.目标为自已
enum EMGroupSelectType
{
    // 选择类型ID定义规则（GROUP_SELECT + 功能涵义）
    GROUP_SELECT_INVALID = -1,		// 无效ID
    GROUP_SELECT_NO = 0,			// 不选目标与目标点
    GROUP_SELECT_ENTITY,			// 选择实体
    GROUP_SELECT_TILE,				// 选目标点TILE
    GROUP_SELECT_MYSELF,			// 自己
    GROUP_SELECT_MAXID,				// 最大目标分类ID
};

// 发送统计处理类型
enum EMGroupSendTotalType
{
    // 发送统计处理类型ID定义规则（GROUP_SENDTOTAL + 功能涵义） 类型,0,杀怪,1:杀人,2:杀载具
    GROUP_SENDTOTAL_MONSTER = 0,	// 杀怪
    GROUP_SENDTOTAL_PERSON,			// 杀人
    GROUP_SENDTOTAL_TANK,			// 杀载具
    GROUP_SENDTOTAL_DAMAGE,			// 总伤害量
    GROUP_SENDTOTAL_HDAMAGE,		// 最高伤害
    GROUP_SENDTOTAL_CURE,			// 总治疗量
    GROUP_SENDTOTAL_HCURE,			// 最高治疗
    GROUP_SENDTOTAL_MAXID,			// 最大方式ID
};

// 玩家统计数据类型
enum EMGroupTotalType
{
    // 玩家统计数据类型ID定义规则（GROUP_TOTAL + 功能涵义）
    GROUP_TOTAL_DEAD = 0,		// 死亡数
    GROUP_TOTAL_PERSON,			// 杀人数
    GROUP_TOTAL_MONSTER,		// 杀怪数
    GROUP_TOTAL_DAMAGE,			// 总伤害量
    GROUP_TOTAL_HDAMAGE,		// 最高伤害
    GROUP_TOTAL_CURE,			// 总治疗量
    GROUP_TOTAL_HCURE,			// 最高治疗
    GROUP_TOTAL_MAXID,			// 最大方式ID
};

// 统计位标识定义
enum EMGroupTotalInfoFlag
{
    // 统计位标识定义规则（GROUP_TOTALFLAG + 功能涵义）
    GROUP_TOTALFLAG_NONE		= 0x0,			// 0	:无
    GROUP_TOTALFLAG_ONLINE		= 0x1,			// 1	:在线
    GROUP_TOTALFLAG_INGROUP		= 0x2,			// 2	:在团中
    //GROUP_TOTALFLAG_GROUP		= 0x4,			// 4	:
    //GROUP_TOTALFLAG_GROUP		= 0x8,			// 8	:
};

// 统计位标识默认
#define GROUP_TOTALFLAG_DEFAULTFLAG		3

// 请求服务器团成员位置信息标识
enum EMGroupRequestMapLocType
{
    // 位标识定义规则（GROUP_MAPLOCTYPE + 功能涵义）
    GROUP_MAPLOCTYPE_NONE			= 0x0,			// 0	:无
    GROUP_MAPLOCTYPE_GROUP			= 0x1,			// 1	:本团成员,不包含本队
    GROUP_MAPLOCTYPE_MYTEMA			= 0x2,			// 2	:本队成员
    GROUP_MAPLOCTYPE_ALLMAP			= 0x4,			// 4	:本服所有地图,否则只是本地图
    GROUP_MAPLOCTYPE_MYNATION		= 0x8,			// 8	:本国所有人物
    GROUP_MAPLOCTYPE_FRIENDNATION	= 0x10,			// 16	:盟国所有人物
    GROUP_MAPLOCTYPE_ENEMYNATION	= 0x20,			// 32	:敌国所有人物
    //GROUP_MAPLOCTYPE_UNUSE3		= 0x40,			// 64	:未用3
};




// 跟随打断条件标志,位标识,多种条件可相加
enum EMGroupFollowBreakFlag
{
    // 跟随打断条件标志定义规则（FOLLOWBREAKFLAG + 功能涵义）
    FOLLOWBREAKFLAG_NONE			= 0x0,			// 0	:无
    FOLLOWBREAKFLAG_USESKILL		= 0x1,			// 1	:使用技能
    FOLLOWBREAKFLAG_DIE				= 0x2,			// 2	:死亡
    FOLLOWBREAKFLAG_RIDE			= 0x4,			// 4	:上马
    FOLLOWBREAKFLAG_DRIVER			= 0x8,			// 8	:上载具
    FOLLOWBREAKFLAG_STARTMOVE		= 0x10,			// 16	:行走
    FOLLOWBREAKFLAG_CHANGELOC		= 0x20,			// 32	:传送
    FOLLOWBREAKFLAG_ADDBUFF			= 0x40,			// 64	:附件特殊状态，比如眩晕
    FOLLOWBREAKFLAG_RIDESTEED		= 0x80,			// 128	:上战骑
    //	FOLLOWBREAKFLAG_NOUSE3			= 0x100,		// 256	:未用
    //	FOLLOWBREAKFLAG_NOUSE4			= 0x200,		// 512	:未用
    //	FOLLOWBREAKFLAG_NOUSE5			= 0x400,		// 1024	:未用
    //	FOLLOWBREAKFLAG_NOUSE6			= 0x800,		// 2048	:未用
    //	FOLLOWBREAKFLAG_NOUSE7			= 0x1000,		// 4096	:未用
    //	FOLLOWBREAKFLAG_NOUSE8			= 0x2000,		// 8192	:未用
    //	FOLLOWBREAKFLAG_NOUSE9			= 0x4000,		// 16384:未用
    //	FOLLOWBREAKFLAG_NOUSE10			= 0x8000,		// 32768:未用
    //	FOLLOWBREAKFLAG_NOUSE11			= 0x10000,		// 65536:未用

};

// 跟随打断原因标志
enum EMGroupFollowBreakReason
{
    // 跟随打断原因定义规则（FOLLOWBREAKREASON + 功能涵义）
    FOLLOWBREAKREASON_CANCEL	= 0,	// 玩家取消
    FOLLOWBREAKREASON_USESKILL,			// 使用技能
    FOLLOWBREAKREASON_DIE,				// 死亡
    FOLLOWBREAKREASON_RIDE,				// 上马
    FOLLOWBREAKREASON_DRIVER,			// 上载具
    FOLLOWBREAKREASON_STARTMOVE,		// 行走
    FOLLOWBREAKREASON_CHANGELOC,		// 传送
    FOLLOWBREAKREASON_ADDBUFF,			// 受异常的状态影响，如打晕、惊吓、冻结
    FOLLOWBREAKREASON_LOGOUT,			// 玩家下线
    FOLLOWBREAKREASON_RIDESTEED,		// 上战骑
    FOLLOWBREAKREASON_OTHER,			// 其他
};



enum
{
    // 自已无队伍，对方无队伍的申请加入
    EJoinFlowType_Request_One = 0,

    // 自已无队伍，对方有队伍的申请加入
    EJoinFlowType_Request_Two,

    // 自已无队伍，对方无队伍的邀请加入
    EJoinFlowType_Invited_Three,

    // 自已有队伍，对方无队伍的邀请加入
    EJoinFlowType_Invited_Four
};

enum
{
    // 离开队伍原因：主动退出队伍
    ELeaveTeamReason_Leave = 0,

    // 离开队伍原因：被队长开除
    ELeaveTeamReason_Kick,

    // 离开队伍原因：主动下线
    ELeaveTeamReason_Logout,

    // 离开队伍原因：队伍解散
    ELeaveTeamReason_Disband,

    // 离开队伍原因：队友跨服去了
    ELeaveTeamReason_StarTrek,

    // 离开队伍原因：系统强退
    ELeaveTeamReason_SysKick,
};

enum
{
    // 队长转移原因：队伍创立之初
    ECaptainShiftReason_Init = 0,

    // 队长转移原因：主动退出队伍
    ECaptainShiftReason_Leave,

    // 队长转移原因：主动下线
    ECaptainShiftReason_Logout,

    // 队长转移原因：被任命
    ECaptainShiftReason_Appoint,

    // 队长转移原因：掉线退出
    ECaptainShiftReason_OffLine,
};



enum
{
    tTeamSetting_Flag_ForbidKick = 0x0001,		// 禁止踢人
    tTeamSetting_Flag_ForbidCaptain = 0x0002,	// 禁用队长拾取
    tTeamSetting_Flag_ForbidFree = 0x0004,		// 禁用自由拾取
    tTeamSetting_Flag_ForbidTurn = 0x0008,		// 禁用轮流拾取	
};

enum EMGroupConveneCostType
{
    GroupConveneCostType_None = 0,		// 无效
    GroupConveneCostType_1,				// 禁止踢人
    GroupConveneCostType_2,				// 禁用队长拾取
    GroupConveneCostType_3,				// 禁用自由拾取
    GroupConveneCostType_Max,			// 禁用轮流拾取	
};

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// 发送团事件到客户端事件类型
enum EMGroupEventType
{
    // 事件类型ID定义规则（GROUP_EVENT + 功能涵义）
    GROUP_EVENT_INVALID = 0,	// 无效方式ID
    GROUP_EVENT_UPDATE,			// 更新团基本数据
    GROUP_EVENT_UPDATETEAM,		// 更新队伍基本数据
    GROUP_EVENT_TEAMINFO,		// 更新团队伍信息数据
    GROUP_EVENT_TEAMMEMBERINFO,	// 更新团中队伍和队员信息数据
    GROUP_EVENT_GROUPINFO,		// 更新团的组信息数据
    GROUP_EVENT_TOTALINFO,		// 更新团统计信息数据
    GROUP_EVENT_TEAMTOTAL,		// 更新队统计信息数据
    GROUP_EVENT_ENTER,			// 加入团
    GROUP_EVENT_EXIT,			// 退出团
    GROUP_EVENT_CAPTAIN,		// 任团长
    GROUP_EVENT_CONFIRM,		// 要求确认
    GROUP_EVENT_CMD,			// 收到命令
    GROUP_EVENT_ADDDIDA,		// 发送到客户端添加Dida
    GROUP_EVENT_CONVENERESPONSE,// 所有的召集信息

    // 发给场景服的 
    GROUP_EVENT_SERVER,			// 发给场景服的
    GROUP_EVENT_ADD,			// 加入组
    GROUP_EVENT_REMOVE,			// 组移除
    GROUP_EVENT_SENDINFO,		// 更新数据
    GROUP_EVENT_CHANGECAPTAIN,	// 更换团长
    GROUP_EVENT_INITGROUP,		// 初始化
    GROUP_EVENT_UPDATESVR,		// 更新团基本数据
    GROUP_EVENT_CLEARTOTAL,		// 清除统计数据
    GROUP_EVENT_CHANGEPICKMODE,	// 修改拾取模式
    GROUP_EVNET_COSTMONEY,		// 扣钱事件

    GROUP_EVENT_MAXID,			// 最大方式ID
};


// 客户端发送团事件到服务器事件类型
enum EMGroupClientEventType
{
    // 事件类型ID定义规则（GROUP_CLIENT + 功能涵义）
    GROUP_CLIENT_INVALID = 0,	// 无效方式ID
    GROUP_CLIENT_ENTER,			// 请求加入指定团
    GROUP_CLIENT_EXIT,			// 请求退出当前团
    GROUP_CLIENT_ENTER1,		// 邀请进入指定团
    GROUP_CLIENT_EXIT1,			// 邀请退出当前团
    GROUP_CLIENT_ADD,			// 加入小队到指定组
    GROUP_CLIENT_REMOVE,		// 把小队从指定组移除
    GROUP_CLIENT_CON_ENTER,		// 团长确认请求进入
    GROUP_CLIENT_CON_ENTER1,	// 队长确认邀请进入
    GROUP_CLIENT_CHANGECAPTAIN,	// 更换团长
    GROUP_CLIENT_CMD,			// 使用指定命令
    GROUP_CLIENT_KILLSELF,		// 解散团
    GROUP_CLIENT_REQUESTDATA,	// 请求数据
    GROUP_CLIENT_SETTOTAL,		// 设定统计参数
    GROUP_CLIENT_CHANGETEAM,	// 更换队伍
    GROUP_CLIENT_CHANGEPICKMODE,// 更换拾取模式
    GROUP_CLIENT_SETLEVELLIST,	// 设置等级限制
    GROUP_CLIENT_STARTCONVENE,	// 开始团召集
    GROUP_CLIENT_PERSONENTER,	// 个人响应团召集
    GROUP_CLIENT_CONVENEQUERY,	// 获取召集信息
    GROUP_CLIENT_CHANGEACTMODE,	// 更换入团模式
    GROUP_CLIENT_MAXID,			// 最大方式ID
};


// 场景服发送团事件类型
enum EMGroupSvrEventType
{
    // 事件类型ID定义规则（GROUP_SVR + 功能涵义）
    GROUP_SVR_INVALID = 0,	// 无效方式ID
    GROUP_SVR_ADD,			// 加入组
    GROUP_SVR_CONVENE,		// 场景服发送到社会服团召集
    GROUP_SVR_MAXID,		// 最大方式ID
};

// tolua_begin

// 团拾取模式
enum EMGroupPickMode
{
    GROUP_PICKMODE_CAPTION = 0,		// 团长拾取
    GROUP_PICKMODE_DICE,			// 团队分配
    GROUP_PICKMODE_AUCTION,			// 金团竞拍
};

// 入团模式
enum EMGroupActionMode
{
    GROUP_ENTER_GROUP = 0,		// 允许进团
    GROUP_BAN_GROUP,			// 禁止入团
    GROUP_ACTION_MAX,			// 最大值
};


// 团队召集查询类型
enum EMGroupConvenQueryType
{
    GROUP_CONVENE_INVALID,			// 无效的召集查询类型
    GROUP_CONVENE_RESPONSE_INFO,	// 所有的团队召集信息
    GROUP_CONVENE_SHOW_INFO,		// 查询国家召集信心
    GROUP_CONVENE_MAX,				// 最大查询类型
};



// 公共结构体定义




/******************************************************************/
///////////////////// 组队的公共结构体定义 /////////////////////////
/******************************************************************/
////////////////////////////////////////////////////////////////////

struct STeamMemberInfo
{
    DWORD		clientID;					// 客户端

    DWORD		dwPDBID;					// PDBID
    char		szName[GAME_NAME_MAXSIZE];	// 名字	
    int			nFaceID;					// 头像ID
    int			nLevel;						// 当前等级

    int			nSex;						// 性别
    int			nVocation;					// 职业
    int			nFromGameWorldID;			// 所属游戏世界ID
    int         nServerID;                  // 服务器ID

    DWORD		nSceneID;					// 场景ID
    int			nMapID;						// 地图ID

    bool		bCaptainFlag;				// 是否队长
    int			nState;						// 状态(0:掉线；1:在线)

    bool		bDieState;					// 是否死亡状态

    POINT		ptTile;						// 当前位置
    POINT		ptMoveTarget;				// 移动目标点

    LONGLONG	uid;						// uid	

    int			nIndex;						// 队友序号

    int			nBattle;					// 战力值

    bool		bSpeaking;					// 正语音说话

	DWORD		dwUserId;					// userId

	BYTE        byMatchType;                 // 排位段位信息 类型nMatchTypeID = 8  
	int         nRankScore;
    int         nRankGrade;
    STeamMemberInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};
typedef list<STeamMemberInfo>	TLIST_MEMBERINFO;

struct SDiceModeInfo
{
    int			nEquipBaseLevel;			// 装备多少档
    bool		bForgetWhite;				// 是否排除白装
    int			nElsePickMode;				// 其次拾取模式

    SDiceModeInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };

    void Clean(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 主角当前地图信息
struct SGroupHeroCurrentMapInfo
{
    DWORD	dwMapID;			// 所在地图ID
    DWORD	dwZoneID;			// 所在场景ID
    POINT	ptTile;				// 位置

    SGroupHeroCurrentMapInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 团成员信息
struct SGroupMemberInfo
{
    int		nTeamID;					// 队伍ID
    DWORD	dwTime;						// 加入时间

    SGroupMemberInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 客户端团成员信息
struct SClientGroupMemberInfo
{
    int		nTeamID;					// 队伍ID
    DWORD	dwCaptain;					// 团长ID
    DWORD	dwTime;						// 加入时间
    BYTE	nCount;						// 成员数
    char	szName[32];					// 团名,用的是就队长名

    SClientGroupMemberInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 客户端团中队伍成员信息
struct SClientGroupTeamMemberInfo
{
    int			nTeamID;					// 队伍ID
    DWORD		clientID;					// 客户端
    DWORD		dwPDBID;					// PDBID
    char		szName[GAME_NAME_MAXSIZE];	// 名字	
    int			nFaceID;					// 头像ID
    int			nLevel;						// 当前等级

    int			nSex;						// 性别
    int			nVocation;					// 职业
    int			nNation;					// 国家

    DWORD		nZoneID;					// 场景ID
    int			nMapID;						// 地图ID
    POINT		ptTile;						// 位置
    POINT		ptMove;						// 移动的目标点

    bool		bCaptainFlag;				// 是否队长
    int			nState;						// 状态(0:掉线；1:在线)

    bool		bDieState;					// 是否死亡状态

    int			nIndex;						// 队友序号

    int			nDataFlag;					// 同步类型 0.要删除,1.不变,2.新加

    SClientGroupTeamMemberInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// 客户端人物位置信息
struct SClientGroupPersonMapLocInfo
{
    DWORD		dwPDBID;					// PDBID
    char		szName[GAME_NAME_MAXSIZE];	// 名字	
    int			nCountryID;					// 国家
    int			nRelation;					// 国家关系,0:本国,1:盟国,2:敌国

    bool		bCaptainFlag;				// 是否队长
    bool		bInGroup;					// 是否在团中

    DWORD		nZoneID;					// 场景ID
    int			nMapID;						// 地图ID
    POINT		ptTile;						// 位置
    POINT		ptMove;						// 移动的目标点

    bool		bOnTracek;					// 是否动态移动
    int			nDataFlag;					// 同步类型 0.要删除,1.不变,2.新加


    SClientGroupPersonMapLocInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SClientGroupPersonMapLocNode
{
    SClientGroupPersonMapLocInfo locData;	// 人物位置信息

    list<POINT>	listMoveTrack;				// 路线
};


/// 使用命令时的上下文
struct SGroupCommandContext
{
    DWORD	dwMapID;			// 使用命令的操作者所在地图ID
    DWORD	dwZoneID;			// 使用命令的操作者所在场景ID
    DWORD	dwSender;			// 使用命令的操作者PDBID
    BYTE	nSenderSex;			// 使用命令的操作者性别
    BYTE	nGroupIndex;		// 接收命令组序号
    DWORD	dwTeamID;			// 接收命令队伍ID
    DWORD	dwCommandID;		// 命令ID
    POINT	ptTarget;			// 位置
    UID		TargetUID;			// 实体对象uid
    BYTE	nSendType;			// 发送者类型 EMGroupSenderType
    char	szCommandName[64];	// 自定义命令名称
    char	szTargetName[32];	// 目标名
    char	szPlayerName[32];	// 操作者角色名
    DWORD	dwTime;				// 时间
    DWORD	dwSnIndex;			// 序号,用来在列表中查找指定命令数据

    SGroupCommandContext()
    { 
        memset(this, 0, sizeof(*this));
    };
    void clear(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

/// 使用命令时的上下文
struct SMsgGroupCommandContext
{
    DWORD	dwMapID;		// 使用命令的操作者所在地图ID
    DWORD	dwZoneID;		// 使用命令的操作者所在场景ID
    DWORD	dwSender;		// 使用命令的操作者PDBID
    BYTE	nSenderSex;		// 使用命令的操作者性别
    BYTE	nGroupIndex;	// 接收命令组序号,为GROUP_CUSTOM_TEAM_INDEX表示自定义队伍,dwTeamID为队伍ID,为GROUP_MEMBER_TEAM_INDEX表示自定义队员,dwTeamID为成员PDBID
    DWORD	dwTeamID;		// 接收命令队伍ID
    UID		uidTarget;		// 使用命令时鼠标点击的位置上的实体对象
    POINT	ptTargetTile;	// 使用命令时鼠标点击的位置
    DWORD	dwCommandID;	// 命令ID 1~255
    BYTE	nSendType;		// 发送者类型 EMGroupSenderType
    BYTE	nStrLens;		// 提示字串长度
    BYTE	nTargetNameLens;// 目标名长度
    BYTE	nNameLens;		// 操作者角色名长度
    // char * szTitleMsg;	// 提示字串

    SMsgGroupCommandContext()
    { 
        memset(this, 0, sizeof(*this));
    }
};

// 当前显示命令处理相关数据
struct SClientCommandDrawInfo
{
    DWORD dwZoneID;						// 使用命令的操作者所在场景ID
    DWORD dwCommandID;					// 命令ID,为0表示没有
    UID HeroUID;						// 主角UID
    UID TargetUID;						// 目标UID
    UID PositionUID;					// 位置UID
    BYTE nTargetType;					// 位置类型
    POINT ptTargetTile;					// 位置
    ulong lArrowEffectID;				// 主角身上的箭头效果ID
    ulong lHeroEffectID;				// 主角身上的附加效果ID
    int	nSoundID;						// 效ID,0为无
    ulong lTargetPosEffectID;			// 目标身上的效果ID,有目标时才有
    ulong lTargetEffectID;				// 目标身上的附加效果ID,有目标时才有
    int nTopTipEffectID;				// 头顶称号效果ID
    DWORD dwTimeOut;					// 过期时间
    bool bFirstShowed;					// 第一次已经显示
    char szCommandTip[32];				// 头顶称号

    SClientCommandDrawInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
    void clear(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// 当前主角显示处理相关数据
struct SClientHeroDrawInfo
{
    ulong lArrowEffectID;				// 主角身上的箭头效果ID
    DWORD dwArrowTimeOut;				// 箭头效果过期时间
    ulong lHeroEffectID;				// 主角身上的附加效果ID
    DWORD dwHeroTimeOut;				// 附加效果过期时间

    SClientHeroDrawInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
    void clear(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// 客户端玩家统计数据信息
struct SClientGroupTotalInfo
{
    DWORD	dwPlayerID;					// 角色ID
    DWORD	dwTime;						// 加入时间
    DWORD	dwTotal[GROUP_TOTAL_MAXID];	// 玩家统计数据,参考 EMGroupTotalType
    BYTE	nLevel;						// 用户等级
    BYTE	nNation;					// 用户所在国家id
    BYTE	nProfession;				// 用户职业
    BYTE	nSex;						// 用户性别
    BYTE	nOnline;					// 在线  参考 EMGroupTotalInfoFlag 组合
    BYTE	nStrLens;					// 队名字串长度
    //char	szPlayerName[32];			// 角色名

    SClientGroupTotalInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// 玩家统计数据信息
struct SGroupTotalInfo
{
    DWORD	dwPlayerID;					// 角色ID
    DWORD	dwTime;						// 加入时间
    DWORD	dwTotal[GROUP_TOTAL_MAXID];	// 玩家统计数据,参考 EMGroupTotalType
    BYTE	nLevel;						// 用户等级
    BYTE	nNation;					// 用户所在国家id
    BYTE	nProfession;				// 用户职业
    BYTE	nSex;						// 用户性别
    BYTE	nOnline;					// 在线  参考 EMGroupTotalInfoFlag 组合
    char	szPlayerName[GAME_NAME_MAXSIZE];			// 角色名

    SGroupTotalInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 玩家统计数据信息Lua用
struct SLuaGroupTotalInfo
{
    DWORD	dwPlayerID;					// 角色ID
    DWORD	dwTime;						// 加入时间
    DWORD	dwTotalDead;				// 死亡数
    DWORD	dwTotalPerson;				// 杀人数
    DWORD	dwTotalMonster;				// 杀怪数
    DWORD	dwTotalDamage;				// 累积总伤害量
    DWORD	dwMaxDamage;				// 最高伤害
    DWORD	dwTotalCure;				// 累积总治疗量	
    DWORD	dwMaxCure;					// 最高治疗
    BYTE	nLevel;						// 用户等级
    BYTE	nNation;					// 用户所在国家id
    BYTE	nProfession;				// 用户职业
    BYTE	nSex;						// 用户性别
    BYTE	nOnline;					// 在线  参考 EMGroupTotalInfoFlag 组合
    char	szPlayerName[GAME_NAME_MAXSIZE];			// 角色名

    SLuaGroupTotalInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 跟随的目标信息
struct SClientFollowTargetInfo
{
    LONGLONG uidTarget;			// 目标UID
    BYTE	nFollowType;		// 跟随类型 0:普通跟随 1:接龙跟随
    DWORD	dwMapID;			// 所在地图ID
    DWORD	dwZoneID;			// 所在场景ID
    POINT	ptTarget;			// 位置
    bool	bClientType;		// 客户端方式跟随

    SClientFollowTargetInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
    void clear(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

struct STeamSyncPersonContext : public SMirrorPersonContext
{		
    int			nAttackPower;				// 攻击能力
    int			nDefendPower;				// 防御能力	

    STeamSyncPersonContext(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SLuaInviteDialogMember
{
    char		szName[GAME_NAME_MAXSIZE];	// 名字

    DWORD		dwPDBID;					// PDBID

    bool		bOnline;					// 在线否

    int			nSex;						// 性别
    int			nVocation;					// 职业
    int			nLevel;						// 等级
    int			nNation;					// 国家		

    bool		bCaptain;					// 队长

    char		szMapName[64];				// 地图名

    int			nAttackPower;				// 攻击能力
    int			nDefendPower;				// 防御能力

    bool		bSameKin;					// 是否同战队
    bool		bSameClan;					// 是否同帮会

    SLuaInviteDialogMember(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 统计信息,以玩家ID为关键字
typedef map<DWORD,SGroupTotalInfo> TMAP_GroupTotalInfo;






//////////////////////////////////////////////////////////////////////////
// 队伍信息
struct TempTeamMemberInfo
{
    DWORD		dwPDBID;					// 用户DBID
    char		szName[GAME_NAME_MAXSIZE];	// 用户的名字
    uchar		nSex;						// 用户性别
    uchar		nNation;					// 用户所在国家id
    uchar		nVocation;					// 用户职业
    bool		bCaptain;					// 是否是队长
    ulong		nLevel;						// 用户等级
    int			nAttackPower;				// 攻击能力
    int			nDefendPower;				// 防御能力
    int         nMapID;						// 用户当前的地图ID
    TempTeamMemberInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// 确认信息数据
struct SMsgTeamClientConfirmInfo
{
    BYTE	nType;			// 类型
    DWORD	dwGroupID;		// 团ID
    DWORD	dwPlayerID;		// 操作人物
    DWORD	dwSpare;		// 数据

    BYTE	nStrLens;		// 提示字串长度
    //char	szTitleMsg[128];	// 提示字串
    BYTE	nTeamSize;		// 队伍个数
    //TempTeamMemberInfo tTeamL[TEAM_MEMBER_MAXCOUNT]; // 队伍个数

    SMsgTeamClientConfirmInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// 确认结果信息数据
struct SMsgTeamClientConfirmResult
{
    BYTE	nType;			// 类型
    DWORD	dwGroupID;		// 团ID
    DWORD	dwPlayerID;		// 操作人物
    DWORD	dwSpare;		// 数据
    BYTE	nResult;		// 结果	 0:取消,1:确定

    SMsgTeamClientConfirmResult(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// 团召集的类型
enum EMGroupConveneType
{
    GROUP_SELF_CONVENE,			// 个人召集
    GROUP_COUNTRY_CONVENE,		// 国家召集
};

// 开始团召集，匹配团的信息
struct SMsgTeamClientStartConvene
{
    EMGroupConveneType emConveneType;		// 召集类型
    int		nLevel;							// 召集等级限制
    int		nCount;							// 召集喊话类型
    char	szDescr[GAME_TITLE_MAXSIZE];	// 团描述

    SMsgTeamClientStartConvene(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// 团信息
struct SMsgTeamClientAllConvene
{
    DWORD	dwGroupNum;						// 团个数
    SMsgTeamClientAllConvene(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// tolua_begin

// 团信息
struct SMsgTeamClientGroupConvene
{
    DWORD	dwGroupID;						// 团ID
    int		nGroupCount;					// 团人数
    DWORD	dwLevelLimit;					// 团等级下限
    char	szGroupName[GAME_NAME_MAXSIZE];	// 召集团名
    char	szDescr[GAME_MSGSTR_MAXSIZE];	// 团描述

    SMsgTeamClientGroupConvene(void)
    {
        memset(this, 0, sizeof(*this));
    };
    bool operator < (const SMsgTeamClientGroupConvene &mNode) const
    {
        if(this->nGroupCount == mNode.nGroupCount)
            return (this->dwGroupID < mNode.dwGroupID);
        return (this->nGroupCount < mNode.nGroupCount);
    };	
};

// 团信息
struct SMsgTeamClienConveneShowInfo
{
    DWORD	dwCountryNum;						// 国家召集次数
    SMsgTeamClienConveneShowInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// GROUP_CLIENT_ADD,GROUP_CLIENT_REMOVE
// 组相关信息数据
struct SMsgTeamClientGroupInfo
{
    DWORD	dwTeamID;		// 小队TeamID
    BYTE	nGroupIndex;	// 组序号(0~9)

    SMsgTeamClientGroupInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};


// GROUP_EVENT_UPDATE 更新团数据
// 更新团数据信息结构
struct SMsgTeamClientUpdateBaseInfo
{
    DWORD	dwGroupID;			// 团ID
    DWORD	dwCaptain;			// 团长PDBID
    bool	bGetTotal;			// 是否统计
    BYTE	nStrLens;			// 团名称长度
    DWORD   dwPickMode;			// 团分配模式
    DWORD	dwActionMode;		// 入团模式
    DWORD	dwLevelLimit;		// 团队等级限制
    //char	szGroupName[32];	// 团名称

    SMsgTeamClientUpdateBaseInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// GROUP_EVENT_UPDATETEAM  更新队伍基本数据
struct SMsgTeamClientUpdateTeamBaseInfo
{
    DWORD	dwTeamID;			// 队伍D
    DWORD	dwCaptain;			// 队长PDBID
    bool	bGetTotal;			// 是否统计

    SMsgTeamClientUpdateTeamBaseInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// GROUP_EVENT_TEAMINFO 更新团队伍信息数据

// 客户端团成员信息
struct SMsgTeamClientTeamInfoNode
{
    int		nTeamID;					// 队伍ID
    DWORD	dwCaptain;					// 团长ID
    BYTE	nCount;						// 成员数
    BYTE	nStrLens;					// 队名字串长度
    //char	szName[32];					// 队名,用的是就队长名

    SMsgTeamClientTeamInfoNode(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 客户端队伍成员信息
struct SMsgTeamClientTeamInfoMemberNode
{
    DWORD	dwPlayerID;					// 玩家角色ID
    char	szName[GAME_NAME_MAXSIZE];					// 角色名

    SMsgTeamClientTeamInfoMemberNode(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 更新团队伍信息数据结构
struct SMsgTeamClientUpdateTeamInfo
{
    BYTE	nTeamCount;			// 队伍个数
    // nTeamCount * SMsgTeamClientTeamInfoNode 
    // nTeamCount * char * szName 

    SMsgTeamClientUpdateTeamInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};


struct SMsgTeamClientMemberNode
{
    int			nTeamID;					// 队伍ID
    DWORD		dwPDBID;					// PDBID
    BYTE		nFaceID;					// 头像ID
    BYTE		nLevel;						// 当前等级
    BYTE		nNation:4;					// 国家
    BYTE		nVocation:4;				// 职业
    BYTE		nSex:4;						// 性别
    bool		bCaptainFlag:2;				// 是否队长
    BYTE		nState:2;					// 状态(0:掉线；1:在线)
    BYTE		nIndex;						// 队友序号
    BYTE		nStrLens;					// 名字串长度

    SMsgTeamClientMemberNode(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 更新团中队伍和队员信息数据结构
struct SMsgTeamClientUpdateTeamMemberInfo
{
    int	nCount;			// 个数
    // nCount * SMsgTeamClientMemberNode 
    // nCount * char * szName 

    SMsgTeamClientUpdateTeamMemberInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// GROUP_EVENT_GROUPINFO 更新团的组信息数据
// 数据信息结构
struct SMsgTeamClientUpdateGroupInfo
{
    BYTE	nTeamCount[GROUP_MAX_GROUP_INDEX];			// 队伍个数
    // nTeamCount * int nTeamID 

    SMsgTeamClientUpdateGroupInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// GROUP_EVENT_TOTALINFO 更新团统计信息数据
// 数据信息结构
struct SMsgTeamClientUpdateTotalInfo
{
    DWORD	dwCount;			// 个数
    // dwCount * SClientGroupTotalInfo
    // dwCount * char* szPlayerName

    SMsgTeamClientUpdateTotalInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// GROUP_EVENT_TEAMTOTAL 更新队统计信息数据
// 数据信息结构
struct SMsgTeamClientUpdateTeamTotalInfo
{
    DWORD	dwCount;			// 个数
    // dwCount * SClientGroupTotalInfo
    // dwCount * char* szPlayerName

    SMsgTeamClientUpdateTeamTotalInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

//	GROUP_EVENT_CHANGECAPTAIN,	更换团长
struct SMsgTeamServerCaptainInfo
{
    DWORD	dwPlayerID;					// 团长玩家角色ID

    SMsgTeamServerCaptainInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

//	GROUP_EVENT_CHANGEPICKMODE,	更换拾取模式
struct SMsgTeamServerPickModeInfo
{
    DWORD	dwPickMode;					// 拾取模式

    SMsgTeamServerPickModeInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// GROUP_EVENT_SENDINFO 	 更新数据
struct SMsgTeamServerUpdateTeamInfo
{
    BYTE	nTeamCount;			// 队伍个数
    // nTeamCount * int nTeamID 

    SMsgTeamServerUpdateTeamInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

//	GROUP_CLIENT_SETTOTAL  设定统计参数
struct SMsgTeamClientSetTotalConfig
{
    BYTE	nType;					// 参数类型,0:队伍统计,1:团统计
    BYTE	nFlag;					// 状态 0:不统计,1:统计,3:清除

    SMsgTeamClientSetTotalConfig(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

//	GROUP_CLIENT_CHANGETEAM  更换队伍
struct SMsgTeamClientChangePlayerTeam
{
    DWORD	dwPlayerID;					// 玩家角色ID
    DWORD	dwTeamID;					// 更换成队的TeamID
    DWORD	dwOpType;					// 操作类型 0:加入队,1:换队,2:离队,3:换队长

    SMsgTeamClientChangePlayerTeam(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 消息
#include "TeamMsgDef.h"