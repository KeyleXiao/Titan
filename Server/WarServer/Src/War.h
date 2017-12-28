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
#include "IWar.h"
#include "EntityDef.h"
#include "SWPerson.h"
#include "WarBase.h"
#include <map>
#include <vector>
#include <list>
#include "EffectCommon_ManagedDef.h"
#include "BuffDef.h"
using namespace War;
using namespace std;

struct CreateMonsterInfo;
struct SWarCamp;
class CEconomyControl;
class CSupport;
class CSoliderVigour;
class CLvExpControl;
class CXPSkill;
class CForceEnd;
class CWarAddBuff;
class CWarCheckBadBehavior;
class CWarLogRecord;
class CAIEcmCtrl;
class CAIExpCtrl;
class CWarDrop;

// 战场，保存战场中的信息和对战场中的处理
// 1、战场数据更新
// 2、战场的广播
// 3、同步战场信息到客户端
// 4、继承事件处理其，A.用于记录玩家的事件(死亡，攻击等)
class CWar : public CWarBase, public IWarMiscManager
{
public:
	CWar();
	virtual ~CWar();

public:
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	virtual void OnTimer(unsigned long dwTimerID);

public:
	////////////////////////////////IWar Start/////////////////////////
	// 初始化
	virtual bool Init() override;

	// 反初始化
	virtual void UnInit() override;

	// 开启战场
	virtual void Start() override;

	// 结束战场
	virtual void End() override;

	// 释放战场
	virtual void Release() override;

	// 销毁战场
	virtual void destroy() override;

	// 战场正式开始
	virtual void onWarRealStart() override;

	// 初始化子功能
	virtual bool InitSubFunc() override;

	// 所有的玩家都已进入战场
	virtual void onAllActorEnter() override;

	// 初始化
	virtual bool create(IWarService* pWarService, SWarCreateContext& context) override;

	// 把客户端添加进战场
	virtual bool ClientPrepareEnterWar(const SActorPreaperEnterWar& sActorPreaperEnterWar) override;

	// 玩家进入战场事件
	virtual void onActorEnterWar(const SActorEnterWarBaseInfo& sActorEnterWarBaseInfo) override;

    // 设置杯赛信息
    virtual void setLegendCupInfo(const SMsgLegendCupRoomInfo& sLegendCupInfo) override;

	/** 序列化战场内玩家的信息
	@param uid		: 玩家UID
	@param out		: 序列化的数据
	@param flag		: 序列化标识
	*/
	virtual void deSerialzed(UID uid, rkt::obuf & out, int flag) override;

	// 强制结束战场
	virtual void forceEndWar(int nCamp, bool bNormalEnd) override;

	// client 的网络消息
	virtual bool onWarClientMsg(UID uidActor, BYTE byKeyAction, const char* pData, size_t nLen) override;

	// 怪物死亡处理
	virtual void onEntityDie(sEntityDieInfo entityDieInfo) override;

	// 玩家死亡处理
	virtual void onEntityDieEx(sEntityDieInfo entityDieInfo, SEntityReliveInfo& info) override;

	// 获取杂项管理器
	virtual IWarMiscManager* getMiscManager()  override { return this; }

    // 获取最大的阵营数
    virtual BYTE getMaxCamp() override;

	/** 获取战场指定类型UID
	@param byGetCampType		: 阵营类型 EWarCampType
	@param bySerchType		    : 查找类型 EWarMonsterExec
	*/
	virtual DWORD warSerchTypeUIDList(BYTE bySelfCamp, BYTE byGetCampType, BYTE bySerchType, PDBID* pReturnArray, DWORD dwArrayMaxSize) override;

	/** 指定类型buff处理
	@param bySelfCamp           : 自己的阵营
	@param byGetCampType		: 阵营类型 EWarCampType
	@param bySerchType		    : 查找类型 EWarMonsterExec
	*/
	virtual void warAddSerchTypeBuff(SWarEffectAddBuff sData) override;

    /////////////////////////////////IWar END/////////////////////////////////////////

    /////////////////////////////////IWarMiscManager Start/////////////////////////////////////////

    // 击杀重要实体广播（人物，boss,塔）
    virtual void sendToClientImportantEntityDie(const msg_war_important_entiy_dieinfo& EntiyDieInfo, const vector<msg_war_entity_assist_info> & assistList) override;

    // 重要怪物重生广播（boss,塔）
    virtual void sendToClientImportantMonsterRelive(msg_war_important_monster_reliveinfo MonsterReliveInfo) override;

    // 重要怪物重生提前广播（boss,塔）
    virtual void SendToClientImportantMonsterEarlyRelive(msg_war_important_monster_reliveinfo MonsterReliveInfo) override;

    // 怪物死亡文字类型广播
    virtual void sendToClientMonsterDieBroadText(msg_war_important_monster_dietxtinfo MonsterDieTextInfo) override;

    // 不死连杀 战斗连杀
    virtual void sendToClientKillCountInfo(const msg_war_entity_kill_countinfo& EntiyDieInfo, const vector<msg_war_entity_assist_info> & assistList) override;

    // 实体死亡响应事件
    virtual void onEntityDieBroadCast(sEntityDieInfo entityDieInfo, int nBroadCastIndex = 0) override;

    virtual const SchemeWarPersonConfigInfo * getPersonConfigInfo() override;

	// 获取敌对阵营
	virtual int getEnemyCamp(int nSelfCamp) override;

    virtual DWORD getCampMatchTypeAverageScore(int nCamp, bool bIncludeAI = true) override;

	// 获取怪物信息
	virtual bool isHaveComputer()  override { return m_bHaveComputer; };

	// 某个阵营是否有真人玩家
	virtual bool isHaveRealPlayer(int nCamp) override;

	// 设置失败方水晶位置
	virtual void setFailedCampBasePos(Vector3 v) override;

	// 设置阵营水晶UID
	virtual void setCampBaseUID(int nCamp, UID uid) override;

	// 获取阵营Ban掉的英雄
	virtual void getCampBanHero(int nCamp, WORD banList[5], size_t banNum) override;

	// 获取所有被Ban掉的影响
	virtual void getAllBanHero(WORD *banList, size_t banNum) override;

    // 设置玩家复活信息
	virtual void setReliveInfo(sEntityDieInfo entityDieInfo, SEntityReliveInfo& info) override;

	// 更新战场结束原因
	virtual void updateWarEndReason() override;

	// 获得某个阵营玩家数量
	virtual int getCampPlayerCount(int nCamp, bool bIncludeComputer = true) override;

	// 获取某个阵营的平均经济
	virtual int getAvgEmcByCamp(int nCamp, bool bInlucdeComputer = true) override;

	// 获取某个阵营的最高经济
	virtual int getEcmMaxByCamp(int nCamp) override;

	// 获取某个阵营的平均经验
	virtual int getAvgExpByCamp(int nCamp, bool bInlucdeComputer = true) override;

	// 获取阵营战斗记录信息
	virtual SCampTeamRecordInfo* getCampRecordInfo(int nCamp) override;

	// 获得综合得分之一 击杀破塔得分
	virtual float getKillAndDestroyTowerScore(const SWPerson& swPerson) override;

	// 获得综合得分之二 团队贡献得分
	virtual float getTeamContributionScore(const SWPerson& swPerson) override;

	// 获得综合得分之三 特殊贡献得分
	virtual float getSpeicalContributionScore(const SWPerson& swPerson) override;

	// 开启流程定时器
	virtual void setBcastProcessTimer()  override {};


	// 设置Ban选英雄
	virtual void setBanHeroList(char* data, size_t len) override;

	// 设置充能值
	virtual void setChargedValue(UID uidTarget, int nEffectID, int nValue) override;

	// 取得充能值
	virtual int getChargedValue(UID uidTarget, int nEffectID) override;

    //得到创建的等级
    virtual int getCreateLevel(int nBaseLevel, int nMonsterRefreshBeginTime, int nMonsterUpgradeTime, int nMaxLevel) override;

	// 判断玩家阵营是否团灭  true代表团灭  false代表没有团灭
	virtual bool judgeCampIsAce(int nCamp) override;

	// 增加玩家经验
	virtual void addPersonExp(UID uid, int nAddExp, int nExpDropType) override;

	// 增加玩家天赋点
	virtual void addPersonTalentPoint(UID uid, int nTalentPoint, int nTalentDropType) override;

	// 假死者复活
	virtual void onFakeDeadRelive(UID uid) override {};
	/////////////////////////////////IWarMiscManager End/////////////////////////////////////////

public:
    // 发送数据到单个客户端
    void                        sendAllTabUpdateInfoToClient( UID uId );

    // 设置整场战斗的最高不死连杀和时间连杀
    void                        setKillMaxInfo(SWPerson* pPerson);

    // 给客户端发送消息把连杀图片显示出来
    void                        sendToClientShowKillIcon( UID uMurderId, int nKillCount );

    // 战场结束数据统计计算
    void                        calculateWarEndData();

    // 增加玩家英雄星级的属性
	void                        addHeroStarPro( UID uID, int nNewLv = 1, int nOldLv = 0 );

    // 战后判断是否达到奖励限制条件
    bool                        bIsCanReachAwarkCondition();

    // 发送战斗小地图信息
    void                        sendMiniMapFightInfo(event_entity_damage damage);

 
    // 发送战场杯赛信息数据
    void                        sendLegendCupWarEndInfo();

 protected:
	// 战场结束增加个人玩家信息
	virtual void addPlayerData();

    ///////////////////////////////////CWar///////////////////////////////////////////
    //                                战场基础事件逻辑处理                           //
    //////////////////////////////////////////////////////////////////////////////////
	// 续玩
	virtual void onPlayerContinuePlay(event_entity_continue_play *pEvent);
	// 受到治疗
	virtual void onHaveCure(CureContext* pCureContext);
    // 战斗受到的伤害
    virtual void onGetDamage(event_entity_damage damage);

	//受到伤害
	void onGetHeroDamage(const event_entity_damage& damage);

    // 战斗改变状态事件
    void onGetChangeState( event_entity_change_state damage );

    // 升级
    void onEntityUpgrade( event_entity_upgrade_level entityUpgrade );

	// 天赋点改变
	void onTalentChange(event_entity_talent_change* pTalentChange );

	void onSetGoodList(event_entity_war_good_list *pEvent);

	void onClearDeadContKill(event_entity_clear_multi_kill *pEvent);

    void onKillingControl(event_game_killingcontrol *pEvent);

    void onThrillingCure(event_game_thrillingcure *pEvent);

    void onGroupControl(event_game_groupcontrol *pEvent);

	// 激活天赋事件
	void onActiveTalent(PDBID pdbid, int nTalentID, int nLevel);

	// 添加助攻Buff事件事件
	void onAddAssistBuff(UID uid, int nBuffCount, LPCSTR pData);

    //////////////////////////////////////玩家死亡记录数据辅助函数////////////////////
    // 玩家死亡信息，添加死亡玩家的负率，添加胜利玩家的奖励
    void addPersonDeadInfo(sEntityDieInfo entityDieInfo, SEntityReliveInfo& info, bool bBcastTips = true);

    // 设置玩家累计死亡时间
	void setSumDieTime( time_t nTime,sEntityDieInfo entityDieInfo);

	///////////////////////////////////战场结束评分相关//////////////////////////////
  

	// 统计战后每个人的评分
	void statisticsPlayerGraded();

	// 设置连杀称号
	void setKillTitle(SWPerson& stWarGraded, int nTimeKill, int nDeadKill);

	// 获得各项评分数值最高值
	void getEveryEDTMaxData(map<int, SEDTPersonData> &pMaxEDTPDBID,SWPerson *pPerson);

	// 获得综合得分 返回分数 = 击杀破塔得分+团队贡献得分+特殊贡献得分
	int getComprehensivePerformanceScore(const SWPerson& swPerson );

	// 设置阵营的总伤害的平均值   总伤害＝对英雄伤害*攻击伤害系数+承受伤害
	void SetCampTeamRecordInfo();
	// 判断能够给称号
	bool canAddPersonTitle(int nEDTIndex,const SWPerson& swPerson, map<int, map<int, SEDTPersonData>>& pMaxEDTGroup);

	/////////////////////////////////////////////////////////////////////////////////
	//记录承受或造成的伤害
	void recordDamageInfo(int& nAllDamage, int& nPhysicDamage, int& nMagicDamage, int& nTrueDamage, const event_entity_damage& damage);

	// 所有玩家加钱
	void addWarTimerPersonTalentValue(int nAdd);

    // 所有玩家加经验
    void addWarTimerPersonExpValue(int nAdd);

	// 设置杯赛比赛结果
	void setLegendCupWarResult();

	// 某阵营分数KDA平均值
	int getCampAvgKDAScore(int nCamp);

	// 某阵营杯赛荣誉数据
	void setCampLegendCupGlory(int nGlory[], int nGloryLen,int nCamp, int nWinCamp);

	void updatePlayerMatchTypeInfo();

	// 设置段位得分 等级相关处理
	void setMatchTypeRankScoreAndGrade();

	// 设置排位赛季数据
	void setMatchRankSeasonInfo();

	// 玩家特殊贡献信息同步(惊现救治 夺命控制 团控)
	void BroadcastSpecialContribution(  ESpecialContributionType eType ,const SWPerson* pSWPerson);

	// 每过30秒把经济经验信息储存
	void updateWarEcmExpSituation();
	// 判断玩家本局结果类型
	void judgeWarPersenResultType();
	// 计算经验经济比例列表
	void calculateEcmExpProportionList();

	// 计算击杀大小龙数
	void calcAroundBossCount(int nCamp, UID uMuderUID, EWBossType bossType);

	// 获取复活时间
	int getHeroReliveTime(int nLevel);

    ////////////////////////////////客户端消息//////////////////////////////////////////
    // 小地图消息处理
    void onClientMsgMiniMapInfo(void* pData, size_t stLen );

	void onClientMsgUpdateTableRequest(UID uId, void* pData, size_t stLen);

protected:
	// 判断同盟中是否有同工会的人  false 代表没有同盟， true为有同盟
	bool isHaveSameClanMember(PDBID idActor);

	// 判断同盟中是否有师徒关系  false 代表没有， true为有
	bool isHaveMentorship(PDBID idActor, BYTE& nMentorshipPost);

	// 判断本场是否获得MVP
	bool isGetMVP(PDBID idActor);

	// 获取多杀最高次数
	int	getTimeKillMaxCount(PDBID idActor);
	
	// 获取玩家的n杀次数
	int getTimeKillCountOfN(PDBID idActor, int n);

	// 超神次数
	int getSuperGodCount(PDBID idActor);


private:
    // 角色加入待创建队列
    void iniWarActorInfo(const SActorPreaperEnterWar & sActorPreaperEnterWar);

	// 获取某个阵营的总经济
	int getTotalEmcByCamp(int nCamp, bool bInlucdeComputer = true);
	
	// 获取某个阵营的总经验
	int getTotalExpByCamp(int nCamp, bool bInlucdeComputer = true);

	// 战场结束，给玩家、联盟和战队增加活跃值
	void AddActivity();
	
	// 本局是否满人
	bool isPlayerFull(int nRoleCount);

	// 获取阵营所有玩家的pdbid字符串（提供给任务系统-lua层）
	const void getWarResultString(SWPerson* pPerson, char* szWarResultStr, size_t nLen, ISchemePersonModel* pPersonModel);

    // 退出压力测试账号
    void    exitTester();

	// 是否开启了恶劣行为检测
	bool	isOpenCheckBadBehavior();

	// 玩家添加buf
	void	playerAddBuff(SWarEffectAddBuff sData);

	/////////////////////////////////////////////////////// 发送数据到客户端 /////////////////////////////
private:
    // 减少社会服记录的观察期
    void warEndPunishSubObserveCount();

    // 创建OB外部监视实体
    bool createOBViewerEntity();

    // 释放OB外部监视实体
    void releaseOBViewerEntity();

	// 计算连杀
	void calcContKill();

	// 计算失败方MVP
	void calcFailedCampMvp(int nCamp, map<int, SEDTPersonData> &pMaxEDTPDBID);
protected:
	int                     m_nBroadcastProcessCount;                           // 阶段性提示广播列表
	SBroadcastProcess       m_BroadCastTimeList[MAX_WAR_BROADCAST_PROCESS];

    bool                    m_bFirstBlood;                                      // 战场总信息

    //自动添加天赋点配置
    DWORD                   m_TempAddTalentTime;
    int                     m_nAutoAddTalentInterval;
    int                     m_nAutoTalentPoint;
    int                     m_nFirstAutoAddTalentInterval;
    int                     m_nInitialTalentPoint;

    // 自动添加经验相关
    DWORD                   m_ConfigAddExpTime;
    int                     m_nFirstAutoAddExpInterval;                         // 第一次加经验时间间隔
    int                     m_nAutoAddExpInterval;                              //战场自动添加天赋点时间间隔(单位秒)
    int                     m_nInitialExp;                                      //战场初始化经验
    int                     m_nAutoAddExp;                                      //战场自动添加经验

	SWarEcmExpSituationInfo						m_EcmExpSituation;		  // 优劣势情况记录 阵营1（CAMP_BLUE）基准 经济&经验综合优劣势信息

private:
	// 连杀时间间隔
	int											m_nTimeCountKillInterval;

	// 助攻buf更新时间map
	std::map<UID, DWORD>						m_mapAssistBuf;

    bool                                        m_bHaveComputer;

    SMsgLegendCupRoomInfo                       m_sLegendCupInfo;

	// 失败方基地坐标位置
	Vector3										m_FailedCampBasePos;

	// 各阵营水晶UID
	UID											m_CampBaseUID[CAMP_MAX];

	// 各阵营的Ban选英雄列表
	vector<int>									m_CampBanHeroList[CAMP_MAX];

    // 物品充能模块记录的数据
    typedef map<int, int>                       CHARGED_EFFECT_MAP;
    typedef map<UID, CHARGED_EFFECT_MAP>        CHARGED_MAP;
    CHARGED_MAP                                 m_ChargedMap;	                //充能相关

    UID                                         m_OBViewerUID;
};