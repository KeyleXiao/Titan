#pragma once

#include "IMonsterEvent.h"
#include "IMonsterExec.h"

class CMonsEventUltimateSoldiers: public IWarMonsterEvent
{
public:
    CMonsEventUltimateSoldiers(void);
    ~CMonsEventUltimateSoldiers(void);

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

    struct SUltimateSoldiersInfo
	{
		bool bIsDestroy;
		int nIndex;
		SUltimateSoldiersInfo()
		{
			//bIsDestroy = false;
			//nIndex = 0;
            memset(this, 0, sizeof(*this));
		}
	};

    // keyλ����  valueΪ����������
	map<int, map<int, SUltimateSoldiersInfo> >        m_mapUltimateSoldiers;

	IWar* m_pWar;

    IMonsterExec* m_pMonsterExec;
};

