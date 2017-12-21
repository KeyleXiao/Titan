#include "Stdafx.h"
#include "HeroManager.h"
#include "IClientGlobal.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"
#include "EntityBase.h"
#include "IBankPart.h"

CHeroManager::CHeroManager()
{
    m_playerHeroMap.clear();
    m_systemHeroVec.clear();
}
CHeroManager::~CHeroManager()
{
    m_playerHeroMap.clear();
    m_systemHeroVec.clear();
}

bool CHeroManager::onSerialize(rkt::ibuffer& in, int nCount)
{
    m_playerHeroMap.clear();
    for ( int i = 0; i < nCount; ++i )
    {
        if ( !in.good())
        {
            return false;
        }
        SHeroInfo sNode;
        in >> sNode;

		m_playerHeroMap[sNode.nHeroID].sHeroInfo = sNode;
	}


	return true;
}

bool CHeroManager::deSerialize(rkt::obuf& out)
{
    PLAYERHEROMAP::iterator it = m_playerHeroMap.begin();
    for (; it != m_playerHeroMap.end(); ++it )
    {
        out.push_back( &it->second.sHeroInfo, sizeof(SHeroInfo));
    }
    return true;
}

bool CHeroManager::onLoad( IEntity* pEntity)
{
    m_pMaster = pEntity;

	if(m_pMaster->isHero())
	{
		// 临时增加系统英雄
		for (int i = 1; i < VOCATION_MAX; ++i )
		{
			m_systemHeroVec.push_back(i);
		}
	}

	// 添加周免英雄列表
	setHeroInfoByFreeWeekly();
    return true;
}

int CHeroManager::getPlayerHeroArray(SHeroInfo* pArrHero, int nArrayCount)
{
    PLAYERHEROMAP::iterator itBegin = m_playerHeroMap.begin();
    int nCount = 0;
    for (; itBegin != m_playerHeroMap.end() && nCount< nArrayCount; ++itBegin )
    {
		// 已购买英雄
		if (isHeroHave(itBegin->second.sHeroInfo.dwFlags))
		{
			pArrHero[nCount++] = itBegin->second.sHeroInfo;
		}
    }
    return nCount;
}

int CHeroManager::getPlayerCanUseHeroArray(SHeroInfo* pArrHero, int nArrayCount)
{
	PLAYERHEROMAP::iterator itBegin = m_playerHeroMap.begin();
	int nCount = 0;
	for (; itBegin != m_playerHeroMap.end() && nCount< nArrayCount; ++itBegin )
	{
		// 周免或者已购买英雄
		if (itBegin->second.bIsFree || isHeroHave(itBegin->second.sHeroInfo.dwFlags))
		{
			pArrHero[nCount++] = itBegin->second.sHeroInfo;
		}
	}
	return nCount;
}

int* CHeroManager::getSystemHeros(WORD& nCount)
{
    nCount = m_systemHeroVec.size();
    return (nCount == 0) ? NULL : &m_systemHeroVec[0];
}

SHeroInfo* CHeroManager::getHeroInfoByID(int nHeroID)
{
    PLAYERHEROMAP::iterator it = m_playerHeroMap.find(nHeroID);
	if ( it == m_playerHeroMap.end() )
	{
		return NULL;
	}


	if (!it->second.bIsFree && !isHeroHave(it->second.sHeroInfo.dwFlags))
	{
		return NULL;
	}

	return &it->second.sHeroInfo;
}

SHeroInfo* CHeroManager::getPossessOrFreeHeroInfoByID(int nHeroID) 
{
	PLAYERHEROMAP::iterator it = m_playerHeroMap.find(nHeroID);
	if ( it == m_playerHeroMap.end() )
	{
		return NULL;
	}

	// 若不是周免也不是已拥有英雄 那就是第三种未拥有却有数据
	if (!it->second.bIsFree && !isHeroHave(it->second.sHeroInfo.dwFlags))
	{
		return NULL;
	}

	return &it->second.sHeroInfo;
}

DBREQ_PARAM_HERO_CARD CHeroManager::getHeroCardInfo( DWORD dwCardID )
{
    std::map<DWORD, DBREQ_PARAM_HERO_CARD>::iterator it = m_mapHeroCard.find( dwCardID );
    if ( it == m_mapHeroCard.end() )
    {
        return DBREQ_PARAM_HERO_CARD();
    }

    return it->second;
}

void CHeroManager::updatePlayerHeroInfo(SHeroInfo* pHeroInfo)
{
    if( pHeroInfo == NULL)
        return;

    m_playerHeroMap[pHeroInfo->nHeroID].sHeroInfo = *pHeroInfo;
}

void CHeroManager::updateHeroCardInfo(LPCSTR pszMsg, int nlen)
{
    if ( pszMsg == NULL || nlen < sizeof(int) )
        return;

    int nCount = *(int*)pszMsg;
    if ( nlen - sizeof(int) < nCount * sizeof(DBREQ_PARAM_HERO_CARD))
        return;

    DBREQ_PARAM_HERO_CARD* pCardInfo = (DBREQ_PARAM_HERO_CARD*)(pszMsg+sizeof(int));
    if ( pCardInfo == NULL)
        return;

	obuf obCmd;
	obCmd.push_back(&nCount, sizeof(nCount));
    for ( int i = 0; i < nCount; ++i )
    {
        m_mapHeroCard[pCardInfo->dwCardID] = *pCardInfo;

		cmd_entity_update_card_info cmdNode;
		cmdNode.nCardID = pCardInfo->dwCardID;
		cmdNode.nCardCount = pCardInfo->nCardCount;
		obCmd.push_back(&cmdNode, sizeof(cmdNode));

        pCardInfo++;
    }

    m_pMaster->sendCommandToEntityView( ENTITY_TOVIEW_RETURN_UPGRADE_CARD_INFO, 0, 0, obCmd.data(), obCmd.size());
}

void CHeroManager::onMessage(int msgId, void * data, size_t len)
{
	switch (msgId)
	{
	case SC_MSG_ENTITY_LIFEHERO_QUERY_RESULT:
	{
		onMessageLifeHeroQueryResult(data, len);
	}
	break;
	case SC_MSG_ENTITY_LIFEHERO_UPDATE:
	{
		onMessageUpdateLifeHero(data, len);
	}
	break;
	default:
		break;
	}
}

bool CHeroManager::onCommand(int cmdid, void * data, size_t len)
{
	switch (cmdid)
	{
	case ENTITY_CMD_LIFEHERO_ADD:
	{
		if (data == NULL || len < sizeof(cmd_entity_lifehero_info))
		{
			ErrorLn(__FUNCTION__ << " : ENTITY_CMD_LIFEHERO_ADD data is null or length is invalid! len = " << len << ", sizeof = " << sizeof(cmd_entity_lifehero_info) << ", UID = " << m_pMaster->getUID());
			return false;
		}
		cmd_entity_lifehero_info * pLifeHero = (cmd_entity_lifehero_info*)data;
		onCommandAddLifeHeroRequest(pLifeHero);
	}
	break;
	case ENTITY_CMD_LIFEHERO_DEL:
	{
		if (data == NULL || len < sizeof(cmd_entity_lifehero_del))
		{
			ErrorLn(__FUNCTION__ << " : ENTITY_CMD_LIFEHERO_DEL data is null or length is invalid! len = " << len << ", sizeof = " << sizeof(cmd_entity_lifehero_del) << ", UID = " << m_pMaster->getUID());
			return false;
		}
		cmd_entity_lifehero_del * pData = (cmd_entity_lifehero_del*)data;
		onCommandDelLifeHeroRequest(pData);
	}
	break;
	case ENTITY_CMD_LIFEHERO_REPLACE:
	{
		if (data == NULL || len < sizeof(cmd_entity_lifehero_replace))
		{
			ErrorLn(__FUNCTION__ << " : ENTITY_CMD_LIFEHERO_REPLACE data is null or length is invalid! len = " << len << ", sizeof = " << sizeof(cmd_entity_lifehero_replace) << ", UID = " << m_pMaster->getUID());
			return false;
		}
		cmd_entity_lifehero_replace * pData = (cmd_entity_lifehero_replace*)data;
		onCommandReplaceLifeHeroRequest(pData);
	}
	break;
	case ENTITY_CMD_LIFEHERO_EXCHANGE:
	{
		if (data == NULL || len < sizeof(cmd_entity_lifehero_exchange))
		{
			ErrorLn(__FUNCTION__ << " : ENTITY_CMD_LIFEHERO_EXCHANGE data is null or length is invalid! len = " << len << ", sizeof = " << sizeof(cmd_entity_lifehero_exchange) << ", UID = " << m_pMaster->getUID());
			return false;
		}
		cmd_entity_lifehero_exchange * pData = (cmd_entity_lifehero_exchange*)data;
		onCommandExchangeLifeHeroRequest(pData);
	}
	break;
	case ENTITY_CMD_LIFEHERO_QUERY:
	{
		if (data == NULL || len < sizeof(cmd_entity_lifehero_query))
		{
			ErrorLn(__FUNCTION__ << " : ENTITY_CMD_LIFEHERO_QUERY data is null or length is invalid! len = " << len << ", sizeof = " << sizeof(cmd_entity_lifehero_query) << ", UID = " << m_pMaster->getUID());
			return false;
		}
		cmd_entity_lifehero_query * pData = (cmd_entity_lifehero_query*)data;
		onCommandQueryLifeHeroRequest(pData);
	}
	break;
	case ENTITY_CMD_LIFEHERO_OBTAIN_PRIZE:
	{
		onCommandLifeHeroObtainPrize();
	}
	break;
	default:
		return false;
	}

	return true;
}

void CHeroManager::onCommandAddLifeHeroRequest(cmd_entity_lifehero_info* pLifeHero)
{
	if (!pLifeHero)
		return;

	msg_entity_lifehero_info lifeHero;
	lifeHero.nHeroID = pLifeHero->nHeroID;
	lifeHero.nCultureID = pLifeHero->nCultureID;
	SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_LIFEHERO_ADD, &lifeHero, sizeof(msg_entity_lifehero_info));
}

void CHeroManager::onCommandDelLifeHeroRequest(cmd_entity_lifehero_del *pData)
{
	if (!pData)
		return;

	msg_entity_lifehero_del delData;
	delData.heroID = pData->heroID;
	SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_LIFEHERO_DEL, &delData, sizeof(msg_entity_lifehero_del));
}

void CHeroManager::onCommandReplaceLifeHeroRequest(cmd_entity_lifehero_replace *pData)
{
	if (!pData)
		return;

	msg_entity_lifehero_replace replaceData;
	replaceData.heroID = pData->heroID;
	replaceData.hero.nHeroID = pData->hero.nHeroID;
	replaceData.hero.nCultureID = pData->hero.nCultureID;

	SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_LIFEHERO_REPLACE, &replaceData, sizeof(msg_entity_lifehero_replace));
}

void CHeroManager::onCommandExchangeLifeHeroRequest(cmd_entity_lifehero_exchange *pData)
{
	if (!pData)
		return;

	msg_entity_lifehero_exchange exchangeData;
	exchangeData.heroID1 = pData->heroID1;
	exchangeData.heroID2 = pData->heroID2;

	SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_LIFEHERO_EXCHANGE, &exchangeData, sizeof(msg_entity_lifehero_exchange));
}

void CHeroManager::onCommandQueryLifeHeroRequest(cmd_entity_lifehero_query *pData)
{
	if (!pData)
		return;

	msg_entity_lifehero_query queryData;
	queryData.actorID = pData->actorID;

	SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_LIFEHERO_QUERY, &queryData, sizeof(msg_entity_lifehero_query));
}

void CHeroManager::onCommandLifeHeroObtainPrize()
{
	SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_LIFEHERO_OBTAIN_PRIZE, NULL, 0);
}

void CHeroManager::onMessageLifeHeroQueryResult(void * data, size_t len)
{
	if (data == NULL || len < sizeof(int))
	{
		ErrorLn(__FUNCTION__ << " : SC_MSG_ENTITY_LIFEHERO_QUERY_RESULT data is null or length is invalid! len = " << len << ", sizeof = " << sizeof(int) << ", UID = " << m_pMaster->getUID());
		return;
	}
	
	int *pCount = (int*)data;
	int count = *pCount;
	if (len < sizeof(int) + sizeof(msg_entity_lifehero_info) * count)
	{
		ErrorLn(__FUNCTION__ << " : SC_MSG_ENTITY_LIFEHERO_QUERY_RESULT length is invalid!len = " << len << ", sizeof = " << sizeof(int) + sizeof(msg_entity_lifehero_info) * count << ", UID = " << m_pMaster->getUID());
		return;
	}

	cmd_count cmdCount;
	cmdCount.nCount = count;
	obuf obData;
	obData << cmdCount;
	msg_entity_lifehero_info* pData = (msg_entity_lifehero_info*)(pCount + 1);
	for (int i = 0; i < count; ++i, ++pData)
	{
		cmd_entity_lifehero_info node;
		node.nHeroID = pData->nHeroID;
		node.nCultureID = pData->nCultureID;
		obData.push_back(&node, sizeof(cmd_entity_lifehero_info));
	}

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_ALL_LIFEHERO, 0, 0, obData.data(), obData.size());
}

void CHeroManager::onMessageUpdateLifeHero(void * data, size_t len)
{
	if (data == NULL || len < sizeof(msg_entity_lifehero_info))
	{
		ErrorLn(__FUNCTION__ << " : SC_MSG_ENTITY_LIFEHERO_UPDATE data is null or length is invalid! len = " << len << ", sizeof = " << sizeof(msg_entity_lifehero_info) << ", UID = " << m_pMaster->getUID());
		return;
	}

	msg_entity_lifehero_info* pData = (msg_entity_lifehero_info*)data;
	cmd_entity_lifehero_info node;
	node.nHeroID = pData->nHeroID;
	node.nCultureID = pData->nCultureID;
	obuf obData;
	obData.push_back(&node, sizeof(cmd_entity_lifehero_info));

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_UPDATE_LIFEHERO, 0, 0, obData.data(), obData.size());
}

bool CHeroManager::isSystemHero(int nHeroID)
{
    SYSTEMHEROVEC::iterator iter =  m_systemHeroVec.begin();
    for (;iter != m_systemHeroVec.end();++iter)
    {
        if (nHeroID == *iter)
        {
            return true;
        }
    }
    return false;
}

void CHeroManager::setHeroInfoByFreeWeekly()
{
	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
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
	IEntityProperty *pProperty = (IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
	if (pProperty == NULL)
	{
		ErrorLn(__FUNCTION__": pProperty == NULL !");
		return;
	}
	int nPlayerLv = pProperty->getProperty_Integer(PROPERTY_LEVEL);

	const int nFreeHeroArrayLen = 2 * HEROFREEWEEKLY_FREEID;
	int arrayFreeHero[nFreeHeroArrayLen] = {0};
	int nFreeHeroCnt = pSchemeHeroFreeWeekly->GetFreeHeroListByLv(arrayFreeHero, nFreeHeroArrayLen, nPlayerLv);

	// 置为周免英雄
	for (int i = 0; i < nFreeHeroCnt; i++)
	{
		PLAYERHEROMAP::iterator iter = m_playerHeroMap.find( arrayFreeHero[i] );

		if (iter == m_playerHeroMap.end())
		{
			SHeroInfoFlags sHeroInfoFlags;

			sHeroInfoFlags.sHeroInfo.dwFlags |= HERO_FLAG_NOT_HAVE;		// 置为非购买英雄
			sHeroInfoFlags.sHeroInfo.nHeroID = arrayFreeHero[i];
			sHeroInfoFlags.sHeroInfo.nLevel = 1;
			sHeroInfoFlags.sHeroInfo.nHeroStar = 1;
			sHeroInfoFlags.bIsFree = true;								// 置为周免英雄

			m_playerHeroMap[arrayFreeHero[i]] = sHeroInfoFlags;
		}
		else
		{
			iter->second.bIsFree = true;								// 置为周免英雄
		}
	}
}

void CHeroManager::onMsgUpdateHeroXPSkillInfo(LPCSTR pszMsg, size_t len )
{
	if (pszMsg == NULL)
	{
		return;
	}

	if(m_pMaster == NULL)
	{
		return;
	}

	int nEfficacyLen=sizeof(int);
	if(len<nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<len<<"< int Size= "<<nEfficacyLen);
		return ;
	}

	int* pHeroInfoNum =(int*)pszMsg;
	char *pHead=(char*)(pHeroInfoNum+1);

	obuf obData;
	cmd_count heroCount;
	heroCount.nCount = *pHeroInfoNum;
	obData.push_back(&heroCount,sizeof(cmd_count));
	int nSkillCmdItemLen = sizeof(cmd_Hero_XP_Skill_Info);
	int nSkillItemLen =  sizeof(Single_Hero_XP_Skill_Info);
	for (int nIndex = 0; nIndex < heroCount.nCount; ++nIndex)
	{		
		int nSkillCount  = (*pHead);
		pHead = pHead+sizeof(int);
		int nHeroId = (*pHead);
		pHead = pHead+sizeof(int);

		cmd_Hero_Additional_Info heroHead;
		heroHead.nAdditionalCount = nSkillCount;
		heroHead.nHeroID = nHeroId;
		obData.push_back(&heroHead,sizeof(heroHead));

		for (int i=0;i<nSkillCount;++i )
		{
			Single_Hero_XP_Skill_Info* pItem = (Single_Hero_XP_Skill_Info*)(pHead);
			cmd_Hero_XP_Skill_Info cmdHeroSkill;
			cmdHeroSkill.nHeroID = nHeroId;
			cmdHeroSkill.nXPSkillID = pItem->nXPSkillID;
			cmdHeroSkill.nGuide = pItem->bGuide?1:0;

			obData.push_back(&cmdHeroSkill, nSkillCmdItemLen);
			pHead += nSkillItemLen;
		}
	}

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_HERO_XP_SKILL_UPDATE,0, 0, obData.data(), obData.size());
}

void CHeroManager::onMsgUpdateHeroXPFragementInfo( LPCSTR pszMsg, size_t len )
{
	if (pszMsg == NULL)
	{
		return;
	}

	if(m_pMaster == NULL)
	{
		return;
	}

	int nEfficacyLen=sizeof(int);
	if(len<nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<len<<"< int Size= "<<nEfficacyLen);
		return ;
	}

	int* pHeroInfoNum =(int*)pszMsg;
	char *pHead=(char*)(pHeroInfoNum+1);

	obuf obData;
	cmd_count heroCount;
	heroCount.nCount = *pHeroInfoNum;
	obData.push_back(&heroCount,sizeof(cmd_count));
	int nSkillCmdItemLen = sizeof(cmd_Hero_XP_Fragment_Info);
	int nSkillItemLen =  sizeof(Single_Hero_XP_Fragment_Info);
	for (int nIndex = 0; nIndex < heroCount.nCount; ++nIndex)
	{		
		int nSkillCount  = (*pHead);
		pHead = pHead+sizeof(int);
		int nHeroId = (*pHead);
		pHead = pHead+sizeof(int);

		cmd_Hero_Additional_Info heroHead;
		heroHead.nAdditionalCount = nSkillCount;
		heroHead.nHeroID = nHeroId;
		obData.push_back(&heroHead,sizeof(heroHead));

		for (int i=0;i<nSkillCount;++i )
		{
			Single_Hero_XP_Fragment_Info* pItem = (Single_Hero_XP_Fragment_Info*)(pHead);
			cmd_Hero_XP_Fragment_Info cmdHeroSkill;
			cmdHeroSkill.nHeroID = nHeroId;
			cmdHeroSkill.nXPSkillID = pItem->nXPSkillID;
			cmdHeroSkill.nXPFragmentCount = pItem->nXPFragmentCount;

			obData.push_back(&cmdHeroSkill, nSkillCmdItemLen);
			pHead += nSkillItemLen;
		}
	}

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_HERO_XP_FRAGMENT_UPDATE,0, 0, obData.data(), obData.size());
}
