#pragma once

#include "IMonsterEvent.h"
#include "IMonsterExec.h"

class CMonsEventProtectBase: public IWarMonsterEvent
{
public:
    CMonsEventProtectBase(void);
    ~CMonsEventProtectBase(void);

    // ��ʼ��
    virtual bool Init(IWar*pWar, IMonsterExec* pMonsterExec);

    // �ͷ�
    virtual bool release();

    // �����¼�����
    virtual bool updateEventData(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn = true);

    // �����¼�
    virtual bool sendMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn = true);

    // �����¼�
    virtual bool handleSlefMonsterEvent( SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn = true);
private:
	IWar* m_pWar;

    IMonsterExec* m_pMonsterExec;
};

