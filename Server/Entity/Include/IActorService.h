/*******************************************************************
** 文件名:	IActorService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/28/2014
** 版  本:	1.0
** 描  述:	

			每个玩家对应一个Actor服务(一个协程)
********************************************************************/

#pragma once
#include "NetMessageDef.h"
#include "AOI_Define.h"
#include "IGameSceneService.h"
#include "MigrateDef.h"
#include "MatchDef.h"
#include "Match_Manageddef.h"
#include "MailDef.h"
using namespace Match_ManagerDef;

struct EFFECT_CONTEXT;
struct BuffTypeList;
struct ProgressBarContext;
struct EntityBelongContext;
/**
	职责: 
	1.封装对内部接口__IPlayerRole的调用
*/
struct SSharePersonProperty;

struct IActorService
{
	//////////////////////////////// IEntity ///////////////////////////////////////////////
	// 取得实体ID
	virtual UID getUID() = 0;

	// 取得实体名称
	virtual string getName()  = 0;

	// 取得当前所在场景ID
	virtual int getSceneID() = 0;

	// 取得实体位置
	virtual Vector3 getLocation() =0;

	// 设置实体位置
	virtual void setLocation(Vector3 vLoc) =0;

	// 取得实体角度
	virtual Vector3 getAngle() = 0;

	// 收到网络消息
	virtual void handleMessage( void *pData,int nLen ) = 0;

	// 投递一个可替换的消息，例如移动消息，队列中只保留一个，后面的替换前面的
	virtual void handleReplaceableMessage(unsigned long ulKey,void * pMsg,int nLen) = 0;

	// 发送数据给客户端 nFlag:SEND_DATA_FLAG
	virtual int send2Client( PACKAGE_PTR package, int nFlag = 0) = 0;

    // 发送数据给客户端 附带发给ob
    virtual void send2ClientAndOB( PACKAGE_PTR package ) = 0; 

	// 改变当前状态 
	virtual void changeState(int nState) = 0;

	// 得到当前状态 
	virtual int getState(void) = 0;

    // 设置游戏状态ACTOR_GAME_STATE
    virtual void setGameState(DWORD dwState) = 0;
    // 获取游戏状态 ACTOR_GAME_STATE
    virtual DWORD getGameState() = 0;

	// 取得玩家所在网关分配ID
	virtual DWORD getClientID() = 0;

	// 导出角色数据
    // bFromBank:        true -- 从数据仓库(bankpart)导出
    //                   false -- 正常导出
	virtual string exportActorData(int nFlag, bool bFromBank = false) = 0;

    // 导出角色的共享属性
    virtual SSharePersonProperty exportShareProperties() = 0;

    // 取得创建上下文中的标识数据 ACTOR_CREATE_CONTEXT_FLAG
    virtual BYTE getCreateContextFlag(BYTE nFromActorType, BYTE nToActorType, BYTE nFlag) = 0;

    // 是否为压力测试账号
    virtual bool isTester() = 0;

	/** 增加/减少经验值
	@param   nExp:            经验值（负值减少，正值增加）
	@param   wApproach:       消耗原因（产生OSS_RESOURCE_ADD_RANGE/消耗的途径OSS_RESOURCE_OPT_DEL/转进OSS_RESOURCE_OPT_TRANSIN/转出OSS_RESOURCE_OPT_TRANSOUT）
	@param   dwTargetID:      如果有目标者，这个就填入目标者ID
	@param   szLogMark:       传入交易日志，供查询  
	@param   nLogReserve:     日志的保留字段
	@return
	*/
    virtual void addExp(int nExp, WORD wApproach, DWORD dwTargetID=0, const char * pszRemark=0, int nLogReserve=0) = 0;
    
    /** 是否可增加或扣除货币
    @param nTicket: 添加的点券，负数表示扣除点卷
    @param nGlod  : 添加的金币，负数表示扣除金币
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

    // 添加效果 返回效果ID记录
    virtual	int mobaGoodsAddEffect(int nEffectID, void * pAddContext, size_t len) = 0;

    // 移除效果
    virtual	void mobaGoodsRemoveEffect( int nID) = 0;

	/////////////////////// IEntityProperty ///////////////////////////////////////////////
	// 取得某个属性的类型
	virtual string getPropertyType( int prop_id ) = 0;

	// 取得某个属性的标志数据(通常是序列化标志)
	virtual int getPropertyFlag( int prop_id ) = 0;

	// 获取实体属性(布尔类型）
	virtual bool getProperty_Bool( int prop_id) = 0;

	// 设置实体属性(布尔类型）
	virtual bool setProperty_Bool( int prop_id,bool value) = 0;

	// 获取实体属性(数字,实际内部支持用int,byte,short三种类型存)
	virtual int getProperty_Integer( int prop_id) = 0;

	// 设置实体属性(数字,实际内部支持用int,byte,short三种类型存)
	virtual bool setProperty_Integer( int prop_id,int value) = 0;

	// 增加属性数值(注意内部需要控制上下限)
	virtual bool addProperty_Integer( int prop_id,int delta) = 0;

	// 获取实体属性(字符串)
	virtual string getProperty_String( int prop_id ) = 0;

	// 设置实体属性(字符串)
	virtual bool setProperty_String( int prop_id,string value) = 0;

	// 获取实体属性(自定义结构,如果属性存在返回)
	virtual string getProperty_custom( int prop_id) = 0;

	// 设置实体属性(自定义结构)
	virtual bool setProperty_custom( int prop_id,string value) = 0;

    // 更改天赋点属性
    virtual void updateTalentPoint( int nPoint, int nCHangeTYpe, BYTE byAddType = 2) = 0;
	
	/////////////////////// IEntityAOI ///////////////////////////////////////////////
	// 取得视野半径
	virtual float getSight() = 0;

	// 调整视野半径
	virtual void setSight( float radii ) = 0;

	// 是否隐身
	virtual bool isHide() = 0;

	// 设置隐身状态
	virtual void setHide( bool state ) = 0;

	virtual void release() = 0;

	/////////////////////// IEntityEnmity ///////////////////////////////////////////////
	virtual void AddEnmityValue(UID uidAdd, int nVal) = 0;

	virtual void AddBroadEnmityValue(UID uidAdd, int nVal) = 0;

	virtual void SetEnmityValue(UID uidAdd, int nVal) = 0;

	/////////////////////// IBuffPart ///////////////////////////////////////////////
	/** 给实体添加buff */	
	virtual bool AddBuff(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime, void *pBuffContext, int nContextLen) = 0;
	/** 给实体移除buff */
	virtual bool RemoveBuff(DWORD dwBuffID, UID uidAdd, UID uidRemove) = 0;
	/** 某个BUFF是否存在 */
	virtual bool IsExistBuff(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd) = 0;
    /** 某个BUFF类型是否存在 */
    virtual bool IsExistBuffType(DWORD dwBuffType) = 0;
    /** 取玩家某个BUFF的等级 */
	virtual DWORD GetBuffLevel(DWORD dwBuffID, UID uidAdd) = 0;
	/** 取玩家某个BUFF剩余时间 */
	virtual int GetLeftTime(DWORD dwBuffID, UID uidAdd) = 0;
	/** 设置累积BUFF */
	virtual void SetAccumulateBuff(DWORD dwBuffID, UID uidAdd, bool bOverlay, DWORD dwTime, void *pBuffContext, int nContextLen) = 0;
	// 通过索引删除buff
	virtual bool RemoveBuffByIndex(DWORD dwIndex)=0;
	/////////////////////// IFreezePart ///////////////////////////////////////////////
	// 获取冷却剩余时间
	virtual DWORD GetFreezeLeftTime(DWORD dwClassID, DWORD dwFreezeID)=0;
	// 启动冷却
	virtual bool FreezeWork(DWORD dwClassID, DWORD dwFreezeID) = 0;
	// 设置冷却时间
	virtual bool SetFreezeTime(DWORD dwClassID, DWORD dwFreezeID, int nTime, bool nIsCleanInterferedFreeze = false,bool bBreak=false) = 0;

    /////////////////////// IMovePart ///////////////////////////////////////////////
    virtual bool Transport(Vector3 newLoc, Vector3 rotation, bool bTransport, int nSceneID, int nReason = TRANSPORT_REASON_FLASH) = 0;
	
	// 人物移动
	virtual bool MoveTo(Vector3 newLoc)=0;

	/////////////////////////IBankPart/////////////////////////////////////////////////
    virtual DWORD getFromGameWorldID() = 0;

    virtual DWORD getUserID() = 0;

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

    // 获取排位Rank数据
    virtual DWORD getMatchTypeRank(EMMatchType eMatchType)= 0;

    // 获取隐藏Rank数据
    virtual DWORD getMatchTypeHideRank(EMMatchType eMatchType)= 0;

    // 获取全排位Rank数据
    virtual string getAllMatchTypeRank()= 0;

    // 获得排位rank等级
    virtual DWORD getMatchTypeRankGrade(EMMatchType eMatchType) = 0;

    // 更新排位Rank数据
    virtual void addMatchTypeRank(EMMatchType eMatchType,int nRankScore = 0, int nHideRankScore = 0)= 0;

    // 设置排位Rank数据
    virtual void setMatchTypeRank(EMMatchType eMatchType,int nRankScore = 0, int nHideRankScore = 0)= 0;

    // 设置排位等级(王者段位处理 只更新段位等级 策划要求 这种需求有什么问题就找策划)
    virtual void setMatchTypeGrade(EMMatchType eMatchType, int nGrade)= 0;

    // 设置英雄熟练度信息
    virtual void setMatchHeroScore(int nHeroID, int nRankScore)= 0;

    // 撮合英雄检测是否通过
    virtual bool isMatchHeroCheckPassed(EMMatchType eMatchType)= 0;

    // 获得召唤师技能id
    virtual int getSummonerSkillInfo(int nSolt) = 0;

    // 设置玩家活跃度
    virtual void setDayActivity( int value, bool isSync=true) = 0;

    // 获取玩家活跃度
    virtual int getDayActivity(void) = 0;

    // 设置玩家周活跃度
    virtual void setWeekActivity(int value, bool isSync = true) = 0;

    // 获取玩家周活跃度
    virtual int getWeekActivity(void) = 0;

    // 设置玩家单独某个属性
    virtual void setActorBaseInfo(LONG lgValue, int nType, int nMode=0, BYTE byNumType=0, BYTE byOpType=0, WORD wApproach=0, DWORD dwTargetID=0, LPCSTR szLogMark=0, int nLogReserve=0) = 0;

    // 设置玩家召唤师技能或者荣耀信息
	virtual void setActorSummonerOrGlory(int nData[], int nLen,int nType, long long llWarDBKey) = 0;

    // 更新英雄单个信息到内存              nType:ENEXPTYPE   nMode:ENCHANGEDATAMODE
    virtual void setHeroOneInfo(int nHeroID, LONG lgValue, int nType, int nMode = 0, BYTE byNumType=0, BYTE byOpType=0, WORD wApproach=0, DWORD dwTargetID=0, LPCSTR szLogMark=0, int nLogReserve=0) = 0;

    // 获得英雄属性信息
    virtual SHeroInfo getHeroInfo( int nHeroID ) = 0;

	// 获得所有英雄信息
	virtual string getHeroArray() = 0;

	// 获得当前玩家战场可用的英雄
	virtual string getPlayerCanUseHeroArray() = 0;

    // 获得英雄数量
    virtual int getHeroCount() = 0;

    // 获得玩家信息
    virtual SPlayerInfo getPlayerInfo() = 0;

    // 设置英雄专属天赋效果
    virtual void setHeroExclusiveTalentEffect( int nHeroID ) = 0;
    
    // 释放英雄专属天赋效果
    virtual void removeHeroExclusiveTalentEffect() = 0;

    virtual bool addHeroCardInfo( DWORD* dwCardID, int* nValue, int nCount = 1) = 0;
    
    virtual void updateMapInfo( int nMapID, bool bIsWin) = 0;

    virtual DWORD getFirstWinTime() = 0;

    virtual void setPlayerFirstWinFlag( DWORD dwTime) = 0;

    virtual int getPlayerFightCapacity() = 0;

    // 获得临时天赋
    virtual void activeTempTalent(int nTalentID) = 0;

    // 取消临时天赋
    virtual void deActiveTempTalent(int nTalentID) = 0;
	
    virtual int getWarID() = 0;

    virtual int getMatchTypeID() = 0;

    virtual int getSmSkillId(int nSlot) = 0;

    // 是否对camp阵营可见
    virtual bool isInsight( int camp ) = 0;

    // 机器人行为
	virtual void onEventRobotAction(BYTE nAction, void * data, size_t len) = 0;

	// 设置任务数据
	virtual bool addTaskDataInt(int nKey, int nData) = 0;
	// 删除任务数据
	virtual bool removeTaskDataInt(int nKey) = 0;
	// 获取任务数据
	virtual int getTaskDataInt(int nKey) = 0;
    // 注册任务数据
    virtual bool declareTaskData(int nKey, bool bIsInt, bool bSave) = 0;

    // 获取英雄的XP技能ID
	virtual int getXPSKillID(int nHeroID) = 0;

    // 获取当前天赋页
    virtual int getCurTalentPage() = 0;

    // 获取当前符文页
    virtual int getCurRunePage() = 0;

    // 获得玩家附加数据
    virtual void getPlayerRoomExData(SMatchPlayerRoomExData *sRoomExData) = 0;

	// 检查玩家能否使用英雄皮肤
	virtual bool canUseHeroSkin(DWORD dwHeroID, DWORD dwHeroSkinID) = 0;

    // 获取总共PK比赛的场次
    virtual DWORD getPKTotalNum() = 0;

    // 获取总共比赛场次
    virtual DWORD getTotalMatchNum() = 0;

    //////////////////////////IGoodsPart////////////////////////////////////////////////

    virtual BuffTypeList getBuff(DWORD dwBuffType) = 0;

    // 选择目标
    virtual bool chooseTarget(int nTargetFilter, UID uidTarget) = 0;
    // 取得PK类型
    virtual int getPKType(void) = 0;
    // 检测切磋
    virtual bool checkPK(UID uidTarget) = 0;
    // 取得PK状态
    virtual int getPKState(void) = 0;

	// 获取其他玩家当前符文数据
	virtual string getOtherRuneData() = 0;

	// 获取其他玩家当前皇冠页数据
	virtual string getOtherCrownPageData() = 0;

	// 获取其他玩家当前宝石数据
	virtual string getOtherCrownGemstoneData() = 0;

	// 获取其他玩家最强英雄信息
	virtual string getOtherStrongHeroInfo() = 0;

	// 是否可以给角色奖励
	virtual bool canPrizeActor(int nPrizeID, int nNum) = 0;
	// 给角色奖励
	virtual bool prizeActor(int nPrizeID, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc) = 0;
	// 给角色奖励组
	virtual bool groupPrizeActor(int nPrizeGroupID, int *pOutPrizeID, int &nOutPrizeCount, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc) = 0;

	// 添加宝箱
	virtual bool addPrizeChest(int nChestType, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc) = 0;

    // 实体通知主人
    virtual void entityNotifyMaster(EntityBelongContext context) = 0;

	// 校验指定英雄信息
	virtual SHeroInfo checkHeroInfo( int nHeroID ) = 0;

	// 储存同队友和自己的上一把的英雄id
	virtual void setActorTeammateHeroID(int* pTeammateHeroID) = 0;

	// 获得随机卡牌
	virtual int getRandHeroCard(DWORD *pCardIDList, int *pCardNumList, int nCardListSize, WORD wApproach, int nParam) = 0;

	// 开始人物读条
	virtual void StartProgressBar( ProgressBarContext* pContext ) = 0 ;

    // 取得人物所在地图ID
    virtual int getMapID(void)=0;

    /////////////////////// ITitlePart ///////////////////////////////////////////////
    virtual void AddTitle(int nTitleID, char * szTitleName, int nTitleNameLen, bool bCoverTitle=true) = 0;

    virtual void deleteTitle(int nTitleID) = 0;

    // 续完
    virtual void continuePlay(ClientID client) = 0;

    virtual void onClientDisconnect() = 0;

    // 设置客户端ID
    virtual void setClientID(ClientID client) = 0;

	// 计算增益卡倍数
	virtual void calcBenefitCardMultiple(int *nGoldMultiple, int *nActorExpMultiple, int *nHeroExpMultiple) = 0;

    // 冲锋
    virtual void shift(void *pData, int nDataLen) = 0;

	///////////////////////IMailPart///////////////////////////////////////////////////
	//// 处理邮件详细数据
	//virtual bool dealPlayerMailDetailData(LONGLONG llMailID, bool isAutoRecvPlus = false) = 0;
	// 添加邮件详细数据
	//virtual void addPlayerMailDetailData(SMailDetailData sDetailData, bool isAutoRecvPlus = false) = 0;
	//// 处理邮件发送附件数据
	virtual bool dealSendMailPlusData(SMailSendData sendData) = 0;

	//// 获取领取附件时间
	//virtual DWORD getLastRecvPlusTime() = 0;

	//// 设置领取附件时间
	//virtual void setLastRecvPlusTime(DWORD dwTime) = 0;

	//// 邮件是否已领取
	//virtual bool isAreadyRecvMail(LONGLONG llMailID) = 0;

	//// 是否在快速取件
	//virtual bool isFastRecvMailPlus() = 0;

	//// 设置快速取件标识
	//virtual void setFastRecvMailPlus(bool bFlag, int nRecvCount) = 0;
	///////////////////////IMailPart end///////////////////////////////////////////////////

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

	// 保存引导步骤
	virtual void saveGuideStep(int nStep) = 0;

	// 设置实名认证标志
	virtual void setRealNameAutherFlag() = 0;

	// 购买商城物品
	virtual bool onMessagePointShopBuyGoods(void *data, size_t len) = 0;	

	// 通知LUA添加交互回复
	virtual void sendLuaAddContactReply(LONGLONG llContactID) = 0;	

    // 获取静态地图皮肤ID
    virtual int getStaticSkin() = 0;

	// 通知LUA添加好友申请
	virtual void sendLuaContactApplyFriend(DWORD dwTargetID, bool bIsDifSex) = 0;

    //强制结束PK状态
    virtual void  forceEndPK() = 0;

	// 获取PK阵营
	virtual int getPKCamp() = 0;

    // 设置核晶效果
    virtual void setCrownEffect(int timeToAddEffect) = 0;

    // 购买物品
    virtual bool luaBuyGoods(int nGoodsID, int nCost) = 0;

    // 切换职业
    virtual bool changeVoc(int nNewVoc, int nNewSkinID) = 0;

	// 战场结束设置数据
	virtual void setMatchRankSeasonInfo(SRankSeasonWarEndInfo pInfo) = 0;
};