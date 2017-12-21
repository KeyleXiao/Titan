/*******************************************************************
** 文件名:	PlayerBankPart.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-22
** 版  本:	1.0
** 描  述:	实体数据仓库部件

********************************************************************/


#pragma once

#include "IBankPart.h"
#include "IPlayerTaskData.h"
#include "IDBEngineService.h"
#include "MatchDef.h"
#include "TalentEffectProxy.h"
#include "IEventEngine.h"
#include "EventDef.h"
#include "HeroManager.h"
#include "MatchSceneHelper.h"
#include "PointShopManager.h"
#include "ActorPrizeManager.h"
#include "PlayerRole.h"
#include "BenefitCardManager.h"
#include "CrownPageManager.h"
#include "CommonManager.h"
#include "PlayerMoneyManager.h"
#include "RuneManager.h"
#include "TalentPageManager.h"
#include "TaskDef.h"
#include "RankSeasonManager.h"
#include "ValueGoodsHelper.h"
#include "EmojiManager.h"

#define  DefaultMobaEffectPageIndex 0
#define  DEFAULTMAXPAGECOUNT 20
#define  DEFAULTMAXRUNETYPECOUNT 100	// 最大符文类型效验值
#define  DEFAULTMAXRUNEITEMCOUNT 100    // 最大符文类型子项效验值
#define  DEFAULTMAXRUNEID 500			// 符文id最大效验值
#define  DEFAULTMAXSKINID 200		    // 皮肤id最大效验值
#define  DEFAULTMAXHEROID 200			// 英雄id最大值	

#include "ActorInOutLog.h"

// 实体数据仓库部件
class CPlayerBankPart : public __IBankPart, public IDBRetSink, public TimerHandler,public IEventExecuteSink
{
	typedef std::map<int, EntityHeroTalentInfoVec>         TALENTSTORE_MAP;

	enum
	{
		ETIMERID_READ_TALENT_FROM_DATABASE,                 // 请求自己天赋数据定时器
		ETIMERID_UPDATE_CHANGED_PLAYERINFO,                 // 向客户端推送变更的玩家信息 
		ETIMERID_READ_RUNE_FROM_DATABASE,                   // 请求自己的符文数据定时器
	};

	enum
	{
		READ_TALENT_INTERVAL = 500,                         // 请求天赋时间间隔
		UPDATE_CHANGED_INFO_INTERVAL = 500,                 // 向客户端推送变更信息时间间隔
		READ_RUNE_INTERVAL = 500,                           // 请求符文时间间隔

		PROPERTY_RUNE_MATCH  = 34,                          // 符文属性对应角色属性映射数
	};

	enum
	{
		ActorServiceType_ChangeSex = 1,						// 请求玩家信息
		ActorServiceType_Rename,						    // 请求英雄信息
	};

public:
    CPlayerBankPart();
    virtual ~CPlayerBankPart();

public:
	///////////////////////////////// ISerializable ////////////////////////////
	virtual bool onSerialize( rkt::ibuffer & in,int flag );

	virtual bool deSerialize( rkt::obuf & out,int flag );

	///////////////////////////////// __IEntityPart //////////////////////////
	virtual short getPartID();

	virtual bool onLoad( __IEntity* pEntity, int nLoginMode);

	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);

	virtual __IEntity * getHost();

	virtual void release();

	// 切换英雄
	virtual void  onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore) { }

    virtual void  restore();

	///////////////////////////////// __IBankPart //////////////////////////	
	// 获取角色信息
	virtual SPlayerInfo * getPlayerInfo(void);

    // 获取附加房间信息
    virtual void getPlayerRoomExData(SMatchPlayerRoomExData *sRoomExData);

	// 获取指定英雄信息
	virtual SHeroInfo * getHeroInfo(int nHeroID);

	// 校验指定英雄信息
	virtual SHeroInfo * checkHeroInfo( int nHeroID );

	// 获取所有英雄
	virtual bool getHeroArray(obuf & outData);

	// 获得当前玩家战场可用的英雄
	virtual bool getPlayerCanUseHeroArray(obuf & outData);

    // 获取全部排位信息
    virtual bool getAllMatchTypeRank(obuf & outData);

	// 获得玩家拥有的英雄数量
	virtual int getHeroCount();

	// 获取来源世界ID
	virtual DWORD getFromGameWorldID();

	// 获取账号ID
	virtual DWORD getUserID();

    // 获取IdCard数据
    virtual DWORD getIdCard();

	// 设置IdCard数据
	virtual void setIdCard(DWORD dwIdCard);

    // 是否为成年人
    virtual bool isAdult();

    // 设置为成年人
    virtual void setAdult();

    // 获得玩家角色等级
    virtual int getPlayerLevel();

    // 获得玩家角色经验
    virtual int getPlayerExp();

    // 撮合英雄检测
    virtual bool isMatchHeroCheckPassed(EMMatchType eMatchType);

	// 获取匹配撮合Rank分数
	virtual DWORD getMatchTypeRank(EMMatchType eMatchType);

    // 获取隐藏Rank数据
    virtual DWORD getMatchTypeHideRank(EMMatchType eMatchType);

	virtual DWORD getMatchTypeRankGrade(EMMatchType eMatchType);

	// 设置排位分数
	virtual bool setMatchTypeRank(EMMatchType eMatchType,int nRankScore,int nHideRankScore);

    // 只设置排位等级
    virtual bool setMatchTypeGrade(EMMatchType eMatchType, int nGrade);

	// 更新排位Rank数据
	virtual bool addMatchTypeRank(EMMatchType eMatchType,int nRankScore = 0, int nHideRankScore = 0);

	// 设置英雄熟练度信息
	virtual bool setMatchHeroScore(int nHeroID, int nRankScore);

	// 获得召唤师技能id信息
	virtual int getSummonerSkillInfo(int nSolt);

	// 检查玩家能否使用英雄皮肤
	virtual bool canUseHeroSkin(DWORD dwHeroID, DWORD dwHeroSkinID);

    // 获取总共PK比赛的场次
    virtual DWORD getPKTotalNum();

    // 获取总共比赛的场次
    virtual DWORD getTotalMatchNum();

    // 获取英雄XP技能
    virtual int getXPSKillID(int nHeroID);

	/////////////////////////////玩家相关/////////////////////////////////////////////
	// 更新玩家基本信息                       nType:ENPLAYERINFOTYPE   nMode:ENCHANGEDATAMODE
	virtual void setActorBaseInfo(LONG lgValue, int nType, int nMode, BYTE byNumType, BYTE byOpType, WORD wApproach, DWORD dwTargetID, LPCSTR szLogMark, int nLogReserve);

	//设置玩家召唤师技能或者荣耀信息          nType:ENPLAYERINFOTYPE
	virtual void setActorSummonerOrGlory(int nData[], int nLen,int nType, long long llWarDBKey);

	// 獲得玩家是否有首勝
	virtual DWORD getFirstWinTime();

	// 設置玩家是否首勝
	virtual void setPlayerFirstWinFlag( DWORD dwTime);

	// 获得战力值
	virtual int getPlayerFightCapacity();

	// 设置任务数据
	virtual bool addTaskDataInt(int nKey, int nData);

	// 删除任务数据
	virtual bool removeTaskDataInt(int nKey);

	// 获取任务数据
	virtual int getTaskDataInt(int nKey);

    // 注册任务数据
    virtual bool declareTaskData(int nKey, bool bIsInt, bool bSave);

    // 设置玩家活跃度
    virtual bool setDayActivity(int value, bool isSync = true);

    // 设置玩家活跃度
    virtual int getDayActivity(void);

    // 设置玩家周跃度
    virtual bool setWeekActivity(int value, bool isSync = true);

    // 获取玩家周活跃度
    virtual int getWeekActivity(void);

	// 获取当前天赋页
	virtual int getCurTalentPage();

	// 获取当前符文数据
	virtual bool getOtherRuneData(obuf & outData);

	// 获取其他玩家当前皇冠页数据
	virtual bool getOtherCrownPageData(obuf & outData);

	// 获取其他玩家当前宝石数据
	virtual bool getOtherCrownGemstoneData(obuf & outData);

	// 储存同队友和自己的上一把的英雄id
	virtual void setActorTeammateHeroID(int* pTeammateHeroID);
	// 获得随机卡牌  nParam根据wApproach不同传相应的参数
	virtual int getRandHeroCard(DWORD *pCardIDList, int *pCardNumList, int nCardListSize, WORD wApproach, int nParam);
	///////////////////////////英雄相关///////////////////////////////////////////////
	// 获得最强英雄信息
	virtual bool getOtherStrongHeroInfo(obuf & outData);
	// 更新英雄单个信息到内存              nType:ENHEROINFOTYPE   nMode:ENCHANGEDATAMODE
	virtual void setHeroOneInfo(int nHeroID, LONG lgValue, int nType, int nMode, BYTE byNumType, BYTE byOpType, WORD wApproach, DWORD dwTargetID, LPCSTR szLogMark, int nLogReserve);

	// 设置当前选择英雄的专属天赋效果
	virtual bool setHeroExclusiveTalentEffect( int nHeroID );

	// 销毁当前选择英雄的专属天赋效果
	virtual bool removeHeroExclusiveTalentEffect();

	// 增加英雄卡牌信息
	virtual bool addHeroCardInfo( DWORD* dwCardID, int* nValue, int nCount = 1);

	// 是否可以增加卡牌
	virtual bool canAddHeroCard(int nCardID, int nCount);

	// 更新地图信息
	virtual void updateMapInfo( int nMatchType, bool bIsWin);

	// 测试使用
	virtual bool setNewHero( SHeroInfo sNewHeroInfo );

	// 增加一个新英雄   dwDeadLine 默认为一直存在
	virtual bool addNewHero( int nHeroID, int nLv = 1, int nStar = 1, DWORD dwDeadLine = 0);

	// 是否可以增加新英雄
	virtual bool canAddHero( int nHeroID );

	// 删除一个英雄
	virtual bool delHero(int nHeroID);
    
    // 设置核晶效果 timeToAddEffect ETIME_TO_ADD_EFFECT
    virtual void setCrownEffect(int timeToAddEffect);
	
    // 是否可以给角色奖励
	virtual bool canPrizeActor(int nPrizeID, int nNum);
	// 给角色奖励
	virtual bool prizeActor(int nPrizeID, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc);
	// 给角色奖励组
	virtual bool groupPrizeActor(int nPrizeGroupID, int *pOutPrizeID, int &nOutPrizeCount, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc);

	// 添加宝箱
	virtual bool addPrizeChest(int nChestType, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc);

	// 建立师徒关系
	virtual void buildMentorship(DWORD dwDesPDBID);

	// 发送任务事件
	virtual void sendActorDataTaskEvent(WORD nTaskEvent, int nParam1, int nParam2, int nParam3);

	// 获取永久英雄个数
	virtual int getPermanentHeroCount();

	// 获取皮肤总个数
	virtual int getTotalSkinCount();

    // 获取静态地图皮肤ID
    virtual int getStaticSkin();

	// 获取达到等级英雄个数
	virtual int getHeroCountByLevel(WORD nLevel);

    // 获取个人货币管理
    virtual IPlayerMoneyManager* getMoneyMgr() { return &m_moneyMgr;};

	virtual IRuneManager* getRuneMgr();

	// 设置实名认证标志
	virtual void setRealNameAutherFlag();

	// 购买商城物品
	virtual bool onMessagePointShopBuyGoods(void *data, size_t len);

	// 战场结束设置数据
	virtual void setMatchRankSeasonInfo(SRankSeasonWarEndInfo pInfo);

	/////////////////////////////TimerHandler/////////////////////////////////////////////
	virtual void OnTimer( unsigned long dwTimerID );

	///////////////////////////////// IDBRetSink ////////////////////////////
	/** 数据库请求返回回调方法
	@param   pDBRetSink		：结果返回接口	
	@param   nCmdID ：命令ＩＤ
	@param   nDBRetCode：数据库请求返回值，参考上面定义
	@param   pszDBRetDesc：数据库请求返回描述，由ＳＰ返回
	@param   nQueueIndex：队列定义
	@param   pOutData：输出数据
	@param   nOutLen：输出数据长度
	@return  
	@note     
	@warning 此对像千万不能在数据库返回前释放，否则会非法！
	@retval buffer 
	*/	
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

private:
    // 服务器更新角色提取点券
    void onTransmitUpdateExtractTicket(void * data, size_t len);

	// 客户端请求匹配撮合数据
	void OnClientRequestMatchTypeRank(msg_entity_matchtype_rank * pRequestData);

	// 客户端请求英雄熟练度数据
	void OnClientRequestHeroScore(msg_entity_match_heroscore * pRequestData);

	// 数据库返回匹配撮合数据
	void OnDBReturnMatchTypeRank(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	// 数据库返回英雄熟练度
	void OnDBReturnMatchHeroScore(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	void onClientRequestActorNameChange( void * data, size_t len );

private:
	////////////////////////////////////玩家信息//////////////////////////////////////
	// 处理玩家请求玩家基本信息
	void onReqPlayerInfo(void * data, size_t nLen);

	// 处理玩家请求已经拥有的英雄信息
	void onReqPlayerHeroInfo(void *pData,size_t nLen);

	// 赋值玩家结构信息
	void fillPlayerInfo(msg_entity_player_info_return & playerInfo, SPlayerInfo & info);
	void fillPlayerInfo(DBREQ_PARAM_SAVEACTOR & playerInfo, SPlayerInfo & info);
	
	// 赋值玩家英雄结构信息
	void fillHeroInfoMsg(msg_entity_hero_info &sMsg, const SHeroInfo &sHeroInfo);


	// 请求更新玩家数据到数据库
	bool onReqUpdageHeroStar(int nHeroID);

	bool onMessageChangeHero(void * data, size_t len);
    // 切换英雄皮肤
    bool onChangeSkin(int nHeroID, int nSkinID);

	// 请求地图信息
	bool requestMapInfo();


	void setPlayerFightCapacity();

	// 获得战场随机卡牌
	int randHeroCardForWar(int nCount, DWORD* pCard, int nArrCount);
	// 获得宝箱随机卡牌
	int randHeroCardForChest(int nPrizeID, DWORD* pCard, int nArrCount);
	// 获得卡牌基础概率表
	void getHeroCardPro(vector<sRandHeroCard>& cardPro);

    // 同步排位共享属性
    void syncMatchTypeRankInfo(MatchTypeRankNode Data);
public:
	// 外部添加英雄天赋信息
	bool addHeroTalentInfo(int nHeroId,int nTalentId);
	// 是否可以添加英雄天赋信息
	bool canAddHeroTalentInfo(int nHeroId,int nTalentId);
	// 外部添加英雄皮肤信息
	bool addHeroSkinInfo(int nHeroId,int nSkinId,DWORD dwDurationTime);
	// 是否可以添加英雄皮肤信息
	bool canAddHeroSkinInfo(int nHeroId,int nSkinId,DWORD dwDurationTime);
	// 请求更新玩家数据到数据库
	bool updatePlayerInfoToDB();
	// 设置玩家的当前天赋页和符文页数据
	void setRuneTalentPageIndex();

	// 处理玩家请求设置新手引导类型
	void onReqSetNoviceGuideData(void * data, size_t nLen);

protected:
	// 玩家增加经验
	void addPlayerExp(LONG lgValue);

	// 设置只可设置不能增加的类型  nMode: ENCHANGEDATAMODE
	void setMustSetTypeInfo(DWORD* pData, LONG lgValue, int nMode);
	void setMustSetTypeInfo(WORD* pData, WORD wValue, int nMode);
	void setMustSetTypeInfo(BYTE* pData, BYTE byValue, int nMode);


	// 玩家登录时将首胜时间发送到客户端
	void sendFirstWinTimeOnLogin();

	// 玩家登录时检测是否首次登录，设置任务数据
	bool checkTodayFirstLogin();

    // 检测通知引导类型任务数据
    void checkNoticeGuideTypeTask();
	// 发送新手引导类型
	void sendNoviceGuideTypeToClient();

protected:
	// 定时将玩家变更的信息推送到客户端
	void onUpdateChangedPlayerInfo(int nInfoChangeType);

    // 新的一天
    void onNewDay();
    // 触发玩家活跃度数值同步
    void syncActivity();

    // 判断两个时间点之间相差几周
    int diffWeek(DWORD dwTime1, DWORD dwTime2);

public:
	// 获取任务数据
	int getTaskDataIntEx(int nKey, int nDefault = 0);
	// 设置任务数据
	bool setTaskDataIntEx(int nKey, int nData, int nDefault = 0);
	///////////////////////////////////////角色等级解锁奖励///////////////////////////////////
	// 玩家刚刚登陆初始化解锁信息 
	bool initActorUnlockPrizeInfo();
	// 角色等级变化去检测解锁玩家奖励
	void checkUnlockActorPrizeByLevelChange();
	////////////////////////////////////角色等级改变统一接口//////////////////////////////////////
	// 设置角色等级
	bool                setActorLevel(int nValue);
	// 增加角色等级
	bool                addActorLevel(int nAddValue);
	// 发出角色等级改变事件
	void                fireActorLevelChangeEvent();
	// 增值服务请求数据库 ：目前有 修改名字 修改性别等等
	bool addService_RequestToDB(int nCmdID, LPCSTR lpszInData, int nInDataLen);
	// 增值服务数据库返回
	bool addService_ResponseFromDB(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// 增值服务人物信息检查 目前有 修改名字 修改性别等等
	bool onDBReturn_AddServiceUserCheck(const DBREQ_RESULT_ADDSERVICES_USER_CHK & param, const char * pszErrorDesc);
	// 将修改名字的结果告知客户端 因为客户端需知晓来动态关闭界面
	void sendRenameResultToClient(bool bSuccess);
	//	更新角色名字修改到数据库和缓存
	bool updateActorNameToDBAndCache(int nMoneyType,int nMoney);
	//  处理改名功能
	int doActorServiceTypeRename(const char* pNewName, const int nMoneyType);
	//	改名失败的字符串打印
	void onDBActorServiceCheckRenameFlagFail( const char * pszErrorDesc );
public:
	////////////////////////////////////////增益卡//////////////////////////////////////////////
	// 是否可以添加增益卡
	bool canAddBenefitCard(int nBenefitCardID, int nNum);
	// 添加增益卡
	bool addBenefitCard(int nBenefitCardID, int nNum);
	// 计算增益卡倍数
	virtual void calcBenefitCardMultiple(int *nGoldMultiple, int *nActorExpMultiple, int *nHeroExpMultiple);
	////////////////////////////////////////英雄xp技能//////////////////////////////////////////////
	// 增加英雄xp技能
	bool            addHeroXPSkill( int nHeroID,int nXPSkillID);
	// 是否可以增加xp技能
	bool            canAddHeroXPSkill(int nHeroID,int nXPSkillID);
	////////////////////////////////////////皇冠宝石//////////////////////////////////////////////
	// 打开宝石宝箱
	void onMessageOpenGemstoneChest(int nChestType);
	// 设置效果: timeToAddEffect:ETIME_TO_ADD_EFFECT
	void setProxyCrownPageEffect(int *propertyList, int timeToAddEffect);
	// 增加宝石经验
	void addCrownInlayGemstoneExp( LONG lgValue );
	// 更新宝石宝箱结果到客户端
	void updateChestInfoToClient();
	// 添加皇冠宝石数据
	bool addCrownGemstone(int nGemstoneID,int nCount = 1, BYTE byGrade = 1, int nExp = 0, bool bAppraisal = false);
	// 判断下是否能添加皇冠宝石
	bool canAddCrownGemstone(int nGemstoneID, BYTE byGrade,int nCount = 1);
	// 能否增加页面
	bool canAddPage(DWORD nNum=1);
	// 创建新页面
	bool createNewPage(DWORD nNum=1);
public:
	//void sendBenefitCardToClient();

	// 发送本命英雄入口完成情况任务数据
	void sendLifeHeroTaskDataToClient();
	// 发送本命英雄奖励任务数据
	void sendLifeHeroPrizeTaskDataToClient();
	// 清空所有本命英雄并删除本命英雄任务数据
	void resetLifeHero();
	// 添加本命英雄
	void addLifeHero(int heroID, int cultureID);

public:
	// DB存价值物品信息
	bool saveValueGoodsInfoToDB(BYTE byType, DWORD dwGoodsID, DWORD dwGoodsCount);
	// DB读价值物品信息
	bool readValueGoodsInfoFromDB(BYTE byType);
	// DB读宝箱结果
	void onDBReturnReadChestInfo(vector<DBREQ_TABLE_VALUE_GOODS> * pvecGoods);
	// 是否可以添加表情包
	bool canAddEmoji(BYTE byEmojiType);
	// 添加表情包
	bool addEmoji(BYTE byEmojiType);
	// DB读表情包结果
	void onDBReturnReadEmojiInfo(vector<DBREQ_TABLE_VALUE_GOODS> * pvecGoods);

public:
	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

protected:
	__IEntity *		                        m_pMaster;

	SPlayerDatabasesInfo                    m_playInfo; 

    // 个人上下日记类
    CActorInOutLog                          m_actorLog;

	CCrownPageManager						m_crownPageMgr;
	// 个人任务数据
	IPlayerTaskData*                        m_pTaskData;
	int										m_nIntTaskDataLen;
	BYTE									*m_pIntTaskData;
	int										m_nByteTaskDataLen;
	BYTE									*m_pByteTaskData;
	bool									m_bWaitForLoadTaskData;

	// 是否正在增加金币给DB，DB返回后自动设置false
	bool                                    m_isAddingTicketToDB;

	//////////////////////////////////////////////////////////////////////////
	// 段位Rank分
	typedef map<EMMatchType, MatchTypeRankNode> MATCHTYPERANK_MAP;
	MATCHTYPERANK_MAP                       m_mapMatchTypeRank;

	// 英雄等级得分
	typedef map<int, MatchHeroScoreNode> MATCHTHEROSCORE_MAP;
	MATCHTHEROSCORE_MAP                     m_mapMatchHeroScore;

	TalentEffectProxy                       m_talentEffectProxy;


	//////////////////////////////////////////////////////////////////////////////////////
	// 价值物品帮助类
	CValueGoodsHelper			m_valueGoodsHelper;

	// 英雄管理器
	CHeroManager                m_heroMgr;
	// 商城管理器
	CPointShopManager			m_pointShopMgr;
	// 角色奖励管理器
	CActorPrizeManager			m_actorPrizeMgr;

	// 地图数据
	//std::map<int, DBREQ_PARAM_PLAYER_MAP>	  m_mapPlayerMapInfo;

	///////////////////////////////////////////////////////////////////////////////////
	// 角色奖励解锁信息
	SActorPrizeUnlockVec m_vecActorUnlockInfo;
	char m_cRenameCache[DBDEF_ACTORNAME_MAXSIZE]; 
	int nRenameMoneyCache;

	// 增益卡
	CBenefitCardManager		m_beneficardMgr;

	// 杂项数据
	CCommonManager			m_CommonMgr;

    // 角色货币管理
    CPlayerMoneyManager     m_moneyMgr;

	// 赛季数据管理
	CRankSeasonManager		m_rankSeasonMgr;

	// 表情包管理
	CEmojiManager			m_emojiMgr;

#ifdef FUNCTION_RUNE_OPEN
    // 符文系统
    CRuneManager            m_runeManager;
#endif

#ifdef FUNCTION_TALENTPAGE_OPEN
	// 天赋页
	CTalentPageManager		m_talentPageMgr;
#endif
};