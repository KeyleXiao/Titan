#include "MonsEventMonsterRefresh.h"
#include "IWarMonsterMgr.h"

CMonsEventMonsterRefresh::CMonsEventMonsterRefresh(void)
{
}


CMonsEventMonsterRefresh::~CMonsEventMonsterRefresh(void)
{
}

bool CMonsEventMonsterRefresh::Init(IWar*pWar, IMonsterExec *pMonsterExec)
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


bool CMonsEventMonsterRefresh::release()
{
    m_pWar = NULL;
    m_pMonsterExec = NULL;
    return true;
}

bool CMonsEventMonsterRefresh::handleSlefMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
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
            bool bOpenRefresh = eventInfo.nParm == 1? true : false;
			refreshIt->RefreshWarInfo.bIsUse = bOpenRefresh;
			refreshIt->bIsOnRefresh = bOpenRefresh;
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

bool CMonsEventMonsterRefresh::sendMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
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

    EWarMonsterExec EMEType = pWarMonsterMgr->getWarMonsterType(eventInfo.nContIndex);
    IMonsterExec* pMonsterExec = pWarMonsterMgr->getWarMonsterExecByType(EMEType);
    if (pMonsterExec != NULL)
    {
        pMonsterExec->getWarMonsterEvent(eventInfo, DataEx, nSize, bBorn);
    }
    return true;
}

bool CMonsEventMonsterRefresh::updateEventData(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
    return true;
}
