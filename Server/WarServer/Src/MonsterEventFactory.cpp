#include "stdafx.h"
#include "MonsterEventFactory.h"
#include "MonsEventProtectTower.h"
#include "MonsEventProtectBase.h"
#include "MonsEventMonsterRefresh.h"
#include "MonsEventMutexProtectBase.h"
#include "MonsEventCreateCampMonster.h"
#include "MonsEventUltimateSoldiers.h"

CMonsterEventFactory::CMonsterEventFactory(void)
{
}


CMonsterEventFactory::~CMonsterEventFactory(void)
{
}

IWarMonsterEvent* CMonsterEventFactory::createMosnterEvent(EWarInfoEvent eEventType)
{
    switch(eEventType)
    {
    case EWIE_ProtectTower:
        {
           return new CMonsEventProtectTower();
        }
        break;
    case EWIE_MonsterRefresh:
        {
            return new CMonsEventMonsterRefresh();
        }
        break;
    case EWIE_ProtectBase:
        {
            return new CMonsEventProtectBase();
        }
        break;
    case EWIE_MutexProtectBase:
        {
            return new CMonsEventMutexProtectBase();
        }
        break;
    case EWIE_CreateCampMonster:
        {
            return new CMonsEventCreateCampMonster();
        }
        break;
    case EWIE_UltimateSoldiers:
        {
            return new CMonsEventUltimateSoldiers();
        }
        break;
    default:
        break;
    }

    return NULL;
}
