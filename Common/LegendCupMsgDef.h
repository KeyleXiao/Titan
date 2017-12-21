//==========================================================================
/**
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/15/2015	17：45
** 版  本:	1.0
** 描  述:	排行榜消息定义
*/
//==========================================================================

#ifndef __LEDENDCUPMSGDEF_H__
#define __LEDENDCUPMSGDEF_H__

#include "GatewayServiceDef.h"
#include "MatchDef.h"
#include "NetMessageDef.h"
#include "IGameDatabaseDef.h"
#include "Match_Manageddef.h"
using namespace Match_ManagerDef;

#include "LegendCup_ManagedDef.h"
using namespace LegendCup_ManagedDef;

using namespace Gateway;
// 消息ID定义
//////////////////////////////////////////////////////////////////////////
enum LEDENDCUP_NET_MESSAGE_KEYACTION
{
    /////////////////////////////////////客户端到场景服/////////////////////////////////////

	/////////////////////////////////////客户端到场景服/////////////////////////////////////
    CS_REQUEST_CREATE_CUP_TYPE,         // 请求创建杯赛的类型
	CS_REQUEST_CREATE_CUP,              // 请求创建杯赛
    CS_REQ_JOIN_LEGENDCUP_ROOM,         // 请求进入房间

    /////////////////////////////////////客户端到社会服/////////////////////////////////////
    CO_REQUEST_CUP_LIST,                    // 请求杯赛列表
    CO_REQ_JOIN_CUP,                            // 请求加入比赛
    CO_REQ_EXIT_CUP,                            // 请求退出比赛
    CO_REQ_ADD_TO_BLACKLIST,                   // 添加到黑名单
    CO_REQ_REMOVE_BLACKLIST,                   // 移除黑名单
    CO_REQ_LOOK_OVER_LEGENDCUP,         // 请求查看比赛
    CO_REQ_ALL_BALCKLIST,               // 请求查看黑名单列表
    CO_REQ_KIN_MEMBER_LIST,             // 请求查看黑名单列表

    CO_REQ_LOOK_UP_CUP_NODE_WARINFO,      // 请求查看节点战场信息
    CO_REQ_CUP_JUDGE_SINGAL_WAR_RESULT, // 裁决单个比赛结果
    CO_REQ_PUBLIC_COMPETTIONNODE,       // 发布比赛
	CO_REQ_QUIT_OBSERVE,				// 退出比赛观察
    CO_REQ_OFFLINE_PRE_END_REGISTER,		// 请求预先结束线下杯赛报名阶段

    /////////////////////////////////////社会服到客户端/////////////////////////////////////
    OC_CUP_SEND_ALL_KINLIST,               // 发送所有请求列表到客户端
    OC_CUP_SEND_ALL_BALCKLIST,             // 发送所有黑名单列表到客户端
    OC_CUP_SEND_KIN_MEMBER,                // 请求战队玩家成员
    OC_CUP_SEND_ALL_LEGENDCUP,             // 请求战队玩家成员
    OC_CUP_SEND_ALL_ROUNDINFO,             // 请求所有杯赛信息
    OC_CUP_SEND_ALL_KIN_MEMBERINFO,        // 请求所有杯赛信息
    OC_CUP_SEND_NODE_LOOKUP_WARINFO,       // 请求杯赛节点查看信息
    OC_CUP_SEND_UPDATE_SINGEL_CUP_INFO,    // 发送更新单个比赛信息
    OC_CUP_SEND_UPDATE_SINGEL_CUP_NODE_INFO,// 发送更新单个杯赛节点信息
    OC_CUP_SEND_ALL_ROUNDTIME,              // 所有轮时间安排信息
    OC_CUP_SEND_REGISTER_SUCCESS,           // 申请加入成功
    OC_CUP_SEND_CREATER_PUBLIC_SUCCESS,      // 发布成功
    OC_CUP_SEND_PUBLIC_SUCCESS,            // 发布成功


	/////////////////////////////////////场景服到社会服/////////////////////////////////////
    SO_CUP_CREATECUP_CHECK,                 // 社会服创建杯赛校验
    SO_CUP_CREATE_CUP_RESLUT,               // 通知社会服创建杯赛结果
    SO_CUP_REQ_JOIN_LEGENDCUP_ROOM,         // 请求进入房间
    SO_CUP_RECORD_WAR_RESULT,               // 单个比赛结果
    SO_CUP_REQ_JOIN_CUP_SUCCESS,            // 请求加入杯赛成功

    /////////////////////////////////////社会服到场景服/////////////////////////////////////
    OS_CUP_CAN_CREATECUP,                   // 可以创建杯赛
    OS_CUP_REQUEST_JOIN_CUP,                // 场景服扣除申请加入杯赛相关

    /////////////////////////////////////社会服到场景服/////////////////////////////////////
    SC_CUP_CREATE_CUP_TYPE,                 // 创建杯赛类型相关
   

};


// 请求退出观察
struct SMsgReqQuitObserve
{
	LONGLONG    llLegendCupID;

	SMsgReqQuitObserve()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 返回比赛类型
struct SMsgClientCreateCupType
{
    ELegendCupType byCreateType;
    bool     bIsFree;

    SMsgClientCreateCupType()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 请求比赛类型
struct SMsgCreateCupType
{
    // 选择创建的杯赛配置信息
    ELegendCupType  byCreateType;        // 创建的类型ID 
    PDBID  dwActorID;                    // 玩家ID 

    SMsgCreateCupType()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgRoundGroupInfo
{
    DWORD    dwKinID;
    char     szKinName[KIN_NAME_SIZE];

    SMsgRoundGroupInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgAllMemebrHead
{
    LONGLONG    llLegendCupID;
    int         nMemberCount;
    

    SMsgAllMemebrHead()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgAllRoundHead
{
    LONGLONG    llLegendCupID;
    int    nRoundCount;
    int    nAllNodeCount;

    SMsgAllRoundHead()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgCompetitionRoundHead
{
    int    nRoundID;
    int    nMaxGoup;             // 分组数量
    int    nGroupMaxKin;         // 每组最大战队数量
    BYTE   eCompetitionType;     // ERoundCompetitionType
    DWORD  dwRoundBeginTime;     // 本轮比赛开始时间
    int    nCount;              //  本轮有多少个节点
    bool   bIsFinal;            // 是否是最终轮

    SMsgRoundGroupInfo KinIDList[DBDEF_LEGENDCUP_KINSCOUNT_LEN];

    SMsgCompetitionRoundHead()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgCompetitionNodeInfo
{
    BYTE    byCompetitionType;   // 1小组赛 2淘汰赛
    int     nSerchID;           // 生成的快速定位ID
    int     nRoundID;           // 第几轮比赛
    int     nGroupID;           // 分配到第几组
    int     nDay;               // 第几天
    int     nPos;               // 位置索引 从1开始
    DWORD   dwKin1;             // 比赛战队1
    char    szKin1Name[KIN_NAME_SIZE];// 名称
    int     nKin1Score;         // 战队1本次比赛分数

    DWORD   dwKin2;             // 比赛战队2
    char    szKin2Name[KIN_NAME_SIZE];// 名称
    int     nKin2Score;         // 战队2本次比赛分数

    BYTE    eNodeState;         // 节点状态 ECompetitionNodeState
    DWORD   dwWinnerKinID;      // 胜利战队ID
    BYTE    byBoType;           // BO类型 ELegendCupBO
    int     nCurrentBo;         // 已经打了多少场

    DWORD   dwBeginTime;		// 节点比赛开始时间
    DWORD   dwEnterTime;		// 节点进场开始时间
    DWORD   dwServerTime;		// 服务器当前时间


    SMsgCompetitionNodeInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};


// 请求杯赛列表信息
// 请求所有申请战队信息
struct SMsgLegendCupListHead
{
    // 数量
    int    nCount;

    SMsgLegendCupListHead()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 请求所有申请战队信息
struct SMsgLegendCupInfo
{
    bool            bNewCreate;                         // 是否新建
    LONGLONG        llLegendCupID;                      // 杯赛ID
    int             nCreateConfigID;                    // 创建的配置ID
    BYTE            byCreateCupType;                    // 杯赛类型 ELegendCupType
    BYTE            bIsOnlineCup;                       // 线上比赛 线下比赛
    DWORD           dwCreataActorID;                    // 创建玩家 系统的为 0
    char            szCupName[DBDEF_LEGENDCUP_NAME_LEN];// 杯赛名称
    BYTE            byMaxKinNumber;                      // 最大参赛队伍
    DWORD           dwRegisterLeftTime;                  // 报名剩余时间
    bool            bClanLimit;                         // 只有联盟限制才能参加
    int             nLevelLimit;                        // 等级限制
    BYTE            eCupState;                          // 状态 ECupState
    bool            bIsEnd;                             // 是否结束
    DWORD           dwEndTime;                          // 结束日期
    bool            bHavePassword;                      // 是否有密码
    char            szCreaterName[DBDEF_ACTORNAME_MAXSIZE];// 创建者名称
    bool            bIsJoinedCup;                         // 是否加入了的比赛
	int				nCurrentKinNum;						// 当前参赛战队数量
	int				nNeedRoundCount;                    // 杯赛需要打几轮
	int				nBeginRound;                        // 本场比赛由第几轮开始
	int				nCurrentRound;                      // 当前打到第几轮
    DWORD			dwTotalCompetitionBonus;            // 比赛总奖金
    DWORD			dwRegistGold;                       // 报名需要的金币
    
    SMsgLegendCupInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};


// 请求所有申请战队信息
struct SMsgRequestLookOverCup
{
    // 选择创建的杯赛配置信息
    LONGLONG    llLegendCupID;

    SMsgRequestLookOverCup()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 请求黑名单列表
struct SMsgRequestAllBlackKinList 
{
    LONGLONG    llLegendCupID;

    SMsgRequestAllBlackKinList()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 请求战队信息
struct SMsgRequestKinMember 
{
    ClientID    clientID;
    LONGLONG    llLegendCupID;
    DWORD       dwKinID;

    SMsgRequestKinMember()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgJoinLegendCupRoom
{
    LONGLONG    llLegendCupID;
    int         nRoundID;
    int         nSerchID;
    DWORD       dwRoomID;
    
    //大厅的数据
    DWORD       dwKinID;                                // 战队ID
    DWORD       dwSrcScene;                             // 来源场景ID
    DWORD       dwSrcWorldID;                           // 来源世界ID
    PDBID	    idActor;	                            // 进入大厅人物PDBID
    DWORD	    dwHeroID;                               // 选的英雄ID
    char	    szName[GAME_NAME_MAXSIZE];	            // 玩家名字
    BYTE        bySex;                                  // 性别
    int         nSummonerSkill[SUMMONER_SKILL_COUNT];   // 召唤师技能
    int         nTalentEffectPageIndex;                 // 当前选择天赋页
    int         nRuneEffectPageIndex;                   // 当前选择符文页
    SMatchPlayerRoomExData sExData;                     // 附加数据

    SMsgJoinLegendCupRoom()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgLookUpCupNodeWarInfo
{
    ClientID    clientID;
    LONGLONG    llLegendCupID;
    int         nRoundID;
    int         nSerchID;
    SMsgLookUpCupNodeWarInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 比赛结束 默认记录比赛结果
struct SMsgRecordNodeWarResult
{
    LONGLONG        llLegendCupID;                      // 杯赛ID 本区线下判定
    int             nRoundID;                           // 第几轮
    int             nBoCount;                           // 第几场
    int             nSerchID;                           // 节点查询ID
    LONGLONG        llWarID;                            // 需要记录比赛ID   (添加到比赛节点)
    DWORD           dwKin1KDA;                          // Kin1 KDA
    DWORD           dwKin2KDA;                          // Kin2 KDA
    BYTE            byJudgeFlag;                        // EJudgeNodeWar （线上自动判定 线下打完比赛设置为 JUDGE_NONE）
    DWORD           dwWinnerKinID;                      // 根据byJudgeFlag 来确定是否选择WinnerID

	DWORD			dwBeginTime;						// 战场开始时间
	DWORD			dwKin1ID;							// 家族1ID
	DWORD			dwKin2ID;							// 家族2ID
	int				nKin1Glory[emLegendGlory_Max];		// 家族1杯赛荣誉buff
	int				nKin2Glory[emLegendGlory_Max];		// 家族2杯赛荣誉buff

    SMsgRecordNodeWarResult()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 比赛结束 默认记录比赛结果
struct SMsgSetCompetitionNodeState
{
    LONGLONG        llLegendCupID;                      // 杯赛ID 本区线下判定
    int             nRoundID;                           // 第几轮
    int             nSerchID;                           // 节点查询ID
    ECompetitionNodeState eState;                       // 设置比赛节点状态

    SMsgSetCompetitionNodeState()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 判定比赛结果
struct SMsgJudgeWarResult
{
    ClientID        clientID;
    DWORD           dwWorldID;                          // 线上自动判定（公共区得转到本区修改）
    LONGLONG        llLegendCupID;                      // 杯赛ID 本区线下判定
    int             nRoundID;
    int             nSerchID;

    int             nBoCount;                           // 第几场
    LONGLONG        llWarID;                            // 比赛ID
    BYTE            byJudgeFlag;                        // EJudgeNodeWar 
    DWORD           dwWinnerKinID;                      // 根据byJudgeFlag 来确定是否选择WinnerID
    DWORD           dwResetNewTime;                       // 重置节点新的开场时间
    DWORD           dwKin1;                                         // 队伍1
    int             nKin1Score;									// 队伍1得分
    DWORD           dwKin2;                                         // 队伍2
    int             nKin2Score;									// 队伍2得分

    SMsgJudgeWarResult()
    {
        memset(this, 0, sizeof(*this));
    }
};


// 请求所有申请战队信息
struct SMsgRegisterHead
{
    
    int    nCount;              // 当前数量
    int    nMaxCount;           // 最大参赛数量
    DWORD  dwRegisterLeftTime;  // 剩余报名时间
	LONGLONG	llLegendCupID;  // 杯赛ID
	// TODO
	DWORD  dwConfirmKinLeftTime;// 比赛名单确认剩余时间


    SMsgRegisterHead()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 请求所有黑名单战队信息
struct SMsgBalckListHead
{
    int    nCount;              // 当前数量
	LONGLONG	llLegendCupID;  // 杯赛ID

    SMsgBalckListHead()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 请求所有申请战队信息
struct SMsgRegisterKinInfo
{
    char szName[KIN_NAME_SIZE];                 // 战队名称
    int  nKinProp_ID;							// 战队ID
    int  nKinProp_Level;						// 战队等级
	// todo
	int nKinWinNum;								// 战队杯赛总胜场
	int nKinMaxNum;								// 战队杯赛总场数

    SMsgRegisterKinInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 请求所有申请战队信息
struct SMsgBlackKinInfo
{
    char szName[KIN_NAME_SIZE];                 // 战队名称
    int  nKinProp_ID;							// 战队ID
    int  nKinProp_Level;						// 战队等级

    int nKinWinNum;								// 战队杯赛总胜场
    int nKinMaxNum;								// 战队杯赛总场数

    SMsgBlackKinInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};




// 请求比赛类型排行榜信息
struct SMsgCreateCup
{
    // 选择创建的杯赛配置信息
    int             nCreateConfigID;                    // 创建的配置ID
    char            szCupName[DBDEF_LEGENDCUP_NAME_LEN];// 杯赛名称
    BYTE            byCreateCupType;                    // 杯赛类型 ELegendCupType
    BYTE            bIsOnlineCup;                       // 线上比赛 线下比赛
    DWORD           dwCreataActorID;                    // 创建玩家 系统的为 0
    char            szPassword[DBDEF_PASSWORD_MAXSIZE];	// 参赛密码
    DWORD           dwClanID;                           // 帮会ID
    bool            bClanLimit;                         // 只有联盟限制才能参加
    int             nLevelLimit;                        // 等级限制
    int             nDayInterval;                       // 时间间隔
    int				nContriBonus;						// 出资奖金
    int				nRegisterNeedTicket;				// 申请参加杯赛需要的点卷
    BYTE            byTicketAllocateMode;              //  杯赛奖励分配方式 ETicketAllocateMode
    BYTE            byRankTicketRatio[DBDEF_LEGENDCUP_KINSCOUNT_LEN]; // 分配奖励比例

    SMsgCreateCup()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgJoinCup
{
    int             nCreateConfigID;                // 杯赛配置ID
    BYTE            byCreateCupType;                // 杯赛类型 ELegendCupType
    LONGLONG        llLegendCupID;
    DWORD           dwKinID;
    DWORD           dwActorID;
    DWORD           dwRegisterNeedTicket;
    char            szPassword[DBDEF_PASSWORD_MAXSIZE];	// 参赛密码

    SMsgJoinCup()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgExitCup
{
    BYTE            byCreateCupType;                // 杯赛类型 ELegendCupType
    LONGLONG        llLegendCupID;
    DWORD           dwKinID;
    DWORD           dwActorID;

    SMsgExitCup()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgOfflineEndRegister
{
    BYTE            byCreateCupType;                // 杯赛类型 ELegendCupType
    LONGLONG        llLegendCupID;
    DWORD           dwKinID;
    DWORD           dwActorID;

    SMsgOfflineEndRegister()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgAddToBlackList
{

    BYTE            byCreateCupType;                // 杯赛类型 ELegendCupType
    LONGLONG        llLegendCupID;
    DWORD           dwKinID;
    DWORD           dwOperateID;

    SMsgAddToBlackList()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgRemoveBlackList
{

    BYTE            byCreateCupType;                // 杯赛类型 ELegendCupType
    LONGLONG        llLegendCupID;
    DWORD           dwKinID;
    DWORD           dwOperateID;

    SMsgRemoveBlackList()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgPublicCompetition
{

    BYTE            byCreateCupType;                // 杯赛类型 ELegendCupType
    LONGLONG        llLegendCupID;
    DWORD           dwOperateID;

    SMsgPublicCompetition()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgAllTimeHead
{
    LONGLONG    llLegendCupID;
    int         nCount;

    SMsgAllTimeHead()
    {
        memset(this, 0, sizeof(*this));
    }
};


#endif // __LEDENDCUPMSGDEF_H__