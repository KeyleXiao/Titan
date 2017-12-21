/*******************************************************************
** 文件名:	EffectAttackCountAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/01/2016
** 版  本:	1.0
** 描  述:	效果-攻击次数增加Buff
            http://172.16.0.120/redmine/issues/2200
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"

class CEffectAttackCountAddBuff : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_AttackCountAddBuff    SCHEME_DATA;

	CEffectAttackCountAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_nAttackCount(0),m_uidAttackTarget(0),m_nLastAddBuffID(0),m_nLastAddBuffLevel(0),m_dwLastTick(0),m_dwLastAttackTick(0)
	{
        vector<string> vTemp;
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
        if (m_data.nCount != nBuffIDCount || m_data.nCount != nBuffLevelCount)
        {
            ErrorLn("CEffectAttackCountAddBuff not match,ID="<< GetID() <<",nCombnCountoCount="<<m_data.nCount<<",nBuffIDCount="<<nBuffIDCount<<",nBuffLevelCount="<<nBuffLevelCount);
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

		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ATTACK_EX, "CEffectAttackCountAddBuff");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ATTACK_EX);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectAttackCountAddBuff(m_data);
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

                if (pszContext == NULL || nLen != sizeof(event_entity_attack_ex))
                {
                    break;
                }
                event_entity_attack_ex *pAttck = (event_entity_attack_ex*)pszContext;

                // 范围伤害不是首目标，忽略
                if (pAttck->nAttackCount != 0)
                {
                    break;
                }

                UID uidSelf = m_pEntity->getUID();
                UID uidTarget = pAttck->uidTarget;

                int nSpellEffectType = pAttck->nSpellEffectType;
                // 判断技能效果类型
                if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
                {
                    break;
                }

                // 检测目标目标
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
                {
                    break;
                }

                // 不是上次目标攻击次数清零
                if (uidTarget != m_uidAttackTarget)
                {
                    // 不是目标无效
                    if (!isInvalidUID(m_uidAttackTarget))
                    {
                        if (m_nLastAddBuffID > 0 && m_nLastAddBuffLevel > 0)
                        {
                            RemoveBuff(m_uidAttackTarget, m_nLastAddBuffID, uidSelf, uidSelf);

                            m_nLastAddBuffID = m_nLastAddBuffLevel = 0;
                        }
                    }

                    m_nAttackCount = 0;
                }

                // 攻击超时 次数清零
                if(dwTick > m_dwLastAttackTick + m_data.nValidAttackInterval)
                {
                    m_nAttackCount = 0;
                }

                // 取得当前BUFF列表
                if (m_nAttackCount < (int)m_vBuffID.size() && m_nAttackCount < (int)m_vBuffLevel.size())
                {
                    int nBuffID = m_vBuffID[m_nAttackCount];
                    int nBuffLevel = m_vBuffLevel[m_nAttackCount];
                    if (nBuffID > 0 && nBuffLevel > 0)
                    {
                        SBuffContext BuffContext;
                        BuffContext.nID = pAttck->nSpellID;
                        AddBuff(uidTarget, nBuffID, nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));

                        m_uidAttackTarget = uidTarget;
                        m_nLastAddBuffID = nBuffID;
                        m_nLastAddBuffLevel = nBuffLevel;
                    }
                }

                ++m_nAttackCount;
                m_dwLastAttackTick = dwTick;

                // 达到BUFF上限
                if (m_nAttackCount >= m_data.nCount)
                {
                    m_nAttackCount = 0;

                    m_dwLastTick = dwTick;

                    m_uidAttackTarget = 0;
                    m_nLastAddBuffID = m_nLastAddBuffLevel = 0;
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

    // BuffID容器
    vector<int>				m_vBuffID;
    // BuffLevel容器
    vector<int>				m_vBuffLevel;

    // 当前攻击次数
    int                     m_nAttackCount;
    // 当前攻击目标
    UID                     m_uidAttackTarget;
    // 上次增加BUFFID
    int                     m_nLastAddBuffID;
    // 上次增加BUFFLevel
    int                     m_nLastAddBuffLevel;

    // 上次冷却时间
    DWORD                   m_dwLastTick;

    // 上次攻击时刻
    DWORD                   m_dwLastAttackTick;
};
