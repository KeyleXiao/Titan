#include "MonsEventProtectTower.h"
#include "IWarMonsterMgr.h"

CMonsEventProtectTower::CMonsEventProtectTower(void)
{
}


CMonsEventProtectTower::~CMonsEventProtectTower(void)
{
}

bool CMonsEventProtectTower::Init(IWar*pWar, IMonsterExec *pMonsterExec)
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


bool CMonsEventProtectTower::release()
{
    m_pWar = NULL;
    m_pMonsterExec = NULL;
    return true;
}

bool CMonsEventProtectTower::handleSlefMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
    if (eventInfo.nParm <= 0)
    {
        return false;
    }

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
            CreateMonUIDList::iterator itMonster = refreshIt->creatMonUIDList.begin();
            for(; itMonster != refreshIt->creatMonUIDList.end() ; ++itMonster )
            {
                if (bBorn)
                {
                    // 添加保护buff
                    AddBuff( itMonster->first, eventInfo.nParm, 1, itMonster->first);
                }
                else
                {
                    // 移除保护buff
                    RemoveBuff(itMonster->first, eventInfo.nParm, itMonster->first, 0);
                }
            }
            break;;
        }
    }
    return true;
}


bool CMonsEventProtectTower::sendMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
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

    // 转到相应的类处理
    EWarMonsterExec EMEType = pWarMonsterMgr->getWarMonsterType(eventInfo.nContIndex);
    IMonsterExec* pMonsterExec = pWarMonsterMgr->getWarMonsterExecByType(EMEType);
    if (pMonsterExec != NULL)
    {
        pMonsterExec->getWarMonsterEvent(eventInfo, DataEx, nSize, bBorn);
    }
    return true;
}

bool CMonsEventProtectTower::updateEventData(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
    return true;
}
