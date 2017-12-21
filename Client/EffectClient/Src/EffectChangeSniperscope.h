/*******************************************************************
** 文件名:	EffectChangeSniperscope.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	10/09/2016
** 版  本:	1.0
** 描  述:	效果-改变狙击镜
            http://172.16.0.120/redmine/issues/2420
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;

class CEffectChangeSniperscope : public IEffect
{
public:
	typedef EffectClient_ChangeSniperscope SCHEME_DATA;

	CEffectChangeSniperscope( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT *pContext )
	{
		if ( pContext==0 || pContext->pEntity==0 )
			return false;

        m_pEntity = pContext->pEntity;

        cmd_Entity_Change_Sniperscope ChangeSniperscope;
        ChangeSniperscope.nType = 1;
        ChangeSniperscope.fFOV = m_data.fFOV;
        m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_CHANGE_SNIPERSCOPE, 0, 0, &ChangeSniperscope, sizeof(ChangeSniperscope));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            cmd_Entity_Change_Sniperscope ChangeSniperscope;
            ChangeSniperscope.nType = 0;
            ChangeSniperscope.fFOV = m_data.fFOV;
            m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_CHANGE_SNIPERSCOPE, 0, 0, &ChangeSniperscope, sizeof(ChangeSniperscope));

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectChangeSniperscope(m_data); }

	// 取得效果ID
	virtual int				GetEffectID(){ return m_data.nID; }

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;

    IEntity                 *m_pEntity;
};
