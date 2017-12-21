/*******************************************************************
** 文件名:	EffectFlight.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/09/2015
** 版  本:	1.0
** 描  述:	效果-浮空效果
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;

class CEffectFlight : public IEffect
{
public:
	typedef  EffectClient_Flight    SCHEME_DATA;

	CEffectFlight( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

        m_pEntity = context->pEntity;

		cmd_effect_Flight flight;
		flight.fHeight = m_data.fHeight;
		flight.nResidenceTime = m_data.nResidenceTime;
		flight.nGoUpAcceleration = m_data.nGoUpAcceleration;
		flight.nDropAcceleration = m_data.nDropAcceleration;
        flight.nFlight = 1;
		// 向显示层播放浮空效果
		m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_EFFECT_FLIGHT, 0, 0, &flight, sizeof(flight));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            cmd_effect_Flight flight;
            flight.nFlight = 0;
            // 向显示层停止浮空效果
            //m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_EFFECT_FLIGHT, 0, 0, &flight, sizeof(flight));

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectFlight(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	// 取得效果ID
	virtual int				GetEffectID()
	{
		return m_data.nID;
	}

private:
	SCHEME_DATA               m_data;

    IEntity	                  *m_pEntity;
};
