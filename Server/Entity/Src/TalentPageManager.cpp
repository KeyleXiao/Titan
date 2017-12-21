/*******************************************************************
** �ļ���:	TalentPageManager.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/8/16
** ��  ��:	1.0
** ��  ��:	�츳ҳϵͳ

********************************************************************/
#include "stdafx.h"
#include "PlayerBankPart.h"
#include "TalentPageManager.h"
#include "IPlayerRole.h"
#include "IOSSLogService.h"
#include "OssLogDef.h"
#include "ExternalHelper.h"

#ifdef FUNCTION_TALENTPAGE_OPEN

// ���캯��
CTalentPageManager::CTalentPageManager()
    : m_pMaster(nullptr)
    , m_pBankPart(nullptr)
	, m_pEffectProxy(nullptr)
	, m_nTalentEffectPageIndex(-1)
{
	m_mapTalentPage.clear();
}

CTalentPageManager::~CTalentPageManager()
{

}

bool CTalentPageManager::onSerialize(rkt::ibuffer& in)
{

	return true;
}

bool CTalentPageManager::onSerializePage(rkt::ibuffer& in)
{
	int nTalentPagePointsNum = 0;
	in >> nTalentPagePointsNum;
	for (int i = 0; i < nTalentPagePointsNum; ++i)
	{
		TALENT_INFO talentInfo;
		in >> talentInfo;
		m_mapTalentPage[DefaultMobaEffectPageIndex].push_back(talentInfo);// ��ʱ��0Ϊս���е�Ч��ҳ
	}
	return true;
}

bool CTalentPageManager::deSerialize(rkt::obuf& out,int flag )
{

	return true;
}

bool CTalentPageManager::deSerializePage(rkt::obuf& out)
{
	if (m_nTalentEffectPageIndex <= 0)
	{
		PDBID nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
		MatchSceneHelper helper;
		m_nTalentEffectPageIndex = helper.m_ptr->getTalentPageSelectIndex(nPDBID);
	}

	TALENTPAGE_MAP::iterator iterMapTalent = m_mapTalentPage.find(m_nTalentEffectPageIndex);
	if (iterMapTalent != m_mapTalentPage.end())
	{
		int nTalentPagePointsNum = m_mapTalentPage[m_nTalentEffectPageIndex].size();
		out.push_back(&nTalentPagePointsNum, sizeof(int));

		TALENT_INFO_VECTOR::iterator iterTalent = m_mapTalentPage[m_nTalentEffectPageIndex].begin();
		for (; iterTalent != m_mapTalentPage[m_nTalentEffectPageIndex].end(); ++iterTalent)
		{
			out.push_back(&(*iterTalent), sizeof(TALENT_INFO));
		}
	}
	else
	{
		int nTalentPagePointsNum = 0;
		out.push_back(&nTalentPagePointsNum, sizeof(int));
	}

	return true;
}


/** ����
*/
bool CTalentPageManager::onLoad( __IEntity* pEntity, CPlayerBankPart* pPlayerBank, TalentEffectProxy* pEffectProxy)
{
	if(pEntity == NULL || pPlayerBank==NULL || pEffectProxy == NULL)	
		return false;

	m_pMaster = pEntity;
	m_pBankPart = pPlayerBank;
	m_pEffectProxy = pEffectProxy;

	// ���ݿ�ӿ�ע��
    IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
    if (pDBEngineService) {
        pDBEngineService->registerHandler(this, __FUNCTION__);
    }

	return true;
}

/** �ͷ�
*/
void CTalentPageManager::restore()
{
	// ���ݿ�ӿڷ�ע��
    IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
    if (pDBEngineService) {
        pDBEngineService->unregisterHandler(this);
    }
    
    m_pMaster = NULL;
    m_pBankPart = NULL;
	m_pEffectProxy = NULL;

	m_nTalentEffectPageIndex = -1;

	m_mapTalentPage.clear();
}

void CTalentPageManager::release(void)
{

}

void CTalentPageManager::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	switch (nCmdID)
	{
	case GAMEDB_REQUEST_GET_TALENTPAGE_POINTS_ALL:      // ���Ӣ�۵������츳ҳ�ļӵ���
		{
			OnDBReturnTalentPagesPoints(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	default:
		{
			ErrorLn(__FUNCTION__ "Unknow DB request.");
			return;
		}
	}
}


//////////////////////////////////////////////////////////////////////////

bool CTalentPageManager::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    switch(msgId)
    {
	case CS_MSG_ENTITY_TALENTPAGEPOINTS_SET_SINGLE:         // ���浥ҳ�츳ҳ�ļӵ�����
		{
			OnClientRequestSetTalentPageAddPointsInfo(data, len);
			return true;
		}
		break;
	case CS_MSG_ENTITY_TALENTPAGEPOINTS_INDEX_EFFECT:		// ���߷�������������ҳ��Ч��
		{
			if (len < sizeof(msg_Entity_TalentPage_Index_Effect))
			{
				return false;
			}

			msg_Entity_TalentPage_Index_Effect* pData = (msg_Entity_TalentPage_Index_Effect*)data;
			if (m_nTalentEffectPageIndex == pData->nCurPageIndex)
			{
				return false;
			}

			m_nTalentEffectPageIndex = pData->nCurPageIndex;

			m_pBankPart->setRuneTalentPageIndex();

			return true;
		}
		break;
	case CS_MSG_ENTITY_TALENTPAGEPOINTS_INDEX_DELETE:       // ɾ��ĳҳ
		{
			if (len < sizeof(msg_Entity_TalentPage_Index_Delete))
			{
				return false;
			}

			msg_Entity_TalentPage_Index_Delete* pData = (msg_Entity_TalentPage_Index_Delete*)data;
			OnClientRequestDeleteTalentPage(pData->nCurPageIndex);

			return true;
		}
		break;
    default: break;
    }

	return false;
}


// �ͻ������󱣴������츳���� 
bool CTalentPageManager::OnClientRequestSetTalentPageAddPointsInfo(void * pSetData, int nLen)
{
	if (pSetData == NULL)
	{
		ErrorLn(__FUNCTION__": failed pSetData == NULL");
		return false;
	}

	if (m_pMaster == NULL)
	{
		return false;
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return false;
	}

	int nEfficacyLen = sizeof(msg_Entity_TalentPage_Points_Set_Single);
	if (nLen<nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength =" << nLen << "< msg_Entity_TalentPage_Points_Set_Single Size= " << nEfficacyLen);
		return false;
	}

	msg_Entity_TalentPage_Points_Set_Single* pBaseTalent = (msg_Entity_TalentPage_Points_Set_Single*)pSetData;
	int nTalentNum = pBaseTalent->nPageTalentNum;
	//Ч�鳤��
	nEfficacyLen += nTalentNum * sizeof(TALENT_INFO);
	if (nLen != nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength =" << nLen << "!= TotalLength= " << nEfficacyLen);
		return false;
	}

	// ��ǰҳ���츳�ӵ���Ϣ
	TALENT_INFO_VECTOR vTalentInfoVec;
	TALENT_INFO* pTalentInfo = (TALENT_INFO*)(pBaseTalent + 1);
	for (int i = 0; i<nTalentNum; i++)
	{
		vTalentInfoVec.push_back(*pTalentInfo);
		pTalentInfo++;
	}

	// Ч������,��ֹ����
	bool bRfficacySuccess = m_pEffectProxy->RfficacyTalentPoints(vTalentInfoVec);
	if (!bRfficacySuccess)
	{
		ErrorLn(__FUNCTION__":RfficacyTalentPoints Fail!  SomeOne May Use Hack Tool");
		return false;
	}

	// ���ػ���һ��
	int nCurPageIndex = pBaseTalent->nCurPageIndex;
	m_mapTalentPage[nCurPageIndex] = vTalentInfoVec;

	int nTalentPageNum = m_mapTalentPage.size();
	if (nTalentPageNum > DEFAULTMAXPAGECOUNT) // ҳ�����������������츳���ౣ�����
	{
		ErrorLn(__FUNCTION__": talentPageNum  =" << nTalentPageNum << "is over defaultMaxPageCount=" << DEFAULTMAXPAGECOUNT);
		return false;
	}
	// ���ݿ��һ�ݣ��ȴ浱ҳ���츳������
	int nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	TALENT_DATA_SET pData;
	pData.dwActorID = nPDBID;     // ��ɫID
	pData.page.nCurPageIndex = nCurPageIndex;	        // Ӣ��ID
	sstrcpyn(pData.page.szTalentPageName, pBaseTalent->szTalentPageName, sizeof(pData.page.szTalentPageName));// ����

																											  // �������浱ǰ�츳ҳ�Ķ�������Ϣ
																											  // ����ѹ�����ݵĳ���
	obuf dbdata;
	size_t sTalentInfoLen = sizeof(TALENT_INFO);
	//�����ĳ���
	DWORD dwAllTalentInfoLen = vTalentInfoVec.size() * sTalentInfoLen;
	TALENT_INFO_VECTOR::iterator itEnd = vTalentInfoVec.end();
	// ����������TALENT_INFO����
	for (TALENT_INFO_VECTOR::iterator iter = vTalentInfoVec.begin(); iter != itEnd; iter++)
	{
		dbdata.push_back(&(*iter), sTalentInfoLen);
	}

	// ѹ��˳��:TALENT_DATA_SET->TALENT_INFO�ĳ���->���TALENT_INFO����
	obuf oBuff;
	oBuff.push_back(&pData, sizeof(TALENT_DATA_SET));
	oBuff.push_back(&dwAllTalentInfoLen, sizeof(dwAllTalentInfoLen));
	if (dwAllTalentInfoLen != 0)
	{
		oBuff.push_back(dbdata.data(), dbdata.size());
	}

	if (pDBEngine->executeSP(GAMEDB_REQUEST_SET_TALENTPAGE_POINTS, nPDBID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_SET_TALENTPAGE_POINTS failed");
		return false;
	}

	return true;
}

// �ͻ�������ɾ���츳ҳ����
void CTalentPageManager::OnClientRequestDeleteTalentPage(int nCurTalentPage)
{
	if (nCurTalentPage < 0)
	{
		return;
	}

	if (m_pMaster == NULL)
	{
		return;
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return;
	}

	TALENT_DATA_DELETE talentPageDelete;
	int nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	talentPageDelete.dwActorID = nPDBID;
	talentPageDelete.nCurPageIndex = nCurTalentPage;

	obuf oBuff;
	oBuff.push_back(&talentPageDelete, sizeof(TALENT_DATA_DELETE));
	if (pDBEngine->executeSP(GAMEDB_REQUEST_DELETE_TALENTPAGE_POINTS, nPDBID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_DELETE_TALENTPAGE_POINTS failed");
		return;
	}

	m_mapTalentPage.erase(nCurTalentPage);
}

// �����츳ҳ����
void CTalentPageManager::RequestDataBaseAllTalentPageInfo()
{
	if (m_pMaster == NULL)
	{
		return;
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return;
	}

	TALENT_DATA_REQUEST pData;
	int nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	if (nPDBID == INVALID_PDBID)
	{
		return;
	}
	pData.dwActorID = nPDBID;   // ��ɫID

	obuf oBuff;
	oBuff.push_back(&pData, sizeof(TALENT_DATA_REQUEST));

	if (pDBEngine->executeSP(GAMEDB_REQUEST_GET_TALENTPAGE_POINTS_ALL, nPDBID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_GET_TALENTPAGE_POINTS_ALL failed");
	}
}

// ���ݿ��·������츳ҳ���츳�����ݣ��浽����
void CTalentPageManager::OnDBReturnTalentPagesPoints(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__ << " nCmdID=" << nCmdID << ",nDBRetCode=" << nDBRetCode << ",Desc=" << pszDBRetDesc);
		return;
	}
	if (pOutData == NULL)
	{
		ErrorLn(__FUNCTION__": failed pOutData == NULL");
		return;
	}

	if (m_pMaster == NULL)
	{
		return;
	}

	// �Լ���ȫ���츳ҳ������
	int nPDBID = nQueueIndex;
	if (nPDBID == INVALID_UID)
	{
		return;
	}

	// Ч�鳤��ֵ
	int nEfficacyLen = sizeof(TALENT_DATA_RESULT);
	if (nOutLen<nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength =" << nOutLen << "< TALENT_DATA_RESULT Size= " << nEfficacyLen);
		return;
	}

	// У�鷵������
	TALENT_DATA_RESULT * pData = (TALENT_DATA_RESULT *)pOutData;
	m_mapTalentPage.clear();

	int nTalentPageNum = pData->byPageNum;
	if (nTalentPageNum > DEFAULTMAXPAGECOUNT)
	{
		ErrorLn(__FUNCTION__": talentPageNum is invalid = " << nTalentPageNum);
		nTalentPageNum = DEFAULTMAXPAGECOUNT;
	}

	if (nTalentPageNum < 0)
	{
		ErrorLn(__FUNCTION__": talentPageNum is invalid = " << nTalentPageNum);
		return;
	}
	// ѭ���츳ҳ
	char *pHead = (char*)(pData + 1);
	for (int i = 0; i < nTalentPageNum; ++i)
	{
		TALENT_PAGE* pTalentBase = (TALENT_PAGE*)(pHead);
		TALENT_INFO_VECTOR vTalentInfoVec;
		int nPageIndex = pTalentBase->nCurPageIndex;
		int* pAllTalentInfoLen = (int*)(pTalentBase + 1);
		int nAllTalentInfoLen = *pAllTalentInfoLen;
		int nTalentCount = nAllTalentInfoLen / sizeof(TALENT_INFO);
		// �õ���Ӷ��TALENT_INFO��obuf
		TALENT_INFO *pTalentInfo = (TALENT_INFO *)(pAllTalentInfoLen + 1);
		for (int j = 0; j<nTalentCount; j++)
		{
			vTalentInfoVec.push_back(*pTalentInfo);
			pTalentInfo++;
		}
		// Ч�鳤��
		nEfficacyLen = nEfficacyLen + sizeof(TALENT_PAGE) + sizeof(int) + nAllTalentInfoLen;
		if (nOutLen<nEfficacyLen)
		{
			ErrorLn(__FUNCTION__":DataLengthFail!  DataLength =" << nOutLen << "< EfficacyTotalLen= " << nEfficacyLen);
			return;
		}

		pHead = (char*)(pTalentInfo);
		m_mapTalentPage.insert(pair<int, TALENT_INFO_VECTOR>(nPageIndex, vTalentInfoVec));
	}

	// �·��ͻ���
	SendTalentPageInfoAllToClient(pOutData, nOutLen);
}

// �·����ͻ����츳ҳ����
void CTalentPageManager::SendTalentPageInfoAllToClient(void * pData, int nOutLen)
{
	obuf obmsg;

	obmsg.push_back(pData, nOutLen);

	__IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
	if (pPlayerRole)
	{
		pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_TALENTPAGEPOINTS_ALL, obmsg.data(), obmsg.size());
	}
}

// ��Ϸ��ʼ����Ч��
void CTalentPageManager::SetTalentPageEffect()
{
	TALENTPAGE_MAP::iterator iterTalentPage = m_mapTalentPage.find(DefaultMobaEffectPageIndex);
	if (iterTalentPage == m_mapTalentPage.end())
	{
		return;   // ���츳����Ч��
	}

	m_pEffectProxy->AddTalentEffect(m_pMaster, m_mapTalentPage[DefaultMobaEffectPageIndex]);

	return;
}

int CTalentPageManager::GetCurTalentPage()
{
	return m_nTalentEffectPageIndex == -1 ? 1 : m_nTalentEffectPageIndex;
}

#endif