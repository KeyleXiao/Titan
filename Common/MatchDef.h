/*******************************************************************
** 文件名:	E:\speed\Common\MatchDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/19/2015	19:54
** 版  本:	1.0
** 描  述:	
            比赛大厅相关公共定义
********************************************************************/

#pragma once

#include "GameDef.h"
#include "GatewayServiceDef.h"
#include "Entity_ManagedDef.h"
#include "NetMessageDef.h"
#include "TeamDef.h"
#include "Match_Manageddef.h"
#include "EntityDef.h"

using namespace  Match_ManagerDef;

using Gateway::ClientID;
#pragma pack(1)
// 输出DEBUG信息
#ifdef VERSEION_DEBUG
#   define MATCH_DEBUG_OUTPUT
#endif
#ifdef MATCH_DEBUG_OUTPUT
#	define	MATCH_TRACELN(x)			TraceLn(x)
#	define	MATCH_WARNINGLN(x)			WarningLn(x)
#	define	MATCH_ERRORLN(x)			ErrorLn(x)
#	define	MATCH_EMPHASISLN(x)			EmphasisLn(x)
#else
#	define	MATCH_TRACELN(x)
#	define	MATCH_WARNINGLN(x)
#	define	MATCH_ERRORLN(x)
#	define	MATCH_EMPHASISLN(x)
#endif

// 初始化匹配类型撮合分数
#define	MATCH_TYPE_BASE_SCORE		        900

// 大厅最大允许多少人进入
#define	MATCH_HALL_PLAYER_MAXCOUNT			30000            // 调整大厅最大人数
#define MATCH_STRIKEDOTASK_INTERVAL			1500			// 撮合竞技主任务处理定时器 INTERVAL = 1000毫秒
#define MATCH_AVGDATA_NODETIME				60000			// 等待数据节点时间长度(毫秒) 1分钟
#define MATCH_AVGTIME_NODE_MAX				180				// 等待数据节点最多个数 180个
#define MATCH_AVGTIME_COUNTS				30				// 默认平均等待时间的数据节点数 30个
#define MATCH_DEFALT_WAIT_SECOND            30              // 秒
#define MATCH_PERSON_AT_LEAST_PKNUM         5               // 匹配玩家撮合至少5场之后 才能开始撮合玩家

// 结束选择英雄之后等待加载场景的最长时间
#define	MATCH_MAX_END_SELECT_HERO_WAIT_INTERVAL		10000

// 收集玩家信息等待最大时间
#define ETIMER_END_COLLECT_PLAYERINFO_MAX_INTERVAL  10000

#define	REQUEST_EXDATA_INTERVAL                     3*60*1000      // 十分钟

#define Invalid_MatchRoomPos	UCHAR_MAX
#define Invalid_MatchRoomCamp	UINT_MAX
#define Invalid_MatchHeroID		0
#define Invalid_MatchHeroSkinID	0
// 最大房间ID 也是最大房间个数 不能>DWORD类型上限值
#define	MatchRoomIDMax								10000

// 场景服最大房间数量 也是最大房间个数 不能>DWORD类型上限值
#define	ServerMatchRoomCountMax						500
// 无效的房间步骤
#define INVALID_ROOM_RULE_STEPID					-1

// 撮合节点ID
#define	MatchNodeIDMax								UINT_MAX

// 撮合检测ID
#define	MatchAcceptCheckIDMax						UINT_MAX

// 撮合检测无效ID
#define	MatchAcceptCheckIDInvalid			        0

// 场景服最大地图数量
#define	SERVER_MATCH_MAP_MAX_COUNT                  200


//  竞技请求撮合玩家状态标识
enum EMCantRequestMatchReason
{
    CantRequest_NoReason      = 0,
    CantRequest_Offline,        // 掉线
    CantRequest_NotCaptain,     // 不是队长
    CantRequest_GameState,      // 状态无法申请组队
    CantRequest_InRoom,         // 在房间中
    CantRequest_ReapeatMatch,	// 重复匹配
    // 最大
    CantRequest_MaxID
};

// 场景服房间状态
enum ESceneRoomState
{
    // 无效
    SRS_StateUnknown	= 0,
    // 玩家加载场景
    SRS_StateWaitLoading, 	
    // 战斗中
    SRS_StateFighting,	
    // 结束了
    SRS_StateEnd,

    SRS_Max
};

// 房间使用英雄限定标示
enum EGlobleSelectFlag
{
    // 全局禁用
    SGS_GlobleBan	= 1,
    // 全局只能允许
    SGS_GlobleOnlyCanUse
};


//  请求节点状态
enum EMMatchRequestState
{
	MatchRequestState_None = 0,		// 未知
    MatchRequestState_Collect,		// 收集数据
	MatchRequestState_Wait,			// 等待
    MatchRequestState_Lock,			// 锁定
	MatchRequestState_Match,		// 撮合
    MatchRequestState_WaitAccept,	// 接收拒绝参赛
	MatchRequestState_Fail,			// 失败
	MatchRequestState_OK,			// 成功
	MatchRequestState_Timeout,		// 超时
	// 最大
	MatchRequestState_MaxID
};

//  撮合节点状态标识
enum EMatchNodeState
{
    // 注意别在前面插入枚举变量 后果自负！！！！！
    MatchNodeState_None = 0,		        // 未知
    MatchNodeState_WaitFriendPlayer,		// 等待队友人物撮合
    MatchNodeState_WaitOppositePlayer,      // 等待对方撮合
    MatchNodeState_InRoom,                  // 匹配房间内
    MatchNodeState_Merged,                  // 合并了
    MatchNodeState_WaitAccept,              // 检测玩家确定或者拒绝参赛
    // 最大
    MatchNodeState_MaxID
};

struct SActorPunishmentInfo
{
    BYTE    byPunishmentType;       // EPunishmentType
    DWORD   dwStartTime;            // 惩罚开始时间
    DWORD   dwEndTime;              // 惩罚结束时间

    // (每天清理一次，当天其他方式不会清理)
    int     nObserveCount;     // 剩余观察次数      
    BYTE    byCurLevel;        // 惩罚等级 多次违反添加等级
    BYTE    byOverlayCount;    // 每天避免惩罚累积次数 次数为0 增加惩罚等级
    int     nEveryPunishTime;    // 每次惩罚的分钟

    SActorPunishmentInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 玩家续玩信息
struct SActorContinueInfo
{
    DWORD dwGameWorldID;    // 游戏世界ID
    DWORD dwZoneServerID;  // 续完场景服节点
    int  nSceneID;         // 续完场景ID
    int  nSceneSession;
    SActorContinueInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// 房间英雄数据
struct sRoomHeroData
{
    bool    bCanUse;
    float   fWinRate;
    int     nMatchNum;
    BYTE    byHeroStarLv;
    BYTE    byHeroStaticLv;
    sRoomHeroData(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 房间附加数据
struct SMatchPlayerRoomExData
{
    // todo撮合的时候玩家切换地图可能会改变这些
    ClientID        clientID;                               // client
    DWORD			dwWorldID;		                        // 来源游戏世界ID
    DWORD           dwZoneServerID;                         // 来源场景服ID
    DWORD			dwSceneID;		                        // 来源场景ID
    int             nSceneSN;
    DWORD           dwUserID;                               // 用户ID

    int nMatchTypeGrade[MatchType_MaxID];
    // 英雄胜率
    sRoomHeroData nHero[VOCATION_MAX];

    SMatchPlayerRoomExData(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 角色角色社会服临时数据
struct SMatchPlayerInfo
{
    bool                bIsComputerPlayer;
    DWORD				UserDBID;
    DWORD               dwPDBID;
 // DWORD				heroID;
//  DWORD				skinID;
 // DWORD				preHeroID;
 // int                 nAIID;
    
    BYTE                bySex;
    char				strPlayerName[GAME_NAME_MAXSIZE];
    int                 nSummonerSkill[SUMMONER_SKILL_COUNT];   // 召唤师技能

    SMatchPlayerRoomExData RoomExData;
public:
    //void resetHeroID()	{ heroID = Invalid_MatchHeroID; }

    bool getCanUseHero(int nHeroID)
    { 
        if ( nHeroID > VOCATION_INVALID && nHeroID <VOCATION_MAX)
        {
            return RoomExData.nHero[nHeroID].bCanUse;
        }
        return false;
    }

    BYTE getHeroStarLv(int nHeroID)
    {
        if ( nHeroID > VOCATION_INVALID && nHeroID <VOCATION_MAX)
        {
            return RoomExData.nHero[nHeroID].byHeroStarLv;
        }
        return 0;
    }

    BYTE getHeroStaticLv(int nHeroID)
    {
        if ( nHeroID > VOCATION_INVALID && nHeroID <VOCATION_MAX)
        {
            return RoomExData.nHero[nHeroID].byHeroStaticLv;
        }
        return 0;
    }

    float getHeroWinRate(int nHeroID)
    {
        if ( nHeroID > VOCATION_INVALID && nHeroID <VOCATION_MAX)
        {
            return RoomExData.nHero[nHeroID].fWinRate;
        }
        return 0.0f;
    }

    int getHeroMatchNum(int nHeroID)
    {
        if ( nHeroID > VOCATION_INVALID && nHeroID <VOCATION_MAX)
        {
            return RoomExData.nHero[nHeroID].nMatchNum;
        }
        return 0;
    }

    int getPlayerMatchGrade()
    {
        return RoomExData.nMatchTypeGrade[MatchType_Rank];
    }

    SMatchPlayerInfo(void) 
    {
        memset(this, 0, sizeof(*this));
    }
};

// 房间角色基础信息
struct SRoomRoleBase
{
    bool    bIsOBPos;                               // 是否是OB位置
    DWORD   clientID;		                        // 角色网关ID   
    DWORD	dwPDBID;					            // 角色PDBID
    DWORD	dwHeroID;					            // 选的英雄ID
	BYTE    bySex;                                  // 性别
    BYTE	nIndex;						            // 房间中位置
    BYTE    byHeroStarLv;                           // 英雄星级
    BYTE    byHeroStaticLv;                         // 英雄等级
	DWORD	dwSkinID;								// 选的英雄皮肤ID
    int     nHeroXPSkillID;                         // xp技能ID
    char	szName[GAME_NAME_MAXSIZE];	            // 名字
    int     nSummonserSkill[SUMMONER_SKILL_COUNT];  // 召唤师技能
 
    int     nTalentEffectPageIndex;                 // 当前选择天赋页
    int     nRuneEffectPageIndex;                   // 当前选择符文页
    DWORD   dwUserID;
    int     nWorldID;                               // 来源世界
    int     nZoneServerID;                          // 来源服务器ID
    int     nSceneID;                               // 来源场景
    int     nSceneSN;
	int     nAIID;                                  // 电脑AIID
	bool    isComputerPlayer;                       // 是否假人
    float   fWinRate;                               // 所选英雄胜率
    int     nMatchNum;                              // 所选英雄选择的场次
    int     nRankGrade;                             // 当前段位等级
    int     nHideRankScore;                         // 隐藏分
    SRoomRoleBase()
    {
        memset(this, 0, sizeof(*this));
        nIndex = Invalid_MatchRoomPos;
        isComputerPlayer = false;
        clientID = INVALID_CLIENT;
        dwPDBID = INVALID_PDBID;
        nAIID =0;
        nHideRankScore = 0;
    }
};


enum State
{
    ES_NONE,
    ES_READY,       // 地图加载就绪
    ES_FIGHTING,
    ES_DESTORY_ACTOR,           // 释放玩家状态
    ES_EXIT_BATTLE_FIELD,      // 离开战场
};

struct MatchScenePlayer
{

    SRoomRoleBase	m_role;
    UID             m_UID;
    int             m_SummonerSlot[SUMMONER_SKILL_COUNT];
    State           m_eState;
    DWORD           m_dwLoadOkTick;
    int             m_nCollectDataSize;
    char*           m_pCollectionRoomData;

    MatchScenePlayer()
    {
        m_UID = INVALID_UID;
        m_pCollectionRoomData = NULL;
        m_nCollectDataSize = 0;
        m_eState = ES_NONE;
        m_dwLoadOkTick = 0;
        memset(m_SummonerSlot,0,sizeof(m_SummonerSlot));
    }
};

// 撮合玩家信息
struct SMatchMemberNode
{
    DWORD   dwMatchFlag;		            // 撮合状态
    char    szName[GAME_NAME_MAXSIZE];	    // 角色名
    DWORD   dwWorldID;					    // 玩家来源游戏世界ID
    DWORD   idActor;					    // 玩家角色ID
    int     nFaceID;                        // 头像ID

    SMatchMemberNode(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

typedef map<PDBID,SMatchMemberNode> MAP_MatchMemberNodeList;


struct SMsgMatchPersonInfo
{
    PDBID		dwRequestPDBID;				// 谁请求的撮合PDBID
    PDBID		dwPDBID;					// PDBID
    char		szName[GAME_NAME_MAXSIZE];	// 名字	
    int			nFaceID;					// 头像ID
    int			nLevel;						// 当前等级
    int			nSex;						// 性别
    int			nVocation;					// 职业
    int			nFromGameWorldID;			// 所属游戏世界ID
    DWORD		nZoneID;					// 场景ID
    int			nMapID;						// 地图ID
    DWORD       nRankScore;                 // 比赛Rank分
    DWORD       nHideRankScore;             // 隐藏分
    int         nCurRunePageIdx;			// 当前符文页数据
    int         nTalentEffectPageIndex;		// 当前天赋页数据
    DWORD       dwTeamID;                   // 组队ID
    DWORD       dwClanID;                   // 联盟ID
    DWORD       dwKinID;                    // 战队ID


    SMsgMatchPersonInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// 确定参赛结构体
struct SMatchAcceptCheck
{
    DWORD idActor;				    // 角色ID
    DWORD dwFromWorldID;			// 游戏世界ID
    DWORD dwRequestID;              // 请求ID
};

// 请求撮合信息节点
struct SMatchRequestNode
{
    DWORD idActor;				    // 角色ID
    DWORD dwWorldID;				// 游戏世界ID
    DWORD dwSrcSceneID;             // 来源场景ID
    int   nCount;                  	// 撮合请求人物数量

    DWORD dwAddTicks;		        // 加入时的Ticks
    BYTE  nReqStateFlag;		    // 状态 EMMatchRequestState
    DWORD dwMatchType;	            // 要撮合的类型
    DWORD dwTryTimes;		        // 重试次数
    DWORD dwMatchTicks;		        // 撮合时的Ticks
    int   nTotalRankScore;          // 撮合类型的总和分数
    int   nTotalHideRankScore;      // 撮合类型的总和分数
    DWORD dwMatchNodeID;            // 被分配到的MatchNodeID
    bool  bIsTeamRequest;           // 是否是组队请求
    DWORD dwTeamID;                 // 队伍ID
    int   nWaitCollectCount;        // 等待收集数据的人数

    DWORD dwAcceptID;               // 分配的比赛校验ID
    map<PDBID, SMsgMatchPersonInfo> MemberInfo;// 请求人员信息

    SMatchRequestNode(void)
    {
        Clear();
    };
    void Clear(void)
    {
        idActor = 0;
        dwWorldID= 0;
        dwSrcSceneID = 0;
        nCount = 0;
        dwAddTicks = 0;
        nReqStateFlag = 0;
        dwMatchType = 0;
        dwTryTimes = 0;
        dwMatchTicks = 0;
        nTotalRankScore=0;
        dwMatchNodeID=0;
        bIsTeamRequest = false;
        nWaitCollectCount = 0;
        dwTeamID = 0;
        dwAcceptID = MatchAcceptCheckIDInvalid;
        nTotalHideRankScore = 0;
    };
};

// 玩家请求撮合信息数据,以玩家ID为关键字
typedef map<DWORD,SMatchRequestNode> TMAP_MatchRequestInfo;

// 请求节点列表
typedef list<DWORD> LIST_RequestNodeList;

// 撮合节点信息
struct SMatchNode
{
    EMatchNodeState eMtchNodeState;			    // 单个节点状态 EMatchNodeState
    DWORD   dwMatchType;		                // 类型
    DWORD   dwCreateTime;		                // 创建时间
    DWORD   dwAvgLevel;						    // 所有成员的平均等级,用来提搞匹配速度
    int     nCurMatchPlanStageID;               // 当前撮合规则ID
    int     nCurPlayerCount;                    // 当前撮合节点玩家数量
    int     nTeamPlayMax;                       // 当前撮合节最大组排人数
    int     nTotalRankScore;				    // 所有成员撮合总分数
    int     nTotalHideRankScore;				// 所有成员隐藏总分数
    int     nCampCount;                         // 阵营数量
    int     nMaxMembers;						// 最大成员数
    BYTE    nCampMaxMember;	                    // 配置单个阵营动态设置最多人数
	int		nRecommendCount;					// 节点已推送排位次数

    DWORD   dwPreMatchID;                       // 上次匹配的最适合撮合节点ID
    BYTE    byPreMatchTogetherCount;            // 上次匹配的符合人数

    LIST_RequestNodeList    RequestNodeList;    // 请求节点列表

    SMatchNode(void)
    {
        eMtchNodeState= MatchNodeState_None;
        nCampCount =0;
        dwAvgLevel = 0;
        nMaxMembers = 0;
        nCampMaxMember = 0;
        dwCreateTime = 0;
        nTotalRankScore=0;
        nCurPlayerCount= 0;
		nRecommendCount = 0;
        byPreMatchTogetherCount =0;
        nTotalHideRankScore =0;
    }
};

// 撮合ID 对应撮合节点
typedef map<DWORD, SMatchNode> TMAP_MatchNodeList;


// 等待时间数据节点
struct SMatchAvgWaitTimeNode
{
	UINT64 nAllWaitTimes;			// 所有等待时间总和
	UINT64 nAllActorCounts;			// 所有人数

	SMatchAvgWaitTimeNode(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

// 等待时间数据节点队列
typedef deque<SMatchAvgWaitTimeNode> TDEQUE_MatchWaitTimeInfo;

// 成员竞技匹配信息
struct SMatchResultInfo
{
    DWORD       dwMatchType;          // 匹配的类型
    DWORD       dwMatchNodeID;          // 撮合节点ID
	bool        bMatchOK;				// 匹配结果
	DWORD       dwScore;				// 匹配得分
	DWORD       dwVoteFlag;			// 成员匹配否决位标识

	SMatchResultInfo(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

// 成员竞技匹配结果排序信息
struct SMatchStrikeResultOrderNode
{
	DWORD dwScore;				// 匹配得分
	DWORD dwActorID;			// 角色ID
	SMatchRequestNode * pRequest;	// 对应的玩家请求节点指针

	SMatchStrikeResultOrderNode(void)
	{
		memset(this, 0, sizeof(*this));
	};

	bool operator < (const SMatchStrikeResultOrderNode &mNode) const
	{
		if(dwScore == mNode.dwScore)
		{
			return dwActorID < mNode.dwActorID;
		}
		return dwScore > mNode.dwScore;
	};
};


// 排序结果列表
typedef list<SMatchStrikeResultOrderNode> TLIST_WarMatchSortResultList;


struct SMatchSceneRoomMigrateInfo
{
	PDBID	uDBID;
	ClientID nClientID;
	int		nSrcWorldID;
	int		nSrcSceneID;
	bool	bMigrateFinished;

	Vector3	vPosition;          // 迁移前坐标
	Vector3	vAngle;             // 迁移前朝向

	SMatchSceneRoomMigrateInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 杯赛房间相关
struct SLegendCupRoomInfo
{
	DWORD           dwWorldID;                          // 来自于哪个世界的ID
	bool            bIsOnlineCup;                       // 线上线下
	LONGLONG        dwLedendCupID;                      // 杯赛ID
	int             nSerchID;                           // 比赛搜索ID
	int				nRoundID;							// 比赛第几轮
    int             nBoCount;                           // 第几场
	DWORD           dwKin1;                             // 参赛队伍1
	DWORD           dwKin2;                             // 参赛队伍2
	DWORD           dwCanEnterTime;                     // 能进入房间时间
	DWORD           dwBeginTimeInterval;                // 能进入房间后开始时间间隔
	int             nMaxCancelCount;                    // 最大能取消的次数
	int				nOnlineCountdown;					// 线上等待倒计时

    BYTE            eRoundType;                   // 本轮类型 淘汰 or 小组赛 ERoundCompetitionType
    int             nMaxRound;                          // 最大多少轮
    BYTE            byBoType;                           // bo类型
    int             nNodeKin1Score;                     // Kin1得分
    int             nNodeKin2Score;                     // Kin2得分
    DWORD           dwWarInterval;                      // 节点未结束下一场比赛时间间隔

    bool            bKin1RoundLastNode;                 // 是否本轮最后节点
    DWORD           dwKin1NodeEndNextWarTime;           // 节点若结束下一场比赛的时间

    bool            bKin2RoundLastNode;                 // 是否本轮最后节点
    DWORD           dwKin2NodeEndNextWarTime;           // 节点若结束下一场比赛的时间

	SLegendCupRoomInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 杯赛战队数据
struct SMLegendCupKinInfo
{
	DWORD	dwKinID;		// 战队ID
	uint32  nCamp;			// 战队阵容
	int		nQuitCount;		// 战队成员退出次数

	SMLegendCupKinInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

/*
SMsg_C2MS_KickOutPlayer* pMsg;
if ( !getMsg( pMsg, pData, stLen ) )
{
	MATCH_WARNINGLN("MatchService::onMsgKickOutPlayer failed! !getMsg");
	return;
}
*/
template<typename TMsg>
bool getMsg(TMsg*& pMsg, void*& pData, size_t& stLen)
{
	if (nullptr == pData)
	{
		MATCH_WARNINGLN("getMsg nullptr == pData");
		return false;
	}
	if ( stLen < sizeof(TMsg) )
	{
		MATCH_WARNINGLN("getMsg stLen < sizeof(TMsg) stLen=" << stLen << " sizeof(TMsg)=" << sizeof(TMsg));
		return false;
	}

	pMsg	= (TMsg *)pData;
	pData	= (void*)(pMsg+1);
	stLen	-= sizeof(TMsg);

	return true;
}

static SNetMsgHead*	getMsgHead( PACKAGE_PTR msg, void*& pData, size_t& stLen )
{
	pData	= (void*)msg->c_str();
	stLen	= msg->size();

	SNetMsgHead* pMsg;
	if ( !getMsg( pMsg, pData, stLen ) )
	{
		MATCH_WARNINGLN("getMsgHead failed! !getMsg");
		return nullptr;
	}
	return pMsg;
}

// 服务器撮合信息子消息数据结构
struct SMsgMatchDataSubMsg
{
	DWORD dwMsgCode;						// 子消息代码2类  1、撮合子消息: EMMatchDataSubMsgCode  2、房间子消息:
	DWORD dwUserData1;						// 用户数据1
	DWORD dwUserData2;						// 用户数据2
	DWORD dwUserData3;						// 用户数据3

	// 附加数据

	SMsgMatchDataSubMsg(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 跨区中转消息数据结构
struct SMsgMatchBridgeData
{
	BYTE	nDestEndPoint;		// 目标端点码
	DWORD	dwFromWorldID;		// 角色来自游戏世界ID
	DWORD	dwActorID;			// 角色ID
	WORD	wKeyAction;			// 行为消息码
	DWORD	nDataLens;			// 数据长度
	//BYTE	pData[nDataLens];				// 数据

	SMsgMatchBridgeData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//   竞技撮合结果标识
enum EMMatchResultFlag
{
	MatchResult_None = 0,		// 未知
	MatchResult_AddOK,		// 加入成功
	MatchResult_ExistAdd,		// 已经加入
	MatchResult_CancleOK,		// 退出撮合
	MatchResult_MatchOK,		// 撮合成功
	MatchResult_MatchFail,	// 撮合失败
    MatchResult_ReMatch,	    // 重新撮合
    MatchResult_ActorLogOut,	// 玩家退出
    MatchResult_OtherCancle,	// 其他玩家退出撮合

    MatchResult_CollectCheckError,	// 收集问题检查错误

    MatchResult_RefuseMatch,	// 拒绝撮合确定
    MatchResult_RefuseReMatch,  // 被拒绝之后重新进入撮合
    MatchResult_CreateRoomFailedReMatch,  // 创建房间失败重新进入撮合
    MatchResult_GroupCampFailedReMatch,  // 阵营分组失败
    MatchResult_NotEnoughPersonReMatch,  // 人数不够！无法开启比赛！

    MatchResult_HavePunishInfo,	    // 有惩罚信息
	// 最大
	MatchResult_MaxID
};

//   竞技撮合状态标识
enum EMMatchResultStateFlag
{
	MatchResultState_None = 0,			// 未知
	MatchResultState_FindExist,			// 找到现在的
	MatchResultState_FindNew,				// 撮合到新
	MatchResultState_WaitNew,				// 撮合加入到新
	MatchResultState_WaitResult,			// 撮合加入到新结果
	// 最大
	MatchStrikeResultState_MaxID
};


inline const char* getGameStateDes(BYTE byGameState)
{
    switch(byGameState)
    {
    case GAME_STATE_MATCH:
        {
            return "撮合申请状态";
        }
        break;
    case GAME_STATE_INROOM:
        {
            return "房间状态";
        }
        break;
    case GAME_STATE_WAR:
        {
            return "战场竞技状态";
        }
        break;
    case GAME_STATE_OFFLINE:
        {
            return "不在线状态";
        }
        break;
    case GAME_STATE_LEAVE:
        {
            return "离开状态";
        }
        break;
    case GAME_STATE_OBSERVE:
        {
            return "观战";
        }
        break;
    default:
        break;
    }
    return "";
}



// CS_MSG_MATCH_REQUEST_ADDMATCH			// 请求撮合
// 请求撮合信息
struct SMsgMatchRequestInfo_SO
{
	bool  bIsTeamRequest;               // 队伍请求撮合
    DWORD dwTeamID;                     // 队伍ID
	PDBID idActor;					    // 角色ID
	DWORD dwWorldID;					// 游戏世界ID
    DWORD dwSrcSceneID;                 // 来源场景ID
	DWORD dwMatchType;					// 选中的竞技撮合项
    int   nPlayerCount;					// 请求所包含人物数量(组队 或者是单人)

	SMsgMatchRequestInfo_SO(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

//	Match_CSMsg_RequestEnterStrike,			// 撮合要传人物进入
struct SMsgMatchRequestEnterStrike_SO
{
	DWORD dwStrikeID;					// ID
	DWORD dwStrikeWorldID;				// 游戏世界ID
	DWORD dwZoneID;						// 场景ID
	POINT ptEnter;						// 传入点
	DWORD dwMapID;						// 地图ID
	DWORD dwSceneID;					// 情景ID
	DWORD dwActorID;					// 角色ID
	DWORD dwWorldID;					// 游戏世界ID
	DWORD nEnterCamp;					// 要进入阵营ID,用来传入时指定阵营,为0时表示自动,STRIKE_VIEWER_CAMPID 表示进入观察

	SMsgMatchRequestEnterStrike_SO(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

// 其他附带数据
struct SMsgMatchRequestOtherInfo_SO
{
    DWORD dwActorID;                        // 玩家ID
    int nCurRunePageIdx;					// 当前符文页数据
    int nTalentEffectPageIndex;				// 当前天赋页数据
    BYTE  bySex;
    char szName[GAME_NAME_MAXSIZE];		    // 角色名

    SMatchPlayerRoomExData sExData;         // 附加数据

    SMsgMatchRequestOtherInfo_SO(void)
    {
        memset(this, 0, sizeof(*this));
    };
};


//	Match_CSMsg_RequestInviteJoinStrike			// 请求邀请玩家加入竞技
struct SMsgMatchRequestInviteJoinStrike_SO
{
	DWORD dwServerID;					// 场景服ID
	DWORD dwStrikeID;					// ID
	DWORD dwStrikeWorldID;				// 游戏世界ID
	DWORD dwActorID;					// 角色ID
	DWORD dwWorldID;					// 游戏世界ID
	DWORD dwMatchCamp;					// 阵营序号
	DWORD dwMatchTypeID;				// 类型ID
	BYTE nNameStrlens;					// 邀请玩家角色名长度
	// char szActorName[nNameStrlens];	// 邀请玩家角色名

	SMsgMatchRequestInviteJoinStrike_SO(void)
	{
		memset(this, 0, sizeof(*this));
	};
};


//	Match_CSMsg_ConfirmInviteJoinStrike			// 确认邀请玩家加入竞技
struct SMsgMatchConfirmInviteJoinStrike_SO
{
	DWORD dwServerID;					// 场景服ID
	DWORD dwStrikeID;					// ID
	DWORD dwStrikeWorldID;				// 游戏世界ID
	DWORD dwStrikeCamp;					// 阵营序号

	SMsgMatchConfirmInviteJoinStrike_SO(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

// Match_CSMsg_RequestJoinStrike			// 请求邀请加入竞技
struct SMsgMatchRequestJoinStrike_SO
{
	DWORD idActor;					// 角色ID
	DWORD dwWorldID;					// 游戏世界ID
	BYTE nCountryID;					// 国家ID
	BYTE nVocation;						// 职业位标识 参考 MatchVocationFlag
	WORD nMaxLevel;						// 3魂最高等级
	BYTE nCurLingHun;					// 当前魂
	BYTE nSex;							// 性别
	BYTE nFaceID;						// 头像ID
	char szName[GAME_NAME_MAXSIZE];		// 角色名
	int nAttackPower;					// 攻击能力
	int nDefendPower;					// 防御能力
	DWORD dwMapID;						// 地图ID
	DWORD dwMatchType;					// 选中的竞技撮合项
	BYTE nLastWin;						// 最后32次竞技结果赢的次数
	BYTE nRateFlag;						// 倍率位标识

	DWORD dwServerID;					// 场景服ID
	DWORD dwStrikeID;					// ID
	DWORD dwStrikeWorldID;				// 游戏世界ID
	DWORD dwStrikeCamp;					// 阵营序号

	SMsgMatchRequestJoinStrike_SO(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

struct SMsgMatchAcceptCheck
{
    DWORD   dwAcceptCheckID;
    BYTE    byMatchType;
    int     nWaitTime;

    SMsgMatchAcceptCheck(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

struct SMsgMatchCurPlayerCount
{
    int    nMaxCount;
    int    nCurCount;
    int    nLeftTime;
    SMsgMatchCurPlayerCount(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

struct SMsgMatchRequestJoinStrike_SS
{
	DWORD dwStrikeCamp;					// 阵营序号,要进入阵营ID,用来传入时指定阵营,为0时表示自动,STRIKE_VIEWER_CAMPID 表示进入观察
	DWORD dwInterfaceType;				// 接口类型定义 EMStrikeInterfaceType

	SMsgMatchRequestJoinStrike_SS(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

//	Match_CSMsg_RequestViewStrike,			// 请求进入查看
struct SMsgMatchRequestViewStrike_SO
{
	DWORD dwServerID;					// 场景服ID
	DWORD dwStrikeID;					// ID
	DWORD dwStrikeWorldID;				// 游戏世界ID
	DWORD dwStrikeCamp;					// 阵营序号

	SMsgMatchRequestViewStrike_SO(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

struct SMsgAddOrRematchResultInfo
{
    DWORD dwCurMatchPlayerCount;        // 当前匹配到的玩家数量
    DWORD dwMatchTypeID;				// 类型ID
    DWORD dwAvgWaitTimes;               // 平均等待时间
    SMsgAddOrRematchResultInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

struct SMsgMatchOutInfo
{
    bool bIsSelf;
    char szName[GAME_NAME_MAXSIZE];
    SMsgMatchOutInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};
 
typedef SMsgMatchOutInfo SMsgMatchOtherCancelInfo;

typedef SMsgMatchOutInfo SMsgMatchRefuseInfo;
// 消息
//////////////////////////////////////////////////////////////////////////
// MSS  MATCH_SCENE_SERVER	场景服上的比赛大厅 
// C	CLIENT				客户端
// MS	MATCH_SERVER		比赛大厅服
#include "MatchMsgDef.h"

#pragma pack()