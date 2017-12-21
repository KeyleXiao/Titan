#include "MobaGoodsIDUniqueProp.h"
#include "ISchemeCenter.h"
#include "IEffect.h"

CMobaGoodsIDUniqueProp::CMobaGoodsIDUniqueProp()
{

}

CMobaGoodsIDUniqueProp::~CMobaGoodsIDUniqueProp()
{
	
}

void CMobaGoodsIDUniqueProp::Release()
{
    RemoveAllEffect();
	delete this;
}

bool CMobaGoodsIDUniqueProp::AddEffect(int nSlotID, int nEffectID, UID OperatorUID, UID TargetUID)
{
	//��Ψһ����Ч����ӹ���
	if (nEffectID == 0 )
	{
		return false;
	}

	bool bAddSucesse = false;
	// IDΨһ�ж�
	T_MAP_ID_EFFECT_ALL::iterator iter = m_EffectAll.find(nEffectID);
	if (iter == m_EffectAll.end())
	{
		
        int nEntityRecEffectIndex = AddEntityGoodsEffect(nSlotID, nEffectID, OperatorUID, TargetUID);
        if (nEntityRecEffectIndex == 0)
        {
            bAddSucesse = false;
        }
        else
        {
            SPropInfo tmpIDUniquePropInfo;
            tmpIDUniquePropInfo.nEffectID = nEffectID;
            tmpIDUniquePropInfo.nSlot = nSlotID;
            tmpIDUniquePropInfo.nEntityRecEffectIndex = nEntityRecEffectIndex;
            tmpIDUniquePropInfo.TargetUID = TargetUID;
            m_EffectAll.insert(pair<int,SPropInfo>(nEffectID, tmpIDUniquePropInfo));
            bAddSucesse = true;
        }
	}
	return bAddSucesse;
}


bool CMobaGoodsIDUniqueProp::RemoveEffect(int nSlotID, int nEffectID)
{
	// ��Ψһ�ж�
	T_MAP_ID_EFFECT_ALL::iterator iter = m_EffectAll.find(nEffectID);
	if (iter == m_EffectAll.end())
	{
		return false;
	}

    // �����Ƴ���λ��Ч�� ����
    if (iter->second.nSlot !=  nSlotID)
    {
        return false;
    }
	// �Ƴ�Ч��
    removeEntityGoodsEffect(iter->second.TargetUID, iter->second.nEntityRecEffectIndex);
	//�Ƴ�Ч��
	m_EffectAll.erase(iter);
	return true;
}

void CMobaGoodsIDUniqueProp::RemoveAllEffect()
{
	T_MAP_ID_EFFECT_ALL::iterator iter = m_EffectAll.begin();
	for (;iter != m_EffectAll.end();++iter)
	{
		// �Ƴ�Ч��
		removeEntityGoodsEffect(iter->second.TargetUID, iter->second.nEntityRecEffectIndex);
	}
	m_EffectAll.clear();
}
