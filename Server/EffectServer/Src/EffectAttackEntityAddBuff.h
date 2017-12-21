/*******************************************************************
** 文件名:	EffectAttackEntityAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/04/2016
** 版  本:	1.0
** 描  述:	效果-攻击实体增加Buff
            http://172.16.0.120/redmine/issues/1660
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"

class CEffectAttackEntityAddBuff : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_AttackEntityAddBuff    SCHEME_DATA;

	CEffectAttackEntityAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0)
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

		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ATTACK, "CEffectAttackEntityAddBuff");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ATTACK);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectAttackEntityAddBuff(m_data);
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
        case EVENT_ENTITY_ATTACK:
            {
                if (m_pEntity == NULL)
                {
                    break;
                }
                UID uidSelf = m_pEntity->getUID();

                DWORD dwTick = getTickCountEx();
                if (dwTick < m_dwLastTick + m_data.nInterval)
                {
                    break;
                }

                __IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
                if (pBuffPart == NULL)
                {
                    break;
                }

                if (pszContext == NULL || nLen != sizeof(event_entity_attack))
                {
                    break;
                }
                event_entity_attack *pAttck = (event_entity_attack*)pszContext;

                UID uidTarget = pAttck->uidTarget;
                if (isInvalidUID(uidTarget))
                {
                    break;
                }

                int nType = UID_2_TYPE(uidTarget);
                switch (nType)
                {
                case TYPE_PLAYER_ROLE:
                    {
                        // 检测死亡目标
                        if (!g_EHelper.chooseTarget(m_pEntity, m_data.nValue, uidTarget))
                        {
                            return;
                        }
                    }
                    break;
                case TYPE_MONSTER:
                    {
                        // 取得怪物ID
                        int nMonsterID = getProperty_Integer(uidTarget, PROPERTY_ID);
                        if (nMonsterID == 0)
                        {
                            return;
                        }
                        if (nMonsterID != m_data.nValue)
                        {
                            return;
                        }
                    }
                    break;
                default:
                    {
                        return;
                    }
                    break;
                }
                
                if (m_data.nBuffID > 0 && m_data.nBuffLevel > 0)
                {
                    pBuffPart->Add(m_data.nBuffID, m_data.nBuffLevel, uidSelf);

                    m_dwLastTick = dwTick;
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

	// 上次时间
	DWORD					m_dwLastTick;
};
