/*******************************************************************
** 文件名: BattleWar.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.q1.com)
** 创建人: 李有红
** 日  期: 2017/10/31
** 版  本: 1.0
** 描  述: 大逃杀战场
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#pragma once
#include "War.h"
#include "WarDef_Internal.h"
#include <map>

using namespace War;
using namespace std;

class CBattleWarSafetyZone;

// 战场，保存战场中的信息和对战场中的处理
// 1、战场数据更新
// 2、战场的广播
// 3、同步战场信息到客户端
// 4、继承事件处理其，A.用于记录玩家的事件(死亡，攻击等)

class CBattleWar : public CWar, public IEventExecuteSink, public TimerHandler
{
public:
	CBattleWar();
	~CBattleWar();

public:
	/////////////////////////////////////IEventExecuteSink///////////////////////////////////
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen) override;

	////////////////////TimerHandler///////////////////////////////
	virtual void OnTimer(unsigned long dwTimerID) override;

public:
	///////////////////////////////IWar Start///////////////////////////////////////////
	// 初始化
	virtual bool    create(IWarService* pWarService, SWarCreateContext& context) override;

	virtual void    Release() override;

	// 初始化
	virtual bool    Init() override;

	// 反初始化
	virtual void    UnInit()  override;

	// 战场基地被摧毁
	virtual void    End() override;

	// client 的网络消息
	virtual bool    onWarClientMsg(UID uidActor, BYTE byKeyAction, const char* pData, size_t nLen) override;

	// 战场正式开始
	virtual void	onWarRealStart() override;

	// 初始化子功能
	virtual bool	InitSubFunc() override;

	// 强制战场结束
	virtual void    forceEndWar(int nCamp, bool bNormalEnd) override;

	// 设置玩家复活信息
	virtual void	setReliveInfo(sEntityDieInfo entityDieInfo, SEntityReliveInfo& info) override;

	// 玩家死亡处理
	virtual void	onEntityDieEx(sEntityDieInfo entityDieInfo, SEntityReliveInfo& info) override;

	// 救治队友
	virtual void	cureMember(UID uid) override;
	///////////////////////////////IWar End///////////////////////////////////////////

	///////////////////////////////IMiscManager Start///////////////////////////////////////////
    // 战场结束增加个人玩家信息
	virtual void addPlayerData() override;

	// 开启流程定时器
	virtual void setBcastProcessTimer() override;

	// 增加玩家经验
	virtual void addPersonExp(UID uid, int nAddExp, int nExpDropType) override;

	// 增加玩家天赋点
	virtual void addPersonTalentPoint(UID uid, int nTalentPoint, int nTalentDropType) override;

	// 假死者复活
	virtual void onFakeDeadRelive(UID uid) override;
	///////////////////////////////IMiscManager End///////////////////////////////////////////

	///////////////////////////////CWar Start//////////////////////////////////////////////////
	// 战斗受到的伤害
	virtual void onGetDamage(event_entity_damage damage) override;
	// 续玩
	virtual void onPlayerContinuePlay(event_entity_continue_play *pEvent) override;
	/////////////////////////////////CWar End///////////////////////////////////////////////////////

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
	// 更新人生命状态
	void onUpdatePersonLifeState(SWPerson* pPerson);
	// 有阵营团灭
	void onAce(int nCamp);
	// 玩家死亡（英雄击杀）
	void onPlayerDie(const sEntityDieInfo& entityDieInfo);
	// 玩家死亡（怪物击杀）
	void onPlayerDieByMonster(const sEntityDieInfo& entityDieInfo);
	// 玩家死亡（被自己击杀）
	void onPlayerDieBySelf(const sEntityDieInfo& entityDieInfo);
	// 人物假死
	void onPlayerFakeDie(UID uidTarget, UID uidMuder);

private:
	// 检测是否游戏结束
	void checkGameOver();

	// 获取剩余存活阵营数
	int getAliveCampCount();

	// 获取阵营杀人数
	int getCampKillNum(int nCamp);
	
	// 获取阵营存活人数
	int getAliveNum(int nCamp);

	// 获取总共剩余人数
	int getAliveTotalNum();
private:
	// 发送广播
	void sendImportDieInfoToClient(UID uidTarget, UID uidMuder);

private:
	// 玩家主动退出
	void onClientReqExitWar(UID uidActor);

private:
	map<UID, STLearnKillInfo> m_mapLearnSkillInfo;		    // key为角色pid

private:
};

