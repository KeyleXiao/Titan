#include "MobaGoodsGroupUniqueProp.h"
#include "ISchemeCenter.h"
#include "IEffect.h"

CMobaGoodsGroupUniqueProp::CMobaGoodsGroupUniqueProp()
{

}

CMobaGoodsGroupUniqueProp::~CMobaGoodsGroupUniqueProp()
{
	
}

void CMobaGoodsGroupUniqueProp::Release()
{
    RemoveAllEffect();
	delete this;
}

bool CMobaGoodsGroupUniqueProp::AddEffect(int nSlotID, int nEffectID, UID OperatorUID, UID TargetUID)
{
	//��Ψһ����Ч����ӹ���
	if (nEffectID == 0)
	{
		return false;
	}

	bool bAddSucesse = false;

	// ��Ψһ�ж�
	SSchemeGoodEffectGroupType *pGoodEffectGroupType = CMobaGoodsProp::GetSchemeInfo(nEffectID);
	if (pGoodEffectGroupType == NULL)
	{
		ErrorLn("CMobaGoodsGroupUniqueProp::AddEffect(int nSlotID, int nEffectID) can't find pGoodEffectGroupType nEffectID = "<< nEffectID);
		return bAddSucesse;
	}
	
	T_MAP_GROUP_EFFECT_ALL::iterator iter = m_EffectAll.find(pGoodEffectGroupType->nEffectGroupID);
	if (iter == m_EffectAll.end())
	{

		int nEntityRecEffectIndex = AddEntityGoodsEffect(nSlotID, nEffectID, OperatorUID, TargetUID);
        if (nEntityRecEffectIndex == 0)
        {
            bAddSucesse = false;
        }
        else
        {
            SPropInfo tmpGroupUniquePropInfo;
            tmpGroupUniquePropInfo.nEffectID = nEffectID;
            tmpGroupUniquePropInfo.nSlot = nSlotID;
            tmpGroupUniquePropInfo.nEntityRecEffectIndex = nEntityRecEffectIndex;
            tmpGroupUniquePropInfo.TargetUID = TargetUID;
            m_EffectAll.insert(pair<int,SPropInfo>(pGoodEffectGroupType->nEffectGroupID,tmpGroupUniquePropInfo));
            bAddSucesse = true;
        }
	}

	return bAddSucesse;
}

bool CMobaGoodsGroupUniqueProp::RemoveEffect(int nSlotID, int nEffectID)
{
	// ��Ψһ�ж�
	SSchemeGoodEffectGroupType *pGoodEffectGroupType = CMobaGoodsProp::GetSchemeInfo(nEffectID);

	if (pGoodEffectGroupType == NULL)
	{
		ErrorLn("CMobaGoodsGroupUniqueProp::RemoveEffect(int nSlotID, int nEffectID) can't find pGoodEffectGroupType nEffectID = "<< nEffectID);
		return false;
	}

	T_MAP_GROUP_EFFECT_ALL::iterator iter = m_EffectAll.find(pGoodEffectGroupType->nEffectGroupID);
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

void CMobaGoodsGroupUniqueProp::RemoveAllEffect()
{
	T_MAP_GROUP_EFFECT_ALL::iterator iter = m_EffectAll.begin();
	for (;iter != m_EffectAll.end();++iter)
	{
		// �Ƴ�Ч��
		removeEntityGoodsEffect(iter->second.TargetUID, iter->second.nEntityRecEffectIndex);
	}
	m_EffectAll.clear();
}
