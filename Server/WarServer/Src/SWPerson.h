/*******************************************************************
** 文件名:	SWPerson.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	28/2/2017
** 版  本:	1.0
** 描  述:	战场中玩家信息

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "War_ManagedDef.h"

//////////////////////////////////////////////////////////////////////////
// 死亡记录
typedef vector<int> DeadVect;
// 战斗受到伤害列表
typedef map<UID, DWORD> MapGetDamageVect;
// 死亡时周围敌方玩家列表
typedef map<UID, bool> MapDeadPersonAroundEnemy;
// 连杀列表, 最后一个为当前的连杀个数。 其他的历史连杀
typedef list<int> ContinuKillList;
// 团控人数量 :记录多次团控制人数
typedef list<int> TeamControlList;
// 增益Buf助攻列表
typedef map<UID, DWORD> MapAssistBuffVect;


// 战场中玩家信息
struct SWPerson
{
	// 战斗时受到的伤害（回程清零，死亡复活清零）
	MapGetDamageVect  GetDamageVect;
	// 死亡时周围的敌方玩家
	MapDeadPersonAroundEnemy  DeadPersonAroundEnemy;
	// 增益Buff助攻列表
	MapAssistBuffVect AssistBuffVect;
	// 连杀: 死亡连杀， 死亡，记录下一个
	ContinuKillList deadContKillList;
	// 多杀: nTimeContKill 时间归零，记录一次多杀
	ContinuKillList timeContKillList;
	// 团控人数量 :记录多次团控制人数
	TeamControlList teamControlList;
	// 死亡: 连死，记录， 最后一个记录，是当前连死记录。 击杀人后，记录下一个
	DeadVect deadVect;
	// 基因信息
	SGenicInfo GenicList[GENIC_MAX_SIZE];

	////////////////////////在后面放普通类型变量/////////////////////////////////

	// 击杀
	//int nKill;
	// 死亡次数
	int nDeadNum;
	// 当前连杀数量
	int nDeadContKill;
	// 当前不死连助攻
	int nAliveContAssist;
	// 战斗连杀
	int nTimeContKill;
	// 上次击杀的时刻（用于计算连杀）
	DWORD dwTimeContKilltick;
	// 当前连死数量
	int nDeadCount;
	// 自身恢复+吸血的治疗量
	int nRecoverNum;
	// 产生的物理攻击力
	int nOutHeroPhysics;
	// 产生的魔法攻击力
	int nOutHeroMagic;
	// 产生的真实攻击力
	int nOutHeroTrue;
	// 承受的魔法伤害
	int nBearMagic;
	// 承受的物理伤害
	int nBearPhysic;
	// 承受的真实伤害
	int nBearTrue;
	// 最后被哪个英雄攻击(判定被怪物击杀之后的归属)
	UID		uLastAttackUID;
	// 记录整个过程中的连杀最多数：  死亡连杀
	int nDeadKill;
	// 记录整个过程中的连杀最多数：  时间多杀
	int nTimeKill;
	// 记录整个过程中的连助攻最多数
	int nAliveContAssistMax;
	// 击杀野怪的数量
	int nFieldMonster;
	// 击杀的策略野怪数量(大龙)
	int nBigBossMonster;
	// 击杀的策略野怪数量(小龙)
	int nSmallBossMonster;
	// 最高暴击伤害
	int nMaxCritical;
	// 总的死亡时间(单位秒)
	int nSumDieTime;
	// 摧毁水晶的数量
	int nCrystalNum;
	// 是否第一滴血
	bool bFirstBlood;
	// 恢复的生命值(自己的)
	int nRecoveryHP;
	// 玩家战绩的基本数据
	int nBaseRecord[EDT_MAX];


	// 账号ID
	DWORD dwUserID;
	// pdbid
	PDBID	uPDBID;
	// 玩家名字
	char szName[ACTORNAME_MAXSIZE];
    // 玩家阵营
    int  nCamp;
    // 是否是电脑
    bool bIsComputer;
    // 玩家序号位
    int nIndex;
    // 玩家ID
    UID uID;
    // clientID
    ClientID ulClient;
    // 英雄ID
    int nHeroID;
    // 英雄皮肤ID
    int nHeroSkinID;
    // 玩家等级
    int nLevel;
    // 经验
    int nExperien;
    // 当前天赋点
    int nCurselTalent;
	// 源场景ID
	int nSrcSceneID;
	// 源世界ID
	int nSrcWorldID;
	// // 迁出坐标
	Vector3 vPosition; 
	// 迁出朝向
	Vector3 vAngle;
	// 召唤师技能
	int  nSummerID;
	// XP技能
	int  nXPSkillID;
    // 英雄星级
    BYTE byHeroStarLv; 
	// 英雄星级（参与计算属性）
	BYTE byHeroStarLvEx;
    // 英雄外部静态等级
    BYTE byHeroStaticLv;
	// 是否在Game中
    bool bIsOnGame;
    // todo 英雄死亡，对其造成伤害的英雄信息
    // 是否加载完成，所有玩家加载完成后，开始游戏
    bool bIsLoad;
    // 生命状态
	ELifeState lifeState;
	// 假死状态时候的凶手
	UID nMurderOfFakeDead;
	bool bIsKillSelf;
    //bool bIsDead;
    // 死亡倒计时
    int nDeadTime;
	// 当前正在使用的读条类型
	EPlayerProgressType eProgressType;
	// 物品列表
	int  nGoodList[MAX_GOODS_SLOT_COUNT];
	// 比赛结果类型
	BYTE byResultType;
	// 玩家当前激活的天赋<等级,天赋ID>
	int TalentList[MAX_ACTIVE_TALENT_COUNT];
	// 挂机次数
	int nBottingCount;
    // 初始天赋点添加
    bool bIniTalentPointAdd;

    // 初始经验添加
    bool bIniExpAdd;

    //是否在战场中
    bool bInWar;
	// 本局获得的撮合匹配类型得分
	int nGetMatchTypeScore;
	// 原来的撮合匹配类型得分
	int nPreMatchTypeScore;
	// 原来的撮合匹配等级
	int nPreMatchTypeGrade;
    // 玩家性别
	BYTE byActorSex;
	// AIID
    int nAIID;
	// 玩家类型
	BYTE byPlayerType; //EPlayerType

	int nKillTimeTitle;         // 最高连杀数称号
	int nKillDeadTitle;         // 最高不死连杀数称号
	int nTitle;                 // 获得的称号
	int nComprehensiveScore;    // 综合评分

    int nHideRank;              // 隐藏分

    SWPerson()
    {
		// 初始化该类的所有变量
		memset(&nDeadNum, 0, sizeof(*this) - ((char*)&nDeadNum - (char*)this));
		lifeState = ELS_Alive;

		// 不死连杀清空
		deadContKillList.clear();
		deadContKillList.push_back(0);

		// 战斗连杀
		timeContKillList.clear();
		timeContKillList.push_back(0);

		// 团控次数
		teamControlList.clear();
		teamControlList.push_back(0);

		GetDamageVect.clear();
		deadVect.clear();
		DeadPersonAroundEnemy.clear();
		AssistBuffVect.clear();
    };
};
