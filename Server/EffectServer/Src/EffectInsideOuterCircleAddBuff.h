/*******************************************************************
** 文件名:	EffectInsideOuterCircleAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭政林
** 日  期:	1/04/2016
** 版  本:	1.0
** 描  述:	效果-内外圈增加Buff
            http://172.16.0.120/redmine/issues/3240
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;

class CEffectInsideOuterCircleAddBuff : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_InsideOuterCircleAddBuff    SCHEME_DATA;

	CEffectInsideOuterCircleAddBuff( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_vTargetTile(0,0,0),m_uidTarget(0),m_nSpellID(0),m_dwLastTime(0)
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
        {
			return false;
        }
        m_pEntity = context->pEntity;

        switch (m_data.nType)
        {
        case USE_SPELL_POS_TILE:
            {
                m_vTargetTile = context->ptTargetTile;
            }
            break;
        case USE_SPELL_POS_SELF:
            {
                m_vTargetTile = m_pEntity->getLocation();
                m_uidTarget = m_pEntity->getUID();
            }
            break;
        case USE_SPELL_POS_TARGET:
            {
                UID uidTarget = context->uidTarget;
                if (isInvalidUID(uidTarget))
                {
                    return false;
                }

                m_vTargetTile = getLocation(uidTarget);
                m_uidTarget = uidTarget;
            }
            break;
        default:
            {
                ErrorLn("CEffectInsideOuterCircleAddBuff error nType="<<m_data.nType);
                return false;
            }
            break;
        }

        m_nSpellID = context->nID;

        m_dwLastTime = getTickCountEx();

        OnTimer(0);

        if(m_data.nCount >= 1)
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, m_data.nCount, "CEffectInsideOuterCircleAddBuff");
        }
        else
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, INFINITY_CALL, "CEffectInsideOuterCircleAddBuff");
        }

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity)
        {
            g_EHelper.KillTimer(0, this);

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectInsideOuterCircleAddBuff(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

    virtual void OnTimer( unsigned long dwTimerID )
    {
        if (m_pEntity == NULL)
        {
            return;
        }
        // 自己UID
        UID uidSelf = m_pEntity->getUID();

        // 不是技能点，取目标实时位置
        if (m_data.nType != USE_SPELL_POS_TILE)
        {
            m_vTargetTile = getLocation(m_uidTarget);
        }

        /** 取得实体
		*/
		int nSceneID = UID_2_SCENE(uidSelf);
		SceneServiceHelper sceneHelper(nSceneID);
		if ( sceneHelper.m_ptr==0 )
		{
			return;
		}

        AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
        // 使用k_nearestInSpell此函数，离目标最近在最前面
        int num = sceneHelper.m_ptr->k_nearestInSpell(m_vTargetTile, m_data.fOuterDistance, m_data.fOuterDistance, pProxyArray, 256, LAYER_ALL);
        if ( num<=0 )
        {
            return;
        }

        for ( int i=0;i<num;++i )
        {
            // 达到增加数量(0表示不限制数量)
            if (m_data.nAddBuffCount != 0 && (int)m_PlayerMap.size() >= m_data.nAddBuffCount)
            {
                break;
            }

            // 目标UID
			UID uidTarget = pProxyArray[i]->m_uid;
            if (isInvalidUID(uidTarget))
            {
                continue;
            }

            // 检测目标
            if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
            {
                continue;
            }

            // 取得目标位置
            Vector3 vTargetLoc = getLocation(uidTarget);
            // 算出距离
            float fDistance = (m_vTargetTile - vTargetLoc).getLength();

            // 在内圈的,不处理
            if (fDistance < m_data.fInsideDistance)
            {
                continue;
            }
            m_PlayerMap[uidTarget] = true;
        }

        DWORD dwTick = getTickCountEx();
        if (dwTick < m_dwLastTime + m_data.nAddTime)
        {
            return;
        }
        m_dwLastTime = dwTick;

        SBuffContext BuffContext;
        BuffContext.nID = m_nSpellID;
        // 增加BUFF数量
        int nAddBuffCount = 0;
        int nMaxAddBuffCount = m_data.nAddBuffCount == 0 ? EFFECT_CHOOSE_TARGET_COUNT : m_data.nAddBuffCount;
        UID uidMonsterAddBuff[MAX_INTEREST_OBJECT] = {0};
        DWORD dwMonsterCount = 0;
        for (map<UID, bool>::iterator Iter = m_PlayerMap.begin(); Iter != m_PlayerMap.end(); ++Iter)
        {
            UID uidTarget = Iter->first;
            if (isMonster(uidTarget))
            {
                uidMonsterAddBuff[dwMonsterCount++] = uidTarget;
            }
            else
            {
                // 增加buff
                AddBuff(uidTarget, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
            }
        }
        if (dwMonsterCount > 0)
        {
            g_EHelper.BatchAddBuff(uidMonsterAddBuff, dwMonsterCount, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
        }

        m_PlayerMap.clear();
    }

private:
	SCHEME_DATA             m_data;

    // 效果信息
    __IEntity               *m_pEntity;

    Vector3                 m_vTargetTile;

    UID                     m_uidTarget;

    int                     m_nSpellID;

    map<UID, bool>          m_PlayerMap;

    DWORD                   m_dwLastTime;
};
