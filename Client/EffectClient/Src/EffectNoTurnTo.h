/*******************************************************************
** 文件名:	EffectNoTurnTo.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/28/2015
** 版  本:	1.0
** 描  述:	效果-不可转向
一个buff持续3秒钟，一个buff持续10秒钟，3秒钟的结束，因为结束的时候调用恢复转向，其实这时候是不能转向的，
因为还有10秒的buff不能转向，在此处是特殊处理，只有所有的元素为空时才可移动。为什么传buffID进去的原因，显示层是特别处理的
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"

using namespace rkt;
using namespace EFFECT_CLIENT;

class CEffectNoTurnTo : public IEffect
{
public:
	typedef  EffectClient_NoTurnTo    SCHEME_DATA;

	CEffectNoTurnTo( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_nBuffID(0)
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
			ErrorLn("Effect NoTurnTo m_nBuffID="<<m_nBuffID);
			return false;
		}

		cmd_creature_NoTurnTo noTurnTo;
		noTurnTo.nType = 0;
		noTurnTo.nBuffID = m_nBuffID;
		// 告诉显示层播放冲锋
		m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_NOTURNTO, 0, 0, &noTurnTo, sizeof(noTurnTo));

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
				cmd_creature_NoTurnTo noTurnTo;
				noTurnTo.nType = 1;
				noTurnTo.nBuffID = m_nBuffID;
				// 告诉显示层恢复移动
				m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_NOTURNTO, m_nBuffID, 0, &noTurnTo, sizeof(noTurnTo));
			}

			m_nBuffID = 0;
			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectNoTurnTo(m_data);
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