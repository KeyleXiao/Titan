/*******************************************************************
** 文件名:	EffectMatchingSpellAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/04/2016
** 版  本:	1.0
** 描  述:	效果-匹配技能增加Buff
            http://172.16.0.120/redmine/issues/1660
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "DamageDef.h"
#include "EntityHelper.h"

class CEffectMatchingSpellAddBuff : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_MatchingSpellAddBuff    SCHEME_DATA;

	CEffectMatchingSpellAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0)
	{
        vector<string> tempVector;
        tempVector.clear();

        m_SpellIDVector.clear();
        StringHelper::split(tempVector, m_data.strSpellID, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_SpellIDVector.push_back(StringHelper::toInt(tempVector[nCount]));
        }
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

		g_EHelper.Subscibe(m_pEntity, this, (EventKey)m_data.nType, "CEffectMatchingSpellAddBuff");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)m_data.nType);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectMatchingSpellAddBuff(m_data);
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
		case EVENT_ENTITY_CAST_SPELL:
			{
				if (m_pEntity == NULL)
				{
					break;
				}

				DWORD dwTick = getTickCountEx();
				if (dwTick < m_dwLastTick + m_data.nInterval)
				{
					break;
				}

				if (pszContext == NULL || nLen != sizeof(event_cast_spell))
				{
					break;
				}
				event_cast_spell *pCastSpell = (event_cast_spell*)pszContext;

                bool bFind = false;
                int nCount = (int)m_SpellIDVector.size();
                for (int i=0; i<nCount; ++i)
                {
                    if (pCastSpell->nID == m_SpellIDVector[i])
                    {
                        bFind = true;
                        break;
                    }
                }
                if (!bFind)
                {
                    break;
                }

                UID uidSelf = m_pEntity->getUID();
                if (m_data.nBuffID > 0 && m_data.nBuffLevel > 0)
                {
                    SBuffContext BuffContext;
                    BuffContext.nID = pCastSpell->nID;
                    AddBuff(uidSelf, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));

                    m_dwLastTick = dwTick;
                }
			}
			break;
        case EVENT_ENTITY_ATTACK:
            {
                if (m_pEntity == NULL)
                {
                    break;
                }

                DWORD dwTick = getTickCountEx();
                if (dwTick < m_dwLastTick + m_data.nInterval)
                {
                    break;
                }

                if (pszContext == NULL || nLen != sizeof(event_entity_attack))
                {
                    break;
                }
                event_entity_attack *pAttck = (event_entity_attack*)pszContext;

                bool bFind = false;
                int nCount = (int)m_SpellIDVector.size();
                for (int i=0; i<nCount; ++i)
                {
                    if (pAttck->nID == m_SpellIDVector[i])
                    {
                        bFind = true;
                        break;
                    }
                }
                if (!bFind)
                {
                    break;
                }

                UID uidSelf = m_pEntity->getUID();
                if (m_data.nBuffID > 0 && m_data.nBuffLevel > 0)
                {
                    SBuffContext BuffContext;
                    BuffContext.nID = pAttck->nID;
                    AddBuff(uidSelf, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));

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

    // SpellID容器
    vector<int>				m_SpellIDVector;
};
