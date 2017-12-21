#include "stdafx.h"
#include "PlayerBankPart.h"
#include "ValueGoodsHelper.h"

CValueGoodsHelper::CValueGoodsHelper()
{
	m_pMaster = NULL;
	m_pBankPart = NULL;
}

CValueGoodsHelper::~CValueGoodsHelper()
{

}

void CValueGoodsHelper::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__<<" nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
	}

	switch(nCmdID)
	{
	case GAMEDB_REQUEST_READ_VALUE_GOODS:
		{
			onDBReturnReadValueGoodsInfo(nQueueIndex, pOutData, nOutLen);
		}
		break;
	case GAMEDB_REQUEST_SAVE_VALUE_GOODS:
		{

		}
		break;
	default:
		break;
	}
}

bool CValueGoodsHelper::init(CPlayerBankPart *pBankPart, __IEntity *pMaster)
{
	if(pMaster == NULL || pBankPart == NULL)
	{
		return false;
	}

	m_pMaster = pMaster;
	m_pBankPart = pBankPart;

	IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
	if(pDBEngineService)
	{
		pDBEngineService->registerHandler(CAST_TYPE(IDBRetSink*, this), __FUNCTION__);
	}

	return true;
}

void CValueGoodsHelper::restore()
{
    IDBEngineService * pDBEngineService = gServerGlobal->getDBEngineService();
    if(pDBEngineService)
    {
        pDBEngineService->unregisterHandler(this);
    }

    m_pMaster = NULL;
    m_pBankPart = NULL;
}

void CValueGoodsHelper::release()
{

}

bool CValueGoodsHelper::saveValueGoodsInfoToDB(BYTE byType, DWORD dwGoodsID, DWORD dwGoodsCount)
{
	IDBEngineService * pDBEngine = (IDBEngineService *)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL || m_pMaster == NULL)
		return false;

	int nSelfID = m_pMaster->getIntProperty(PROPERTY_ID);
	DBREQ_PARAM_SAVE_VALUE_GOODS head;
	head.nCount = 1;

	int nBuffLen = sizeof(DBREQ_TABLE_VALUE_GOODS);

	DBREQ_TABLE_VALUE_GOODS goods;
	goods.dwActorID = nSelfID;
	goods.byType	= byType;
	goods.dwID		= dwGoodsID;
	goods.dwCount	= dwGoodsCount;

	obuf ob;
	ob << head << nBuffLen << goods;
	if (pDBEngine->executeSP(GAMEDB_REQUEST_SAVE_VALUE_GOODS, nSelfID, ob.data(), ob.size(), static_cast<IDBRetSink*>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_SAVE_VALUE_GOODS failed");
		return false;
	}

	return true;
}

bool CValueGoodsHelper::readValueGoodsInfoFromDB(BYTE byType)
{
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL || m_pMaster == NULL)
		return false;

	int nSelfID = m_pMaster->getIntProperty(PROPERTY_ID);

	DBREQ_PARAM_GET_VALUE_GOODS param;
	param.dwActorID = nSelfID;
	param.byType	= byType;
	if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_VALUE_GOODS, (DWORD)byType, (LPCSTR)&param, sizeof(param), static_cast<IDBRetSink*>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_READ_VALUE_GOODS failed");
		return false;
	}

	return true;
}

void CValueGoodsHelper::onDBReturnReadValueGoodsInfo(int nQueueIndex, void * pOutData, int nOutLen)
{
	//if (m_pMaster == NULL)
	//	return;

	//DWORD dwSelfID = m_pMaster->getIntProperty(PROPERTY_ID);
	//if (nQueueIndex == INVALID_PDBID || nQueueIndex != dwSelfID)
	//{
	//	ErrorLn(__FUNCTION__": Invalid ActorID!");
	//	return;
	//}
	BYTE byGoodsType = (BYTE)nQueueIndex;

	if (pOutData == NULL)
	{
		ErrorLn(__FUNCTION__": failed pOutData == NULL!");
		return;
	}

	if (nOutLen < sizeof(DBREQ_RESULT_GET_VALUE_GOODS))
	{
		ErrorLn(__FUNCTION__": Data length fail! Data length = " << nOutLen << " < sizeof(DBREQ_RESULT_GET_VALUE_GOODS) = " << sizeof(DBREQ_RESULT_GET_VALUE_GOODS));
		return;
	}

	DBREQ_RESULT_GET_VALUE_GOODS* pHead = (DBREQ_RESULT_GET_VALUE_GOODS*)pOutData;
	int nCorrectLen = sizeof(DBREQ_RESULT_GET_VALUE_GOODS) + pHead->nCount * sizeof(DBREQ_TABLE_VALUE_GOODS);
	if (nOutLen != nCorrectLen)
	{
		ErrorLn(__FUNCTION__": Data length fail! Data length = " << nOutLen << " != sizeof(DBREQ_RESULT_GET_VALUE_GOODS) + pHead->nCount * sizeof(DBREQ_TABLE_VALUE_GOODS) = " << nCorrectLen);
		return;
	}

	//if (pHead->nCount <= 0)
	//{
	//	return;
	//}

	// ½âÎö
	vector<DBREQ_TABLE_VALUE_GOODS> vecGoods;
	DBREQ_TABLE_VALUE_GOODS * pGoods = (DBREQ_TABLE_VALUE_GOODS*)(pHead + 1);
	for (int i = 0; i < pHead->nCount; ++i, ++pGoods)
	{
		vecGoods.push_back(*pGoods);
	}

	// ¸üÐÂ
	switch(byGoodsType)
	{
	case ENVALUEGOODSTYPE_CHEST:
		{
			m_pBankPart->onDBReturnReadChestInfo(&vecGoods);
		}
		break;
	case ENVALUEGOODSTYPE_EMOJI:
		{
			m_pBankPart->onDBReturnReadEmojiInfo(&vecGoods);
		}
		break;
	default:
		break;
	}
}

