/*******************************************************************
/*******************************************************************
** �ļ���:	RuneManager.cpp 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��绪
** ��  ��:	8/16/2017
** ��  ��:	1.0
** ��  ��:	����ϵͳ

********************************************************************/
#include "stdafx.h"
#include "PlayerBankPart.h"
#include "RuneManager.h"
#include "IPlayerRole.h"
#include "IOSSLogService.h"
#include "OssLogDef.h"
#include "ExternalHelper.h"

#ifdef FUNCTION_RUNE_OPEN

// ���캯��
CRuneManager::CRuneManager()
    : m_pMaster(nullptr)
    , m_pBankPart(nullptr)
    , m_nCurRunePageIdx(-1)     /*  ��ǰ����ҳ�� */
{
    // ��������
    m_mapRunePage.clear();
    m_mapRuneStore.clear();
}

CRuneManager::~CRuneManager()
{

}

bool CRuneManager::onSerialize(rkt::ibuffer& in)
{
	m_mapRuneStore.clear();

    // ȡ����������
    int nRuneStoreLen = 0;		
    in >> nRuneStoreLen;

    int nRuneStoreCount = nRuneStoreLen/ sizeof(EntityHeroRuneStore);
    for(int i = 0; i < nRuneStoreCount; ++i)
    {
        if (!in.good())
        {
            ErrorLn(__FUNCTION__ << __LINE__ << ",i=" << i);
            return false;
        }

        EntityHeroRuneStore node;
        in >> node;

        m_mapRuneStore[node.nRuneId] = node;
    }

	return true;
}

bool CRuneManager::onSerializePage(rkt::ibuffer& in)
{
    int nRunePagePointsNum=0;
    in >> nRunePagePointsNum; 
    for(int j = 0; j < nRunePagePointsNum; ++j)
    {
        int nRuneId = 0;
        in >> nRuneId;
        m_mapRunePage[DefaultMobaEffectPageIndex].nRuneGroove[j] = nRuneId;// ��ʱ��0Ϊս���е�Ч��ҳ
        m_mapRunePage[DefaultMobaEffectPageIndex].nRuneNum++;
    }
    
	return true;
}

bool CRuneManager::deSerialize(rkt::obuf& out,int flag )
{
    int nRuneStoreItemLen =  sizeof(EntityHeroRuneStore);
    int nRuneLen = m_mapRuneStore.size()*nRuneStoreItemLen;

    out.push_back(&nRuneLen, sizeof(int));
    RUNESTORE_MAP::iterator iterRuneStore = m_mapRuneStore.begin();
    for (;iterRuneStore != m_mapRuneStore.end(); ++iterRuneStore)
    {
        out.push_back(&iterRuneStore->second, nRuneStoreItemLen);
    }

    if(flag&TO_OTHER_SEVER || flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
    {    
        // ѹ���츳��������
        if(m_nCurRunePageIdx <= 0)
        {
            PDBID nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
            MatchSceneHelper helper;
            m_nCurRunePageIdx =  helper.m_ptr->getRunePageSelectIndex(nPDBID);
        }
    }

	return true;
}

bool CRuneManager::deSerializePage(rkt::obuf& out)
{
    // ѹ����ķ�������
    RUNE_PAGE_MAP::iterator iterMapRune = m_mapRunePage.find(m_nCurRunePageIdx);
    if (iterMapRune != m_mapRunePage.end())
    {
        obuf obRune;
        int nRunePageCount = 0;
        for( int i=0; i<RUNE_GROOVE_MAX ;++i)
        {
            int nRuneId = m_mapRunePage[m_nCurRunePageIdx].nRuneGroove[i];
            if( nRuneId ==0)
            {
                continue;
            }
            obRune.push_back(&nRuneId, sizeof(int));
            nRunePageCount++;
        }

        out.push_back(&nRunePageCount,sizeof(int));
        if(obRune.size() > 0) out.push_back(obRune.data(),obRune.size());
    }
    else
    { 
        int nRunePageCount = 0;
        out.push_back(&nRunePageCount, sizeof(int));
    }

	return true;
}


/** ����
*/
bool CRuneManager::onLoad( __IEntity* pEntity, CPlayerBankPart* pPlayerBank, TalentEffectProxy* pEffectProxy)
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
void CRuneManager::restore()
{
	// ���ݿ�ӿڷ�ע��
    IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
    if (pDBEngineService) {
        pDBEngineService->unregisterHandler(this);
    }
    
    m_pMaster = NULL;
    m_pBankPart = NULL;
}

void CRuneManager::release(void)
{

}

void CRuneManager::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	switch (nCmdID)
	{
	case GAMEDB_REQUEST_QUERY_RUNE_INFO:		// ��ȡ��ɫ���еķ�������
		{
			onDBReturnRuneInfo(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	default:
		{
			ErrorLn(__FUNCTION__ "Unknow DB request.");
			return;
		}
	}
}

/////////////////////////////////////�߼�/////////////////////////////////////
bool CRuneManager::addRuneStoreInfo(int nRuneId,int nCount)
{
    if(m_mapRuneStore.find(nRuneId) == m_mapRuneStore.end())
    {
        EntityHeroRuneStore heroRuneStore;
        heroRuneStore.nRuneId = nRuneId;
        m_mapRuneStore[nRuneId] = heroRuneStore;
    }

    m_mapRuneStore[nRuneId].byActiveRuneCount += nCount;

    // ��������Ϊ0���棬�Ժ�������˼��ٷ��� ɾ���ж�
    if(m_mapRuneStore[nRuneId].byActiveRuneCount <= 0)
    {
        return false;
    }

    m_pBankPart->updatePlayerInfoToDB();
    updateActiveRuneStoreItemToClient(nRuneId,nCount);	
    return  true;
}

size_t CRuneManager::getStore(rkt::obuf& out)
{
    int nRuneStoreItemLen =  sizeof(EntityHeroRuneStore);
    RUNESTORE_MAP::iterator iterStore = m_mapRuneStore.begin();
    for (;iterStore!= m_mapRuneStore.end();++iterStore)
    {
        out.push_back(&(iterStore->second),nRuneStoreItemLen);
    }

    return m_mapRuneStore.size();
}

void CRuneManager::setRuneTalentPageIndex(int nTalentEffectPageIndex)
{
    // �����츳����
    if(m_nCurRunePageIdx > 0 )
    {
        PDBID nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
        MatchSceneHelper helper;
        helper.m_ptr->setRuneTalentPageIndex(nPDBID,m_nCurRunePageIdx,nTalentEffectPageIndex);
    }
}

bool CRuneManager::canAddRuneStoreInfo(int nRuneId,RUNE_BUY_ERROR_TYPE& eErrorTyp,int nCount)
{	
    if(nRuneId <= 0 || nRuneId > DEFAULTMAXRUNEID)
    {
        return false;
    }

    // ��������С��0���棬�Ժ�������˼��ٷ��� ɾ���ж�
    if(nCount <= 0)
    {
        return false;
    }

    // �޶��·���������������
    if(m_mapRuneStore.size() > DEFAULTMAXRUNETYPECOUNT) 
    {
        ErrorLn(__FUNCTION__": runeTypeCount is over "<<DEFAULTMAXRUNETYPECOUNT);
        return false;
    }

    // ���id�Ƿ���������
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
    {
        return false;
    }

    ISchemeRuneInfo *pRuneInfoService = pSchemeCenter->getSchemeRuneInfo();
    if ((pRuneInfoService == NULL ))
    {
        ErrorLn(__FUNCTION__ << " Query RuneInfo Failed !!!");
        return  false;
    }

    RuneInfoScheme *pRuneInfoScheme= pRuneInfoService->GetRuneSchemeInfo(nRuneId);
    if(pRuneInfoScheme==NULL)
    {
        return  false;
    }

    // �ж��·�������id��������
    int nRuneLimitCount = pRuneInfoScheme->nLimitCount;
    if(m_mapRuneStore.find(nRuneId) != m_mapRuneStore.end())
    {
        if(m_mapRuneStore[nRuneId].byActiveRuneCount+nCount > nRuneLimitCount)
        {
            ErrorLn(__FUNCTION__": runeActiveItemCount is over "<<nRuneLimitCount);
            eErrorTyp = RBYEY_COUNTOVER;
            return false;
        }
    }
    else
    {
        if(nCount > nRuneLimitCount)
        {
            ErrorLn(__FUNCTION__": runeActiveItemCount is over "<<nRuneLimitCount);
            eErrorTyp = RBYEY_COUNTOVER;
            return false;
        }
    }

    return  true;
}


// ��ȡ��ǰ��������
bool CRuneManager::getOtherRuneData(obuf & outData)
{
    int nCount = (int)m_mapRunePage.size();
    outData<<nCount;
    RUNE_PAGE_MAP::iterator iter = m_mapRunePage.begin();
    for (iter; iter != m_mapRunePage.end(); iter++)
    {
        outData.push_back(&(iter->second), sizeof(Entity_Rune_Page_Info));
    }

    return true;
}

int CRuneManager::getCurRunePage()
{
    //return m_nCurRunePageIdx == -1 ? 1: m_nTalentEffectPageIndex;
    return m_nCurRunePageIdx == -1 ? 1: m_nCurRunePageIdx;
}

void CRuneManager::requestDataBaseRuneInlayInfo()
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

    DBREQ_PARAM_QUERY_RUNE_INFO pData;
    pData.dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);

    if (pData.dwActorID == INVALID_PDBID)
    {
        return;
    }

    obuf oBuff;
    oBuff.push_back(&pData, sizeof(DBREQ_PARAM_QUERY_RUNE_INFO));

    if (pDBEngine->executeSP(GAMEDB_REQUEST_QUERY_RUNE_INFO, pData.dwActorID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_GET_MATCH_ALL_HEROSCORE failed");
    }
}


void CRuneManager::onClientRequestOtherSyncRuneInfo(void * data, size_t nLen)
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

    PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
    if (data == NULL || nLen < sizeof(msg_Request_Other_Rune_Points) )
    {
        ErrorLn(__FUNCTION__": message data is null or length is invalid! len="<< nLen <<", sizeof="<< sizeof(msg_Request_Other_Rune_Points) <<", pdbid="<<dwPDBID );
        return ;
    }

    msg_Request_Other_Rune_Points *pRequestData = (msg_Request_Other_Rune_Points *)data;

    // �������鿴�������,ֱ�Ӵӷ������ڴ��ȡ,������������ݿ��ȡ
    IShareReceiver * pShareReceiver = gServerGlobal->getShareReceiver();// �ڹ����ȡһ����Ϣ
    if (pShareReceiver==NULL || pRequestData->dwFriendID == INVALID_PDBID)
    {
        ErrorLn(__FUNCTION__ << "pRequestPDBID == INVALID_PDBID or failed pShareReceiver == NULL");
        return;
    }	
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pRequestData->dwFriendID);

    if (shareProperty.dwPDBID != pRequestData->dwFriendID)
    {
        DBREQ_PARAM_QUERY_RUNE_INFO pData;
        pData.dwActorID = pRequestData->dwFriendID;// ������˵ķ������ݵ��Լ�part��

        obuf oBuff;
        oBuff.push_back(&pData, sizeof(DBREQ_PARAM_QUERY_RUNE_INFO));

        if (pDBEngine->executeSP(GAMEDB_REQUEST_QUERY_RUNE_INFO, dwPDBID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
        {
            ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_GET_MATCH_ALL_HEROSCORE failed");
        }
    }
    else
    {
        ActorHelper actorHelper(pRequestData->dwFriendID,ACTOR_DBID);
        IActorService *pActorService = actorHelper.m_ptr;
        if (pActorService == NULL)
        {
            ErrorLn(__FUNCTION__ << " : failed pActorService == NULL");
            return;
        }

        string mapRuneData = pActorService->getOtherRuneData();
        const char * head = mapRuneData.c_str();
        if (head == NULL)
        {
            ErrorLn(__FUNCTION__ << " : failed pActorService->getOtherRuneData()");
            return;
        }

        obuf obData;
        int * pRunePageNum = (int *)head;
        DBREQ_RESULT_QUERYRUNEINFO queryRuneInfo;
        queryRuneInfo.dwActorID = pRequestData->dwFriendID;
        queryRuneInfo.nPageNum = *pRunePageNum;
        obData.push_back(&queryRuneInfo, sizeof(queryRuneInfo));

        Entity_Rune_Page_Info *pRunePageInfo = (Entity_Rune_Page_Info *)(pRunePageNum + 1);
        for (int i = 0; i < *pRunePageNum; i++)
        {
            RUNE_PAGE runePage;
            runePage.nPageIndex = pRunePageInfo->nPageIdx;
            sstrcpyn(runePage.szTalentPageName, pRunePageInfo->szRunePageName, sizeof(runePage.szTalentPageName));
            obData.push_back(&runePage, sizeof(runePage));

            int nRuneNum = pRunePageInfo->nRuneNum;
            if (nRuneNum > RUNE_GROOVE_MAX)
            {
                nRuneNum = RUNE_GROOVE_MAX;
                ErrorLn(__FUNCTION__": nRuneNum > RUNE_GROOVE_MAX" << "nRuneNum = " << pRunePageInfo->nRuneNum);
            }
            int nRuneLen = nRuneNum * sizeof(RUNE_INFO);
            obData.push_back(&nRuneLen, sizeof(nRuneLen));

            for (int j = 0; j < RUNE_GROOVE_MAX; j++)
            {
                if (pRunePageInfo->nRuneGroove[j] == 0)
                {
                    continue;
                }

                RUNE_INFO runeInfo;
                runeInfo.nPostion = j;
                runeInfo.nID = pRunePageInfo->nRuneGroove[j];
                obData.push_back(&runeInfo, sizeof(runeInfo));
            }
            ++pRunePageInfo;
        }

        sendOtherRuneInfoToClient(obData.data(),obData.size());
    }
}



bool CRuneManager::onClientRequestSaveRunePage(LPCSTR pszMsg, size_t nLen)
{
    if( m_pMaster == NULL )
    {
        ErrorLn(__FUNCTION__": m_pMaster == NULL!");
        return false;
    }
    PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
    if (pszMsg == NULL || nLen < sizeof(msg_runePage_save) )
    {
        ErrorLn(__FUNCTION__": message data is null or length is invalid! len="<< nLen <<", sizeof="<< sizeof(msg_runePage_save) <<", pdbid="<<dwPDBID );
        return false;
    }
    msg_runePage_save *pData = (msg_runePage_save *)pszMsg;
    int nRealLen = sizeof(msg_runePage_save) + (unsigned int)pData->nRuneNum * sizeof(RUNE_INFO);
    if ( nLen != nRealLen)
    {
        ErrorLn(__FUNCTION__": length is invalid! len="<< nLen <<", sizeof="<< nRealLen <<", pdbid="<<dwPDBID );
        return false;
    }
    // ���ݺϷ�У��
    if( pData->nRuneNum < 0  || pData->nRuneNum > RUNE_GROOVE_MAX)
    {
        ErrorLn(__FUNCTION__": Rune num is invalid! len="<< pData->nRuneNum << ", pdbid="<<dwPDBID );
        return false;
    }

    IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__" : failed pDBEngine == NULL");
        return false;
    }

    int nPageIndex = pData->nRunePageIdx;

    RUNE_PAGE_MAP::iterator iterRunePage = m_mapRunePage.find(nPageIndex);
    if (iterRunePage != m_mapRunePage.end())
    {
        m_mapRunePage.erase(iterRunePage);
    }

    // �ͻ��˴��и��ķ������ã�����ͬ������������
    obuf obData;
    int nRuneNum= pData->nRuneNum;
    if( nRuneNum > 0 )
    {
        Entity_Rune_Page_Info page;
        page.nPageIdx = nPageIndex;      // ����ҳ��
        page.nRuneNum = nRuneNum;           // ����Ƕ������
        sstrcpyn(page.szRunePageName, pData->szRunePageName, sizeof(page.szRunePageName) );
        // ����ID���飬ÿһ�����������Ա��Ӧ�̶���λ,һ�������30����λ,������λֵΪ��0ֵʱ���ò�λ����Ƕ�ķ���
        int nMsgRuneInfoLen = RUNE_GROOVE_MAX * sizeof(RUNE_INFO);
        RUNE_INFO *pRuneInfo = (RUNE_INFO*)(pData+1);

        for (int i = 0; i < nRuneNum; ++i)
        {
            int nPostion = pRuneInfo->nPostion;
            if(pRuneInfo->nPostion > RUNE_GROOVE_MAX-1)
            {
                nPostion = RUNE_GROOVE_MAX-1;
                ErrorLn(__FUNCTION__": pRuneInfo->nPostion > RUNE_GROOVE_MAX - 1��PDUID= " << dwPDBID <<"pRuneInfo->nPostion ="<<pRuneInfo->nPostion);
            }

            page.nRuneGroove[ nPostion ] = pRuneInfo->nID;


            DBREQ_PARAM_RUNE_INFO pSaveDataRune;
            pSaveDataRune.nID = pRuneInfo->nID;
            pSaveDataRune.nPostion = nPostion ;
            obData.push_back(&(pSaveDataRune), sizeof(DBREQ_PARAM_RUNE_INFO));
            pRuneInfo++;
        }
        // memcpy(page.nRuneGroove, (void*)(pData+1), nMsgRuneInfoLen );�����������

        // �����ڴ�����
        m_mapRunePage[ nPageIndex ] = page;
        //////////////////////////////////////////////////////////////////////////
    }

    int nRunePageNum = m_mapRunePage.size();
    if(nRunePageNum > DEFAULTMAXPAGECOUNT) // �����������ݿ�
    {
        ErrorLn(__FUNCTION__": runePageNum  =" << nRunePageNum <<"is over defaultMaxPageCount=" << DEFAULTMAXPAGECOUNT );
        return false;
    }

    DBREQ_SAVE_RUNEPAGE pSaveDataHead;
    pSaveDataHead.dwActorID = dwPDBID;
    pSaveDataHead.page.nPageIndex = nPageIndex;
    sstrcpyn( pSaveDataHead.page.szTalentPageName, pData->szRunePageName, sizeof( pSaveDataHead.page.szTalentPageName));// ����

    obuf oBuffer;
    oBuffer.push_back(&pSaveDataHead, sizeof(DBREQ_SAVE_RUNEPAGE));
    int nRuneLen = nRuneNum * sizeof(DBREQ_PARAM_RUNE_INFO);
    oBuffer.push_back(&nRuneLen, sizeof(nRuneLen));
    oBuffer.push_back(obData.data(), obData.size());

    // ����DB
    if (pDBEngine->executeSP(GAMEDB_REQUEST_SAVE_RUNE_INFO, pSaveDataHead.dwActorID, oBuffer.data(), oBuffer.size(), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__" : pDBEngine->executeSP() GAMEDB_REQUEST_SAVE_RUNE_INFO failed");
        return false;
    }

    return true;
}



bool CRuneManager::onClientRequestDeleteRunePage(void * data, size_t len)
{
    if (len != sizeof(msg_runePage_delete))
    {
        return false;
    }
    msg_runePage_delete *pMsg = reinterpret_cast<msg_runePage_delete *>(data);

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

    DBREQ_DELETE_RUNEPAGE dbParam;
	dbParam.dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);
	dbParam.nPageIndex = pMsg->nPageIdx;

    // ����DB
    if ( !pDBEngine->executeSP(GAMEDB_REQUEST_DELETE_RUNE_INFO, dbParam.dwActorID, CAST_TYPE(LPCSTR, &dbParam), sizeof(dbParam), static_cast<IDBRetSink *>(this)) )
    {
        ErrorLn(__FUNCTION__ << " : pDBEngine->executeSP() GAMEDB_REQUEST_DELETE_RUNE_INFO failed");
        return false;
    }

    return true;
}



void CRuneManager::onDBReturnRuneInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
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
        ErrorLn(__FUNCTION__ << " Invilad ActorID !!! ");
        return;
    }

    // У�鷵������
    DBREQ_RESULT_QUERYRUNEINFO * pData = reinterpret_cast<DBREQ_RESULT_QUERYRUNEINFO *>(pOutData);
    if ( nOutLen < sizeof(DBREQ_RESULT_QUERYRUNEINFO) )
    {
        ErrorLn(__FUNCTION__ << " : nEffecicyLen length error !");
        return;
    }

    if ( pData->dwActorID != selfPDBIU)
    {		
        sendOtherRuneInfoToClient(pOutData,nOutLen);
        return;
    }

    m_mapRunePage.clear();
    int nRunePageNum = pData->nPageNum;
    if(nRunePageNum > DEFAULTMAXPAGECOUNT)
    {
        ErrorLn(__FUNCTION__": runePageNum is invalid = " << nRunePageNum );
        nRunePageNum = DEFAULTMAXPAGECOUNT;
    }

    if( nRunePageNum < 0)
    {
        ErrorLn(__FUNCTION__": runePageNum is invalid = " << nRunePageNum );
        return;
    }
    // У�����ҳ
    // ������ǰ�������ݣ�����ҳ���ݣ�
    char *pHead = reinterpret_cast<char *>(pData+1);
    for(int i = 0; i < nRunePageNum; ++i)
    {
        RUNE_PAGE *pRunePageInfo = reinterpret_cast<DBREQ_PARAM_RUNE_PAGE *>(pHead);

        Entity_Rune_Page_Info pRunePage;
        pRunePage.nPageIdx = pRunePageInfo->nPageIndex;
        sstrcpyn( pRunePage.szRunePageName, pRunePageInfo->szTalentPageName, sizeof(pRunePage.szRunePageName));// ����

        int *pRuneInfoLen = (int*)(pRunePageInfo+1);
        pRunePage.nRuneNum = *pRuneInfoLen / sizeof(RUNE_INFO);
        RUNE_INFO *pRuneInfo = (RUNE_INFO*)(pRuneInfoLen+1);
        int nRuneNum = pRunePage.nRuneNum;
        if(nRuneNum > RUNE_GROOVE_MAX)
        {
            nRuneNum = RUNE_GROOVE_MAX;
            ErrorLn(__FUNCTION__": nRuneNum > RUNE_GROOVE_MAX��PDUID= " << dwActorID <<"nRuneNum ="<<pRunePage.nRuneNum);
        }
        for (int j = 0; j < nRuneNum; ++j)
        {
            int nPostion = pRuneInfo->nPostion;
            if(pRuneInfo->nPostion > RUNE_GROOVE_MAX-1)
            {
                nPostion = RUNE_GROOVE_MAX-1;
                ErrorLn(__FUNCTION__": pRuneInfo->nPostion > RUNE_GROOVE_MAX - 1��PDUID= " << dwActorID <<"pRuneInfo->nPostion ="<<pRuneInfo->nPostion);
            }
            pRunePage.nRuneGroove[nPostion] = pRuneInfo->nID;
            pRuneInfo++;
        }

        pHead = reinterpret_cast<char *>(pRuneInfo);
        m_mapRunePage.insert(make_pair(pRunePage.nPageIdx, pRunePage));
    }

    sendRunePageInfoToClient();
}


bool CRuneManager::setRuneEffect()
{
    RUNE_PAGE_MAP::iterator iterRunePage = m_mapRunePage.find(DefaultMobaEffectPageIndex);
    if (iterRunePage == m_mapRunePage.end())
    {
        return false;  // �շ���ҳ����Ч��
    }

    // ���r�������x�����м�Ч�������ڰ����x����ĳɼ�Ч���������
	m_pEffectProxy->AddRuneEffect(m_pMaster,m_mapRunePage[DefaultMobaEffectPageIndex]);

    return true;
}

void CRuneManager::sendOtherRuneInfoToClient(void *pOutData,int nOutLen )
{
    if(m_pMaster == NULL)
    {
        return;
    }
    obuf obmsg;
    obmsg.push_back(pOutData, nOutLen);

    __IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
    if (pPlayerRole)
    {
        pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_OTHERRUNE_SYNC, obmsg.data(), obmsg.size());
    }
}


void CRuneManager::sendRunePageInfoToClient()
{
    if(m_pMaster == NULL)
    {
        return;
    }
    // ����������������Ϣͬ�����ͻ���
    // ͬ��������Ϣ
    // �·����ͻ��� ��������
    obuf obmsg;

    int nRunePageNum = m_mapRunePage.size();
    // �������ҳ��Ϣ
    // ����Ϣ����ӷ���ҳ��
    obmsg.push_back(&nRunePageNum, sizeof(nRunePageNum));
    // ����Ϣ����ӷ���ҳ��Ϣ
    for (RUNE_PAGE_MAP::iterator iterRunePage = m_mapRunePage.begin(); 
        iterRunePage != m_mapRunePage.end();
        iterRunePage++)
    {
        obmsg.push_back(&(iterRunePage->second), sizeof(Entity_Rune_Page_Info));
    }

    __IPlayerRole *pPlayerRole=CAST_TYPE(__IPlayerRole*, m_pMaster);
    if(pPlayerRole)
    {
        pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_RUNE_SYNC, obmsg.data(), obmsg.size());
    }
}


void CRuneManager::updateActiveRuneStoreItemToClient(int nRuneId,int nCount)
{
    if(m_pMaster == NULL)
    {
        return;
    }
    // ���¼�����츳
    obuf obmsg;
    EntityHeroRuneStore heroRuneStore;
    heroRuneStore.nRuneId = nRuneId;
    heroRuneStore.byActiveRuneCount = nCount;
    obmsg.push_back(&heroRuneStore,sizeof(EntityHeroRuneStore));

    __IPlayerRole *pPlayerRole=CAST_TYPE(__IPlayerRole*, m_pMaster);
    if(pPlayerRole)
    {
        pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_RUNESTORE_UPTATE, obmsg.data(), obmsg.size());
    }
}


//////////////////////////////////////////////////////////////////////////

bool CRuneManager::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    switch(msgId)
    {
    case CS_MSG_ENTITY_RUNE_DELETE:     // �ͻ�������ɾ������ҳ
        {
            onClientRequestDeleteRunePage(data, len);
			return true;
        }
        break;

    case CS_MSG_ENTITY_RUNE_INDEX_EFFECT:
        {
            if ( len < sizeof(msg_Entity_RunePage_Index_Effect))
            {
                return false;
            }
            msg_Entity_RunePage_Index_Effect* pData = (msg_Entity_RunePage_Index_Effect*)data;
            if(m_nCurRunePageIdx == pData->nCurPageIndex)
            {
                return false;
            }

            m_nCurRunePageIdx = pData->nCurPageIndex;

			m_pBankPart->setRuneTalentPageIndex();

            return true;
        }
        break;

    default: break;
    }

	return false;
}

#endif