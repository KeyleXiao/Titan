#include "StdAfx.h"
#include "HeroManager.h"
#include "IServerGlobal.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "ITimerService.h"
#include "IPlayerRole.h"
#include "EntityHelper.h"
#include "IDBEngineService.h"
#include "EventDef.h"
#include "IEntityEvent.h"
#include "OssLogDef.h"
#include "IOSSLogService.h"
#include "PlayerBankPart.h"
#include "IAntiAddictServer.h"
#include "ExternalHelper.h"
#include "ISpell.h"
#include "IWarPart.h"
#include "ICenterConnectorService.h"
#include "TaskStorageDef.h"

bool lessMark(const SHeroInfo& heroInfo1, const SHeroInfo& heroInfo2)
{

	return heroInfo1.dwMatchNum > heroInfo2.dwMatchNum;
}
CHeroManager::CHeroManager()
{
	m_mapHero.clear();
	m_listExclusiveEffect.clear();
	m_pMaster = NULL;
}
CHeroManager::~CHeroManager()
{
	m_mapHero.clear();
	m_listExclusiveEffect.clear();
	m_pMaster = NULL;

}

void CHeroManager::OnTimer( unsigned long dwTimerID )
{
	if ( m_pMaster == NULL )
	{
		return;
	}

	switch( dwTimerID )
	{
	case ETIMERID_READ_HEROADDITIONAL:
		{
			// 请求英雄附加信息
			requestDataBaseHeroAdditional((int)HERO_ADD_TYPE_TALENT);
			requestDataBaseHeroAdditional((int)HERO_ADD_TYPE_SKIN);  
			requestDataBaseHeroAdditional((int)HERO_ADD_TYPE_XPSKILL);
			requestDataBaseHeroAdditional((int)HERO_ADD_TYPE_XPFRAGMENT);
		}
		break;
	default:
		break;
	}
}

void CHeroManager::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if ( nDBRetCode != DBRET_CODE_SUCCEED )
	{
		ErrorLn( __FUNCTION__<<" nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
		return;
	}

	switch( nCmdID )
	{
	case GAMEDB_REQUEST_GET_HERO_CARD:
		{
			onGAMEDB_REQUEST_GET_HERO_CARD(nQueueIndex, pOutData, nOutLen);
		}
		break;

	case GAMEDB_REQUEST_GET_HEROADDITIONAL:            //  英雄附加信息 1、卡牌2、天赋3、皮肤
		{
			onDBReturnHeroAdditional(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;

	case GAMEDB_REQUEST_READ_LIFEHERO:
		{
			onDBReturnReadLifeHero(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;

	default:
		{
			return;
		}
	}
}

bool CHeroManager::onSerialize(rkt::ibuffer& in, int nCount)
{
	for ( int i = 0; i < nCount; ++i )
	{
		if ( !in.good())
		{
			return false;
		}
		SHeroInfo sNode;
		in >> sNode;

		SHeroInfoFlags sHeroInfo;
		sHeroInfo.sHeroInfo = sNode;
		sHeroInfo.bIsChange = false;

		// 临时处理， 之后默认星级为1
		if (sHeroInfo.sHeroInfo.nHeroStar == 0 )
		{
			sHeroInfo.sHeroInfo.nHeroStar = 1;
		}

		m_mapHero[sNode.nHeroID] = sHeroInfo;
	}

	//initHeroPrizeUnlockInfo();
	return true;
}

bool CHeroManager::deSerialize(rkt::obuf& out)
{
	PLAYERHEROMAP::iterator it = m_mapHero.begin();
	for (; it != m_mapHero.end(); ++it )
	{
		out.push_back( &it->second.sHeroInfo, sizeof(SHeroInfo));
	}
	return true;
}

bool CHeroManager::init( __IEntity* pEntity,CPlayerBankPart* pPlayerBank, int nLoginMode)
{
	if ( pEntity == NULL )
	{
		return false;
	}

	if ( pPlayerBank == NULL )
	{
		return false;
	}

	m_pMaster = pEntity;
	m_pBankPart = pPlayerBank;
	gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);
	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if(pTimerService)
	{
		pTimerService->SetTimer(ETIMERID_READ_HEROADDITIONAL, READ_HERO_ADDITIONAL, this,  1);
	}

	// 请求玩家英雄卡牌信息
	getHeroCardInfo();
	//initHeroLevelPrizeUnlockInfo();
	initHeroStarPrizeUnlockInfo();
	// 添加周免英雄列表
	setHeroInfoByFreeWeekly();

	readLifeHeroFromDB();

	// 登陆时，如果当前英雄关闭，随机一个已有未关闭英雄
	if (nLoginMode == elogin_online)
	{
		checkCurHeroIsLocked();
	}
	
	return true;
}

void CHeroManager::restore()
{
	gServerGlobal->getDBEngineService()->unregisterHandler(this);

    // 杀死定时器
    ITimerService * pTimerService = gServerGlobal->getTimerService();
    if(pTimerService) {
        pTimerService->KillTimer(ETIMERID_READ_HEROADDITIONAL, this);
    }
}

void CHeroManager::release()
{

}

// 英雄数据更新到数据库
bool CHeroManager::updateHerosToDB()
{
	for (PLAYERHEROMAP::iterator iter = m_mapHero.begin(); iter != m_mapHero.end(); ++iter )
	{
		SHeroInfoFlags& hero = iter->second;
		if( hero.bIsChange == false )
			continue;

		updateHeroInfo(hero.sHeroInfo.nHeroID);
		hero.bIsChange = false;
	}

	return true;
}

SHeroInfo* CHeroManager::getHeroInfo(int nHeroID)
{
	PLAYERHEROMAP::iterator it = m_mapHero.find(nHeroID);
	if ( it == m_mapHero.end())
	{
		return NULL;
	}
	return &it->second.sHeroInfo;
}

SHeroInfo* CHeroManager::checkHeroInfo(int nHeroID)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL !");
		return NULL;
	}

	ISchemeHeroFreeWeekly* pSchemeHeroFreeWeekly = pSchemeCenter->getSchemeHeroFreeWeekly();
	if ( pSchemeHeroFreeWeekly == NULL )
	{
		ErrorLn(__FUNCTION__": pSchemeHeroFreeWeekly == NULL !");
		return NULL;
	}

	if (m_pMaster == NULL)
	{
		ErrorLn(__FUNCTION__": m_pMaster == NULL !");
		return NULL;
	}
	int nPlayerLv = m_pMaster->getIntProperty(PROPERTY_LEVEL);
	if (pSchemeHeroFreeWeekly->IsFreeHeroListChanged(nPlayerLv))
	{
		setHeroInfoByFreeWeekly();
		// TODO 通知客户端也重置一次

	}
	
	PLAYERHEROMAP::iterator it = m_mapHero.find(nHeroID);
	if ( it == m_mapHero.end())
	{
		return NULL;
	}
	if (it->second.bIsFree || isHeroHave(it->second.sHeroInfo.dwFlags))
	{
		return &it->second.sHeroInfo;
	}
	return NULL;
}

// 获取所有英雄
bool CHeroManager::getHeroArray(obuf & outData)
{
	// 添加周免英雄列表
	setHeroInfoByFreeWeekly();

	int nCount = m_mapHero.size();
	outData << nCount;
	PLAYERHEROMAP::iterator it = m_mapHero.begin();
	for (; it != m_mapHero.end(); ++it)
	{
		outData.push_back(&(it->second.sHeroInfo), sizeof(SHeroInfo));
	}

	return true;
}

bool CHeroManager::getPlayerCanUseHeroArray(obuf & outData)
{
	obuf obData;
	PLAYERHEROMAP::iterator it = m_mapHero.begin();
	int nCount = 0;
	for (; it != m_mapHero.end(); ++it)
	{
		// 周免或者已购买英雄
		if (it->second.bIsFree || isHeroHave(it->second.sHeroInfo.dwFlags))
		{
			obData.push_back(&(it->second.sHeroInfo), sizeof(SHeroInfo));
			nCount++;
		}
	}
	outData << nCount;
	outData.push_back(obData.data(), obData.size());

	return true;
}

PLAYERHEROMAP CHeroManager::getHeroMap()
{
	return m_mapHero;
}

// 购买新英雄，设置标志位
bool CHeroManager::setNewHero( SHeroInfo sNewHeroInfo )
{
	int nHeroID = sNewHeroInfo.nHeroID;
	PLAYERHEROMAP::iterator it = m_mapHero.find( nHeroID );

	SHeroInfoFlags sHeroInfoFlags;
	if ( it == m_mapHero.end() )
	{
		sHeroInfoFlags.sHeroInfo = sNewHeroInfo;
		sHeroInfoFlags.bIsChange = true;
		m_mapHero[nHeroID] = sHeroInfoFlags;
	}else
	{
		it->second.bIsChange = true;
		compareHeroInfo(sNewHeroInfo, it->second.sHeroInfo);
	}

	onUpdateChangedHeroInfo();

	return true;
}

bool CHeroManager::addNewHero( int nHeroID, int nLv, int nStar, DWORD dwDeadLine)
{
	PLAYERHEROMAP::iterator it = m_mapHero.find( nHeroID );

    DWORD dwFlags = 0;

	if ( it != m_mapHero.end() )
	{
		if (isHeroHave(it->second.sHeroInfo.dwFlags))
		{
			ErrorLn(__FUNCTION__": player is have this hero , nHeroID = " << nHeroID );
			return false;
		}
		else
		{
			// 置为已拥有英雄
			it->second.sHeroInfo.dwFlags &= ~HERO_FLAG_NOT_HAVE;
			it->second.bIsChange = true;
			it->second.sHeroInfo.dwUnlockTime = (DWORD)time(NULL);
		}
	}
	else
	{
		SHeroInfoFlags sHeroInfoFlags;
		sHeroInfoFlags.sHeroInfo.nHeroID = nHeroID;
		sHeroInfoFlags.sHeroInfo.nLevel = nLv;
		sHeroInfoFlags.sHeroInfo.nHeroStar = nStar;
		sHeroInfoFlags.sHeroInfo.dwDeadLine = dwDeadLine;
		// 置为已拥有英雄
		sHeroInfoFlags.sHeroInfo.dwFlags &= ~HERO_FLAG_NOT_HAVE;
		sHeroInfoFlags.bIsChange = true;
		sHeroInfoFlags.sHeroInfo.dwUnlockTime = (DWORD)time(NULL);
		m_mapHero[nHeroID] = sHeroInfoFlags;
		initOneHeroStarPrizeUnlockInfo(nHeroID);
	}

    onUpdateSocialHeroInofo();

	onUpdateChangedHeroInfo();

	activeTalentByHeroStar(nHeroID, 1);

    event_entity_buy_hero eventData;
    eventData.idActor = m_pMaster->getIntProperty(PROPERTY_ID);
    eventData.nHeroID = nHeroID;
    eventData.nHeroStar = nStar;
    eventData.dwFlags = dwFlags;

    rkt::IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
    if (pEventEngine)
    {
        pEventEngine->FireExecute(EVENT_GAME_ACTOR_ADD_HERO, SOURCE_TYPE_PERSON, 0, (LPCSTR)&eventData, sizeof(eventData));
    }

	return true;
}

bool CHeroManager::canAddHero( int nHeroID )
{
	// 判断玩家是否拥有这种英雄
	PLAYERHEROMAP::iterator it = m_mapHero.find( nHeroID );
	if ( it != m_mapHero.end() && isHeroHave(it->second.sHeroInfo.dwFlags))
		return false;

	// 判断配置表中是否有这种英雄
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return false;
	ISchemePersonModel* pPersonModel = pSchemeCenter->getSchemePersonModel();
	if ( pPersonModel == NULL )
		return false;
	return pPersonModel->isHaveThisHero(nHeroID);
}

bool CHeroManager::delHero(int nHeroID)
{
	// 不能删除当前使用英雄
	if (m_pMaster->getIntProperty(PROPERTY_VOCATION) == nHeroID)
	{
		return false;
	}

	PLAYERHEROMAP::iterator iter = m_mapHero.find(nHeroID);
	if (iter == m_mapHero.end())
	{
		return false;
	}
	memset(&iter->second.sHeroInfo, 0, sizeof(iter->second.sHeroInfo));
	iter->second.sHeroInfo.nHeroID = nHeroID;
	iter->second.sHeroInfo.dwFlags |= HERO_FLAG_NOT_HAVE;
	iter->second.bIsChange = true;

	//updateHeroInfo(nHeroID);

	m_mapHero[nHeroID].sHeroTalentList.TalentInfoVec.clear();
	m_mapHero[nHeroID].sHeroSkinList.SkinInfoVec.clear();
	m_mapHero[nHeroID].sHeroStarPrizeUnlockVec.heroStarPrizeUnlockVec.clear();
	m_mapHero[nHeroID].sHeroXPSkillList.clear();
	m_mapHero[nHeroID].sHeroXPFragmentList.clear();

	saveHeroSkinInfoToDB(nHeroID);
	updateHeroTalentToDB(nHeroID);
	updateHeroXPSkillFragmentToDB(nHeroID);
	updateHeroXPSkillToDB(nHeroID);

	onUpdateChangedHeroInfo();

	return true;
}

bool CHeroManager::setHeroExclusiveTalentEffect(int nHeroID )
{
	PLAYERHEROMAP::iterator itHero = m_mapHero.find(nHeroID);
	if ( itHero == m_mapHero.end() )
	{
		//TraceLn(__FUNCTION__": this hero is system or can't find this hero nHeroID = " << nHeroID );
		return false;
	}

	int nExclusive[DBDEF_EXCLUSIVE_MAXCOUNT] = {0};
	BYTE byActivated[DBDEF_EXCLUSIVE_MAXCOUNT] = {0};
	int nIntLen = sizeof(int);
	int nByteLen = sizeof(BYTE);
	for ( int i = 0; i < DBDEF_EXCLUSIVE_MAXCOUNT; ++i )
	{
		SHeroInfo sHeroInfo = itHero->second.sHeroInfo;
		memcpy(nExclusive + i, sHeroInfo.byExclusive + i * (nIntLen + nByteLen), nIntLen);
		memcpy(byActivated + i, sHeroInfo.byExclusive + i * (nIntLen + nByteLen) + nIntLen, nByteLen);
	}
	IEffectFactory* pEffectFactory = gServerGlobal->getEffectFactory();
	ISchemeHeroStarData* pStarData = gServerGlobal->getSchemeCenter()->getSchemeHeroStarData();
	if( pEffectFactory == NULL || pStarData == NULL)
	{
		ErrorLn(__FUNCTION__": pEffectFactory == NULL or pStarData == NULL !!!");
		return false;
	}

	UID uID = m_pMaster->getUID();
	for ( int i = 0; i < DBDEF_EXCLUSIVE_MAXCOUNT; ++i )
	{
		if( !nExclusive[i] || !byActivated[i] )
			continue;

		SHeroExclusiveTalentSchemeInfo sExclusiveData = pStarData->getHeroExclusive(nExclusive[i]);
		std::vector<int>::iterator it = sExclusiveData.vecEffect.begin();
		for (; it != sExclusiveData.vecEffect.end(); ++it)
		{
			IEffect* pEffect = pEffectFactory->BuildEffect(*it);
			if ( pEffect == NULL )
			{
				ErrorLn(__FUNCTION__": pEffect == NULL nActivated[i] = " << *it);
				continue;
			}
			EFFECT_CONTEXT context;
			context.pEntity = m_pMaster;
			context.uidOperator = uID;
			context.uidTarget = uID;
			context.nID = *it;
			pEffect->Start(&context, NULL);
			m_listExclusiveEffect.push_back(pEffect);
		}
	}
	return true;
}

bool CHeroManager::removeHeroExclusiveTalentEffect()
{
	ExclusiveEffectList::iterator it = m_listExclusiveEffect.begin();
	for (; it != m_listExclusiveEffect.end(); ++it )
	{
		(*it)->Stop();
		(*it)->Release();
	}
	m_listExclusiveEffect.clear();
	return true;
}

void CHeroManager::setHeroAllInfo(int nHeroID, SHeroInfo sValue)
{
	PLAYERHEROMAP::iterator it = m_mapHero.find(nHeroID);
	if ( it == m_mapHero.end())
	{
		return;
	}
	SHeroInfoFlags* pHeroInfoFlags = &it->second;
	SHeroInfo& sHeroInfo = pHeroInfoFlags->sHeroInfo;


	sHeroInfo.nSkinID = sValue.nSkinID == 0 ? sHeroInfo.nSkinID : sValue.nSkinID;
	sHeroInfo.nLevel = sValue.nLevel == 0 ? sHeroInfo.nLevel : sValue.nLevel;
	sHeroInfo.dwDeadLine = sValue.dwDeadLine == 0 ? sHeroInfo.dwDeadLine : sValue.dwDeadLine;
	sHeroInfo.dwFlags = sValue.dwFlags == 0 ? sHeroInfo.dwFlags : sValue.dwFlags;
	sHeroInfo.dwHeroExp = sValue.dwHeroExp == 0 ? sHeroInfo.dwHeroExp : sValue.dwHeroExp;
	sHeroInfo.dwWinNum = sValue.dwWinNum == 0 ? sHeroInfo.dwWinNum : sValue.dwWinNum;
	sHeroInfo.dwMatchNum = sValue.dwMatchNum == 0 ? sHeroInfo.dwMatchNum : sValue.dwMatchNum;
	sHeroInfo.nHeroStar = sValue.nHeroStar == 0 ? sHeroInfo.nHeroStar : sValue.nHeroStar;
	sHeroInfo.dwUseTime = sValue.dwUseTime == 0 ? sHeroInfo.dwUseTime : sValue.dwUseTime;

	pHeroInfoFlags->bIsChange = true;
}

void CHeroManager::setHeroOneInfo(int nHeroID, LONG lgValue, int nType, int nMode)
{
	IOSSLogService * pOSSLogService = gServerGlobal->getOSSLogService();
	if (pOSSLogService == NULL)
		return;
	int nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	
	PLAYERHEROMAP::iterator it = m_mapHero.find(nHeroID);
	if ( it == m_mapHero.end() || lgValue == 0 )
		return;

	SHeroInfoFlags* pHeroInfoFlags = &it->second;
	SHeroInfo* pHeroInfo = &pHeroInfoFlags->sHeroInfo;
	if ( pHeroInfoFlags == NULL || pHeroInfo == NULL )
		return;

	pHeroInfoFlags->bIsChange = true;
	switch( nType )
	{
	case ENHEROINFOTYPE_SKIN:
		{
			setMustSetTypeInfo(&pHeroInfo->nSkinID, lgValue, nMode);
		}
		break;
	case ENHEROINFOTYPE_LV:
		{
			if ( ENCHANGEDATAMODE_SET == nMode )
				setHeroLevel(nHeroID,lgValue) ;
			else if ( ENCHANGEDATAMODE_ADD == nMode )
				addHeroLevel(nHeroID,lgValue) ;
		}
		break;
	case ENHEROINFOTYPE_DEADLINE:
		{
			setMustSetTypeInfo(&pHeroInfo->dwDeadLine, lgValue, nMode);
		}
		break;
	case ENHEROINFOTYPE_FLAGS:
		{
			setMustSetTypeInfo(&pHeroInfo->dwFlags, lgValue, nMode);
		}
		break;
	case ENHEROINFOTYPE_EXP:
		{
			if ( ENCHANGEDATAMODE_SET == nMode && lgValue > 0)
				pHeroInfo->dwHeroExp = lgValue;
			else if ( ENCHANGEDATAMODE_ADD == nMode )
				addStaticHeroExp(nHeroID, lgValue);
		}
		break;
	case ENHEROINFOTYPE_WINNUM:
		{
			if ( ENCHANGEDATAMODE_SET == nMode )
				pHeroInfo->dwWinNum = lgValue;
			else if ( ENCHANGEDATAMODE_ADD == nMode )
				pHeroInfo->dwWinNum += lgValue;
		}
		break;
	case ENHEROINFOTYPE_MATCHNUM:
		{
			if ( ENCHANGEDATAMODE_SET == nMode )
				pHeroInfo->dwMatchNum = lgValue;
			else if ( ENCHANGEDATAMODE_ADD == nMode )
				pHeroInfo->dwMatchNum += lgValue;
		}
		break;
	case ENHEROINFOTYPE_STAR:
		{
			if ( ENCHANGEDATAMODE_SET == nMode )
				pHeroInfo->nHeroStar = lgValue;
			else if ( ENCHANGEDATAMODE_ADD == nMode )
				pHeroInfo->nHeroStar += lgValue;

			checkHeroStarUnlockPrize(nHeroID);

			// 英雄晋级
			pOSSLogService->addActorUpgradeLog(m_pBankPart->getFromGameWorldID(), m_pBankPart->getUserID(), nPDBID, 3, pHeroInfo->nHeroStar, time(NULL), nHeroID);
		
			event_entity_buy_hero eventData;
			eventData.idActor = nPDBID;
			eventData.nHeroID = nHeroID;
			eventData.nHeroStar = pHeroInfo->nHeroStar;
			eventData.dwFlags = pHeroInfo->dwFlags;
			rkt::IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
			if (pEventEngine)
			{
				pEventEngine->FireExecute(EVENT_GAME_ACTOR_ADD_HERO, SOURCE_TYPE_PERSON, 0, (LPCSTR)&eventData, sizeof(eventData));
			}
		}
		break;
	case ENHEROINFOTYPE_USETIME:
		{
			setMustSetTypeInfo(&pHeroInfo->dwUseTime, lgValue, nMode);
		}
		break;
	case ENHEROINFOTYPE_SCORE:
		{
			if ( ENCHANGEDATAMODE_SET == nMode )
				pHeroInfo->nHeroScore = lgValue;
			else if ( ENCHANGEDATAMODE_ADD == nMode )
				pHeroInfo->nHeroScore += lgValue;
		}
		break;
	default:
		break;
	}

	onUpdateChangedHeroInfo();
}

bool CHeroManager::addHeroCardInfo( DWORD* dwCardID, int* nValue, int nCount)
{
	if ( dwCardID == NULL || nValue == NULL )
	{
		ErrorLn(__FUNCTION__": dwCardID == NUll or nValue == NULL ");
		return false;
	}
	std::map<DWORD, int> mapNewCard;
	for ( int i = 0; i < nCount; ++i )
	{
		if ( dwCardID[i] == 0 )
			continue;

		std::map<DWORD, DBREQ_PARAM_HERO_CARD>::iterator it = m_mapHeroCard.find(dwCardID[i]);
		if ( (it != m_mapHeroCard.end() && it->second.nCardCount + nValue[i] < 0) || (it == m_mapHeroCard.end() && nValue[i] < 0))
		{
			return false;
		}
		std::map<DWORD, int>::iterator itNew = mapNewCard.find( dwCardID[i]);
		if ( itNew == mapNewCard.end() )
		{
			mapNewCard[dwCardID[i]] = nValue[i];
		}else
		{
			itNew->second += nValue[i];
		}
	}

	std::map<DWORD, int>::iterator itBegin =  mapNewCard.begin();
	DWORD dwNewCardArr[GETCARD_MAX_NUM] = {0};
	int nNewCount = 0;
	for (; itBegin != mapNewCard.end(); ++itBegin )
	{
		DWORD dwCardID = itBegin->first;
		int nCount = itBegin->second;
		std::map<DWORD, DBREQ_PARAM_HERO_CARD>::iterator it = m_mapHeroCard.find(dwCardID);
		if ( it == m_mapHeroCard.end() )
		{
			DBREQ_PARAM_HERO_CARD dbParam;
			dbParam.dwCardID = dwCardID;
			dbParam.nCardCount = nCount;
			m_mapHeroCard[dwCardID] = dbParam;
		}else
		{
			it->second.nCardCount += nCount;
		}
		dwNewCardArr[nNewCount] = dwCardID;
		nNewCount++;
	}
	updateHeroCardInfoToDB(dwNewCardArr, nNewCount);
	return true;
}

bool CHeroManager::canAddHeroCard(int nCardID, int nCount)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if( pSchemeCenter == NULL )
		return false;
	ISchemeHeroCard* pHeroCard = pSchemeCenter->getSchemeHeroCard();
	if( pHeroCard == NULL )
		return false;
	// TODO 判断卡牌是否达到上限

	// 查看配置表中是否有此卡牌id
	return pHeroCard->isHaveThisCard(nCardID);
}

bool CHeroManager::isCanUpStar(int nHeroID, DWORD dwCardID, int nCount)
{
	ISchemeHeroStarData* pStarData = gServerGlobal->getSchemeCenter()->getSchemeHeroStarData();
	SHeroInfo* pHeroInfo = getHeroInfo(nHeroID);
	if ( pStarData == NULL || pHeroInfo == NULL)
		return false;

	int nStarLimit = pStarData->getHeroStarLvLimit(nHeroID);

	std::map<DWORD, DBREQ_PARAM_HERO_CARD>::iterator it = m_mapHeroCard.find(dwCardID);
	if ( it == m_mapHeroCard.end() || it->second.nCardCount < nCount || pHeroInfo->nHeroStar >= nStarLimit)
	{
		return false;
	}
	return true;
}

// 添加或扣除英雄卡牌
bool CHeroManager::expendHeroCardInfo( DWORD dwCardID, int nCount)
{
	std::map<DWORD, DBREQ_PARAM_HERO_CARD>::iterator it = m_mapHeroCard.find(dwCardID);
	if ( it == m_mapHeroCard.end() )
	{
		return false;
	}

	it->second.nCardCount += nCount;

	updateHeroCardInfoToDB(&dwCardID,1);
	return true;
}

int CHeroManager::getOneCardCount( DWORD dwCardID )
{
	std::map<DWORD, DBREQ_PARAM_HERO_CARD>::iterator it = m_mapHeroCard.find(dwCardID);
	if ( it == m_mapHeroCard.end() )
	{
		return 0;
	}

	return it->second.nCardCount;
}

// 检查玩家能否使用英雄皮肤
bool CHeroManager::canUseHeroSkin(DWORD dwHeroID, DWORD dwHeroSkinID)
{
	if(m_mapHero.find(dwHeroID) == m_mapHero.end())
	{
		return false;
	}

	HERO_SKIN_INFO tmpSkinInfo;
	tmpSkinInfo.dwSkinID = dwHeroSkinID;

	HERO_SKIN_VEC::iterator iterEnd = m_mapHero[dwHeroID].sHeroSkinList.SkinInfoVec.end();
	HERO_SKIN_VEC::iterator iterResult = find(m_mapHero[dwHeroID].sHeroSkinList.SkinInfoVec.begin(), iterEnd, tmpSkinInfo);
	if (iterEnd == iterResult)
	{
		return false;
	}

	DWORD dwCurrentTime = time(NULL);
	bool bForeverSkin = iterResult->dwDurationTime == PERMANENT_SKIN_TIME;

	if(!bForeverSkin && dwCurrentTime > iterResult->dwDurationTime) 
	{
		return false;
	}

	return true;	
}

int CHeroManager::fillHeroInfo( obuf &obData )
{
	ISchemeHeroStarData* pStarData = gServerGlobal->getSchemeCenter()->getSchemeHeroStarData();
	if( pStarData == NULL)
		return 0;

	PLAYERHEROMAP::iterator it = m_mapHero.begin();
	std::list<SHeroInfo> listHero;
	for (; it != m_mapHero.end(); ++it )
	{
		listHero.push_back(it->second.sHeroInfo);
	}
	listHero.sort(lessMark);
	//sort( listHero.begin(), listHero.end(),lessMark);

	int nCount = 0;
	std::list<SHeroInfo>::iterator itBegin = listHero.begin();
	for (; itBegin != listHero.end(); ++itBegin )
	{
		if ( nCount >= STRONGEST_HERO_COUNT)
		{
			break;
		}

		SHeroInfo sHeroInfo = *itBegin;
		int nMatchNum = sHeroInfo.dwMatchNum;
		msg_entity_hero_info heroInfo;
		heroInfo.dwExp = sHeroInfo.dwHeroExp;
		heroInfo.dwGameCount = nMatchNum;
		heroInfo.nHeroID = sHeroInfo.nHeroID;
		heroInfo.nLv = sHeroInfo.nLevel;
		heroInfo.byStarLv = sHeroInfo.nHeroStar;
		heroInfo.dwScore = sHeroInfo.nHeroScore;
		if ( nMatchNum == 0 )
		{
			heroInfo.nWinPro = 0;
		}else
		{
			heroInfo.nWinPro = (sHeroInfo.dwWinNum*100)/(sHeroInfo.dwMatchNum);
		}
		heroInfo.bIsBuy = isHeroHave(sHeroInfo.dwFlags);
		heroInfo.dwUnlockTime = sHeroInfo.dwUnlockTime;

		obData.push_back( &heroInfo, sizeof(heroInfo) );
		++nCount;
	}

	return nCount;

}

SHeroInfo CHeroManager::getNewHeroInfo( int nType, int nStar )
{
	SHeroInfo sHeroInfo;
	switch( nType )
	{
	case ENGETHEROWAY_NORMAL:
		{
			sHeroInfo = getNewHeroNormalWay();
		}
		break;;
	default:
		break;
	}
	setNewHero(sHeroInfo);

	return sHeroInfo;
}

int CHeroManager::getHeroCount()
{
	return m_mapHero.size();
}

// 获取永久英雄个数
int	CHeroManager::getPermanentHeroCount()
{
	int nCount = 0;
	for (PLAYERHEROMAP::iterator itBegin = m_mapHero.begin(); itBegin != m_mapHero.end(); ++itBegin )
	{
		SHeroInfoFlags& sHeroInfoFlags = itBegin->second;
		if( isHeroHave(sHeroInfoFlags.sHeroInfo.dwFlags) && sHeroInfoFlags.sHeroInfo.dwDeadLine == 0)
			++nCount;
	}

	return nCount;
}

int CHeroManager::getAppointHeroStarCount(int nStarLevel)
{
    int nCount = 0;
    for (PLAYERHEROMAP::iterator itBegin = m_mapHero.begin(); itBegin != m_mapHero.end(); ++itBegin )
    {
        SHeroInfoFlags& sHeroInfoFlags = itBegin->second;
        if( isHeroHave(sHeroInfoFlags.sHeroInfo.dwFlags) && sHeroInfoFlags.sHeroInfo.dwDeadLine == 0 && nStarLevel <= sHeroInfoFlags.sHeroInfo.nHeroStar )
        {
             ++nCount;
        }
    }
    return nCount;
}

// 获取皮肤总个数
int CHeroManager::getTotalSkinCount()
{
	int nCount = 0;
	for (PLAYERHEROMAP::iterator itBegin = m_mapHero.begin(); itBegin != m_mapHero.end(); ++itBegin )
	{
		SHeroInfoFlags& sHeroInfoFlags = itBegin->second;
		size_t nSize = sHeroInfoFlags.sHeroSkinList.SkinInfoVec.size();
		for (int i = 0; i < nSize; ++i)
		{
			HERO_SKIN_INFO& skinInfo = sHeroInfoFlags.sHeroSkinList.SkinInfoVec[i];
			if (skinInfo.dwSkinID > 0 && skinInfo.dwDurationTime == PERMANENT_SKIN_TIME)
				++nCount;
		}
	}

	return nCount;
}

// 获取达到等级英雄个数
int	CHeroManager::getHeroCountByLevel(WORD nLevel)
{
	int nCount = 0;
	for (PLAYERHEROMAP::iterator itBegin = m_mapHero.begin(); itBegin != m_mapHero.end(); ++itBegin )
	{
		SHeroInfoFlags& sHeroInfoFlags = itBegin->second;
		if(sHeroInfoFlags.sHeroInfo.nHeroStar >= nLevel)
			++nCount;
	}

	return nCount;

}

bool CHeroManager::getHeroCardInfo()
{
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL || m_pMaster == NULL)
		return false;

	DWORD dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);

	DBREQ_REQUEST_GET_HERO_CARD dbRequest;
	dbRequest.dwActorID = dwActorID;

	if ( pDBEngine->executeSP(GAMEDB_REQUEST_GET_HERO_CARD, dwActorID, (LPCSTR)&dbRequest, sizeof(dbRequest), static_cast<IDBRetSink*>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_GET_HERO_CARD failed");
		return false;
	}

	return true;
}

void CHeroManager::compareHeroInfo(SHeroInfo sNewHero, SHeroInfo& sOldHero)
{
	if ( sOldHero.nHeroID != sNewHero.nHeroID )
	{
		WarningLn(__FUNCTION__": two different hero can't fuse nOldID = " << sOldHero.nHeroID << ", nNewID = " << sNewHero.nHeroID);
		return;
	}

	int nOldStar = sOldHero.nHeroStar;
	int nNewStar = sNewHero.nHeroStar;
	sOldHero.nHeroStar = nOldStar < nNewStar ? nNewStar : nOldStar;
	sOldHero.nLevel = sOldHero.nLevel < sNewHero.nLevel ? sNewHero.nLevel : sOldHero.nLevel;
	sOldHero.dwFlags = sNewHero.dwFlags;

	ISchemeHeroStarData* pHeroStarData = gServerGlobal->getSchemeCenter()->getSchemeHeroStarData();
	if( pHeroStarData == NULL)
		return;
	SHeroStarSchemeInfo sStarData = pHeroStarData->getHeroStarData( sOldHero.nHeroID, sOldHero.nHeroStar);
	int nExclusiveMaxCount = sStarData.nExclusiveTalentMaxCount;

	//// TODO获得当前已经拥有的专属天赋
	//int nOldExclusiveID[DBDEF_EXCLUSIVE_MAXCOUNT] = {0};
	//BYTE byOldFlags[DBDEF_EXCLUSIVE_MAXCOUNT] = {0};
	//int nOldCount = pariseExclusive(sOldHero.byExclusive, nOldExclusiveID, byOldFlags, DBDEF_EXCLUSIVE_MAXCOUNT);

	//int nNewExclusiveID[DBDEF_EXCLUSIVE_MAXCOUNT] = {0};
	//BYTE byNewFlags[DBDEF_EXCLUSIVE_MAXCOUNT] = {0};
	//int nNewCount = int nNewCount = pariseExclusive( sNewHero.byExclusive, nNewExclusiveID, byNewFlags, DBDEF_EXCLUSIVE_MAXCOUNT);

	//// 两个英雄合并的还是有问题


	// 临时处理特权全部覆盖
	memcpy( sOldHero.byExclusive, sNewHero.byExclusive, sizeof(sOldHero.byExclusive));
}

void CHeroManager::setMustSetTypeInfo(DWORD* pData, LONG lgValue, int nMode)
{
	if ( ENCHANGEDATAMODE_SET == nMode)
		*pData = lgValue;
	else if ( ENCHANGEDATAMODE_ADD == nMode )
		WarningLn(__FUNCTION__": this data muset set can't add!!!");
}
void CHeroManager::setMustSetTypeInfo(WORD* pData, WORD wValue, int nMode)
{
	if ( ENCHANGEDATAMODE_SET == nMode)
		*pData = wValue;
	else if ( ENCHANGEDATAMODE_ADD == nMode )
		WarningLn(__FUNCTION__": this data muset set can't add!!!");
}
void CHeroManager::setMustSetTypeInfo(BYTE* pData, BYTE byValue, int nMode)
{
	if ( ENCHANGEDATAMODE_SET == nMode)
		*pData = byValue;
	else if ( ENCHANGEDATAMODE_ADD == nMode )
		WarningLn(__FUNCTION__": this data muset set can't add!!!");
}

void CHeroManager::addStaticHeroExp(int nHeroID, LONG lgValue)
{
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if(pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL!")
			return;
	}
	ISchemeHeroStarData* pHeroStar = pSchemeCenter->getSchemeHeroStarData();
	ISchemeLevelUpgrade* pLvUpgrade = pSchemeCenter->getLevelUpgradeSheme();
	if( pHeroStar == NULL || pLvUpgrade == NULL)
	{
		return;
	}

	SHeroInfo* pHeroInfo = getHeroInfo(nHeroID);
	if( pHeroInfo == NULL )
		return;

	DWORD &nHeroExp = pHeroInfo->dwHeroExp;
	int nHeroStar = pHeroInfo->nHeroStar;
	int nExpTypeID = pHeroStar->getUpgradeNeedExpTypeID(nHeroID, nHeroStar);
	LONG lgNeedExp = pLvUpgrade->GetNextLevelExp(pHeroInfo->nLevel, ENEXPTYPE_STATICHEROEXP, nExpTypeID);
	SHeroStarSchemeInfo sStarInfo = pHeroStar->getHeroStarData(nHeroID, nHeroStar);

	if ( lgNeedExp == 0 || sStarInfo.nLimitLV == 0 || pHeroInfo->nLevel >= sStarInfo.nLimitLV )
	{
		return;
	}

	// 防沉迷衰减
	IAntiAddictServer * pAntiAddict = gServerGlobal->getAntiAddictServer();
	if ( pAntiAddict && lgValue > 0)
	{
		UID uid = m_pMaster->getUID();
		int reduce = pAntiAddict->getReduceRate(uid);
		if ( reduce < 100 )
		{
			lgValue = ( lgValue * reduce ) / 100;
			char szParam[256];
			ssprintf_s(szParam, sizeof(szParam), a2utf8("%d;防沉迷状态,收益衰减至%d％,实际获得英雄经验%d"), CHAT_CHANNEL_SYSTEM, reduce, lgValue);
			g_EHelper.showSystemMessage(uid, CHAT_TIP_ANTIADDICT_INCOME_REDUCE, szParam);
		}
	}

	LONG lgExpResult = nHeroExp  + lgValue;
	if ( lgExpResult <= 0 )
	{
		nHeroExp = 0;
	}else if ( lgExpResult > 0 && lgExpResult < lgNeedExp )
	{
		nHeroExp = lgExpResult;
	}else if ( lgExpResult > 0 && lgExpResult >= lgNeedExp )
	{
		nHeroExp = lgExpResult;
		while( nHeroExp >= lgNeedExp && pHeroInfo->nLevel < sStarInfo.nLimitLV )
		{
			addHeroLevel(nHeroID,1);
			nHeroExp = nHeroExp - lgNeedExp;
			int nExpTypeID = pHeroStar->getUpgradeNeedExpTypeID(nHeroID, nHeroStar);
			lgNeedExp = pLvUpgrade->GetNextLevelExp(pHeroInfo->nLevel, ENEXPTYPE_STATICHEROEXP, nExpTypeID);
		}

		IOSSLogService * pOSSLogServer = gServerGlobal->getOSSLogService();
		if ( pOSSLogServer != NULL)
		{
			PDBID pDBID = m_pMaster->getIntProperty(PROPERTY_ID);
			// 记录等级信息
			char szlogDesc[215] = {0};
			ssprintf_s( szlogDesc, sizeof(szlogDesc), a2utf8("英雄获得经验%d 升级到%d"), lgValue, pHeroInfo->nLevel);
			pOSSLogServer->addNumericLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_PERSON, pDBID, 0, pHeroInfo->nLevel, OSS_NUMERIC_HEROLV, OSS_RESOURCE_OPT_ADD,OSSRES_ADD_GETEXPUPGRADE, time(NULL),szlogDesc, nHeroID);
		}
	}

}

SHeroInfo CHeroManager::getNewHeroNormalWay()
{
	ISchemePersonModel* pModel = gServerGlobal->getSchemeCenter()->getSchemePersonModel();
	ISchemeHeroStarData* pStarData = gServerGlobal->getSchemeCenter()->getSchemeHeroStarData();
	if( pModel == NULL || pStarData == NULL)
		return SHeroInfo();

	// 临时规定可以随机任何类型的英雄
	SHeroInfo sHeroInfo;
	sHeroInfo.nHeroID = rand()%(VOCATION_MAX - 1) + 1;
	sHeroInfo.nLevel = 1;
	// 临时规定最高获得4星英雄
	sHeroInfo.nHeroStar = rand()% 4 + 1;

    const PersonSkinModelItem * pPersonSkinModelItem = gServerGlobal->getSchemeCenter()->getSchemePersonModel()->getPersonSkinModelItem(sHeroInfo.nHeroID); 
    if(pPersonSkinModelItem)
    {
	    sHeroInfo.nSkinID = pPersonSkinModelItem->nDefaultSkinID;
    }

	SHeroStarSchemeInfo sStarData = pStarData->getHeroStarData(sHeroInfo.nHeroID, sHeroInfo.nHeroStar);
	int nExclusiveCount = sStarData.nExclusiveTalentMaxCount;
	sStarData.chExclusiveTalent;

	if ( nExclusiveCount == 0 )
	{
		return sHeroInfo;
	}else
	{
		int nArr[12] = {0};

		int nIDCount = parseIntArray(sStarData.chExclusiveTalent, nArr, 12);
		if ( nIDCount == 0 )
			return sHeroInfo;
		// 先随机获得多少个特权
		int nCount = rand()%(nExclusiveCount + 1);
		if ( nCount == 0 )
			return sHeroInfo;

		// 如果随机到的特权大于可随机的数量是return
		if ( nCount > nIDCount )
		{
			ErrorLn(__FUNCTION__": need Count > can have count nCount = " << nCount << ", nIDCount = " << nIDCount);
			return SHeroInfo();
		}
		// 此容器是为了排查是否随机到重复的数字
		std::map<int, int> mapRandomNum;
		while( nCount )
		{
			int nIndex = rand()%nIDCount;
			std::map<int, int>::iterator it = mapRandomNum.find(nIndex);
			if ( it == mapRandomNum.end())
			{
				mapRandomNum[nIndex] = nIndex;
				nCount--;
			}
		}
		std::map<int, int>::iterator itIndex = mapRandomNum.begin();
		int nIndex = 0;
		int nIntLen = sizeof(int);
		int nByLen = sizeof(BYTE);
		for (; itIndex != mapRandomNum.end(); ++itIndex )
		{
			memcpy( sHeroInfo.byExclusive + nIndex* (nIntLen + nByLen), nArr + itIndex->first, nIntLen);
			nIndex++;
		}
	}
	return sHeroInfo;
}

int CHeroManager::pariseExclusive(BYTE* pArrExclusive, int* pArrID, BYTE* pArrFlags, int nSize )
{
	int nIntLen = sizeof(int);
	int nByLen = sizeof(BYTE);

	int nCount = 0;
	for (int i = 0; i < nSize; ++i )
	{
		memcpy( pArrID + i, pArrExclusive + i*(nIntLen + nByLen), nIntLen);
		memcpy( pArrFlags + i, pArrExclusive + i*(nIntLen + nByLen) + nIntLen, nByLen );
		if ( pArrID[i] != 0 )
			nCount++;
	}

	return nCount;
}

bool CHeroManager::updateHeroInfo(int nHeroID)
{
	IDBEngineService* pDBEngine = gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
		return false;

	SHeroInfo* pHeroInfo = getHeroInfo(nHeroID);
	if(pHeroInfo == NULL)
		return false;

	DBREQ_PARAM_UPDATE_HEROINFO dbHeroInfo;

	DWORD dwActorID         =   m_pMaster->getIntProperty(PROPERTY_ID);
	dbHeroInfo.dwActorID    =   dwActorID;
	dbHeroInfo.nHeroID      =   nHeroID;
	dbHeroInfo.nSkinID      =   pHeroInfo->nSkinID;
	dbHeroInfo.nLevel       =   pHeroInfo->nLevel;
	dbHeroInfo.dwDeadLine   =   pHeroInfo->dwDeadLine;
	dbHeroInfo.dwFlags      =   pHeroInfo->dwFlags;
	dbHeroInfo.dwHeroExp    =   pHeroInfo->dwHeroExp;
	dbHeroInfo.dwWinNum     =   pHeroInfo->dwWinNum;
	dbHeroInfo.dwMatchCount =   pHeroInfo->dwMatchNum;
	dbHeroInfo.nHeroStar    =   pHeroInfo->nHeroStar;
	dbHeroInfo.dwUseTime    =   pHeroInfo->dwUseTime;
	dbHeroInfo.nHeroScore   =   pHeroInfo->nHeroScore;
	dbHeroInfo.nUseXPID = pHeroInfo->nUseXPID;
	dbHeroInfo.dwUnlockTime =	pHeroInfo->dwUnlockTime;
	memcpy( dbHeroInfo.byExclusive, pHeroInfo->byExclusive, sizeof(dbHeroInfo.byExclusive));

	obuf obData;
	obData.push_back(&dbHeroInfo, sizeof(dbHeroInfo));

	if ( pDBEngine->executeSP(GAMEDB_REQUEST_UPDATEHERO, dwActorID, obData.data(), obData.size(), static_cast<IDBRetSink*>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_UPDATEHERO failed");
		return false;
	}
	return true;
}

void CHeroManager::sendCardInfoToClient()
{
	obuf obData;
	SNetMsgHead head;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule    = MSG_MODULEID_ENTITY;
	head.byKeyAction    = SC_MSG_ENTITY_UPDATE_HERO_CARD;

	// 部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_BANK;

	obData << head << entityHead;

	int nCount = 0;
	obuf obCardData;
	std::map<DWORD, DBREQ_PARAM_HERO_CARD>::iterator itBegin = m_mapHeroCard.begin();
	for (; itBegin != m_mapHeroCard.end(); ++itBegin )
	{
		DBREQ_PARAM_HERO_CARD dbParam = itBegin->second;
		obCardData.push_back(&dbParam, sizeof(dbParam));
		nCount++;
	}
	obData.push_back( &nCount, sizeof(nCount));
	obData.push_back( obCardData.data(), obCardData.size());

	((__IPlayerRole*)m_pMaster)->send2Client(obData.data(), obData.size());
}

void CHeroManager::sendCardInfoToClient(DWORD dwCardID[], int nCount)
{
	obuf obData;
	SNetMsgHead head;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule    = MSG_MODULEID_ENTITY;
	head.byKeyAction    = SC_MSG_ENTITY_UPDATE_HERO_CARD;

	// 部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_BANK;

	obData << head << entityHead;
	int nChangeCount = 0;
	obuf obCardData;
	for ( int i = 0; i < nCount; ++i )
	{
		std::map<DWORD, DBREQ_PARAM_HERO_CARD>::iterator it = m_mapHeroCard.find(dwCardID[i]);
		if ( it == m_mapHeroCard.end())
			continue;
		obCardData.push_back( &it->second, sizeof(DBREQ_PARAM_HERO_CARD));
		nChangeCount++;
	}
	obData.push_back(&nChangeCount, sizeof(nChangeCount));
	obData.push_back(obCardData.data(), obCardData.size());

	((__IPlayerRole*)m_pMaster)->send2Client(obData.data(), obData.size());
}

bool CHeroManager::updateHeroCardInfoToDB( DWORD nHeroID[], int nCount )
{
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
		return false;

	obuf obData;
	obuf obParam;
	int nNeedUpdateCount = 0;
	for ( int i = 0; i < nCount; ++i )
	{
		std::map<DWORD, DBREQ_PARAM_HERO_CARD>::iterator it = m_mapHeroCard.find( nHeroID[i] );
		if ( it == m_mapHeroCard.end() )
		{
			continue;
		}

		obParam.push_back( &it->second, sizeof(it->second));
		DBREQ_PARAM_HERO_CARD dbReqInfo = it->second;
		nNeedUpdateCount++;
	}

	DBREQ_REQUEST_SAVE_HERO_CARD dbReqInfo;
	dbReqInfo.dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);
	dbReqInfo.nCount = nNeedUpdateCount;
	obData.push_back(&dbReqInfo, sizeof(dbReqInfo));
	DWORD dwBytes = dbReqInfo.nCount * sizeof(DBREQ_PARAM_HERO_CARD);
	obData.push_back(&dwBytes, sizeof(dwBytes));
	obData.push_back( obParam.data(), obParam.size());
	if ( pDBEngine->executeSP(GAMEDB_REQUEST_SAVE_HERO_CARD, dbReqInfo.dwActorID, obData.data(), obData.size(), static_cast<IDBRetSink*>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_SAVE_HERO_CARD failed");
		return false;
	}

	sendCardInfoToClient(nHeroID, nCount);
	return true;
}

void CHeroManager::onGAMEDB_REQUEST_GET_HERO_CARD(int nQueueIndex,void * pOutData,int nOutLen)
{
	if ( pOutData == NULL || m_pMaster == NULL )
		return;

	DWORD dwActorID = (DWORD)nQueueIndex;
	if (dwActorID != m_pMaster->getIntProperty(PROPERTY_ID))
	{
		ErrorLn(__FUNCTION__ << " Invilad ActorID !!! ");
		return;
	}

	if ( nOutLen < sizeof(DBREQ_REQUEST_GET_HERO_CARD))
	{
		ErrorLn(__FUNCTION__": nOutLen < sizeof(DBREQ_REQUEST_GET_HERO_CARD) failed!!");
		return;
	}

	DBREQ_RESULT_GET_HERO_CARD* pData = (DBREQ_RESULT_GET_HERO_CARD*)(pOutData);

	int nCount = pData->nCount;
	if ( nOutLen - sizeof(DBREQ_RESULT_GET_HERO_CARD) < nCount * sizeof(DBREQ_PARAM_HERO_CARD) )
	{
		ErrorLn(__FUNCTION__": nOutLen - sizeof(DBREQ_RESULT_GET_HERO_CARD) < nCount * sizeof(DBREQ_PARAM_HERO_CARD) !!!");
		return;
	}

	DBREQ_PARAM_HERO_CARD* pParamCard = (DBREQ_PARAM_HERO_CARD*)(pData + 1);
	for (int i = 0; i < nCount; ++i )
	{
		m_mapHeroCard[pParamCard->dwCardID] = *pParamCard;
		pParamCard++;
	}

	sendCardInfoToClient();
}


void CHeroManager::onUpdateSocialHeroInofo()
{
    obuf obData;
    int nCount =0;
    obuf obHero;
    for (PLAYERHEROMAP::iterator itBegin = m_mapHero.begin(); itBegin != m_mapHero.end(); ++itBegin )
    {
        SHeroInfoFlags& sHeroInfoFlags = itBegin->second;
        if( sHeroInfoFlags.bIsChange == false )
            continue;
        obHero.push_back(&sHeroInfoFlags.sHeroInfo, sizeof(sHeroInfoFlags.sHeroInfo));
        nCount++;
    }
    if ( nCount <= 0 )
    {
        return;   
    }

    DWORD dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);
    obData.push_back(&dwActorID,sizeof(dwActorID));
    obData.push_back(&nCount, sizeof(int));
    obData.push_back(obHero.data(), obHero.size());

    ICenterConnectorService* pCenterConnectorService =  gServerGlobal->getCenterConnectorService();
    if(pCenterConnectorService)
    {
        // 子消息相关
        SMsgMatchDataSubMsg subMsg;
        subMsg.dwMsgCode = MSG_MATCH_UPDATE_HERO_CANUSE;

        // 数据打包
        obuf256 obMsg;
        obMsg<<subMsg;
        obMsg.push_back(obData.data(), obData.size());

        pCenterConnectorService->sendToSocial(MSG_MODULEID_MATCH, MSG_MATCH_SERVICE_MODULEMSG, (const char*)obMsg.data(), obMsg.size());
    }
}


void CHeroManager::onUpdateChangedHeroInfo()
{
	obuf obData;
	SNetMsgHead head;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule    = MSG_MODULEID_ENTITY;
	head.byKeyAction    = SC_MSG_ENTITY_UPDATE_HERO_INFO;
	// 部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_BANK;

	int nCount = 0;
	obData << head << entityHead;

	obuf obHero;
	for (PLAYERHEROMAP::iterator itBegin = m_mapHero.begin(); itBegin != m_mapHero.end(); ++itBegin )
	{
		SHeroInfoFlags& sHeroInfoFlags = itBegin->second;
		if( sHeroInfoFlags.bIsChange == false )
			continue;
		obHero.push_back(&sHeroInfoFlags.sHeroInfo, sizeof(sHeroInfoFlags.sHeroInfo));
		nCount++;
	}
	if ( nCount != 0 )
	{
		obData.push_back(&nCount, sizeof(int));
		obData.push_back(obHero.data(), obHero.size());
		((__IPlayerRole*)m_pMaster)->send2Client(obData.data(), obData.size());
	}
}

void CHeroManager::checkCurHeroIsLocked()
{
	if(m_pBankPart == NULL || m_pMaster == NULL)
	{
		ErrorLn(__FUNCTION__": m_pBankPart == NULL || m_pMaster == NULL");
		return;
	}
	SPlayerInfo* pPlayerInfo = m_pBankPart->getPlayerInfo();
	__IEntityProperty* pProperty = (__IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
	if (pPlayerInfo == NULL || pProperty == NULL)
	{
		ErrorLn(__FUNCTION__": pPlayerInfo == NULL || pProperty == NULL");
	}

	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL");
		return;
	}
	ISchemePersonModel * pPersonModel = pSchemeCenter->getSchemePersonModel();
	if (pPersonModel == NULL)
	{
		ErrorLn(__FUNCTION__": pPersonModel == NULL");
		return;
	}

	// 未关闭则返回
	const PersonSkinModelItem * pPersonSkinModelItem = pPersonModel->getPersonSkinModelItem(pPlayerInfo->nHeroID);
	if (pPersonSkinModelItem != NULL)
		return;

	// 随机一个已有、未关闭的英雄
	vector<int> vecPossessHero;
	PLAYERHEROMAP::iterator it = m_mapHero.begin();
	for (it; it != m_mapHero.end(); ++it)
	{
		int nHeroID = it->first;
		if (!isHeroHave(nHeroID))
			continue;
		if (pPersonModel->getPersonSkinModelItem(nHeroID) == NULL)
			continue;
		vecPossessHero.push_back(nHeroID);
	}
	if (vecPossessHero.size() <= 0)
		return;
	int nRandIndex = mt_range_rand(0, vecPossessHero.size() - 1);
	int nRandHeroID = vecPossessHero[nRandIndex];
	if (nRandHeroID <= 0)
		return;

	// 设置为新的随机英雄、皮肤
	const PersonSkinModelItem * pHeroItem = pPersonModel->getPersonSkinModelItem(nRandHeroID);
	int nRandSkinID = pHeroItem->nDefaultSkinID;
	pPlayerInfo->nHeroID = nRandHeroID;
	pPlayerInfo->nSkinID = nRandSkinID;
	pProperty->setProperty_Integer(PROPERTY_VOCATION, nRandHeroID);
	pProperty->setProperty_Integer(PROPERTY_SKIN, nRandSkinID);
}

void CHeroManager::setHeroInfoByFreeWeekly()
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL !");
		return;
	}

	ISchemeHeroFreeWeekly* pSchemeHeroFreeWeekly = pSchemeCenter->getSchemeHeroFreeWeekly();
	if ( pSchemeHeroFreeWeekly == NULL )
	{
		ErrorLn(__FUNCTION__": pSchemeHeroFreeWeekly == NULL !");
		return;
	}

	if (m_pMaster == NULL)
	{
		ErrorLn(__FUNCTION__": m_pMaster == NULL !");
		return;
	}
	int nPlayerLv = m_pMaster->getIntProperty(PROPERTY_LEVEL);
	const int nFreeHeroArrayLen = 2 * HEROFREEWEEKLY_FREEID;
	int arrayFreeHero[nFreeHeroArrayLen] = {0};
	int nFreeHeroCnt = pSchemeHeroFreeWeekly->GetFreeHeroListByLv(arrayFreeHero, nFreeHeroArrayLen, nPlayerLv);
	
	// 置位周免英雄
	for (int i = 0; i < nFreeHeroCnt; i++)
	{
		PLAYERHEROMAP::iterator iter = m_mapHero.find( arrayFreeHero[i] );

		if (iter == m_mapHero.end())
		{
			SHeroInfoFlags sHeroInfoFlags;

			sHeroInfoFlags.sHeroInfo.dwFlags |= HERO_FLAG_NOT_HAVE;		// 置为非拥有英雄
			sHeroInfoFlags.sHeroInfo.nHeroID = arrayFreeHero[i];
			sHeroInfoFlags.sHeroInfo.nLevel = 1;
			sHeroInfoFlags.sHeroInfo.nHeroStar = 1;
			sHeroInfoFlags.bIsFree = true;								// 置为周免英雄

			m_mapHero[arrayFreeHero[i]] = sHeroInfoFlags;
		}
		else
		{
			iter->second.bIsFree = true;								// 置为周免英雄
		}
	}
}

//bool CHeroManager::initHeroLevelPrizeUnlockInfo()
//{
//	if(m_mapHero.size() <= 0)
//	{
//		return false;
//	}
//
//	//初始过有数据返回
//	PLAYERHEROMAP::iterator iterHeroMap  = m_mapHero.begin();
//	for(;iterHeroMap!=m_mapHero.end();++iterHeroMap)
//	{
//		if(iterHeroMap->second.sHeroLevelPrizeUnlockVec.heroLevelPrizeUnlockVec.size() > 0)
//		{
//			return true;
//		}
//	}
//
//	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
//	if (pSchemeCenter == NULL)
//	{
//		ErrorLn(__FUNCTION__": pSchemeCenter == NULL !");
//		return false;
//	}
//
//	ISchemePrizeUnlock* pSchemePrizeUnlock = pSchemeCenter->getSchemePrizeUnlock();
//	if ( pSchemePrizeUnlock == NULL )
//	{
//		ErrorLn(__FUNCTION__": pSchemePrizeUnlock == NULL !");
//		return false;
//	}
//
//	SHeroLevelPrizeUnlockMap* pMapHeroLevelUnlock = pSchemePrizeUnlock->getHeroLevelPrizeUnlockInfo();
//	if(pMapHeroLevelUnlock == NULL||pMapHeroLevelUnlock->empty())
//	{
//		ErrorLn(__FUNCTION__": getHeroPrizeUnlockInfo Fail !,pHeroUnlock ==NULL");
//		return false;
//	}
//
//	bool bHasPrizeInfo = false;
//	iterHeroMap  = m_mapHero.begin();
//	for(;iterHeroMap!=m_mapHero.end();++iterHeroMap)
//	{
//		int nHeroId  = iterHeroMap->first;
//		iterHeroMap->second.sHeroLevelPrizeUnlockVec.heroLevelPrizeUnlockVec.clear();
//
//		SHeroLevelPrizeUnlockMap::iterator iterUnlockResult = pMapHeroLevelUnlock->find(nHeroId) ;
//		if(iterUnlockResult == pMapHeroLevelUnlock->end())
//		{
//			continue;;
//		}
//
//		if(iterUnlockResult->second.heroLevelPrizeUnlockVec.size() <= 0)
//		{
//			continue;
//		}
//
//		bHasPrizeInfo = true;
//		int nHeroLevel = iterHeroMap->second.sHeroInfo.nLevel;      
//		iterHeroMap->second.sHeroLevelPrizeUnlockVec.heroLevelPrizeUnlockVec = iterUnlockResult->second.heroLevelPrizeUnlockVec;
//
//		vector<SHeroLevelPrizeUnlockItem> ::iterator iterUnlockVec  =  iterHeroMap->second.sHeroLevelPrizeUnlockVec.heroLevelPrizeUnlockVec.begin();
//		vector<SHeroLevelPrizeUnlockItem> ::iterator iteUnlockVecEnd  =  iterHeroMap->second.sHeroLevelPrizeUnlockVec.heroLevelPrizeUnlockVec.end();
//
//		for(;iterUnlockVec!=iteUnlockVecEnd;++iterUnlockVec)
//		{
//			if((*iterUnlockVec).bUnlock)
//			{
//				continue;
//			}
//
//			if(nHeroLevel >= (*iterUnlockVec).nUnlockLevel)
//			{
//				(*iterUnlockVec).bUnlock = true;
//			}
//		}
//	}
//
//	bool bInitSuccess = bHasPrizeInfo;
//	return bInitSuccess;
//}

//void CHeroManager::checkHeroLevelUnlockPrize(int nHeroId)
//{
//	if(m_pMaster == NULL)
//	{
//		return;
//	}
//
//	if(m_pBankPart == NULL)
//	{
//		ErrorLn(__FUNCTION__": m_pPlayerBank == NULL");    
//		return;
//	}
//
//	PLAYERHEROMAP ::iterator iterHeroMap = m_mapHero.find(nHeroId);
//	if(iterHeroMap == m_mapHero.end())
//	{
//		return;
//	}
//
//	if(iterHeroMap->second.sHeroLevelPrizeUnlockVec.heroLevelPrizeUnlockVec.size() <=0 )
//	{
//		return;
//	}
//
//	int nHeroLevel = iterHeroMap->second.sHeroInfo.nLevel;
//	DWORD uidSelf = m_pMaster->getUID();
//
//	vector<SHeroLevelPrizeUnlockItem> ::iterator iterUnlockVec  =  iterHeroMap->second.sHeroLevelPrizeUnlockVec.heroLevelPrizeUnlockVec.begin();
//	vector<SHeroLevelPrizeUnlockItem> ::iterator iteUnlockVecEnd  =  iterHeroMap->second.sHeroLevelPrizeUnlockVec.heroLevelPrizeUnlockVec.end();
//
//	for(;iterUnlockVec!=iteUnlockVecEnd;++iterUnlockVec)
//	{
//		if((*iterUnlockVec).bUnlock)
//		{
//			continue;
//		}
//
//		if(nHeroLevel >= (*iterUnlockVec).nUnlockLevel)
//		{
//			for (int i = 0; i < PRIZEUNLOCK_PRIZEID_MAXCOUNT; i++)
//			{
//				int nPrizeId = iterUnlockVec->nPrizeId[i];
//				if (nPrizeId <= 0)
//					break;;
//
//				if(m_pBankPart->canPrizeActor(nPrizeId,1))
//				{
//					m_pBankPart->prizeActor(nPrizeId,1,OSSRES_ADD_ACTORUPGRADE,uidSelf,NULL);
//				}
//			}
//
//			(*iterUnlockVec).bUnlock = true;
//		}
//	}
//}


// 初始化某个英雄星级解锁奖励数据


bool CHeroManager::initOneHeroStarPrizeUnlockInfo(int nHeroId)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL !");
		return false;
	}

	ISchemePrizeUnlock* pSchemePrizeUnlock = pSchemeCenter->getSchemePrizeUnlock();
	if ( pSchemePrizeUnlock == NULL )
	{
		ErrorLn(__FUNCTION__": pSchemePrizeUnlock == NULL !");
		return false;
	}

	SHeroStarPrizeUnlockMap* pMapHeroStarUnlock = pSchemePrizeUnlock->getHeroStarPrizeUnlockInfo();
	if(pMapHeroStarUnlock == NULL||pMapHeroStarUnlock->empty())
	{
		ErrorLn(__FUNCTION__": pMapHeroStarUnlock Fail !pHeroUnlock ==NULL");
		return false;
	}

	SHeroStarPrizeUnlockMap::iterator iterUnlockResult = pMapHeroStarUnlock->find(nHeroId) ;
	if(iterUnlockResult == pMapHeroStarUnlock->end())
	{
		ErrorLn(__FUNCTION__": SHeroStarPrizeUnlockMap Fail !");
		return false;
	}

	if(iterUnlockResult->second.heroStarPrizeUnlockVec.size() <= 0)
	{
		ErrorLn(__FUNCTION__": heroStarPrizeUnlockVec Fail !");
		return false;
	}

	PLAYERHEROMAP::iterator iterHeroMap = m_mapHero.find(nHeroId);
	if (iterHeroMap == m_mapHero.end())
	{
		ErrorLn(__FUNCTION__": PLAYERHEROMAP::iterator Fail ! heroId = " << nHeroId);
		return false;
	}

	int nHeroStar = iterHeroMap->second.sHeroInfo.nHeroStar;    
	iterHeroMap->second.sHeroStarPrizeUnlockVec.heroStarPrizeUnlockVec.clear();
	iterHeroMap->second.sHeroStarPrizeUnlockVec.heroStarPrizeUnlockVec = iterUnlockResult->second.heroStarPrizeUnlockVec;

	vector<SHeroStarPrizeUnlockItem> ::iterator iterUnlockVec  =  iterHeroMap->second.sHeroStarPrizeUnlockVec.heroStarPrizeUnlockVec.begin();
	vector<SHeroStarPrizeUnlockItem> ::iterator iteUnlockVecEnd  =  iterHeroMap->second.sHeroStarPrizeUnlockVec.heroStarPrizeUnlockVec.end();

	for(;iterUnlockVec!=iteUnlockVecEnd;++iterUnlockVec)
	{
		if((*iterUnlockVec).bUnlock)
		{
			continue;
		}

		if(nHeroStar >= (*iterUnlockVec).nUnlockLevel)
		{
			(*iterUnlockVec).bUnlock = true;
		}
	}

    return true;
}

bool CHeroManager::initHeroStarPrizeUnlockInfo()
{
	if(m_mapHero.size() <= 0)
	{
		return false;
	}

	//初始过有数据返回
	PLAYERHEROMAP::iterator iterHeroMap  = m_mapHero.begin();
	for(;iterHeroMap!=m_mapHero.end();++iterHeroMap)
	{
		if(iterHeroMap->second.sHeroStarPrizeUnlockVec.heroStarPrizeUnlockVec.size() > 0)
		{
			return true;
		}
	}

	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL !");
		return false;
	}

	ISchemePrizeUnlock* pSchemePrizeUnlock = pSchemeCenter->getSchemePrizeUnlock();
	if ( pSchemePrizeUnlock == NULL )
	{
		ErrorLn(__FUNCTION__": pSchemePrizeUnlock == NULL !");
		return false;
	}

	SHeroStarPrizeUnlockMap* pMapHeroStarUnlock = pSchemePrizeUnlock->getHeroStarPrizeUnlockInfo();
	if(pMapHeroStarUnlock == NULL||pMapHeroStarUnlock->empty())
	{
		ErrorLn(__FUNCTION__": pMapHeroStarUnlock Fail !,pHeroUnlock ==NULL");
		return false;
	}

	bool bHasPrizeInfo = false;
	iterHeroMap  = m_mapHero.begin();
	for(;iterHeroMap!=m_mapHero.end();++iterHeroMap)
	{
		int nHeroId  = iterHeroMap->first;
		iterHeroMap->second.sHeroStarPrizeUnlockVec.heroStarPrizeUnlockVec.clear();

		SHeroStarPrizeUnlockMap::iterator iterUnlockResult = pMapHeroStarUnlock->find(nHeroId) ;
		if(iterUnlockResult == pMapHeroStarUnlock->end())
		{
			continue;
		}

		if(iterUnlockResult->second.heroStarPrizeUnlockVec.size() <= 0)
		{
			continue;
		}

		bHasPrizeInfo = true;
		int nHeroStar = iterHeroMap->second.sHeroInfo.nHeroStar;      
		iterHeroMap->second.sHeroStarPrizeUnlockVec.heroStarPrizeUnlockVec = iterUnlockResult->second.heroStarPrizeUnlockVec;

		vector<SHeroStarPrizeUnlockItem> ::iterator iterUnlockVec  =  iterHeroMap->second.sHeroStarPrizeUnlockVec.heroStarPrizeUnlockVec.begin();
		vector<SHeroStarPrizeUnlockItem> ::iterator iteUnlockVecEnd  =  iterHeroMap->second.sHeroStarPrizeUnlockVec.heroStarPrizeUnlockVec.end();

		for(;iterUnlockVec!=iteUnlockVecEnd;++iterUnlockVec)
		{
			if((*iterUnlockVec).bUnlock)
			{
				continue;
			}

			if(nHeroStar >= (*iterUnlockVec).nUnlockLevel)
			{
				(*iterUnlockVec).bUnlock = true;
			}
		}
	}

	bool bInitSuccess = bHasPrizeInfo;
	return bInitSuccess;
}

void CHeroManager::checkHeroStarUnlockPrize(int nHeroId)
{
	if(m_pMaster == NULL)
	{
		return;
	}

	if(m_pBankPart == NULL)
	{
		ErrorLn(__FUNCTION__": m_pPlayerBank == NULL");    
		return;
	}

	PLAYERHEROMAP ::iterator iterHeroMap = m_mapHero.find(nHeroId);
	if(iterHeroMap == m_mapHero.end())
	{
		return;
	}

	if(iterHeroMap->second.sHeroStarPrizeUnlockVec.heroStarPrizeUnlockVec.size() <=0 )
	{
		return;
	}

	int nHeroStar = iterHeroMap->second.sHeroInfo.nHeroStar;
	DWORD uidSelf = m_pMaster->getUID();

	vector<SHeroStarPrizeUnlockItem> ::iterator iterUnlockVec  =  iterHeroMap->second.sHeroStarPrizeUnlockVec.heroStarPrizeUnlockVec.begin();
	vector<SHeroStarPrizeUnlockItem> ::iterator iteUnlockVecEnd  =  iterHeroMap->second.sHeroStarPrizeUnlockVec.heroStarPrizeUnlockVec.end();

	for(;iterUnlockVec!=iteUnlockVecEnd;++iterUnlockVec)
	{
		if((*iterUnlockVec).bUnlock)
		{
			continue;
		}

		if(nHeroStar >= (*iterUnlockVec).nUnlockLevel)
		{
			for (int i = 0; i < PRIZEUNLOCK_PRIZEID_MAXCOUNT; i++)
			{
				int nPrizeId = iterUnlockVec->nPrizeId[i];
				if (nPrizeId <= 0)
					break;;

				if(m_pBankPart->canPrizeActor(nPrizeId,1))
				{
					m_pBankPart->prizeActor(nPrizeId,1,OSSRES_ADD_ACTORUPGRADE,uidSelf,NULL);
				}
			}

			(*iterUnlockVec).bUnlock = true;
			sendHeroStarUnlockPrizeToClient(nHeroId, nHeroStar);
		}
	}

	// 奖励天赋
	activeTalentByHeroStar(nHeroId, nHeroStar);
}

void CHeroManager::sendHeroStarUnlockPrizeToClient(int nHeroId, int nStarLv)
{
	obuf obmsg;
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
	head.byKeyModule   = MSG_MODULEID_ENTITY;
	head.byKeyAction   = SC_MSG_ENTITY_HEROUPSTAR_UNLOCK;               

	// 部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
	entityHead.byPartID = PART_BANK;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

	obmsg << head << entityHead;

	msg_entity_heroupstar_unlock unLockData;
	unLockData.nHeroID = nHeroId;
	unLockData.nStarLevel = nStarLv;
	obmsg.push_back(&unLockData, sizeof(msg_entity_heroupstar_unlock));

	__IPlayerRole *pPlayerRole=CAST_TYPE(__IPlayerRole*, m_pMaster);
	if(pPlayerRole)
	{
		pPlayerRole->send2Client(obmsg.data(), obmsg.size());
	}
}

// 设置英雄等级
bool     CHeroManager::           setHeroLevel(int nHeroID,int nValue)
{
	if(nHeroID <=0 )
	{
		return false;
	}

	if(m_mapHero.find(nHeroID)==m_mapHero.end())
	{
		return false;
	}

	m_mapHero[nHeroID].sHeroInfo.nLevel = nValue;
	heroLevelChangeHandel(nHeroID);
	return true;
}
// 增加英雄等级
bool      CHeroManager::          addHeroLevel(int nHeroID,int nAddValue)
{
	if(nHeroID <=0 )
	{
		return false;
	}

	if(m_mapHero.find(nHeroID)==m_mapHero.end())
	{
		return false;
	}

	m_mapHero[nHeroID].sHeroInfo.nLevel += nAddValue;
	heroLevelChangeHandel(nHeroID);
	return true;
}
// 英雄等级触发事件,以后其他期需要的话就再发个事件
void     CHeroManager::           heroLevelChangeHandel(int nHeroID)
{
	//checkHeroLevelUnlockPrize(nHeroID);
	sendHeroLevelChangeToClient(nHeroID);
}

void     CHeroManager::  sendHeroLevelChangeToClient(int nHeroID)
{
	obuf obData;
	SNetMsgHead head;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule    = MSG_MODULEID_ENTITY;
	head.byKeyAction    = SC_MSG_ENTITY_STATICHERO_UPGRADE;

	// 部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_BANK;

	obData << head << entityHead;

	msg_entity_static_hero_upgrade msgUpgradeInfo;
	SHeroInfo* pHeroInfo  = &m_mapHero[nHeroID].sHeroInfo;
	msgUpgradeInfo.nFrontLv = pHeroInfo->nLevel;
	msgUpgradeInfo.nHeroID = nHeroID;
	msgUpgradeInfo.nCurrentStarLv = pHeroInfo->nHeroStar;
	msgUpgradeInfo.nCurrntLv = pHeroInfo->nLevel;
	msgUpgradeInfo.nCurrntExp = pHeroInfo->dwHeroExp;
	obData << msgUpgradeInfo;

	((__IPlayerRole*)m_pMaster)->send2Client(obData.data(), obData.size());
}

////////////////////////////////////英雄附加信息数据库通用接口//////////////////////////////////////

void CHeroManager::requestDataBaseHeroAdditional(int nHeroAdditionalType)
{
	if(m_pMaster == NULL)
	{
		return;
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__ << " : failed pDBEngine == NULL");
		return;
	}

	DBREQ_PARAM_GET_HEROADDITIONAL pData;
	pData.dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);
	if (pData.dwActorID == INVALID_PDBID)
	{
		return;
	}

	pData.nType = nHeroAdditionalType;

	obuf oBuff;
	oBuff.push_back(&pData, sizeof(DBREQ_PARAM_GET_HEROADDITIONAL));

	if (pDBEngine->executeSP(GAMEDB_REQUEST_GET_HEROADDITIONAL, pData.dwActorID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_GET_HEROADDITIONAL failed");
	}
}

void CHeroManager::onDBReturnHeroAdditional(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc,int nQueueIndex,void * pOutData,int nOutLen)
{
	if ( nDBRetCode != DBRET_CODE_SUCCEED )
	{
		ErrorLn( __FUNCTION__<<" nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
		return;
	}

	if(pOutData==NULL)
	{
		ErrorLn(__FUNCTION__": failed pOutData == NULL");
		return;
	}

	if(m_pMaster==NULL)
	{
		return ;
	}

	int nPDBID =nQueueIndex;
	PDBID selfPDBIU =  m_pMaster->getIntProperty(PROPERTY_ID);
	if (nPDBID != selfPDBIU || nPDBID == INVALID_UID)
	{
		ErrorLn(__FUNCTION__ << " Invilad ActorID !!! ");
		return;
	}
	// 效验长度值
	int nEfficacyLen=sizeof(DBREQ_RESULT_GET_HEROADDITIONAL);
	if(nOutLen<nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nOutLen<<"< DBREQ_RESULT_GET_HEROADDITIONAL Size= "<<nEfficacyLen);
		return;
	}

	DBREQ_RESULT_GET_HEROADDITIONAL* pResultData = (DBREQ_RESULT_GET_HEROADDITIONAL*)pOutData;
	void* pHeadData = (void*)(pResultData+1);
	int nLen = nOutLen - nEfficacyLen;  
	switch(pResultData->nType)
	{
	case (int)HERO_ADD_TYPE_TALENT:
		{
			onDBReturnHeroTalentInfo(pHeadData,nLen,pResultData->nCount);

			// 根据英雄星级激活所有符合等级的天赋
			activeAllHeroTalentByHeroStar();

			sendHeroTalentInfoToClient();
		}
		break;
	case (int)HERO_ADD_TYPE_SKIN:
		{
			onDBReturnHeroSkinInfo(pHeadData,nLen,pResultData->nCount);
			checkToCleanSkinToDB();
		}
		break;
	case (int)HERO_ADD_TYPE_XPSKILL:
		{
			onDBReturnHeroXPSkillInfo(pHeadData,nLen,pResultData->nCount);

			autoActiveHeroXpSkill();
			equipDefaultHeroXPSKill();
			updateHeroXPSkillToCilent();
		}
		break;
	case (int)HERO_ADD_TYPE_XPFRAGMENT:
		{
			onDBReturnHeroXPSkillFragmentInfo(pHeadData,nLen,pResultData->nCount);
		}
		break;
	default:
		{
			ErrorLn(__FUNCTION__":pResultData->nType =" << pResultData->nType << "is not find" );
		}
		break;
	}	
}
/////////////////////////////////////英雄天赋/////////////////////////////////////

bool  CHeroManager::canAddHeroTalentInfo(int nHeroId,int nTalentId)
{
	return true;
}

void CHeroManager::updateHeroTalentToDB(int nHeroId)
{
	if (m_pMaster == NULL)
	{
		return ;
	}
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return;
	}

	PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	SHeroInfoFlags & heroInfo = getMapHeroInfo(nHeroId);

	HERO_TALENT_VEC & talents = heroInfo.sHeroTalentList.TalentInfoVec;

	int nNodeCount = 0; // talents.size();
	obuf obData;
	for (HERO_TALENT_VEC::iterator iterVec = talents.begin(); iterVec != talents.end(); ++iterVec)
	{
		if (iterVec->dwTalentID <= 0)
		{
			continue;
		}

		HERO_TALENT_INFO heroTalentInfo;
		heroTalentInfo.bActive = iterVec->bActive;
		heroTalentInfo.dwTalentID = iterVec->dwTalentID;
		obData.push_back(&heroTalentInfo, sizeof(heroTalentInfo));
		nNodeCount++;
	}

	int nBufSize = nNodeCount * sizeof(HERO_TALENT_INFO);

	DBREQ_PARAM_SAVE_HEROADDITIONAL pSaveDataHead;
	pSaveDataHead.dwActorID = dwPDBID;
	pSaveDataHead.nType = HERO_ADD_TYPE_TALENT;
	pSaveDataHead.dwHeroID = nHeroId;

	obuf oBuffer;
	oBuffer.push_back(&pSaveDataHead, sizeof(pSaveDataHead));
	oBuffer.push_back(&nBufSize, sizeof(nBufSize));
	oBuffer.push_back(obData.data(), obData.size());
	// 发给DB
	if (pDBEngine->executeSP(GAMEDB_REQUEST_SAVE_HEROADDITIONAL, dwPDBID, oBuffer.data(), oBuffer.size(), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__" : pDBEngine->executeSP() GAMEDB_REQUEST_SAVE_HEROADDITIONAL failed");
		return ;
	}
}

bool  CHeroManager::addHeroTalentInfo(int nHeroId,int nTalentId, bool bSendToClient /*= true*/)
{
    if(m_pMaster==NULL)
    {
        return false;
    }

	if (nTalentId <= 0 || nHeroId <= 0)
	{
		return false;
	}

    IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return false;
    }

	// 存入系统天赋
	if(!checkTalentHeroIdIsCorrect(nHeroId,nTalentId))
	{
		return false;
	}

    PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	SHeroInfoFlags & heroInfo = getMapHeroInfo(nHeroId);

    HERO_TALENT_INFO heroTalent;
    heroTalent.dwTalentID = nTalentId;
    heroTalent.bActive = true;
	// 已经有此英雄天赋id 返回 不存
	HERO_TALENT_VEC::iterator iterEnd = heroInfo.sHeroTalentList.TalentInfoVec.end();
	HERO_TALENT_VEC::iterator iterResult = find(heroInfo.sHeroTalentList.TalentInfoVec.begin(),iterEnd, heroTalent) ;
	if(iterResult != iterEnd)
	{
		return false;
	}
    // 压入到容器中
	heroInfo.sHeroTalentList.TalentInfoVec.push_back(heroTalent);
	
    HERO_TALENT_VEC & talents = heroInfo.sHeroTalentList.TalentInfoVec;

    int nNodeCount = talents.size();
	obuf obData;	
	for (HERO_TALENT_VEC::iterator iterVec = talents.begin(); iterVec!= talents.end(); ++iterVec)
	{
		HERO_TALENT_INFO heroTalentInfo;
		heroTalentInfo.bActive      = iterVec->bActive;
		heroTalentInfo.dwTalentID   = iterVec->dwTalentID;
		obData.push_back(&heroTalentInfo, sizeof(heroTalentInfo));
	}
    
	int nBufSize = nNodeCount * sizeof(HERO_TALENT_INFO);

    DBREQ_PARAM_SAVE_HEROADDITIONAL pSaveDataHead;
    pSaveDataHead.dwActorID = dwPDBID;
    pSaveDataHead.nType     = HERO_ADD_TYPE_TALENT;
    pSaveDataHead.dwHeroID  = nHeroId;

	obuf oBuffer;
	oBuffer.push_back(&pSaveDataHead, sizeof(pSaveDataHead));
	oBuffer.push_back(&nBufSize, sizeof(nBufSize));
	oBuffer.push_back(obData.data(), obData.size());
	// 发给DB
	if (pDBEngine->executeSP(GAMEDB_REQUEST_SAVE_HEROADDITIONAL, dwPDBID, oBuffer.data(), oBuffer.size(), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__" : pDBEngine->executeSP() GAMEDB_REQUEST_SAVE_HEROADDITIONAL failed");
		return false;
	}

	if (bSendToClient)
	{
		updateActiveHeroTalentToClient(&heroTalent,nHeroId);	
	}

	return true;
}

void CHeroManager::onDBReturnHeroTalentInfo(void * pOutData,int nOutLen,int nHeroCount)
{
    if(m_pMaster==NULL)
    {
        ErrorLn(__FUNCTION__": m_pMaster==NULL");
        return;
    }
    if(pOutData == NULL || nOutLen < sizeof(DBREQ_PARAM_HEROADDITIONAL))
    {
        //ErrorLn(__FUNCTION__": pOutData == NULL || nOutLen["<< nOutLen <<"] < sizeof(DBREQ_PARAM_HEROADDITIONAL)["<< sizeof(DBREQ_PARAM_HEROADDITIONAL) <<"]");
        return;
    }

    char * pHeroHead = 0;
    int nEfficacyLen = 0;
	for (int nIndex = 0; nIndex <nHeroCount; ++nIndex)
	{
        pHeroHead = (char*)(pOutData)+nEfficacyLen;

        nEfficacyLen += sizeof(DBREQ_PARAM_HEROADDITIONAL)+sizeof(int);
        // 已到数据最后
        if(nOutLen < nEfficacyLen)
        {
            break;
        }

        // 英雄附加信息数据
        DBREQ_PARAM_HEROADDITIONAL* pHeroData = (DBREQ_PARAM_HEROADDITIONAL*)(pHeroHead);
        int nHeroId = pHeroData->dwHeroID;
        // 取得此节点对应BUFF长度数据
        int * pBufSize  = (int*)(pHeroData+1);
        int & nBuffSize = (*pBufSize);
        // 此节点下没有BUFF
        if(nBuffSize <= 0)
        {
            continue;
        }
        // 已到数据最后
        if( nOutLen < nEfficacyLen + nBuffSize )
        {
            break;
        }

        // 定位到BUFF位置
        char * pBuff = (char*)(pBufSize+1);
        // 增加遍历长度
        nEfficacyLen += nBuffSize;
        // 无效英雄ID时不用读取下面BUFF中的数据了
        if(nHeroId <= 0)
        {
            WarningLn(__FUNCTION__": hero is invalid: hero="<<nHeroId <<", name="<<m_pMaster->getName());
            continue;
        }
        
        SHeroInfoFlags & heroInfo = getMapHeroInfo(nHeroId);

        // 遍历BUFF中的节点
        HERO_TALENT_INFO* pNode = (HERO_TALENT_INFO*)pBuff;
        // 存有节点数量
        int nNodeCount = nBuffSize / sizeof(HERO_TALENT_INFO);
        for( int i = 0; i < nNodeCount; ++i )
        {
            if( checkTalentHeroIdIsCorrect(nHeroId, pNode->dwTalentID) )
            {
			    heroInfo.sHeroTalentList.TalentInfoVec.push_back(*pNode);
            }
            // 移到下一个节点
            ++pNode;
        }
	}
}

bool CHeroManager::checkTalentHeroIdIsCorrect(int nHeroId ,int nTalentId)
{
	// 效验
	if(nTalentId <= 0 || nHeroId <= 0)
	{
		WarningLn(__FUNCTION__": failed dwTalentID <= 0 or nHeroId <= 0, nHeroId=" << nHeroId << ",nTalentId=" << nTalentId);
		return false ;
	}
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if(pSchemeCenter == NULL)
	{
		return false;
	}
	ISchemeHeroTalentConfig* pSchemeHeroTalent = pSchemeCenter->getSchemeHeroTalentConfig();
	if(pSchemeHeroTalent == NULL)
	{
		return false;
	}
	std::map<int, std::vector<Single_Hero_Talent_InfoEx> >* pMapSchemeTalent = pSchemeHeroTalent->GetHeroTalentInfo();
	if(pMapSchemeTalent == NULL)
	{
		return false;
	}
	std::map<int, std::vector<Single_Hero_Talent_InfoEx> >::iterator iterResult = pMapSchemeTalent->find(nHeroId);
	if(iterResult == pMapSchemeTalent->end())
	{
		ErrorLn(__FUNCTION__": hero talent is not in config, nHeroId=" << nHeroId << ",nTalentId=" << nTalentId);
		return false;
	}

    __IWarPart * pWarPart = CAST_TYPE(__IWarPart *, m_pMaster->getEntityPart(PART_WAR) );
    int nMatchTypeID = pWarPart ? pWarPart->getMatchTypeID(): 0;

	bool bFindTalent = false;
    std::vector<Single_Hero_Talent_InfoEx>& talents = iterResult->second;
	
	for(std::vector<Single_Hero_Talent_InfoEx>::iterator iter = talents.begin(); iter != talents.end();++iter)
	{
		if(iter->dwTalentID == nTalentId)
		{
            // 要用战场类型ID过滤下
            bFindTalent = ( find(iter->matchTypes.begin(), iter->matchTypes.end(), nMatchTypeID) != iter->matchTypes.end() );
			break;
		}
	}

	if(!bFindTalent)
	{
		//TraceLn(__FUNCTION__": hero talent is not in config, nHeroId=" << nHeroId << ",nTalentId=" << nTalentId << ",nMatchType=" << nMatchTypeID);
		return false;
	}

	return true;
}

// 激活所有达到英雄星级的可激活天赋
void CHeroManager::activeTalentByHeroStar(int nHeroId, int nHeroStarLv, bool bSendToClient /*= true*/)
{
	ISchemeHeroTalentConfig* pTalentConfig = gServerGlobal->getSchemeCenter()->getSchemeHeroTalentConfig();
	if (pTalentConfig)
	{
		vector<int> talentList = pTalentConfig->getAllHeroStarActiveTalent(nHeroId, nHeroStarLv);
		for (int i = 0; i < talentList.size(); ++i)
		{
			addHeroTalentInfo(nHeroId, talentList[i], bSendToClient);
		}
	}
}

// 激活英雄所有英雄星级符合条件的天赋
void CHeroManager::activeAllHeroTalentByHeroStar()
{
	PLAYERHEROMAP::iterator iter = m_mapHero.begin();
	for (; iter != m_mapHero.end(); ++iter)
	{
		if (isHeroHave(iter->second.sHeroInfo.dwFlags))
		{
			int nHeroId = iter->second.sHeroInfo.nHeroID;
			int nHeroStar = iter->second.sHeroInfo.nHeroStar;

			activeTalentByHeroStar(nHeroId, nHeroStar, false);
		}
	}
}

// 发送天赋初始数据到客户端
void CHeroManager::sendHeroTalentInfoToClient()
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if(pSchemeCenter == NULL)
	{
		return;
	}
	ISchemeHeroTalentConfig* pSchemeHeroTalent = pSchemeCenter->getSchemeHeroTalentConfig();
	if(pSchemeHeroTalent == NULL)
	{
		return;
	}
	std::map<int, std::vector<Single_Hero_Talent_InfoEx> >* pMapSchemeTalent = pSchemeHeroTalent->GetHeroTalentInfo();
	if(pMapSchemeTalent == NULL)
	{
		return;
	}

	// 发到客户端
	obuf obmsg;
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
	head.byKeyModule   = MSG_MODULEID_ENTITY;
	head.byKeyAction   = SC_MSG_ENTITY_HEROTALENT_INFO; // 英雄天赋同步              

	// 部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
	entityHead.byPartID = PART_BANK;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

	obmsg << head << entityHead;

	// 放入英雄天赋仓库信息
	int nHeroTalentNum = pMapSchemeTalent->size();            // 英雄个数
	// 英雄数
	obmsg.push_back(&nHeroTalentNum, sizeof(nHeroTalentNum));

    // 比赛类型
    __IWarPart * pWarPart = CAST_TYPE(__IWarPart*, m_pMaster->getEntityPart(PART_WAR));
    int nMatchTypeID = pWarPart ? pWarPart->getMatchTypeID() : 0 ;

	// 往消息中添加英雄天赋信息
	for (std::map<int, std::vector<Single_Hero_Talent_InfoEx> >::iterator iterHero = pMapSchemeTalent->begin(); iterHero != pMapSchemeTalent->end(); ++iterHero)
	{
        obuf obTalent;
        int nTalentCount = 0;
		
        int nHeroID = iterHero->first;
        std::vector<Single_Hero_Talent_InfoEx>& talents = iterHero->second;

		for(std::vector<Single_Hero_Talent_InfoEx>::iterator iterTalent = talents.begin(); iterTalent != talents.end(); ++iterTalent)
		{
            Single_Hero_Talent_InfoEx& node = (*iterTalent);
            // 不符合此战场类型
            if( find(node.matchTypes.begin(), node.matchTypes.end(), nMatchTypeID) == node.matchTypes.end() )
            {
                continue;
            }

            ++nTalentCount;

            HERO_TALENT_INFO talent;
            talent.dwTalentID = node.dwTalentID;
            talent.bActive = node.bActive;
			if( m_mapHero.find(nHeroID) != m_mapHero.end() )
			{
				HERO_TALENT_VEC::iterator iterEnd =  m_mapHero[nHeroID].sHeroTalentList.TalentInfoVec.end();
				HERO_TALENT_VEC::iterator iterResult = find(m_mapHero[nHeroID].sHeroTalentList.TalentInfoVec.begin(), iterEnd, *(iterTalent)) ;
				if(iterResult != iterEnd)
				{
					talent.bActive = iterResult->bActive;
				}
			}
			obTalent.push_back( &talent, sizeof(talent) );
		}
        // 压入天赋数量
        obmsg.push_back(&nTalentCount, sizeof(nTalentCount));
        // 压入英雄ID
        obmsg.push_back(&nHeroID, sizeof(nHeroID));
        // 压入天赋数据
        if(nTalentCount > 0) {
            obmsg.push_back(obTalent.data(), obTalent.size());
        }

	}

	__IPlayerRole *pPlayerRole=CAST_TYPE(__IPlayerRole*, m_pMaster);
	if(pPlayerRole)
	{
		pPlayerRole->send2Client(obmsg.data(), obmsg.size());
	}
}

void CHeroManager::updateActiveHeroTalentToClient(HERO_TALENT_INFO * pHeroTalent,int nHeroId)
{
	// 更新激活的天赋
	if(!pHeroTalent->bActive)
	{
		return;
	}

	obuf obmsg;
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
	head.byKeyModule   = MSG_MODULEID_ENTITY;
	head.byKeyAction   = SC_MSG_ENTITY_HEROTALENT_UPTATE; // 英雄天赋同步   
	// 部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
	entityHead.byPartID = PART_BANK;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

	obmsg << head << entityHead;

	obmsg.push_back(&nHeroId,sizeof(int));
	obmsg.push_back(pHeroTalent,sizeof(HERO_TALENT_INFO));

	__IPlayerRole *pPlayerRole=CAST_TYPE(__IPlayerRole*, m_pMaster);
	if(pPlayerRole)
	{
		pPlayerRole->send2Client(obmsg.data(), obmsg.size());
	}
}

////////////////////////////////////英雄皮肤//////////////////////////////////////
// 是否拥有某个皮肤
bool CHeroManager::checkIsPossessSkin(int nHeroId,int nSkinId)
{
	if( nSkinId <= 0 || nHeroId <=0 || nSkinId > DEFAULTMAXSKINID)
	{
		return false;
	}
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL");
		return false;
	}
	ISchemeSkinInfo * pSchemeSkinInfo = pSchemeCenter->getSchemeSkinInfo();
	if (pSchemeSkinInfo == NULL)
	{
		ErrorLn(__FUNCTION__":pSchemeSkinInfo == NULL");
		return false;
	}
	SkinInfoScheme_S * pSkinInfoScheme = pSchemeSkinInfo->getSkinShemeInfo(nSkinId);
	if (pSkinInfoScheme == NULL)
	{
		ErrorLn(__FUNCTION__":pSkinInfoScheme == NULL, skinId = " << nSkinId);
		return false;
	}
	PLAYERHEROMAP::iterator itHero = m_mapHero.find(nHeroId);
	if (itHero == m_mapHero.end() || !isHeroHave(itHero->second.sHeroInfo.dwFlags))
	{
		return false;
	}

	bool bPossessSkin = false;
	Single_Hero_Skin_Info heroSkin;
	heroSkin.dwSkinID = nSkinId;
	HERO_SKIN_VEC & skinVec = itHero->second.sHeroSkinList.SkinInfoVec;
	HERO_SKIN_VEC::iterator itSkin = find(skinVec.begin(), skinVec.end(), heroSkin);
	if (itSkin != skinVec.end())
		bPossessSkin = true;
	else
		bPossessSkin = (pSkinInfoScheme->nSkinLevel == ESLT_NORMAL);

	return bPossessSkin;
}

bool CHeroManager::canAddHeroSkinInfo(int nHeroId,int nSkinId,DWORD dwDurationTime)
{
	return true;
}

bool CHeroManager::addHeroSkinInfo(int nHeroId,int nSkinId,DWORD dwDurationTime)
{
	// 以后最好可以有个效验皮肤id表，现在效验默认值
	if( nSkinId <= 0 || nHeroId <=0 || nSkinId > DEFAULTMAXSKINID)
	{
		return false;
	}

	bool bForeverSkin = dwDurationTime == PERMANENT_SKIN_TIME;
	if(!bForeverSkin)
	{
		return false;
	}

	if(m_mapHero.find(nHeroId) == m_mapHero.end())
	{
		SHeroInfoFlags heroInfo;
		m_mapHero[nHeroId] = heroInfo;
	}

	Single_Hero_Skin_Info heroSkin;
	heroSkin.dwSkinID = nSkinId;
	
	HERO_SKIN_INFO* pHeroSkin = NULL;
	HERO_SKIN_VEC::iterator iterEnd = m_mapHero[nHeroId].sHeroSkinList.SkinInfoVec.end();
	HERO_SKIN_VEC::iterator iterResult = find(m_mapHero[nHeroId].sHeroSkinList.SkinInfoVec.begin(),iterEnd, heroSkin) ;
	if(iterResult!= iterEnd)
	{
		if(iterResult->dwDurationTime == PERMANENT_SKIN_TIME && bForeverSkin)
		{
			return false;
		}
		else if(dwDurationTime > 0 && !bForeverSkin) 
		{
			iterResult->dwDurationTime += dwDurationTime;
		}
		else if(iterResult->dwDurationTime > 0 && bForeverSkin)
		{
			iterResult->dwDurationTime  = PERMANENT_SKIN_TIME;
		}

		pHeroSkin = &(*iterResult);
	}
	else
	{
		heroSkin.dwDurationTime = dwDurationTime;
		if(!bForeverSkin)
		{
			DWORD dwCurrentTime = time(NULL);
			heroSkin.dwDurationTime += dwCurrentTime;
		}

		pHeroSkin = &heroSkin;
		m_mapHero[nHeroId].sHeroSkinList.SkinInfoVec.push_back(heroSkin);
	}

	saveHeroSkinInfoToDB(nHeroId);
	updateActiveHeroSkinToClient(pHeroSkin,nHeroId);	
	return true;
}

SLifeHero CHeroManager::translatIntoSLifeHero(const msg_entity_lifehero_info& msgInfo)
{
	SLifeHero lifeHero;
	lifeHero.nHeroID = msgInfo.nHeroID;
	lifeHero.nCultureID = msgInfo.nCultureID;

	return lifeHero;
}

void CHeroManager::onMessage(int msgId, void * data, size_t len)
{
	switch (msgId)
	{
	case CS_MSG_ENTITY_LIFEHERO_ADD:
	{
		if (data == NULL || len < sizeof(msg_entity_lifehero_info))
		{
			ErrorLn(__FUNCTION__ << " : CS_MSG_ENTITY_LIFEHERO_ADD data is null or length is invalid! len = " << len << ", sizeof = " << sizeof(msg_entity_lifehero_info) << ", PDBID = " << m_pMaster->getIntProperty(PROPERTY_ID));
			return;
		}
		msg_entity_lifehero_info * pData = (msg_entity_lifehero_info*)data;

		onMessageAddLifeHero(translatIntoSLifeHero(*pData));
	}
	break;
	case CS_MSG_ENTITY_LIFEHERO_DEL:
	{
		if (data == NULL || len < sizeof(msg_entity_lifehero_del))
		{
			ErrorLn(__FUNCTION__ << " : CS_MSG_ENTITY_LIFEHERO_DEL data is null or length is invalid! len = " << len << ", sizeof = " << sizeof(msg_entity_lifehero_del) << ", PDBID = " << m_pMaster->getIntProperty(PROPERTY_ID));
			return;
		}
		msg_entity_lifehero_del* param = (msg_entity_lifehero_del*)data;
		onMessageDelLifeHero(param->heroID);
	}
	break;
	case CS_MSG_ENTITY_LIFEHERO_REPLACE:
	{
		if (data == NULL || len < sizeof(msg_entity_lifehero_replace))
		{
			ErrorLn(__FUNCTION__ << " : CS_MSG_ENTITY_LIFEHERO_REPLACE data is null or length is invalid! len = " << len << ", sizeof(msg_entity_lifehero_replace) = " << sizeof(msg_entity_lifehero_replace) << ", PDBID = " << m_pMaster->getIntProperty(PROPERTY_ID));
			return;
		}
		msg_entity_lifehero_replace* pData = (msg_entity_lifehero_replace*)data;

		onMessageReplaceLifeHero(pData->heroID, translatIntoSLifeHero(pData->hero));
	}
	break;
	case CS_MSG_ENTITY_LIFEHERO_EXCHANGE:
	{
		if (data == NULL || len < sizeof(msg_entity_lifehero_exchange))
		{
			ErrorLn(__FUNCTION__ << " : CS_MSG_ENTITY_LIFEHERO_EXCHANGE data is null or length is invalid! len = " << len << ", sizeof(msg_entity_lifehero_exchange) = " << sizeof(msg_entity_lifehero_exchange) << ", PDBID = " << m_pMaster->getIntProperty(PROPERTY_ID));
			return;
		}
		msg_entity_lifehero_exchange * pData = (msg_entity_lifehero_exchange*)data;
		onMessageExchangeCultureSlot(pData->heroID1, pData->heroID2);
	}
	break;
	case CS_MSG_ENTITY_LIFEHERO_QUERY:
	{
		if (data == NULL || len < sizeof(msg_entity_lifehero_query))
		{
			ErrorLn(__FUNCTION__ << " : CS_MSG_ENTITY_LIFEHERO_QUERY data is null or length is invalid! len = " << len << ", sizeof(msg_entity_lifehero_query) = " << sizeof(msg_entity_lifehero_query) << ", PDBID = " << m_pMaster->getIntProperty(PROPERTY_ID));
			return;
		}
		msg_entity_lifehero_query * pData = (msg_entity_lifehero_query*)data;
		onMessageQueryAndSendAllLifeHero(pData);
	}
	break;
	case CS_MSG_ENTITY_LIFEHERO_OBTAIN_PRIZE:
	{
		onMessageObtainLifeHeroPrize();
	}
	break;
	default:
		break;
	}
}

#pragma region 本命英雄相关

// 添加本命英雄
void CHeroManager::onMessageAddLifeHero(const SLifeHero & lifeHero)
{
	if (lifeHero.nHeroID <= 0)
		return;

	if (lifeHero.nCultureID < 0 || lifeHero.nCultureID >= ELifeHeroType::LIFEHERO_MAX_COUNT)
		return;

	SHeroInfo* heroInfo = getHeroInfo(lifeHero.nHeroID);
	if (heroInfo == NULL)
		return;

	// 是否拥有英雄
	if (!isHeroHave(heroInfo->dwFlags))
		return;

	// 是否已经加入本命英雄
	if (m_mapLifeHero.find(lifeHero.nHeroID) != m_mapLifeHero.end())
	{
		return;
	}

	// 是否在原有的培养仓
	for (std::map<int, SLifeHero>::iterator iter = m_mapLifeHero.begin(); iter != m_mapLifeHero.end(); ++iter)
	{
		if (iter->second.nCultureID == lifeHero.nCultureID)
		{
			return;
		}
	}

	m_mapLifeHero[lifeHero.nHeroID] = lifeHero;
    heroInfo->dwFlags |= HERO_FLAG_LIFEHERO;		// 置为本命英雄
	// 写入数据库
	writeLifeHeroToDB(lifeHero);
	sendLifeHeroInfoToClient(lifeHero);

	// 当玩家收集完本命英雄时，写入任务数据
	if (m_mapLifeHero.size() == ELifeHeroType::LIFEHERO_MAX_COUNT)
	{
		if (!m_pBankPart->setTaskDataIntEx(TASKKEY_ACTOR_LIFEHERO_FINISHED, 1))
		{
			ErrorLn(__FUNCTION__": setTaskDataIntEx(TASKKEY_ACTOR_LIFEHERO_FINISHED, 1) failed !");
			return;
		}

		m_pBankPart->sendLifeHeroTaskDataToClient();
	}

	recordLifeHeroGameNoteLog(LIFEHERO_ADD, &lifeHero, NULL);
#pragma region 需求改变，添加完成后，不升级英雄（先注释掉，待系统稳定后删除此处代码）
	/*
	else if (heroInfo->nHeroStar < 5) // 小于5级，直接升到5级
	{
	    setHeroOneInfo(lifeHero.nHeroID, 5, ENHEROINFOTYPE_STAR, ENCHANGEDATAMODE_SET);
		SHeroInfo* pHeroInfo = getHeroInfo(lifeHero.nHeroID);
		int nHeroID = lifeHero.nHeroID;
		if (pHeroInfo == NULL)
		{
			ErrorLn(__FUNCTION__": can't find nHeroId = " << nHeroID << ", this hero Info");
			return ;
		}

		// 通知任务
		__IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
		if (pEntityEvent != NULL)
		{
			event_game_actor_data_task eventdata;
			eventdata.nTaskLuaEvent = emTaskLuaEventType_HeroUpGrade;
			eventdata.dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
			eventdata.nParam1 = nHeroID;
			eventdata.nParam2 = pHeroInfo->nHeroStar;

			pEntityEvent->FireExecute(EVENT_GAME_ACTOR_DATA_TASK, (LPCSTR)&eventdata, sizeof(eventdata));
		}

		// 将升星成功后的数据发送到客户端
		obuf obData;
		msg_up_star_sucess msgInfo;
		msgInfo.nHeroID = nHeroID;
		msgInfo.bIsSucess = true;
		msgInfo.nCurrentStar = pHeroInfo->nHeroStar;
		msgInfo.nCurrentGold = m_pMaster->getIntProperty(PROPERTY_GOLD);
		ISchemeHeroStarData* pStarData = gServerGlobal->getSchemeCenter()->getSchemeHeroStarData();
		SHeroStarSchemeInfo starInfo = pStarData->getHeroStarData(nHeroID, pHeroInfo->nHeroStar);
		DWORD dwNeedCardID = starInfo.dwNeedCardID;
		msgInfo.nCurrentCardCount = getOneCardCount(dwNeedCardID);
		obData.push_back(&msgInfo, sizeof(msgInfo));
		__IPlayerRole * pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
		if (pPlayerRole)
		{
			pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_UPGRADE_STAR_SUCESS, obData.data(), obData.size());
		}
	}*/
	
#pragma endregion

}

// 删除本命英雄
void CHeroManager::onMessageDelLifeHero(int nHeroId)
{
	if (nHeroId <= 0)
		return;

	if (m_mapLifeHero.find(nHeroId) == m_mapLifeHero.end())
	{
		return;
	}

	delLifeHeroFromDB(m_pMaster->getIntProperty(PROPERTY_ID), m_mapLifeHero[nHeroId].nHeroID);
	m_mapLifeHero.erase(nHeroId);	
    SHeroInfo* heroInfo = getHeroInfo(nHeroId);
    if (heroInfo == NULL)
        return;
    heroInfo->dwFlags &= ~HERO_FLAG_LIFEHERO;		// 置为非本命英雄
}

/*替换本命英雄
@param nOldHeroID: 被替换的英雄ID
@param lifeHero: 新的本命英雄
*/
void CHeroManager::onMessageReplaceLifeHero(int nOldHeroID, const SLifeHero & lifeHero)
{
	if (nOldHeroID <= 0 || lifeHero.nHeroID <= 0)
		return;

	if (m_mapLifeHero.size() != ELifeHeroType::LIFEHERO_MAX_COUNT)
		return;

	if (m_mapLifeHero.find(nOldHeroID) == m_mapLifeHero.end())
		return;

	if (m_mapLifeHero.find(lifeHero.nHeroID) != m_mapLifeHero.end())
		return;

	SHeroInfo* pNewHeroInfo = getHeroInfo(lifeHero.nHeroID);
	if (!pNewHeroInfo)
		return;

	// 是否拥有英雄
	if (!isHeroHave(pNewHeroInfo->dwFlags))
		return;

	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": LifeHero:pSchemeCenter == NULL !");
		return;
	}
	ISchemeLifeHero* pLifeHeroScheme = pSchemeCenter->getSchemeLifeHero();
	if (!pLifeHeroScheme)
	{
		ErrorLn(__FUNCTION__": LifeHero:pLifeHeroScheme == NULL !");
		return;
	}

	SLifeHeroScheme* pConfig = pLifeHeroScheme->getLifeHeroScehme(m_mapLifeHero.size() - 1);
	if (!pConfig)
		return;

	int pdbid = m_pMaster->getIntProperty(PROPERTY_ID);
	if (!m_pBankPart->getMoneyMgr()->canAddMoney(0, -(pConfig->goldCount)))
	{
		TraceLn(__FUNCTION__": 玩家金币不够，不能替换 " << pdbid);
		return;
	}

	ISchemePersonModel * pPersonModel = pSchemeCenter->getSchemePersonModel();
	if (!pPersonModel)
		return;

	char szLogInfo[256] = { 0 };
	ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("%s(%d)更换为%s(%d)"), pPersonModel->getHeroName(nOldHeroID), nOldHeroID, pPersonModel->getHeroName(lifeHero.nHeroID), lifeHero.nHeroID);
	m_pBankPart->getMoneyMgr()->addMoney(0, -pConfig->goldCount, OSSRES_DEL_LIFEHERO_GOLD, szLogInfo, 0, 0, false);

	ISchemeHeroStarData*	pStarData = pSchemeCenter->getSchemeHeroStarData();
	if (!pStarData)
		return;
	SHeroStarSchemeInfo starInfo = pStarData->getHeroStarData(lifeHero.nHeroID, pNewHeroInfo->nHeroStar);
	DWORD dwNeedCardID = starInfo.dwNeedCardID;
	if (getOneCardCount(dwNeedCardID) < pConfig->cardCount)
	{
		TraceLn(__FUNCTION__": 玩家该英雄卡牌不够，不能替换:pdbid=" << pdbid << ",heroID=" << lifeHero.nHeroID);
		return;
	}
	expendHeroCardInfo(dwNeedCardID, -pConfig->cardCount);
	IOSSLogService *		pOSSLogServer = gServerGlobal->getOSSLogService();
	if (!pOSSLogServer)
	{
		ErrorLn(__FUNCTION__": pOSSLogServer == NULL dwActorID= " << pdbid);
		return;
	}
	memset(szLogInfo, 0, sizeof(szLogInfo));
	ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("替换本命英雄id:%d, 卡牌消耗:%d"), lifeHero.nHeroID, pConfig->cardCount);
	pOSSLogServer->addActorGoodsLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_PERSON, pdbid, OSS_OPT_TYPE_SYSTEM, 0, OSS_GOODS_HEROCARD, OSS_RESOURCE_OPT_DEL, OSSRES_DEL_LIFEHERO_CARD, dwNeedCardID, pConfig->cardCount, time(NULL), szLogInfo);

	SLifeHero oldLifeHero = m_mapLifeHero[nOldHeroID];
	SLifeHero newLifeHero = lifeHero;
	newLifeHero.nCultureID = oldLifeHero.nCultureID;
	m_mapLifeHero.erase(nOldHeroID);
	m_mapLifeHero[newLifeHero.nHeroID] = newLifeHero;
    SHeroInfo* pOldHeroInfo = getHeroInfo(nOldHeroID);
    if (pOldHeroInfo == NULL)
        return;
    pOldHeroInfo->dwFlags &= ~HERO_FLAG_LIFEHERO;	// 置为非本命英雄
    pNewHeroInfo->dwFlags |= HERO_FLAG_LIFEHERO;	// 置为本命英雄
	// 更新数据库
	delLifeHeroFromDB(pdbid, oldLifeHero.nHeroID);
	writeLifeHeroToDB(newLifeHero);
	// 发送到客户端
	sendLifeHeroInfoToClient(newLifeHero);
	// 记录文字类型日志
	recordLifeHeroGameNoteLog(LIFEHERO_REPLACE, &oldLifeHero, &newLifeHero);
}

// 交换本命英雄培养仓位置
void CHeroManager::onMessageExchangeCultureSlot(int nHeroId1, int nHeroId2)
{
	if (m_mapLifeHero.find(nHeroId1) == m_mapLifeHero.end() || m_mapLifeHero.find(nHeroId2) == m_mapLifeHero.end())
	{
		return;
	}

	int cultureID = m_mapLifeHero[nHeroId1].nCultureID;
	m_mapLifeHero[nHeroId1].nCultureID = m_mapLifeHero[nHeroId2].nCultureID;
	m_mapLifeHero[nHeroId2].nCultureID = cultureID;
	writeLifeHeroToDB(m_mapLifeHero[nHeroId1]);
	writeLifeHeroToDB(m_mapLifeHero[nHeroId2]);
}

// 获取我的所有本命英雄
std::map<int, SLifeHero> CHeroManager::getAllLifeHero()
{
	return m_mapLifeHero;
}

// 客户端请求获取所有本命英雄
void CHeroManager::onMessageQueryAndSendAllLifeHero(msg_entity_lifehero_query* pData)
{
	if (!pData || pData->actorID != m_pMaster->getIntProperty(PROPERTY_ID))
		return;

	obuf obData;
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_ENTITY;
	head.byKeyAction = SC_MSG_ENTITY_LIFEHERO_QUERY_RESULT;

	// 部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_BANK;
	obData << head << entityHead;

	int nCount = m_mapLifeHero.size();
	obData.push_back(&nCount, sizeof(nCount));
	if (nCount > 0)
	{
		for (map<int, SLifeHero>::iterator it = m_mapLifeHero.begin(); it != m_mapLifeHero.end(); ++it)
		{
			msg_entity_lifehero_info msgData;
			msgData.nHeroID = it->second.nHeroID;
			msgData.nCultureID = it->second.nCultureID;
			obData.push_back(&msgData, sizeof(msgData));
		}
	}

	((__IPlayerRole*)m_pMaster)->send2Client(obData.data(), obData.size());
}

// 客户端请求领取本命英雄奖励
void CHeroManager::onMessageObtainLifeHeroPrize()
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn("LifeHero:pSchemeCenter == NULL !");
		return;
	}
	ISchemeLifeHero* pLifeHeroScheme = pSchemeCenter->getSchemeLifeHero();
	if (!pLifeHeroScheme)
	{
		ErrorLn("LifeHero:pLifeHeroScheme == NULL !");
		return;
	}

	vector<SLifeHeroPrizeScheme>* pVecPrizeScheme = pLifeHeroScheme->getLifeHeroPrizeScehme();
	int taskID = m_pBankPart->getTaskDataIntEx(TASKKEY_ACTOR_LIFEHERO_PRIZE, 0);
	if (taskID < 0 || taskID >= pVecPrizeScheme->size())
	{
		WarningLn("LifeHero: not config prize data !! taskID=" << taskID << ",Prize size=" << pVecPrizeScheme->size());
		return;
	}
    // 由于配置文件是从1开始，而任务数据从0开始，所以这个任务ID将要加多1
    int nPrizeTaskID = taskID+1;

    const vector<SLifeHeroPrizeScheme>::const_iterator result = find_if(pVecPrizeScheme->begin(), pVecPrizeScheme->end(), [nPrizeTaskID](const SLifeHeroPrizeScheme& item){ return(item.targetID == nPrizeTaskID); } );
    if(result == pVecPrizeScheme->end())
    {
		ErrorLn("LifeHero not found task prize! taskID=" << nPrizeTaskID << ",Prize size=" << pVecPrizeScheme->size());
        return;
    }
    // 奖励列表
    const SLifeHeroPrizeScheme& nodeResult = *(result);
	const vector<int>& vecPrizeID = nodeResult.prizeIDVec;
	if (vecPrizeID.size() < 1)
	{
		ErrorLn("LifeHero: cofig prize data is empty!taskID=" << nPrizeTaskID << ", prizes are empty");
		return;
	}

	int heroCount = 0;
	for (map<int, SLifeHero>::iterator iter = m_mapLifeHero.begin(); iter != m_mapLifeHero.end(); ++iter)
	{
		if (m_mapHero[iter->second.nHeroID].sHeroInfo.nHeroStar >= nodeResult.starLv)
		{
			++heroCount;
		}
	}
    // 没有满足英雄数量
	if (heroCount < nodeResult.heroCount)
	{
        return;
    }

    // 设置下一个任务的键值
	if ( !m_pBankPart->setTaskDataIntEx(TASKKEY_ACTOR_LIFEHERO_PRIZE, (++taskID)) )
	{
		ErrorLn(__FUNCTION__": setTaskDataIntEx(TASKKEY_ACTOR_LIFEHERO_PRIZE, taskID=" << taskID << " ) failed !");
		return;
	}
	m_pBankPart->sendLifeHeroPrizeTaskDataToClient();

	char szLogInfo[256] = { 0 };
	ssprintf_s(szLogInfo, sizeof(szLogInfo), "Finish goal:%d", nodeResult.targetID);

    // 派发奖励
    UID uid = m_pMaster->getUID();
	for (int i = 0; i < vecPrizeID.size(); ++i)
	{
		const int& prizeID = vecPrizeID[i];
        // 奖励没有给成功还要继续派发后面的奖励
		if (m_pBankPart->canPrizeActor(prizeID, 1))
		{
			m_pBankPart->prizeActor(prizeID, 1, OSSRES_ADD_LIFEHERO, uid, szLogInfo);
		}
        else
        {
            ErrorLn("LifeHero: check prize find this prize can't give user ! "<< m_pMaster->getName() <<"["<< m_pMaster->getIntProperty(PROPERTY_ID)<<"], prizeID=" << prizeID);
        }
	}

	recordLifeHeroGameNoteLog(LIFEHERO_OBTAIN_PRIZE, NULL, NULL);
}

// 发送本命英雄信息到客户端
void CHeroManager::sendLifeHeroInfoToClient(const SLifeHero & lifeHero)
{	
	obuf obData;
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_ENTITY;
	head.byKeyAction = SC_MSG_ENTITY_LIFEHERO_UPDATE;
	// 部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_BANK;
	obData << head << entityHead;

	msg_entity_lifehero_info msg;
	msg.nHeroID = lifeHero.nHeroID;
	msg.nCultureID = lifeHero.nCultureID;
	obData << msg;
	((__IPlayerRole*)m_pMaster)->send2Client(obData.data(), obData.size());
}

// 从数据库读取本命英雄信息
void CHeroManager::readLifeHeroFromDB()
{
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL || m_pMaster == NULL)
		return;

	DBREQ_PARAM_READ_LIFEHERO param;
	param.dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);
	if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_LIFEHERO, param.dwActorID, (LPCSTR)&param, sizeof(param), static_cast<IDBRetSink*>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() DBREQ_PARAM_READ_LIFEHERO failed");
		return;
	}
}

// 写入本命英雄信息到数据库
void CHeroManager::writeLifeHeroToDB(const SLifeHero & lifeHero)
{
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL || m_pMaster == NULL)
		return;

	DBREQ_PARAM_WRITE_LIFEHERO param;
	param.dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);
	param.nHeroID = lifeHero.nHeroID;
	param.nCultureID = lifeHero.nCultureID;
	if (pDBEngine->executeSP(GAMEDB_REQUEST_WRITE_LIFEHERO, param.dwActorID, (LPCSTR)&param, sizeof(param), static_cast<IDBRetSink*>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_WRITE_LIFEHERO failed");
		return;
	}
}

// 从数据库删除本命英雄信息
void CHeroManager::delLifeHeroFromDB(DWORD dwActorId, int nHeroId)
{
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL || m_pMaster == NULL)
		return;

	DBREQ_PARAM_DEL_LIFEHERO param;
	param.dwActorID = dwActorId;
	param.nHeroID = nHeroId;
	if (pDBEngine->executeSP(GAMEDB_REQUEST_DEL_LIFEHERO, m_pMaster->getIntProperty(PROPERTY_ID), (LPCSTR)&param, sizeof(param), static_cast<IDBRetSink*>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_DEL_LIFEHERO failed");
		return;
	}
}

// 记录本命英雄文字日志
void CHeroManager::recordLifeHeroGameNoteLog(enum LIFEHERO_NOTE_LOG_TYPE logType, const SLifeHero* pLifeHero1, const SLifeHero* pLifeHero2)
{
	int pdbid = m_pMaster->getIntProperty(PROPERTY_ID);
	ActorHelper helper(pdbid, ACTOR_DBID);
	IActorService * pActorService = helper.m_ptr;
	if (pActorService == NULL)
	{
		ErrorLn(__FUNCTION__": pActorService == NULL dwActorID= " << pdbid);
		return;
	}

	IOSSLogService *		pOSSLogServer = gServerGlobal->getOSSLogService();
	if (!pOSSLogServer)
	{
		ErrorLn(__FUNCTION__": pOSSLogServer == NULL dwActorID= " << pdbid);
		return;
	}

	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": LifeHero:pSchemeCenter == NULL !");
		return;
	}
	ISchemePersonModel * pPersonModel = pSchemeCenter->getSchemePersonModel();
	if (!pPersonModel)
		return;
	char szLogInfo[256] = { 0 };
	switch (logType)
	{
	case CHeroManager::LIFEHERO_ADD:
	{
		if (pLifeHero1)
		{
			const char * pHeroName = pPersonModel->getHeroName(pLifeHero1->nHeroID);
			ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("%s %d号槽位觉醒本命英雄%s"), pActorService->getName(), pLifeHero1->nCultureID + 1, pHeroName);
			pOSSLogServer->addGameNoteLog(m_pBankPart->getFromGameWorldID(), OSS_NOTE_TRACK_PERSON, pdbid, szLogInfo);
		}
	}
	break;
	case CHeroManager::LIFEHERO_REPLACE:
	{
		if (pLifeHero1 && pLifeHero2)
		{
			const char * pHeroName = pPersonModel->getHeroName(pLifeHero1->nHeroID);
			const char * pReplaceHeroName = pPersonModel->getHeroName(pLifeHero2->nHeroID);
			ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("%s %d号槽位本命英雄%s换成%s"), pActorService->getName(), pLifeHero1->nCultureID + 1, pHeroName, pReplaceHeroName);
			pOSSLogServer->addGameNoteLog(m_pBankPart->getFromGameWorldID(), OSS_NOTE_TRACK_PERSON, pdbid, szLogInfo);
		}
	}
	break;
	case CHeroManager::LIFEHERO_OBTAIN_PRIZE:
	{
		vector<SLifeHeroPrizeScheme>* pVecPrizeScheme = pSchemeCenter->getSchemeLifeHero()->getLifeHeroPrizeScehme();
		int taskID = m_pBankPart->getTaskDataIntEx(TASKKEY_ACTOR_LIFEHERO_PRIZE, 0);
		if (taskID < 0 || taskID >= pVecPrizeScheme->size())
		{
			ErrorLn(__FUNCTION__": LifeHero:taskID=" << taskID << "pVecPrizeScheme.size=" << pVecPrizeScheme->size());
			return;
		}
		SLifeHeroPrizeScheme scheme = pVecPrizeScheme->at(taskID);
		ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("%s 完成%d号目标%d名英雄等级%d获得本命英雄奖励"), pActorService->getName(), scheme.targetID, scheme.heroCount, scheme.starLv);
		pOSSLogServer->addGameNoteLog(m_pBankPart->getFromGameWorldID(), OSS_NOTE_TRACK_PERSON, pdbid, szLogInfo);
	}
	break;
	default:
		break;
	}
}

void CHeroManager::onDBReturnReadLifeHero(int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, void * pOutData, int nOutLen)
{
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__ << ",nDBRetCode=" << nDBRetCode << ",Desc=" << pszDBRetDesc << ",nQueueIndex=" << nQueueIndex);
		return;
	}

	if (pOutData == NULL)
	{
		ErrorLn(__FUNCTION__": failed pOutData == NULL");
		return;
	}

	int nPDBID = nQueueIndex;
	PDBID selfPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	if (nPDBID != selfPDBID || nPDBID == INVALID_UID)
	{
		ErrorLn(__FUNCTION__ << " Invilad ActorID !!! ");
		return;
	}

	if (nOutLen < sizeof(DBREQ_RESULT_READ_LIFEHERO))
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength =" << nOutLen << "< sizeof(DBREQ_RESULT_READ_LIFEHERO) = " << sizeof(DBREQ_RESULT_READ_LIFEHERO));
		return;
	}

	DBREQ_RESULT_READ_LIFEHERO* pCount = (DBREQ_RESULT_READ_LIFEHERO*)pOutData;
	if (pCount->nCount < 0 || pCount->nCount > ELifeHeroType::LIFEHERO_MAX_COUNT)
	{
		ErrorLn(__FUNCTION__":LifeHero:db return lifehero count=" << pCount->nCount);
		return;
	}

	// 效验长度值
	int nEfficacyLen = sizeof(DBREQ_TABLE_LIFEHERO) * pCount->nCount + sizeof(DBREQ_RESULT_READ_LIFEHERO);
	if (nOutLen < nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength =" << nOutLen << "< sizeof(DBREQ_TABLE_LIFEHERO) * count + sizeof(DBREQ_RESULT_READ_LIFEHERO) Size= " << nEfficacyLen);
		return;
	}

	m_mapLifeHero.clear();
	DBREQ_TABLE_LIFEHERO* pLifeHero = (DBREQ_TABLE_LIFEHERO*)((char*)pOutData + sizeof(int));
	for (int i = 0; i < pCount->nCount; ++i, ++pLifeHero)
	{
		SLifeHero lifeHero;
		lifeHero.nHeroID = pLifeHero->nHeroID;
		lifeHero.nCultureID = pLifeHero->nCultureID;
		m_mapLifeHero[lifeHero.nHeroID] = lifeHero;
	}
}

void CHeroManager::resetLifeHero()
{
	PDBID selfPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

	for (map<int, SLifeHero>::iterator it = m_mapLifeHero.begin(); it != m_mapLifeHero.end(); ++it)
	{
		delLifeHeroFromDB(selfPDBID, it->second.nHeroID);
	}

	m_mapLifeHero.clear();

	if (!m_pBankPart->setTaskDataIntEx(TASKKEY_ACTOR_LIFEHERO_FINISHED, 0))
	{
		ErrorLn(__FUNCTION__": setTaskDataIntEx(TASKKEY_ACTOR_LIFEHERO_FINISHED, 0) failed !");
		return;
	}
	m_pBankPart->sendLifeHeroTaskDataToClient();
	if (!m_pBankPart->setTaskDataIntEx(TASKKEY_ACTOR_LIFEHERO_PRIZE, 0))
	{
		ErrorLn(__FUNCTION__": setTaskDataIntEx(TASKKEY_ACTOR_LIFEHERO_PRIZE, 0) failed !");
		return;
	}
	m_pBankPart->sendLifeHeroPrizeTaskDataToClient();
	msg_entity_lifehero_query msgData;
	msgData.actorID = selfPDBID;
	onMessageQueryAndSendAllLifeHero(&msgData);
}

void CHeroManager::addLifeHero(int heroID, int cultureID)
{
	SLifeHero lifeHero;
	lifeHero.nHeroID = heroID;
	lifeHero.nCultureID = cultureID;

	onMessageAddLifeHero(lifeHero);
}

bool CHeroManager::checkIsLifeHero(int nHeroID)
{
	std::map<int, SLifeHero>::iterator it = m_mapLifeHero.find(nHeroID);
	if (it != m_mapLifeHero.end())
	{
		return true;
	}

	return false;
}

#pragma endregion 本命英雄相关


void CHeroManager::onDBReturnHeroSkinInfo(void * pOutData,int nOutLen,int nHeroCount)
{
    if(m_pMaster==NULL)
    {
        ErrorLn(__FUNCTION__": m_pMaster==NULL");
        return;
    }
    if(pOutData == NULL || nOutLen < sizeof(DBREQ_PARAM_HEROADDITIONAL))
    {
        //ErrorLn(__FUNCTION__": pOutData == NULL || nOutLen["<< nOutLen <<"] < sizeof(DBREQ_PARAM_HEROADDITIONAL)["<< sizeof(DBREQ_PARAM_HEROADDITIONAL) <<"]");
        return;
    }

    char * pHeroHead = 0;
    int nEfficacyLen = 0;
	for (int nIndex = 0; nIndex <nHeroCount; ++nIndex)
	{
        pHeroHead = (char*)(pOutData)+nEfficacyLen;

        nEfficacyLen += sizeof(DBREQ_PARAM_HEROADDITIONAL)+sizeof(int);
        // 已到数据最后
        if(nOutLen < nEfficacyLen)
        {
            break;
        }

        // 英雄附加信息数据
        DBREQ_PARAM_HEROADDITIONAL* pHeroData = (DBREQ_PARAM_HEROADDITIONAL*)(pHeroHead);
        int nHeroId = pHeroData->dwHeroID;
        // 取得此节点对应BUFF长度数据
        int * pBufSize  = (int*)(pHeroData+1);
        int & nBuffSize = (*pBufSize);
        // 此节点下没有BUFF
        if(nBuffSize <= 0)
        {
            continue;
        }
        // 已到数据最后
        if( nOutLen < nEfficacyLen + nBuffSize )
        {
            break;
        }

        // 定位到BUFF位置
        char * pBuff = (char*)(pBufSize+1);
        // 增加遍历长度
        nEfficacyLen += nBuffSize;
        // 无效英雄ID时不用读取下面BUFF中的数据了
        if(nHeroId <= 0)
        {
            WarningLn(__FUNCTION__": hero is invalid: hero="<<nHeroId <<", name="<<m_pMaster->getName());
            continue;
        }

        SHeroInfoFlags & heroInfo = getMapHeroInfo(nHeroId);

        // 遍历BUFF中的节点
        HERO_SKIN_INFO* pNode = (HERO_SKIN_INFO*)pBuff;
        // 存有节点数量
        int nNodeCount = nBuffSize / sizeof(HERO_SKIN_INFO);
        for( int i = 0; i < nNodeCount; ++i )
        {
            heroInfo.sHeroSkinList.SkinInfoVec.push_back(*pNode);			

            // 移到下一个节点
            ++pNode;
        }
	}
	sendHeroSkinInfoToClient();
}
// 检查皮肤是否有过期是否需要更新数据库 
void  CHeroManager::checkToCleanSkinToDB()
{
	DWORD dwCurrentTime = time(NULL);

	obuf obData;
	int nSkinLen = sizeof(HERO_SKIN_INFO);
	for (PLAYERHEROMAP::iterator iterHero = m_mapHero.begin(); 
		iterHero != m_mapHero.end();
		++iterHero)
	{
		bool bNeedClean = false;
		HERO_SKIN_VEC::iterator iterVec = iterHero->second.sHeroSkinList.SkinInfoVec.begin();
		for (;iterVec!= iterHero->second.sHeroSkinList.SkinInfoVec.end();)
		{
			bool bForeverSkin = iterVec->dwDurationTime == PERMANENT_SKIN_TIME;

			if(dwCurrentTime > iterVec->dwDurationTime&&!bForeverSkin)// 非永久皮肤过期
			{
				bNeedClean  = true;
				iterVec = iterHero->second.sHeroSkinList.SkinInfoVec.erase(iterVec);
			}
			else
			{
				++iterVec;
			}		
		}

		if(bNeedClean)
		{
			saveHeroSkinInfoToDB(iterHero->first);// 删掉过期的皮肤然后覆盖db
		}
	}
}

void CHeroManager::saveHeroSkinInfoToDB(int nHeroId)
{
	if(m_pMaster==NULL)
	{
		return;
	}
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return;
	}

    PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
    SHeroInfoFlags & heroInfo = getMapHeroInfo(nHeroId);
    HERO_SKIN_VEC & skins = heroInfo.sHeroSkinList.SkinInfoVec;

    int nNodeCount = skins.size();
	obuf obData;
	for (HERO_SKIN_VEC::iterator iterVec = skins.begin(); iterVec!= skins.end(); ++iterVec)
	{
		HERO_SKIN_INFO heroSkinInfo;
		heroSkinInfo.dwDurationTime = iterVec->dwDurationTime;
		heroSkinInfo.dwSkinID = iterVec->dwSkinID;
		obData.push_back(&heroSkinInfo, sizeof(heroSkinInfo));
	}
    
	int nBufSize = nNodeCount * sizeof(HERO_SKIN_INFO);

    DBREQ_PARAM_SAVE_HEROADDITIONAL pSaveDataHead;
    pSaveDataHead.dwActorID = dwPDBID;
    pSaveDataHead.nType     = HERO_ADD_TYPE_SKIN;
    pSaveDataHead.dwHeroID  = nHeroId;

	obuf oBuffer;
	oBuffer.push_back(&pSaveDataHead, sizeof(DBREQ_PARAM_SAVE_HEROADDITIONAL));
	oBuffer.push_back(&nBufSize, sizeof(nBufSize));
	oBuffer.push_back(obData.data(), obData.size());
	// 发给DB
	if (pDBEngine->executeSP(GAMEDB_REQUEST_SAVE_HEROADDITIONAL, dwPDBID, oBuffer.data(), oBuffer.size(), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__" : pDBEngine->executeSP() GAMEDB_REQUEST_SAVE_HEROADDITIONAL :HERO_ADD_TYPE_SKIN failed");
		return ;
	}
}

void CHeroManager::updateActiveHeroSkinToClient(HERO_SKIN_INFO * pHeroSkin,int nHeroId)
{
	if(m_pMaster == NULL)
	{
		return;
	}

	if(pHeroSkin == NULL)
		return;

	obuf obmsg;
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
	head.byKeyModule   = MSG_MODULEID_ENTITY;
	head.byKeyAction   = SC_MSG_ENTITY_HEROSKIN_UPTATE; // 英雄皮肤同步   
	// 部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
	entityHead.byPartID = PART_BANK;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

	obmsg << head << entityHead;

	obmsg.push_back(&nHeroId,sizeof(int));
	obmsg.push_back(pHeroSkin,sizeof(HERO_SKIN_INFO));

	__IPlayerRole *pPlayerRole=CAST_TYPE(__IPlayerRole*, m_pMaster);
	if(pPlayerRole)
	{
		pPlayerRole->send2Client(obmsg.data(), obmsg.size());
	}
}

void CHeroManager::sendHeroSkinInfoToClient()
{
	if(m_pMaster == NULL)
	{
		return;
	}

	// 发到客户端
	obuf obmsg;
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
	head.byKeyModule   = MSG_MODULEID_ENTITY;
	head.byKeyAction   = SC_MSG_ENTITY_HEROSKIN_INFO; // 英雄皮肤同步              

	// 部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
	entityHead.byPartID = PART_BANK;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

	obmsg << head << entityHead;

	int nHeroNum = m_mapHero.size();             // 英雄皮肤个数
	// 英雄数
	obmsg.push_back(&nHeroNum, sizeof(int));
	int nSingleInfoLen = sizeof(HERO_SKIN_INFO);
	// 往消息中添加英雄皮肤信息
	for (PLAYERHEROMAP::iterator iterHero = m_mapHero.begin(); 
		iterHero != m_mapHero.end();
		++iterHero)
	{
		int nSkinCount = iterHero->second.sHeroSkinList.SkinInfoVec.size();
		int nHeroId = iterHero->first;
		obmsg.push_back(&nSkinCount,sizeof(int));
		obmsg.push_back(&nHeroId,sizeof(int));

		for(HERO_SKIN_VEC::iterator iterSkin = iterHero->second.sHeroSkinList.SkinInfoVec.begin();
			iterSkin != iterHero->second.sHeroSkinList.SkinInfoVec.end();
			++iterSkin)
		{		
			obmsg.push_back(&*(iterSkin),nSingleInfoLen);
		}		
	}

	__IPlayerRole *pPlayerRole=CAST_TYPE(__IPlayerRole*, m_pMaster);
	if(pPlayerRole)
	{
		pPlayerRole->send2Client(obmsg.data(), obmsg.size());
	}
}

////////////////////////////////// 英雄xp技能///////////////////////////////////////
////////////////////////////////// 英雄xp技能逻辑///////////////////////////////////////
// 增加英雄xp技能
bool  CHeroManager:: addHeroXPSkill( int nHeroID,int nXPSkillID)
{
	Single_Hero_XP_Skill_Info heroXpSkill;
	heroXpSkill.nXPSkillID = nXPSkillID;
	heroXpSkill.bGuide = true;

	if(m_mapHero.find(nHeroID) == m_mapHero.end())
	{
		SHeroInfoFlags sHeroInfoFlags;
		sHeroInfoFlags.sHeroInfo.dwFlags |= HERO_FLAG_NOT_HAVE;		// 置为非拥有英雄
		sHeroInfoFlags.sHeroInfo.nHeroID = nHeroID;
		sHeroInfoFlags.sHeroInfo.nLevel = 1;
		sHeroInfoFlags.sHeroInfo.nHeroStar = 1;
		sHeroInfoFlags.bIsFree = false;				

		m_mapHero[nHeroID] = sHeroInfoFlags;
	}

	heroXPSkillList::iterator iterEnd = m_mapHero[nHeroID].sHeroXPSkillList.end();
	heroXPSkillList::iterator iterResult = find(m_mapHero[nHeroID].sHeroXPSkillList.begin(),iterEnd, heroXpSkill) ;
	if(iterResult!= iterEnd)
		return false;

	m_mapHero[nHeroID].sHeroXPSkillList.push_back(heroXpSkill);


	updateHeroXPSkillToDB(nHeroID);
	updateHeroXPSkillToCilent(nHeroID);
	return true;
}
// 是否可以增加xp技能
bool  CHeroManager::canAddHeroXPSkill(int nHeroID,int nXPSkillID)
{
	if(nHeroID<=0||nXPSkillID<=0)
		return false;

	// todo config
	if(!checkHeroXPIDIsCorrectInSchemeConfig(nHeroID,nXPSkillID))
		return false;

	// 获得XP增加上限判断
	PLAYERHEROMAP::iterator it = m_mapHero.find( nHeroID );
	if (it != m_mapHero.end())
	{
		std::vector<Single_Hero_XP_Skill_Info>::iterator skillIter = it->second.sHeroXPSkillList.begin();
		for ( ; skillIter != it->second.sHeroXPSkillList.end(); skillIter++)
		{
			if (skillIter->nXPSkillID == nXPSkillID)
			{
				return false;
			}
		}
	}
	
	return true;
}
// 更新英雄xp技能db
bool CHeroManager:: updateHeroXPSkillToDB(int nHeroID)
{
	if(m_pMaster==NULL)
	{
		return false;
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return false;
	}

    PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

    SHeroInfoFlags & flags = getMapHeroInfo(nHeroID);
    heroXPSkillList & xpList = flags.sHeroXPSkillList;

	int nXpCount = xpList.size();

    obuf obData;
	for (heroXPSkillList::iterator iter = xpList.begin(); iter != xpList.end(); ++iter)
	{
		Single_Hero_XP_Skill_Info node;
		node.nXPSkillID = iter->nXPSkillID;
		node.bGuide    = iter->bGuide;
		obData.push_back(&node, sizeof(node));
	}

	obuf oBuffer;
	int nBufSize = nXpCount * sizeof(Single_Hero_XP_Skill_Info);

    DBREQ_PARAM_SAVE_HEROADDITIONAL pSaveDataHead;
    pSaveDataHead.dwActorID = dwPDBID;
    pSaveDataHead.nType     = HERO_ADD_TYPE_XPSKILL;
    pSaveDataHead.dwHeroID  = nHeroID;

	oBuffer.push_back(&pSaveDataHead, sizeof(pSaveDataHead));
	oBuffer.push_back(&nBufSize, sizeof(nBufSize));
	oBuffer.push_back(obData.data(), obData.size());

	if (pDBEngine->executeSP(GAMEDB_REQUEST_SAVE_HEROADDITIONAL, dwPDBID, oBuffer.data(), oBuffer.size(), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__" : pDBEngine->executeSP() GAMEDB_REQUEST_SAVE_HEROADDITIONAL :HERO_ADD_TYPE_XPSKILL failed");
		return false;
	}
	return true;
}
// 更新英雄技能到客户端
bool CHeroManager::updateHeroXPSkillToCilent(int nHeroID)
{
	if(m_pMaster == NULL)
	{
		return false;
	}

	// 发到客户端
	obuf obmsg;
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
	head.byKeyModule   = MSG_MODULEID_ENTITY;
	head.byKeyAction   = SC_MSG_ENTITY_HERO_XP_SKILL_UPDATE; // 英雄皮肤同步              

	// 部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
	entityHead.byPartID = PART_BANK;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

	obmsg << head << entityHead;

	int nHeroNum = nHeroID == DEFALLHEROID?m_mapHero.size():1;            
	// 英雄数
	obmsg.push_back(&nHeroNum, sizeof(int));
	int nSingleInfoLen = sizeof(Single_Hero_XP_Skill_Info);
	// 往消息中添加英雄皮肤信息
	for (PLAYERHEROMAP::iterator iterHero = m_mapHero.begin(); 
		iterHero != m_mapHero.end();
		++iterHero)
	{
		if(iterHero->first!=nHeroID&&nHeroID != DEFALLHEROID)
			continue;

		int nSkillCount = iterHero->second.sHeroXPSkillList.size();
		int nHeroId = iterHero->first;
		obmsg.push_back(&nSkillCount,sizeof(int));
		obmsg.push_back(&nHeroId,sizeof(int));

		for(heroXPSkillList::iterator iterSkill = iterHero->second.sHeroXPSkillList.begin();
			iterSkill != iterHero->second.sHeroXPSkillList.end();
			++iterSkill)
		{		
			obmsg.push_back(&*(iterSkill),nSingleInfoLen);
		}		
	}

	__IPlayerRole *pPlayerRole=CAST_TYPE(__IPlayerRole*, m_pMaster);
	if(pPlayerRole)
	{
		pPlayerRole->send2Client(obmsg.data(), obmsg.size());
	}

	return true;
}

// 数据库返回
void CHeroManager::onDBReturnHeroXPSkillInfo(void * pOutData, int nOutLen, int nHeroCount)
{
	if(m_pMaster==NULL)
	{
        ErrorLn(__FUNCTION__": m_pMaster==NULL");
		return;
	}
    if(pOutData == NULL || nOutLen < sizeof(DBREQ_PARAM_HEROADDITIONAL))
    {
        //ErrorLn(__FUNCTION__": pOutData == NULL || nOutLen["<< nOutLen <<"] < sizeof(DBREQ_PARAM_HEROADDITIONAL)["<< sizeof(DBREQ_PARAM_HEROADDITIONAL) <<"]");
        return;
    }

    char * pHeroHead = 0;
	int nEfficacyLen = 0;
	for (int nIndex = 0; nIndex <nHeroCount; ++nIndex)
	{
        pHeroHead = (char*)(pOutData)+nEfficacyLen;

		nEfficacyLen += sizeof(DBREQ_PARAM_HEROADDITIONAL)+sizeof(int);
        // 已到数据最后
		if(nOutLen < nEfficacyLen)
		{
			break;
		}

        // 英雄附加信息数据
		DBREQ_PARAM_HEROADDITIONAL* pHeroData = (DBREQ_PARAM_HEROADDITIONAL*)(pHeroHead);
		int nHeroId = pHeroData->dwHeroID;
        // 取得此节点对应BUFF长度数据
        int * pBufSize  = (int*)(pHeroData+1);
        int & nBuffSize = (*pBufSize);
        // 此节点下没有BUFF
        if(nBuffSize <= 0)
        {
            continue;
        }
        // 已到数据最后
        if( nOutLen < nEfficacyLen + nBuffSize )
        {
            break;
        }
        
        // 定位到BUFF位置
        char * pBuff = (char*)(pBufSize+1);
        // 增加遍历长度
        nEfficacyLen += nBuffSize;
        // 无效英雄ID时不用读取下面BUFF中的数据了
        if(nHeroId <= 0)
        {
            WarningLn(__FUNCTION__": hero is invalid: hero="<<nHeroId <<", name="<<m_pMaster->getName());
            continue;
        }

        SHeroInfoFlags & heroInfo = getMapHeroInfo(nHeroId);

        // 遍历BUFF中的节点
        Single_Hero_XP_Skill_Info* pNode = (Single_Hero_XP_Skill_Info*)pBuff;        
        // 存有节点数量
        int nNodeCount = nBuffSize / sizeof(Single_Hero_XP_Skill_Info);
        for( int i = 0; i < nNodeCount; ++i )
        {
            heroInfo.sHeroXPSkillList.push_back(*pNode);			

            // 移到下一个节点
            ++pNode;
        }
	}
}

// 学习xp技能
void CHeroManager::learnHeroXPSpellInWar()
{
	if(m_pMaster == NULL)
		return;

	__IWarPart *pWarPart = (__IWarPart *)m_pMaster->getEntityPart(PART_WAR);
	if (pWarPart == NULL)
		return;
	
	// 静态场景不学习
	if(pWarPart->getWarID() == -1)
		return;

	__ISpellPart *pSpellPart = (__ISpellPart *)m_pMaster->getEntityPart(PART_SPELL);

	if (pSpellPart == NULL)
		return ;

	int nHeroID = m_pMaster->getIntProperty(PROPERTY_VOCATION);
	if(m_mapHero.find(nHeroID) != m_mapHero.end())
	{	
		int nUseXPID = m_mapHero[nHeroID].sHeroInfo.nUseXPID;
		if (checkHeroXPIDIsCorrectInSchemeConfig(nHeroID, nUseXPID))
		{
			pSpellPart->addSpell(nUseXPID, SLOT_TYPE_SKILL, XP_SLOT_INDEX, 0 );
		}
	}
}

// 大厅动态点选英雄xp技能
void CHeroManager::OnClientSelectHeroXPSkill( void * data, size_t len )
{
	if (data == NULL || len < sizeof(Msg_Hero_XP_Skill_Select) )
	{
		ErrorLn(__FUNCTION__": message data is null or length is invalid! len="<< len <<", sizeof="<< sizeof(Msg_Request_Other_Crown_Points)  );
		return ;
	}

	Msg_Hero_XP_Skill_Select *pData = (Msg_Hero_XP_Skill_Select *)data;

	if(!checkHeroXPIDIsCorrectInSchemeConfig(pData->nHeroID,pData->nXPSkillID))
		return ;

	PLAYERHEROMAP ::iterator itHeroResult = m_mapHero.find(pData->nHeroID);
	if( itHeroResult == m_mapHero.end())
	{
		ErrorLn(__FUNCTION__": nHeroID is error "<< pData->nHeroID  );
		return ;
	}

	Single_Hero_XP_Skill_Info skillInfo;
	skillInfo.nXPSkillID = pData->nXPSkillID;

	heroXPSkillList& tmpHeroXPSkillList = itHeroResult->second.sHeroXPSkillList;
	heroXPSkillList::iterator skillIter = find(tmpHeroXPSkillList.begin(), tmpHeroXPSkillList.end(), skillInfo);

	if (skillIter == tmpHeroXPSkillList.end())
	{
		ErrorLn(__FUNCTION__": There isn't xp skill on this hero, nHeroID=" << pData->nHeroID << ", xpSkillID=" << pData->nXPSkillID);
		return;
	}

	itHeroResult->second.sHeroInfo.nUseXPID = pData->nXPSkillID;
	itHeroResult->second.bIsChange = true;
	onUpdateChangedHeroInfo();

	if(!skillIter->bGuide)
	{
		skillIter->bGuide = false;
		updateHeroXPSkillToDB(pData->nHeroID);
		updateHeroXPSkillToCilent(pData->nHeroID);
	}
}

bool CHeroManager::checkHeroXPIDIsCorrectInSchemeConfig( int nHeroID, int nXPSkillID )
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL !");
		return false;
	}

	ISchemeHeroXpSkill* pSchemeHeroXpSkill = pSchemeCenter->getSchemeHeroXpSkill();
	if ( pSchemeHeroXpSkill == NULL )
	{
		ErrorLn(__FUNCTION__": pSchemeHeroXpSkill == NULL !");
		return false;
	}

	SHeroXPSkillConfig_S* pSchemeXpConfig = pSchemeHeroXpSkill->getHeroXpSkillConfig(nHeroID,nXPSkillID);
	if(pSchemeXpConfig == NULL)
	{
		return false;
	}

	return true;
}

// 自动激活默认xp技能
void CHeroManager::autoActiveHeroXpSkill()
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL !");
		return ;
	}

	ISchemeHeroXpSkill* pSchemeHeroXpSkill = pSchemeCenter->getSchemeHeroXpSkill();
	if ( pSchemeHeroXpSkill == NULL )
	{
		ErrorLn(__FUNCTION__": pSchemeHeroXpSkill == NULL !");
		return ;
	}

	const int nActiveCountMax = 512;
	SHeroXPSkillConfig_S activeXpSkillList[nActiveCountMax];
	if(!pSchemeHeroXpSkill->getHeroXpActiveList(activeXpSkillList,nActiveCountMax))
	{
		return;
	}

	for (int i =0 ;i<nActiveCountMax;++i)
	{
		SHeroXPSkillConfig_S* pConfig =&activeXpSkillList[i];
		if(pConfig->nHeroID<=0)
			continue;

		if(!canAddHeroXPSkill(pConfig->nHeroID,pConfig->nXPSkillID))
			continue;

		addHeroXPSkill(pConfig->nHeroID,pConfig->nXPSkillID);
	}

	return;
}

// 装备默认xp技能
void CHeroManager::equipDefaultHeroXPSKill()
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL !");
		return ;
	}

	ISchemeHeroXpSkill* pSchemeHeroXpSkill = pSchemeCenter->getSchemeHeroXpSkill();
	if ( pSchemeHeroXpSkill == NULL )
	{
		ErrorLn(__FUNCTION__": pSchemeHeroXpSkill == NULL !");
		return ;
	}

	bool bUpdateDB = false;
	SHeroXPSkillConfig_S* pSchemeXpConfig = NULL;
	for (PLAYERHEROMAP::iterator iterHero = m_mapHero.begin(); 
		iterHero != m_mapHero.end();
		++iterHero)
	{	
		if(iterHero->second.sHeroInfo.nUseXPID>0)
			continue;

		for(heroXPSkillList::iterator iterSkill = iterHero->second.sHeroXPSkillList.begin();
			iterSkill != iterHero->second.sHeroXPSkillList.end();
			++iterSkill)
		{		
			int nXPSkillID = iterSkill->nXPSkillID;
			pSchemeXpConfig =pSchemeHeroXpSkill->getHeroXpSkillConfig(iterHero->first,iterSkill->nXPSkillID);
			if(pSchemeXpConfig == NULL)
			{
				continue ;
			}

			if(!pSchemeXpConfig->bDefualtEquip)
				continue;

			if(iterHero->second.sHeroInfo.nUseXPID == nXPSkillID)
				continue;

			iterHero->second.sHeroInfo.nUseXPID = nXPSkillID;
			iterHero->second.bIsChange = true;
			iterSkill->bGuide = false;
			bUpdateDB  = true;
			break;
		}		
	}

	if(bUpdateDB)
		onUpdateChangedHeroInfo();
}

////////////////////////////////// 英雄xp技能碎片逻辑///////////////////////////////////////
// 增加英雄xp技能碎片
bool  CHeroManager::  addHeroXPSkillFragment( int nHeroID,int nXPSkillID,int nCount)
{
	Single_Hero_XP_Fragment_Info heroXpSkill;
	heroXpSkill.nXPSkillID = nXPSkillID;
	heroXpSkill.nXPFragmentCount = nCount;

	if(m_mapHero.find(nHeroID) == m_mapHero.end())
	{
		SHeroInfoFlags heroInfo;
		m_mapHero[nHeroID] = heroInfo;
	}

	heroXPFragmentList::iterator iterEnd = m_mapHero[nHeroID].sHeroXPFragmentList.end();
	heroXPFragmentList::iterator iterResult = find(m_mapHero[nHeroID].sHeroXPFragmentList.begin(),iterEnd, heroXpSkill) ;
	if(iterResult!= iterEnd)
	{
		iterResult->nXPFragmentCount+=nCount;
	}
	else
	{
		m_mapHero[nHeroID].sHeroXPFragmentList.push_back(heroXpSkill);
	}

	updateHeroXPSkillFragmentToDB(nHeroID);
	updateHeroXPSkillFragmentToCilent(nHeroID);
	return true;
}
// 是否可以增加xp技能碎片
bool  CHeroManager:: canAddHeroXPSkillFragment(int nHeroID,int nXPSkillID,int nCount)
{
	if(nHeroID<=0||nXPSkillID<=0||nCount<=0)
		return false;

	// todo config
	if(!checkHeroXPIDIsCorrectInSchemeConfig(nHeroID,nXPSkillID))
		return false;

	return true;
}
// 更新英雄xp技能碎片db
bool CHeroManager:: updateHeroXPSkillFragmentToDB(int nHeroID)
{
	if(m_pMaster==NULL)
	{
		return false;
	}
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return false;
	}

    PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
    SHeroInfoFlags & heroInfo = getMapHeroInfo(nHeroID);

    heroXPFragmentList & xpFrams = heroInfo.sHeroXPFragmentList;
    int nNodeCount = xpFrams.size();

	obuf obData;
	for (heroXPFragmentList::iterator iterVec = xpFrams.begin(); iterVec!= xpFrams.end(); ++iterVec)
	{
		Single_Hero_XP_Fragment_Info heroSkillInfo;
		heroSkillInfo.nXPSkillID = iterVec->nXPSkillID;
		heroSkillInfo.nXPFragmentCount = iterVec->nXPFragmentCount;
		obData.push_back(&heroSkillInfo, sizeof(heroSkillInfo));
	}

	int nBufSize = nNodeCount * sizeof(Single_Hero_XP_Fragment_Info);

    DBREQ_PARAM_SAVE_HEROADDITIONAL pSaveDataHead;
    pSaveDataHead.dwActorID = dwPDBID;
    pSaveDataHead.nType     = HERO_ADD_TYPE_XPFRAGMENT;
    pSaveDataHead.dwHeroID  = nHeroID;

	obuf oBuffer;
	oBuffer.push_back(&pSaveDataHead, sizeof(pSaveDataHead));
	oBuffer.push_back(&nBufSize, sizeof(nBufSize));
	oBuffer.push_back(obData.data(), obData.size());
	// 发给DB
	if (pDBEngine->executeSP(GAMEDB_REQUEST_SAVE_HEROADDITIONAL, dwPDBID, oBuffer.data(), oBuffer.size(), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__" : pDBEngine->executeSP() GAMEDB_REQUEST_SAVE_HEROADDITIONAL :HERO_ADD_TYPE_XPFRAGMENT failed");
		return false;
	}

	return true;
}
// 更新英雄技能到碎片客户端
bool CHeroManager::	updateHeroXPSkillFragmentToCilent(int nHeroID)
{
	if(m_pMaster == NULL)
	{
		return false;
	}

	// 发到客户端
	obuf obmsg;
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
	head.byKeyModule   = MSG_MODULEID_ENTITY;
	head.byKeyAction   = SC_MSG_ENTITY_HERO_XP_FRAGMENT_UPDATE; // 英雄皮肤同步              

	// 部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
	entityHead.byPartID = PART_BANK;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

	obmsg << head << entityHead;

	int nHeroNum = nHeroID == DEFALLHEROID?m_mapHero.size():1;            
	// 英雄数
	obmsg.push_back(&nHeroNum, sizeof(int));
	int nSingleInfoLen = sizeof(Single_Hero_XP_Fragment_Info);
	// 往消息中添加英雄皮肤信息
	for (PLAYERHEROMAP::iterator iterHero = m_mapHero.begin(); 
		iterHero != m_mapHero.end();
		++iterHero)
	{
		if(iterHero->first!=nHeroID&&nHeroID != DEFALLHEROID)
			continue;

		int nSkillCount = iterHero->second.sHeroXPFragmentList.size();
		int nHeroId = iterHero->first;
		obmsg.push_back(&nSkillCount,sizeof(int));
		obmsg.push_back(&nHeroId,sizeof(int));

		for(heroXPFragmentList::iterator iterSkill = iterHero->second.sHeroXPFragmentList.begin();
			iterSkill != iterHero->second.sHeroXPFragmentList.end();
			++iterSkill)
		{		
			obmsg.push_back(&*(iterSkill),nSingleInfoLen);
		}		
	}

	__IPlayerRole *pPlayerRole=CAST_TYPE(__IPlayerRole*, m_pMaster);
	if(pPlayerRole)
	{
		pPlayerRole->send2Client(obmsg.data(), obmsg.size());
	}

	return true;
}
// 数据库返回
void CHeroManager::onDBReturnHeroXPSkillFragmentInfo(void * pOutData,int nOutLen,int nHeroCount)
{
    if(m_pMaster==NULL)
    {
        ErrorLn(__FUNCTION__": m_pMaster==NULL");
        return;
    }
    if(pOutData == NULL || nOutLen < sizeof(DBREQ_PARAM_HEROADDITIONAL))
    {
        //ErrorLn(__FUNCTION__": pOutData == NULL || nOutLen["<< nOutLen <<"] < sizeof(DBREQ_PARAM_HEROADDITIONAL)["<< sizeof(DBREQ_PARAM_HEROADDITIONAL) <<"]");
        return;
    }

    char * pHeroHead = 0;
	int nEfficacyLen = 0;
	for (int nIndex = 0; nIndex <nHeroCount; ++nIndex)
	{
        pHeroHead = (char*)(pOutData)+nEfficacyLen;

        nEfficacyLen += sizeof(DBREQ_PARAM_HEROADDITIONAL)+sizeof(int);
        // 已到数据最后
        if(nOutLen < nEfficacyLen)
        {
            break;
        }

        // 英雄附加信息数据
        DBREQ_PARAM_HEROADDITIONAL* pHeroData = (DBREQ_PARAM_HEROADDITIONAL*)(pHeroHead);
        int nHeroId = pHeroData->dwHeroID;
        // 取得此节点对应BUFF长度数据
        int * pBufSize  = (int*)(pHeroData+1);
        int & nBuffSize = (*pBufSize);
        // 此节点下没有BUFF
        if(nBuffSize <= 0)
        {
            continue;
        }
        // 已到数据最后
        if( nOutLen < nEfficacyLen + nBuffSize )
        {
            break;
        }

        // 定位到BUFF位置
        char * pBuff = (char*)(pBufSize+1);
        // 增加遍历长度
        nEfficacyLen += nBuffSize;
        // 无效英雄ID时不用读取下面BUFF中的数据了
        if(nHeroId <= 0)
        {
            WarningLn(__FUNCTION__": hero is invalid: hero="<<nHeroId <<", name="<<m_pMaster->getName());
            continue;
        }

        SHeroInfoFlags & heroInfo = getMapHeroInfo(nHeroId);

        // 遍历BUFF中的节点
        Single_Hero_XP_Fragment_Info* pNode = (Single_Hero_XP_Fragment_Info*)pBuff;
        // 存有节点数量
        int nNodeCount = nBuffSize / sizeof(Single_Hero_XP_Fragment_Info);
        for( int i = 0; i < nNodeCount; ++i )
        {
            heroInfo.sHeroXPFragmentList.push_back(*pNode);			

            // 移到下一个节点
            ++pNode;
        }
	}

	updateHeroXPSkillFragmentToCilent();
}

// 解锁xp 通过碎片
void CHeroManager::OnClientUnlockXPSkillByFragments( void * data, size_t len )
{
	if (data == NULL || len < sizeof(Msg_Hero_XP_Skill_Unlock) )
	{
		ErrorLn(__FUNCTION__": message data is null or length is invalid! len="<< len <<", sizeof="<< sizeof(Msg_Request_Other_Crown_Points)  );
		return ;
	}

	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL !");
		return ;
	}

	ISchemeHeroXpSkill* pSchemeHeroXpSkill = pSchemeCenter->getSchemeHeroXpSkill();
	if ( pSchemeHeroXpSkill == NULL )
	{
		ErrorLn(__FUNCTION__": pSchemeHeroXpSkill == NULL !");
		return ;
	}

	Msg_Hero_XP_Skill_Unlock *pData = (Msg_Hero_XP_Skill_Unlock *)data;
	PLAYERHEROMAP ::iterator itHeroResult = m_mapHero.find(pData->nHeroID);
	if( itHeroResult == m_mapHero.end())
	{
		ErrorLn(__FUNCTION__": nHeroID is error "<< pData->nHeroID  );
		return ;
	}

	Single_Hero_XP_Skill_Info heroXPSkil;
	heroXPSkil.nXPSkillID = pData->nXPSkillID;
	heroXPSkillList::iterator itXPSkillResult = find(itHeroResult->second.sHeroXPSkillList.begin(),itHeroResult->second.sHeroXPSkillList.end(),heroXPSkil);
	if(itXPSkillResult != itHeroResult->second.sHeroXPSkillList.end())
		return;

	Single_Hero_XP_Fragment_Info heroXPFragment;
	heroXPFragment.nXPSkillID = pData->nXPSkillID;
	heroXPFragmentList::iterator itXPFragmentResult = find(itHeroResult->second.sHeroXPFragmentList.begin(),itHeroResult->second.sHeroXPFragmentList.end(),heroXPFragment);
	if(itXPFragmentResult == itHeroResult->second.sHeroXPFragmentList.end())
		return;

	SHeroXPSkillConfig_S* pSchemeXpConfig = pSchemeHeroXpSkill->getHeroXpSkillConfig(pData->nHeroID,pData->nXPSkillID);
	if(pSchemeXpConfig == NULL)
		return ;

	if(itXPFragmentResult->nXPFragmentCount<pSchemeXpConfig->nLockXPFragmentNum)
		return;

	itXPFragmentResult->nXPFragmentCount-=pSchemeXpConfig->nLockXPFragmentNum;
	int nXPType = pSchemeXpConfig->nXPType;

	// 通用技能 所有英雄都开启 专属只开启本英雄
	if(nXPType == (int)EMXP_SKILLTYPE_GENERAL)
	{
		for (int i = 1; i < VOCATION_MAX; ++i )
		{
			if(!canAddHeroXPSkill(i,pData->nXPSkillID))
				continue;

			addHeroXPSkill(i,pData->nXPSkillID);
		}
	}
	else if(nXPType ==(int) EMXP_SKILLTYPE_EXCLUSIVE)
	{
		if(!canAddHeroXPSkill(pData->nHeroID,pData->nXPSkillID))
			return;

		addHeroXPSkill(pData->nHeroID,pData->nXPSkillID);
	}
	
	return;
}

SHeroInfoFlags&  CHeroManager::getMapHeroInfo(int nHeroID)
{
    bool isNew = (m_mapHero.find(nHeroID) == m_mapHero.end() );
    SHeroInfoFlags & heroInfo = m_mapHero[nHeroID];
    // 第一次加入时要初始化这些数据
    if(isNew)
    {
        heroInfo.sHeroInfo.dwFlags |= HERO_FLAG_NOT_HAVE;		// 置为非拥有英雄
        heroInfo.sHeroInfo.nHeroID = nHeroID;
        heroInfo.sHeroInfo.nLevel = 1;
        heroInfo.sHeroInfo.nHeroStar = 1;
        heroInfo.bIsFree = false;
    }

    return heroInfo;
}

bool CHeroManager::getHeroIsFree(int nHeroID)
{
    PLAYERHEROMAP::iterator iter = m_mapHero.find(nHeroID);
    if(iter != m_mapHero.end())
    {
        return iter->second.bIsFree;
    }

    return false;
}



////////////////////////////////// 英雄xp技能///////////////////////////////////////

