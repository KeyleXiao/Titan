#include "MobaLeechdom.h"
#include "ISchemeCenter.h"
#include "IEffect.h"
#include "IMobaGoodsPart.h"
#include "MobaGoodsDef.h"
#include "MobaGoods.h"
#include "EntityHelper.h"

CMobaLeechdom::CMobaLeechdom()
{

}



CMobaLeechdom::~CMobaLeechdom()
{
}

bool CMobaLeechdom::CanUse()
{
	if (!CMobaGoods::CanUse())
	{
		return false;
	}
	return true;
}

bool CMobaLeechdom::Use()
{
	//ʹ����Ʒ
	return CMobaGoods::Use();
}

void CMobaLeechdom::AddGoodsEffect()
{
	CMobaGoods::AddGoodsEffect();
}

void CMobaLeechdom::RemoveGoodsEffect()
{
	CMobaGoods::RemoveGoodsEffect();
}

void CMobaLeechdom::Initial(__IEntity * pPerson, int nSlotIndex, int nGoodID)
{
	CMobaGoods::Initial(pPerson, nSlotIndex, nGoodID);
}

void CMobaLeechdom::Release()
{
    // ������Ʒ�ͷſ��Դ���һЩ�������
	CMobaGoods::Release();
}

bool CMobaLeechdom::Remove()
{
	return CMobaGoods::Remove();
}

bool CMobaLeechdom::ResetCanUseCount()
{
	return CMobaGoods::ResetCanUseCount();
}

