#include "MonsEventUltimateSoldiers.h"
#include "IWarMonsterMgr.h"

CMonsEventUltimateSoldiers::CMonsEventUltimateSoldiers(void)
{
}


CMonsEventUltimateSoldiers::~CMonsEventUltimateSoldiers(void)
{
}

bool CMonsEventUltimateSoldiers::Init(IWar*pWar, IMonsterExec *pMonsterExec)
{
    if(!pWar )
    {
        ErrorLn(__FUNCTION__": pWar == NULL");
        return false;
    }

    if (!pMonsterExec)
    {
        ErrorLn(__FUNCTION__": pMonsterExec == NULL");
        return false;
    }

    m_pWar = pWar;
    m_pMonsterExec = pMonsterExec;
    return true;
}


bool CMonsEventUltimateSoldiers::release()
{
    m_pWar = NULL;
    m_pMonsterExec = NULL;
	delete this;
	return true;
}

bool CMonsEventUltimateSoldiers::handleSlefMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{

    if(nSize != sizeof(bool))
    {
        return false;
    }
    bool* pbSatus = (bool*)DataEx;
    MonsterRefreshVect* pMonsterRefresh = m_pMonsterExec->getMonsterRefresh();
    if (pMonsterRefresh == NULL)
    {
        ErrorLn(__FUNCTION__": pMonsterRefresh == NULL")
        return false;
    }

    MonsterRefreshVect::iterator refreshIt = pMonsterRefresh->begin();
	for(; refreshIt != pMonsterRefresh->end(); ++refreshIt )
	{
		// 查找到当前的信息，然后进行处理
		if(refreshIt->RefreshWarInfo.nIndex == eventInfo.nContIndex)
		{
			refreshIt->RefreshWarInfo.bIsUse = *pbSatus;
			refreshIt->bIsOnRefresh = *pbSatus;
			if (refreshIt->bIsFirstRefresh == true)
			{
				refreshIt->nRefreshTime = refreshIt->RefreshWarInfo.nStartTime;
			}
			else
			{
				refreshIt->nRefreshTime = refreshIt->RefreshWarInfo.nRefreshTime;
			}
			break;
		}
	}
    return true;
}


bool CMonsEventUltimateSoldiers::sendMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return false;
	}


	IWarMonsterMgr* pWarMonsterMgr = (IWarMonsterMgr*)m_pWar->getWarMgr(Mgr_Monster);
	if (pWarMonsterMgr == NULL)
	{
		ErrorLn(__FUNCTION__":pWarMonsterMgr == NULL");
		return false;
	}


    if(nSize != sizeof(int))
    {
        return false;
    }
    int* pCamp = (int*)DataEx;
    map<int, map<int,SUltimateSoldiersInfo> >::iterator it = m_mapUltimateSoldiers.find(*pCamp);
	if ( it == m_mapUltimateSoldiers.end() )
	{
		ErrorLn(__FUNCTION__": can't find element at m_mapUltimateSoldiers nCamp = " << *pCamp );
		return false;
	}

	int nDestoryNum = 0;
	map<int,SUltimateSoldiersInfo>* pUltimateInfo = &(it->second);
	map<int,SUltimateSoldiersInfo>::iterator soldiersInfoBegin = pUltimateInfo->begin();
	for (;soldiersInfoBegin != pUltimateInfo->end(); ++soldiersInfoBegin)
	{
		if ( soldiersInfoBegin->second.bIsDestroy == true )
		{
			nDestoryNum++;
		}
	}

    MonsterRefreshVect* pMonsterRefresh = m_pMonsterExec->getMonsterRefresh();
    if (pMonsterRefresh == NULL)
    {
        ErrorLn(__FUNCTION__": pMonsterRefresh == NULL")
        return false;
    }

	if ( nDestoryNum == pUltimateInfo->size())
	{
		map<int,SUltimateSoldiersInfo>::iterator soldiersInfoBegin = pUltimateInfo->begin();
		for (;soldiersInfoBegin != pUltimateInfo->end(); ++soldiersInfoBegin)
		{
			int nRefreshIndex = soldiersInfoBegin->second.nIndex;
			MonsterRefreshVect::iterator refreshItBegin = pMonsterRefresh->begin();
			for (; refreshItBegin != pMonsterRefresh->end(); ++refreshItBegin)
			{
				
				if ( refreshItBegin->RefreshWarInfo.nIndex != nRefreshIndex)
				{
                    continue;
				}

                // 找到了
                for (int i = 0; i < MAX_WAR_EVENT_DEFINE; ++i )
                {
                    EWarInfoEvent warEvent = refreshItBegin->RefreshWarInfo.WarBornEventDefine[i].eWarEvent;
                    if ( warEvent == EWIE_UltimateSoldiers )
                    {
                        SWarEventDefine sEventDefine = refreshItBegin->RefreshWarInfo.WarBornEventDefine[i];
                        EWarMonsterExec EMEType = pWarMonsterMgr->getWarMonsterType(sEventDefine.nContIndex);
                        IMonsterExec* pMonsterExec = pWarMonsterMgr->getWarMonsterExecByType(EMEType);
                        if (pMonsterExec != NULL)
                        {
                            bool bReFreshState = true;
                            pMonsterExec->getWarMonsterEvent(sEventDefine, &bReFreshState, sizeof(bReFreshState), bBorn);
                        }
                    }
                }
                break;
			}

            if(refreshItBegin->RefreshWarInfo.nRefreshTime > 0)
            {
                refreshItBegin->bIsOnRefresh = true;
                refreshItBegin->nRefreshTime = refreshItBegin->RefreshWarInfo.nRefreshTime;
                refreshItBegin->nLatestRefreshTime = gServerGlobal->getTimeSyncService()->GetTick();
            }
		}
	}
    else
	{

		map<int,SUltimateSoldiersInfo>::iterator it = pUltimateInfo->begin();
		for (; it != pUltimateInfo->end(); ++it )
		{
			int nRefreshIndex = it->second.nIndex;
			MonsterRefreshVect::iterator refreshItBegin = pMonsterRefresh->begin();
			SMonsterRefreshInfo sMonsterRefreshInfo;
			for (; refreshItBegin != pMonsterRefresh->end(); ++refreshItBegin)
			{
                if ( refreshItBegin->RefreshWarInfo.nIndex != nRefreshIndex)
                {
                    continue;
                }

                
                for (int i = 0; i < MAX_WAR_EVENT_DEFINE; ++i )
                {
                    EWarInfoEvent warEvent = refreshItBegin->RefreshWarInfo.WarBornEventDefine[i].eWarEvent;
                    if ( warEvent == EWIE_UltimateSoldiers )
                    {
                        SWarEventDefine sEventDefine = refreshItBegin->RefreshWarInfo.WarBornEventDefine[i];
                        EWarMonsterExec EMEType = pWarMonsterMgr->getWarMonsterType(sEventDefine.nContIndex);
                        IMonsterExec* pMonsterExec = pWarMonsterMgr->getWarMonsterExecByType(EMEType);
                        if (pMonsterExec != NULL)
                        {
                            bool bReFreshState = false;
                            pMonsterExec->getWarMonsterEvent(sEventDefine, &bReFreshState, sizeof(bReFreshState), bBorn);
                        }
                    }
                }
                break;
			}
			
		}
	}
    return true;
}

bool CMonsEventUltimateSoldiers::updateEventData(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
    if(nSize != sizeof(int)*2)
    {
        ErrorLn(__FUNCTION__": not right nSize!!");
        return false;
    }
    int* pData = (int*)DataEx;
    int nCamp = *pData;
    int nIndex = *(++pData);

    if(bBorn)
    {
        map<int, map<int,SUltimateSoldiersInfo> >::iterator it = m_mapUltimateSoldiers.find(nCamp);
        if (it == m_mapUltimateSoldiers.end())
        {
            map<int,SUltimateSoldiersInfo> UltimateInfoVec;
            SUltimateSoldiersInfo sUltimateSoldiersInfo;
            sUltimateSoldiersInfo.bIsDestroy = false;
            sUltimateSoldiersInfo.nIndex = nIndex;
            UltimateInfoVec.insert(pair<int, SUltimateSoldiersInfo>(nIndex,sUltimateSoldiersInfo));
            m_mapUltimateSoldiers[nCamp] = UltimateInfoVec;
        }
        else
        {

            map<int,SUltimateSoldiersInfo>* pUltimateInfoVec = &it->second;
            map<int, SUltimateSoldiersInfo>::iterator itBegin = pUltimateInfoVec->find(nIndex);
            if (itBegin == pUltimateInfoVec->end())
            {
                SUltimateSoldiersInfo sUltimateSoldiersInfo;
                sUltimateSoldiersInfo.bIsDestroy = false;
                sUltimateSoldiersInfo.nIndex = nIndex;
                pUltimateInfoVec->insert(pair<int, SUltimateSoldiersInfo>(nIndex,sUltimateSoldiersInfo));
            }
            else
            {
                itBegin->second.nIndex = false;
            }
        }
    }
    else
    {
        map<int, map<int, SUltimateSoldiersInfo> >::iterator itBegin = m_mapUltimateSoldiers.find(nCamp);
        if ( itBegin == m_mapUltimateSoldiers.end() )
        {
            ErrorLn("can't find info in m_mapUltimateSoldiers nCamp:" << nCamp );
            return false;
        }

        map<int, SUltimateSoldiersInfo>::iterator soldiersInfoBegin = itBegin->second.find(nIndex);
        if (soldiersInfoBegin != itBegin->second.end())
        {
            soldiersInfoBegin->second.bIsDestroy = true;
        }
    }
    return true;
    
}
