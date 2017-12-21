#include "MobaGoodsProp.h"
#include "ISchemeCenter.h"
#include "IEffect.h"
#include "EntityHelper.h"

CMobaGoodsProp::CMobaGoodsProp()
{

}

CMobaGoodsProp::~CMobaGoodsProp()
{
}

void CMobaGoodsProp::Release()
{
    RemoveAllEffect();
	delete this;
}

bool CMobaGoodsProp::AddEffect(int nSlotID, int nEffectID, UID OperatorUID, UID TargetUID)
{
	return true;
}

bool CMobaGoodsProp::RemoveEffect(int nSlotID, int nEffectID)
{
	//��ӹ��� ����ȥʵ��
	return true;
}


void CMobaGoodsProp::RemoveAllEffect()
{
	return;
}

SSchemeGoodEffectGroupType* CMobaGoodsProp::GetSchemeInfo(int nEffectID)
{
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if(pSchemeCenter == NULL)
	{
		return nullptr;
	}
	ISchemeMobaGoods * pSchemeMobaGoods = pSchemeCenter->getSchemeMobaGoods();
	if(pSchemeMobaGoods == NULL)
	{
		return nullptr;
	}

	return pSchemeMobaGoods->getSchemeMobaGoodsPropInfo(nEffectID);
	
}

int CMobaGoodsProp::AddEntityGoodsEffect(int nSlotID, int nEffectID, UID OperatorUID, UID TargetUID)
{
    EFFECT_CONTEXT context;
    context.uidOperator = OperatorUID;
    context.uidTarget = TargetUID;
    context.nSlotIndex=nSlotID;
    context.nSlotType=SLOT_TYPE_GOODS;

    return mobaGoodsAddEffect(TargetUID, nEffectID, &context, sizeof(context));
}

void CMobaGoodsProp::removeEntityGoodsEffect(UID TargetUID, int nEntityRecEffectIndex)
{
    mobaGoodsRemoveEffect(TargetUID, nEntityRecEffectIndex);
}
