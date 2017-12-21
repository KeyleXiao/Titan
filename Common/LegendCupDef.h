/*******************************************************************
** 文件名:	E:\speed\Common\LegendCupDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/15/2015	15：50
** 版  本:	1.0
** 描  述:	
            杯赛相关定义
********************************************************************/

#pragma once
#include "PackagePtr.h"
#include "IGameDatabaseDef.h"
#include "LegendCup_ManagedDef.h"
using namespace LegendCup_ManagedDef;

// 输出DEBUG信息
//#ifdef VERSEION_DEBUG
#   define LEDENDCUP_DEBUG_OUTPUT
//#endif
#ifdef LEDENDCUP_DEBUG_OUTPUT
#	define	LEDENDCUP_TRACELN(x)			TraceLn(x)
#	define	LEDENDCUP_WARNINGLN(x)			WarningLn(x)
#	define	LEDENDCUP_ERRORLN(x)			ErrorLn(x)
#	define	LEDENDCUP_EMPHASISLN(x)			EmphasisLn(x)
#else
#	define	LEDENDCUP_TRACELN(x)
#	define	LEDENDCUP_WARNINGLN(x)
#	define	LEDENDCUP_ERRORLN(x)
#	define	LEDENDCUP_EMPHASISLN(x)
#endif

#define CUP_MAX_KIN_COUNT         32

#define CUP_MAX_KIN_PLAYER_COUNT  10


typedef DBREQ_PARAM_COMPETITION_NODE SCompetitionNode;

typedef DBREQ_RESULT_LEGEND_CUP_BASE_INFO SLegendCupBaseInfo;

typedef DBREQ_PARAM_LEGENDCUP_MEMBER    SLegendCupMemberInfo;

typedef DBREQ_REQUEST_UPDATE_CUP_KININFO SCupKinInfo;

typedef DBREQ_PARAM_ROUND_GROUP_KIN_POS SGroupKinPos;

typedef DBREQ_PARAM_ROUND_TIMEINFO SRoundTimeInfo;

typedef DBREQ_RESULT_READ_PLAYER_CUP_INFO	SLegendCupPlayerInfo;

enum ELegendCupBO
{
    emBO_None,
    emBO_1,     // bo1
    emBO_3,	    // bo3
    emBO_5,	    // bo5
    emBO_Max,	// 最大
};

// 比赛类型 得分胜利条件
static int g_OBWinCondition[emBO_Max] =
{
    0, 1, 2, 3
};

static int getWinScoreByType(BYTE byType)
{
    if (byType <= emBO_None ||  byType >= emBO_Max)
    {
        return 1;
    }
    else
    {
        return g_OBWinCondition[byType];
    }
}

// 比赛类型 最大多少场能确定输赢
static int g_OBWinMaxScore[emBO_Max] =
{
    0, 1, 3, 5
};

static int getWinMaxScoreByType(BYTE byType)
{
    if (byType <= emBO_None ||  byType >= emBO_Max)
    {
        return 1;
    }
    else
    {
        return g_OBWinMaxScore[byType];
    }
}

enum ELegendDBReadFlag
{
    emLegendRead_NoIni              = 0x0000,
    emLegendRead_BaseData           = 0x0001,				// 基本数据
    emLegendRead_MemberData         = 0x0002,				// 玩家数据
    emLegendRead_KinData            = 0x0004,				// 战队记录数据
    emLegendRead_CompetitionData    = 0x0008,				// 比赛数据
    emLegendRead_RoundInKinList     = 0x0010,               // 每轮晋级列表
    emLegendRead_RoundTimeInfo      = 0x0020,               // 每轮时间信息
    emLegendRead_EXListData         = 0x0040,				// 附加列表数据
    emLegendRead_AllData            = 0x007F,				// 所有数据
};

struct SCreaterLegendCupInfo
{
    LONGLONG llLegendCupID;
	char     szCupName[COMMON_LEGENDCUP_NAME_LEN];// 杯赛名称
    BYTE     byState;
    int      nRegisterKinCount;
    int      nRegisterKinMax;

    SCreaterLegendCupInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SCupRegisterInfo
{
    int nRankID;
    DWORD dwRegisterPDBID;

    SCupRegisterInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SNodeWarRecord
{
    BYTE     byWinFlag;     // 胜利标志 EJudgeWar
    LONGLONG llWarID;       // 战场ID
    DWORD    dwWinnerKin;   // 战场胜利ID
    int      nBoCount;     // 第几场
    bool     bDBHaveRecord;    // 是否已经记录
	// todo
	DWORD	 dwBeginTime;	// 比赛开始时间

    SNodeWarRecord(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// 创建杯赛房间相关
struct SCreateLegendCupRoomInfo
{
    DWORD           dwWorldID;                          // 来自于哪个世界的ID
    int             nBoCount;                            // 第几场 (从1开始)
    int             nRoundID;                           // 第几轮
    bool            bIsOnlineCup;                       // 线上线下
    LONGLONG        dwLedendCupID;                      // 杯赛ID
    int             nSerchID;                           // 比赛搜索ID
    DWORD           dwMatchType;                        // 创建的比赛类型
    DWORD           dwKin1;                             // 参赛队伍1
    DWORD           dwKin2;                             // 参赛队伍2
    DWORD           dwCanEnterTime;                     // 能进入房间时间
    DWORD           dwBeginTimeInterval;                // 能进入房间后开始时间间隔
    int             nMaxCancelCount;                    // 最大能取消的次数
    int             nMaxRound;                          // 最大多少轮

    ERoundCompetitionType eRoundType;                   // 本轮类型 淘汰 or 小组赛
    BYTE            byBoType;                           // bo类型
    int             nNodeKin1Score;                     // Kin1得分
    int             nNodeKin2Score;                     // Kin2得分
    DWORD           dwWarInterval;                      // 节点未结束下一场比赛时间间隔
    bool            bKin1RoundLastNode;                 // kin1本轮最后的节点
    DWORD           dwKin1NodeEndNextWarTime;           // 节点若结束Kin1下一场比赛的时间

    bool            bKin2RoundLastNode;                 // kin2本轮最后的节点
    DWORD           dwKin2NodeEndNextWarTime;           // 节点若结束Kin2下一场比赛的时间
    
    SCreateLegendCupRoomInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// 查看杯赛节点信息
struct SMsgLookUpClientNodeHead
{
    bool bHaveRightJudge;
    DWORD   dwKin1;             // 比赛战队1
    int     nKin1Score;
    char    szKin1Name[KIN_NAME_SIZE];// 名称

    DWORD   dwKin2;             // 比赛战队2
    int     nKin2Score;
    char    szKin2Name[KIN_NAME_SIZE];// 名称
    DWORD   dwResetEndTime;         // 可重置最终时间点

    int  nCount;

    // SNodeWarRecord * nCount;
    SMsgLookUpClientNodeHead(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 节点比分相关
struct SCompetitionScoreInfo
{
    DWORD           dwKinID;                            // 战队ID
    DWORD           dwNodeScore;                        // 比赛胜利得分
    DWORD           dwKDA;                              // KDA

	bool operator < (SCompetitionScoreInfo & NodeInfo) 
	{
		int selfScore = dwNodeScore *10000 + dwKDA;
		int cmpareScore = NodeInfo.dwNodeScore *10000 + NodeInfo.dwKDA;
		return selfScore < cmpareScore;
	}

    SCompetitionScoreInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SCreateCupInfo
{
    DWORD           dwLedendCupID;                      // 杯赛ID
    BYTE            bIsOnlineCup;                       // 线上比赛 线下比赛
    DWORD           dwWorldID;                          // 创建世界ID
    PDBID           dwCreataActorID;                    // 创建玩家
    char            szCupName[COMMON_LEGENDCUP_NAME_LEN];// 杯赛名称
    int             nMaxKinNumber;                    // 最大参赛队伍
    DWORD           dwRegisterDeadline;                 // 报名截止时间
    BYTE            byCupFlowType;                      // 杯赛流程类型
    BYTE            nRoundCount;                        // 打几轮
    ECupState       eCurrentCupState;                   // 状态

    SCreateCupInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SCreateRoundBaseInfo
{
    LONGLONG  llLegendCupID;
    int nDetailConfigID;
    int nRoundID; 
    int nMaxGroup;
    int nGroupMaxKin;
    int nGroupPromotionCount;
    BYTE byRoundCompetitionType;
    BYTE byBoType;

    SCreateRoundBaseInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SNextWarTimeInfo
{
    bool bRoundLastNode;
    DWORD   dwNextWarTime;

    SNextWarTimeInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 公共结构体定义

template<typename TMsg>
bool getLegendCupMsg(TMsg*& pMsg, void*& pData, size_t& stLen)
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


static SNetMsgHead*	getLegendCupMsgHead( PACKAGE_PTR msg, void*& pData, size_t& stLen )
{
	pData	= (void*)msg->c_str();
	stLen	= msg->size();

	Trace("SNetMsgHead*	getLegendCupMsgHead" << msg->size() );

	SNetMsgHead* pMsg;
	if ( !getLegendCupMsg( pMsg, pData, stLen ) )
	{
		WarningLn("getMsgHead failed! !getMsg");
		return nullptr;
	}
	return pMsg;
}

// 消息
//////////////////////////////////////////////////////////////////////////
// CHSS	LEDENDCUP_SCENE_SERVER		场景服上消息
// C	CLIENT					客户端
// CHS	LEDENDCUP_SERVER				聊天服

#include "LegendCupMsgDef.h"

