#include "MobaEquipment.h"
#include "ISchemeCenter.h"
#include "IEffect.h"
#include "IMobaGoodsPart.h"
#include "MobaGoodsDef.h"
#include "MobaGoods.h"

CMobaEquipment::CMobaEquipment()
{

}



CMobaEquipment::~CMobaEquipment()
{

}

bool CMobaEquipment::CanUse()
{
	if (!CMobaGoods::CanUse())
	{
		return false;
	}

	return true;
}

bool CMobaEquipment::Use()
{
	return CMobaGoods::Use();
}

void CMobaEquipment::AddGoodsEffect()
{
	CMobaGoods::AddGoodsEffect();
}

void CMobaEquipment::RemoveGoodsEffect()
{
	CMobaGoods::RemoveGoodsEffect();
}

void CMobaEquipment::Initial(__IEntity * pPerson, int nSlotIndex, int nGoodID)
{
	CMobaGoods::Initial(pPerson, nSlotIndex, nGoodID);

	//添加技能处理
	SSchemeMobaGoods* peMobaGoods = CMobaGoods::GetSchemeInfo();
	if (peMobaGoods->nUseSkillID != 0)
	{
		__IEntity *pMaster = GetMaster();
		if (pMaster == NULL)
		{
			return;
		}

		__ISpellPart *pSpellPart = (__ISpellPart *)pMaster->getEntityPart(PART_SPELL);

		if (pSpellPart == nullptr)
		{
			return;
		}

		pSpellPart->addSpell(peMobaGoods->nUseSkillID, SLOT_TYPE_GOODS, SPELL_SLOT_MAX_COUNT, 0);
	}
}


bool CMobaEquipment::Remove()
{
	//移除技能处理
	SSchemeMobaGoods* pMobaGoods = CMobaGoods::GetSchemeInfo();
    if (pMobaGoods == NULL)
    {
        ErrorLn(__FUNCTION__" -- pSchemeMobaGoods == NULL SlotIndex ="<<GetSlotIndex()<<" GoodID = "<<GetGoodID());
        return false;
    }
	if (pMobaGoods->nUseSkillID != 0)
	{
		__IEntity *pMaster = GetMaster();
		if (pMaster == NULL)
		{
			return false;
		}

		__IMobaGoodsPart *pMobaGoodsPart = (__IMobaGoodsPart *)pMaster->getEntityPart(PART_MOBAGOODS);
		if (pMobaGoodsPart == NULL)
		{
			return false;
		}

		__ISpellPart *pSpellPart = (__ISpellPart *)pMaster->getEntityPart(PART_SPELL);
		if (pSpellPart == nullptr)
		{
			return false;
		}

		if (!pMobaGoodsPart->HaveOtherSameGoods( GetGoodID(), GetSlotIndex()))
		{
			pSpellPart->removeSpell(pMobaGoods->nUseSkillID);
		}
	}

	return CMobaGoods::Remove();
}

void CMobaEquipment::Release()
{
    // 特殊物品释放可以处理一些清除处理
    CMobaGoods::Release();
}

bool CMobaEquipment::ResetCanUseCount()
{
  return CMobaGoods::ResetCanUseCount();
}


