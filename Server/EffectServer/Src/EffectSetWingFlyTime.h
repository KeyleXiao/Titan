/*******************************************************************
** 文件名:	EffectSetWingFlyTime.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/19/2016
** 版  本:	1.0
** 描  述:	效果-设置翅膀飞行时间
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;

class CEffectSetWingFlyTime : public IEffectEx
{
public:
	typedef  EffectServer_SetWingFlyTime    SCHEME_DATA;

	CEffectSetWingFlyTime( SCHEME_DATA & data ) : m_data(data)
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
			return false;
		}

		__IWingPart *pWing = (__IWingPart * )context->pEntity->getEntityPart(PART_WING);
		if (pWing == 0)
		{
			return false;
		}
        switch (m_data.nChangeType)
        {
        case MA_CHANGE_ADD:
            {
                // 增加飞行时间
                pWing->AddFlyTime(m_data.nValue);
            }
            break;
        case MA_CHANGE_SET:
            {
                // 设置飞行时间
                pWing->SetFlyTime(m_data.nValue);
            }
            break;
        default:
            {
                return false;
            }
            break;
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
		return new CEffectSetWingFlyTime(m_data);
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
