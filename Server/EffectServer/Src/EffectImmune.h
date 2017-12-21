/*******************************************************************
** 文件名:	EffectImmune.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/26/2015
** 版  本:	1.0
** 描  述:	效果-免疫(无敌)
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IMovePart.h"
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "DamageDef.h"

class CEffectImmune : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_Immune    SCHEME_DATA;

	CEffectImmune( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
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

		m_pEntity = context->pEntity;

		// 注册准备伤害
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE, "CEffectImmune");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销准备伤害
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectImmune(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		switch (wEventID)
		{
		case EVENT_ENTITY_PREPARE_DAMAGE:
			{
				if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
				{
					return;
				}

				DamageCalcContext *pDamage = (DamageCalcContext *)pszContext;
				// 设置免疫
				pDamage->nAttackResultType |= AttackResult_Immune;
			}
			break;
		default:
			break;
		}
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;
};
