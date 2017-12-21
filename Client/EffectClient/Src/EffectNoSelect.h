/*******************************************************************
** 文件名:	EffectNoSelect.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/29/2016
** 版  本:	1.0
** 描  述:	效果-不可选择
一个buff持续3秒钟，一个buff持续10秒钟，3秒钟的结束，因为结束的时候调用恢复选择，其实这时候是不能选择的，
因为还有10秒的buff不能选择，在此处是特殊处理，只有所有的元素为空时才可选择。为什么传buffID进去的原因，显示层是特别处理的
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"

using namespace rkt;
using namespace EFFECT_CLIENT;

class CEffectNoSelect : public IEffect
{
public:
	typedef  EffectClient_NoSelect    SCHEME_DATA;

	CEffectNoSelect( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_nBuffID(0)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

		m_pEntity = context->pEntity;
		m_nBuffID = context->nID;
		if (m_nBuffID <= 0)
		{
			ErrorLn("Effect NoSelect m_nBuffID="<<m_nBuffID);
			return false;
		}

		cmd_creature_NoSelect cmd_data;
        cmd_data.nType = 0;
        cmd_data.nBuffID = m_nBuffID;
        cmd_data.nSelectTargetFilter = m_data.nSelectTargetFilter;

		// 告诉显示层播放冲锋
		m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_NO_SELECT, 0, 0, &cmd_data, sizeof(cmd_data));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// BuffID对，才发送命令
			if (m_nBuffID > 0)
			{
				cmd_creature_NoSelect NoSelect;
				NoSelect.nType = 1;
				NoSelect.nBuffID = m_nBuffID;
				// 告诉显示层恢复移动
				m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_NO_SELECT, m_nBuffID, 0, &NoSelect, sizeof(NoSelect));
			}

			m_nBuffID = 0;
			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectNoSelect(m_data);
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
	SCHEME_DATA             m_data;
	IEntity *				m_pEntity;
	int						m_nBuffID;
};