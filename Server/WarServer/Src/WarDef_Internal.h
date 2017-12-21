/*******************************************************************
** 文件名:	WarDef_Internal.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/10/31
** 版  本:	1.0
** 描  述:	战场内部结构使用定义

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "Base.h"
#include "SchemeDef.h"
#include "WarDef.h"
#include "War_ManagedDef.h"
#include "SWPerson.h"
#include "GameDef.h"
#include "GatewayServiceDef.h"

using namespace War;
using namespace Gateway;


#define BCASTALLCAMP -1    // 广播所有阵营



struct SCampTeamRecordInfo
{
	int nCampOutput;            // 阵营总输出
	int nCampMoney;             // 阵营总金币
	int nCampKillNum;			// 阵营击杀总数
	int nCampDeadNum;			// 阵营死亡总数
	int	nCampBearDamage;		// 阵营承受伤害

	SCampTeamRecordInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};



typedef std::list<int> EcmExpSitnList;
struct SWarEcmExpSituationInfo
{
	EcmExpSitnList ecmExpSitnList;          // 经济经验优劣势标志位列表
	int	nEcmExpSitn;			            // 当前优劣势标志位 
	float fEcmExpProList[(int)EWEEST_MAX];  // 优劣势比例
	SWarEcmExpSituationInfo()
	{
		ecmExpSitnList.clear();
		nEcmExpSitn = 0;
		fill_n(fEcmExpProList, (int)EWEEST_MAX, 0.0f);
	}
};

// 勋章信息
struct Medal
{
	// 勋章ID
	int nID;
	// 勋章类型 todo
	Medal()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 怪物死亡信息
struct CreateMonsterInfo
{
	// 怪物处理类型
	EWarMonsterExec eType;
	// 静态怪物ID 
	int nMonsterID;
	// WarPoint.csv的位置索引字段
	int nLocalIndex;
	// WarPoint.csv的类型ID
	int nRefreshIndex;
	// 是否随机刷怪
	bool bRandomRefresh;
	// 创建时间（毫秒）
	DWORD dwCreateTick;


	CreateMonsterInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 战场中玩家信息 玩家UID和战场信息
typedef std::vector<SWPerson> WarPersonVector;
// 战场阵营信息
struct SWarCamp
{
	// 阵营ID
	int nCampID;
	// 复活点
	std::vector<SPosWithAngle> vecRebornPos;
	// 当前阵营的等级
	int nLevel;
	// 经验池中的经验
	int nExperien;
	// 当前阵营的玩家
	WarPersonVector warPersVect;
	// 真人玩家个数
	int nRealPlayerCount;
	// 假人玩家个数
	int nAIPlayerCount;
	// 全部玩家的匹配分
	int nTotalMatchScore;
	// 真人玩家的总匹配分
	int nMatchScoreRealPlayer;
	// 所有玩家是否都死亡
	bool isAce;
	// 总击杀
	int nTotalKillNum;

	SWarCamp()
	{
		nCampID = 0;
		nLevel = 0;
		nExperien = 0;
		nRealPlayerCount = 0;
		nAIPlayerCount = 0;
		nTotalMatchScore = 0;
		nMatchScoreRealPlayer = 0;
		isAce = false;
		nTotalMatchScore = 0;
	};
};

struct SPendingCreateActor
{
	ClientID ulClient;
	PDBID	uPDBID;
	char szName[256];
	int nHeroID;
	int nHeroSkinID;
	int nCamp;
	int nIndex;
	bool bCreated;
	int nSrcWorldID;
	int nSrcSceneID;
	Vector3 vPosition;
	Vector3 vAngle;
	int     nSummonerSkillId[SUMMONER_SKILL_COUNT];
	bool    bIsComputer;
	BYTE    byHeroStarLv;                             // 英雄星级
	BYTE    byHeroStaticLv;                            // 英雄等级
	BYTE    byActorSex;                             //玩家性别

	SPendingCreateActor() : bCreated(false)
	{
		memset(this, 0, sizeof(*this));
	}
};

typedef std::map<UID, CreateMonsterInfo> CreateMonsterMap;

// 战场阵营信息, 根据阵营信息
typedef std::map<int, SWarCamp> WarCampMap;

// 怪物的处理列表
class IMonsterExec;
typedef std::map<EWarMonsterExec, IMonsterExec *> MonsterExecMap;


struct STLearnKillInfo
{
	DWORD			dwSkillId;		// 技能id
	DWORD			dwFreezeId;		// 冷却id
	SLOT_TYPE		nSlotType;		// 技能槽位类型
	UID				uId;			// 角色pid

	STLearnKillInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};


struct SWarInfo
{
	// 本局是否满员
	bool					isPlayerFull;
	DWORD                   dwContinueTime;                                     // 总的比赛时间(单位为秒)
	bool                    bIsFinish;                                        // 比赛是否结束
	DWORD                   BeginTime;                                        // 战场创建时间(单位为秒)
	DWORD                   EndTime;                                        // 战场结束时间(单位为秒)
	EWarPhase               WarPhase;                                         // 战场状态
	int                     nStartTimeTick;                                   // 战场正式开始时间（从第一个玩家进入战场开始计算）
	int                     nSceneID;                                         // 战场场景ID
	bool                    bIsStart;                                         // 战场是否已开启
	int                     nPersonNum;                                       // 进入的玩家个数
	bool                    bNormalEnd;										// 是否正常结束
	LONGLONG                dwDBWarID;                                        // 战场数据库唯一id
	int						nWinCamp;
	EWarEndReason			EndWarReason;
	bool					bIsWarShowAddEXPAndMoney;                         // 战场经验天赋打印开关
	int						nCurEnterWarCount;
    int                     nTotalCampCount;                                    // 最大阵营数量
	SWarInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};