
#include "WarMonsterEventMgr.h"
#include "IWar.h"
#include "MonsterEventFactory.h"

CWarMonsterEventMgr::CWarMonsterEventMgr()
{

}

CWarMonsterEventMgr::~CWarMonsterEventMgr()
{

}

bool CWarMonsterEventMgr::Init(IWar*pWar, IMonsterExec * pMonsterExec)
{
    if (!pWar)
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

    for (int i = EWIE_ProtectTower; i < EWIE_Max; i++)
    {
        EWarInfoEvent eEventType = (EWarInfoEvent)i;
        IWarMonsterEvent* pMonsterEvent = CMonsterEventFactory::getInstance().createMosnterEvent(eEventType);
        if (pMonsterEvent == NULL)
        {
            WarningLn(__FUNCTION__": createMonsterEvent error!! eEventType ="<<eEventType);
            continue;
        }
        if (!pMonsterEvent->Init(m_pWar,m_pMonsterExec))
        {
            WarningLn(__FUNCTION__": pMonsterEvent->Init faild!! eEventType ="<<eEventType);
            continue;
        }
        m_mapMonsterEvent[eEventType] = pMonsterEvent;
    }

    return true;
}

void CWarMonsterEventMgr::Release()
{
    map<EWarInfoEvent, IWarMonsterEvent*>::iterator iter = m_mapMonsterEvent.begin();
    for (;iter != m_mapMonsterEvent.end(); ++iter)
    {
        iter->second->release();
        iter->second = NULL;
    }

    m_mapMonsterEvent.clear();

    m_pWar = NULL;
    m_pMonsterExec = NULL;

	delete this;
}


bool CWarMonsterEventMgr::handleSlefMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
    map<EWarInfoEvent, IWarMonsterEvent*>::iterator iter = m_mapMonsterEvent.find(eventInfo.eWarEvent);
    if (iter == m_mapMonsterEvent.end())
    {
        WarningLn(__FUNCTION__": not find monster event class eWarEvent = "<<eventInfo.eWarEvent);
        return false;
    }
    return iter->second->handleSlefMonsterEvent(eventInfo, DataEx, nSize, bBorn);
}

void CWarMonsterEventMgr::handleMonsReliveEvent()
{
    MonsterRefreshVect* pMonsterRefresh = m_pMonsterExec->getMonsterRefresh();
    if (pMonsterRefresh == NULL)
    {
        ErrorLn(__FUNCTION__": pMonsterRefresh == NULL");
        return;
    }

    MonsterRefreshVect::iterator refreshEventIt = pMonsterRefresh->begin();
    for(; refreshEventIt != pMonsterRefresh->end(); ++refreshEventIt )
    {
        if (refreshEventIt->bEventHaveChanged == true)
        {
            if(refreshEventIt->RefreshWarInfo.bHaveBornEvent)
            {
                for (int i = 0; i < MAX_WAR_EVENT_DEFINE; ++i)
                {
                    SWarEventDefine EventDefine = refreshEventIt->RefreshWarInfo.WarBornEventDefine[i];

                    map<EWarInfoEvent, IWarMonsterEvent*>::iterator iterMonsterEvent = m_mapMonsterEvent.find(EventDefine.eWarEvent);
                    if (iterMonsterEvent == m_mapMonsterEvent.end() || iterMonsterEvent->second == NULL)
                    {
                        continue;
                    }

                    switch (EventDefine.eWarEvent)
                    {
                    case EWIE_ProtectTower:
                        {   
                            iterMonsterEvent->second->sendMonsterEvent(EventDefine, NULL, 0, true);
                        }
                        break;
                    case EWIE_ProtectBase:
                        {
                            // 保护基地事件
                            iterMonsterEvent->second->sendMonsterEvent(EventDefine, NULL, 0, true);
                        }
                        break;
                    case EWIE_MutexProtectBase:
                        {
                            // 要保护的索引
                            int nEffectIndex = refreshEventIt->RefreshWarInfo.nIndex;
                            iterMonsterEvent->second->updateEventData(EventDefine, &nEffectIndex, sizeof(nEffectIndex), true);
                            iterMonsterEvent->second->sendMonsterEvent(EventDefine, NULL, 0, true);
                        }
                        break;
                    case EWIE_MonsterRefresh:
                        {
                            // 如果是兵营，就进行处理 最后一个参数是状态 复活
                            iterMonsterEvent->second->sendMonsterEvent(EventDefine, NULL, 0, true);
                        }
                        break;

                    case EWIE_UltimateSoldiers:
                        {
                            // 三路水晶有复活的处理终极兵
                            obuf obufData;
                            int nCamp = refreshEventIt->RefreshWarInfo.nCamp;
                            int nIndex = refreshEventIt->RefreshWarInfo.nIndex;
                            obufData<<nCamp<<nIndex;
                            iterMonsterEvent->second->updateEventData(EventDefine, obufData.data(), obufData.size(),true);
                            iterMonsterEvent->second->sendMonsterEvent(EventDefine, &nCamp, sizeof(nCamp), true);
                        }
                        break;
                    default:
                        {
                            WarningLn(__FUNCTION__": have a event not handle eEventID="<<EventDefine.eWarEvent);
                        }
                        break;
                    }
                }
            }

            //处理完事件则设置改变事件标识
            refreshEventIt->bEventHaveChanged = false;
        }
    }
}

void CWarMonsterEventMgr::handleMonsDieEvent(sEntityDieInfo entityDieInfo, SMonsterRefreshInfo* pRefreshInfo)
{
    
    if(pRefreshInfo == NULL)
    {
        ErrorLn(__FUNCTION__": pWarInfoScheme == NULL");
        return;
    }

    if(pRefreshInfo->RefreshWarInfo.bHaveDeadEvent)
    {
        for (int i = 0; i < MAX_WAR_EVENT_DEFINE; ++i)
        {
            // 保护的事件, 保护的塔显露出来
            SWarEventDefine EventDefine = pRefreshInfo->RefreshWarInfo.WarDeadEventDefine[i];
            map<EWarInfoEvent, IWarMonsterEvent*>::iterator iterMonsterEvent = m_mapMonsterEvent.find(EventDefine.eWarEvent);
            if (iterMonsterEvent == m_mapMonsterEvent.end() || iterMonsterEvent->second == NULL)
            {
                continue;
            }

            switch (EventDefine.eWarEvent)
            {
            case EWIE_ProtectTower:
                {
                    iterMonsterEvent->second->sendMonsterEvent(EventDefine, NULL, 0, false);
                }
                break;

            case EWIE_MonsterRefresh:
                {
                    // 添加事件，给敌对添加超级兵
                    iterMonsterEvent->second->sendMonsterEvent(EventDefine,NULL, 0, false);
                    //如果是刷新时间大于0 则开启重刷
                    if(pRefreshInfo->RefreshWarInfo.nRefreshTime > 0)
                    {
                        pRefreshInfo->bIsOnRefresh = true;
                        pRefreshInfo->nRefreshTime = pRefreshInfo->RefreshWarInfo.nRefreshTime;
                        pRefreshInfo->nLatestRefreshTime = gServerGlobal->getTimeSyncService()->GetTick();
                    }
                }
                break;
            case EWIE_ProtectBase:
                {
                    bool isEmpty = pRefreshInfo->creatMonUIDList.empty();
                    iterMonsterEvent->second->sendMonsterEvent(EventDefine, &isEmpty, sizeof(isEmpty), false);

                }
                break;

            case EWIE_MutexProtectBase:
                {
                    // 互斥保护有死亡
                    int nEffectIndex = pRefreshInfo->RefreshWarInfo.nIndex;
                    iterMonsterEvent->second->updateEventData(EventDefine, &nEffectIndex, sizeof(nEffectIndex), false);
                    iterMonsterEvent->second->sendMonsterEvent(EventDefine, NULL, 0, false);

                }
                break;
            case EWIE_UltimateSoldiers:
                {
                    // 三路水晶同时破掉刷新超级兵
                    obuf obufData;
                    int nCamp = entityDieInfo.nDiedCamp;
                    int nIndex = pRefreshInfo->RefreshWarInfo.nIndex;
                    obufData<<nCamp<<nIndex;
                    iterMonsterEvent->second->updateEventData(EventDefine, obufData.data(), obufData.size(),false);
                    iterMonsterEvent->second->sendMonsterEvent(EventDefine, &nCamp, sizeof(nCamp), false);
                }
                break;
            case EWIE_CreateCampMonster:
                {
                    // 确定索引要创建的怪物
                    iterMonsterEvent->second->sendMonsterEvent(EventDefine, &(entityDieInfo.nMurderCamp), sizeof(entityDieInfo.nMurderCamp), false);
                }
                break;
            default:
                {
                    WarningLn(__FUNCTION__": have a event not handle eEventID="<<EventDefine.eWarEvent);
                }
                
                break;
            }
        }
    }

}
