#include "StdAfx.h"
#include "CommonManager.h"
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

CCommonManager::CCommonManager()
	:m_pMaster(NULL)
	,m_pBankPart(NULL)
{
}

CCommonManager::~CCommonManager()
{
	m_pMaster = NULL;
	m_pBankPart = NULL;
	m_LabelList.clear();
}

void CCommonManager::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	switch( nCmdID )
	{
	case GAMEDB_REQUEST_READ_LABEL_LIST:
		{
			onDBReturnAllLabel_Read(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	default:
		{
			ErrorLn(__FUNCTION__ "Unknow DB request.");
			return;
		}
	}
}

bool CCommonManager::onSerialize(rkt::ibuffer& in, int flag)
{
	return true;
}

bool CCommonManager::deSerialize(rkt::obuf& out, int flag)
{
	return true;
}

bool CCommonManager::init( __IEntity* pEntity,CPlayerBankPart* pPlayerBank)
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
	if (!gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__))
	{
		ErrorLn(__FUNCTION__ " Register DBEngine service failed.");
		return false;
	}

	// 读取玩家所有标签
	readAllLabel();

	return true;
}

void CCommonManager::restore()
{
	if (!gServerGlobal->getDBEngineService()->unregisterHandler(this))
	{
		WarningLn(__FUNCTION__ " Unregister DBEngine service failed.");
	}

	m_LabelList.clear();

	m_pMaster = NULL;
	m_pBankPart = NULL;
}

void CCommonManager::release(void)
{

}

void CCommonManager::readAllLabel()
{
	// 读取数据库
	DBREQ_PARAM_READ_LABEL_LIST dbParam;
	dbParam.dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

	IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
	if (NULL == pDBEngineService)
	{
		ErrorLn(__FUNCTION__ "NULL == pDBEngineService");
		return;
	}

	if (!pDBEngineService->executeSP(GAMEDB_REQUEST_READ_LABEL_LIST, dbParam.dwPDBID, (LPCSTR)&dbParam, sizeof(dbParam), CAST_TYPE(IDBRetSink*, this)))
	{
		ErrorLn("Execute GAMEDB_REQUEST_READ_LABEL_LIST failed! dwPDBID =" << dbParam.dwPDBID);
		return;
	}
}

void CCommonManager::onDBReturnAllLabel_Read(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	PDBID curPdbid = m_pMaster->getIntProperty(PROPERTY_ID);

	// 读取标签数据库结果返回
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__ << " nCmdID=" << nCmdID << ",nDBRetCode=" << nDBRetCode << ",Desc=" << pszDBRetDesc);
		return;
	}

	if (NULL == pOutData || nOutLen < sizeof(DBREQ_RESULT_READ_LABEL_LIST))
	{
		ErrorLn(__FUNCTION__ " The pointer of pOutData is null, or the length too short, nOutLen=" << nOutLen);
		return;
	}

	DBREQ_RESULT_READ_LABEL_LIST *result = (DBREQ_RESULT_READ_LABEL_LIST*)pOutData;

	if (curPdbid != result->dwPDBID)
	{
		ErrorLn(__FUNCTION__ " The pdbid of db isn't matched with current player.");
		return;
	}

	if (nOutLen != (sizeof(DBREQ_RESULT_READ_LABEL_LIST) + result->nCount * sizeof(DBREQ_RESULT_READ_LABEL_LIST)))
	{
		ErrorLn(__FUNCTION__ " nOutLen isn't matched with nRecordCount, nRecordCount=" << result->nCount);
		return;
	}

	ENTITY_TRACELN(__FUNCTION__ " The number of label is " << result->nCount);
	if (result->nCount < 1)
	{
		return;
	}

	DBREQ_TABLE_LABEL *pLabelList = (DBREQ_TABLE_LABEL*)(pOutData + sizeof(DBREQ_RESULT_READ_LABEL_LIST));
	for (size_t i = 0; i < result->nCount; ++i)
	{
		m_LabelList.push_back(*pLabelList);

		pLabelList++;
	}

	// 发送标签列表到客户端
	sendAllLabelToClient();
}

void CCommonManager::sendAllLabelToClient()
{
	if (m_LabelList.empty())
	{
		return;
	}

	msg_entity_label_list info;
	info.nCount = m_LabelList.size();

	obuf obmsg;
	obmsg << info;

	LABEL_LIST::iterator iter = m_LabelList.begin();
	for (; iter != m_LabelList.end(); ++iter)
	{
		LabelInfo tmpLabelInfo= *iter;
		obmsg.push_back(&tmpLabelInfo, sizeof(LabelInfo));
	}

	sendDataToClient(SC_MSG_ENTITY_LABEL_LIST, obmsg.data(), obmsg.size());
}

void CCommonManager::sendDataToClient(int nMsgID, void* data, size_t len)
{
	obuf obmsg;
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_ENTITY;
	head.byKeyAction = nMsgID;
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
	entityHead.byPartID = PART_BANK;			// 部件ID，ENTITY_PART_MAX时将由实体来处理
	obmsg << head << entityHead;

	obmsg.push_back(data, len);

	__IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
	if (pPlayerRole == NULL)
	{
		ErrorLn(__FUNCTION__ << " : failed pPlayerRole == NULL");
		return;
	}

	pPlayerRole->send2Client(obmsg.data(), obmsg.size());
}
