/*******************************************************************
** 文件名:	EffectAttackAddCRD.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/02/2016
** 版  本:	1.0
** 描  述:	效果-攻击增加暴击概率
            http://172.16.0.120/redmine/issues/1660
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"

class CEffectAttackAddCRD : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_AttackAddCRD    SCHEME_DATA;

	CEffectAttackAddCRD( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0),m_nAddCRD(0)
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

		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ATTACK_EX, "CEffectAttackAddCRD");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            if (m_nAddCRD != 0)
            {
                __IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
                if (pEntityProperty != NULL)
                {
                    pEntityProperty->addProperty_Integer(PROPERTY_APPEND_CRC, -m_nAddCRD);
                }
            }
            m_nAddCRD = 0;
            
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ATTACK_EX);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectAttackAddCRD(m_data);
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
        case EVENT_ENTITY_ATTACK_EX:
            {
                if (m_pEntity == NULL)
                {
                    break;
                }

                __IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
                if (pEntityProperty == NULL)
                {
                    break;
                }

                DWORD dwTick = getTickCountEx();
                if (dwTick < m_dwLastTick + m_data.nInterval)
                {
                    break;
                }

                if (pszContext == NULL || nLen != sizeof(event_entity_attack_ex))
                {
                    break;
                }
                event_entity_attack_ex *pAttck = (event_entity_attack_ex*)pszContext;

                int	nSpellEffectType = pAttck->nSpellEffectType;
                // 技能效果类型匹配
                if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
                {
                    break;
                }

                // 范围伤害不是首目标，忽略
                if (pAttck->nAttackCount != 0)
                {
                    break;
                }
                if (pAttck->nCRD >= 0.01f)
                {
                    // 减少暴击概率,进入冷却
                    pEntityProperty->addProperty_Integer(PROPERTY_APPEND_CRC, -m_nAddCRD);
                    m_nAddCRD = 0;
                    // 设置冷却
                    m_dwLastTick = dwTick;
                    break;
                }

                if (m_nAddCRD >= m_data.nLimitValue)
                {
                    break;
                }

                int nAddCRD = m_data.nValue;
                if (m_nAddCRD + m_data.nValue >= m_data.nLimitValue)
                {
                    nAddCRD = m_data.nLimitValue - m_nAddCRD;
                }

                // 增加暴击概率
                pEntityProperty->addProperty_Integer(PROPERTY_APPEND_CRC, nAddCRD);
                m_nAddCRD += nAddCRD;
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

    // 增加暴击概率
    int                     m_nAddCRD;
};
