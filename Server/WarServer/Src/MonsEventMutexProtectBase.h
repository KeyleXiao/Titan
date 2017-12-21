#pragma once

#include "IMonsterEvent.h"
#include "IMonsterExec.h"

class CMonsEventMutexProtectBase: public IWarMonsterEvent
{
public:
    CMonsEventMutexProtectBase(void);
    ~CMonsEventMutexProtectBase(void);

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

    // ��¼������ر�����Ϣ ���� ֵ
    struct SProtectInfo
    {
        bool bHaveExist;
        int nEffectIndex;
        int nParam;
        SProtectInfo()
        {
            //bHaveExist = false;
            //nEffectIndex = 0;
            //nParam = 0;
            memset(this, 0, sizeof(*this));
        }
    };

    typedef map<int,SProtectInfo> MAP_RecProtect;

    // ���������� ���Ᵽ������������¼
    map<int,MAP_RecProtect>  m_mapNeedProtectRec;

    IWar* m_pWar;

    IMonsterExec* m_pMonsterExec;
};

