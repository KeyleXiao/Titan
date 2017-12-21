/*******************************************************************
** 文件名:	IBankPart.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-22
** 版  本:	1.0
** 描  述:	实体数据仓库部件

********************************************************************/

#pragma once

#include "IEntity.h"
#include "IGameDatabaseDef.h"
#include "MatchDef.h"
#include "Match_Manageddef.h"
using namespace Match_ManagerDef;

struct IPlayerMoneyManager;
struct IRuneManager;


// 实体数据仓库部件接口
struct __IBankPart : public __IEntityPart
{
	// 获取角色信息
	virtual SPlayerInfo * getPlayerInfo(void) = 0;

	// 获取指定英雄信息
	virtual SHeroInfo* getHeroInfo(int nHeroID) = 0;
	
	// 附加数据信息
	virtual void getPlayerRoomExData(SMatchPlayerRoomExData *sRoomExData) = 0;

	// 获取所有英雄
	virtual bool getHeroArray(obuf & outData) = 0;

	// 获得当前玩家战场可用的英雄
	virtual bool getPlayerCanUseHeroArray(obuf & outData) = 0;

    // 获取全部排位信息
    virtual bool getAllMatchTypeRank(obuf & outData) = 0;
    
    // 获取玩家数量
    virtual int getHeroCount() = 0;

	// 获取来源世界ID
	virtual DWORD getFromGameWorldID()= 0;

    // 获取账号ID
    virtual DWORD getUserID()= 0;

    // 获取IdCard数据
    virtual DWORD getIdCard() = 0;

    // 是否为成年人
    virtual bool isAdult() = 0;

    // 设置为成年人
    virtual void setAdult() = 0;

    // 获得玩家角色等级
    virtual int getPlayerLevel() = 0;

    // 获得玩家角色经验
    virtual int getPlayerExp() = 0;

    // 撮合英雄检测是否通过
    virtual bool isMatchHeroCheckPassed(EMMatchType eMatchType)= 0;

    // 获取排位Rank数据
    virtual DWORD getMatchTypeRank(EMMatchType eMatchType) = 0;

    // 获取隐藏Rank数据
    virtual DWORD getMatchTypeHideRank(EMMatchType eMatchType)= 0;

    // 获得rank等级
    virtual DWORD getMatchTypeRankGrade(EMMatchType eMatchType) = 0;

    // 添加或者减少排位Rank数据
    virtual bool addMatchTypeRank(EMMatchType eMatchType,int nRankScore = 0, int nHideRankScore = 0)= 0;

    // 设置排位Rank数据
    virtual bool setMatchTypeRank(EMMatchType eMatchType,int nRankScore = 0, int nHideRankScore = 0)= 0;

    // 设置排位等级(王者段位处理 只更新段位等级 策划要求 这种需求有什么问题就找策划)
    virtual bool setMatchTypeGrade(EMMatchType eMatchType, int nGrade) = 0;

    // 设置英雄熟练度信息
    virtual bool setMatchHeroScore(int nHeroID, int nRankScore)= 0;

    // 获得召唤师技能id信息
    virtual int getSummonerSkillInfo(int nSolt) = 0;

    // 设置玩家单独某个属性
    virtual void setActorBaseInfo(LONG lgValue, int nType, int nMode= ENCHANGEDATAMODE_SET, BYTE byNumType=0, BYTE byOpType=0, WORD wApproach=0, DWORD dwTargetID=0, LPCSTR szLogMark=0, int nLogReserve=0) = 0;

    // 设置玩家召唤师技能或者荣耀信息
	virtual void setActorSummonerOrGlory(int nData[], int nLen,int nType, long long llWarDBKey) = 0;

    // 更新英雄单个信息到内存
    virtual void setHeroOneInfo(int nHeroID, LONG lgValue, int nType, int nMode = ENCHANGEDATAMODE_SET, BYTE byNumType=0, BYTE byOpType=0, WORD wApproach=0, DWORD dwTargetID=0, LPCSTR szLogMark=0, int nLogReserve=0) = 0;

    // 设置英雄专属天赋效果
    virtual bool setHeroExclusiveTalentEffect( int nHeroID ) = 0;

    // 释放英雄专属天赋效果
    virtual bool removeHeroExclusiveTalentEffect() = 0;

    // 增加英雄卡牌信息
    virtual bool addHeroCardInfo( DWORD* dwCardID, int* nValue, int nCount = 1) = 0;

    // 是否可以增加卡牌
    virtual bool canAddHeroCard(int nCardID, int nCount) = 0;

    // 直接添加英雄
    virtual bool setNewHero( SHeroInfo sNewHeroInfo ) = 0;

    // 增加一个新英雄   dwDeadLine 默认为一直存在
    virtual bool addNewHero( int nHeroID, int nLv = 1, int nStar = 1, DWORD dwDeadLine = 0) = 0;

    // 是否可以增加新英雄
    virtual bool canAddHero( int nHeroID ) = 0;

    // 更新地图信息
    virtual void updateMapInfo( int nMapID, bool bIsWin) = 0;

    // 獲得是否有首勝
    virtual DWORD getFirstWinTime() = 0;

    // 设置玩家首胜信息
    virtual void setPlayerFirstWinFlag(DWORD dwTime) = 0;

    // 获得玩家战力值
	virtual int getPlayerFightCapacity() = 0;

	// 设置任务数据
	virtual bool addTaskDataInt(int nKey, int nData) = 0;

	// 删除任务数据
	virtual bool removeTaskDataInt(int nKey) = 0;

	// 获取任务数据
	virtual int getTaskDataInt(int nKey) = 0;

    // 注册任务数据
    virtual bool declareTaskData(int nKey, bool bIsInt, bool bSave) = 0;

    // 设置玩家活动度
    virtual bool setDayActivity(int value, bool isSync = true) = 0;

    // 获取玩家活动度
    virtual int getDayActivity(void) = 0;

    // 设置玩家周活动度
    virtual bool setWeekActivity(int value, bool isSync = true) = 0;

    // 获取玩家周活动度
    virtual int getWeekActivity(void) = 0;

    // 获取英雄的XP技能ID
	virtual int getXPSKillID(int nHeroID) = 0;

    // 获取当前天赋页
    virtual int getCurTalentPage() = 0;

	// 是否可以给角色奖励
	virtual bool canPrizeActor(int nPrizeID, int nNum) = 0;
	// 给角色奖励
	virtual bool prizeActor(int nPrizeID, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc) = 0;
	// 给角色奖励组
	virtual bool groupPrizeActor(int nPrizeGroupID, int *pOutPrizeID, int &nOutPrizeCount, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc) = 0;

	// 添加宝箱
	virtual bool addPrizeChest(int nChestType, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc) = 0;
	// 获取其他玩家当前符文数据
	virtual bool getOtherCrownPageData(obuf & outData) = 0;

	// 获取其他玩家当前宝石数据
	virtual bool getOtherCrownGemstoneData(obuf & outData) = 0;

	// 获得其他玩家最强英雄信息
	virtual bool getOtherStrongHeroInfo(obuf & outData) = 0;

	// 校验指定英雄信息
	virtual SHeroInfo* checkHeroInfo( int nHeroID ) = 0;

	// 储存同队友和自己的上一把的英雄id
	virtual void setActorTeammateHeroID( int* pTeammateHeroID )  = 0;

	// 获得随机卡牌
	virtual int getRandHeroCard(DWORD *pCardIDList, int *pCardNumList, int nCardListSize, WORD wApproach, int nParam) = 0;

	// 计算增益卡倍数
	virtual void calcBenefitCardMultiple(int *nGoldMultiple, int *nActorExpMultiple, int *nHeroExpMultiple) = 0;

	// 检查玩家能否使用英雄皮肤
	virtual bool canUseHeroSkin(DWORD dwHeroID, DWORD dwHeroSkinID) = 0;

    // 获取总共PK比赛的场次
    virtual DWORD getPKTotalNum() = 0;

    // 获取总共比赛场次
    virtual DWORD getTotalMatchNum() = 0;

	// 设置IdCard数据
	virtual void setIdCard(DWORD dwIdCard) = 0;

	// 建立师徒关系
	virtual void buildMentorship(DWORD dwDesPDBID) = 0;

	// 发送任务事件
	virtual void sendActorDataTaskEvent(WORD nTaskEvent, int nParam1, int nParam2, int nParam3) = 0;

	// 获取永久英雄个数
	virtual int getPermanentHeroCount() = 0;

	// 获取皮肤总个数
	virtual int getTotalSkinCount() = 0;

	// 获取达到等级英雄个数
	virtual int getHeroCountByLevel(WORD nLevel) = 0;

    // 获取个人货币管理
    virtual IPlayerMoneyManager* getMoneyMgr() = 0;

    // 获取服务管理对象
    virtual IRuneManager* getRuneMgr() = 0;

	// 设置实名认证标志
	virtual void setRealNameAutherFlag() = 0;

	// 购买商城物品
	virtual bool onMessagePointShopBuyGoods(void *data, size_t len) = 0;	

    // 获取静态地图皮肤ID
    virtual int getStaticSkin() = 0;

	// 删除一个英雄
	virtual bool delHero(int nHeroID) = 0;

    // 设置核晶效果 timeToAddEffect ETIME_TO_ADD_EFFECT
    virtual void setCrownEffect(int timeToAddEffect) = 0;

	// 战场结束设置数据
	virtual void setMatchRankSeasonInfo(SRankSeasonWarEndInfo pInfo) = 0;

};

// 人物货币管理
struct IPlayerMoneyManager
{
    /** 取得点券兑换金币的汇率
    @return : 
    */
    virtual double getExchangeRate(void) = 0;

    /** 是否可增加或扣除货币（只操作一种货币，支持一个点券转金币功能）
    @param nMoney:      操作点券/金币数量，注：负数为扣除，正数为增加
    @param nMoneyType:  货币类型 GAME_MONEY_TYPE,如果为
    @return : 
    */
    virtual bool canAddMoneyEx(int nMoney, BYTE nMoneyType) = 0;

    /** 是否可增加或扣除货币
    @param nTicket:  操作点券数量，注：负数为扣除，正数为增加
    @param nGlod  :  操作金币数量，注：负数为扣除，正数为增加
    @return : 
    */
    virtual bool canAddMoney(int nTicket, int nGlod) = 0;

    /** 增加或扣除货币
    @param nTicket: 添加的点券，负数表示扣除点卷
    @param nGlod  : 添加的金币，负数表示扣除金币
    @param  : nReason 添加的原因（产生OSS_RESOURCE_ADD_RANGE/消耗的途径OSS_RESOURCE_OPT_DEL/转进OSS_RESOURCE_OPT_TRANSIN/转出OSS_RESOURCE_OPT_TRANSOUT）
    @param  : pszRemark 添加的点券日志的备注（记日志用）
    @param  : nSubclassID, 子类ID，购买物品则填物品ID，没有子类默认0
    @param  : nNum 个数，子类是物品ID则填物品个数
	@param  : bAddictLimit，是否考虑沉迷限制
    @ note  : 在添加点券操作提交数据库（包括其他模块提交的）还没有返回前，这时候你提交点券是会失败的
    */
    virtual bool addMoney(int nTicket, int nGlod, int nReason, const char * pszRemark=0, int nSubclassID=0, short nNum=0, bool bAddictLimit=true) = 0;

    // 返回Ticket数据库操作
    virtual void onDBReturnTicketAdd(int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen) = 0;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IBankPart * CreateBankPart();
#	define	CreateBankPartProc	CreateDiePart
#else													/// 动态库版本
typedef __IBankPart * (RKT_CDECL * procCreateBankPart)();
#	define	CreateBankPartProc	DllApi<procCreateBankPart>::load(MAKE_DLL_NAME(ENTITY), "CreateBankPart")
#endif
