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
	//组唯一属性效果添加规则
	if (nEffectID == 0 )
	{
		return false;
	}

	bool bAddSucesse = false;
	// ID唯一判定
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
	// 组唯一判定
	T_MAP_ID_EFFECT_ALL::iterator iter = m_EffectAll.find(nEffectID);
	if (iter == m_EffectAll.end())
	{
		return false;
	}

    // 不是移除槽位的效果 不动
    if (iter->second.nSlot !=  nSlotID)
    {
        return false;
    }
	// 移除效果
    removeEntityGoodsEffect(iter->second.TargetUID, iter->second.nEntityRecEffectIndex);
	//移除效果
	m_EffectAll.erase(iter);
	return true;
}

void CMobaGoodsIDUniqueProp::RemoveAllEffect()
{
	T_MAP_ID_EFFECT_ALL::iterator iter = m_EffectAll.begin();
	for (;iter != m_EffectAll.end();++iter)
	{
		// 移除效果
		removeEntityGoodsEffect(iter->second.TargetUID, iter->second.nEntityRecEffectIndex);
	}
	m_EffectAll.clear();
}
