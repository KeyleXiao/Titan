#include "StdAfx.h"
#include "BenefitCardManager.h"
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
#include "ExternalHelper.h"

CBenefitCardManager::CBenefitCardManager()
	:m_pMaster(NULL)
	,m_pBankPart(NULL)
{
	m_BenefitCardInfoList.clear();
}

CBenefitCardManager::~CBenefitCardManager()
{
}


void CBenefitCardManager::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if ( nDBRetCode != DBRET_CODE_SUCCEED )
	{
		ErrorLn( __FUNCTION__<<" nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
		return;
	}

	switch( nCmdID )
	{
	case GAMEDB_REQUEST_BENEFITCARD_ADD:
		{
			onDBReturnBenefitCard_Add(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	case GAMEDB_REQUEST_BENEFITCARD_UPDATE:
		{
			onDBReturnBenefitCard_Update(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	case GAMEDB_REQUEST_BENEFITCARD_READ:
		{
			onDBReturnBenefitCard_Read(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;

	default:
		{
			return;
		}
	}
}

bool CBenefitCardManager::onSerialize(rkt::ibuffer& in)
{
	int nBenefitCardCount = 0;
	in >> nBenefitCardCount;
	for (int i = 0; i < nBenefitCardCount; ++i)
	{
		if (!in.good())
			return false;

		BenefitCardInfo info;
		in >> info;
		m_BenefitCardInfoList.push_back(info);
	}

	return true;
}

bool CBenefitCardManager::deSerialize(rkt::obuf& out)
{
	int nCount = m_BenefitCardInfoList.size();
	out.push_back(&nCount, sizeof(int));
	std::vector<BenefitCardInfo>::iterator iterBenefitCard = m_BenefitCardInfoList.begin();
	for(; iterBenefitCard != m_BenefitCardInfoList.end(); ++iterBenefitCard)
	{
		BenefitCardInfo tmpInfo = *iterBenefitCard;
		out.push_back(&tmpInfo, sizeof(BenefitCardInfo));
	}

	return true;
}

bool CBenefitCardManager::init( __IEntity* pEntity,CPlayerBankPart* pPlayerBank)
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

	// ��ȡ������濨����
	if (IsMainMap(m_pMaster->getMapID()))
	{
		readAllBenefitCard();
	}

	return true;
}

void CBenefitCardManager::restore()
{
	gServerGlobal->getDBEngineService()->unregisterHandler(this);

	// ���濨
	m_BenefitCardInfoList.clear();
	m_pMaster = NULL;
	m_pBankPart = NULL;
}

void CBenefitCardManager::release(void)
{

}

bool CBenefitCardManager::addBenefitCard(int nBenefitCardID, int nNum)
{
	PDBID actorID = m_pMaster->getIntProperty(PROPERTY_ID);
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return false;

	ISchemeBenefitCard *pSchemeBenefitCard = pSchemeCenter->getSchemeBenefitCard();
	if (pSchemeBenefitCard == NULL)
		return false;


	SBenefitCardConfig_S *pBenefitCardConfig = pSchemeBenefitCard->getBenefitCardConfig(nBenefitCardID);
	if (pBenefitCardConfig == NULL)
	{
		ErrorLn(__FUNCTION__": pBenefitCardConfig == NULL, nBenefitCardID = " << nBenefitCardID << " actorID = " << actorID);
		return false;
	}
	
	BenefitCardInfo tmpBenefitCard;
	tmpBenefitCard.nBenefitCardType = pBenefitCardConfig->nBenefitCardType;
	tmpBenefitCard.nBenefitCardSubType = pBenefitCardConfig->nBenefitCardSubType;
	tmpBenefitCard.nMultiple = pBenefitCardConfig->nMultiple;


	// �������濨���������ʱ��
	time_t now = time(NULL);
	int nCardType = pBenefitCardConfig->nBenefitCardType;
	BenefitCardInfo resultBenefitCard;
	bool bHas = getBenefitCardByType(pBenefitCardConfig->nBenefitCardType, pBenefitCardConfig->nBenefitCardSubType, resultBenefitCard);
	// �����ǰû�����������ֱ�Ӵ�
	if (!bHas)
	{
		if (pBenefitCardConfig->nBenefitCardSubType == BENEFITCARDSUBTYPE_PERIOD)
		{
			tmpBenefitCard.nBenefitValue = now + pBenefitCardConfig->nBenefitValue * (60 * 60 * 24) * nNum;
		}
		else if(pBenefitCardConfig->nBenefitCardSubType == BENEFITCARDSUBTYPE_USECOUNT)
		{
			tmpBenefitCard.nBenefitValue = pBenefitCardConfig->nBenefitValue * nNum;
		}
		else
		{
			ErrorLn(__FUNCTION__": nBenefitCardSubType��error type : " << pBenefitCardConfig->nBenefitCardSubType);
			return false;
		}

		// ��ӵ����ݿ�
		if( !addBenefitCardToDB(tmpBenefitCard))
		{
			ErrorLn(__FUNCTION__": Add benefitcard to db fail: actorID=" << actorID);
			return false;
		}
	}
	else
	{
		tmpBenefitCard.nBenefitValue = resultBenefitCard.nBenefitValue;

		if (pBenefitCardConfig->nBenefitCardSubType == BENEFITCARDSUBTYPE_PERIOD)
		{
			if (resultBenefitCard.nBenefitValue < now)   // �����ˣ�������ڿ�ʼ��ʱ
			{
				tmpBenefitCard.nBenefitValue = now + pBenefitCardConfig->nBenefitValue * (60 * 60 *24) * nNum;
			}
			else // ʹ��ʱ�����
			{
				tmpBenefitCard.nBenefitValue += pBenefitCardConfig->nBenefitValue * (60 * 60 *24) * nNum;
			}
		}
		else if (pBenefitCardConfig->nBenefitCardSubType == BENEFITCARDSUBTYPE_USECOUNT)
		{
			tmpBenefitCard.nBenefitValue += pBenefitCardConfig->nBenefitValue * nNum;
		}
		else
		{
			ErrorLn(__FUNCTION__": m_benefitCardInfo[nCardType].nBenefitCardSubType��error type : " << pBenefitCardConfig->nBenefitCardSubType);
			return false;
		}

		// ���µ����ݿ�
		if( !updateBenefitCardToDB(tmpBenefitCard))
		{
			ErrorLn(__FUNCTION__": Add benefitcard to db fail: actorID=" << actorID);
			return false;
		}

	}

	setBenefitCard(tmpBenefitCard);

#ifdef VERSEION_DEBUG
	// ���ڵ���
	printAllBenefitCard();
#endif

	return true;
}

// �ж��Ƿ����������濨���ж����ݣ��Ƿ��Ѿ���ͬ���͵Ŀ���nBenefitCardType��ͬ������nBenefitCardSubType���Ͳ�ͬ
bool CBenefitCardManager::canAddBenefitCard(int nBenefitCardID, int nNum)
{
	return true;
}

// ���ݿⷵ�ؽ����������濨
void CBenefitCardManager::onDBReturnBenefitCard_Add( IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen )
{
	// ������濨���ݿ�������
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		// д������濨ʧ����־
		IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
		if (NULL != pOSSLogService && NULL != pszDBRetDesc)
		{
			char szText[512] = {0};
			sprintf_s(szText, sizeof(szText), "Add benefitCard failed,%s", pszDBRetDesc);
			pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_PERSON, nQueueIndex, szText);
		}

		// ���߿ͻ��ˣ�������濨ʧ��
		g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_BENEFITCARD_ADD_FAIL);
		return;
	}

	// ���͵��ͻ���
	sendReaminBenefitCardToClient();
}

// ��ȡ������濨��Ϣ
void CBenefitCardManager::readAllBenefitCard()
{
	// ��ȡ���ݿ�
	DB_Param_Read_BenefitCard dbParam;
	dbParam.dwPlayerID = m_pMaster->getIntProperty(PROPERTY_ID);

	IDBEngineService* pDBEngineService =  gServerGlobal->getDBEngineService();
	if(NULL == pDBEngineService)
	{
		ErrorLn(__FUNCTION__ "NULL == pDBEngineService");
		return ;
	}

	if (!pDBEngineService->executeSP(GAMEDB_REQUEST_BENEFITCARD_READ, dbParam.dwPlayerID, (LPCSTR)&dbParam, sizeof(dbParam), CAST_TYPE(IDBRetSink*, this)))
	{
		ErrorLn("Execute GAMEDB_REQUEST_BENEFITCARD_READ failed! dwPlayerID ="<<dbParam.dwPlayerID);
		return ;
	}
}


void CBenefitCardManager::onDBReturnBenefitCard_Read( IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen )
{
	// ��ȡ���濨���ݿ�������
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__ " Read benefitCard fail. PlayerID=" << m_pMaster->getIntProperty(PROPERTY_ID));
		return;
	}

	if (NULL == pOutData || nOutLen < sizeof(DB_Result_Read_BenefitCard))
	{
		ErrorLn(__FUNCTION__ " The length too short, nOutLen=" << nOutLen);
		return;
	}

	DB_Result_Read_BenefitCard *result = (DB_Result_Read_BenefitCard*)pOutData;

	if (nOutLen !=  (sizeof(DB_Result_Read_BenefitCard) + result->nRecordCount * sizeof(DB_Table_BenefitCard)) )
	{
		ErrorLn(__FUNCTION__ " nOutLen isn't matched with nRecordCount, nRecordCount=" << result->nRecordCount);
		return;
	}


	if (result->nRecordCount < 1)
	{
		ENTITY_TRACELN(__FUNCTION__ " The number of benefitCard is 0, nRecordCount=" << result->nRecordCount);
		return;
	}

	DB_Table_BenefitCard *pBenefitCardList = (DB_Table_BenefitCard*) (pOutData + sizeof(DB_Result_Read_BenefitCard));

	ENTITY_TRACELN(__FUNCTION__ " The number of benefitCard is " << result->nRecordCount);

	// �������濨��Ϣ
	for (int i = 0; i < result->nRecordCount; ++i)
	{
		int type = pBenefitCardList->nBenefitCardType;
		BenefitCardInfo tmpInfo;

		tmpInfo.nBenefitCardType = pBenefitCardList->nBenefitCardType;
		tmpInfo.nBenefitCardSubType = pBenefitCardList->nBenefitCardSubType;
		tmpInfo.nBenefitValue = pBenefitCardList->nBenefitValue;
		tmpInfo.nMultiple = pBenefitCardList->nMultiple;

		setBenefitCard(tmpInfo);

		pBenefitCardList++;
	}

	sendReaminBenefitCardToClient();
}

// ���ݿⷵ�ؽ������������������濨��Ϣ
void CBenefitCardManager::onDBReturnBenefitCard_Update( IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen )
{
	// ��ȡ���濨���ݿ�������
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__ " Update benefitCard fail. PlayerID=" << m_pMaster->getIntProperty(PROPERTY_ID));
		return;
	}

	if (m_BenefitCardInfoList.empty())
	{
		return ;
	}

	sendReaminBenefitCardToClient();
}

// �������濨����
void CBenefitCardManager::calcBenefitCardMultiple(int *nGoldMultiple, int *nActorExpMultiple, int *nHeroExpMultiple)
{
	BENEFITCARD_VECTOR usedCardList;

	// ���������濨����
	*nGoldMultiple = calcBenefitCardMultiple(BENEFITCARDTYPE_GOLD, usedCardList);

	// �����ɫ���鿨����
	*nActorExpMultiple = calcBenefitCardMultiple(BENEFITCARDTYPE_ACTOR_EXP, usedCardList);

	// ����Ӣ�۾��鿨����
	*nHeroExpMultiple = calcBenefitCardMultiple(BENEFITCARDTYPE_HERO_EXP, usedCardList);

	// ���ͱ��ִ���Ч���Ŀ�
	sendBenefitCardToClient(usedCardList, BENEFITCARDLISTTYPE_USED);

#ifdef VERSEION_DEBUG
	// ���ڵ���
	printBenefitCardList(usedCardList, "usedCardList:");
#endif

	// ����ʣ�����濨��Ϣ
	sendReaminBenefitCardToClient();
}

bool CBenefitCardManager::updateBenefitCardToDB( BenefitCardInfo& info)
{
	// �������ݵ����ݿ�
	DB_Param_Update_BenefitCard dbParam;
	dbParam.dwPlayerID = m_pMaster->getIntProperty(PROPERTY_ID);
	dbParam.nBenefitCardType = info.nBenefitCardType;
	dbParam.nBenefitCardSubType = info.nBenefitCardSubType;
	dbParam.nBenefitValue = info.nBenefitValue;
	dbParam.nMultiple = info.nMultiple;

	IDBEngineService* pDBEngineService =  gServerGlobal->getDBEngineService();
	if(NULL == pDBEngineService)
	{
		ErrorLn(__FUNCTION__ "NULL == pDBEngineService");
		return false;
	}

	if (!pDBEngineService->executeSP(GAMEDB_REQUEST_BENEFITCARD_UPDATE, dbParam.dwPlayerID, (LPCSTR)&dbParam, sizeof(dbParam), CAST_TYPE(IDBRetSink*, this)))
	{
		ErrorLn("Execute GAMEDB_REQUEST_BENEFITCARD_UPDATE failed! dwPlayerID ="<<dbParam.dwPlayerID<<" cardType="<<dbParam.nBenefitCardType);
		return false;
	}

	return true;
}

bool CBenefitCardManager::addBenefitCardToDB( BenefitCardInfo& info)
{
	// д�����ݿ�
	DB_Param_Add_BenefitCard dbParam;
	dbParam.dwPlayerID = m_pMaster->getIntProperty(PROPERTY_ID);
	dbParam.nBenefitCardType = info.nBenefitCardType;
	dbParam.nBenefitCardSubType = info.nBenefitCardSubType;
	dbParam.nBenefitValue = info.nBenefitValue;
	dbParam.nMultiple = info.nMultiple;

	IDBEngineService* pDBEngineService =  gServerGlobal->getDBEngineService();
	if(NULL == pDBEngineService)
	{
		ErrorLn(__FUNCTION__ "NULL == pDBEngineService");
		return false;
	}

	if (!pDBEngineService->executeSP(GAMEDB_REQUEST_BENEFITCARD_ADD, dbParam.dwPlayerID, (LPCSTR)&dbParam, sizeof(dbParam), CAST_TYPE(IDBRetSink*, this)))
	{
		ErrorLn("Execute GAMEDB_REQUEST_BENEFITCARD_ADD failed! BenefitCardType="<<dbParam.nBenefitCardType);
		return false;
	}

	return true;
}

void CBenefitCardManager::sendBenefitCardToClient(BENEFITCARD_VECTOR& inCardList, int nListType)
{
	if (inCardList.empty())
	{
		return;
	}

	obuf obmsg;
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
	head.byKeyModule   = MSG_MODULEID_ENTITY;
	head.byKeyAction   = SC_MSG_ENTITY_BENEFITCARD_INFO; 
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();	// ʵ��UID
	entityHead.byPartID = PART_BANK;			// ����ID��ENTITY_PART_MAXʱ����ʵ��������
	obmsg << head << entityHead;

	msg_entity_benefitcard_info info;
	
	info.nListType = nListType;
	info.nCount = inCardList.size();
	obmsg << info;

	BENEFITCARD_VECTOR::iterator iter = inCardList.begin();
	for (; iter != inCardList.end(); ++iter)
	{
		BenefitCardInfo tmpCardInfo = *iter;
		obmsg.push_back(&tmpCardInfo, sizeof(BenefitCardInfo));
	}

	__IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
	if (pPlayerRole == NULL)
	{
		ErrorLn(__FUNCTION__ << " : failed pPlayerRole == NULL");
		return;
	}
	pPlayerRole->send2Client(obmsg.data(), obmsg.size());
}

void CBenefitCardManager::printAllBenefitCard()
{
	printBenefitCardList(m_BenefitCardInfoList, "m_BenefitCardInfoList");
}

bool CBenefitCardManager::getBenefitCardByType( int type, int subType, BenefitCardInfo& cardInfo)
{
	BENEFITCARD_VECTOR::iterator iter = m_BenefitCardInfoList.begin();
	for (;iter != m_BenefitCardInfoList.end(); ++iter)
	{
		if (iter->nBenefitCardType == type && iter->nBenefitCardSubType == subType)
		{
			cardInfo = *iter;
			return true;
		}
	}

	return false;
}

void CBenefitCardManager::setBenefitCard(const BenefitCardInfo& cardInfo )
{
	// ���Ҹ����͵Ŀ�
	BENEFITCARD_VECTOR::iterator iter = m_BenefitCardInfoList.begin();
	for (;iter != m_BenefitCardInfoList.end(); ++iter)
	{
		if (iter->nBenefitCardType == cardInfo.nBenefitCardType && iter->nBenefitCardSubType == cardInfo.nBenefitCardSubType)
		{
			break;
		}
	}

	if (iter != m_BenefitCardInfoList.end())
	{
		iter->nBenefitCardType = cardInfo.nBenefitCardType;
		iter->nBenefitCardSubType = cardInfo.nBenefitCardSubType;
		iter->nBenefitValue = cardInfo.nBenefitValue;
		iter->nMultiple = cardInfo.nMultiple;
	}
	else
	{
		m_BenefitCardInfoList.push_back(cardInfo);
	}
}

bool CBenefitCardManager::isValidBenefitCard( BenefitCardInfo& card )
{
	if (card.nBenefitCardSubType == BENEFITCARDSUBTYPE_PERIOD)
	{
		time_t now = time(NULL);
		return now < card.nBenefitValue;
	}
	else if (card.nBenefitCardSubType == BENEFITCARDSUBTYPE_USECOUNT)
	{
		return card.nBenefitValue > 0;
	}
	else
	{
		ErrorLn(__FUNCTION__ " Invalid benefitcard subtype, subtype=" << card.nBenefitCardSubType);
		return false;
	}
}

void CBenefitCardManager::printBenefitCardList( BENEFITCARD_VECTOR& cardList, string extraLog)
{
	BENEFITCARD_VECTOR::iterator iter = cardList.begin();
	for (; iter != cardList.end(); ++iter)
	{
		TraceLn(extraLog << ": BenefitCardInfo: type=" << iter->nBenefitCardType << " subType=" << iter->nBenefitCardSubType << 
			" Value=" << iter->nBenefitValue << " Multiple=" << iter->nMultiple);
	}
}

void CBenefitCardManager::sendReaminBenefitCardToClient()
{
	sendBenefitCardToClient(m_BenefitCardInfoList, BENEFITCARDLISTTYPE_REMAIN);
}

int CBenefitCardManager::calcBenefitCardMultiple( int type, BENEFITCARD_VECTOR& usedList)
{
	int nMultiple = 1;
	BENEFITCARD_VECTOR::iterator iter = m_BenefitCardInfoList.begin();
	for (; iter != m_BenefitCardInfoList.end(); ++iter)
	{
		if (iter->nBenefitCardType == type)
		{
			if (isValidBenefitCard(*iter))
			{
				// ��������ȥʹ�ô�������ˢ�µ����ݿ�
				if (iter->nBenefitCardSubType == BENEFITCARDSUBTYPE_USECOUNT)
				{
					iter->nBenefitValue--;
					updateBenefitCardToDB(*iter);
				}

				nMultiple += iter->nMultiple;
				usedList.push_back(*iter);
			}
		}
	}
	return nMultiple;
}
