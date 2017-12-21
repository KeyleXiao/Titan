/*******************************************************************
** 文件名:	EffectRoot.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	penglu
** 日  期:	2014/12/29
** 版  本:	1.0
** 描  述:	定身效果(不可移动)类声明
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 一个buff持续3秒钟，一个buff持续10秒钟，3秒钟的结束，因为结束的时候调用恢复移动，其实这时候是不能移动的，因为还有10秒的buff定身在
此处是特殊处理，只有ENTITY_TOVIEW_RESUME_MOVE中所有的元素为空时才可移动。为什么传buffID进去的原因，显示层是特别处理的
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_CLIENT;

class CEffectRoot : public IEffect
{
public:
	typedef  EffectClient_Root    SCHEME_DATA;

	CEffectRoot( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_nBuffID(0){}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

		m_pEntity = context->pEntity;
		m_nBuffID = context->nID;
		if (m_nBuffID <= 0)
		{
			ErrorLn("Effect Root m_nBuffID="<<m_nBuffID);
			return false;
		}

		// 告诉显示层停止移动
		m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_STOP_MOVE, m_nBuffID, 0, 0, 0);

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
				// 告诉显示层恢复移动
				m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_RESUME_MOVE, m_nBuffID, 0, 0, 0);
			}

			m_nBuffID = 0;
			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectRoot(m_data); }

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	// 取得效果ID
	virtual int				GetEffectID() { return m_data.nID; }

private:
	SCHEME_DATA             m_data;
	IEntity *				m_pEntity;
	int						m_nBuffID;
};
