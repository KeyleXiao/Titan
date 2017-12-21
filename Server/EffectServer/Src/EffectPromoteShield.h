/*******************************************************************
** 文件名:	EffectPromoteShield.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	5/25/2016
** 版  本:	1.0
** 描  述:	效果-提升护盾值效果
需求描述：http://172.16.0.120/redmine/issues/1820
实现描述：增加护盾时，护盾提升
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "EventDef.h"

class CEffectPromoteShield : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_PromoteShield    SCHEME_DATA;

	CEffectPromoteShield( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
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

		// 注册增加护盾事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ADD_SHIELD, "CEffectPromoteShield");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销增加护盾事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ADD_SHIELD);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectPromoteShield(m_data);
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
		case EVENT_ENTITY_ADD_SHIELD:
			{
				if (m_pEntity == NULL)
				{
					return;
				}

				if (pszContext == NULL || nLen != sizeof(AddShield))
				{
					return;
				}

				AddShield *pShield = (AddShield *)pszContext;

                switch (m_data.nValueType)
                {
                case MA_VALUE_NUM:
                    {
                        pShield->fValue += m_data.fValue;
                    }
                    break;
                case MA_VALUE_PER:
                    {
                        pShield->fValue = pShield->fValue*(1 + m_data.fValue);
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectPromoteDamage error nValueType="<<m_data.nValueType);
                        return;
                    }
                    break;
                }
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
