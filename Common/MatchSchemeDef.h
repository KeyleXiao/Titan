/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\Match_SchemeDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/18/2015	16:36
** 版  本:	1.0
** 描  述:	
            比赛 文件数据定义
********************************************************************/

#pragma once
#include "MatchDef.h"
#include <string>
#include "Stage_ManagedDef.h"
#include "Match_Manageddef.h"
using namespace Match_ManagerDef;

using namespace Stage_ManagerDef;
// 比赛大厅房间配置
#define DMATCHROOM_SCHEME_FILENAME "Scp\\MatchRoom.csv"

#define DMATCHROOM_SELECT_CONTROL_SCHEME_RULE "Scp\\MatchRoomSelectControl.csv"

#define DMATCHROOM_SCHEME_RULE "Scp\\MatchRoomRule.xml"

#define DMATCHROOM_SCHEME_COMPUTER_SUM_SKILL "Scp\\MatchRoomComSumSkill.csv"

#define DMATCHROOM_SCHEME_PUNISH_FILENAME "Scp\\MatchPunish.csv"


#define MAX_ROOM_POS			10
#define MAX_RULE_STEP			20
#define MAX_SELECT_FLAG_COUT		20
#define MAX_BAN_COUNT			10

struct SSchemeMatchRoom
{
    // 比赛类型
    DWORD       m_dwMatchTypeID;

    // 比赛类型描述
    char       m_szMatchTypeDes[GAME_NAME_MAXSIZE];

	// 模式 EGameMode
	BYTE		m_nModeID;
	// 地图
	DWORD		m_dwMapID;
	// 玩法名称TextInfoID
	int			m_NameTextInfoID;
	// 战场ID
	int			m_WarID;
	// 阵营数
	BYTE		m_CampCount;
	// 每个阵营最大人数
	BYTE		m_CampRoleMax;
	// 开始选英雄需要的最小人数
	BYTE		m_StartSelectHeroNeedRoleMin;
	// 开始选英雄需要每阵营最小人数
	BYTE		m_StartSelectHeroNeedCampRoleMin;
	// 描述TextInfoID
	int			m_DescTextInfoID;
	// 图片路径TextInfoID
	int			m_PicPathTextInfoID;
	// 选人倒计时毫秒
	DWORD		m_SelectHeroTime;	
	// 选人确定后缩短倒计时到毫秒
	DWORD		m_ReadyGoTime;	
	// 那些状态的房间在大厅中显示
	bool		m_RoomStateShow[MatchRoomStates];
	// 那些状态可以换到本阵营中的空位置
	bool		m_RoomStateChangePos2SelfCamp[MatchRoomStates];
	// 那些状态可以换到其他阵营中的空位置
	bool		m_RoomStateChangePos2OtherCamp[MatchRoomStates];
	// 那些状态可以从大厅加入房间
	bool		m_RoomStateEnter[MatchRoomStates];
	// 那些状态可以离开房间
	bool		m_RoomStateExit[MatchRoomStates];
	// 那些状态可以踢人
	bool		m_RoomStateKick[MatchRoomStates];
	// 选英雄阶段可以选几次英雄
	DWORD		m_SelectHeroCount;
	// 客户端加载场景最长时间 ms
	DWORD		m_CLLoadSceneTimeMS;
	// 同一英雄在所有阵营中最多能有几个
	BYTE		m_SameHeroCountMax;

	//选择规则 包括禁用，选择顺序
	BYTE		m_HeroSelectRuleID;

	//选择英雄规则ID （重复可选控制）
	BYTE		m_SelectHeroControlID;

    // 全局英雄标示 EGlobleSelectFlag
    BYTE		m_byGlobleSelectFlag;

	//根据标示设置的英雄列表
	int			m_SelectFlagHeroList[MAX_SELECT_FLAG_COUT];

    //延迟进入房间
    bool		m_bCanDeLayJoinRoom;

    //延迟进入房间时间	
    int		    m_nDeLayJoinTime;

    //进入房间离线检测
    bool		m_bDisconnectCheck;

    // 自动添加电脑
    bool        m_bOpenAddComputer;

    // 匹配添加电脑时间
    int         m_nAddComputerTime;

    // 电脑开始选人定时器
    int         m_nComputerStartSelectTime;

    //ban选开关	
    bool        m_bOpenBan;

    //开始房间ban选时间(秒)	
    int         m_nBanTime;

    //ban选预选英雄广播其他阵营
    bool        m_bBroadCastOther;

    //阵营ban选数量
    int         m_nCampBanCount;

    // 不读配置 跟俊m_BanPosList算出来的大小
    int         m_nBanPosCount;
    //ban选位置
    int			m_BanPosList[MAX_SELECT_FLAG_COUT];

    //英雄星级显示相关
    bool        m_bShowHeroStarInfo;

    // 显示升级降级界面
    bool        m_nbShowDownOrUpWnd;

    // 匹配接受确定时间
    int        m_nMatchAcceptTime;
    
    // 战场中退出
    bool       m_bCanLogOutInWar;

    // 最大OB人数
    int        m_nMaxOBCount;

    // 随机选择英雄开关
    bool        m_bRandomSelectHero;

	// 获取房间位置[0, ...)对应的阵营[0, ...)
	DWORD	getPosCamp(uchar Pos)
	{
		return getPosCamp(Pos, m_CampRoleMax);
	}
	// 获取房间位置[0, ...)对应的 阵营[0, ...)
	static DWORD	getPosCamp(uchar Pos, DWORD u32CampRoleMax)
	{
		return (Pos / u32CampRoleMax);
	}

	// 获取房间位置[0, ...)对应的 阵营中的位置[0, ...)
	DWORD	getPosCampPos(uchar Pos)
	{
		return (Pos % m_CampRoleMax);
	}

	// 最大角色个数
	DWORD getPlayerCountMax()
	{
		return (m_CampCount * m_CampRoleMax);
	}

    SSchemeMatchRoom()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SSchemeSelectControl
{
	// 选择英雄规则ID
	int		m_nID;
	// 限制类型
	EMHeroSelectControlType	m_nType;
	// 最大数量
	BYTE		m_HeroMaxCount;
	SSchemeSelectControl()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SSchemeComputerSumSkill
{
    // 英雄ID
    int		m_nHeroID;

    // 技能ID
    int		m_nSkillID;

    // XP技能ID
    int		m_nXPSkillID;
    SSchemeComputerSumSkill()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SSchemePunish
{
    // 惩罚类型
    BYTE		byPunishType;

    // 免罚次数
    BYTE        byOverlayCount;
    
    // 惩罚时间基数
    int         nEveryPunishTime;

    // 观察周期
    int         nObserveCount;

    // 是否需要检测比赛类型
    bool        bMatchTypeCheck;

    int         nMatchTypeCount;

    int			nMatchTypeArray[MatchType_MaxID];

    bool needPunish(int nMatchType)
    {
        for (int n = 0; n < nMatchTypeCount; ++n)
        {
            if (nMatchTypeArray[n] == nMatchType)
            {
                return true;
            }
            
        }
        return false;
    }

    SSchemePunish()
    {
        memset(this, 0, sizeof(*this));
    }
};


struct SRuleStep
{
	int						nStepId;
	EMRoomSelectRuleType	nTypeID;
	int						nTimeInterval;
	int						nPosCount;
	int						nPos[MAX_ROOM_POS];
	bool					bEnd;
	SRuleStep()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SchemeMatchRoomRule
{
	int nRuleID;
	SRuleStep  RuleStep[MAX_RULE_STEP];

	SchemeMatchRoomRule()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 比赛大厅房间配置接口
class ISchemeMatchRoom
{
public:
	virtual vector<SSchemeMatchRoom* >&	getByMode(uchar ModeID) = 0;
	virtual vector<SSchemeMatchRoom* >&	getByMap(DWORD MapID) = 0;
	virtual vector<uchar>&				getAllMode() = 0;
	virtual SSchemeSelectControl*		getSelectControlScheme(int nID) = 0;
    virtual SSchemeMatchRoom*			getSchemeMatchRoomByMatchTypeID(DWORD dwMatchTypeID) = 0;
    virtual SSchemeComputerSumSkill*	getSchemeComputerSumSkill(int nHeroID) = 0;
};

// 比赛大厅房间配置接口
class ISchemeMatchRoomRule
{
public:

	virtual SchemeMatchRoomRule*			getSchemeMatchRoomRule(int nRuleID) = 0;
};


// todo:pt 先放这里 后面移到公共的地方
#define TEXTINFO_SCHEME_FILENAME "Scp\\TextInfo.csv"
class ISchemeTextInfo
{
public:
	virtual string&	getText(int nID) = 0;
};

// SWarBroadCastInfoScheme
#define WARBROADCASTINFO_SCHEME_FILENAME "Scp\\WarBroadCastInfo.csv"

struct SWarBroadCastInfoScheme
{
	// 序列号
	int nIndex;

	// 我方音效ID	
	int nSelfCampSoundID;

	// 本阵营广播描述	
	char szSelfCampDes[GAME_MSGSTR_MAXSIZE];

	// 敌方音效ID	
	int nEnimyCampSoundID;

	// 敌方阵营广播描述
	char szEnimyCampDes[GAME_MSGSTR_MAXSIZE];

	SWarBroadCastInfoScheme()
	{
		memset(this, 0, sizeof(*this));
	};
};

#define MATCHRANKCONFIG_SCHEME_FILENAME "Scp\\MatchRankConfig.csv"

struct SMatchRankConfigSchemeInfo
{
    //撮合类型ID	
    EMMatchType eMatchType;
    //段位 id
    int         nGradeID;
    //保底段位标识	
    bool        bIsBaseGrade;
    //段位分数	
    int         nGradeScore;
    //系数	
    int         nKRatio;

    //段位分数	
    int         nHideGradeScore;
    //系数	
    int         nHideKRatio;

    //图标ID	
    int         nIconID;
    //升级动画ID	
    int         nRaiseAnimationID;
    //降级动画ID	
    int         nFallAnimationID;
    //匹配加成界限
    int         nMatchAddLimit;
    //匹配加成值
    int         nMatchAddValue;
    
    //描述
    char        szDes[GAME_MSGSTR_MAXSIZE];

    // 星级分段
    int         nStarSection;
    //王者段位标识
    int         bKingFlag;
    //王者排名区间
    int         nKingOrderSection;

    SMatchRankConfigSchemeInfo()
    {
        memset(this, 0, sizeof(*this));
    };
};

#define COMPUTERRANKCONFIG_SCHEME_FILENAME "Scp\\ComputerRankConfig.csv"

struct SComputerRankConfigSchemeInfo
{
    int     ID;
    BYTE    bySex;
    int     nMatchRankGrade;
    float   fWinRate;
    int     nMatchNum;

    SComputerRankConfigSchemeInfo()
    {
        memset(this, 0, sizeof(*this));
    };
};


#define WEEK_DAY_COUNT_MAX 7


#define MATCHROOM_RANK_HERO_LIMITE_SCHEME_FILENAME "Scp\\MatchRankLimitHero.csv"

struct SMatchRankLimitTimeSchemeInfo
{
    int nDay;            // 每周周几
    int nBeginTrigger; // 开始日期触发器
    int nBeginHour;  // 几点开始
    int nBeginMinute;

    int nEndTrigger; // 结束日期触发器
    int nEndHour;    // 几点结束
    int nEndMinute;

    int nFailedDesID;   // 失败描述ID
    int nBeginDesID;    // 开始描述ID
    int nEndDesID;      // 结束描述ID

    SMatchRankLimitTimeSchemeInfo()
    {
        memset(this, 0, sizeof(*this));
    };
};


#define MATCHROOM_RANK_TIME_LIMITE_SCHEME_FILENAME "Scp\\MatchRankLimitTime.csv"


struct SMatchRankLimitHeroSchemeInfo
{
    int     nMatchType;         //比赛类型
    int     nLimitHeroCount;    //限制的英雄数量
    int     nLimitHeroStarLevel;//限制的英雄星级
    int     nLimitPlayerLevel;//限制的英雄星级
    
    int     nOtherFailedDesID;  //其他人失败描述ID
    int     nSelfFailedDesID;   //自己失败描述ID

    int     nOtherLevelFailedID;  //其他人等级失败描述ID
    int     nSelfLevelFailedDesID;   //自己失败等级描述ID

    SMatchRankLimitHeroSchemeInfo()
    {
        memset(this, 0, sizeof(*this));
    };
};



#define MATCHCOMPUTERNAME_SCHEME_FILENAME "Scp\\MatchComputerName.csv" 

struct SMatchComputerName
{
    //撮合类型ID	
    int         nID;

    //描述
    char        szName[ACTORNAME_MAXSIZE];

    SMatchComputerName()
    {
        memset(this, 0, sizeof(*this));
    };
};


// 比赛电脑名称配置
class ISchemeMatchComputerName
{
public:

    virtual string GetComputerNameByIndex(int nIndex) = 0;

    virtual int GetConfigNameCount() = 0;
};


#define MATCHPLAN_SCHEME_FILENAME "Scp\\SchemeMatchPlan.csv" 
enum stageFlag
{
    EMP_FirstStage = 1,
    EMP_EndStage
};

struct SSchemeMatchPlan
{
    int nStageID;               // 阶段ID
    int nMatchType;             //比赛类型	
    int nMatchFriendOrOpposite;  //匹配类型(1、匹配队友 2、匹配敌方)
    int nWaitSeconds;           //等待时间（秒）
    int nCountUpperLimit;       //队友人数上限	
    int nCountLowerLimit;       //队友人数下限
    int nRankUpperLimit;        //匹配段位上限	
    int nRankLowerLimit;        //匹配段位下限
    int nNextStageID;           // 下一阶段ID 
    bool bIsFirstStage;         // 是否是初始阶段
    bool bIsEndStage;           // 是否是初始阶段
	int nFirstRecommendTime;	// 首次推荐等待时间
	int nRecommendGapTime;		// 推荐时间间隔
	int nRecommendActorCount;	// 单次推荐人数

    int nStartPlayerCount;      // 开启人数
    int nDelayStartTime;          //进入战场延时
    int nStartLowestPlayerCount;  // 本阶段开始最低人数
    SSchemeMatchPlan()
    {
        memset(this, 0, sizeof(*this));
    };
};




#define MATCHPLANRATIO_SCHEME_FILENAME "Scp\\SchemeMatchPlanRatio.csv" 

struct SSchemeMatchPlanRatio
{
    //比赛类型
    int nMatchType;//比赛类型
    int nBaseContinueWin;//保底连胜场次
    int nContinueWinRatio;//连胜附加系数
    int nBaseContinueLose;//保底连负场次
    int nContinueLoseRatio;//连负附加系数

    SSchemeMatchPlanRatio()
    {
        memset(this, 0, sizeof(*this));
    };
};

// 比赛大厅房间配置接口
class ISchemeMatchPlan
{
public:
    virtual SSchemeMatchPlan*			getMatchPlanByWaitSecond(int nMatchType, int nMatchFriendOrOpposite, DWORD nWaitSeconds) = 0;

    virtual SSchemeMatchPlan*			getSchemeFristMatchPlan(int nMatchType, int nMatchFriendOrOpposite) = 0;

    virtual SSchemeMatchPlan*			getSchemeMatchPlan(int nMatchType,int nStageID) = 0;

    virtual SSchemeMatchPlanRatio*      getSchemeMatchPlanRatio(int nMatchType) = 0;
};

// 排位赛季时间配置
#define MATCH_SEASON_TIME_SCHEME_FILENAME "Scp\\SchemeMatchSeasonTime.csv"

struct SSchemeMatchSeasonTime 
{
	int nIndex;
	char szName[GAME_NAME_MAXSIZE];
	time_t tBeginTime;
	time_t tEndTime;
	int nPrizeConfigID;

	SSchemeMatchSeasonTime()
	{
		memset(this, 0, sizeof(*this));
	}
};

class ISchemeMatchSeasonTime
{
public:
	// 获取某赛季配置
	virtual SSchemeMatchSeasonTime*		getSchemeMatchSeasonTimeByIndex(int nIndex) = 0;

	// 获取某时间所在的赛季配置
	virtual SSchemeMatchSeasonTime*		getSchemeMatchSeasonTimeByTime(time_t tTime) = 0;

	// 获取离当前时间最近的前一个开始时间所在配置
	virtual SSchemeMatchSeasonTime*		getSchemeMatchSeasonTimeByLastBeginTime() = 0;

	// 获取离当前时间最近的前一个结束时间所在配置
	virtual SSchemeMatchSeasonTime*		getSchemeMatchSeasonTimeByLastEndTime() = 0;

	// 获取离当前时间最近的下一个开始时间所在配置
	virtual SSchemeMatchSeasonTime*		getSchemeMatchSeasonTimeByNextBeginTime() = 0;

	// 获取离当前时间最近的下一个结束时间所在配置
	virtual SSchemeMatchSeasonTime*		getSchemeMatchSeasonTimeByNextEndTime() = 0;

};

// 排位赛季奖励配置
#define MATCH_SEASON_PRIZE_SCHEME_FILENAME "Scp\\SchemeMatchSeasonPrize.csv"

struct SSchemeMatchSeasonPrize 
{
	int nPrizeConfigID;
	char szSeasonDesc[GAME_NAME_MAXSIZE];
	int nRankGrade;
	char szRankDesc[GAME_NAME_MAXSIZE];
	int arrNormalPrize[MATCH_SEASON_PRIZE_COUNT];
	int arrExtraCondition[MATCH_SEASON_PRIZE_COUNT];
	int arrExtraPrize[MATCH_SEASON_PRIZE_COUNT];
	int nAccountScore;

	SSchemeMatchSeasonPrize()
	{
		memset(this, 0, sizeof(*this));
	}
};

class ISchemeMatchSeasonPrize
{
public:
	virtual SSchemeMatchSeasonPrize*	getSchemeMatchSeasonPrize(int nPrizeID, int nRankGrade) = 0;
};