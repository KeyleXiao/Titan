/*******************************************************************
** 文件名:	EffectDieAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	5/06/2015
** 版  本:	1.0
** 描  述:	效果-死亡增加Buff
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "EntityHelper.h"
#include "ExternalHelper.h"

class CEffectDieAddBuff : public IEffectEx, public IEventExecuteSink, public TimerHandler
{
public:
	typedef  EffectServer_DieAddBuff    SCHEME_DATA;

	CEffectDieAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTime(0),m_uidAdd(0),m_nSpellID(0)
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

        m_nSpellID = context->nID;

        __IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
        if (pEntityProperty == NULL)
        {
            return false;
        }

        switch (m_data.nAddTargetType)
        {
        case MA_TAGET_TO_SELF:
            {
                m_uidAdd = m_pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                m_uidAdd = context->uidOperator;
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                m_uidAdd = context->uidTarget;
            }
            break;
        case MA_TAGET_TO_ALL:
            {
                m_uidAdd = INVALID_UID;
            }
            break;
        default:
            {
                ErrorLn("CEffectDieAddBuff error nAddTargetType="<<m_data.nAddTargetType);
                return false;
            }
            break;
        }

		// 注册实体死亡事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_DIE, "CEffectDieAddBuff");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // 删除定时器
            g_EHelper.KillTimer(0, this);

			// 注销实体死亡事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DIE);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectDieAddBuff(m_data);
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
		case EVENT_ENTITY_DIE:
			{
                if (pszContext == NULL || nLen != sizeof(event_entity_die))
                {
                    break;
                }
                event_entity_die *pDie = (event_entity_die *)pszContext;

				// 取得当前时间
				DWORD dwTick = getTickCountEx();
				if (dwTick < m_dwLastTime + m_data.nInterval)
				{
					break;
				}

                // 延迟释放
                if (m_data.nDelay > 0)
                {
                    g_EHelper.SetTimer(0, m_data.nDelay, this, 1, "CEffectDieAddBuff");
                }
                else
                {
                    OnTimer(0);
                }

				// 设置上次时间
				m_dwLastTime = dwTick;
			}
			break;
		default:
			break;
		}
	}

    virtual void OnTimer( unsigned long dwTimerID )
    {
        if (m_pEntity == NULL)
        {
            return;
        }

		UID uidSelf = m_pEntity->getUID();
		int nSceneID = UID_2_SCENE(uidSelf);
		SceneServiceHelper sceneHelper(nSceneID);
		if ( sceneHelper.m_ptr==0 )
		{
			return;
		}
        
		AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
		int num = sceneHelper.m_ptr->k_nearest(m_pEntity->getLocation(), m_data.fDistance, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL, true);
        if ( num <= 0 )
        {
            return;
        }

        SBuffContext BuffContext;
        BuffContext.nID = m_nSpellID;

        // 增加BUFF数量
        int nAddBuffCount = 0;
        int nMaxAddBuffCount = m_data.nAddBuffCount == 0 ? EFFECT_CHOOSE_TARGET_COUNT : m_data.nAddBuffCount;
        UID uidMonsterAddBuff[MAX_INTEREST_OBJECT] = {0};
        DWORD dwMonsterCount = 0;
        for ( int i=0; i<num; ++i )
        {
            // 目标UID
			UID uidTarget = pProxyArray[i]->m_uid;
            if (isInvalidUID(uidTarget))
            {
                continue;
            }

            if (nAddBuffCount >= nMaxAddBuffCount)
            {
                break;
            }

            // 检测目标
            if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
            {
                continue;
            }

            if (isMonster(uidTarget))
            {
                uidMonsterAddBuff[dwMonsterCount++] = uidTarget;
            }
            else
            {
                // 增加buff
                AddBuff(uidTarget, m_data.nBuffID, m_data.nBuffLevel, m_uidAdd, 0, &BuffContext, sizeof(BuffContext));
            }

            ++nAddBuffCount;
        }

        if (dwMonsterCount > 0)
        {
            g_EHelper.BatchAddBuff(uidMonsterAddBuff, dwMonsterCount, m_data.nBuffID, m_data.nBuffLevel, m_uidAdd, 0, &BuffContext, sizeof(BuffContext));
        }
    }

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

	// 上次时间
	DWORD					m_dwLastTime;

    // 增加BUFF者
    UID						m_uidAdd;

    int                     m_nSpellID;
};
