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
		ETIMERID_READ_HEROADDITIONAL,                       // �����Լ�Ӣ�۸������ݶ�ʱ��
	};

	enum
	{
		READ_HERO_ADDITIONAL = 500,                        // �����Լ�Ӣ���츳���ݶ�ʱ��ʱ��
	};

	enum
	{
		HERO_ADD_TYPE_CAID = 1,                            // Ӣ�۸�������_����
		HERO_ADD_TYPE_TALENT ,                             // Ӣ�۸�������_�츳
		HERO_ADD_TYPE_SKIN ,                               // Ӣ�۸�������_Ƥ��
		HERO_ADD_TYPE_XPSKILL ,                            // Ӣ�۸�������_XP����
		HERO_ADD_TYPE_XPFRAGMENT ,                         // Ӣ�۸�������_XP��Ƭ
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
	// Ӣ�����ݸ��µ����ݿ�
	bool            updateHerosToDB();
	// ���ָ��Ӣ����Ϣ
	SHeroInfo*      getHeroInfo(int nHeroID);

	// У��ָ��Ӣ����Ϣ
	SHeroInfo*      checkHeroInfo(int nHeroID);

	// ��ȡ����Ӣ��
	bool			getHeroArray(obuf & outData);

	// ��õ�ǰ���ս�����õ�Ӣ��
	bool			getPlayerCanUseHeroArray(obuf & outData);

	// ��ȡӢ������
	int             getHeroCount();

    // ��ȡӢ��ָ���Ǽ�������
    int             getAppointHeroStarCount(int nStarLevel);

	// ��ȡ����Ӣ�۸���
	int				getPermanentHeroCount();

	// ��ȡƤ���ܸ���
	int				getTotalSkinCount();

	// ��ȡ�ﵽ�ȼ�Ӣ�۸���
	int				getHeroCountByLevel(WORD nLevel);


	// �������Ӣ��
	PLAYERHEROMAP         getHeroMap();

	// ������Ӣ��
	bool            setNewHero( SHeroInfo sNewHeroInfo );

	// ����һ����Ӣ��   dwDeadLine Ĭ��Ϊһֱ����
	bool            addNewHero( int nHeroID, int nLv = 1, int nStar = 1, DWORD dwDeadLine = 0);

	// �Ƿ����������Ӣ��
	bool            canAddHero( int nHeroID );

	// ɾ��һ��Ӣ��
	bool			delHero(int nHeroID);

	// ���õ�ǰѡ��Ӣ�۵�ר���츳Ч��
	bool            setHeroExclusiveTalentEffect(int nHeroID);

	// ���ٵ�ǰѡ��Ӣ�۵�ר���츳Ч��
	bool            removeHeroExclusiveTalentEffect();

	// ����Ӣ�����ݵ��ڴ�
	void            setHeroAllInfo(int nHeroID, SHeroInfo sHeroInfo);

	// ����Ӣ�۵�����Ϣ���ڴ�nType:ENEXPTYPE   nMode:ENCHANGEDATAMODE
	void            setHeroOneInfo(int nHeroID, LONG lgValue, int nType, int nMode = ENCHANGEDATAMODE_SET);

	// ����Ӣ�ۿ�����Ϣ
	bool            addHeroCardInfo( DWORD* dwCardID, int* nValue, int nCount = 1);

	// �Ƿ�������ӿ���
	bool            canAddHeroCard(int nCardID, int nCount);

	// �ж��Ƿ������������
	bool            isCanUpStar(int nHeroID, DWORD dwCardID, int nCount);

	// ���Ŀ�����Ϣ
	bool            expendHeroCardInfo( DWORD dwCardID, int nCount);

	// ���ĳ�����Ƶ�����
	int             getOneCardCount( DWORD dwCardID );

	//	�Ƿ���ӵ��Ӣ��
	inline bool		isHeroHave(int flags) const {return (flags & HERO_FLAG_NOT_HAVE) != HERO_FLAG_NOT_HAVE;}

    // �Ƿ����Ӣ��
    bool            getHeroIsFree(int nHeroID);

	// �������ܷ�ʹ��Ӣ��Ƥ��
	bool canUseHeroSkin(DWORD dwHeroID, DWORD dwHeroSkinID);

public:
	// ���Ӣ������
	int            fillHeroInfo( obuf &obData );

	// �����Ӣ��  ��ʱ����ʱû����
	SHeroInfo       getNewHeroInfo( int nType = ENGETHEROWAY_NORMAL, int nStar = 1 );

	// �����ȡ����Ӣ�ۿ�����Ϣ
	bool            getHeroCardInfo();

	// �������Ӣ�ۿ�����Ϣ
	bool            updateHeroCardInfoToDB( DWORD nHeroID[], int nCount );

	// ���Ӣ�ۿ�����Ϣ
	void            sendCardInfoToClient();
	void            sendCardInfoToClient(DWORD dwCardID[], int nCount = 1);

protected:
	// ����Ӣ�۱Ƚϲ����д���
	void            compareHeroInfo(SHeroInfo sNewHero, SHeroInfo& sOldHero);

	// ����ֻ�����ò������ӵ�����  nMode: ENCHANGEDATAMODE
	void            setMustSetTypeInfo(DWORD* pData, LONG lgValue, int nMode);
	void            setMustSetTypeInfo(WORD* pData, WORD wValue, int nMode);
	void            setMustSetTypeInfo(BYTE* pData, BYTE byValue, int nMode);

	// Ӣ�����Ӿ���
	void            addStaticHeroExp(int nHeroID, LONG lgValue);

	// �������Ӣ��;��
	SHeroInfo       getNewHeroNormalWay();

	// ����ӵ�е���Ȩ
	int             pariseExclusive(BYTE* pArrExclusive, int* pArrID, BYTE* pArrFlags, int nSize );

	// ����Ӣ�����ݵ����ݿ�
	bool            updateHeroInfo(int nHeroID);
protected:
	// �������ݿⷵ�ص�Ӣ�ۿ�����Ϣ
	void            onGAMEDB_REQUEST_GET_HERO_CARD(int nQueueIndex,void * pOutData,int nOutLen);

protected:
	// ��ʱ�����б����Ӣ����Ϣ
	void            onUpdateChangedHeroInfo();

    // ���͵���������ʹ��Ӣ��
    void            onUpdateSocialHeroInofo();
    
private: 
    // ��⵱ǰ����Ӣ���Ƿ�رգ�����ر����Ϊ����һ�����С�δ�ر�Ӣ��
	void			checkCurHeroIsLocked();

	// ͨ���������ñ�����Ӣ����Ϣ
	void			setHeroInfoByFreeWeekly();
	// ��ʼ��Ӣ�۵ȼ�������������
	//bool            initHeroLevelPrizeUnlockInfo();
	//// ���Ӣ�۵ȼ���������
	//void            checkHeroLevelUnlockPrize(int nHeroId);

	// ��ʼ��ĳ��Ӣ���Ǽ�������������
	bool            initOneHeroStarPrizeUnlockInfo(int nHeroId);
	// ��ʼ��Ӣ���Ǽ�������������
	bool            initHeroStarPrizeUnlockInfo();
	// ���Ӣ���Ǽ���������
	void            checkHeroStarUnlockPrize(int nHeroId);
	// ����Ӣ���Ǽ������������ͻ���
	void			sendHeroStarUnlockPrizeToClient(int nHeroId, int nStarLv);

	// ����Ӣ�۵ȼ�
	bool                setHeroLevel(int nHeroID,int nValue);
	// ����Ӣ�۵ȼ�
	bool                addHeroLevel(int nHeroID,int nAddValue);
	// Ӣ�۵ȼ������¼�,�Ժ���������Ҫ�Ļ����ٷ����¼�
	void                heroLevelChangeHandel(int nHeroID);
	// Ӣ�۵ȼ��ı䷢���ͻ���
	void                sendHeroLevelChangeToClient(int nHeroID);

	////////////////////////////////////Ӣ�۸�����Ϣ���ݿ�ͨ�ýӿ�//////////////////////////////////////
	// ����Ӣ�۸�����Ϣ ������  Ӣ�۸�����Ϣ���� 1������2���츳3��Ƥ��
	void requestDataBaseHeroAdditional(int nHeroAdditionalType);
	// ���ݿⷵ��Ӣ�۸�����Ϣ  
	void onDBReturnHeroAdditional(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc,int nQueueIndex,void * pOutData,int nOutLen);
	////////////////////////////////////Ӣ���츳ҳ///////////////////////////////////////
public:
	// �ⲿ���Ӣ���츳��Ϣ
	bool addHeroTalentInfo(int nHeroId,int nTalentId, bool bSendToClient = true);
	// �Ƿ�������Ӣ���츳��Ϣ
	bool canAddHeroTalentInfo(int nHeroId,int nTalentId);
	// ����Ӣ���츳�����ݿ�
	void updateHeroTalentToDB(int nHeroID);
private:
	// ���ݿⷵ��
	void onDBReturnHeroTalentInfo(void * pOutData,int nOutLen,int nHeroCount);
	// ϵͳӢ���츳������µ��ͻ���
	void updateActiveHeroTalentToClient(HERO_TALENT_INFO * pHeroTalent,int nHeroId);
	// Ч����ֵ
	bool checkTalentHeroIdIsCorrect(int nHeroId ,int nTalentId);
	// �������дﵽӢ���Ǽ��Ŀɼ����츳
	void activeTalentByHeroStar(int nHeroId, int nHeroStarLv, bool bSendToClient = true);
	// ����Ӣ������Ӣ���Ǽ������������츳
	void activeAllHeroTalentByHeroStar();
public:
	// �������е��츳���ͻ���
	void sendHeroTalentInfoToClient();
	///////////////////////////////////Ӣ���츳ҳ///////////////////////////////////////
	////////////////////////////////// Ӣ��Ƥ��///////////////////////////////////////
public:
	// �Ƿ�ӵ��ĳ��Ƥ��
	bool checkIsPossessSkin(int nHeroId,int nSkinId);
	// �ⲿ���Ӣ��Ƥ����Ϣ
	bool addHeroSkinInfo(int nHeroId,int nSkinId,DWORD dwDurationTime);
	// �Ƿ�������Ӣ��Ƥ����Ϣ
	bool canAddHeroSkinInfo(int nHeroId,int nSkinId,DWORD dwDurationTime);
	////////////////////////////////// ����Ӣ�۹��ܴ���///////////////////////////////////////
public:
	void onMessage(int msgId, void * data, size_t len);
	// ��ȡ�ҵ����б���Ӣ��
	std::map<int, SLifeHero> getAllLifeHero();
	void resetLifeHero();
	void addLifeHero(int heroID, int cultureID);
	bool checkIsLifeHero(int nHeroID);
private:
	// ��ӱ���Ӣ��
	void onMessageAddLifeHero(const SLifeHero & lifeHero);
	// ɾ������Ӣ��
	void onMessageDelLifeHero(int nHeroId);
	/*�滻����Ӣ��
	@param nHeroId: ���滻��Ӣ��ID
	@param lifeHero: �µ�Ӣ��
	*/
	void onMessageReplaceLifeHero(int nHeroId, const SLifeHero& lifeHero);
	// ��������Ӣ��������λ��
	void onMessageExchangeCultureSlot(int nHeroId1, int nHeroId2);
	// �ͻ��������ȡ���б���Ӣ��
	void onMessageQueryAndSendAllLifeHero(msg_entity_lifehero_query* pData);
	// �ͻ���������ȡ����Ӣ�۽���
	void onMessageObtainLifeHeroPrize();
	// ���ͱ���Ӣ�����ݵ��ͻ���
	void sendLifeHeroInfoToClient(const SLifeHero & lifeHero);
	// �����ݿ��ȡ����Ӣ����Ϣ
	void readLifeHeroFromDB();
	// д�뱾��Ӣ����Ϣ�����ݿ�
	void writeLifeHeroToDB(const SLifeHero & lifeHero);
	// �����ݿ�ɾ������Ӣ��
	void delLifeHeroFromDB(DWORD dwActorId, int nHeroId);
	void onDBReturnReadLifeHero(int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, void * pOutData, int nOutLen);
	SLifeHero translatIntoSLifeHero(const msg_entity_lifehero_info& msgInfo);
	// ��¼����Ӣ��������־
	void recordLifeHeroGameNoteLog(enum LIFEHERO_NOTE_LOG_TYPE logType, const SLifeHero* pLifeHero1, const SLifeHero* pLifeHero2);
private:
	// ���ݿⷵ��
	void onDBReturnHeroSkinInfo(void * pOutData,int nOutLen,int nHeroCount);
	// ϵͳӢ��Ƥ��������µ��ͻ���
	void updateActiveHeroSkinToClient(HERO_SKIN_INFO * pHeroSkin,int nHeroId);
	// �������е�Ƥ�����ͻ���
	void sendHeroSkinInfoToClient();
	// Ƥ����Ϣ���º󱣴�Ƥ�������ݿ�
	void saveHeroSkinInfoToDB(int nHeroId);
	// ���Ƥ���Ƿ��ڣ����������ݿ�
	void checkToCleanSkinToDB();
	///////////////////////////////////Ӣ��Ƥ��	///////////////////////////////////////
	////////////////////////////////// Ӣ��xp����///////////////////////////////////////
	////////////////////////////////// Ӣ��xp�����߼�///////////////////////////////////////
public:
	// ����Ӣ��xp����
	bool            addHeroXPSkill( int nHeroID,int nXPSkillID);
	// �Ƿ��������xp����
	bool            canAddHeroXPSkill(int nHeroID,int nXPSkillID);
private:
	// ����Ӣ��xp����db
	bool			updateHeroXPSkillToDB(int nHeroID);
	// ����Ӣ�ۼ��ܵ��ͻ���
	bool			updateHeroXPSkillToCilent(int nHeroID = DEFALLHEROID);
	// ���ݿⷵ��
	void			onDBReturnHeroXPSkillInfo(void * pOutData,int nOutLen,int nHeroCount);
	////////////////////////////////// Ӣ��xp������Ƭ�߼�///////////////////////////////////////
public:
	// ����Ӣ��xp������Ƭ
	bool            addHeroXPSkillFragment( int nHeroID,int nXPSkillID,int nCount);
	// �Ƿ��������xp������Ƭ
	bool            canAddHeroXPSkillFragment(int nHeroID,int nXPSkillID,int nCount);
	// ������̬��ѡӢ��xp����
	void OnClientSelectHeroXPSkill( void * data, size_t len );
	// ѧϰxp����
	void learnHeroXPSpellInWar();
	// �ű���Ч��xpid�Ƿ���ȷ
	bool checkHeroXPIDIsCorrectInSchemeConfig( int nHeroID, int nXPSkillID );
	// �Զ�����Ĭ��xp����
	void autoActiveHeroXpSkill();
	// װ��Ĭ��xp����
	void equipDefaultHeroXPSKill();
private:
	// ����Ӣ��xp������Ƭdb
	bool			updateHeroXPSkillFragmentToDB(int nHeroID);
	// ����Ӣ�ۼ��ܵ���Ƭ�ͻ���
	bool			updateHeroXPSkillFragmentToCilent(int nHeroID = DEFALLHEROID);
	// ���ݿⷵ��
	void			onDBReturnHeroXPSkillFragmentInfo(void * pOutData,int nOutLen,int nHeroCount);
public:
	// �ͻ�����Ƭ�ϳ�xp
	void 			OnClientUnlockXPSkillByFragments( void * data, size_t len );

    SHeroInfoFlags&  getMapHeroInfo(int nHeroID);

	////////////////////////////////// Ӣ��xp����///////////////////////////////////////
		///////////////////////////////////Ӣ�۸�����Ϣ���ݿ�ͨ�ýӿ�//////////////////////////////////////
private:

	PLAYERHEROMAP                               m_mapHero;	// �ݶ�Ϊ������Ӣ�ۣ���ӵ�С�δӵ�У�����ʱ�����ݣ���
	ExclusiveEffectList                         m_listExclusiveEffect;

	__IEntity *		                            m_pMaster;

	std::map<DWORD, DBREQ_PARAM_HERO_CARD>      m_mapHeroCard;    //key ΪӢ�ۿ��Ƶ�Ψһid

	CPlayerBankPart*                            m_pBankPart;

	std::map<int, SLifeHero>                    m_mapLifeHero;  // ����Ӣ���б�
};
