#pragma once
#include "Singleton.h"
#include "IMonsterEvent.h"

using namespace rkt;

class CMonsterEventFactory: public Singleton<CMonsterEventFactory>
{
public:
    CMonsterEventFactory(void);
    ~CMonsterEventFactory(void);

    IWarMonsterEvent* createMosnterEvent(EWarInfoEvent eEventType);
};
