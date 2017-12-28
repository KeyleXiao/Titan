/*******************************************************************
** 文件名: e:\CoGame\SchemeEngine\include\ISchemeEngine.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 陈俊涛
** 日  期: 2015/3/11
** 版  本: 1.0
** 描  述: 战场信息
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  ljw
** 日  期: 
** 描  述:  重构代码
********************************************************************/
#pragma once

#include "EntityDef.h"
#include "IWarService.h"
#include "SchemeWarDef.h"
#include "WarMessageDef.h"
#include "IMonsterExec.h"
#include "ICamp.h"
#include "ITimeSyncService.h"
#include "IServerGlobal.h"
#include "WarDef.h"
#include "WarDef_Internal.h"

struct SWPerson;
struct CreateMonsterInfo;
struct SWarCamp;
struct SWarCreateContext;
struct SWarInfo;
struct SCampTeamRecordInfo;
class __IWarMgr;

// 战场杂项管理，先这样重构一版，后续继续优化这个里面的东西，可以再细分
class IWarMiscManager
{
public:
    // 击杀重要实体广播（人物，boss,塔）
	virtual void sendToClientImportantEntityDie(const msg_war_important_entiy_dieinfo& EntiyDieInfo, const vector<msg_war_entity_assist_info> & assistList) = 0;

    // 重要怪物重生广播（boss,塔）
    virtual void sendToClientImportantMonsterRelive(msg_war_important_monster_reliveinfo MonsterReliveInfo) = 0;

    // 重要怪物重生提前广播（boss,塔）
    virtual void SendToClientImportantMonsterEarlyRelive(msg_war_important_monster_reliveinfo MonsterReliveInfo) = 0;

    // 怪物死亡文字类型广播
    virtual void sendToClientMonsterDieBroadText(msg_war_important_monster_dietxtinfo MonsterDieTextInfo) = 0;

    // 不死连杀 战斗连杀
	virtual void sendToClientKillCountInfo(const msg_war_entity_kill_countinfo& EntiyDieInfo, const vector<msg_war_entity_assist_info> & assistList) = 0;

    // 实体死亡响应事件
    virtual void onEntityDieBroadCast(sEntityDieInfo entityDieInfo, int nBroadCastIndex = 0) = 0;

    virtual const SchemeWarPersonConfigInfo * getPersonConfigInfo() = 0;

	// 获取敌对阵营
	virtual int getEnemyCamp(int nSelfCamp) = 0;

    virtual DWORD   getCampMatchTypeAverageScore(int nCamp, bool bIncludeAI = true) = 0;

    // 战场结束增加个人玩家信息
	virtual void addPlayerData() = 0;

	// 某个阵营是否有真人玩家
	virtual bool isHaveRealPlayer(int nCamp) = 0;

	// 设置失败方水晶位置
	virtual void setFailedCampBasePos(Vector3 v) = 0;

	// 设置阵营水晶UID
	virtual void setCampBaseUID(int nCamp, UID uid) = 0;

	virtual bool isHaveComputer() = 0;

	// 获取所有被Ban掉的英雄
	virtual void getAllBanHero(WORD *banList, size_t banNum) = 0;

	// 获取阵营Ban掉的英雄
	virtual void getCampBanHero(int nCamp, WORD banList[5], size_t banNum) = 0;

	// 设置玩家复活信息
	virtual void setReliveInfo(sEntityDieInfo entityDieInfo, SEntityReliveInfo& info) = 0;

	// 更新战场结束原因
	virtual void updateWarEndReason() = 0;

	// 获得某个阵营玩家数量
	virtual int getCampPlayerCount(int nCamp, bool bIncludeComputer = true) = 0;

	// 获取某个阵营的平均经济
	virtual int getAvgEmcByCamp(int nCamp, bool bInlucdeComputer = true) = 0;

	// 获取某个阵营的最高经济
	virtual int getEcmMaxByCamp(int nCamp) = 0;

	// 获取某个阵营的平均经验
	virtual int getAvgExpByCamp(int nCamp, bool bInlucdeComputer = true) = 0;

	// 获取阵营战斗记录信息
	virtual SCampTeamRecordInfo* getCampRecordInfo(int nCamp) = 0;

	// 获得综合得分之一 击杀破塔得分
	virtual float getKillAndDestroyTowerScore(const SWPerson& swPerson) = 0;

	// 获得综合得分之二 团队贡献得分
	virtual float getTeamContributionScore(const SWPerson& swPerson) = 0;

	// 获得综合得分之三 特殊贡献得分
	virtual float getSpeicalContributionScore(const SWPerson& swPerson) = 0;

	// 开启流程定时器
	virtual void setBcastProcessTimer() = 0;

	// 设置充能值
	virtual void setChargedValue(UID uidTarget, int nEffectID, int nValue) = 0;

	// 取得充能值
	virtual int getChargedValue(UID uidTarget, int nEffectID) = 0;

	// 设置Ban选英雄
	virtual void setBanHeroList(char* data, size_t len) = 0;
	
    // 根据配置得到创建的等级
    virtual int getCreateLevel(int nBaseLevel, int nMonsterRefreshBeginTime, int nMonsterUpgradeTime, int nMaxLevel) = 0;

	// 判断玩家阵营是否团灭  true代表团灭  false代表没有团灭
	virtual bool judgeCampIsAce(int nCamp) = 0;

	// 增加玩家经验
	virtual void addPersonExp(UID uid, int nAddExp, int nExpDropType) = 0;

	// 增加玩家天赋点
	virtual void addPersonTalentPoint(UID uid, int nTalentPoint, int nTalentDropType) = 0;

	// 假死者复活
	virtual void onFakeDeadRelive(UID uid) = 0;
};

// 战场基类接口
class IWar
{
public:
	// 初始化
	virtual bool Init() = 0;

	// 反初始化
	virtual void UnInit() = 0;

	// 开启战场
	virtual void Start() = 0;

	// 结束战场
	virtual void End() = 0;

	// 释放战场
	virtual void Release() = 0;

	// 销毁战场
	virtual void destroy() = 0;

	// 战场正式开始
	virtual void onWarRealStart() = 0;

	// 初始化子功能
	virtual bool InitSubFunc() = 0;

	// 所有的玩家都已进入战场
	virtual void onAllActorEnter() = 0;

	// 是否结束
	virtual bool isEnd() = 0;

	// 初始化
	virtual bool create(IWarService* pWarService, SWarCreateContext& context) = 0;

	// 获取战场所有人物UID
	virtual int getAllPerson(UID *PersonArray, int nArraySize) = 0;

	// 获取场景ID
	virtual int getWarSceneID() = 0;

	// 获得战场类型ID
	virtual int getWarTypeId() = 0;

	virtual int getStaticID() = 0;

	// 获得战场的创建序列号 创建怪物使用
	virtual int getMapID() = 0;

	virtual int getWarID() = 0;

	virtual DWORD  getMatchTypeID() = 0;

	// 获得战场持续的时间
	virtual DWORD getWarContinueTime() = 0;

	// 把客户端添加进战场
	virtual bool ClientPrepareEnterWar(const SActorPreaperEnterWar& sActorPreaperEnterWar) = 0;

	// 玩家进入战场事件
	virtual void onActorEnterWar(const SActorEnterWarBaseInfo& sActorEnterWarBaseInfo) = 0;

	// 设置杯赛信息
	virtual void setLegendCupInfo(const SMsgLegendCupRoomInfo& sLegendCupInfo) = 0;

	// 广播玩家信息
	virtual bool broadCastDataToAllPlayer(BYTE byKeyAction, LPCSTR pszData, int nDataLen, int nCamp = BCASTALLCAMP, bool bToView = false) = 0;

	// 广播给其他阵营玩家 除了 nCamp 阵营 其他全部广播
	virtual bool broadCastDataToAllOtherCamp(BYTE byKeyAction, LPCSTR pszData, int nDataLen, int nCamp = BCASTALLCAMP, bool bToView = false) = 0;

	// 广播给其他同阵营玩家
	virtual bool broadCastDataToSameCamp(BYTE byKeyAction, LPCSTR pszData, int nDataLen, int nCamp, bool bToView = false) = 0;

	// 广播给有视野的玩家信息
	virtual bool broadCastDataToHaveSightPlayer(Vector3 vec, BYTE byKeyAction, LPCSTR pszData, int nDataLen, bool bToView = false) = 0;

	// 玩家信息广播到客户端
	virtual void bcastPersonInfo(UID uid, EWarPersonInfoUpdateType nUpdateType) = 0;

	// 发送数据到玩家客户端
	virtual void sendDataToClient(UID uid, BYTE byKeyAction, void* data, size_t len, bool bToView=false) = 0;

	virtual void setWinCamp(int nCamp) = 0;

	virtual int getWinCamp() = 0;

	// 战场游戏时间（正式开始后计时）
	virtual DWORD getWarPlayTime() = 0;

	/** 序列化战场内玩家的信息
	@param uid		: 玩家UID
	@param out		: 序列化的数据
	@param flag		: 序列化标识
	*/
	virtual void deSerialzed(UID uid, rkt::obuf & out, int flag) = 0;

	// 获得战场DBKey
	virtual LONGLONG getWarDBKey() = 0;

	// 强制结束战场
	virtual void forceEndWar(int nCamp, bool bNormalEnd) = 0;

	// 获取上下文
	virtual SWarCreateContext* getWarCreateContext() = 0;

	// 获取战场信息
	virtual SWarInfo* getWarInfo() = 0;

	// 根据uPBBID获得玩家当前的信息
	virtual SWPerson* getPlayerInfoByPDBIU(PDBID uPBBID) = 0;

	virtual int getPlayTimeMin() = 0;

	// 获取阵营列表
	virtual vector<int> getCampList() = 0;

	// 根据阵营ID获得阵营信息
	virtual SWarCamp* getWarCamp(int nCamp) = 0;

	// 根据uid获得玩家当前的信息
	virtual SWPerson* getPlayerInfo(UID uid) = 0;

	// client 的网络消息
	virtual bool onWarClientMsg(UID uidActor, BYTE byKeyAction, const char* pData, size_t nLen) = 0;

	// 获得胜利条件
	virtual EWarEndReason getWarEndReason() = 0;

	// 设置结束原因
	virtual void setWarEndReason(EWarEndReason eReason) = 0;

	// 战场正常结束标示
	virtual void setNormalEnd(bool bNormalEnd) = 0;

	// 怪物死亡
	virtual void onEntityDie(sEntityDieInfo entityDieInfo) = 0;

	// 玩家死亡
	virtual void onEntityDieEx(sEntityDieInfo entityDieInfo, SEntityReliveInfo& info) = 0;

	// 玩家复活
	virtual void onEntityRelive(UID uid) = 0;

	// 获取杂项管理器
	virtual IWarMiscManager* getMiscManager() = 0;

    // 获取最大的阵营数
    virtual BYTE getMaxCamp() = 0;

	// 获取管理器
	virtual __IWarMgr* getWarMgr(int nWarMgrID) = 0;

	// 救治队友
	virtual void cureMember(UID uid) = 0;

	/** 获取战场指定类型UID
	@param byGetCampType		: 阵营类型 EWarCampType
	@param bySerchType		    : 查找类型 EWarMonsterExec（怪物）、0（人物）
	*/
	virtual DWORD warSerchTypeUIDList(BYTE bySelfCamp, BYTE byGetCampType, BYTE bySerchType, PDBID* pReturnArray, DWORD dwArrayMaxSize) = 0;

	/** 指定类型buff处理
	@param bySelfCamp           : 自己的阵营
	@param byGetCampType		: 阵营类型 EWarCampType
	@param bySerchType		    : 查找类型 EWarMonsterExec
	*/
	virtual void warAddSerchTypeBuff(SWarEffectAddBuff sData) = 0;
};



/////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(WARSERVER_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(WARSERVER))
extern "C" IWar * CreateMobaWarProc();
#	define	CreateMobaWarProc	procCreateMobaWar
#else													/// 动态库版本
typedef IWar * (RKT_CDECL * procCreateMobaWar)();
#	define	CreateMobaWarProc	DllApi<procCreateMobaWar>::load(MAKE_DLL_NAME(WarServer), "CreateMobaWar")
#endif