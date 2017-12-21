/*******************************************************************
** 文件名:	EffectShowChooseTarget.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/22/2016
** 版  本:	1.0
** 描  述:	效果-显示选择目标
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;

class CEffectShowChooseTarget : public IEffect
{
public:
	typedef  EffectClient_ShowChooseTarget    SCHEME_DATA;

	CEffectShowChooseTarget( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

        m_pEntity = context->pEntity;

        cmd_creature_ShowChooseTarget data;
        data.nType = 1;                         // 类型(0隐藏,不为0显示)
        data.nSpellID = m_data.nSpellID;        // 后续技能ID
        data.nTargetType = m_data.nTargetType;  // 显示选择目标类型 SHOW_CHOOSE_TARGET_TYPE
        data.nDistance = m_data.nDistance;      // 距离

		m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SHOW_CHOOSE_TARGET, 0, 0, &data, sizeof(data));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            cmd_creature_ShowChooseTarget ShowChooseTarget;
            ShowChooseTarget.nType = 0;
            m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SHOW_CHOOSE_TARGET, 0, 0, &ShowChooseTarget, sizeof(ShowChooseTarget));

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectShowChooseTarget(m_data);
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

    IEntity                   *m_pEntity;
};
