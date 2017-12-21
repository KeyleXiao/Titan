#pragma once
#include "IGameDatabaseDef.h"
#include <map>
#include "IEntity.h"
#include "IEffect.h"
#include "TimerHandler.h"
#include "IDBEngine.h"
#include "IBankPart.h"
#include "SchemeDef.h"
#include "Entity_ManagedDef.h"
class CPlayerBankPart;
#define  DEFALLHEROID -1

#define XP_SLOT_INDEX 5

struct SHeroInfoFlags
{
	SHeroInfo   sHeroInfo;
	bool        bIsChange;
	bool		bIsFree;
	//SHeroLevelPrizeUnlockVec sHeroLevelPrizeUnlockVec;
	SHeroStarPrizeUnlockVec sHeroStarPrizeUnlockVec;
	EntityHeroSkinInfoVec sHeroSkinList;
	EntityHeroTalentInfoVec sHeroTalentList;
	heroXPSkillList sHeroXPSkillList;
	heroXPFragmentList sHeroXPFragmentList;
		
	SHeroInfoFlags()
	{
		bIsChange = false;
		bIsFree = false;
	}
};

typedef std::map<int, SHeroInfoFlags> PLAYERHEROMAP;
typedef list<IEffect*> ExclusiveEffectList;
class CHeroManager:public TimerHandler,public IDBRetSink
{
	enum
	{
		ETIMERID_READ_HEROADDITIONAL,                       // 请求自己英雄附加数据定时器
	};

	enum
	{
		READ_HERO_ADDITIONAL = 500,                        // 请求自己英雄天赋数据定时器时间
	};

	enum
	{
		HERO_ADD_TYPE_CAID = 1,                            // 英雄附加类型_卡牌
		HERO_ADD_TYPE_TALENT ,                             // 英雄附加类型_天赋
		HERO_ADD_TYPE_SKIN ,                               // 英雄附加类型_皮肤
		HERO_ADD_TYPE_XPSKILL ,                            // 英雄附加类型_XP技能
		HERO_ADD_TYPE_XPFRAGMENT ,                         // 英雄附加类型_XP碎片
	};

	enum LIFEHERO_NOTE_LOG_TYPE
	{
		LIFEHERO_ADD,
		LIFEHERO_REPLACE,
		LIFEHERO_OBTAIN_PRIZE,
	};
public:
	CHeroManager();
	~CHeroManager();
	/////////////////////////////////TimerHandler/////////////////////////////////////////
	virtual void OnTimer( unsigned long dwTimerID );
	/////////////////////////////////IDBRetSink/////////////////////////////////////////
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

public:
	bool onSerialize(rkt::ibuffer& in, int nCount);

	bool deSerialize(rkt::obuf& out);

	bool init( __IEntity* pEntity,CPlayerBankPart* pPlayerBank, int nLoginMode);

    void restore();

	void release();

public:
	// 英雄数据更新到数据库
	bool            updateHerosToDB();
	// 获得指定英雄信息
	SHeroInfo*      getHeroInfo(int nHeroID);

	// 校验指定英雄信息
	SHeroInfo*      checkHeroInfo(int nHeroID);

	// 获取所有英雄
	bool			getHeroArray(obuf & outData);

	// 获得当前玩家战场可用的英雄
	bool			getPlayerCanUseHeroArray(obuf & outData);

	// 获取英雄数量
	int             getHeroCount();

    // 获取英雄指定星级的数量
    int             getAppointHeroStarCount(int nStarLevel);

	// 获取永久英雄个数
	int				getPermanentHeroCount();

	// 获取皮肤总个数
	int				getTotalSkinCount();

	// 获取达到等级英雄个数
	int				getHeroCountByLevel(WORD nLevel);


	// 获得所有英雄
	PLAYERHEROMAP         getHeroMap();

	// 设置新英雄
	bool            setNewHero( SHeroInfo sNewHeroInfo );

	// 增加一个新英雄   dwDeadLine 默认为一直存在
	bool            addNewHero( int nHeroID, int nLv = 1, int nStar = 1, DWORD dwDeadLine = 0);

	// 是否可以增加新英雄
	bool            canAddHero( int nHeroID );

	// 删除一个英雄
	bool			delHero(int nHeroID);

	// 设置当前选择英雄的专属天赋效果
	bool            setHeroExclusiveTalentEffect(int nHeroID);

	// 销毁当前选择英雄的专属天赋效果
	bool            removeHeroExclusiveTalentEffect();

	// 更新英雄数据到内存
	void            setHeroAllInfo(int nHeroID, SHeroInfo sHeroInfo);

	// 更新英雄单个信息到内存nType:ENEXPTYPE   nMode:ENCHANGEDATAMODE
	void            setHeroOneInfo(int nHeroID, LONG lgValue, int nType, int nMode = ENCHANGEDATAMODE_SET);

	// 更新英雄卡牌信息
	bool            addHeroCardInfo( DWORD* dwCardID, int* nValue, int nCount = 1);

	// 是否可以增加卡牌
	bool            canAddHeroCard(int nCardID, int nCount);

	// 判断是否符合升星条件
	bool            isCanUpStar(int nHeroID, DWORD dwCardID, int nCount);

	// 消耗卡牌信息
	bool            expendHeroCardInfo( DWORD dwCardID, int nCount);

	// 获得某个卡牌的数量
	int             getOneCardCount( DWORD dwCardID );

	//	是否已拥有英雄
	inline bool		isHeroHave(int flags) const {return (flags & HERO_FLAG_NOT_HAVE) != HERO_FLAG_NOT_HAVE;}

    // 是否免费英雄
    bool            getHeroIsFree(int nHeroID);

	// 检查玩家能否使用英雄皮肤
	bool canUseHeroSkin(DWORD dwHeroID, DWORD dwHeroSkinID);

public:
	// 填充英雄数据
	int            fillHeroInfo( obuf &obData );

	// 获得新英雄  临时的暂时没有用
	SHeroInfo       getNewHeroInfo( int nType = ENGETHEROWAY_NORMAL, int nStar = 1 );

	// 请求获取所有英雄卡牌信息
	bool            getHeroCardInfo();

	// 保存玩家英雄卡牌信息
	bool            updateHeroCardInfoToDB( DWORD nHeroID[], int nCount );

	// 填充英雄卡牌信息
	void            sendCardInfoToClient();
	void            sendCardInfoToClient(DWORD dwCardID[], int nCount = 1);

protected:
	// 新老英雄比较并进行处理
	void            compareHeroInfo(SHeroInfo sNewHero, SHeroInfo& sOldHero);

	// 设置只可设置不能增加的类型  nMode: ENCHANGEDATAMODE
	void            setMustSetTypeInfo(DWORD* pData, LONG lgValue, int nMode);
	void            setMustSetTypeInfo(WORD* pData, WORD wValue, int nMode);
	void            setMustSetTypeInfo(BYTE* pData, BYTE byValue, int nMode);

	// 英雄增加经验
	void            addStaticHeroExp(int nHeroID, LONG lgValue);

	// 正常获得英雄途径
	SHeroInfo       getNewHeroNormalWay();

	// 解析拥有的特权
	int             pariseExclusive(BYTE* pArrExclusive, int* pArrID, BYTE* pArrFlags, int nSize );

	// 保存英雄数据到数据库
	bool            updateHeroInfo(int nHeroID);
protected:
	// 接收数据库返回的英雄卡牌信息
	void            onGAMEDB_REQUEST_GET_HERO_CARD(int nQueueIndex,void * pOutData,int nOutLen);

protected:
	// 定时推送有变更的英雄信息
	void            onUpdateChangedHeroInfo();

    // 发送到社会服可以使用英雄
    void            onUpdateSocialHeroInofo();
    
private: 
    // 检测当前在用英雄是否关闭，如果关闭随机为其他一个已有、未关闭英雄
	void			checkCurHeroIsLocked();

	// 通过周免配置表设置英雄信息
	void			setHeroInfoByFreeWeekly();
	// 初始化英雄等级解锁奖励数据
	//bool            initHeroLevelPrizeUnlockInfo();
	//// 检测英雄等级解锁奖励
	//void            checkHeroLevelUnlockPrize(int nHeroId);

	// 初始化某个英雄星级解锁奖励数据
	bool            initOneHeroStarPrizeUnlockInfo(int nHeroId);
	// 初始化英雄星级解锁奖励数据
	bool            initHeroStarPrizeUnlockInfo();
	// 检测英雄星级解锁奖励
	void            checkHeroStarUnlockPrize(int nHeroId);
	// 发送英雄星级解锁奖励到客户端
	void			sendHeroStarUnlockPrizeToClient(int nHeroId, int nStarLv);

	// 设置英雄等级
	bool                setHeroLevel(int nHeroID,int nValue);
	// 增加英雄等级
	bool                addHeroLevel(int nHeroID,int nAddValue);
	// 英雄等级触发事件,以后其他期需要的话就再发个事件
	void                heroLevelChangeHandel(int nHeroID);
	// 英雄等级改变发到客户端
	void                sendHeroLevelChangeToClient(int nHeroID);

	////////////////////////////////////英雄附加信息数据库通用接口//////////////////////////////////////
	// 请求英雄附加信息 参数：  英雄附加信息类型 1、卡牌2、天赋3、皮肤
	void requestDataBaseHeroAdditional(int nHeroAdditionalType);
	// 数据库返回英雄附加信息  
	void onDBReturnHeroAdditional(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc,int nQueueIndex,void * pOutData,int nOutLen);
	////////////////////////////////////英雄天赋页///////////////////////////////////////
public:
	// 外部添加英雄天赋信息
	bool addHeroTalentInfo(int nHeroId,int nTalentId, bool bSendToClient = true);
	// 是否可以添加英雄天赋信息
	bool canAddHeroTalentInfo(int nHeroId,int nTalentId);
	// 更新英雄天赋到数据库
	void updateHeroTalentToDB(int nHeroID);
private:
	// 数据库返回
	void onDBReturnHeroTalentInfo(void * pOutData,int nOutLen,int nHeroCount);
	// 系统英雄天赋激活更新到客户端
	void updateActiveHeroTalentToClient(HERO_TALENT_INFO * pHeroTalent,int nHeroId);
	// 效验数值
	bool checkTalentHeroIdIsCorrect(int nHeroId ,int nTalentId);
	// 激活所有达到英雄星级的可激活天赋
	void activeTalentByHeroStar(int nHeroId, int nHeroStarLv, bool bSendToClient = true);
	// 激活英雄所有英雄星级符合条件的天赋
	void activeAllHeroTalentByHeroStar();
public:
	// 发送所有的天赋到客户端
	void sendHeroTalentInfoToClient();
	///////////////////////////////////英雄天赋页///////////////////////////////////////
	////////////////////////////////// 英雄皮肤///////////////////////////////////////
public:
	// 是否拥有某个皮肤
	bool checkIsPossessSkin(int nHeroId,int nSkinId);
	// 外部添加英雄皮肤信息
	bool addHeroSkinInfo(int nHeroId,int nSkinId,DWORD dwDurationTime);
	// 是否可以添加英雄皮肤信息
	bool canAddHeroSkinInfo(int nHeroId,int nSkinId,DWORD dwDurationTime);
	////////////////////////////////// 本命英雄功能处理///////////////////////////////////////
public:
	void onMessage(int msgId, void * data, size_t len);
	// 获取我的所有本命英雄
	std::map<int, SLifeHero> getAllLifeHero();
	void resetLifeHero();
	void addLifeHero(int heroID, int cultureID);
	bool checkIsLifeHero(int nHeroID);
private:
	// 添加本命英雄
	void onMessageAddLifeHero(const SLifeHero & lifeHero);
	// 删除本命英雄
	void onMessageDelLifeHero(int nHeroId);
	/*替换本命英雄
	@param nHeroId: 被替换的英雄ID
	@param lifeHero: 新的英雄
	*/
	void onMessageReplaceLifeHero(int nHeroId, const SLifeHero& lifeHero);
	// 交换本命英雄培养仓位置
	void onMessageExchangeCultureSlot(int nHeroId1, int nHeroId2);
	// 客户端请求获取所有本命英雄
	void onMessageQueryAndSendAllLifeHero(msg_entity_lifehero_query* pData);
	// 客户端请求领取本命英雄奖励
	void onMessageObtainLifeHeroPrize();
	// 发送本命英雄数据到客户端
	void sendLifeHeroInfoToClient(const SLifeHero & lifeHero);
	// 从数据库读取本命英雄信息
	void readLifeHeroFromDB();
	// 写入本命英雄信息到数据库
	void writeLifeHeroToDB(const SLifeHero & lifeHero);
	// 从数据库删除本命英雄
	void delLifeHeroFromDB(DWORD dwActorId, int nHeroId);
	void onDBReturnReadLifeHero(int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, void * pOutData, int nOutLen);
	SLifeHero translatIntoSLifeHero(const msg_entity_lifehero_info& msgInfo);
	// 记录本命英雄文字日志
	void recordLifeHeroGameNoteLog(enum LIFEHERO_NOTE_LOG_TYPE logType, const SLifeHero* pLifeHero1, const SLifeHero* pLifeHero2);
private:
	// 数据库返回
	void onDBReturnHeroSkinInfo(void * pOutData,int nOutLen,int nHeroCount);
	// 系统英雄皮肤激活更新到客户端
	void updateActiveHeroSkinToClient(HERO_SKIN_INFO * pHeroSkin,int nHeroId);
	// 发送所有的皮肤到客户端
	void sendHeroSkinInfoToClient();
	// 皮肤信息更新后保存皮肤到数据库
	void saveHeroSkinInfoToDB(int nHeroId);
	// 检查皮肤是否到期，到期清数据库
	void checkToCleanSkinToDB();
	///////////////////////////////////英雄皮肤	///////////////////////////////////////
	////////////////////////////////// 英雄xp技能///////////////////////////////////////
	////////////////////////////////// 英雄xp技能逻辑///////////////////////////////////////
public:
	// 增加英雄xp技能
	bool            addHeroXPSkill( int nHeroID,int nXPSkillID);
	// 是否可以增加xp技能
	bool            canAddHeroXPSkill(int nHeroID,int nXPSkillID);
private:
	// 更新英雄xp技能db
	bool			updateHeroXPSkillToDB(int nHeroID);
	// 更新英雄技能到客户端
	bool			updateHeroXPSkillToCilent(int nHeroID = DEFALLHEROID);
	// 数据库返回
	void			onDBReturnHeroXPSkillInfo(void * pOutData,int nOutLen,int nHeroCount);
	////////////////////////////////// 英雄xp技能碎片逻辑///////////////////////////////////////
public:
	// 增加英雄xp技能碎片
	bool            addHeroXPSkillFragment( int nHeroID,int nXPSkillID,int nCount);
	// 是否可以增加xp技能碎片
	bool            canAddHeroXPSkillFragment(int nHeroID,int nXPSkillID,int nCount);
	// 大厅动态点选英雄xp技能
	void OnClientSelectHeroXPSkill( void * data, size_t len );
	// 学习xp技能
	void learnHeroXPSpellInWar();
	// 脚本中效验xpid是否正确
	bool checkHeroXPIDIsCorrectInSchemeConfig( int nHeroID, int nXPSkillID );
	// 自动激活默认xp技能
	void autoActiveHeroXpSkill();
	// 装备默认xp技能
	void equipDefaultHeroXPSKill();
private:
	// 更新英雄xp技能碎片db
	bool			updateHeroXPSkillFragmentToDB(int nHeroID);
	// 更新英雄技能到碎片客户端
	bool			updateHeroXPSkillFragmentToCilent(int nHeroID = DEFALLHEROID);
	// 数据库返回
	void			onDBReturnHeroXPSkillFragmentInfo(void * pOutData,int nOutLen,int nHeroCount);
public:
	// 客户端碎片合成xp
	void 			OnClientUnlockXPSkillByFragments( void * data, size_t len );

    SHeroInfoFlags&  getMapHeroInfo(int nHeroID);

	////////////////////////////////// 英雄xp技能///////////////////////////////////////
		///////////////////////////////////英雄附加信息数据库通用接口//////////////////////////////////////
private:

	PLAYERHEROMAP                               m_mapHero;	// 暂定为有数据英雄（已拥有、未拥有（周免时有数据））
	ExclusiveEffectList                         m_listExclusiveEffect;

	__IEntity *		                            m_pMaster;

	std::map<DWORD, DBREQ_PARAM_HERO_CARD>      m_mapHeroCard;    //key 为英雄卡牌的唯一id

	CPlayerBankPart*                            m_pBankPart;

	std::map<int, SLifeHero>                    m_mapLifeHero;  // 本命英雄列表
};
