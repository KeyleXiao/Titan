#pragma once

#include "IMonsterEvent.h"
#include "IMonsterExec.h"

class CMonsEventCreateCampMonster: public IWarMonsterEvent , public TimerHandler
{
public:
    CMonsEventCreateCampMonster(void);
    ~CMonsEventCreateCampMonster(void);

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

    /////////////////////////////////////TimerHandler/////////////////////////////////////
    virtual void OnTimer(unsigned long dwTimerID);

private:
    void        WarEventCreateNPC(int nIndex);

private:

    list<DWORD> m_CreateMonsterTimer;

	IWar* m_pWar;

    IMonsterExec* m_pMonsterExec;
};

