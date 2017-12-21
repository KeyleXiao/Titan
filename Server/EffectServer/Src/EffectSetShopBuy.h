/*******************************************************************
** 文件名:	EffectSetShopBuy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/02/2015
** 版  本:	1.0
** 描  述:	效果-设置商城购买
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"
#include "IMobaGoodsPart.h"

class CEffectSetShopBuy : public IEffectEx
{
public:
	typedef  EffectServer_SetShopBuy    SCHEME_DATA;

	CEffectSetShopBuy( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
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
		if( context==0 || context->pEntity==0 )
		{
            
            ErrorLn(__FUNCTION__": uidTarget ="<<context->uidTarget);
			return false;
		}
		m_pEntity = context->pEntity;

		__IMobaGoodsPart *pMobaGoodsPart = (__IMobaGoodsPart *)m_pEntity->getEntityPart(PART_MOBAGOODS);
		if (pMobaGoodsPart == NULL)
		{
            ErrorLn(__FUNCTION__": pMobaGoodsPart == NULL uidTarget ="<<context->uidTarget);
			return false;
		}
		bool bBuy = false;
		if (m_data.nBuy == MA_TRUE)
		{
			bBuy = true;
		}
		pMobaGoodsPart->SetCanBuyOrSell(bBuy);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			__IMobaGoodsPart *pMobaGoodsPart = (__IMobaGoodsPart *)m_pEntity->getEntityPart(PART_MOBAGOODS);
			if (pMobaGoodsPart == NULL)
			{
                ErrorLn(__FUNCTION__": pMobaGoodsPart == NULL UID ="<<m_pEntity->getUID()<< "effectID ="<<GetID() );
				return;
			}
			bool bBuy = false;
			if (m_data.nBuy == MA_TRUE)
			{
				bBuy = true;
			}
			pMobaGoodsPart->SetCanBuyOrSell(!bBuy);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectSetShopBuy(m_data);
	}

	// 释放
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;

	__IEntity				  *m_pEntity;
};
