/*******************************************************************
** 文件名:	EffectDamageCountAttackAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/04/2017
** 版  本:	1.0
** 描  述:	效果-伤害次数攻击增加buff
            http://172.16.0.120/redmine/issues/3232
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

class CEffectDamageCountAttackAddBuff : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_DamageCountAttackAddBuff    SCHEME_DATA;

	CEffectDamageCountAttackAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_nDamageCount(0),m_bAddBuff(false)
	{
        vector<string> vTemp;

        vTemp.clear();
        m_vSpellID.clear();
        StringHelper::split(vTemp, m_data.strSpellID, ';', " ");
        for (UINT nCount=0; nCount<vTemp.size(); ++nCount)
        {
            m_vSpellID.push_back(StringHelper::toInt(vTemp[nCount]));
        }

        vTemp.clear();
        m_vCountID.clear();
        StringHelper::split(vTemp, m_data.strCountID, ';', " ");
        for (UINT nCount=0; nCount<vTemp.size(); ++nCount)
        {
            m_vCountID.push_back(StringHelper::toInt(vTemp[nCount]));
        }
        // 次数数量
        int nCount = (int)m_vCountID.size();

        vTemp.clear();
        m_vBuffID.clear();
        StringHelper::split(vTemp, m_data.strBuffID, ';', " ");
        for (UINT nCount=0; nCount<vTemp.size(); ++nCount)
        {
            m_vBuffID.push_back(StringHelper::toInt(vTemp[nCount]));
        }
        // BUFFID数量
        int nBuffIDCount = (int)m_vBuffID.size();

        vTemp.clear();
        m_vBuffLevel.clear();
        StringHelper::split(vTemp, m_data.strBuffLevel, ';', " ");
        for (UINT nCount=0; nCount<vTemp.size(); ++nCount)
        {
            m_vBuffLevel.push_back(StringHelper::toInt(vTemp[nCount]));
        }
        // BUFFLevel数量
        int nBuffLevelCount = (int)m_vBuffLevel.size();
        if (nCount != nBuffIDCount || nBuffIDCount != nBuffLevelCount)
        {
            ErrorLn("CEffectDamageCountAttackAddBuff not match,ID="<< GetID() << ",nCount=" << nCount << ",nBuffIDCount=" << nBuffIDCount << ",nBuffLevelCount=" << nBuffLevelCount);
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

		// 注册伤害事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE, "CEffectDamageCountAttackAddBuff");

        // 注册攻击事件
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ATTACK, "CEffectDamageCountAttackAddBuff");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销伤害事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE);

            // 注销攻击事件
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ATTACK);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectDamageCountAttackAddBuff(m_data);
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
		case EVENT_ENTITY_DAMAGE:	// 伤害事件
			{
				if (m_pEntity == NULL)
				{
					break;
				}
				if (pszContext == NULL || nLen != sizeof(event_entity_damage))
				{
					break;
				}
				event_entity_damage *pDamage = (event_entity_damage*)pszContext;

                // 已增加buff,不再处理
                if (m_bAddBuff)
                {
                    break;
                }

                int nSpellEffectType = pDamage->nSpellEffectType;
                // 判断技能效果类型
                if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
                {
                    break;
                }

                int nDamageType = pDamage->nDamageType;
                // 判断技能效果类型
                if ((m_data.nDamageType & nDamageType) != m_data.nDamageType)
                {
                    break;
                }

                UID uidTarget = pDamage->uidOperator;
                // 检测目标目标
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
                {
                    break;
                }

                ++m_nDamageCount;
                if (m_nDamageCount >= m_data.nMaxCount)
                {
                    m_nDamageCount = m_data.nMaxCount;
                }
			}
			break;
        case EVENT_ENTITY_ATTACK:
            {
                if (m_pEntity == NULL)
                {
                    break;
                }
                if (pszContext == NULL || nLen != sizeof(event_entity_attack))
                {
                    break;
                }
                event_entity_attack *pAttack = (event_entity_attack*)pszContext;

                // 已增加buff,不再处理
                if (m_bAddBuff)
                {
                    break;
                }

                // 技能ID
                int nSpellID = pAttack->nID;
                vector<int>::iterator Iter = find(m_vSpellID.begin(), m_vSpellID.end(), nSpellID);
                if (Iter == m_vSpellID.end())
                {
                    break;
                }

                // 算出当前次数索引
                int nCount = -1;
                int nCountIDSize = (int)m_vCountID.size();
                for (int i=0; i<nCountIDSize - 1; ++i)
                {
                    if (m_nDamageCount >= m_vCountID[i] && m_nDamageCount <= m_vCountID[i+i])
                    {
                        nCount = i;
                        break;
                    }
                }
                if (nCount < 0 || nCount >= nCountIDSize)
                {
                    break;
                }

                if (nCount >= (int)m_vBuffID.size() || nCount >= (int)m_vBuffLevel.size())
                {
                    break;
                }

                int nBuffID = m_vBuffID[nCount];
                int nBuffLevel = m_vBuffLevel[nCount];
                int nBuffTime = m_nDamageCount*m_data.nBuffTime;

                // 给目标增加BUFF
                AddBuff(pAttack->uidTarget, nBuffID, nBuffLevel, nBuffTime);

                // 设置已增加
                m_bAddBuff = true;
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

    // 技能容器
    vector<int>				m_vSpellID;
    // 次数容器
    vector<int>				m_vCountID;
    // BuffID容器
    vector<int>				m_vBuffID;
    // BuffLevel容器
    vector<int>				m_vBuffLevel;

    // 伤害累积次数
    int                     m_nDamageCount;
    // 已增加buff
    bool                    m_bAddBuff;
};
