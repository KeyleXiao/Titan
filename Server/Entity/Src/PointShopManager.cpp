#include "stdafx.h"
#include "PlayerBankPart.h"
#include "PointShopManager.h"
#include "TaskStorageDef.h"
#include "IPlayerRole.h"
#include "IOSSLogService.h"
#include "OssLogDef.h"
#include "ExternalHelper.h"


// ��ȡ����ID
#define  SHOP_GET_SELL_ID(n)            ((n) >> 16);
// ��ȡ��������
#define  SHOP_GET_SELL_COUNT(n)         ((n) & 0xFFFF);
// ��Ϲ���ID������
#define  SHOP_PACK_SELL(id, count)      (((id)<<16) | (count)); 

// Ӣ��ID��ӦSLOT�������
#define  SHOP_HERO_SLOT_ARRAY_INDEX(h)  ((h) / 32)
// Ӣ��ID��ӦSLOT���
#define  SHOT_HERO_SLOT_BIT_INDEX(h)    ((h) % 32)


// ���캯��
CPointShopManager::CPointShopManager()
{
	m_pMaster = NULL;
	m_pBankPart = NULL;
}

// ��������
CPointShopManager::~CPointShopManager()
{
	
}

// ��ʼ��
bool CPointShopManager::init(CPlayerBankPart *pBankPart, __IEntity *pMaster)
{
	if (pBankPart == NULL || pMaster == NULL)
	{
		return false;
	}

	m_pBankPart = pBankPart;
	m_pMaster = pMaster;

	return true;
}

void CPointShopManager::initReflushCardInfo()
{
	if (m_pBankPart == NULL)
	{
		return;
	}

	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if (pTimerService == NULL)
	{
		return;
	}

	//����ˢ��ʱ�䣬ˢ�¿��� С��ˢ��ʱ�䣬��ˢ��ֻ����
	time_t nCurrentTime = time(NULL);
	time_t nLastReflushTime = m_pBankPart->getTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDLIST_LAST_REFLUSH_TIME, 0);
	if (nCurrentTime - nLastReflushTime >= REFLUSHCARDLISTINTERVAL)
	{
		// ˢ��ʱ�� = �ϴ�ˢ��ʱ�� + n * ˢ��ʱ����
		time_t nReflushTime = nLastReflushTime + ((nCurrentTime - nLastReflushTime) / REFLUSHCARDLISTINTERVAL) * REFLUSHCARDLISTINTERVAL;
		m_pBankPart->setTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDLIST_LAST_REFLUSH_TIME, nReflushTime);

		reflushPointShopCardList();
	}
	else
	{
		updatePointShopCardList();
	}

	// ���ÿ���ˢ�¶�ʱ��,��ͻ��˱���һ��
	DWORD dwLeftTime = REFLUSHCARDLISTINTERVAL - (nCurrentTime - nLastReflushTime) % REFLUSHCARDLISTINTERVAL;
	pTimerService->SetTimer(ETIMERID_REFLUSH_CARDLIST, dwLeftTime * 1000, this,  1);
}

void CPointShopManager::restore()
{
    ITimerService * pTimerService = gServerGlobal->getTimerService();
    if(pTimerService)
    {
        pTimerService->KillTimer(ETIMERID_REFLUSH_CARDLIST, this);
    }
}

void CPointShopManager::release()
{

}

void CPointShopManager::OnTimer( unsigned long dwTimerID )
{
	if ( m_pMaster == NULL )
	{
		return;
	}

	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if (pTimerService == NULL)
	{
		return;
	}

	switch( dwTimerID )
	{
	case ETIMERID_REFLUSH_CARDLIST:
		{
			m_pBankPart->setTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDLIST_LAST_REFLUSH_TIME, time(NULL));
			reflushPointShopCardList();
			
			pTimerService->KillTimer(ETIMERID_REFLUSH_CARDLIST, this);
			pTimerService->SetTimer(ETIMERID_REFLUSH_CARDLIST, REFLUSHCARDLISTINTERVAL * 1000, this,  1);
		}
		break;
	default:
		break;
	}
}


// �µ�һ��
void CPointShopManager::onNewDay()
{
    if(m_pMaster == nullptr) return;

    TASK_TRACELN(__FUNCTION__": pdbid="<< m_pMaster->getIntProperty(PROPERTY_ID) );

    // ˢ���̳ǿ����б�
    //reflushPointShopCardList();

	reflushBuyNumAndResetOptCount();
}

// �����̳���Ʒ
bool CPointShopManager::onMessagePointShopBuyGoods(void *data, size_t len)
{
    __IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
    if( pPlayerRole == NULL)
        return false;

	int nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

	if (len < sizeof(msg_entity_pointshop_buygoods))
	{
		ErrorLn(__FUNCTION__": message is invalid! len=" << len <<", pdbid="<< nPDBID);
		return false;
	}

	int nSucceed = 0;
	do
	{
		ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
		IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
		if (pSchemeCenter == NULL || pOSSLogService == NULL)
			break;

		ISchemePointShop *pSchemeShop = pSchemeCenter->getSchemePointShop();
		if (pSchemeShop == NULL)
			break;
        IPlayerMoneyManager * pMoneyMgr = m_pBankPart->getMoneyMgr();

		msg_entity_pointshop_buygoods sMsg = *(msg_entity_pointshop_buygoods*)data;
        int nSellID = sMsg.nSellID;
        if(nSellID < 0 || nSellID >= USHRT_MAX)
        {
            ErrorLn(__FUNCTION__": sell id of message is invalid !! nSellID=" << nSellID << ",pdbid=" << nPDBID);
            break;
        }
        if(sMsg.nNum <= 0 || sMsg.nNum >= USHRT_MAX)
        {
            ErrorLn(__FUNCTION__": sell amount of message is invalid !! num=" << sMsg.nNum << ",pdbid=" << nPDBID);
            break;
        }
		SPointShopGoodsConfig_S *pConfig = pSchemeShop->getPointShopGoodsConfig(nSellID);
		if (pConfig == NULL)
		{
			ErrorLn(__FUNCTION__": pConfig = NULL nSellID=" << nSellID << ",pdbid=" << nPDBID);
			break;
		}
		ISchemeActorPrize *pActorPrizeScheme = pSchemeCenter->getSchemeActorPrize();
		if (pActorPrizeScheme == NULL)
			break;
		SActorPrizeConfig_S *pPrizeConfig = pActorPrizeScheme->getActorPrizeConfig(pConfig->nPrizeID);
		if (pPrizeConfig == NULL)
			break;

		// �ж�VIP�ȼ�
		int nVipLevel = 0;  // TODO ��ȡ��ɫ��VIP�ȼ�
		if (nVipLevel < pConfig->nNeedVipLevel)
		{
			ErrorLn(__FUNCTION__": viplevel not enough nSellID=" << nSellID << ",pdbid=" << nPDBID);
			break;
		}

		// �жϽ�ɫ�ȼ�
		int nActorLevel = m_pMaster->getIntProperty(PROPERTY_LEVEL);
		if (nActorLevel < pConfig->nLevelLimitMin || nActorLevel > pConfig->nLevelLimitMax)
		{
			ErrorLn(__FUNCTION__": nActorLevel not enough level=" << nActorLevel << "nSellID=" << nSellID << "  actorID=" << nPDBID);
			break;
		}

		if (pSchemeShop->checkIfPointShopCardSellID(nSellID))
		{
			// �ж��Ƿ��ڳ��ۿ����б�
			bool bInCardList = false;
			for (int i = 0; i < POINTSHOP_CARDLIST_MAXCOUNT; ++i)
			{
				int nCardSellID = m_pBankPart->getTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDLIST_START + i);
				if (nCardSellID == 0)
					break;

				if (nCardSellID == nSellID)
				{
					bInCardList = true;
					break;
				}
			}

			if (!bInCardList)
			{
				ErrorLn(__FUNCTION__": !bInCardList nSellID=" << nSellID << "  actorID=" << nPDBID);
				break;
			}
		}

		// TODO �ж��Ƿ��ڿ���ʱ��

		// �ж��Ƿ�ɽ���
		if (!m_pBankPart->canPrizeActor(pConfig->nPrizeID, sMsg.nNum))
		{
			g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_POINTSHOP_BUYFAILED);
			break;
		}

		LONGLONG lRealPrice = 0;
		int nNewBoughtNum = 0;
		if (pConfig->nNumPriceID > 0)
		{
			// ��̬�۸�
			int nBoughtNum = getPointShopLimitGoodsNum(pConfig->nSellID);
            // ��ȡϵͳ���ÿɹ�������
            int nCanTotal = pSchemeShop->getLimitNumTotal(pConfig->nNumPriceID);
            // �����ٹ�����
            if(nBoughtNum >= nCanTotal)
            {
                break;
            }            
			nNewBoughtNum = nBoughtNum + sMsg.nNum;
            // �������������ʱ��Ҫ�Զ����ٿͻ�������Ĺ�������
            if( nNewBoughtNum > nCanTotal)
            {
                sMsg.nNum = nCanTotal - nBoughtNum;
                // ���¸���������
                nNewBoughtNum = nBoughtNum + sMsg.nNum;
            }

			if (nNewBoughtNum > 0xFFFF)
			{
				ErrorLn(__FUNCTION__": nBoughtNum=" << nBoughtNum << "  nNum=" << sMsg.nNum << "  nSellID=" << nSellID << "  actorID=" << nPDBID);
				break;
			}

			int nPrice = pSchemeShop->getPointShopNumLimitPrice(pConfig->nNumPriceID, sMsg.nResType, nBoughtNum, sMsg.nNum);
			if (nPrice <= 0)
			{
				ErrorLn(__FUNCTION__": nBoughtNum=" << nBoughtNum << "  nNum=" << sMsg.nNum << "  nSellID=" << nSellID << "  actorID=" << nPDBID);
				break;
			}

			lRealPrice = nPrice;
		}
		else
		{
			int nSinglePrice = 0;
			// �̶��۸�
			if (sMsg.nResType == ENPOINTSHOPRESTYPE_POINT)
				nSinglePrice = pConfig->nPointPrice;
			else
				nSinglePrice = pConfig->nMoneyPrice;

			if (nSinglePrice <= 0)
			{
				ErrorLn(__FUNCTION__": nPrice <= 0  nSellID=" << nSellID << "  actorID=" << nPDBID);
				break;
			}

			lRealPrice = (LONGLONG)nSinglePrice * sMsg.nNum;
		}

		if (lRealPrice < 0 || lRealPrice > INT_MAX)
		{
			ErrorLn(__FUNCTION__": lRealPrice > INT_MAX  nSellID=" << nSellID << "  nBuyNum=" << sMsg.nNum << "  actorID=" << nPDBID);
			break;
		}

		if (pConfig->nDiscountRate > 0)
		{
			// TODO �ж��Ƿ����ۿ��ڼ�

			lRealPrice = lRealPrice * pConfig->nDiscountRate / 10000;
		}

		if (lRealPrice <= 0 || lRealPrice >= INT_MAX)
		{
			ErrorLn(__FUNCTION__": lRealPrice=" << lRealPrice << "  nSellID=" << nSellID << "  nBuyNum=" << sMsg.nNum << "  actorID=" << nPDBID);
			break;
		}

		if (lRealPrice != sMsg.nClientPrice)
		{
			g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_POINTSHOP_PRICE_DISMATCH);
			break;
		}

		int nPrice = (int)lRealPrice;

		char szDesc[128] = {0};
		if (pPrizeConfig->nPrizeType == (int)ACTOR_PRIZE_HEROCARD)
			ssprintf_s(szDesc, sizeof(szDesc), a2utf8("�����̵깺��%sx%d"), pConfig->szSellName, sMsg.nNum);
		else
			ssprintf_s(szDesc, sizeof(szDesc), a2utf8("�̳ǹ���%sx%d"), pConfig->szSellName, sMsg.nNum);
		
        int nThisCostTicket = 0;
        int nThisCostGlod = 0;

        sMsg.nResType == ENPOINTSHOPRESTYPE_POINT ? nThisCostTicket=-nPrice : nThisCostGlod=-nPrice;

		if (!pMoneyMgr->canAddMoney(nThisCostTicket, nThisCostGlod))
		{
			g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_POINTSHOP_SERVERBUSY);
			break;
		}

        if (pConfig->nNumPriceID > 0 && nNewBoughtNum > 0 && nNewBoughtNum <= USHRT_MAX)
        {
            if (!setPointShopLimitGoodsNum(pConfig->nSellID, nNewBoughtNum))
            {
                g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_POINTSHOP_SERVERBUSY);
                break;
            }
        }

        // TODO
        if (!pMoneyMgr->addMoney(nThisCostTicket, nThisCostGlod, OSSRES_DEL_POINTSHOP, szDesc, pConfig->nSellID, sMsg.nNum, true))
        {
            g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_POINTSHOP_SERVERBUSY);
            break;
        }

		if (!m_pBankPart->prizeActor(pConfig->nPrizeID, sMsg.nNum, OSSRES_ADD_POINTSHOP, pConfig->nSellID, szDesc))
		{
			// ��ʾ����ʧ�ܣ�����ϵ�ͷ�����
			g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_POINTSHOP_PRIZEFAILED);
			ErrorLn(__FUNCTION__": prizeActor failed!!  nSellID=" << sMsg.nSellID << "  nResType=" << sMsg.nResType << "  nNum=" << sMsg.nNum << "  nPrice=" << nPrice << "  actorID=" << nPDBID);
			break;
		}

		nSucceed = 1;
		// Ʈ����ʾ
		g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_POINTSHOP_BUYSUCCEED);

		// ��¼��־���߻�Ҫ����ƷID�����Ƹ�Ϊ����ID�����ƣ�
		ActorHelper actorHelper(nPDBID, ACTOR_DBID);
		IActorService *pActor = actorHelper.m_ptr;
		if (pActor == NULL)
			break;

        // ����н�Һ͵�ȯһ������ʱ��Ҫ����ȯת���½��
        BYTE nMoneyType = MONEY_GOLD;
        if (sMsg.nResType == ENPOINTSHOPRESTYPE_POINT )
        {
            nMoneyType = MONEY_TICKET; 
        }

        // �̳ǹ�����Ʒ��Ҽ�¼����־�⣩
		DBREQ_OSS_MONITOR_ACTORTICKET_ITEM actorTicketLog;
		actorTicketLog.nActorID = nPDBID;																// ��ɫID
		actorTicketLog.wActorLevel = m_pMaster->getIntProperty(PROPERTY_LEVEL);                         // ��ɫ�ȼ�
		actorTicketLog.nUserID = pActor->getUserID();													// �û�ID
		actorTicketLog.nItemID = pConfig->nPrizeID;																// ����ID
		sstrcpyn(actorTicketLog.szGoodsName, pPrizeConfig->szPrizeName, sizeof(actorTicketLog.szGoodsName));	// ��������
		actorTicketLog.nItemNum = sMsg.nNum;															// ����
		actorTicketLog.nAgio = pConfig->nDiscountRate;													// �ۿ���
		actorTicketLog.nPrice = nPrice;																	// ����
		actorTicketLog.nCostTicket = nPrice;															// ���ѵ��
		actorTicketLog.byTicketType = nMoneyType;														// �������
		sstrcpyn(actorTicketLog.szRemark, szDesc, sizeof(actorTicketLog.szRemark));						// ��ע
		int nClientID = pActor->getClientID();
		IGatewayAcceptorService *pGatewayAcceptorService = gServerGlobal->getGatewayAcceptorService();
		if (pGatewayAcceptorService != NULL)
		{
			sstrcpyn(actorTicketLog.szIP, pGatewayAcceptorService->getIpAddress(nClientID).c_str(), sizeof(actorTicketLog.szIP));// IP��ַ
		}           
		actorTicketLog.byUserType = 0;																	// �û�����  1Ϊ���Ƽ��û� 
		actorTicketLog.nBuyTime = (int)time(NULL);														// ����ʱ��
		pOSSLogService->addPlazaTicketLog(pActor->getFromGameWorldID(), &actorTicketLog, sizeof(DBREQ_OSS_MONITOR_ACTORTICKET_ITEM));
	} while (false);

	// ֪ͨ�ͻ��˹�����
	msg_entity_pointshop_buyresult resultInfo;
	resultInfo.nSucceed = nSucceed;

	obuf obmsg;
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
	head.byKeyModule   = MSG_MODULEID_ENTITY;
	head.byKeyAction   = SC_MSG_ENTITY_POINTSHOP_BUYRESULT;
	obmsg << head;

	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_BANK;
	obmsg << entityHead;

	obmsg << resultInfo;

	pPlayerRole->send2Client(obmsg.data(), obmsg.size());

	return nSucceed > 0;
}

// �����̳��ѹ����������ݵ��ͻ���
void CPointShopManager::updatePointShopBuyNumToClient(int nSellID)
{
	obuf obData;
	SNetMsgHead head;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule    = MSG_MODULEID_ENTITY;
	head.byKeyAction    = SC_MSG_ENTITY_POINTSHOP_UPDATE_BOUGHTNUM;
	obData << head;

	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_BANK;
	obData << entityHead;

	if (nSellID > 0)
	{
		msg_entity_pointshop_update_boughtnum numInfo;
		numInfo.nCount = 1;
		obData << numInfo;

		msg_entity_pointshop_update_boughtnum_sub subInfo;
		subInfo.nSellID = nSellID;
		subInfo.nBoughtNum = getPointShopLimitGoodsNum(nSellID);
		obData << subInfo;
	}
	else
	{
		vector<msg_entity_pointshop_update_boughtnum_sub> tempVec;
		for (int i = TASKKEY_ACTOR_POINTSHOP_BUYNUM_START; i <= TASKKEY_ACTOR_POINTSHOP_BUYNUM_END; ++i)
		{
			int nData = m_pBankPart->getTaskDataInt(i);
			if (nData == TASKDATA_DEFAULT_VALUE)
				continue;

			msg_entity_pointshop_update_boughtnum_sub subInfo;
			subInfo.nSellID = SHOP_GET_SELL_ID(nData);
			subInfo.nBoughtNum = SHOP_GET_SELL_COUNT(nData);
			tempVec.push_back(subInfo);
		}

		msg_entity_pointshop_update_boughtnum numInfo;
		numInfo.nCount = (int)tempVec.size();
		obData << numInfo;

		vector<msg_entity_pointshop_update_boughtnum_sub>::iterator iter = tempVec.begin();
		vector<msg_entity_pointshop_update_boughtnum_sub>::iterator iterEnd = tempVec.end();
		for ( ; iter != iterEnd; ++iter)
		{
			obData << (*iter);
		}
	}

	((__IPlayerRole*)m_pMaster)->send2Client(obData.data(), obData.size());
}

// ˢ���̳��ѹ�������
void CPointShopManager::reflushPointShopBuyNum()
{
	obuf obData;
	SNetMsgHead head;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule    = MSG_MODULEID_ENTITY;
	head.byKeyAction    = SC_MSG_ENTITY_POINTSHOP_REFLUSH_BOUGHTNUM;
	obData << head;

	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_BANK;
	obData << entityHead;

	((__IPlayerRole*)m_pMaster)->send2Client(obData.data(), obData.size());
}

// ˢ���̳��ѹ��������������ֶ�ˢ�´���
void CPointShopManager::reflushBuyNumAndResetOptCount()
{
	// ˢ���̳��ѹ�������
	reflushPointShopBuyNum();

	// �����ֶ�ˢ�´���
	m_pBankPart->setTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDLIST_REFLUSH_NUM, REFLUSHCARDLISTMAXNUM);	
}

// �����̳ǿ����б���Ϣ���ͻ���
void CPointShopManager::updatePointShopCardList()
{
	obuf obData;
	SNetMsgHead head;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule    = MSG_MODULEID_ENTITY;
	head.byKeyAction    = SC_MSG_ENTITY_POINTSHOP_UPDATE_CARDLIST;
	obData << head;

	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_BANK;
	obData << entityHead;

	msg_entity_pointshop_update_cardlist cardInfo;

	// ���ÿ���ˢ�¶�ʱ��ʱ��,�����������һ��
	time_t nCurrentTime = time(NULL);
	time_t nLastReflushTime = m_pBankPart->getTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDLIST_LAST_REFLUSH_TIME, 0);
	cardInfo.nReflushLeftTime = REFLUSHCARDLISTINTERVAL - (nCurrentTime - nLastReflushTime) % REFLUSHCARDLISTINTERVAL;

	cardInfo.nReflushLeftNum = m_pBankPart->getTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDLIST_REFLUSH_NUM, REFLUSHCARDLISTMAXNUM);

	for (int i = 0; i < POINTSHOP_CARDLIST_MAXCOUNT; ++i)
	{
		cardInfo.nSellID[i] = m_pBankPart->getTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDLIST_START + i);
	}
	obData << cardInfo;

	((__IPlayerRole*)m_pMaster)->send2Client(obData.data(), obData.size());
}

enum EMCardlistRuleCombineFlag
{
	CardlistRule_HaveHero = 1 << 0,         // ��ӵ��Ӣ��
	CardlistRule_FreeHero = 1 << 1,         // ����Ӣ��
	CardlistRule_DisableHero = 1 << 2,      // ����ʹ��Ӣ��
};

// ˢ���̳ǿ����б�
void CPointShopManager::reflushPointShopCardList()
{
	// �⺯��ĿǰӦ��ͦ���ģ��������ò�Ƶ��(ÿ��ÿ��һ��)���Ժ���û�б�Ҫ�Ż���
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return;

	ISchemePointShop *pSchemeShop = pSchemeCenter->getSchemePointShop();
	if (pSchemeShop == NULL)
		return;

	ISchemeHeroFreeWeekly *pSchemeHeroFreeWeekly = pSchemeCenter->getSchemeHeroFreeWeekly();
	if ( pSchemeHeroFreeWeekly == NULL )
		return;

	// ����Ӣ��
	obuf obData;
	m_pBankPart->getHeroArray(obData);
	const char * pHeroData = obData.data();
	int * pCount = (int *)pHeroData;
	int nBankHeroCount = *pCount;
	SHeroInfo *pBankHeroInfo = (SHeroInfo *)(pCount + 1);

	//int nBankHeroCount = m_pBankPart->getHeroCount();
	//SHeroInfo *pBankHeroInfo = new SHeroInfo[nBankHeroCount];
	//nBankHeroCount = m_pBankPart->getHeroArray(pBankHeroInfo, nBankHeroCount);
	vector<int> haveHero;
	set<int> totalHero;
	for (int i = 0; i < nBankHeroCount; ++i, ++pBankHeroInfo)
	{
		if (pBankHeroInfo->nHeroID <= 0 || pBankHeroInfo->nHeroID > DEFAULTMAXHEROID)
		{
			continue;
		}

		if ((pBankHeroInfo->dwFlags & HERO_FLAG_NOT_HAVE) == 0)
		{
			haveHero.push_back(pBankHeroInfo->nHeroID);
			totalHero.insert(pBankHeroInfo->nHeroID);
		}
	}
	

	// ���Ӣ��
	int nPlayerLv = m_pMaster->getIntProperty(PROPERTY_LEVEL);
	const int nFreeHeroArrayLen = 2 * HEROFREEWEEKLY_FREEID;
	int arrayFreeHero[nFreeHeroArrayLen] = {0};
	int nFreeHeroCnt = pSchemeHeroFreeWeekly->GetFreeHeroListByLv(arrayFreeHero, nFreeHeroArrayLen, nPlayerLv);
	for (int i = 0; i < nFreeHeroCnt; ++i)
	{
		if (arrayFreeHero[i] <= 0 || arrayFreeHero[i] > DEFAULTMAXHEROID)
		{
			continue;
		}

		totalHero.insert(arrayFreeHero[i]);
	}

	// δӵ��Ӣ��
	ISchemePersonModel *pSchemeModel = pSchemeCenter->getSchemePersonModel();
	int nSchemeHeroCount = pSchemeModel->getHeroCount();
	int *pSchemeHeroIDList = new int[nSchemeHeroCount];
	pSchemeModel->getOpenHeroList(pSchemeHeroIDList, nSchemeHeroCount);
	set<int> disableHeroSet;
	for (int i = 0; i < nSchemeHeroCount; ++i)
	{
		if (pSchemeHeroIDList[i] <= 0 || pSchemeHeroIDList[i] > DEFAULTMAXHEROID)
		{
			continue;
		}

		if (totalHero.find(pSchemeHeroIDList[i]) == totalHero.end())
			disableHeroSet.insert(pSchemeHeroIDList[i]);
	}
	SAFE_DELETEARRAY(pSchemeHeroIDList);

	int nCurCanUseHeroNum = totalHero.size();
	SPointShopCardListRule *pConfig = pSchemeShop->getPointShopCardListRuleConfig(nCurCanUseHeroNum);
	if (pConfig == NULL)
	{
		ErrorLn(__FUNCTION__": pConfig = NULL nCurCanUseHeroNum=" << nCurCanUseHeroNum << "  actorID=" << m_pMaster->getIntProperty(PROPERTY_ID));
		return;
	}

	const int nHeroUseDataCount = 8;
	int nHeroUsedData[nHeroUseDataCount] = {0};
	int nHeroNewUsedData[nHeroUseDataCount] = {0};
	for (int i = 0; i < nHeroUseDataCount; ++i)
	{
		nHeroUsedData[i] = m_pBankPart->getTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDHERORECORD_START + i);
		nHeroNewUsedData[i] = nHeroUsedData[i];
	}

	int nLastResetTime = m_pBankPart->getTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDHERORECORD_CLEARTIME);
	time_t lTime = time(NULL);
	int nCurTime = (int)lTime;
    // �̵��������ʱ��
	if (nCurTime - nLastResetTime > pConfig->nResetUsedHeroDay * 24 * 60 * 60 || nCurTime - nLastResetTime < 0)
	{
		m_pBankPart->setTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDHERORECORD_CLEARTIME, nCurTime);
		memset(nHeroNewUsedData, 0, sizeof(nHeroNewUsedData));
	}

	bool bIsExDay = pSchemeShop->checkIfHoliday(lTime);
	int nTaskDataIndex = 0;
	vector<int> lastRoundUsedHero;
	for (int i = 0; i < POINTSHOP_CARDLISTRULE_MAXCOMBINECOUNT; ++i)
	{
        // �������(1��ӵ��Ӣ�ۣ�2����Ӣ�ۣ�4����ʹ��Ӣ��)
		int nCombineFlag = pConfig->nCombineFlag[i];
		if (nCombineFlag == 0)
			continue;

		set<int> heroSet;
        // ����Ӣ��
		if (nCombineFlag & CardlistRule_HaveHero)
		{
			heroSet.insert(haveHero.begin(), haveHero.end());
		}
        // ����Ӣ��
		if (nCombineFlag & CardlistRule_FreeHero)
		{
			for (int j = 0; j < nFreeHeroCnt; ++j)
			{
				heroSet.insert(arrayFreeHero[j]);
			}
		}
        // δӵ��Ӣ��
		if (nCombineFlag & CardlistRule_DisableHero)
		{
			heroSet.insert(disableHeroSet.begin(), disableHeroSet.end());
		}

		// �޳�����ѡ���Ӣ��
		vector<int>::iterator iterLastHero = lastRoundUsedHero.begin();
		vector<int>::iterator iterLastHeroEnd = lastRoundUsedHero.end();
		for ( ; iterLastHero != iterLastHeroEnd; ++iterLastHero)
		{
			heroSet.erase(*iterLastHero);
		}

		// �޳������ѹر�Ӣ��
		ISchemePersonModel * pPersonModel = gServerGlobal->getSchemeCenter()->getSchemePersonModel();
		if (pPersonModel == NULL)
		{
			ErrorLn(__FUNCTION__": pPersonModel == NULL");
			return;
		}
		vector<int> vecLockHeroID;
		for (set<int>::iterator iter = heroSet.begin(); iter != heroSet.end(); ++iter)
		{
			const PersonSkinModelItem * pPersonSkinModelItem = pPersonModel->getPersonSkinModelItem(*iter);
			if (pPersonSkinModelItem == NULL)
			{
				vecLockHeroID.push_back(*iter);
				WarningLn(__FUNCTION__": scheme PersonModel error! heroid = " << *iter);
			}
		}
		for (vector<int>::iterator it = vecLockHeroID.begin(); it != vecLockHeroID.end(); ++it)
		{
			set<int>::iterator itFind = heroSet.find(*it);
			heroSet.erase(itFind);
		}

		// ����ȫ���Ա����治��
		set<int> heroSetBak = heroSet;

        // �ų��������Ӣ��
		if (pConfig->nPassUsedHero[i])
		{
			set<int>::iterator iterHeroSet = heroSet.begin();
			set<int>::iterator iterHeroSetEnd = heroSet.end();
			for ( ; iterHeroSet != iterHeroSetEnd; )
			{
				int nHeroID = *iterHeroSet;
                int nMainIndex  = SHOP_HERO_SLOT_ARRAY_INDEX(nHeroID);
                int nOffset     = SHOT_HERO_SLOT_BIT_INDEX(nHeroID);
				if ((uint)(nHeroNewUsedData[nMainIndex] & (1 << nOffset)) > 0)
					iterHeroSet = heroSet.erase(iterHeroSet);
				else
					++iterHeroSet;
			}
		}

		int nLoopNum = (bIsExDay ? pConfig->nHeroNumEx[i] : pConfig->nHeroNum[i]);
        if(nLoopNum > 10) nLoopNum = 10;

		if (nLoopNum > heroSet.size())
		{
			// ʣ���ѡ�����������������
			set<int>::iterator iterSet = heroSet.begin();
			set<int>::iterator iterSetEnd = heroSet.end();
			for ( ; iterSet != iterSetEnd; ++iterSet)
			{
				int nHeroID = *iterSet;
				heroSetBak.erase(nHeroID);

                int nMainIndex  = SHOP_HERO_SLOT_ARRAY_INDEX(nHeroID);
                int nOffset     = SHOT_HERO_SLOT_BIT_INDEX(nHeroID);
				nHeroNewUsedData[nMainIndex] |= (1 << nOffset);

				int nSellID = pSchemeShop->getPointShopCardSellID(nHeroID);
				m_pBankPart->setTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDLIST_START + nTaskDataIndex, nSellID);
				++nTaskDataIndex;

				lastRoundUsedHero.push_back(nHeroID);
			}

			nLoopNum -= heroSet.size();
			heroSet = heroSetBak;
		}

		vector<int> randomHeroList;
		randomHeroList.reserve(heroSet.size());
		set<int>::iterator iterSet = heroSet.begin();
		set<int>::iterator iterSetEnd = heroSet.end();
		for ( ; iterSet != iterSetEnd; ++iterSet)
		{
			randomHeroList.push_back(*iterSet);
		}

		for (int j = 0; j < nLoopNum; ++j)
		{
			int nCurListSize = randomHeroList.size();
			if (nCurListSize == 0)
				break;

			int nRandIndex = mt_range_rand(0, nCurListSize - 1);
			int nHeroID = randomHeroList[nRandIndex];

			int nMainIndex  = SHOP_HERO_SLOT_ARRAY_INDEX(nHeroID);
			int nOffset     = SHOT_HERO_SLOT_BIT_INDEX(nHeroID);
			nHeroNewUsedData[nMainIndex] |= (1 << nOffset);

			int nSellID = pSchemeShop->getPointShopCardSellID(nHeroID);
			m_pBankPart->setTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDLIST_START + nTaskDataIndex, nSellID);
			++nTaskDataIndex;

			lastRoundUsedHero.push_back(nHeroID);

			randomHeroList[nRandIndex] = randomHeroList[nCurListSize - 1];
			randomHeroList.resize(nCurListSize - 1);
		}
	}

	for (int i = nTaskDataIndex; i < POINTSHOP_CARDLIST_MAXCOUNT; ++i)
	{
		m_pBankPart->setTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDLIST_START + i, 0);
	}

	for (int i = 0; i < nHeroUseDataCount; ++i)
	{
		if (nHeroUsedData[i] != nHeroNewUsedData[i])
		{
			m_pBankPart->setTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDHERORECORD_START + i, nHeroNewUsedData[i]);
		}
	}

	updatePointShopCardList();

	// ��շ������̳�������Ʒ�ѹ��������������ݣ���տͻ���������Ʒ�ѹ�������
	for (int i = TASKKEY_ACTOR_POINTSHOP_BUYNUM_START; i <= TASKKEY_ACTOR_POINTSHOP_BUYNUM_END; ++i)
	{
		m_pBankPart->setTaskDataIntEx(i, 0);
	}
	reflushPointShopBuyNum();
}

// ��ȡ�̳�������Ʒ�ѹ�������
int CPointShopManager::getPointShopLimitGoodsNum(WORD nSellID)
{
	for (int i = TASKKEY_ACTOR_POINTSHOP_BUYNUM_START; i <= TASKKEY_ACTOR_POINTSHOP_BUYNUM_END; ++i)
	{
		int nData = m_pBankPart->getTaskDataInt(i);
		if (nData == TASKDATA_DEFAULT_VALUE)
			continue;

		int nID = SHOP_GET_SELL_ID(nData);
		if (nSellID == nID)
		{
			return SHOP_GET_SELL_COUNT(nData);
		}
	}

	return 0;
}

// �����̳�������Ʒ�ѹ�������
bool CPointShopManager::setPointShopLimitGoodsNum(WORD nSellID, WORD nNum)
{
	// ������Ʒ�������ݲ�λ����
	int nTargetSlot = -1;
	for (int i = TASKKEY_ACTOR_POINTSHOP_BUYNUM_START; i <= TASKKEY_ACTOR_POINTSHOP_BUYNUM_END; ++i)
	{
		int nData = m_pBankPart->getTaskDataInt(i);
		if (nData == TASKDATA_DEFAULT_VALUE)
		{
			if (nTargetSlot < 0)
				nTargetSlot = i;
			continue;
		}

		int nID = SHOP_GET_SELL_ID(nData);
		if (nSellID == nID)
		{
			nTargetSlot = i;
			break;
		}
	}

	// ��Ӧ��λ������Ʒ�������ݸ���
	if (nTargetSlot < 0)
	{
		ErrorLn(__FUNCTION__": nTargetSlot < 0 , nSellID="<< nSellID <<",nNum="<<nNum<<",actorID=" << m_pMaster->getIntProperty(PROPERTY_ID));
		return false;
	}

	if (nNum > 0)
	{
		int nNewData = SHOP_PACK_SELL(nSellID, nNum);
		if (!m_pBankPart->addTaskDataInt(nTargetSlot, nNewData))
		{
			ErrorLn(__FUNCTION__": !addTaskDataInt  nSellID=" << nSellID);
			return false;
		}
	}
	else
	{
		if (!m_pBankPart->removeTaskDataInt(nTargetSlot))
		{
			ErrorLn(__FUNCTION__": !removeTaskDataInt  nSellID=" << nSellID);
			return false;
		}
	}

	updatePointShopBuyNumToClient(nSellID);
	return true;
}

// �ͻ�������ˢ�¿����б�
void CPointShopManager::onMessageReflushCardList()
{
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return;

	ISchemePointShop *pSchemeShop = pSchemeCenter->getSchemePointShop();
	if (pSchemeShop == NULL)
		return;

	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if (pTimerService == NULL)
		return;
	
	// �д���
	int nReflushNum = m_pBankPart->getTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDLIST_REFLUSH_NUM, REFLUSHCARDLISTMAXNUM);
	if (nReflushNum <= 0)
	{
		g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_POINTSHOP_NOTHAVE_REFLUSHNUM);
		return;
	}

	// ��Ǯ
	vector<SPointShopGoodsConfig_S*> listSellConfig;
	pSchemeShop->getPointShopSellConfigList(POINTSHOP_GOODSTYPE_SPECIAL, listSellConfig);
	if(listSellConfig.empty())
		return;

	SPointShopGoodsConfig_S* pReflushConfig = NULL;
	for (int i = 0; i <	listSellConfig.size(); ++i)
	{
		if (listSellConfig[i]->nConsumpApproach == (int)CONSUMP_APPROACH_MYSTERYSHOP_REFLUSH)
		{
			pReflushConfig = listSellConfig[i];
			break;
		}
	}
	if (pReflushConfig == NULL)
		return;

	msg_entity_pointshop_buygoods msgInfo;
	char szMoneyType[32]= {0};
	if(pReflushConfig->nPointPrice > 0)
	{
		msgInfo.nResType = (int)ENPOINTSHOPRESTYPE_POINT;
		msgInfo.nClientPrice = pReflushConfig->nPointPrice;
		sprintf(szMoneyType, "%s", a2utf8("��ȯ"));
	}
	else if(pReflushConfig->nMoneyPrice > 0)
	{
		msgInfo.nResType = (int)ENPOINTSHOPRESTYPE_MONEY;
		msgInfo.nClientPrice = pReflushConfig->nMoneyPrice;
		sprintf(szMoneyType, "%s", a2utf8("���"));
	}
	msgInfo.nSellID = pReflushConfig->nSellID;
	msgInfo.nNum = 1;
	if (msgInfo.nClientPrice <= 0)
	{
		return;
	}

	if(!onMessagePointShopBuyGoods((void*)&msgInfo, sizeof(msgInfo)))
	{
		return;
	}

	// ��¼������־
	IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
	if (NULL != pOSSLogService)
	{
		char szText[512] = {0};
		SPlayerInfo * playerInfo = m_pBankPart->getPlayerInfo();
		sprintf_s(szText, sizeof(szText), a2utf8("%s(%d)ˢ�������̵�,����%s%d"), &playerInfo->szActorName, playerInfo->dwActorID, szMoneyType, msgInfo.nClientPrice);
		pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_PERSON, playerInfo->dwActorID, szText);
	}

	// �۴���
	m_pBankPart->setTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDLIST_REFLUSH_NUM, --nReflushNum, REFLUSHCARDLISTMAXNUM);

	// ˢ�¿���
	m_pBankPart->setTaskDataIntEx(TASKKEY_ACTOR_POINTSHOP_CARDLIST_LAST_REFLUSH_TIME, time(NULL));
	reflushPointShopCardList();
	
	pTimerService->KillTimer(ETIMERID_REFLUSH_CARDLIST, this);
	pTimerService->SetTimer(ETIMERID_REFLUSH_CARDLIST, REFLUSHCARDLISTINTERVAL * 1000, this,  1);

	g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_POINTSHOP_REFLUSH_SUCCESS);
}