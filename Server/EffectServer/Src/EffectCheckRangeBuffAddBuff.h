/*******************************************************************
** 文件名:	EffectCheckRangeBuffAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/19/2016
** 版  本:	1.0
** 描  述:	效果-检测范围buff标识增加buff
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "TimerHandler.h"
#include "IServerGlobal.h"
#include "EntityHelper.h"
#include "ICamp.h"
#include "IEntityAOI.h"

using namespace EFFECT_SERVER;

class CEffectCheckRangeBuffAddBuff : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_CheckRangeBuffAddBuff    SCHEME_DATA;

	CEffectCheckRangeBuffAddBuff( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL),m_vTargetTile(0,0,0),m_uidTarget(0),m_nSpellID(0)
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
		if (context == NULL || context->pEntity == NULL)
		{
			return false;
		}

		m_pEntity = context->pEntity;

        m_nSpellID = context->nID;

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
                ErrorLn("CEffectCheckRangeBuffAddBuff error nType="<<m_data.nType);
                return false;
            }
            break;
        }

        // 首次执行，也算次数
        int nCount = 0;
        // 首次立即执行
        if (m_data.nFirst > 0)
        {
            OnTimer(0);

            nCount = 1;
        }

        if(m_data.nCount >= 1)
        {
            int nTimerCount = m_data.nCount - nCount;
            if (nTimerCount <= 0)
            {
                return true;
            }

            g_EHelper.SetTimer(0, m_data.nInterval, this, nTimerCount, "CEffectCheckDistanceAddBuff");
        }
        else
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, INFINITY_CALL, "CEffectCheckDistanceAddBuff");
        }

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != NULL)
		{
			g_EHelper.KillTimer(0, this);

			m_pEntity = NULL;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectCheckRangeBuffAddBuff(m_data);
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
		// 死亡状态，不再加范围BUFF
		if (m_pEntity->getState() == ENTITY_STATE_DIE)
		{
			return;
		}

        // 自己UID
        UID uidSelf = m_pEntity->getUID();
		Vector3 vSelfLoc = m_pEntity->getLocation();

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
        int num = sceneHelper.m_ptr->k_nearest(m_vTargetTile, m_data.fDistance, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL, true);
        if ( num<=0 )
        {
            return;
        }

        SBuffContext BuffContext;
        BuffContext.nID = m_nSpellID;

        // 被加BUFF人数
        int nActorAddBuffCount = 0;
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

            // 取得目标位置
			Vector3 vTargetLoc = pProxyArray[i]->m_Loc;

            // 检测是否在范围内
            bool bInside = true;
            if (m_data.nRangeType == CHECK_TARGET_RANGE_OUTSIDE)
            {
                bInside = false;
            }
            if (vSelfLoc.checkDistance(vTargetLoc, m_data.fDistance) != bInside)
            {
                continue;
            }

            if (isMonster(uidTarget))
            {
                uidMonsterAddBuff[dwMonsterCount++] = uidTarget;
            }
            else
            {
                // 无标识(非怪物)
                if (!IsExistBuffType(uidTarget, m_data.dwBuffType))
                {
                    continue;
                }
			    // 增加buff
			    AddBuff(uidTarget, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
                // 汇总给人加BUFF数量
                if(isActor(uidTarget))
                {
                    ++nActorAddBuffCount;
                }
            }

            ++nAddBuffCount;
		}

        if (dwMonsterCount > 0)
        {
            g_EHelper.BatchCheckAddBuff(uidMonsterAddBuff, dwMonsterCount, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, 0, m_data.dwBuffType, 0, &BuffContext, sizeof(BuffContext));
        }

        // 给自己加BUFF
        if(m_data.nSelfBuffID > 0 && m_data.nSelfBuffLevel > 0 && nActorAddBuffCount > 0)
        {
            __IBuffPart * pBuffPart = CAST_TYPE(__IBuffPart*, m_pEntity->getEntityPart(PART_BUFF));
            if(pBuffPart)
            {
                pBuffPart->Add(m_data.nSelfBuffID, m_data.nSelfBuffLevel, uidSelf);
            }
        }
	}

private:
	SCHEME_DATA               m_data;

	__IEntity			      *m_pEntity;

    Vector3                   m_vTargetTile;

    UID                       m_uidTarget;

    int                       m_nSpellID;
};
