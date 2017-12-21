/*******************************************************************
** 文件名: e:\CoGame\SchemeEngine\include\ISchemeEngine.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 秦其勇
** 日  期: 2015/3/11
** 版  本: 1.0
** 描  述: 祭坛战场
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifdef MOBA_ALTAR_WAR_OPEN

#pragma once
#include <map>
#include <vector>
#include "StdAfx.h"
#include "WarDef.h"
#include "IMonsterExec.h"
#include "War.h"
#include "EntityDef.h"

#include "War_ManagedDef.h"
#include "IMigrateService.h"
#include "IDBEngineService.h"

using namespace War;
using namespace std;

// 祭坛战场，保存战场中的信息和对战场中的处理
// 1、祭坛战场数据更新
// 2、祭坛战场的广播
// 3、同步战场信息到客户端
// 4、继承事件处理其，A.用于记录玩家的事件(死亡，攻击等)

class CMobaAltarWar : public CWar
{
	struct STLearnKillInfo
	{
		DWORD			dwSkillId;		// 技能id
		DWORD			dwFreezeId;		// 冷却id
		SLOT_TYPE		nSlotType;		// 技能槽位类型
		UID				uId;			// 角色pid
	};

public:
	CMobaAltarWar();
	~CMobaAltarWar();

	///////////////////////////////CWar///////////////////////////////////////////
	// 初始化
	virtual bool    create(IWarService* pWarService, SWarCreateContext& context);

	// 初始化
	virtual bool    Init();

	// 反初始化
	virtual void    UnInit();

	// 战场基地被摧毁
	virtual void    End();

	// client 的网络消息
	virtual bool    onWarClientMsg(UID uidActor, BYTE byKeyAction, PACKAGE_PTR msg);

	// 获取不同类型怪物信息
	virtual IMonsterExec* getWarMonsterExecByType(EWarMonsterExec WarMonsterExec);

	virtual void    Realease();

	// 获得当前战场id
	virtual LONGLONG getWarDBKey() { return m_dwDBWarID;}

	//////////////////////TimerHandler///////////////////////////////
	virtual void    OnTimer( unsigned long dwTimerID );

	///////////////////////IEventExecuteSink///////////////////////////////////////////////////
	/** 容器事件触发后执行函数
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	*/
	virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	///////////////////////IDBRetSink///////////////////////////////////////////////////
	virtual void		OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen){};

	// 断开连接
	virtual void		onClientDisconnect(ClientID client,DWORD dwReason);

    // 战场结束增加个人玩家信息
	virtual void		addPlayerData();
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
    
    ///////////////////////////////////CMobaAltarWar//////////////////////////////////
    //                                战场逻辑处理                                  //
    //////////////////////////////////////////////////////////////////////////////////

    // 当实体死亡时移除xpbuff
	void onHeroDieRemovBuff( UID uid);

    // 学习技能事件处理
    void onLearnSkill( event_learn_spell* pEntityLearnSpell );

    // 当有技能冷却时
    void onSkillFreesChangeed(event_freeze_changed* pEntityFreezeChanged);

    //////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////祭坛和瞭望台 检测 控制 读条//////////////////////////
	// 客户端选取特殊实体  目前用处只有祭坛
	void onClientSelectSpecialEntity(void* pData, int stLen);
	// 判断玩家能否开始上交宝物到祭坛并开始进度读条
	bool canStartPlayerAltarProgress(UID uidActor,UID uidNpc);
	// 玩家上交宝物成功 进度条完毕
	void playerHandInAltarSuccess(int nCamp,UID uidNpc);
	// 玩家上交宝物失败 进度条打断
	void playerHandInAltarFail(  int nCamp,  UID uidNpc );
	// 获得祭坛信息
	SWarAltarInfo* getWarAltarInfo(UID uidAltar);
	
	// 客户端特殊实体范围搜索 目前用处只有瞭望台
	void onClientSpecialEntityRangeSearch( void* pData, int stLen);
	// 判断玩家能否开始占领瞭望台并开始进度读条
	bool canStartPlayerWatchOutTowerProgress( UID uidActor, UID uidNpc );
	// 玩家占领成功 进度条完毕
	void playerCaptureWatchTowerSuccess( int nCamp ,UID uidNpc) ;
	// 检测瞭望台是否可以恢复无人占领状态
	void checkWatchTowerRecover();
	// 获得瞭望台信息
	SWarWatchTowerInfo* getWarWatchTowerInfo(UID uidWatchTower);
	// 特殊实体搜索状态改变 告知客户端 目前有瞭望台实体搜索状态改变
	void SendClientSpecialEntitySearchStateChange(UID uidNpc,bool bSearch);
	
	// 开始玩家读条 根据读条类型取读条配置
	void startPlayerProgress(UID uidActor,UID uidNpc,EPlayerProgressType eType);
	// 玩家读条完毕
	void onPlayerProgressComplete(UID uidActor,UID uidNpc);
	// 玩家读条打破
	void onPlayerProgressBreak(UID uidActor,UID uidNpc);
	
	//////////////////////////////////////////////////////////////////////////

private:
	
	map<UID, STLearnKillInfo>		            m_mapLearnSkillInfo;		    // key为角色pid

    typedef map<int, int>                       CHARGED_EFFECT_MAP;
    typedef map<UID, CHARGED_EFFECT_MAP>        CHARGED_MAP;
    CHARGED_MAP                                 m_ChargedMap;	                //充能相关

	vector<SWarAltarInfo>                       m_vecAltarInfo;					// 祭坛信息
	vector<SWarWatchTowerInfo>                  m_vecWatchTowerInfo;			// 瞭望台信息
};

#endif