/*******************************************************************
** 文件名:	EffectCDOverlay.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	11/11/2015
** 版  本:	1.0
** 描  述:	效果—多重冷却效果
            modify by qqy 策划自己保证这个效果FreezeID唯一性
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IMobaGoodsPart.h"
#include "MobaGoodsSchemeDef.h"
using namespace rkt;
using namespace EFFECT_SERVER;

class CEffectCDOverlay : public IEffectEx
{

public:
	typedef  EffectServer_MultipleCoolDown    SCHEME_DATA;

	CEffectCDOverlay( SCHEME_DATA & data ) 
        : m_data(data)

	{
        
	}
    // 获取效果ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context,CONDITION_CONTEXT *pConditionContext )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

		m_pEntity=context->pEntity;

		// 获取物品部件
		__IMobaGoodsPart *pMobaGoodsPart = (__IMobaGoodsPart *)m_pEntity->getEntityPart(PART_MOBAGOODS);
		if (pMobaGoodsPart == NULL)
		{
			return false;
		}
		__IFreezePart *pFreezePart = (__IFreezePart *)m_pEntity->getEntityPart(PART_FREEZE);
		if (pFreezePart == NULL)
		{
			return false;
		}
		IFreezeServer *pFreezeServer = gServerGlobal->getFreezeServer();
		if (pFreezeServer == NULL)
		{
			return false;
		}
		// 不是物品类型直接返回
		if(context->nSlotType!=SLOT_TYPE_GOODS)
		{
			return false;
		}
		int nSlotIndex=context->nSlotIndex;
		// 得到物品对象
		IMobaGoods* pMobaGoods=pMobaGoodsPart->getMobaGoods(SLOT_TYPE_GOODS,nSlotIndex);
		if(pMobaGoods==NULL)
		{
			return false;
		}

        bool bResetUseCount = m_data.nResetChargeCount <= 0 ? false:true;
        pMobaGoods->SetMultipleCoolDown(m_data.nInterval, m_data.nChargedCount, bResetUseCount, m_data.nInitalChargeCount);
		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectCDOverlay(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;
    int                     m_nBuffFreezeID;
    // 实体指针
    __IEntity				*m_pEntity;
};