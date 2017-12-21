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
    // ��õ�ǰ���ӵ�е�Ӣ��
    int         getPlayerHeroArray(SHeroInfo* pArrHero, int nArrayCount);

	// ��õ�ǰ���ս�����õ�Ӣ��
	int         getPlayerCanUseHeroArray(SHeroInfo* pArrHero, int nArrayCount);

    // ��ñ���ϵͳӢ��
    int*        getSystemHeros(WORD& nCount);

    // ��ȡĳ��Ӣ����Ϣ
    SHeroInfo*  getHeroInfoByID(int nHeroID);

    // �Ƿ���ϵͳ��ʱ����Ӣ��
    bool        isSystemHero(int nHeroID);

    // ��ÿ�����Ϣ
    DBREQ_PARAM_HERO_CARD getHeroCardInfo( DWORD dwCardID );

    // �������Ӣ����Ϣ
    void        updatePlayerHeroInfo(SHeroInfo* pHeroInfo);

    // ����Ӣ�ۿ�����Ϣ
    void        updateHeroCardInfo(LPCSTR pszMsg, int nlen);

	// ͨ���������ñ�����Ӣ����Ϣ
	void		setHeroInfoByFreeWeekly();

	//	�Ƿ���ӵ��Ӣ��
	inline bool		isHeroHave(int flags) const {return (flags & HERO_FLAG_NOT_HAVE) != HERO_FLAG_NOT_HAVE;}

	//  ���ĳ��Ӣ�۵���Ϣ (�Լ�ӵ�еĺ������)
	SHeroInfo*         getPossessOrFreeHeroInfoByID(int nHeroID);
	
	//  XP����ͬ��
	void onMsgUpdateHeroXPSkillInfo( LPCSTR pszMsg, size_t len );
	
	//  XP������Ƭͬ��
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