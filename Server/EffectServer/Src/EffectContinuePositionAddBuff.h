/*******************************************************************
** 文件名:	EffectContinuePositionAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/24/2016
** 版  本:	1.0
** 描  述:	效果-持续位置增加Buff
            http://172.16.0.120/redmine/issues/1651
            http://172.16.0.120/redmine/issues/1652
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;

class CEffectContinuePositionAddBuff : public IEffectEx, public rkt::TimerHandler
{
public:
	typedef  EffectServer_ContinuePositionAddBuff    SCHEME_DATA;

	CEffectContinuePositionAddBuff( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL),m_vTargetTile(0,0,0),m_uidTarget(0),m_nSpellID(0)
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
                ErrorLn("CEffectContinuePositionAddBuff error nType="<<m_data.nType);
                return false;
            }
            break;
        }

        if(m_data.nCount >= 1)
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, m_data.nCount, "CEffectContinuedSetProperty");
        }
        else
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, INFINITY_CALL, "CEffectContinuedSetProperty");
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
		return new CEffectContinuePositionAddBuff(m_data);
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
        int num = sceneHelper.m_ptr->k_nearest(m_vTargetTile, m_data.fDistance, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL, true);
        if ( num<=0 )
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
                // 给目标增加buff
                AddBuff(uidTarget, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
            }

            ++nAddBuffCount;
        }

        if (dwMonsterCount > 0)
        {
            g_EHelper.BatchAddBuff(uidMonsterAddBuff, dwMonsterCount, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
        }
    }

private:
	SCHEME_DATA               m_data;

    __IEntity                 *m_pEntity;

    Vector3                   m_vTargetTile;

    UID                       m_uidTarget;

    int                       m_nSpellID;
};
