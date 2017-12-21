#pragma once
#include "Entity_ManagedDef.h"
#include "EntityViewDef.h"

struct SHeroInfoFlags
{
	SHeroInfo   sHeroInfo;
	bool		bIsFree;

	SHeroInfoFlags()
	{
		bIsFree = false;
	}
};

typedef map<int, SHeroInfoFlags> PLAYERHEROMAP;
typedef vector<int>         SYSTEMHEROVEC;

class CHeroManager
{
public:
    CHeroManager();
    ~CHeroManager();

public:
    bool onSerialize(rkt::ibuffer& in, int nCount);
    bool deSerialize(rkt::obuf& out);

    bool onLoad( IEntity* pEntity);

public:
    // 获得当前玩家拥有的英雄
    int         getPlayerHeroArray(SHeroInfo* pArrHero, int nArrayCount);

	// 获得当前玩家战场可用的英雄
	int         getPlayerCanUseHeroArray(SHeroInfo* pArrHero, int nArrayCount);

    // 获得本周系统英雄
    int*        getSystemHeros(WORD& nCount);

    // 获取某个英雄信息
    SHeroInfo*  getHeroInfoByID(int nHeroID);

    // 是否是系统临时可用英雄
    bool        isSystemHero(int nHeroID);

    // 获得卡牌信息
    DBREQ_PARAM_HERO_CARD getHeroCardInfo( DWORD dwCardID );

    // 更新玩家英雄信息
    void        updatePlayerHeroInfo(SHeroInfo* pHeroInfo);

    // 更新英雄卡牌信息
    void        updateHeroCardInfo(LPCSTR pszMsg, int nlen);

	// 通过周免配置表设置英雄信息
	void		setHeroInfoByFreeWeekly();

	//	是否已拥有英雄
	inline bool		isHeroHave(int flags) const {return (flags & HERO_FLAG_NOT_HAVE) != HERO_FLAG_NOT_HAVE;}

	//  获得某个英雄的信息 (自己拥有的和周免的)
	SHeroInfo*         getPossessOrFreeHeroInfoByID(int nHeroID);
	
	//  XP技能同步
	void onMsgUpdateHeroXPSkillInfo( LPCSTR pszMsg, size_t len );
	
	//  XP技能碎片同步
	void onMsgUpdateHeroXPFragementInfo( LPCSTR pszMsg, size_t len );
public:
	void onMessage(int msgId, void * data, size_t len);
	bool onCommand(int cmdid, void * data, size_t len);
private:
	void onCommandAddLifeHeroRequest(cmd_entity_lifehero_info* pLifeHero);
	void onCommandDelLifeHeroRequest(cmd_entity_lifehero_del *pData);
	void onCommandReplaceLifeHeroRequest(cmd_entity_lifehero_replace *pData);
	void onCommandExchangeLifeHeroRequest(cmd_entity_lifehero_exchange *pData);
	void onCommandQueryLifeHeroRequest(cmd_entity_lifehero_query *pData);
	void onCommandLifeHeroObtainPrize();
	void onMessageLifeHeroQueryResult(void * data, size_t len);
	void onMessageUpdateLifeHero(void * data, size_t len);
	void onMessageReturnHistoryRecord(void * data, size_t len);
private:
    PLAYERHEROMAP m_playerHeroMap;
    SYSTEMHEROVEC m_systemHeroVec;
    std::map<DWORD, DBREQ_PARAM_HERO_CARD> m_mapHeroCard;

    IEntity* m_pMaster;
};