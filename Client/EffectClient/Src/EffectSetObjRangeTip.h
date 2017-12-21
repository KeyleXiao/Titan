/*******************************************************************
** 文件名:	EffectSetObjRangeTip.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/29/2016
** 版  本:	1.0
** 描  述:	效果-设置小地图范围提示
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;

class CEffectSetObjRangeTip : public IEffect
{
public:
	typedef  EffectClient_SetObjRangeTip    SCHEME_DATA;

	CEffectSetObjRangeTip( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

        m_pEntity = context->pEntity;

		cmd_entity_SetObjRangeTip SetObjRangeTip;
		SetObjRangeTip.bAdd = 1;
		SetObjRangeTip.nObjID = m_pEntity->getUID();
		SetObjRangeTip.nRange = m_data.nRange;
		m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SET_OBJ_RANGE_TIP, 0, 0, &SetObjRangeTip, sizeof(SetObjRangeTip));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            cmd_entity_SetObjRangeTip SetObjRangeTip;
            SetObjRangeTip.bAdd = 0;
            SetObjRangeTip.nObjID = m_pEntity->getUID();
            SetObjRangeTip.nRange = m_data.nRange;
            m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SET_OBJ_RANGE_TIP, 0, 0, &SetObjRangeTip, sizeof(SetObjRangeTip));

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectSetObjRangeTip(m_data);
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
