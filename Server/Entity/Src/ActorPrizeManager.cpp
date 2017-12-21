#include "stdafx.h"
#include "PlayerBankPart.h"
#include "ActorPrizeManager.h"
#include "TaskStorageDef.h"
#include "IPlayerRole.h"
#include "IRuneManager.h"
#include "IOSSLogService.h"
#include "OssLogDef.h"
#include "ExternalHelper.h"
#include "TaskDef.h"

// ���캯��
CActorPrizeManager::CActorPrizeManager()
{
	m_pMaster = NULL;
	m_pBankPart = NULL;
	m_nChestResultBroadcastFlag = DEFAULTBROADCASTFLAG;
	m_strChestResult.clear();
	m_mapChestInfo.clear();
}

// ��������
CActorPrizeManager::~CActorPrizeManager()
{

}

// ���л�
bool CActorPrizeManager::onSerialize(ibuffer& in)
{
	if (!in.good())
		return false;

	int nCount = 0;
	in >> nCount;

	m_mapChestInfo.clear();
	for (int i = 0; i < nCount; ++i)
	{
		if (!in.good())
			return false;

		DWORD dwChestID, dwChestCount;
		in >> dwChestID >> dwChestCount;
		m_mapChestInfo[dwChestID] = dwChestCount;
	}

	return true;
}

// �����л�
bool CActorPrizeManager::deSerialize(obuf& out)
{
	int nCount = m_mapChestInfo.size();
	out << nCount;

	for (map<DWORD, DWORD>::iterator it	 = m_mapChestInfo.begin(); it != m_mapChestInfo.end(); ++it)
	{
		out << it->first << it->second;
	}

	return true;
}

// ��ʼ��
bool CActorPrizeManager::init(CPlayerBankPart *pBankPart, __IEntity *pMaster, int nLoginMode)
{
	if (pBankPart == NULL || pMaster == NULL)
	{
		return false;
	}

	m_pBankPart = pBankPart;
	m_pMaster = pMaster;

	if (nLoginMode == elogin_online)
	{
		m_pBankPart->readValueGoodsInfoFromDB(ENVALUEGOODSTYPE_CHEST);
	}

	return true;
}

void CActorPrizeManager::restore()
{

}

// �ͷ�
void CActorPrizeManager::release()
{

}


// �µ�һ��
void CActorPrizeManager::onNewDay()
{
    if(m_pMaster == nullptr) return;

    TASK_TRACELN(__FUNCTION__": pdbid="<< m_pMaster->getIntProperty(PROPERTY_ID) );
}

// �Ƿ���Ը���ɫ����
bool CActorPrizeManager::canPrizeActor(int nPrizeID, int nNum)
{
	if (nNum <= 0)
		return false;

	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return false;

	ISchemeActorPrize *pScheme = pSchemeCenter->getSchemeActorPrize();
	if (pScheme == NULL)
		return false;

	SActorPrizeConfig_S *pPrizeConfig = pScheme->getActorPrizeConfig(nPrizeID);
	if (pPrizeConfig == NULL)
	{
		ErrorLn(__FUNCTION__" prize no config " << nPrizeID);
		return false;
	}

	switch (pPrizeConfig->nPrizeType)
	{
	case ACTOR_PRIZE_ACTOREXP:
		break;
	case ACTOR_PRIZE_GOLD:
        {
            if (!m_pBankPart->getMoneyMgr()->canAddMoney(0, pPrizeConfig->nPrizeParam[0] * nNum))
                return false;
        }
		break;
	case ACTOR_PRIZE_HERO:
		{
			if (nNum > 1)
			{
				ErrorLn(__FUNCTION__" ACTOR_PRIZE_HERO nNum = " << nNum << " nPrizeID = " << nPrizeID);
				return false;
			}

			if (!m_pBankPart->canAddHero(pPrizeConfig->nPrizeParam[0]))
				return false;
		}
		break;
#ifdef FUNCTION_RUNE_OPEN
	case ACTOR_PRIZE_RUNE:
		{
			RUNE_BUY_ERROR_TYPE eErrorTyp = RBYEY_INVALID;
			if (!m_pBankPart->getRuneMgr()->canAddRuneStoreInfo(pPrizeConfig->nPrizeParam[0],eErrorTyp, pPrizeConfig->nPrizeParam[1] * nNum))
				return false;
		}
		break;
#endif
	case ACTOR_PRIZE_HEROCARD:
		{
			DWORD dwCardID = pPrizeConfig->nPrizeParam[0];
			int nCardCount = pPrizeConfig->nPrizeParam[1] * nNum;
			if (!m_pBankPart->canAddHeroCard(dwCardID, nCardCount))
				return false;
		}
		break;
	case ACTOR_PRIZE_HEROCARD_RANDOM:
		break;
	case ACTOR_PRIZE_TALENT:
		{
			if (nNum > 1)
			{
				ErrorLn(__FUNCTION__" ACTOR_PRIZE_TALENT nNum = " << nNum << " nPrizeID = " << nPrizeID);
				return false;
			}

			if (!m_pBankPart->canAddHeroTalentInfo(pPrizeConfig->nPrizeParam[0], pPrizeConfig->nPrizeParam[1]))
				return false;
		}
		break;
	case ACTOR_PRIZE_CHEST:
        {
            if ( !canAddChest(pPrizeConfig->nPrizeParam[0], pPrizeConfig->nPrizeParam[1] * nNum) )
                return false;
        }
		break;
	case ACTOR_PRIZE_SPECAIL:
		break;
	case ACTOR_PRIZE_BENEFITCARD:
		{
			// �ж��Ƿ����������濨
			if (!m_pBankPart->canAddBenefitCard(pPrizeConfig->nPrizeParam[0], nNum))
				return false;
		}
		break;
	case ACTOR_PRIZE_HEROSKIN:
        {
            int nHeroID = pPrizeConfig->nPrizeParam[0];
            int nSkinID = pPrizeConfig->nPrizeParam[1];
            int nDay = pPrizeConfig->nPrizeParam[2];
            DWORD dwDurationTime = nDay * 24 * 3600;

            if (!m_pBankPart->canAddHeroSkinInfo(nHeroID, nSkinID, dwDurationTime))
                return false;
        }
		break;
	case ACTOR_PRIZE_GEMSTONE:
		{
            int nGemStoneID = pPrizeConfig->nPrizeParam[0];
            BYTE byGrade = pPrizeConfig->nPrizeParam[1];

            if (!m_pBankPart->canAddCrownGemstone(nGemStoneID, byGrade, nNum))
                return false;
		}
		break;
	case ACTOR_PRIZE_HEROXPSKILL:
		{
			if (nNum > 1)
			{
				ErrorLn(__FUNCTION__" ACTOR_PRIZE_XP nNum = " << nNum << " nPrizeID = " << nPrizeID);
				return false;
			}

			int nHeroID = pPrizeConfig->nPrizeParam[0];
			int nXPSkillID = pPrizeConfig->nPrizeParam[1];
			if (!m_pBankPart->canAddHeroXPSkill(nHeroID, nXPSkillID))
				return false;
		}
		break;
	case  ACTOR_PRIZE_GEMSTONEPAGE:
		{	
			if(!m_pBankPart->canAddPage(nNum))
				return false;
		}
		break;
	case ACTOR_PRIZE_EMOJI:
		{
			if (nNum != 1)
				return false;

			int nEmojiType = pPrizeConfig->nPrizeParam[0];
			if (!m_pBankPart->canAddEmoji((BYTE)nEmojiType))
				return false;
		}
		break;
	case ACTOR_PRIZE_TASKSEARCH:
		break;
	default:
		ErrorLn(__FUNCTION__" unknown prize type " << nPrizeID);
		return false;
	}

	return true;
}

// ����ɫ����
bool CActorPrizeManager::prizeActor(int nPrizeID, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc, bool bBroadcast)
{
	if (nNum <= 0)
		return false;

	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return false;

	IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
	if (pOSSLogService == NULL)
		return false;

	ISchemeActorPrize *pScheme = pSchemeCenter->getSchemeActorPrize();
	if (pScheme == NULL)
		return false;

	SActorPrizeConfig_S *pPrizeConfig = pScheme->getActorPrizeConfig(nPrizeID);
	if (pPrizeConfig == NULL)
	{
		ErrorLn(__FUNCTION__" prize no config! prizeID=" << nPrizeID <<",name="<<m_pMaster->getName() );
		return false;
	}

	char szDesc[128] = {0};
	if (szLogDesc != NULL)
		ssprintf_s(szDesc, sizeof(szDesc), "%s", szLogDesc);
	else
		ssprintf_s(szDesc, sizeof(szDesc), "prizeActor");

	int nTime = (int)time(NULL);
	switch (pPrizeConfig->nPrizeType)
	{
	case ACTOR_PRIZE_ACTOREXP:          // �ٻ�ʦ����
		{
			// ����ʽ�Ӿ���ӿ�
			m_pBankPart->setActorBaseInfo(pPrizeConfig->nPrizeParam[0] * nNum, ENPLAYERINFOTYPE_EXP, ENCHANGEDATAMODE_ADD, OSS_NUMERIC_EXP, OSS_RESOURCE_OPT_ADD, wApproach, dwTargetID, szLogDesc, 0);
		}
		break;
	case ACTOR_PRIZE_GOLD:				// ���
		{
			// �̳ǡ������䲻���з�����˥��
			bool bAddictLimit = !(wApproach == OSSRES_ADD_POINTSHOP || wApproach == OSSRES_ADD_PRIZECHEST);
			if (((__IPlayerRole*)m_pMaster)->addMoney(0, pPrizeConfig->nPrizeParam[0] * nNum, wApproach, szDesc, 0, 0, bAddictLimit) == false)
				return false;
		}
		break;
	case ACTOR_PRIZE_HERO:              // Ӣ��
		{
			if (nNum > 1)
			{
				ErrorLn(__FUNCTION__" ACTOR_PRIZE_HERO invalid! Num=" << nNum << ",prizeID=" << nPrizeID <<",name="<<m_pMaster->getName() );
				return false;
			}

			if (!m_pBankPart->canAddHero(pPrizeConfig->nPrizeParam[0]))
				return false;

			if (!m_pBankPart->addNewHero(pPrizeConfig->nPrizeParam[0]))
				return false;

			m_pBankPart->sendActorDataTaskEvent(emTaskLuaEventType_AddHero, 0, 0, 0);
			//pOSSLogService->addNumericLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_PERSON, (DWORD)m_pMaster->getIntProperty(PROPERTY_ID), pPrizeConfig->nPrizeParam[0], 1, OSS_NUMERIC_HERO, OSS_RESOURCE_OPT_ADD, wApproach, nTime, szDesc);
			pOSSLogService->addActorGoodsLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_SYSTEM, 0, OSS_OPT_TYPE_PERSON, m_pMaster->getIntProperty(PROPERTY_ID), OSS_GOODS_HERO, OSS_RESOURCE_OPT_ADD, wApproach, pPrizeConfig->nPrizeParam[0], 1, nTime, szDesc);
		}
		break;

#ifdef FUNCTION_RUNE_OPEN
	case ACTOR_PRIZE_RUNE:              // ����
		{
			RUNE_BUY_ERROR_TYPE eErrorTyp = RBYEY_INVALID;
			if (!m_pBankPart->getRuneMgr()->canAddRuneStoreInfo(pPrizeConfig->nPrizeParam[0],eErrorTyp, pPrizeConfig->nPrizeParam[1] * nNum))
				return false;

			if (!m_pBankPart->getRuneMgr()->addRuneStoreInfo(pPrizeConfig->nPrizeParam[0], pPrizeConfig->nPrizeParam[1] * nNum))
				return false;
			pOSSLogService->addNumericLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_PERSON, (DWORD)m_pMaster->getIntProperty(PROPERTY_ID), pPrizeConfig->nPrizeParam[0], pPrizeConfig->nPrizeParam[1], OSS_NUMERIC_RUNE, OSS_RESOURCE_OPT_ADD, wApproach, nTime, szDesc);
		}
		break;
#endif // #ifdef FUNCTION_RUNE_OPEN

	case ACTOR_PRIZE_HEROCARD:          // Ӣ�ۿ���
		{
			DWORD dwCardID = pPrizeConfig->nPrizeParam[0];
			int nCardCount = pPrizeConfig->nPrizeParam[1] * nNum;
			if (!m_pBankPart->canAddHeroCard(dwCardID, nCardCount))
				return false;

			if (!m_pBankPart->addHeroCardInfo(&dwCardID, &nCardCount))
				return false;
			//pOSSLogService->addNumericLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_PERSON, (DWORD)m_pMaster->getIntProperty(PROPERTY_ID), (int)dwCardID, nCardCount, OSS_NUMERIC_HEROCARD, OSS_RESOURCE_OPT_ADD, wApproach, nTime, szDesc);
			pOSSLogService->addActorGoodsLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_SYSTEM, 0, OSS_OPT_TYPE_PERSON, m_pMaster->getIntProperty(PROPERTY_ID), OSS_GOODS_HEROCARD, OSS_RESOURCE_OPT_ADD, wApproach, dwCardID, nCardCount, nTime, szDesc);
		}
		break;
	case ACTOR_PRIZE_HEROCARD_RANDOM:   // ���Ӣ�ۿ���
		{
			int nNum = pPrizeConfig->nPrizeParam[0];
			DWORD cardIDList[GETCARD_MAX_NUM] = {0};
			int cardNumList[GETCARD_MAX_NUM] = {0};
			int nCount = m_pBankPart->getRandHeroCard(cardIDList, cardNumList, GETCARD_MAX_NUM, wApproach, nPrizeID);

			// �߻�Ҫ����忨�ƽ��Ҫ��ʾ
			if (nCount > 0)
			{
				sendRandomCardInfoToClient(nPrizeID, cardIDList, cardNumList, nCount);
			}
		}
		break;
	case ACTOR_PRIZE_TALENT:            // �츳
		{
			if (nNum > 1)
			{
				ErrorLn(__FUNCTION__" ACTOR_PRIZE_TALENT nNum = " << nNum << " nPrizeID = " << nPrizeID <<",name="<<m_pMaster->getName());
				return false;
			}

			if (!m_pBankPart->canAddHeroTalentInfo(pPrizeConfig->nPrizeParam[0], pPrizeConfig->nPrizeParam[1]))
				return false;

			if (!m_pBankPart->addHeroTalentInfo(pPrizeConfig->nPrizeParam[0], pPrizeConfig->nPrizeParam[1]))
				return false;
			pOSSLogService->addNumericLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_PERSON, (DWORD)m_pMaster->getIntProperty(PROPERTY_ID), pPrizeConfig->nPrizeParam[0], pPrizeConfig->nPrizeParam[1], OSS_NUMERIC_TALENT, OSS_RESOURCE_OPT_ADD, wApproach, nTime, szDesc);
		}
		break;
	case ACTOR_PRIZE_CHEST:             // ����
		{
			if (!addPrizeChest(pPrizeConfig->nPrizeParam[0], pPrizeConfig->nPrizeParam[1] * nNum, wApproach, dwTargetID, szDesc))
				return false;
		}
		break;
	case ACTOR_PRIZE_SPECAIL:            // ���⽱�� ���������,����ˢ��
		break;
	case ACTOR_PRIZE_BENEFITCARD:		// ���濨
		{
			if (!m_pBankPart->addBenefitCard(pPrizeConfig->nPrizeParam[0], nNum))
				return false;

			// ��¼�����־
			pOSSLogService->addNumericLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_PERSON, (DWORD)m_pMaster->getIntProperty(PROPERTY_ID), pPrizeConfig->nPrizeParam[0], nNum, OSS_NUMERIC_BENEFITCARD, OSS_RESOURCE_OPT_ADD, wApproach, nTime, szDesc);
		}
		break;
	case ACTOR_PRIZE_HEROXPSKILL:		// Ӣ��xp����
		{
			if (nNum > 1)
			{
				ErrorLn(__FUNCTION__" ACTOR_PRIZE_XP invalid! Num=" << nNum << ",prizeID=" << nPrizeID <<",name="<<m_pMaster->getName() );
				return false;
			}

			int nHeroID = pPrizeConfig->nPrizeParam[0];
			int nXPSkillID = pPrizeConfig->nPrizeParam[1];
			if (!m_pBankPart->canAddHeroXPSkill(nHeroID, nXPSkillID))
				return false;

			if (!m_pBankPart->addHeroXPSkill(nHeroID, nXPSkillID))
				return false;
			//pOSSLogService->addNumericLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_PERSON, (DWORD)m_pMaster->getIntProperty(PROPERTY_ID), nHeroID, 1, OSS_NUMERIC_HEROXPSKILL, OSS_RESOURCE_OPT_ADD, wApproach, nTime, szDesc);
			pOSSLogService->addActorGoodsLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_SYSTEM, 0, OSS_OPT_TYPE_PERSON, m_pMaster->getIntProperty(PROPERTY_ID), OSS_GOODS_HEROXPSKILL, OSS_RESOURCE_OPT_ADD, wApproach, nXPSkillID, 1, nTime, szDesc, nHeroID);
		}
		break;
	case ACTOR_PRIZE_HEROSKIN:			// Ӣ��Ƥ��
		{
			int nHeroID = pPrizeConfig->nPrizeParam[0];
			int nSkinID = pPrizeConfig->nPrizeParam[1];
			int nDay = pPrizeConfig->nPrizeParam[2];
			DWORD dwDurationTime = nDay*24*3600;

			if (!m_pBankPart->canAddHeroSkinInfo(nHeroID, nSkinID,dwDurationTime))
				return false;

			if (!m_pBankPart->addHeroSkinInfo(nHeroID, nSkinID,dwDurationTime))
				return false;

			m_pBankPart->sendActorDataTaskEvent(emTaskLuaEventType_AddSkin, 0, 0, 0);
			//pOSSLogService->addNumericLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_PERSON, (DWORD)m_pMaster->getIntProperty(PROPERTY_ID), nHeroID, 1, OSS_NUMERIC_HEROSKIN, OSS_RESOURCE_OPT_ADD, wApproach, nTime, szDesc);
			pOSSLogService->addActorGoodsLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_SYSTEM, 0, OSS_OPT_TYPE_PERSON, m_pMaster->getIntProperty(PROPERTY_ID), OSS_GOODS_SKIN, OSS_RESOURCE_OPT_ADD, wApproach, nSkinID, 1, nTime, szDesc, dwDurationTime, nHeroID);
		}
		break;
	case ACTOR_PRIZE_GEMSTONE:  // ˮ��
		{
			int nGemStoneID = pPrizeConfig->nPrizeParam[0];
			BYTE byGrade = pPrizeConfig->nPrizeParam[1];
			int nExp = pPrizeConfig->nPrizeParam[2];
			bool bAppraisal = (pPrizeConfig->nPrizeParam[3] == 1 ? true : false);

			if(!m_pBankPart->canAddCrownGemstone(nGemStoneID, byGrade, nNum))
				return false;

			if (!m_pBankPart->addCrownGemstone(nGemStoneID, nNum, byGrade, nExp, bAppraisal))
				return false;

			pOSSLogService->addActorGoodsLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_SYSTEM, 0, OSS_OPT_TYPE_PERSON, 
                m_pMaster->getIntProperty(PROPERTY_ID), OSS_GOODS_GEMSTONE, OSS_RESOURCE_OPT_ADD, wApproach, nGemStoneID, 1, nTime, szDesc);
		}
		break;
	case ACTOR_PRIZE_GEMSTONEPAGE:		// ˮ��ģ��
		{
			// ˮ��ģ�����ӽӿ�
			if(!m_pBankPart->canAddPage(nNum))
				return false;

			if (!m_pBankPart->createNewPage(nNum))
				return false;

			// TODO ��¼��־
		}
		break;
	case ACTOR_PRIZE_TASKSEARCH:
		{
			int nAddSeachNum = pPrizeConfig->nPrizeParam[0];
			if(!addTaskSearchNum(nAddSeachNum))
				return false;
		}
		break;
	case ACTOR_PRIZE_EMOJI:
		{
			BYTE byEmojiType = pPrizeConfig->nPrizeParam[0];
			if (!m_pBankPart->canAddEmoji(byEmojiType))
				return false;
			
			if (!m_pBankPart->addEmoji(byEmojiType))
				return false;
		}
		break;
	default:
		ErrorLn(__FUNCTION__" unknown prize type " << nPrizeID);
		return false;
	}

	if (bBroadcast && (pPrizeConfig->nBroadcastFlag > 0))
	{
		char szParam[64] = {0};
		ssprintf_s(szParam, sizeof(szParam), "%s;%d", m_pMaster->getName(), pPrizeConfig->nPrizeID);
		broadcastSystemMessageByFlag(CHAT_TIP_ACTORPRIZE_SINGLEPRIZEBROADCAST, pPrizeConfig->nBroadcastFlag, szParam, 20);
	}

	return true;
}

// ����ɫ������
bool CActorPrizeManager::groupPrizeActor(int nPrizeGroupID, int *pOutPrizeID, int &nOutPrizeCount, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc)
{
	int nCount = nOutPrizeCount;
	nOutPrizeCount = 0;
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return false;

	ISchemeActorPrize *pScheme = pSchemeCenter->getSchemeActorPrize();
	if (pScheme == NULL)
		return false;

	SActorPrizeGroupConfig *pGroupConfig = pScheme->getActorPrizeGroupConfig(nPrizeGroupID);
	if (pGroupConfig == NULL)
    {
        ErrorLn(__FUNCTION__": not found prize group, id="<<nPrizeGroupID<<", name="<<m_pMaster->getName());
		return false;
    }

	IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
	if (pOSSLogService == NULL)
		return false;

	int nOutIndex = 0;
	vector<int> vecPrizeResult;
	for (int i = 0; i < ACTORPRIZEGROUP_SINGLEGROUPMAX; ++i)
	{
		if (pGroupConfig->nConstantPrize[i] == 0)
			break;
		vecPrizeResult.push_back(pGroupConfig->nConstantPrize[i]);
	}

	if (pOutPrizeID != NULL && nCount < vecPrizeResult.size())
	{
		// ����������
		ErrorLn("Prize capacity not enough! config="<< vecPrizeResult.size()<<",real=" << nCount << ";id="<<nPrizeGroupID<<",name="<<m_pMaster->getName());
		return false;
	}

	if (pGroupConfig->nRandomPrizeCount > 0)
	{
		for (int i = 0; i < pGroupConfig->nRandomPrizeCount; ++i)
		{
			int nRand = mt_range_rand(0, 1000000 - 1);
			for (int j = 0; j < ACTORPRIZEGROUP_RANDOMPRIZEMAX; ++j)
			{
				if (nRand < pGroupConfig->nRandomPrizeRate[j])
				{
					// ����j
					int nCount = 0;
					for (int k = 0; k < ACTORPRIZEGROUP_SINGLEGROUPMAX; ++k)
					{
						if (pGroupConfig->nRandomPrizeID[j][k] == 0)
							break;
						++nCount;
					}

					if (nCount == 0)
					{
						ErrorLn(__FUNCTION__ " nRandomPrizeID empty " << nOutPrizeCount);
						return false;
					}

					int nTargetIndex = mt_range_rand(0, nCount - 1);
					vecPrizeResult.push_back(pGroupConfig->nRandomPrizeID[j][nTargetIndex]);
					break;
				}
			}
		}
	}

	if (pOutPrizeID != NULL)
	{
		nOutPrizeCount = (int)vecPrizeResult.size();
		if (nCount < nOutPrizeCount)
		{
			// ����������
			ErrorLn(__FUNCTION__" nOutPrizeCount not enough " << nCount);
			return false;
		}

		for (int i = 0; i < nOutPrizeCount; ++i)
		{
			pOutPrizeID[i] = vecPrizeResult[i];
		}
	}

	for (int i = 0; i < nOutPrizeCount; ++i)
	{
		int nPrizeID = pOutPrizeID[i];
		if (!prizeActor(nPrizeID, 1, wApproach, dwTargetID, szLogDesc, false))
		{
			// ���ܸ�ʱѰ���������
			SActorPrizeExchangeConfig *pExchangeConfig = pScheme->getPrizeExchangeConfig(nPrizeID);
			if (pExchangeConfig != NULL)
			{
				prizeActor(pExchangeConfig->nExchangePrizeID, 1, wApproach, dwTargetID, szLogDesc, false);
				pOutPrizeID[i] = pExchangeConfig->nExchangePrizeID;
			}
		}
	}

	if (pGroupConfig->nBroadcastFlag > 0)
	{
		vector<int> vecBroadcastPrizeID;
		for (int i = 0; i < nOutPrizeCount; ++i)
		{
			int nPrizeID = pOutPrizeID[i];
			SActorPrizeConfig_S *pPrizeConfig = pScheme->getActorPrizeConfig(nPrizeID);
			if (pPrizeConfig != NULL && pPrizeConfig->nGrade >= pGroupConfig->nBroadcastGrade)
			{
				vecBroadcastPrizeID.push_back(nPrizeID);
			}
		}

		if (vecBroadcastPrizeID.size() > 0)
		{
			string strParam = "";
			strParam += m_pMaster->getName();

			EMChatTipID nTipID = CHAT_TIP_ACTORPRIZE_PRIZEGROUPBROADCAST;
			if (wApproach == OSSRES_ADD_PRIZECHEST)
			{
				nTipID = CHAT_TIP_ACTORPRIZE_CHESTBROADCAST;
				strParam += ";";
				ISchemePrizeChest *pSchemeChest = pSchemeCenter->getSchemePrizeChest();
				if (pSchemeChest != NULL)
				{
					SPrizeChestConfig_S *pChestConfig = pSchemeChest->getPrizeChestConfig(dwTargetID);
					if (pChestConfig != NULL)
					{
						char szNum[16] = {0};
						ssprintf_s(szNum, sizeof(szNum), "%d", pChestConfig->nShowPrizeID);
						strParam += szNum;
					}
				}
			}

			strParam += ";";
			for (int i = 0; i < vecBroadcastPrizeID.size(); ++i)
			{
				int nPrizeID = vecBroadcastPrizeID[i];
				strParam += CHAT_MSG_ITEM_FLAG;
				strParam += "[prize id=";
				char szNum[16] = {0};
				ssprintf_s(szNum, sizeof(szNum), "%d", nPrizeID);
				strParam += szNum;
				strParam += "][/prize]";
			}

			//broadcastSystemMessageByFlag(nTipID, pGroupConfig->nBroadcastFlag, strParam.c_str(), 20);
			// ������Ĭ�Ϲ㲥ʱ�㲥   ������㲥�ȴ���Ϣ����
			if (nTipID == CHAT_TIP_ACTORPRIZE_PRIZEGROUPBROADCAST)
			{
				broadcastSystemMessageByFlag(nTipID, pGroupConfig->nBroadcastFlag, strParam.c_str());
			}
			else
			{
				m_nChestResultBroadcastFlag = pGroupConfig->nBroadcastFlag;
				m_strChestResult = strParam;
			}
		}
	}

	return true;
}

// �Ƿ�������ĳ������ָ�������ı���
bool CActorPrizeManager::canAddChest(int nChestType, int nNum)
{
	int nSelfID = m_pMaster->getIntProperty(PROPERTY_ID);
    if (nNum <= 0)
    {
        ErrorLn(__FUNCTION__": nNum = " << nNum << " actorID = " << nSelfID);
        return false;
    }
    if (nNum > 100000)
    {
        ErrorLn(__FUNCTION__": add num is too large! nNum=" << nNum << ",actorID=" << nSelfID);
        return false;
    }

    ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
        return false;

    ISchemePrizeChest *pSchemeChest = pSchemeCenter->getSchemePrizeChest();
    if (pSchemeChest == NULL)
        return false;

    SPrizeChestConfig_S *pChestConfig = pSchemeChest->getPrizeChestConfig(nChestType);
    if (pChestConfig == NULL)
    {
        ErrorLn(__FUNCTION__": pChestConfig == NULL, type = " << nChestType << " actorID = " << nSelfID);
        return false;
    }

	if (nChestType <= 0 || nChestType > PRIZECHEST_TYPE_MAX)
		return false;


    if (m_mapChestInfo[nChestType] + nNum >= INT_MAX)
    {
        ErrorLn(__FUNCTION__": add num is too large! nNum=" << nNum << ",haveNum=" << m_mapChestInfo[nChestType] << ",actorID=" << nSelfID);
        return false;
    }

    return true;
}

// ��ӱ���
bool CActorPrizeManager::addPrizeChest(int nChestType, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc)
{
    if (!canAddChest(nChestType, nNum))
        return false;

	if (nChestType <= 0 || nChestType > PRIZECHEST_TYPE_MAX)
		return false;

	m_mapChestInfo[nChestType] += nNum;

	m_pBankPart->saveValueGoodsInfoToDB(ENVALUEGOODSTYPE_CHEST, nChestType, m_mapChestInfo[nChestType]);

	updateChestInfoToClient();

	IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
	if (pOSSLogService != NULL)
	{
		pOSSLogService->addActorGoodsLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_SYSTEM, 0, OSS_OPT_TYPE_PERSON, m_pMaster->getIntProperty(PROPERTY_ID), OSS_GOODS_CHEST, OSS_RESOURCE_OPT_ADD, wApproach, nChestType, nNum, (int)time(NULL), szLogDesc);
	}

	return true;
}

// �򿪱���
bool CActorPrizeManager::onMessageOpenChest(void *data, size_t len)
{
	if (len < sizeof(msg_entity_open_chest))
	{
		ErrorLn(__FUNCTION__": message is invalid! len="<< len <<",pdbid="<< m_pMaster->getIntProperty(PROPERTY_ID));
		return false;
	}

	DWORD dwSelfID = m_pMaster->getIntProperty(PROPERTY_ID);
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
	if (pSchemeCenter == NULL || pOSSLogService == NULL)
		return false;

	ISchemePrizeChest *pSchemeChest = pSchemeCenter->getSchemePrizeChest();
	if (pSchemeChest == NULL)
		return false;

	msg_entity_open_chest *pMsg = (msg_entity_open_chest*)data;
	int nChestType = pMsg->nChestType;
	if (nChestType <= 0 || nChestType > PRIZECHEST_TYPE_MAX)
		return false;

	SPrizeChestConfig_S *pChestConfig = pSchemeChest->getPrizeChestConfig(nChestType);
	if (pChestConfig == NULL)
	{
		WarningLn(__FUNCTION__": pChestConfig == NULL! type=" << nChestType << ",pdbid="<< dwSelfID);
		return false;
	}

	if (m_mapChestInfo[nChestType] <= 0)
	{
		// ��ʾû�ж�Ӧ����
        WarningLn(__FUNCTION__": There is no chest! type=" << nChestType << ",pdbid="<< dwSelfID);
		return false;
	}

	if((pChestConfig->nChestFlag&EPCFT_GEMSTONE) == EPCFT_GEMSTONE)
	{
		m_pBankPart->onMessageOpenGemstoneChest(nChestType);
		return true;
	}

	--m_mapChestInfo[nChestType];

	char szLogInfo[256] = { 0 };
	ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("��%s"), pChestConfig->szChestName);

	int nPrizeIDList[10] = {0};
	int nListSize = 10;
	if (groupPrizeActor(pChestConfig->nPrizeGroupID, nPrizeIDList, nListSize, OSSRES_ADD_PRIZECHEST, nChestType, szLogInfo) == false)
	{
		ErrorLn(__FUNCTION__": groupPrizeActor == false!  type=" << nChestType << ",pdbid=" << dwSelfID);
		// ��������������ȥ
		++m_mapChestInfo[nChestType];
		return false;
	}

	// ֪ͨ����
	__IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
	if (pEntityEvent != NULL)
	{
		event_game_actor_data_task eventdata;
		eventdata.nTaskLuaEvent = emTaskLuaEventType_OpenChest;
		eventdata.dwPDBID = dwSelfID;
		eventdata.nParam1 = nChestType;

		pEntityEvent->FireExecute(EVENT_GAME_ACTOR_DATA_TASK, (LPCSTR)&eventdata, sizeof(eventdata));
	}

	// ����־
	pOSSLogService->addActorGoodsLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_PERSON, dwSelfID, OSS_OPT_TYPE_SYSTEM, 0,  OSS_GOODS_CHEST, OSS_RESOURCE_OPT_DEL, OSSRES_DEL_PRIZECHEST, nChestType, 1, (int)time(NULL), a2utf8("������"));
	
	// ����DB������Ϣ
	m_pBankPart->saveValueGoodsInfoToDB(ENVALUEGOODSTYPE_CHEST, nChestType, m_mapChestInfo[nChestType]);

	// ���¿ͻ��˱�����Ϣ
	updateChestInfoToClient();

	// ���Ϳͻ��˿�������
	msg_entity_open_chest_result resultInfo;
	resultInfo.nChestType = nChestType;
	for (int i = 0; i < PRIZEGROUP_RESULT_MAX; ++i)
	{
		resultInfo.nPrizeID[i] = nPrizeIDList[i];
	}

	obuf obmsg;
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
	head.byKeyModule   = MSG_MODULEID_ENTITY;
	head.byKeyAction   = SC_MSG_ENTITY_OPENCHEST_RESULT;
	obmsg << head;

	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_BANK;
	obmsg << entityHead;

	obmsg << resultInfo;

	__IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
	if (pPlayerRole != NULL)
	{
		pPlayerRole->send2Client(obmsg.data(), obmsg.size());
	}

	return true;
}

// �㲥��������
bool CActorPrizeManager::broadcastChestResult()
{
	if (m_nChestResultBroadcastFlag == DEFAULTBROADCASTFLAG)
		return false;

	// �㲥��������
	broadcastSystemMessageByFlag(CHAT_TIP_ACTORPRIZE_CHESTBROADCAST, m_nChestResultBroadcastFlag, m_strChestResult.c_str());
	
	// ���ò��ܹ㲥״̬
	m_nChestResultBroadcastFlag = DEFAULTBROADCASTFLAG;
	m_strChestResult.clear();

	return true;
}

// ���±������ݵ��ͻ���
void CActorPrizeManager::updateChestInfoToClient()
{
	obuf obData;
	SNetMsgHead head;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule    = MSG_MODULEID_ENTITY;
	head.byKeyAction    = SC_MSG_ENTITY_UPDATE_CHEST_INFO;
	obData << head;

	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_BANK;
	obData << entityHead;

	int nCount = m_mapChestInfo.size();
	obData << nCount;
	for (map<DWORD, DWORD>::iterator it = m_mapChestInfo.begin(); it != m_mapChestInfo.end(); ++it)
	{
		obData << it->first << it->second;
	}

	((__IPlayerRole*)m_pMaster)->send2Client(obData.data(), obData.size());
}

// DB��������Ϣ���
void CActorPrizeManager::onDBReturnReadChestInfo(vector<DBREQ_TABLE_VALUE_GOODS> * pvecGoods)
{
	if (pvecGoods == NULL)
	{
		return;
	}

	vector<DBREQ_TABLE_VALUE_GOODS> vecGoods = *pvecGoods;

	for (int i = 0; i < vecGoods.size(); ++i)
	{
		m_mapChestInfo[vecGoods[i].dwID] = vecGoods[i].dwCount;
	}

	updateChestInfoToClient();
}

// ���ݹ㲥��־����ʾ��Ϣ
void CActorPrizeManager::broadcastSystemMessageByFlag(EMChatTipID nTipID, int nBroadcastFlag, LPCSTR szPlusParam, int nDelayTime)
{
	// �㲥
	char szParam[1024] = {0};
	if (nBroadcastFlag & PRIZE_BROADCAST_WORLD)
	{
		ssprintf_s(szParam, sizeof(szParam), "%d;%s", CHAT_CHANNEL_WORLD, szPlusParam);
		g_EHelper.showSystemMessage(m_pMaster, nTipID, szParam, CHAT_BROADCASTTYPE_WORLD, nDelayTime);
	}

	if (nBroadcastFlag & PRIZE_BROADCAST_CLAN)
	{
		ssprintf_s(szParam, sizeof(szParam), "%d;%s", CHAT_CHANNEL_CLAN, szPlusParam);
		g_EHelper.showSystemMessage(m_pMaster, nTipID, szParam, CHAT_BROADCASTTYPE_CLAN, nDelayTime);
	}

	if (nBroadcastFlag & PRIZE_BROADCAST_KIN)
	{
		ssprintf_s(szParam, sizeof(szParam), "%d;%s", CHAT_CHANNEL_KIN, szPlusParam);
		g_EHelper.showSystemMessage(m_pMaster, nTipID, szParam, CHAT_BROADCASTTYPE_KIN, nDelayTime);
	}

	if (nBroadcastFlag & PRIZE_BROADCAST_NOTICE)
	{
		ssprintf_s(szParam, sizeof(szParam), "%d;%s", CHAT_CHANNEL_KIN, szPlusParam);
		g_EHelper.showSystemMessage(m_pMaster, nTipID, szParam, CHAT_BROADCASTTYPE_LOUDSPEAKER, nDelayTime);
	}
}

// ����������ƽ�����ͻ���
void CActorPrizeManager::sendRandomCardInfoToClient(int nPrizeID, DWORD* dwCardID, int* nValue, int nCount)
{
	obuf obData;
	SNetMsgHead head;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule    = MSG_MODULEID_ENTITY;
	head.byKeyAction    = SC_MSG_ENTITY_RETURN_CHEST_RANDCARD_INFO;
	obData << head;

	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_BANK;
	obData << entityHead << nPrizeID << nCount;

	for (int i = 0; i < nCount; ++i)
	{
		msg_entity_hero_card msgCard;
		msgCard.nHeroID = dwCardID[i];
		msgCard.nCount = nValue[i];
		obData.push_back( &msgCard, sizeof(msgCard));
	}

	((__IPlayerRole*)m_pMaster)->send2Client(obData.data(), obData.size());
}

// ����������������
bool CActorPrizeManager::addTaskSearchNum(int nNum)
{
	int nSceneID = m_pMaster->getSceneID();
	ILuaService * pLuaService = NULL;
	SceneLogicHelper helper(nSceneID);
	if (helper.m_ptr)
	{
		pLuaService = helper.m_ptr->getLuaService();
	}
	if (pLuaService == NULL)
	{
		return false;
	}

	CLuaParam params[2];
	params[0] = m_pMaster->getIntProperty(PROPERTY_ID);
	params[1] = nNum;
	pLuaService->RunLuaFunction("RequestAddTaskCatchNum", params, _countof(params));

	return true;
}