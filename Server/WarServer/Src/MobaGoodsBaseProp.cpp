#include "MobaGoodsBaseProp.h"
#include "ISchemeCenter.h"
#include "IEffect.h"

CMobaGoodsBaseProp::CMobaGoodsBaseProp()
{

}

CMobaGoodsBaseProp::~CMobaGoodsBaseProp()
{
}

void CMobaGoodsBaseProp::Release()
{
    RemoveAllEffect();
	delete this;
}

bool CMobaGoodsBaseProp::AddEffect(int nSlotID, int nEffectID, UID OperatorUID, UID TargetUID)
{
	//基础属性效果添加规则
	if (nEffectID == 0)
	{
		return false;
	}

    int nEntityRecEffectID = AddEntityGoodsEffect(nSlotID, nEffectID, OperatorUID, TargetUID);
    if (nEntityRecEffectID == 0)
    {
        return false;
    }

    SPropInfo tmpBasePropInfo;
    tmpBasePropInfo.nSlot = nSlotID;
    tmpBasePropInfo.nEffectID = nEffectID;
    tmpBasePropInfo.nEntityRecEffectIndex = nEntityRecEffectID;
    tmpBasePropInfo.TargetUID = TargetUID;

    m_EffectAll.push_back(tmpBasePropInfo);

	return true;
}

void CMobaGoodsBaseProp::RemoveAllEffect()
{
	IEffectFactory *pEffectFactory = gServerGlobal->getEffectFactory();
	if(pEffectFactory == NULL)
	{
		return;
	}

	T_LIST_BASE_EFFECT_ALL::iterator iter =  m_EffectAll.begin();
	for (;iter != m_EffectAll.end();++iter)
	{
		removeEntityGoodsEffect(iter->TargetUID, iter->nEntityRecEffectIndex);
	}
	m_EffectAll.clear();
}

bool CMobaGoodsBaseProp::RemoveEffect(int nSlotID, int nEffectID)
{
	// 组唯一判定
	T_LIST_BASE_EFFECT_ALL::iterator iter = m_EffectAll.begin();
	for (;iter != m_EffectAll.end(); ++iter)
	{
		if (iter->nEffectID == nEffectID && iter->nSlot == nSlotID)
		{
			removeEntityGoodsEffect(iter->TargetUID, iter->nEntityRecEffectIndex);
			//移除效果
			m_EffectAll.erase(iter);
			return true;
		}
	}
	return false;
}
