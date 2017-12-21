/*******************************************************************
** 文件名:	EffectInvisible
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	rjc
** 日  期:	2015/5/18
** 版  本:	1.0
** 描  述:	设置隐身属性
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"

class CEffectInvisible : public IEffectEx
{
public:
	typedef  EffectServer_Invisible    SCHEME_DATA;

	CEffectInvisible( SCHEME_DATA & data ) : m_data(data), m_pMaster(NULL)
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
			return false;

		m_pMaster = context->pEntity;
		__IEntityProperty * property = (__IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
		if(property != NULL)
		{
			int nMask = property->getProperty_Integer( PROPERTY_MASK );
			if (nMask & MASKFLAG_HIDE_ENEMY)
			{
				return false;
			}
			nMask |= MASKFLAG_HIDE_ENEMY;
			if ( m_data.bHideFriend )
			{
				nMask |= MASKFLAG_HIDE_FRIEND;
			}
			property->setProperty_Integer( PROPERTY_MASK, nMask );
			return true;
		}	
		
		return false;
	}

	// 效果停止
	virtual void			Stop()
	{
        if(m_pMaster)
        {
            __IEntityProperty * property = (__IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
            if(property != NULL)
            {
                int nMask = property->getProperty_Integer( PROPERTY_MASK );
                if (nMask & MASKFLAG_HIDE_ENEMY)
                {
                    nMask &= ~MASKFLAG_HIDE_ENEMY;
                    nMask &= ~MASKFLAG_HIDE_FRIEND;
                    property->setProperty_Integer( PROPERTY_MASK, nMask );
                }
            }
        }
		m_pMaster = NULL;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectInvisible(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
	__IEntity *				  m_pMaster;
};
