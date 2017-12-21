/*******************************************************************
** 文件名: e:\CoGame\SchemeEngine\include\ISchemeEngine.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 陈俊涛
** 日  期: 2015/3/11
** 版  本: 1.0
** 描  述: 战场信息
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "WarDef.h"
#include "IMonsterExec.h"
#include "War.h"
#include <map>

using namespace War;
using namespace std;

// 战场，保存战场中的信息和对战场中的处理
// 1、战场数据更新
// 2、战场的广播
// 3、同步战场信息到客户端
// 4、继承事件处理其，A.用于记录玩家的事件(死亡，攻击等)

class CXPSkill;
class CForceEnd;

class CMobaWar : public CWar, public IEventExecuteSink, public TimerHandler
{
public:
	CMobaWar();
	~CMobaWar();

public:
	/////////////////////////////////////IEventExecuteSink///////////////////////////////////
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen) override;

	////////////////////TimerHandler///////////////////////////////
	virtual void OnTimer(unsigned long dwTimerID) override;

public:
	///////////////////////////////CWar///////////////////////////////////////////
	// 初始化
	virtual bool    create(IWarService* pWarService, SWarCreateContext& context) override;

	virtual void    Release() override;

	// 初始化
	virtual bool    Init() override;

	// 反初始化
	virtual void    UnInit() override;

	// 战场基地被摧毁
	virtual void    End() override;

	// client 的网络消息
	virtual bool    onWarClientMsg(UID uidActor, BYTE byKeyAction, PACKAGE_PTR msg) override;

	// 战场正式开始
	virtual void	onWarRealStart() override;

	// 初始化子功能
	virtual bool	InitSubFunc() override;
	///////////////////////////////IWar End///////////////////////////////////////////

	///////////////////////////////IWar Start///////////////////////////////////////////
	// 战场结束增加个人玩家信息
	virtual void	addPlayerData() override;

	// 开启流程定时器
	virtual void setBcastProcessTimer() override;

	///////////////////////////////IWar End///////////////////////////////////////////

	////////////////////////////CWar Start///////////////////////////////////////
	virtual void onPlayerContinuePlay(event_entity_continue_play *pEvent) override;

	////////////////////////////CWar End///////////////////////////////////////
private:
    ///////////////////////////////////战场结束奖励相关///////////////////////////////////////

    // 设置当前使用英雄的专属天赋效果
	void setHeroExclusiveTalentEffect(UID uID);

	// 玩家是否胜利  false 为失败  true为胜利
	bool playerIsWin(PDBID idActor);

	void recordTeammateHeroID(int nCamp, PDBID idActor, int nSelfHeroID);

	// 获得某玩家一场比赛后的杀人情况
	void setPlayerKillGloryInfo(PDBID idActor, int nCamp, int* pGlory);

   // 设置玩家荣耀信息
	void setPlayerGlory(PDBID idActor);

    // 获得本场比赛英雄成就得分
    int getAchievementScore( PDBID idActor);

	// 获得战场评分奖励
	int getWarScoringRewards( PDBID idActor, const SHeroAchievementScoreConfig* pScoreConfig );

	// 胜利失败系数
	float getAchievementCoeff( int nCurrentScore, const SHeroAchievementScoreConfig* pScoreConfig );


#pragma region 随机英雄卡牌
private:
    // 返回可获得卡牌数量
    int getWarEndCardCount(PDBID idActor, int nCamp, bool bIsHaveSameClan, SEntityXml_Server* pEntityXml);
    // 获得英雄卡牌数量概率
    int getHeroCardCount(int nPlayerLv, int nWarType);

#pragma endregion 随机英雄卡牌

#pragma region 经验金币产出
public:
    // 计算战后经验得失
    void ExpAndGoldCreate(PDBID idActor, bool bIsHaveSameClan, SEntityXml_Server* pEntityXml,SExpOrGoldCreateConfig* pConfig, msg_entity_warend_awark& msgAwark);

private:
    // 获得经验 nType 查看枚举ENEXPCREATETYPE
    int getStaticExp(PDBID idActor, int nType, SExpOrGoldCreateConfig* pConfig);

    // 获得金币
    int getStaticGold( bool bIsWin, int nType = ENEXPCREATETYPE_PLAYER );
#pragma endregion 经验金币产出
    ///////////////////////////////////////////////////////////////////////////////////////////
private:
    ///////////////////////////////////CMobaWar///////////////////////////////////////
    //                                战场逻辑处理                                  //
    //////////////////////////////////////////////////////////////////////////////////

    // 当实体死亡时移除xpbuff
	void onHeroDieRemovBuff( UID uid);

    // 学习技能事件处理
    void onLearnSkill( event_learn_spell* pEntityLearnSpell );

    // 当有技能冷却时
    void onSkillFreesChangeed(event_freeze_changed* pEntityFreezeChanged);

    //////////////////////////////////////////////////////////////////////////////////

private:
	map<UID, STLearnKillInfo>		            m_mapLearnSkillInfo;		    // key为角色pid
};

