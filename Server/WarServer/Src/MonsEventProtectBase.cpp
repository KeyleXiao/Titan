#include "MonsEventProtectBase.h"
#include "IWarMonsterMgr.h"


CMonsEventProtectBase::CMonsEventProtectBase(void)
{
}


CMonsEventProtectBase::~CMonsEventProtectBase(void)
{
}

bool CMonsEventProtectBase::Init(IWar*pWar, IMonsterExec *pMonsterExec)
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


bool CMonsEventProtectBase::release()
{
    m_pWar = NULL;
    m_pMonsterExec = NULL;
    return true;
}

bool CMonsEventProtectBase::handleSlefMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
    if (eventInfo.nParm <= 0)
    {
        return false;
    }
    SceneLogicHelper helper(m_pWar->getWarSceneID());
    ISceneLogicService * pSceneLogicService = helper.m_ptr;
    if(pSceneLogicService == NULL)
    {
        return false;
    }
    INPCService * pNPCService = pSceneLogicService->getNPCService();
    if (pNPCService == NULL)
    {
        return false;
    }

    obuf arrayUID;
    int nCount = 0;
    UID uid = INVALID_UID;
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
                uid = itMonster->first;
                arrayUID.push_back(&uid, sizeof(UID));
                ++nCount;
            }
            break;
        }
    }
    if (nCount == 0)
        return false;

    // 处理
    UID* pUID = CAST_TYPE(UID*, arrayUID.data());
    if (bBorn)
    {
        // 添加保护buff
        pNPCService->BatchCheckAddBuff(pUID, nCount, eventInfo.nParm, 1, uid, eventInfo.nParm, 0, 0, 0, 0, 0, false);
    }
    else
    {
        // 移除保护buff
        pNPCService->BatchRemoveBuff(pUID, nCount, eventInfo.nParm, uid, 0);
    }
    return true;
}


bool CMonsEventProtectBase::sendMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
    if (!bBorn)
    {
        // 发送保护事件的实体死亡判定
        if (nSize != sizeof(bool))
        {
            ErrorLn(__FUNCTION__": nSize is not right!!");
            return false;
        }
        bool* pisEmpty = (bool*)DataEx;

        if (!(*pisEmpty))
        {
            // 还有实体没有死亡则不执行
            return true;
        }
    }

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

    // 发送到需要执行的相关的类型去处理
    EWarMonsterExec EMEType = pWarMonsterMgr->getWarMonsterType(eventInfo.nContIndex);
    IMonsterExec* pMonsterExec = pWarMonsterMgr->getWarMonsterExecByType(EMEType);
    if (pMonsterExec != NULL)
    {
        pMonsterExec->getWarMonsterEvent(eventInfo, DataEx, nSize, bBorn);
    }
    return true;
}

bool CMonsEventProtectBase::updateEventData(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
    return true;
}
