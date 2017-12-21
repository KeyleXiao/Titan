/*******************************************************************
** 文件名:	EffectImmuneDrains.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/16/2015
** 版  本:	1.0
** 描  述:	效果-免疫吸血

-------------------------------------------------------------------
修改：http://172.16.0.120/redmine/issues/6357
      现在把这个扩展成免疫计算数值的一个效果
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "EventDef.h"
#include "IEntityEvent.h"

using namespace rkt;
using namespace EFFECT_SERVER;

class CEffectImmuneEffect : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_ImmuneEffect    SCHEME_DATA;

    CEffectImmuneEffect( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
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

        // 订阅 准备伤害事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE, "EffectImmuneDrains");
        // 订阅 准备计算伤害事件
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_CACL_DAMAGE, "EffectImmuneDrains");


		return true;
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
		case EVENT_ENTITY_PREPARE_DAMAGE:           // 准备伤害事件
			{
				if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
				{
					return;
				}
				DamageCalcContext *pDamage = (DamageCalcContext *)pszContext;

				// 免疫吸血
				if(m_data.nType == IMMUNE_EFFECT_TYPE_DRAINS)
				{
                    // 设置免疫吸血
                    pDamage->bImmuneDrains = true;
				}
			}
			break;

        case EVENT_ENTITY_PREPARE_CACL_DAMAGE:      // 准备计算伤害事件
            {
                if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
                {
                    return;
                }
                DamageCalcContext *pDamage = (DamageCalcContext *)pszContext;

                // 免疫暴击
                if (m_data.nType == IMMUNE_EFFECT_TYPE_CRIT)
                {
                    pDamage->nCRD = 0;      // 暴击伤害
                }
            }
            break;

		default:
			break;
		}
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // 取消订阅 准备伤害事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE);
            // 取消订阅 准备计算伤害事件
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_CACL_DAMAGE);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectImmuneEffect(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;
};