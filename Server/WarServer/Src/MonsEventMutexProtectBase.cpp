#include "MonsEventMutexProtectBase.h"
#include "IWarMonsterMgr.h"


CMonsEventMutexProtectBase::CMonsEventMutexProtectBase(void)
{
}


CMonsEventMutexProtectBase::~CMonsEventMutexProtectBase(void)
{
}

bool CMonsEventMutexProtectBase::Init(IWar*pWar, IMonsterExec *pMonsterExec)
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


bool CMonsEventMutexProtectBase::release()
{
    m_mapNeedProtectRec.clear();
    m_pWar = NULL;
    m_pMonsterExec = NULL;
    return true;
}

bool CMonsEventMutexProtectBase::handleSlefMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
    if (eventInfo.nParm <= 0)
    {
        return false;
    }

    if(nSize != sizeof(bool))
    {
        return false;
    }

    bool* pAddOrRemoveBuf = (bool*)DataEx;
    MonsterRefreshVect* pMonsterRefresh = m_pMonsterExec->getMonsterRefresh();
    if (pMonsterRefresh == NULL)
    {
        ErrorLn(__FUNCTION__": pMonsterRefresh == NULL")
        return false;
    }

    MonsterRefreshVect::iterator refreshIt = pMonsterRefresh->begin();
    for(; refreshIt != pMonsterRefresh->end(); ++refreshIt )
    {
        // ���ҵ���ǰ����Ϣ��Ȼ����д���
        if(refreshIt->RefreshWarInfo.nIndex == eventInfo.nContIndex)
        {
            CreateMonUIDList::iterator itMonster = refreshIt->creatMonUIDList.begin();
            for(; itMonster != refreshIt->creatMonUIDList.end() ; ++itMonster )
            {
                if (*pAddOrRemoveBuf)
                {
                    // ��ӱ���buff
                    AddBuff( itMonster->first, eventInfo.nParm, 1, itMonster->first);
                }
                else
                {
                    // �Ƴ�����buff
                    RemoveBuff(itMonster->first, eventInfo.nParm, itMonster->first, 0);
                }
            }
            break;
        }
    }
    return true;
}


bool CMonsEventMutexProtectBase::sendMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
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

    map<int,MAP_RecProtect>::iterator iter = m_mapNeedProtectRec.begin();
	for (;iter != m_mapNeedProtectRec.end();++iter)
	{
		bool bAddParam = true;
		int nEffectIndex = iter->first;
		int nParam = 0;
		MAP_RecProtect::iterator iterRecBaseProtect = iter->second.begin();
		for (;iterRecBaseProtect != iter->second.end(); ++iterRecBaseProtect)
		{
			bAddParam = iterRecBaseProtect->second.bHaveExist & bAddParam;
			nParam = iterRecBaseProtect->second.nParam;
		}

        EWarMonsterExec EMEType = pWarMonsterMgr->getWarMonsterType(nEffectIndex);
        IMonsterExec* pMonsterExec = pWarMonsterMgr->getWarMonsterExecByType(EMEType);
        if (pMonsterExec != NULL)
        {
            // ת����Ӧ���ദ��
            SWarEventDefine sEvent;
            sEvent.nContIndex = nEffectIndex;
            sEvent.nParm = nParam;
            sEvent.eWarEvent = EWIE_MutexProtectBase;
            pMonsterExec->getWarMonsterEvent(sEvent, &bAddParam, sizeof(bAddParam), bBorn);
        }
	}
    return true;
}

bool CMonsEventMutexProtectBase::updateEventData(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
    // Ҫ����������
    if(nSize != sizeof(int))
    {
        ErrorLn(__FUNCTION__": nSize is not right!!");
        return false;
    }

    int* nEffectIndex = (int*)DataEx;
    int NeedProtectIndex = eventInfo.nContIndex;
    SProtectInfo tmp_ProtectInfo;
    //����Ӱ�������
    tmp_ProtectInfo.nEffectIndex = *nEffectIndex;
    // �Ƿ����
    tmp_ProtectInfo.bHaveExist = bBorn;
    //�������Ᵽ���¼�buff����
    tmp_ProtectInfo.nParam = eventInfo.nParm;

    map<int,MAP_RecProtect>::iterator iter = m_mapNeedProtectRec.find(NeedProtectIndex);
    if(iter != m_mapNeedProtectRec.end())
    {
        iter->second[tmp_ProtectInfo.nEffectIndex] = tmp_ProtectInfo;
    }
    else
    {

        MAP_RecProtect map_RecBaseProtect;
        map_RecBaseProtect[tmp_ProtectInfo.nEffectIndex] = tmp_ProtectInfo;
        m_mapNeedProtectRec[NeedProtectIndex] = map_RecBaseProtect;
    }

    return true;
}
