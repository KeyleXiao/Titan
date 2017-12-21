/*******************************************************************
** �ļ���:	CrownPageManager.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	12/1/2016
** ��  ��:	1.0
** ��  ��:	�ʹ�ϵͳ

********************************************************************/
#include "stdafx.h"
#include "PlayerBankPart.h"
#include "CrownPageManager.h"
#include "TaskStorageDef.h"
#include "IPlayerRole.h"
#include "IOSSLogService.h"
#include "OssLogDef.h"
#include "ExternalHelper.h"
#include "ChatHelper.h"

// ���캯��
CCrownPageManager::CCrownPageManager()
    : m_nPageCount(0)
    , m_nDBPageMaxIndex(0)
    , m_nCurrentCrownPage(0)
    , m_nDBGemstoneMaxIndex(0)
    , m_nGemstoneChestID(0)
    , m_pMaster(nullptr)
    , m_pBankPart(nullptr)
{
	memset(m_crownPageList,0,sizeof(m_crownPageList));
	memset(m_crownGemstoneList,0,sizeof(m_crownGemstoneList));
	memset(m_crownPropertyList,0,sizeof(m_crownPropertyList));
}

bool CCrownPageManager::onSerialize(rkt::ibuffer& in)
{
	in>>m_nCurrentCrownPage;
	int nPropertyCount = 0;

	in >> nPropertyCount;
	for (int i = 0; i < nPropertyCount; ++i)
	{
		if (!in.good())
			return false;

		int nPropertyID = 0;
		in >> nPropertyID;
		m_crownPropertyList[i] = nPropertyID;
	}

	return true;
}

bool CCrownPageManager::deSerialize(rkt::obuf& out)
{
	deSerializeCrownPropertyList(out);
	return true;
}

/** ����
*/
bool CCrownPageManager::onLoad( __IEntity* pEntity, CPlayerBankPart* pPlayerBank)
{
	if(pEntity == NULL || pPlayerBank==NULL)	
		return false;

	m_pMaster = pEntity;
	m_pBankPart = pPlayerBank;

	// ���ݿ�ӿ�ע��
    IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
    if (pDBEngineService) {
        pDBEngineService->registerHandler(this, __FUNCTION__);
    }

	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if(pTimerService) {
		pTimerService->SetTimer(ETIMERID_READ_CROWNDATA, READ_CROWNDATA_TIME, this,  1);
	}
    
	return true;
}

/** �ͷ�
*/
void CCrownPageManager::restore()
{
    ITimerService * pTimerService = gServerGlobal->getTimerService();
    if (pTimerService)
    {
        pTimerService->KillTimer(ETIMERID_READ_CROWNDATA, this);
    }

	// ���ݿ�ӿڷ�ע��
    IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
    if (pDBEngineService) {
        pDBEngineService->unregisterHandler(this);
    }
    
    m_nPageCount = 0;
    memset(m_crownPageList, 0, sizeof(m_crownPageList));
    memset(m_crownGemstoneList, 0, sizeof(m_crownGemstoneList));
    m_nCurrentCrownPage = 0;
    m_nDBGemstoneMaxIndex = 0;
    m_nDBPageMaxIndex = 0;
    m_nGemstoneChestID = 0;
    memset(m_crownPropertyList, 0, sizeof(m_crownPropertyList));
    m_pMaster = NULL;
    m_pBankPart = NULL;
}

void CCrownPageManager::release(void)
{

}

void CCrownPageManager::OnTimer( unsigned long dwTimerID )
{
	if ( m_pMaster == NULL )
	{
		return;
	}

	switch( dwTimerID )
	{
	case ETIMERID_READ_CROWNDATA:
		{
			readCrownPagesFromDatabase();
			readCrownGemstonesFromDatabase();
			readCrownGemstoneProductFromDatabase();
		}
		break;
	default:
		break;
	}
}
/////////////////////////////////////��ʯ�߼�/////////////////////////////////////
// �Ƿ�ʯ����
bool CCrownPageManager::isFullForGemsone(void)
{
    int nCount = getGemstoneCount();
    return nCount >= CROWN_GEMSTONE_MAX_COUNT;
}

// �ж����Ƿ�����ӻʹڱ�ʯ
bool CCrownPageManager::canAddCrownGemstone(int nGemstoneID, BYTE byGrade, int nCount)
{
    if (nGemstoneID <= CROWN_GAMESTONE_INVALID_ID)
    {
        return false;
    }
    if (nCount <= 0)
    {
        return false;
    }
    int nCurrentCount = getGemstoneCount();
    if (nCount + nCurrentCount > CROWN_GEMSTONE_MAX_COUNT)
    {
        return false;
    }

    // ���id�Ƿ���������
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        return false;
    }
    ISchemeCrownGemstoneInfo *pCrownInfoService = pSchemeCenter->getSchemeCrownGemstoneInfo();
    if ((pCrownInfoService == NULL))
    {
        ErrorLn(__FUNCTION__ << " Query CrownInfo Failed !!!");
        return  false;
    }
    SCrownGemstoneConfig_S *pCrownInfoScheme = pCrownInfoService->getCrownGemstoneConfig(nGemstoneID);
    if (pCrownInfoScheme == NULL)
    {
        ErrorLn(__FUNCTION__ << " nGemstoneID Is Not Find !!!" << nGemstoneID);
        return  false;
    }
    // ��ʯƷ�ʲ��ܳ������ýű��е�
    if (byGrade > pCrownInfoScheme->nGradeLimitUpper)
    {
        return false;
    }
    return true;
}

// ��ӻʹڱ�ʯ����
bool CCrownPageManager::addCrownGemstone(int nGemstoneID, int nCount, BYTE byGrade, int nExp, bool bAppraisal)
{
    for (int i = 0; i < nCount; ++i)
    {
        int nIndex = getGemstoneListEmptyIndex();
        // û��λ�þ�ֱ������
        if (nIndex == -1)
            break;

        GEMSTONE_ITEM* pGemstoneItem = &m_crownGemstoneList[nIndex];
        pGemstoneItem->nGemstoneDBID = ++m_nDBGemstoneMaxIndex;
        pGemstoneItem->nGemstoneID = nGemstoneID;
        pGemstoneItem->byGrade = (byGrade > 0 ? byGrade : 1);
        pGemstoneItem->nExp = nExp;
        pGemstoneItem->bAppraisal = bAppraisal;

        // ���±�ʯ���������Ժ���ֵ
        updateCrownGemstoneFaceScoreAndPropValue(pGemstoneItem);
        // ��ʯ�洢�����ݿ�
        saveCrownGemstoneToDatabase(pGemstoneItem);
        // ��ʯ����ͬ�����ͻ���
        sendCrownGemstoneToClient(pGemstoneItem);
    }
    sortCrownGemstoneList();

    return  true;
}

bool CCrownPageManager::canDeleteCrownGemstone(int nGemstoneDBID)
{
    if (nGemstoneDBID == CROWN_GAMESTONE_INVALID_ID)
        return false;

    for (int i = 0; i < CROWN_GEMSTONE_MAX_COUNT; ++i ) {
        if (m_crownGemstoneList[i].nGemstoneDBID == nGemstoneDBID)
            return true;
    }

    return false;
}

bool CCrownPageManager::deleteCrownGemstone(int nGemstoneDBID)
{
    GEMSTONE_ITEM* pGemstoneItem = getGemstonePossessItem(nGemstoneDBID);
    if (pGemstoneItem == NULL)
        return false;
    
    // �����ݿ����Ƴ������ʹڱ�ʯ 
    if (!deleteCrownGemstoneFromDatabase(nGemstoneDBID))
        return false;

    memset(pGemstoneItem, 0, sizeof(GEMSTONE_ITEM));
    sortCrownGemstoneList();
   
    GEMSTONE_ITEM deleteItem;
    deleteItem.nGemstoneDBID = nGemstoneDBID;
    // ͬ�����ͻ���
    sendCrownGemstoneToClient(&deleteItem);

    return true;
}

// �����ݿ����Ƴ������ʹڱ�ʯ
bool CCrownPageManager::deleteCrownGemstoneFromDatabase(int nGemstoneDBID)
{
    if (nGemstoneDBID <= CROWN_GAMESTONE_INVALID_ID) {
        return false;
    }
    if (m_pMaster == NULL ) {
        ErrorLn(__FUNCTION__": m_pMaster == NULL!");
        return false;
    }
    IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL) {
        ErrorLn(__FUNCTION__" : failed pDBEngine == NULL");
        return false;
    }

    DBREQ_DELETE_CROWN_GEMSTONE_POSSESS_ITEM dbParam;
    dbParam.dwActorID = (DWORD)m_pMaster->getIntProperty(PROPERTY_ID);
    dbParam.nGemstoneDBID = nGemstoneDBID;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_DELETE_CROWN_GMESTONEITEM, dbParam.dwActorID, (LPCSTR)&dbParam, sizeof(dbParam), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__" : pDBEngine->executeSP() GAMEDB_REQUEST_DELETE_CROWN_GMESTONEITEM failed");
        return false;
    }
    return true;
}


// ��ʯ����� 
void CCrownPageManager::onMessageOpenGemstoneChest(int nChestType)
{
	m_nGemstoneChestID = nChestType;
    // �����Ƿ���� ����:������0 
	if(m_gemstoneProduce.bGemstoneProduceEnd)
	{
		newGemstoneProduceDistribution();
	}

	sendGemstoneDistributionToClient();
}

// �ͻ���ѡ��ԭʯ����ʯ�齱��
bool CCrownPageManager::onMessageGemstoneSelectCobblestone(LPCSTR pszMsg, size_t nLen)
{
	if (pszMsg == NULL || nLen != sizeof(Msg_Cobbleston_Selected_Crown_Gemstone) )
	{
		ErrorLn(__FUNCTION__": message data is null or length is invalid! len="<< nLen <<", sizeof="<< sizeof(Msg_Cobbleston_Selected_Crown_Gemstone) );
		return false;
	}

	Msg_Cobbleston_Selected_Crown_Gemstone *pMsg = (Msg_Cobbleston_Selected_Crown_Gemstone *)pszMsg;
	if(pMsg == NULL)
		return false;

	if(!decreaseGemstoneChest())
	{
		return false;
	}
    int nGemstoneID = pMsg->nGemstoneID;

	if(!canAddCrownGemstone(nGemstoneID))
		return false;

	if(!addCrownGemstone(nGemstoneID))
		return false;

    // �����Ƿ���� ����:������0 
	m_gemstoneProduce.bGemstoneProduceEnd = true;

	saveGemstoneDistributionToDatabase();

	// ֪ͨ����
	__IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
	if (pEntityEvent != NULL)
	{
		event_game_actor_data_task eventdata;
		eventdata.nTaskLuaEvent = emTaskLuaEventType_OpenChest;
		eventdata.dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
		eventdata.nParam1 = nGemstoneID;

		pEntityEvent->FireExecute(EVENT_GAME_ACTOR_DATA_TASK, (LPCSTR)&eventdata, sizeof(eventdata));
	}

	return true;
}

// �ͻ��˻��ѵ�ȯ����
bool CCrownPageManager::onMessageGemstoneOpenWindow(LPCSTR pszMsg, size_t nLen)
{
	// ���id�Ƿ���������
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if ( pSchemeCenter == NULL )
	{
		return false;
	}

	ISchemePrizeChest *pSchemeChest = pSchemeCenter->getSchemePrizeChest();
	if (pSchemeChest == NULL)
		return false;

	SPrizeChestConfig_S *pChestConfig = pSchemeChest->getPrizeChestConfig((int)PRIZECHEST_TYPE_GEMSTONE);
	if (pChestConfig == NULL)
	{
		WarningLn(__FUNCTION__": pChestConfig == NULL!  ");
		return false;
	}

	ISchemeCrownGemstoneInfo *pCrownInfoService = pSchemeCenter->getSchemeCrownGemstoneInfo();
	if ((pCrownInfoService == NULL ))
	{
		ErrorLn(__FUNCTION__ << " Query CrownInfo Failed !!!");
		return false;
	}

	SCrownGemstoneRandomConfig_S* pSchemeRandomConfig = pCrownInfoService->getCrownGemstoneRandomConfig(pChestConfig->nPrizeGroupID);
	if (pSchemeRandomConfig == NULL)
	{
		WarningLn(__FUNCTION__": pSchemeRandomConfig == NULL! ");
		return false;
	}

	int nOpenWinCount = 0;
	for (int i = 0;i<DBDEF_PRODUCEGEMSTONECOUNTMAX;++i)
	{
		if(m_gemstoneProduce.windowGemstoneIDList[i]<=0)
			continue;

		nOpenWinCount++;
	}

	int nBaseTicket = pSchemeRandomConfig->nOpenWinCost;;
	int nAdditionTicket = pSchemeRandomConfig->nAdditionWinCost;
	int nCostTicket = nBaseTicket+nOpenWinCount*nAdditionTicket;

	if(!m_pBankPart->getMoneyMgr()->canAddMoney(0, -nCostTicket))
	{
		return false;
	}
    if( !m_pBankPart->getMoneyMgr()->addMoney(0, -nCostTicket, OSSRES_DEL_OPENWINFSTONE, utf82a("��ʯ����"), 0, 0, true) )
	{
		return false;
	}

	m_gemstoneProduce.bOpenWinEnd = false;
	sendGemstoneDistributionToClient();
	saveGemstoneDistributionToDatabase();

	return true;
}

// �ͻ��˿������ѡ��ʯ
bool CCrownPageManager::onMessageGemstoneWindowOpenToSelecet(LPCSTR pszMsg, size_t nLen)
{
	if (pszMsg == NULL || nLen != sizeof(Msg_WindowStone_Selected_Crown_Gemstone) )
	{
		ErrorLn(__FUNCTION__": message data is null or length is invalid! len="<< nLen <<", sizeof="<< sizeof(Msg_WindowStone_Selected_Crown_Gemstone) );
		return false;
	}

	Msg_WindowStone_Selected_Crown_Gemstone *pData = (Msg_WindowStone_Selected_Crown_Gemstone *)pszMsg;
	if(pData == NULL)
		return false;

	int nListIndex = pData->nSlotID;
	if(nListIndex>=0&&nListIndex<DBDEF_PRODUCEGEMSTONECOUNTMAX)
		m_gemstoneProduce.windowGemstoneIDList[nListIndex] = pData->nGemstoneID;

	m_gemstoneProduce.bOpenWinEnd = true;
	sendGemstoneDistributionToClient();
	saveGemstoneDistributionToDatabase();

	return true;
}

// �����µı�ʯ������Ϣ
void CCrownPageManager::newGemstoneProduceDistribution( )
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if ( pSchemeCenter == NULL ) {
		return;
	}
	ISchemePrizeChest *pSchemeChest = pSchemeCenter->getSchemePrizeChest();
    if (pSchemeChest == NULL) {
        return;
    }
	SPrizeChestConfig_S *pChestConfig = pSchemeChest->getPrizeChestConfig(m_nGemstoneChestID);
	if (pChestConfig == NULL) {
		WarningLn(__FUNCTION__": pChestConfig == NULL!  ");
		return;
	}
	ISchemeCrownGemstoneInfo *pCrownInfoService = pSchemeCenter->getSchemeCrownGemstoneInfo();
	if ( pCrownInfoService == NULL ) {
		ErrorLn(__FUNCTION__ << " Query CrownInfo Failed !!!");
		return;
	}
	SCrownGemstoneRandomConfig_S* pSchemeRandomConfig = pCrownInfoService->getCrownGemstoneRandomConfig(pChestConfig->nPrizeGroupID);
	if (pSchemeRandomConfig == NULL) {
		WarningLn(__FUNCTION__": pSchemeRandomConfig == NULL! ");
		return;
	}

	memset( m_gemstoneProduce.cobblestonGemstoneIDList, 0, sizeof(m_gemstoneProduce.cobblestonGemstoneIDList) );

	// �����ʯ
	int nRanCount = 0;
	for (int i=0;i<CROWN_GEMSTONE_RANDOMGROUP_MAX;++i)
	{
		if(nRanCount >= DBDEF_PRODUCEGEMSTONECOUNTMAX)
			break;

		SCrownGemstoneRandomItemConfig_S* pItemConfig = &pSchemeRandomConfig->gemStoneIDArray[i];
		if(pItemConfig == NULL||pItemConfig->nGemstoneCount<=0)
			continue;

		int nCanRanCount = pItemConfig->Size();
		if(nCanRanCount<=0)
			continue;

        int nRandGemstoneID = 0;
		int nSafeCount = 0;
		int nItemRanCount = pItemConfig->nGemstoneCount;
		while(nItemRanCount>0)
		{
			if(nSafeCount++>500)
				break;

			int nRandValue = mt_range_rand(0,nCanRanCount-1);			
			if(nRandValue<0||nRandValue>=CROWN_GEMSTONE_RANDOMID_MAX)
				continue;

            nRandGemstoneID = pItemConfig->Get(nRandValue);
			if(nRandGemstoneID<=0)
				continue;

			if(nRanCount >= DBDEF_PRODUCEGEMSTONECOUNTMAX)
				break;

			m_gemstoneProduce.cobblestonGemstoneIDList[nRanCount++] = nRandGemstoneID;
			nItemRanCount--;
		}
	}

	// ������������ı�ʯ�б�˳��λ��
	for (int i=0;i<(int)DBDEF_PRODUCEGEMSTONECOUNTMAX;++i)
	{
		int nRandSlot1 =  mt_range_rand(0,(int)DBDEF_PRODUCEGEMSTONECOUNTMAX-1);
		int nRandSlot2 =  mt_range_rand(0,(int)DBDEF_PRODUCEGEMSTONECOUNTMAX-1);

		int nTmpGemstoneID = m_gemstoneProduce.cobblestonGemstoneIDList[nRandSlot1];
		m_gemstoneProduce.cobblestonGemstoneIDList[nRandSlot1] = m_gemstoneProduce.cobblestonGemstoneIDList[nRandSlot2];
		m_gemstoneProduce.cobblestonGemstoneIDList[nRandSlot2] = nTmpGemstoneID;
	}

	// �����ʯ��ԭʯ����
	memset(m_gemstoneProduce.cobStoneIDList,0,sizeof(m_gemstoneProduce.cobStoneIDList));
	for (int i=0;i<DBDEF_PRODUCEGEMSTONECOUNTMAX;++i)
	{
		int nGemstoneID = m_gemstoneProduce.cobblestonGemstoneIDList[i];
		if(nGemstoneID<=0)
			continue;

		SCrownGemstoneConfig_S *pCrownInfoScheme= pCrownInfoService->getCrownGemstoneConfig(nGemstoneID);
		if(pCrownInfoScheme==NULL)
		{
			ErrorLn(__FUNCTION__ << " nGemstoneID Is Not Find !!!"<<nGemstoneID);
			continue;
		}

		int nCobstoneGroupID = pCrownInfoScheme->nCobGroupID;
		if(nCobstoneGroupID<=0)
			continue;

		SCrownCobstoneRandomConfig_S* pSchemeCobRandomConfig = pCrownInfoService->getCrownCobstoneRandomConfig(nCobstoneGroupID);
		if (pSchemeCobRandomConfig == NULL)
		{
			WarningLn(__FUNCTION__": pSchemeRandomConfig == NULL! ");
			continue ;
		}

		int nCanRanCount = 0;
		for (int j = 0;j<CROWN_GEMSTONE_RANDOMGROUP_MAX;++j)
		{
			if(pSchemeCobRandomConfig->cobstoneRandomArray[j].nCobstoneIDLower>0)
				nCanRanCount++;
		}

		if(nCanRanCount<=0)
			continue;

		int nSafeCount = 0;
		while(1)
		{
			if(nSafeCount++>500)
				break;

			int nRandValue = mt_range_rand(0,nCanRanCount-1);
			int nRandCobID = 0;
			if(nRandValue<0||nRandValue>=CROWN_GEMSTONE_RANDOMGROUP_MAX)
				continue;

			SCrownCobstoneRandomItemConfig_S* pItemConfig = &pSchemeCobRandomConfig->cobstoneRandomArray[nRandValue];

			int nRandValueEx = mt_range_rand(0,100);
			if(nRandValueEx>pItemConfig->nCobstoneIDRandomRate)
				continue;

			nRandCobID = mt_range_rand(pItemConfig->nCobstoneIDLower, pItemConfig->nCobstoneIDUpper);
			if(nRandCobID<=0)
				continue;

			m_gemstoneProduce.cobStoneIDList[i] = nRandCobID;
			break;
		}
	}
	
	m_gemstoneProduce.bOpenWinEnd = true;
	m_gemstoneProduce.bGemstoneProduceEnd = false;
	memset(m_gemstoneProduce.windowGemstoneIDList,0,sizeof(m_gemstoneProduce.windowGemstoneIDList));

	saveGemstoneDistributionToDatabase();
}

// ���±�ʯ�����ֲ�
void CCrownPageManager::saveGemstoneDistributionToDatabase()
{
    if (m_pMaster == NULL)
    {
        ErrorLn(__FUNCTION__": m_pMaster == NULL!");
        return;
    }
    IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__" : failed pDBEngine == NULL");
        return;
    }

    DBREQ_SAVE_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION dbParam;
    dbParam.dwActorID = (DWORD)m_pMaster->getIntProperty(PROPERTY_ID);
    dbParam.gemstoneProduce = m_gemstoneProduce;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION, dbParam.dwActorID, (LPCSTR)&dbParam, sizeof(dbParam), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__" : pDBEngine->executeSP() GAMEDB_REQUEST_UPDATE_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION failed");
    }
}

// ��ʯ�������
bool CCrownPageManager::decreaseGemstoneChest()
{
    if (m_pBankPart == NULL)
        return false;

    int nChestType = m_nGemstoneChestID;
    if (nChestType <= 0)
        return false;

    int nChestKey = TASKKEY_ACTOR_PRIZECHEST_NUM_START + nChestType - 1;
    int nCurHaveNum = m_pBankPart->getTaskDataIntEx(nChestKey);
    if (nCurHaveNum <= 0)
    {
        // ��ʾû�ж�Ӧ����
        ErrorLn(__FUNCTION__": There is no gemstonechest but had select gemstone! type=" << nChestType << ",pdbid=" << m_pMaster->getIntProperty(PROPERTY_ID));
        return false;
    }

    --nCurHaveNum;
    if (!m_pBankPart->setTaskDataIntEx(nChestKey, nCurHaveNum))
    {
        WarningLn(__FUNCTION__": chest setTaskDataIntEx == false, pdbid=" << m_pMaster->getIntProperty(PROPERTY_ID));
        return false;
    }

    IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
    if (pOSSLogService)
    {
        pOSSLogService->addNumericLog(m_pBankPart->getFromGameWorldID(), OSS_OPT_TYPE_PERSON, (DWORD)m_pMaster->getIntProperty(PROPERTY_ID), nChestType, 1, OSS_NUMERIC_CHEST, OSS_RESOURCE_OPT_DEL, OSSRES_DEL_PRIZECHEST, 0, a2utf8("������"));
    }

    // ͬ��������Ϣ���ͻ���
    m_pBankPart->updateChestInfoToClient();

    return true;
}

// ���±�ʯ��ֵ������
void CCrownPageManager::updateCrownGemstoneFaceScoreAndPropValue(GEMSTONE_ITEM* pItem)
{
    if (pItem == NULL) {
        return;
    }
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL) {
        return;
    }
	ISchemeCrownGemstoneInfo *pCrownInfoService = pSchemeCenter->getSchemeCrownGemstoneInfo();
	if (pCrownInfoService == NULL) {
		ErrorLn(__FUNCTION__ << " Query CrownInfo Failed !!!");
		return;
	}
	SCrownGemstoneConfig_S *pGemstoneCfg = pCrownInfoService->getCrownGemstoneConfig(pItem->nGemstoneID);
	if (pGemstoneCfg == NULL)
	{
		ErrorLn(__FUNCTION__ << " SCrownGemstoneConfig_S Query CrownInfo Failed !!!");
		return;
	}
    // ��������
    BYTE byGrade = pItem->byGrade;
    if (byGrade >= CGI_LEVEL_COUNT_MAX) {
        return;
    }

    // X��������
	vector<string> propVector;
	StringHelper::split(propVector, pGemstoneCfg->stPropertyIDArray[byGrade], ';', " ");
	for (vector<string>::iterator iterBegin = propVector.begin(); iterBegin!=propVector.end(); ++iterBegin)
	{
		int nPropID = atoi((*iterBegin).c_str());
		if(nPropID <= 0)
			continue;

		SCrownGemstoneEffectConfig_S *pGemstoneEffectCfg = pCrownInfoService->getCrownGemstoneEffectConfig(nPropID);
		if (pGemstoneEffectCfg == NULL)
			continue;
        // ��������
		pItem->nPropValue += pGemstoneEffectCfg->nPropertyValue;
	}

	SCrownGemstoneExteriorConfig_S *pGemstoneExteriorCfg = pCrownInfoService->getCrownCobstoneExteriorConfig(pGemstoneCfg->nExteriorIDArray[byGrade]);
	if (pGemstoneExteriorCfg != NULL)
	{
		pItem->nFaceScore = pGemstoneExteriorCfg->nFacescore;
	}
}

// ��ʯ����
bool CCrownPageManager::onMessageRequestSaleGemstone(LPCSTR pszMsg, size_t nLen)
{
	if (pszMsg == NULL || nLen != sizeof(Msg_Seil_Crown_Gemstone) )
	{
		ErrorLn(__FUNCTION__": message data is null or length is invalid! len="<< nLen <<", sizeof="<< sizeof(Msg_Seil_Crown_Gemstone) );
		return false;
	}
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if ( pSchemeCenter == NULL )
	{
		return false;
	}
	ISchemeCrownGemstoneInfo *pCrownInfoService = pSchemeCenter->getSchemeCrownGemstoneInfo();
	if ((pCrownInfoService == NULL ))
	{
		ErrorLn(__FUNCTION__ << " Query CrownInfo Failed !!!");
		return  false;
	}

	Msg_Seil_Crown_Gemstone *pData = (Msg_Seil_Crown_Gemstone *)pszMsg;
    if (pData == NULL)
    {
        return false;
    }
	int nGemstoneDBID = pData->nGemstoneDBID;

    GEMSTONE_ITEM* pGemstone = getGemstonePossessItem(nGemstoneDBID);
	if(pGemstone == NULL)
	{
        ErrorLn(__FUNCTION__ << " pGemstone == NULL !!!" << nGemstoneDBID)
		return false;
	}
    BYTE byGrade = pGemstone->byGrade;
    if (byGrade <= 0)
    {
        ErrorLn(__FUNCTION__ << " byGrade < = 0!" << nGemstoneDBID);
        return false;
    }

	SCrownGemstoneConfig_S *pCrownInfoScheme= pCrownInfoService->getCrownGemstoneConfig(pGemstone->nGemstoneID);
	if(pCrownInfoScheme==NULL)
	{
		ErrorLn(__FUNCTION__ << " nGemstoneID Is Not Find !!!"<<nGemstoneDBID);
		return  false;
	}

	vector<string> seilMoneyVector;
	StringHelper::split(seilMoneyVector, pCrownInfoScheme->stSeilMoneyArray, ';', " ");
    if (byGrade > seilMoneyVector.size())
    {
        ErrorLn(__FUNCTION__ << " byGrade > seilMoneyVector.size()! byGrade=" << byGrade << ", size=" << seilMoneyVector.size());
        return false;
    }

    int nSeilMoney = atoi(seilMoneyVector[byGrade - 1].c_str());

	if(!canDeleteCrownGemstone(nGemstoneDBID))
    {
        ErrorLn(__FUNCTION__ << " canDeleteCrownGemstone is false! byGrade=" << byGrade << ", nGemstoneDBID=" << nGemstoneDBID);
        return false;
    }
	if(!deleteCrownGemstone(nGemstoneDBID))
    {
        ErrorLn(__FUNCTION__ << " deleteCrownGemstone is false! byGrade=" << byGrade << ", nGemstoneDBID=" << nGemstoneDBID);
        return false;
    }

	// ������
	char szLogDesc[256] = {0};
	ssprintf_s(szLogDesc, sizeof(szLogDesc), a2utf8("���۱�ʯ[��ʯID=%d]��ý��=%d"), pCrownInfoScheme->nGemstoneID, nSeilMoney);
	if(!m_pBankPart->getMoneyMgr()->canAddMoney(0, nSeilMoney))
    {
        ErrorLn(__FUNCTION__ << " canAddMoney is false! nSeilMoney=" << nSeilMoney << ", nGemstoneDBID=" << nGemstoneDBID);
        return false;
	}
	if(!m_pBankPart->getMoneyMgr()->addMoney(0, nSeilMoney, OSSRES_ADD_SEILSTONE, szLogDesc, 0, 0, true))
    {
        ErrorLn(__FUNCTION__ << " addMoney is false! nSeilMoney=" << nSeilMoney << ", nGemstoneDBID=" << nGemstoneDBID);
        return false;
    }

	char szParam[64] = {0};
	ssprintf_s(szParam, sizeof(szParam), "%s;%d",   pCrownInfoScheme->chGemstoneName,nSeilMoney);
    g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_CROWNS_GEMSTONESEILSUCCESS,szParam,CHAT_BROADCASTTYPE_SINGLE);
	return true;
}

// ��ʯ����
bool CCrownPageManager::onMessageRequestAppraisalGemstone(LPCSTR pszMsg, size_t nLen)
{
	bool bAppraisalSucess = doCrownAppraisalGemstone(pszMsg,nLen);
	if(!bAppraisalSucess)
	{
		sendGemstoneAppraisalRetToClient(0, 0, false);
		g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_CROWNS_GEMSTONEAPPRAISA_FAIL);	
	}

	// д������־
	//writeGemstoneAppraisalLog(pszMsg,nLen,bAppraisalSucess);
	return bAppraisalSucess;
}

bool CCrownPageManager::doCrownAppraisalGemstone( LPCSTR pszMsg, size_t nLen )
{
    if (m_pBankPart == NULL)
    {
        return false;
    }
	if (pszMsg == NULL || nLen != sizeof(Msg_Appraisal_Crown_Gemstone) )
	{
		ErrorLn(__FUNCTION__": message data is null or length is invalid! len="<< nLen <<", sizeof="<< sizeof(Msg_Appraisal_Crown_Gemstone) );
		return false;
	}
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        return false;
    }
    ISchemeCrownGemstoneInfo *pCrownInfoService = pSchemeCenter->getSchemeCrownGemstoneInfo();
    if ((pCrownInfoService == NULL))
    {
        ErrorLn(__FUNCTION__ << " Query CrownInfo Failed !!!");
        return  false;
    }

	Msg_Appraisal_Crown_Gemstone *pData = (Msg_Appraisal_Crown_Gemstone *)pszMsg;

    // ����ʯ
    int nMainGemstoneID = pData->gemstoneDBIDList[(int)EAGT_MAINGENSTONE];
    // ������ʯ
	int nAssistGemstoneID = pData->gemstoneDBIDList[(int)EAGT_ASSISTANTGENSTONE];

	int nCostMoney = 0;
	int nCostTicket = 0;

	if(!canAppraisalCrownGemstone(pCrownInfoService, nMainGemstoneID, nAssistGemstoneID, nCostMoney, nCostTicket))
		return false;

	if(!canDeleteCrownGemstone(nAssistGemstoneID))
		return false;

	if(!deleteCrownGemstone(nAssistGemstoneID))
		return false;

	char szTicketLogDesc[256] = {0};
	ssprintf_s(szTicketLogDesc, sizeof(szTicketLogDesc), "������ʯ[��ʯDBID=%d]���Ľ��=%d�����ĵ��=%d", nMainGemstoneID, nCostMoney, nCostTicket);
	if (nCostMoney < 0 || nCostTicket < 0 || !m_pBankPart->getMoneyMgr()->addMoney(-nCostTicket, -nCostMoney, OSSRES_DEL_OPENWINFSTONE, a2utf8(szTicketLogDesc), 0, 0, true))
		return false;

	if(!appraisalCrownGemstone(pCrownInfoService, nMainGemstoneID))
		return false;

	// ������־
	IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
	if (NULL != pOSSLogService)
	{
		char szText[512] = {0};
		SPlayerInfo * playerInfo = m_pBankPart->getPlayerInfo();
		sprintf_s(szText, sizeof(szText), a2utf8("%s(%d)ʹ�ú˾�����,���˾�ID%d,���˾�ID%d,���Ľ��%d,���ĵ�ȯ%d"), &playerInfo->szActorName, playerInfo->dwActorID, nMainGemstoneID, nAssistGemstoneID, nCostMoney, nCostTicket);
		pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_PERSON, playerInfo->dwActorID, szText);
	}

	return true;
}


bool CCrownPageManager::canAppraisalCrownGemstone(ISchemeCrownGemstoneInfo *pCrownInfoService, int nMainStoneDBID, int nAssistantStoneDBID, int& nCostMoney, int& nCostTicket)
{
    if (nMainStoneDBID <= 0 || nAssistantStoneDBID <= 0 || nMainStoneDBID == nAssistantStoneDBID) {
        ErrorLn(__FUNCTION__ << "() stone is invalid ! nMainStoneDBID="<< nMainStoneDBID <<",nAssistantStoneDBID="<< nAssistantStoneDBID);
		return false;
    }

	GEMSTONE_ITEM* pMainGemstone = getGemstonePossessItem(nMainStoneDBID);
    if (pMainGemstone == NULL)
    {
        ErrorLn(__FUNCTION__ << "() pMainGemstone == NUL ! nMainStoneDBID="<< nMainStoneDBID);
        return false;
    }
	GEMSTONE_ITEM* pAssistantGemstone = getGemstonePossessItem(nAssistantStoneDBID);
    if (pAssistantGemstone == NULL)
    {
        ErrorLn(__FUNCTION__ << "() pAssistantGemstone == NUL ! nAssistantStoneDBID=" << nAssistantStoneDBID);
        return false;
    }
    // �¾ɱ�ʯ
	if(pMainGemstone->byGrade != (int)EGGT_OBSOLETEGRADE)
		return false;

	SCrownGemstoneConfig_S *pMainStoneCfg = pCrownInfoService->getCrownGemstoneConfig(pMainGemstone->nGemstoneID);
    if (pMainStoneCfg == NULL)
    {
        ErrorLn(__FUNCTION__ << "() pMainStoneCfg == NUL ! nGemstoneID=" << pMainGemstone->nGemstoneID);
        return false;
    }
	SCrownGemstoneConfig_S *pAssistantStoneCfg = pCrownInfoService->getCrownGemstoneConfig(pAssistantGemstone->nGemstoneID);
    if (pAssistantStoneCfg == NULL)
    {
        ErrorLn(__FUNCTION__ << "() pAssistantStoneCfg == NUL ! nGemstoneID=" << pAssistantGemstone->nGemstoneID);
        return false;
    }

    if (pAssistantStoneCfg->nGradeLimitUpper > pMainStoneCfg->nGradeLimitUpper)
    {
        ErrorLn(__FUNCTION__ << "() Assistant.MaxGrade > Main.MaxGrade! Assistant.MaxGrade=" << pAssistantStoneCfg->nGradeLimitUpper <<",Main.MaxGrade="<< pMainStoneCfg->nGradeLimitUpper);
        return false;
    }
    if (pMainStoneCfg->nAppraisalProcessMax <= 0)
    {
        ErrorLn(__FUNCTION__ << "() nAppraisalProcessMax is invalid! nAppraisalProcessMax=" << pMainStoneCfg->nAppraisalProcessMax);
        return false;
    }
    if (pMainStoneCfg->nAppraisalCostMoney < 0 || pMainStoneCfg->nAppraisalCostTicket < 0)
    {
        ErrorLn(__FUNCTION__ << "() nAppraisalCostMoney or nAppraisalCostTicket is invalid! CostMoney=" << pMainStoneCfg->nAppraisalCostMoney<<",CostTicket="<< pMainStoneCfg->nAppraisalCostTicket);
        return false;
    }

	nCostMoney = (int)((float)pMainStoneCfg->nAppraisalProcess / (float)pMainStoneCfg->nAppraisalProcessMax * (float)pMainStoneCfg->nAppraisalCostMoney);
	nCostTicket = (int)((float)pMainStoneCfg->nAppraisalProcess / (float)pMainStoneCfg->nAppraisalProcessMax * (float)pMainStoneCfg->nAppraisalCostTicket);

    if (!m_pBankPart->getMoneyMgr()->canAddMoney(-nCostTicket, -nCostMoney))
    {
        ErrorLn(__FUNCTION__ << "() canAddMoney fail ! nCostTicket=" << nCostTicket <<",nCostMoney="<< nCostMoney);
        return false;
    }

	return true;
}

bool CCrownPageManager::appraisalCrownGemstone(ISchemeCrownGemstoneInfo *pCrownInfoService, int nMainStoneDBID)
{
    GEMSTONE_ITEM* pMainGemstoneItem = getGemstonePossessItem(nMainStoneDBID);
    if (pMainGemstoneItem == NULL)
    {
        ErrorLn(__FUNCTION__ << " pMainGemstoneItem == NULL! nMainStoneDBID=" << nMainStoneDBID);
        return false;
    };
    SCrownGemstoneConfig_S *pCrownInfoScheme = pCrownInfoService->getCrownGemstoneConfig(pMainGemstoneItem->nGemstoneID);
    if (pCrownInfoScheme == NULL)
    {
        ErrorLn(__FUNCTION__ << " pCrownInfoScheme==NULL! nGemstoneID=" << pMainGemstoneItem->nGemstoneID);
        return false;
    }

    int nCritNum = 1;
    int nRandValue = mt_range_rand(0, 10000);
    int nTmpCritRate = 0;
    bool bAppraisalRet = false;
    for (int i = 0; i < CROWN_GEMSTONE_APPRAISALCRIT_MAX; ++i)
    {
        nTmpCritRate += pCrownInfoScheme->appraisalCritArray[i].nCritRate * BASE_IN_MULTIPLE;
        if (nRandValue <= nTmpCritRate)
        {
            nCritNum = pCrownInfoScheme->appraisalCritArray[i].nCritNum;
            break;
        }
    }

    int addProcessNum = pCrownInfoScheme->nAppraisalProcess * nCritNum;
    pMainGemstoneItem->nAppraisalCount += addProcessNum;

    if (pMainGemstoneItem->nAppraisalCount >= pCrownInfoScheme->nAppraisalProcessMax)
    {
        pMainGemstoneItem->byGrade = pCrownInfoScheme->nGradeLimitUpper;
        pMainGemstoneItem->nAppraisalCount = 0;
        pMainGemstoneItem->nExp = 0;
        bAppraisalRet = true;
    }

    saveCrownGemstoneToDatabase(pMainGemstoneItem);

    sendCrownGemstoneToClient(pMainGemstoneItem);

    sendGemstoneAppraisalRetToClient(nMainStoneDBID, nCritNum, true);

    if (bAppraisalRet)
    {
        broadCastGemstoneLevelUpInChatChannel(pMainGemstoneItem->byGrade);
        g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_CROWNS_GEMSTONEAPPRAISA_SUCESS);
    }
    else
    {
        char szParam[256] = { 0 };
        if (nCritNum > 1)
        {
            ssprintf_s(szParam, sizeof(szParam), "%d;%d", nCritNum, addProcessNum);
            g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_CROWNS_GEMSTONEAPPRAISA_CRITPROCESS, szParam);
        }
        else
        {
            ssprintf_s(szParam, sizeof(szParam), "%d", addProcessNum);
            g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_CROWNS_GEMSTONEAPPRAISA_PROCESS, szParam);
        }
    }

    return true;
}

// ������־
void CCrownPageManager::writeGemstoneAppraisalLog(LPCSTR pszMsg, size_t nLen, bool bAppraisalSucess)
{
    if (pszMsg == NULL || nLen != sizeof(Msg_Appraisal_Crown_Gemstone))
    {
        ErrorLn(__FUNCTION__": message data is null or length is invalid! len=" << nLen << ", sizeof=" << sizeof(Msg_Appraisal_Crown_Gemstone));
        return;
    }
    Msg_Appraisal_Crown_Gemstone *pData = (Msg_Appraisal_Crown_Gemstone *)pszMsg;

    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        return;
    }
    ISchemeCrownGemstoneInfo *pCrownInfoService = pSchemeCenter->getSchemeCrownGemstoneInfo();
    if ((pCrownInfoService == NULL))
    {
        ErrorLn(__FUNCTION__ << " pCrownInfoService == NULL!");
        return;
    }
    GEMSTONE_ITEM* pMainGemstoneItem = getGemstonePossessItem(pData->gemstoneDBIDList[(int)EAGT_MAINGENSTONE]);
    if (pMainGemstoneItem == NULL)
    {
        ErrorLn(__FUNCTION__ << " pMainGemstoneItem == NULL!");
        return;
    }

    int nGemstoneID = pMainGemstoneItem->nGemstoneID;
    SCrownGemstoneConfig_S *pCrownInfoScheme = pCrownInfoService->getCrownGemstoneConfig(nGemstoneID);
    if (pCrownInfoScheme == NULL)
    {
        ErrorLn(__FUNCTION__ << " nGemstoneID Is Not Find !!!" << nGemstoneID);
        return;
    }

    IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
    if (pOSSLogService)
    {
        char szForgeDesc[DBDEF_OSS_ACTOR_FORGE_DESC_LEN] = { 0 };	    // ��������
        ssprintf_s(szForgeDesc, sizeof(szForgeDesc), a2utf8("��ǰƷ����%d��������%s�����ı�ʯID��%d����ǰ�������ȣ�%d"), (int)pMainGemstoneItem->byGrade, bAppraisalSucess ? "�ɹ�" : "ʧ��", nGemstoneID, pMainGemstoneItem->nAppraisalCount);
        pOSSLogService->addActorForgeLog(m_pBankPart->getFromGameWorldID(), m_pBankPart->getUserID(), (DWORD)m_pMaster->getIntProperty(PROPERTY_ID), OSS_GOOD_FORGE_GEMAPPRAISAL, nGemstoneID, (pCrownInfoScheme->chGemstoneName), (int)time(NULL), szForgeDesc, 0);
    }
}

bool CCrownPageManager::broadCastGemstoneLevelUpInChatChannel(int nGemstoneGrade)
{
	if(m_pMaster == NULL)
		return false;

	EMChatChannelType eChannelType;
	switch ((EGEMSTONE_GRADE_TYPE)nGemstoneGrade)
	{
	case EGGT_THIRDGRADE:
	case EGGT_FOURTHGRADE:
		{
			eChannelType = CHAT_CHANNEL_KIN;
		}
		break;
	case EGGT_FIVEGRADE:
		{
			eChannelType = CHAT_CHANNEL_CLAN;
		}
		break;
	case EGGT_SIXTHTGRADE:
		{
			eChannelType = CHAT_CHANNEL_WORLD;
		}
		break;
	default:
		return false;
	}

	ChatHelper chatHelper;
	if (chatHelper.m_ptr == NULL)
		return false;

	PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

	char szParam[32] = {0};
	ssprintf_s(szParam, sizeof(szParam), "%d", nGemstoneGrade);
	chatHelper.m_ptr->broadcastActorTipMessage(dwPDBID, m_pMaster->getName(), CHAT_CHANNEL_KIN, 0, CHAT_TIP_CROWNS_GEMSTONEAPPRAISA_BRODCAST_SUCESS, szParam, 0);

	return true;
}





void CCrownPageManager::addCrownInlayGemstoneExp( LONG lgValue )
{
	if(m_crownPageList == NULL)
		return;

	CROWN_PAGE_ITEM* pPageItem = getGemstoneCrownPageItem(m_nCurrentCrownPage);
	if(pPageItem == NULL)
		return;

	for (int i=0;i<DBDEF_CROWN_PAGE_INLAYSlOT_MAX;++i)
	{
		int nGemstoneDBID = pPageItem->crownInlaySlotList[i];
		if(!canAddCrownGemstoneExp(nGemstoneDBID,lgValue))
			continue;

		if(!addCrownGemstoneExp(nGemstoneDBID,lgValue))
			continue;
	}
}

bool CCrownPageManager::canAddCrownGemstoneExp(int nGemstoneDBID, int nExp)
{
    if (nGemstoneDBID <= 0 || nExp <= 0)
        return false;

    return true;
}

// ��ʯ����
bool CCrownPageManager::addCrownGemstoneExp(int nGemstoneDBID, int nExp)
{
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        return false;
    }
    ISchemeCrownGemstoneInfo *pCrownInfoService = pSchemeCenter->getSchemeCrownGemstoneInfo();
    if ((pCrownInfoService == NULL))
    {
        ErrorLn(__FUNCTION__ << " pCrownInfoService == NULL! nGemstoneDBID="<< nGemstoneDBID);
        return  false;
    }
    GEMSTONE_ITEM* pGemstoneItem = getGemstonePossessItem(nGemstoneDBID);
    if (pGemstoneItem == NULL)
    {
        ErrorLn(__FUNCTION__ << " pGemstoneItem == NULL! nGemstoneDBID=" << nGemstoneDBID);
        return  false;
    }

    if (pGemstoneItem->byGrade == (int)EGGT_OBSOLETEGRADE)
        return false;

    SCrownGemstoneConfig_S *pCrownInfoScheme = pCrownInfoService->getCrownGemstoneConfig(pGemstoneItem->nGemstoneID);
    if (pCrownInfoScheme == NULL)
    {
        ErrorLn(__FUNCTION__ << " pCrownInfoScheme == NULL! nGemstoneID="<< pGemstoneItem->nGemstoneID);
        return false;
    }

    CROWN_PAGE_ITEM* pPageItem = getGemstoneCrownPageItem(m_nCurrentCrownPage);
    if (pPageItem == NULL)
        return false;

    bool bSlotFind = false;
    for (int i = 0; i < DBDEF_CROWN_PAGE_INLAYSlOT_MAX; ++i)
    {
        if (pPageItem->crownInlaySlotList[i] == nGemstoneDBID)
        {
            bSlotFind = true;
            break;
        }
    }
    if (!bSlotFind)
        return false;

    pGemstoneItem->nExp += nExp;

    if (pGemstoneItem->nExp >= pCrownInfoScheme->nExpLimitUpper)
    {
        pGemstoneItem->byGrade = EGGT_OBSOLETEGRADE;
        pGemstoneItem->nExp = 0;
    }
    else
    {
        vector<string> expVector;
        StringHelper::split(expVector, pCrownInfoScheme->stExpArray, ';', " ");

        int nCalGemstoneGrade = 1;
        for (vector<string>::iterator iterBegin = expVector.begin(); iterBegin != expVector.end(); ++iterBegin)
        {
            int nLimitExp = atoi((*iterBegin).c_str());
            if (pGemstoneItem->nExp < nLimitExp)
                break;

            nCalGemstoneGrade++;
        }

        if (nCalGemstoneGrade > pCrownInfoScheme->nGradeLimitUpper)
            nCalGemstoneGrade = (int)EGGT_OBSOLETEGRADE;

        if (pGemstoneItem->bAppraisal)
        {
            if (nCalGemstoneGrade == (int)EGGT_OBSOLETEGRADE)
            {
                pGemstoneItem->byGrade = nCalGemstoneGrade;
                pGemstoneItem->nExp = 0;
            }
        }
        else
        {
            pGemstoneItem->byGrade = nCalGemstoneGrade;
            pGemstoneItem->nExp = (nCalGemstoneGrade == EGGT_OBSOLETEGRADE ? 0 : pGemstoneItem->nExp);
        }
    }

    updateCrownGemstoneFaceScoreAndPropValue(pGemstoneItem);
    saveCrownGemstoneToDatabase(pGemstoneItem);
    sendCrownGemstoneToClient(pGemstoneItem);

    return true;
}

////////////////////////////////////��ʯ���ݿ�//////////////////////////////////////
void CCrownPageManager::readCrownGemstonesFromDatabase()
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

	DBREQ_PARAM_QUERY_ALLGEMSTONE_INFO pData;
	pData.dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);

	if (pData.dwActorID == INVALID_PDBID)
	{
		return;
	}

	obuf oBuff;
	oBuff.push_back(&pData, sizeof(DBREQ_PARAM_QUERY_ALLGEMSTONE_INFO));

	if (pDBEngine->executeSP(GAMEDB_REQUEST_QUERY_CROWN_GMESTONEITEM, pData.dwActorID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_QUERY_CROWN_GMESTONEITEM failed");
	}
}

void CCrownPageManager::onDBReturnCrownAllGemstoneInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if ( nDBRetCode != DBRET_CODE_SUCCEED )
	{
		ErrorLn( __FUNCTION__<<" nCmdID="<<nCmdID<<",nQueueIndex=" << nQueueIndex<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc);
		return;
	}
	if( m_pMaster == NULL )
	{
		ErrorLn(__FUNCTION__": m_pMaster == NULL!");
		return ;
	}
	if(nOutLen < sizeof(DBREQ_RESULT_ALLGEMSTONE_INFO))
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nOutLen<<"< DBREQ_RESULT_ALLGEMSTONE_INFO Size= "<< sizeof(DBREQ_RESULT_ALLGEMSTONE_INFO));
		return;
	}
    PDBID dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);
    if (dwActorID != (DWORD)nQueueIndex)
    {
        ErrorLn(__FUNCTION__ << " dwActorID != nQueueIndex !!! nQueueIndex=" << nQueueIndex << ",dwActorID=" << dwActorID);
        return;
    }

	DBREQ_RESULT_ALLGEMSTONE_INFO * pData = (DBREQ_RESULT_ALLGEMSTONE_INFO *)(pOutData);
	if ( pData->dwActorID != dwActorID)
	{
		return;
	}

	memset(m_crownGemstoneList,0,sizeof(m_crownGemstoneList));

	int nGemstoneNum = pData->nGemstoneCount;
	if(nGemstoneNum <= 0)
	{
        if (nGemstoneNum < 0) {
		    ErrorLn(__FUNCTION__": nGemstoneNum is invalid = " << nGemstoneNum );
        }
		return;
	}

	if(nGemstoneNum > CROWN_GEMSTONE_MAX_COUNT)
	{
		ErrorLn(__FUNCTION__": GemstonePageNum is invalid = " << nGemstoneNum << ",dwActorID=" << dwActorID);
		nGemstoneNum = CROWN_GEMSTONE_MAX_COUNT;
	}

    GEMSTONE_ITEM *pGemstoneInfo = (GEMSTONE_ITEM*)(pData+1);
	for (int i = 0; i<nGemstoneNum; ++i,pGemstoneInfo++)
	{	
		m_crownGemstoneList[i] = *pGemstoneInfo;

        // ��ȡ��ʯ�������ֵ
        m_nDBGemstoneMaxIndex = max(m_nDBGemstoneMaxIndex, pGemstoneInfo->nGemstoneDBID);
	}

	sortCrownGemstoneList();
	sendCrownGemstonesToClient();
}

void CCrownPageManager::readCrownGemstoneProductFromDatabase()
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

	DBREQ_PARAM_QUERY_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION pData;
	pData.dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);

	if (pData.dwActorID == INVALID_PDBID)
	{
		return;
	}

	obuf oBuff;
	oBuff.push_back(&pData, sizeof(DBREQ_PARAM_QUERY_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION));

	if (pDBEngine->executeSP(GAMEDB_REQUEST_QUERY_GEMSTONE_PRODUCE_DISTRIBUTION, pData.dwActorID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_QUERY_GEMSTONE_PRODUCE_DISTRIBUTION failed");
	}
}

void CCrownPageManager::onDBReturnCrownGemstoneProductInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if ( nDBRetCode != DBRET_CODE_SUCCEED )
	{
		ErrorLn( __FUNCTION__<<" nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
		return;
	}
	if( m_pMaster == NULL )
	{
		ErrorLn(__FUNCTION__": m_pMaster == NULL!");
		return ;
	}
	DWORD dwActorID = (DWORD)nQueueIndex;
	PDBID selfPDBIU =  m_pMaster->getIntProperty(PROPERTY_ID);
	if (dwActorID != selfPDBIU)
	{
		ErrorLn(__FUNCTION__ << " dwActorID != selfPDBIU !!! dwActorID="<<dwActorID);
		return;
	}

	int nEfficacyLen=sizeof(DBREQ_RESULT_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION);
	if(nOutLen<nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nOutLen<<"< DBREQ_RESULT_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION Size= "<<nEfficacyLen);
		return;
	}

	DBREQ_RESULT_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION * pData = (DBREQ_RESULT_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION *)(pOutData);
	if(pData == NULL)
		return;

	if ( pData->dwActorID != selfPDBIU)
	{		
		return;
	}

	m_gemstoneProduce = (pData->gemstoneProduce);
}

void CCrownPageManager::saveCrownGemstoneToDatabase(GEMSTONE_ITEM* pItem)
{
    if (pItem == NULL || pItem->nGemstoneDBID <= CROWN_GAMESTONE_INVALID_ID) {
        return;
    }
	if( m_pMaster == NULL )
	{
		ErrorLn(__FUNCTION__": m_pMaster == NULL!");
		return;
	}
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__" : failed pDBEngine == NULL");
		return;
	}

	DBREQ_SAVE_CROWN_GEMSTONE_POSSESS_ITEM dbParam;
    dbParam.dwActorID = (DWORD)m_pMaster->getIntProperty(PROPERTY_ID);
    dbParam.gemstoneItem = *pItem;

	if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CROWN_GMESTONEITEM, dbParam.dwActorID, (LPCSTR)&dbParam, sizeof(dbParam), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__" : pDBEngine->executeSP() GAMEDB_REQUEST_UPDATE_CROWN_GMESTONEITEM failed");
	}
}


////////////////////////////////////��ʯ���¿ͻ���//////////////////////////////////////
// ���±�ʯ���ͻ���
void CCrownPageManager::sendCrownGemstoneToClient(GEMSTONE_ITEM* pItem)
{
    if (pItem == NULL || pItem->nGemstoneDBID <= CROWN_GAMESTONE_INVALID_ID)
        return;
    if (m_pMaster == NULL)
        return;

    __IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
    if (pPlayerRole)
    {
        pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_CROWNGEMSTONE_UPDATE, pItem, sizeof(GEMSTONE_ITEM));
    }
}

void CCrownPageManager::sendCrownGemstonesToClient()
{
	if(m_pMaster == NULL)
		return;

	__IPlayerRole *pPlayerRole=CAST_TYPE(__IPlayerRole*, m_pMaster);
	if(pPlayerRole)
	{
        pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_CROWNALLGEMSTONE_UPDATE, &m_crownGemstoneList, sizeof(m_crownGemstoneList));
	}
}

// ���²�����Ϣ���ͻ���
void CCrownPageManager::sendGemstoneDistributionToClient()
{
    if (m_pMaster == NULL)
        return;

    __IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
    if (pPlayerRole)
    {
        pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_CROWNGEMSTONE_PRODUCE, &m_gemstoneProduce, sizeof(m_gemstoneProduce));
    }
}

// ��ʯ����������ͻ���
void CCrownPageManager::sendGemstoneAppraisalRetToClient(int nGemstoneDBID, int nCritNum, bool bAppraisalRet)
{
	if(m_pMaster == NULL)
		return;

	msg_entity_gemstone_appraisal_ret retData;
	retData.nGemstoneDBID = nGemstoneDBID;
	retData.nCritNum = nCritNum;
	retData.bAppraisalRet = bAppraisalRet;

	__IPlayerRole *pPlayerRole=CAST_TYPE(__IPlayerRole*, m_pMaster);
	if(pPlayerRole)
	{
        pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_CROWNGEMSTONE_APPRAISAL_RET, &retData, sizeof(retData));
	}
}

////////////////////////////////////�ʹ�ҳ�߼�///////////////////////////////////
// �ܷ�����ҳ��
bool CCrownPageManager::canAddPage(DWORD nNum/*=1*/)
{
    return (m_nPageCount+nNum) <= CROWN_PAGE_MAX_COUNT;
}

// ������ҳ��
bool CCrownPageManager::createNewPage(DWORD nNum/*=1*/)
{
    if(m_pMaster == NULL)
        return false;

    if (!canAddPage(nNum))
    {
        return false;
    }

    int nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

    bool bResult = true;
    for (DWORD i=0; i<nNum; ++i)
    {
        if(!(bResult=innerCreateOnePage()))
        {
            ErrorLn(__FUNCTION__": create failed ! page index="<< (m_nDBPageMaxIndex+1) <<", pdbid="<< nPDBID );
            break;
        }
        else
        {
            TraceLn(__FUNCTION__": create one page success, page index="<< m_nDBPageMaxIndex <<", pdbid="<< nPDBID);
        }
    }

	return bResult;
}

// �Զ�����Ĭ��ҳ
void CCrownPageManager::autoCreateDefaultPage()
{
    createNewPage(CROWN_PAGE_DEFAULT_COUNT);
}

// ����һҳ
int CCrownPageManager::innerCreateOnePage()
{
    int nReturnPageIndex = 0;

    do 
    {
        IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
        if (pDBEngine == NULL)
        {
            ErrorLn(__FUNCTION__" : failed pDBEngine == NULL");
            break;
        }

        DBREQ_SAVE_CROWNPAGE dbParam;
        dbParam.dwActorID = (DWORD) m_pMaster->getIntProperty(PROPERTY_ID);

        DBREQ_PARAM_CROWN_PAGE_ITEM & page = dbParam.page;
        page.nPageDBID = ++m_nDBPageMaxIndex;

        char szCrownPageName[GAME_NAME_MAXSIZE] = { 0 };
        ssprintf_s(szCrownPageName, sizeof(szCrownPageName), "�˾�ģ��%d", m_nDBPageMaxIndex);
        sstrcpyn(page.szName, a2utf8(szCrownPageName), sizeof(page.szName));

        if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CROWN_INFO, dbParam.dwActorID, LPCSTR(&dbParam), sizeof(dbParam), static_cast<IDBRetSink *>(this) ) == false)
        {
            ErrorLn(__FUNCTION__" : pDBEngine->executeSP() GAMEDB_REQUEST_UPDATE_CROWN_INFO failed, pdbid="<< dbParam.dwActorID);
            --m_nDBPageMaxIndex;
            break; 
        }

        // ����ҳ������
        ++m_nPageCount;

        for (int i = 0; i < CROWN_PAGE_MAX_COUNT; ++i)
        {
            if (m_crownPageList[i].nPageDBID <= 0)
            {
                m_crownPageList[i] = page;
                break;
            }
        }

        nReturnPageIndex = m_nDBPageMaxIndex;

    } while (false);

    return nReturnPageIndex;
}

bool CCrownPageManager::onMessageRequestSaveCrownPage(LPCSTR pszMsg, size_t nLen)
{
	if( m_pMaster == NULL )
	{
		ErrorLn(__FUNCTION__": m_pMaster == NULL!");
		return false;
	}
    PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	if (pszMsg == NULL || nLen != sizeof(Msg_CrownPage_Save) )
	{
		ErrorLn(__FUNCTION__": message data is null or length is invalid! len="<< nLen <<", sizeof="<< sizeof(Msg_CrownPage_Save) <<", pdbid="<<dwPDBID );
		return false;
	}
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__" : failed pDBEngine == NULL");
		return false;
	}

    Msg_CrownPage_Save *pMsg = (Msg_CrownPage_Save *)pszMsg;
    int nPageDBID = pMsg->nPageDBID;
    

	CROWN_PAGE_ITEM* pCrownPageItem = getGemstoneCrownPageItem(nPageDBID);
    // ����ϵͳ�Զ��������Ĭ�ϸ�����ҳ�������пͻ���������ҳ����¼������Կ���ֱ�ӻ�ȡ������Ӧ����
	if(pCrownPageItem == NULL)
	{
        return false;
	}

	memcpy(pCrownPageItem->crownInlaySlotList,pMsg->crownInlaySlotList,sizeof(pCrownPageItem->crownInlaySlotList));
	sstrcpyn(pCrownPageItem->szName,pMsg->szCrownPageName,sizeof(pCrownPageItem->szName));
	pCrownPageItem->nPageDBID = nPageDBID;

	DBREQ_SAVE_CROWNPAGE dbParam;
    dbParam.dwActorID = dwPDBID;
    dbParam.page = *pCrownPageItem;
	if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CROWN_INFO, dbParam.dwActorID, (LPCSTR)&dbParam, sizeof(dbParam), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__" : pDBEngine->executeSP() GAMEDB_REQUEST_UPDATE_CROWN_INFO failed");
		return false;
	}

	bool bInlayGem = false;
	for (int i = 0; i < DBDEF_CROWN_PAGE_INLAYSlOT_MAX; ++i)
	{
		if (pMsg->crownInlaySlotList[i] > 0)
		{
			bInlayGem = true;
			break;
		}
	}

	if (bInlayGem)
	{
		// ֪ͨ����
		__IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
		if (pEntityEvent != NULL)
		{
			event_game_actor_data_task eventdata;
			eventdata.nTaskLuaEvent = emTaskLuaEventType_InlayGem;
			eventdata.dwPDBID = dwPDBID;

			pEntityEvent->FireExecute(EVENT_GAME_ACTOR_DATA_TASK, (LPCSTR)&eventdata, sizeof(eventdata));
		}
	}

	return true;
}

bool CCrownPageManager::onMessageRequestDeleteCrownPage(LPCSTR pszMsg, size_t nLen)
{
	if( m_pMaster == NULL )
	{
		ErrorLn(__FUNCTION__": m_pMaster == NULL!");
		return false;
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__ << " : failed pDBEngine == NULL");
		return false;
	}

	PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	if (pszMsg == NULL || nLen != sizeof(Msg_CrownPage_Delete) )
	{
		ErrorLn(__FUNCTION__": message data is null or length is invalid! len="<< nLen <<", sizeof="<< sizeof(Msg_CrownPage_Delete) <<", pdbid="<<dwPDBID );
		return false;
	}

	Msg_CrownPage_Delete *pData = (Msg_CrownPage_Delete *)pszMsg;
	if(pData == NULL)
		return false;

	CROWN_PAGE_ITEM * pPageItem = getGemstoneCrownPageItem( pData->nPageDBID);
	if(pPageItem == NULL)
		return false;

	memset(pPageItem,0,sizeof(CROWN_PAGE_ITEM));
	sortCrownPageList();

	DBREQ_DELETE_CROWNPAGE data;
	data.dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);
	data.nPageDBID = pData->nPageDBID;

	if ( !pDBEngine->executeSP(GAMEDB_REQUEST_DELETE_CROWN_INFO, data.dwActorID, CAST_TYPE(LPCSTR, &data), sizeof(data), static_cast<IDBRetSink *>(this)) )
	{
		ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_DELETE_CROWN_INFO failed");
		return false;
	}

	return true;
}

// �л��ʹ�ҳ
bool CCrownPageManager::onMessageRequestSwitchCrownPage(int nPageDBID)
{
    if (nPageDBID > m_nDBPageMaxIndex)
        return false;

	m_nCurrentCrownPage	= nPageDBID;
	return true;
}

bool CCrownPageManager::deSerializeCrownPropertyList(rkt::obuf& out)
{
    if (m_pBankPart == NULL)
        return false;
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        ErrorLn(__FUNCTION__ << " pSchemeCenter==NULL");
        return false;
    }
    ISchemeCrownGemstoneInfo *pCrownInfoService = pSchemeCenter->getSchemeCrownGemstoneInfo();
    if ((pCrownInfoService == NULL))
    {
        ErrorLn(__FUNCTION__ << " Query CrownInfo Failed !!!");
        return  false;
    }
	CROWN_PAGE_ITEM* pPageItem = getGemstoneCrownPageItem(m_nCurrentCrownPage);
	if(pPageItem ==  NULL)
		return false;

	GEMSTONE_ITEM* pGemstoneItem = NULL;
	SCrownGemstoneConfig_S *pCrownInfoScheme = NULL;
	vector<string> propertyIDVector;
	int nPropertyCount = 0;

	memset(m_crownPropertyList,0,sizeof(m_crownPropertyList));

	for (int i=0; i<DBDEF_CROWN_PAGE_INLAYSlOT_MAX; ++i)
	{
		int nGemstoneDBID = pPageItem->crownInlaySlotList[i];
		if(nGemstoneDBID<=0)
			continue;

		pGemstoneItem = getGemstonePossessItem(nGemstoneDBID);
		if(pGemstoneItem == NULL)
			continue;

		 pCrownInfoScheme= pCrownInfoService->getCrownGemstoneConfig(pGemstoneItem->nGemstoneID);
		if(pCrownInfoScheme==NULL)
			continue;

		if(pGemstoneItem->byGrade<0||pGemstoneItem->byGrade>=CGI_LEVEL_COUNT_MAX)
			continue;

		propertyIDVector.clear();
		StringHelper::split(propertyIDVector, pCrownInfoScheme->stPropertyIDArray[pGemstoneItem->byGrade], ';', " ");
		for (vector<string>::iterator iterProIDBegin = propertyIDVector.begin(); iterProIDBegin!=propertyIDVector.end(); ++iterProIDBegin)
		{
			if(nPropertyCount>=CROWN_PROPERTY_COUNT_MAX)
				break;

			int nPropertyID = atoi((*iterProIDBegin).c_str());
			if(nPropertyID<=0)
				continue;

			m_crownPropertyList[nPropertyCount++] = nPropertyID;
		}
	}

	out.push_back(&m_nCurrentCrownPage,sizeof(int));
	out.push_back(&nPropertyCount,sizeof(int));
	for (int j=0;j<nPropertyCount;++j)
	{
		out.push_back(&m_crownPropertyList[j],sizeof(int));
	}

	return true;
}

bool CCrownPageManager::getOtherCrownPageData(obuf & outData)
{
	outData.push_back(&m_crownPageList,sizeof(m_crownPageList));
	return true;
}

bool CCrownPageManager::getOtherCrownGemstoneData(obuf & outData)
{
	outData.push_back(&m_crownGemstoneList,sizeof(m_crownGemstoneList));
	return true;
}

void CCrownPageManager::setCrownEffect(int timeToAddEffect)
{
	this->m_pBankPart->setProxyCrownPageEffect(m_crownPropertyList, timeToAddEffect);
}

////////////////////////////////////�ʹ�ҳ���ݿ�//////////////////////////////////////
// ���ݿ��ж�ȡ�Լ����лʹ�ҳ����
void CCrownPageManager::readCrownPagesFromDatabase()
{
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__ << " : failed pDBEngine == NULL");
		return;
	}

	DBREQ_PARAM_QUERY_CROWN_INFO dbParam;
	dbParam.dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);

	if (pDBEngine->executeSP(GAMEDB_REQUEST_QUERY_CROWN_INFO, dbParam.dwActorID, (LPCSTR)&dbParam, sizeof(dbParam), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__ << ": pDBEngine->executeSP() GAMEDB_REQUEST_QUERY_CROWN_INFO failed");
	}
}

void CCrownPageManager::onReturnReadCrownPages(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if ( nDBRetCode != DBRET_CODE_SUCCEED )
	{
		ErrorLn( __FUNCTION__<<" nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
		return;
	}
	if( m_pMaster == NULL )
	{
		ErrorLn(__FUNCTION__": m_pMaster == NULL!");
		return;
	}

	int nActorID =  m_pMaster->getIntProperty(PROPERTY_ID);
	if (nActorID != nQueueIndex)
	{
		ErrorLn(__FUNCTION__ <<"(): db call back is not this player! nQueueIndex="<< nQueueIndex <<", nActorID="<< nActorID);
		return;
	}
	if( nOutLen < sizeof(DBREQ_RESULT_QUERY_CROWN_PAGE) )
	{
		ErrorLn(__FUNCTION__":DataLengthFail! DataLength ="<< nOutLen <<"< DBREQ_RESULT_QUERY_CROWN_PAGE Size= "<< sizeof(DBREQ_RESULT_QUERY_CROWN_PAGE) );
		return;
	}

	DBREQ_RESULT_QUERY_CROWN_PAGE * pData = (DBREQ_RESULT_QUERY_CROWN_PAGE *)(pOutData);
    if (nOutLen != sizeof(DBREQ_RESULT_QUERY_CROWN_PAGE) + pData->nPagecount*sizeof(DBREQ_PARAM_CROWN_PAGE_ITEM))
    {
        ErrorLn(__FUNCTION__":Data Length is invalid! nOutLen=" << nOutLen << ", Len="<< (sizeof(DBREQ_RESULT_QUERY_CROWN_PAGE) + pData->nPagecount*sizeof(DBREQ_PARAM_CROWN_PAGE_ITEM)) );
        return;
    }

	// �鿴���˵ı�ʯ����
	if ( pData->dwActorID != nActorID)
	{
		return;
	}

	int nPageNum = pData->nPagecount;
    if (nPageNum < 0)
    {
        ErrorLn(__FUNCTION__": nPageNum is invalid = " << nPageNum);
        return;
    }
    if(nPageNum > CROWN_PAGE_MAX_COUNT)
	{
		ErrorLn(__FUNCTION__": Gemstone Page Count is overflow ! nPageNum="<< nPageNum);
		nPageNum = CROWN_PAGE_MAX_COUNT;
	}

    // ��ǰӵ��ҳ������
    m_nPageCount = nPageNum;
    // ÿҳ��Ƕ��ϸ��Ϣ
	memset(m_crownPageList,0,sizeof(m_crownPageList));

    // ������ݿ���û�б�ʯҳ��Ĭ��ϵͳ�Զ�����2��ҳ
    if (m_nPageCount == 0)
    {
        autoCreateDefaultPage();
        return;
    }

    // �ʹڵ�ҳ��ϸ��Ϣ
    DBREQ_PARAM_CROWN_PAGE_ITEM *pPage =(DBREQ_PARAM_CROWN_PAGE_ITEM*)(pData+1);
    for (int i = 0; i<nPageNum; ++i)
	{	
	    m_crownPageList[i] = *pPage;
        // ��ȡ���ҳ������ֵ
        m_nDBPageMaxIndex = max(m_nDBPageMaxIndex, pPage->nPageDBID);
        ++pPage;
	}

	sortCrownPageList();
	sendCrownCrownPagesToClient();
}

// ���ݿⷵ�� ���»ʹ�ҳ����
void CCrownPageManager::onReturnUpdateCrownPage(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    if (nDBRetCode != DBRET_CODE_SUCCEED)
    {
        ErrorLn(__FUNCTION__ << " nCmdID=" << nCmdID << ",nDBRetCode=" << nDBRetCode << ",Desc=" << pszDBRetDesc);
        return;
    }

    sortCrownPageList();
    sendCrownCrownPagesToClient();
}

// ���ݿⷵ�� ɾ���ʹ�ҳ����
void CCrownPageManager::onReturnDeleteCrownPage(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    if (nDBRetCode != DBRET_CODE_SUCCEED)
    {
        ErrorLn(__FUNCTION__ << " nCmdID=" << nCmdID << ",nDBRetCode=" << nDBRetCode << ",Desc=" << pszDBRetDesc);
        return;
    }

    sortCrownPageList();
    sendCrownCrownPagesToClient();
}

///////////////////////////////////�ʹ�ҳ���¿ͻ���//////////////////////////////////////
void CCrownPageManager::sendCrownCrownPagesToClient()
{
	if( m_pMaster == nullptr ) {
		return;
    }
	__IPlayerRole *pPlayerRole=CAST_TYPE(__IPlayerRole*, m_pMaster);
    if ( pPlayerRole == nullptr ) {
        return;
    }

    pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_CROWNALLCROWNPAGE_UPDATE, m_crownPageList, sizeof(m_crownPageList));
}

///////////////////////////////////���ֻ�÷���///////////////////////////////////

GEMSTONE_ITEM* CCrownPageManager::getGemstonePossessItem(int nGemstoneDBID)
{
	int nStart = 0;  
	int nEnd = CROWN_GEMSTONE_MAX_COUNT -1;
	int nIndex = -1;  
	while(nStart<=nEnd)  
	{  
		nIndex = (nStart+ nEnd)/2;  
		if(m_crownGemstoneList[nIndex].nGemstoneDBID == nGemstoneDBID)  
		{  
			return m_crownGemstoneList[nIndex].nGemstoneDBID>0?&m_crownGemstoneList[nIndex]:NULL;  
		}  
		else if(m_crownGemstoneList[nIndex].nGemstoneDBID < nGemstoneDBID)  
		{  
			nStart = nIndex + 1;  
		}  
		else  
		{  
			nEnd = nIndex -1;  
		}  
	}  

	return NULL;
}

// ����ϵͳ�Զ��������Ĭ�ϸ�����ҳ�������пͻ���������ҳ����¼������Կ���ֱ�ӻ�ȡ������Ӧ������
CROWN_PAGE_ITEM* CCrownPageManager::getGemstoneCrownPageItem(int nPageIndex)
{
    if (nPageIndex > m_nDBPageMaxIndex)
        return NULL;

    for (int i = 0; i < CROWN_PAGE_MAX_COUNT; ++i)
    {
        if (m_crownPageList[i].nPageDBID == nPageIndex)
            return &m_crownPageList[i];
    }
    return NULL;
}

int CCrownPageManager::getGemstoneDBMaxIndex()
{
	return m_nDBGemstoneMaxIndex;
}

int CCrownPageManager:: getCrownDBPageMaxIndex()
{
	return m_nDBPageMaxIndex;
}

int  CCrownPageManager:: getGemstoneListEmptyIndex(void)
{
    int nIndex = -1;
	for (int i=0; i<CROWN_GEMSTONE_MAX_COUNT; ++i)
	{
		if(m_crownGemstoneList[i].nGemstoneDBID>0)
			continue;

        nIndex = i;
        break;
	}

	return nIndex;
}

int CCrownPageManager::getCrownPageListEmptyIndex(void)
{
    int nIndex = -1;
	for (int i=0; i<CROWN_PAGE_MAX_COUNT; ++i)
	{
		if(m_crownPageList[i].nPageDBID>0)
			continue;

        nIndex = i;
        break;
	}

	return nIndex;
}

int  CCrownPageManager:: getGemstoneCount()
{
	int nProcessCount = 0;
	for (int i = 0;i<CROWN_GEMSTONE_MAX_COUNT;++i)
	{
		if(m_crownGemstoneList[i].nGemstoneDBID<=0)
			continue;

		nProcessCount++;
	}

	return nProcessCount;
}

int CCrownPageManager::getCrownPageCount()
{
    return m_nPageCount;
}

bool cmpPageList(CROWN_PAGE_ITEM& a,CROWN_PAGE_ITEM& b)
{
	return a.nPageDBID<b.nPageDBID;
}

bool cmpGemstoneList(GEMSTONE_ITEM& a,GEMSTONE_ITEM& b)
{
	return a.nGemstoneDBID<b.nGemstoneDBID;
}


void CCrownPageManager::sortCrownPageList()
{
	std::sort(m_crownPageList,m_crownPageList+(int)CROWN_PAGE_MAX_COUNT,cmpPageList);
}

void CCrownPageManager::sortCrownGemstoneList()
{
	std::sort(m_crownGemstoneList,m_crownGemstoneList+(int)CROWN_GEMSTONE_MAX_COUNT,cmpGemstoneList);
}


///////////////////////////////// IDBRetSink ////////////////////////////
/** ���ݿ����󷵻ػص�����
@param   pDBRetSink		��������ؽӿ�	
@param   nCmdID ������ɣ�
@param   nDBRetCode�����ݿ����󷵻�ֵ���ο����涨��
@param   pszDBRetDesc�����ݿ����󷵻��������ɣӣз���
@param   nQueueIndex�����ж���
@param   pOutData���������
@param   nOutLen��������ݳ���
@return  
@note     
@warning �˶���ǧ���������ݿⷵ��ǰ�ͷţ������Ƿ���
@retval buffer 
*/	
void CCrownPageManager::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	switch ( nCmdID )
	{
	case GAMEDB_REQUEST_QUERY_CROWN_INFO:        
		{
			onReturnReadCrownPages(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
    case GAMEDB_REQUEST_UPDATE_CROWN_INFO:  // ���»ʹ�ҳ����
        {
            onReturnUpdateCrownPage(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
        }
        break;
    case GAMEDB_REQUEST_DELETE_CROWN_INFO:  // ɾ���ʹ�ҳ����
        {
            onReturnDeleteCrownPage(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
        }
        break;
	case GAMEDB_REQUEST_QUERY_CROWN_GMESTONEITEM:        
		{
			onDBReturnCrownAllGemstoneInfo(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	case GAMEDB_REQUEST_QUERY_GEMSTONE_PRODUCE_DISTRIBUTION:        
		{
			onDBReturnCrownGemstoneProductInfo(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	default:
		return;
	}

	return;
}
//
//void CCrownPageManager::doTest()
//{
//	for (int i=0;i<2;++i)
//	{
//		m_crownPageList[i].nPageDBID =i+1;
//		ssprintf_s(m_crownPageList[i].szName, sizeof(m_crownPageList[i].szName) ,"CrownPage%d", i+1);
//	}
//
//	int nGemstoneList[5] = {211001,221002,231003,241004,251005};
//
//	for (int i=0;i<5;++i)
//	{
//		m_crownGemstoneList[i].nGemstoneDBID =i+10;
//		m_crownGemstoneList[i].nGemstoneID =nGemstoneList[i];
//		m_crownGemstoneList[i].byGrade =1;
//		m_crownGemstoneList[i].nExp = 10;
//	}
//
//	m_gemstoneProduce.bGemstoneProduceEnd = true;
//	sortCrownGemstoneList();
//	sortCrownPageList();
//	updateCrownCrownAllPageToClient();
//	sendCrownGemstonesToClient();
//}








