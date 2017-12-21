/*******************************************************************
** 文件名:	EffectAddGoods.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/18/2016
** 版  本:	1.0
** 描  述:	效果-效果增加物品
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IMobaGoodsPart.h"
using namespace EFFECT_SERVER;

class CEffectAddGoods : public IEffectEx
{
public:
	typedef  EffectServer_AddGoods    SCHEME_DATA;

	CEffectAddGoods( SCHEME_DATA & data ) : m_data(data)
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
         
        __IMobaGoodsPart *pMobaGoodsPart = (__IMobaGoodsPart *)context->pEntity->getEntityPart(PART_MOBAGOODS);
        if (pMobaGoodsPart == NULL)
        {
            return false;
        }

        if (m_data.nAdd)
        {
            pMobaGoodsPart->effectAddGood(m_data.nGoodsID, m_data.nGoodsCount);
        }
        else
        {
            pMobaGoodsPart->effectRemoveGood(m_data.nGoodsID,m_data.nGoodsCount);
        }

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectAddGoods(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
};
