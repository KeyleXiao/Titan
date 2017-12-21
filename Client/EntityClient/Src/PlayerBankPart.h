/*******************************************************************
** 文件名:	PlayerBankPart.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015/8/27
** 版  本:	1.0
** 描  述:	实体数据仓库部件

********************************************************************/


#pragma once
#include "IBankPart.h"
#include "IGameDatabaseDef.h"
#include "MatchDef.h"
#include "HeroManager.h"
#include "SchemeDef.h"
#include "EntityViewDef.h"
#include "Match_Manageddef.h"
#include "CrownPageManager.h"
#include "CommonManager.h"

using namespace Match_ManagerDef;

#define  DefaulTalentPageCount 2 //默认创建2页天赋页。最多20页
#define  DefaulRunePageCount 2   //默认创建2页符文页。最多20页

struct SGlorySchemeInfo;
// 实体数据仓库部件
class CPlayerBankPart : public IBankPart
{
    enum TopSideButtonFuncType
    {
        FUNCTYPE_TOPSIDEJOINCLANBTN,         // 提醒加入联盟
        FUNCTYPE_TOPSIDEJOINKINBTN,          // 提醒加入战队
        FUNCTYPE_TOPSIDESETTALENTBTN,        // 提醒设置天赋
        FUNCTYPE_TOPSIDESETRUNEBTN,           // 提醒设置符文

        FUNCTYPE_MAX,
    };

public:
    ////////////////////IEntityPart/////////////////////////////////////////////
    virtual bool		onLoad( IEntity* pEntity);

    virtual void		release();

    virtual short		getPartID();

    virtual IEntity *	getHost();

    // 消息通讯处理
    virtual bool		onMessage(void * pEntityHead, int msgId, void * data, size_t len);

    // 显示层的命令处理
    virtual bool 		onCommand( int cmdid,void * data,size_t len );

    virtual void        restore();

    ////////////////////ISerializable/////////////////////////////////////////////
    virtual bool		onSerialize( UID uid, rkt::ibuffer & in,int flag );

    virtual bool		deSerialize( rkt::obuf & out,int flag );

    ////////////////////IBankPart/////////////////////////////////////////////
    // 取得来源游戏世界ID
    virtual DWORD		getWorldID(void);

    // 获得引导数据
    virtual int         getPlayerNoviceGuideTypeData();

	/* 获取本命英雄引导完成情况数据，表示是否走完本命英雄的引导流程
	* Return: 1表示已走完完所有本命英雄，0表示还未完成
	*/
	virtual int         getLifeHeroTaskData();

    // 获取当前拥有的英雄信息
    virtual int         getPlayerHeroArray(SHeroInfo* pArrHero, int nArrayCount);
	// 获取当前战场可用的英雄信息
	virtual int         getPlayerCanUseHeroArray(SHeroInfo* pArrHero, int nArrayCount);

	// 获得某个英雄的信息 (自己拥有的和周免的)
	virtual SHeroInfo*  getPossessOrFreeHeroInfoByID(int nHeroID) ;

    // 获得系统英雄
    virtual int*        getSystemHeros(WORD& nCount);
    // 获得某个英雄的信息
    virtual SHeroInfo*  getHeroInfoByID(int nHeroID);
    // 是否是系统临时可用英雄
    virtual bool        isSystemHero(int nHeroID);
    // 根据卡牌id获得卡牌信息
    virtual DBREQ_PARAM_HERO_CARD getHeroCardInfo( int nCardID );
    // 获得英雄卡牌对应的配置信息
    virtual SHeroStarSchemeInfo* getHeroCardInfoConfig(int nHeroID );
    // 获得玩家信息
    virtual const SPlayerDatabasesInfo* getPlayerDatabasesInfo();

    ////////////////////CPlayerBankPart/////////////////////////////////////////////
    CPlayerBankPart();
    ~CPlayerBankPart();
private:
    /** SC_MSG_ENTITY_MATCHTYP_RANK
    @param   
    @param   
    @return  
    */
    void				OnMsgMatchTypeRank(LPCSTR pszMsg, int nLen);

    /** SC_MSG_ENTITY_MATCHTYP_SINGLE_RANK
    @param   
    @param   
    @return  
    */
    void				OnMsgMatchSingleTypeRank(LPCSTR pszMsg, int nLen);

    /** SC_MSG_ENTITY_MATCH_HEROSCORE
    @param   
    @param   
    @return  
    */
    void				OnMsgMatchHeroScore(LPCSTR pszMsg, int nLen);

    /** SC_MSG_ENTITY_MATCH_SINGLE_HEROSCORE
    @param   
    @param   
    @return  
    */
    void				OnMsgMatchSingleHeroScore(LPCSTR pszMsg, int nLen);

    void                OnMsgTalentPageInfoAll(LPCSTR pszMsg, int nLen);

	void                SendHeroDetailInfoToView(LPCSTR pszMsg, int nLen);

    void                SendMyTalentPageInfoAllToView();

    void                SendRunePageInfoAllToView();

    void                CreateDefaultTalentPage();

    void                CreateDefaultRunePage();
    /////////////////////////// Rune 符文 /////////////////////////////
    /** SC_MSG_ENTITY_RUNE_SYNC
    @param   
    @param   
    @return  
    */
    void				OnMsgRuneInfo(LPCSTR pszMsg, int nLen);
    void                OnMsgUpdateActiveRuneStoreInfo(LPCSTR data, int len);
    /////////////////////////// Rune 符文 /////////////////////////////

    void                OnMsgMatchReturnPlayerInfo(LPCSTR pszMsg, int nLen);
    void                OnMsgMatchReturnHeroInfo(LPCSTR pszMsg, int nLen);
    void                OnMsgUpdateHeroInfo(LPCSTR pszMsg, int nLen);
    void                OnMsgUpdatePlayerInfo(LPCSTR pszMsg, int nLen);
    void                OnMsgUpdateHeroCard(LPCSTR data, int nlen);
    void                OnMsgUpgradeStarSucess(LPCSTR data, int nlen);
    void                onMsgWarEndAwarkResult(LPCSTR data, int len);
    void                onMsgStaticHeroUpgrade(LPCSTR data, int len);
    void                onMsgLoginRecvData(LPCSTR data, int len);
    void                OnMsgHeroTalentInfo(LPCSTR data, int len);
	void                OnMsgUpdateActiveHeroTalentInfo(LPCSTR data, int len);
	void                OnMsgOtherPlayerRuneInfo(LPCSTR data, int len);
	void                OnMsgUpdateChestInfo(LPCSTR data, int len);
	void                OnMsgUpdateEmojiInfo(LPCSTR data, int len);
	void                OnMsgOpenChestResult(LPCSTR data, int len);
	void				OnMsgReturnChestRandCardInfo(LPCSTR data, int len);
    void                OnMsgHeroSkin(LPCSTR data, int len);
    void                OnMsgUpdateHeroSkin(LPCSTR data, int len);
	void                OnMsgPointshopUpdateBoughtNum(LPCSTR data, int len);
	void                OnMsgPointshopBuyResult(LPCSTR data, int len);
	void                OnMsgPointshopUpdateCardList(LPCSTR data, int len);
	void                OnMsgPointshopReflushBoughtNum();
	void				OnMsgHeroUpStarUnlockPrize(LPCSTR data, int len);
	void                onMsgNoviceGuideData(LPCSTR data, int len);
	void                onMsgLifeHeroTaskData(LPCSTR data, int len);
	void                onMsgLifeHeroPrizeTaskData(LPCSTR data, int len);
	void				onMsgActorUnlockData(LPCSTR data, int len);
	void				onMsgRankSeasonDetail(LPCSTR data, int len);
	void				onMsgRankSeasonRecord(LPCSTR data, int len);

protected:
    // 返回给显示层一个玩家的详细信息
    void                sendOneHeroDetailInfoToView(int nHeroID);

    // 填充属性信息
    void                fillHeroProperty( int nHeroID, int nStar,int nStaticLv, cmd_entity_hero_detail_info& cmdHeroDetailInfo);

    // 返回给显示层英雄升星前后属性信息
    void                sendHeroInfoUpgradeFrontAndBack(int nHeroID, int nStar = 0,int nGold = 0, int nCardCount = 0, int nStaticLv = 0);

protected:
    // 加载玩家已经获得的称号
    void                loadPlayerGlory();

    // 获得最强称号
    string              getMostGlory();

    // 填充英雄信息
    int					fillHeroInfo(msg_entity_hero_info* pMsgHeroInfo, int nCount, obuf& obData, PDBID nPDBID);

	// 填充不拥有的英雄信息
	void				fillNoHaveHeroInfo(map<int, msg_entity_hero_info *> & m_tmpHero, obuf& obData, PDBID nPDBID);


	void				fillHeroInfoMsg(msg_entity_hero_info &sMsg, const SHeroInfo &sHeroInfo);
    // 加载英雄技能信息
    void                loadHeroSkillInfo( int nHeroID , obuf& obData );

    // 更新首胜时间
    void				updateFirstwinTime(DWORD time);
	void				onMsgActorRenameResult( LPCSTR pszMsg,  int nLen );

	// 增益卡信息
	void				onMsgBenefitCardInfo(LPCSTR pszMsg, int nLen);

	// 是否升级特殊特效
	bool				isGradeSuccessSpecialEffect(int nHeroID, int nCurStarLv, int& nOldStarIconID, int& nCurStarIconID);

	// 更新新手引导数据
	void				updateNoviceGuideData(DWORD dwNoviceGuideType);

private:
    IEntity*			        m_pMaster;			// 实体对象

    SPlayerDatabasesInfo	    m_playInfo;

    CHeroManager                m_heroMgr;

    // 段位Rank分
    typedef map<EMMatchType, MatchTypeRankNode> MATCHTYPERANK;
    MATCHTYPERANK               m_MatchTypeRank;

    // 英雄等级得分
    typedef map<int, MatchHeroScoreNode> MATCHTHEROSCORE;
    MATCHTHEROSCORE             m_MatchHeroScore;

#ifdef  FUNCTION_TALENTPAGE_OPEN
	// 天赋页加点缓存
    typedef Entity_TalentPage_Points TALENTPAGEINFO;
    typedef map<int, TALENTPAGEINFO> MAPTALENTPAGE;
    MAPTALENTPAGE               m_mapTalentPage;  // 和符文一样数据在c#，此为初始化数据 
#endif

    /////////////////////////////////// Rune 符文系统/////////////////////////////////////

    // 以下两个数据的存在只是单纯的为了符文第一次点击时可以请求数据(如果设定服务器一下发就往c#发数据(怕卡)，则可不用)，具体数据都在c#，天赋同样

    // 本玩家符文页中符文的情况,一个玩家最多有20页符文
    // <RunePage.PageIdx, RUnePage>
#ifdef  FUNCTION_RUNE_OPEN

    typedef std::map<int, Entity_Rune_Page_Info> T_RunePageMap;
    T_RunePageMap m_mRunePage;
    // 符文库 <符文ID， struct Rune>

    RUNESTORE_MAP                           m_mapRuneStore;
#endif

    //////////////////////////////////////////////////////////////////////////////////////

    // 已经拥有的称号信息
    std::map<int,SGlorySchemeInfo>       m_mapGloryInfo;



	CCrownPageManager		m_crownPageMgr;

	// 杂项数据
	CCommonManager			m_CommonMgr;

    int                     m_nPlayerGuideTypeData;
	// 本命英雄引导完成情况数据，表示是否走完本命英雄的引导流程。1表示完成，0表示还未完成
	int                     m_nLifeHeroTaskData; 
};

#pragma pack()