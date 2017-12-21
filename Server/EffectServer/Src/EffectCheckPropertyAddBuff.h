/*******************************************************************
** 文件名:	EffectCheckPropertyAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/24/2016
** 版  本:	1.0
** 描  述:	效果-检测属性增加buff
            http://172.16.0.120/redmine/issues/1654
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;

class CEffectCheckPropertyAddBuff : public IEffectEx, public rkt::TimerHandler
{
public:
	typedef  EffectServer_CheckPropertyAddBuff    SCHEME_DATA;

	CEffectCheckPropertyAddBuff( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL),m_bSelfAddBuff(false),m_nSpellID(0)
	{
        m_addBuffMap.clear();
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

            UID uidSelf = m_pEntity->getUID();
            if (m_bSelfAddBuff)
            {
                // 移除buff
                RemoveBuff(uidSelf, m_data.nBuffID, uidSelf, uidSelf);

                m_bSelfAddBuff = false;
            }

            ADD_BUFF_MAP::iterator Iter = m_addBuffMap.begin();
            while (Iter != m_addBuffMap.end())
            {
                UID uidTarget = Iter->first;
                // 移除buff
                RemoveBuff(uidTarget, m_data.nTargetBuffID, uidSelf, uidSelf);

                Iter = m_addBuffMap.erase(Iter);
            }
            m_addBuffMap.clear();

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectCheckPropertyAddBuff(m_data);
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

        ADD_BUFF_MAP::iterator Iter = m_addBuffMap.begin();
        while (Iter != m_addBuffMap.end())
        {
            UID uidTarget = Iter->first;

            // 检测增加buff条件,不满足移除目标buff
            if (!checkAddBuff(uidTarget))
            {
                // 移除buff
                RemoveBuff(uidTarget, m_data.nTargetBuffID, uidTarget, uidTarget);

                Iter = m_addBuffMap.erase(Iter);
            }
            else
            {
                ++Iter;
            }
        }

        // 如果目标都没有buff了
        if (m_addBuffMap.empty())
        {
            // 移除自己身上的buff
            if (m_bSelfAddBuff)
            {
                // 移除buff
                RemoveBuff(uidSelf, m_data.nBuffID, uidSelf, uidSelf);

                m_bSelfAddBuff = false;
            }
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

            if (nAddBuffCount >= nMaxAddBuffCount)
            {
                break;
            }

            // 检测增加buff条件
            if (!checkAddBuff(uidTarget))
            {
                continue;
            }

            if (!m_bSelfAddBuff)
            {
                // 给自己增加Buff
                AddBuff(uidSelf, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));

                // 设置自己增加Buff
                m_bSelfAddBuff = true;
            }

            if (isMonster(uidTarget))
            {
                uidMonsterAddBuff[dwMonsterCount++] = uidTarget;
            }
            else
            {
                // 给目标增加buff
                AddBuff(uidTarget, m_data.nTargetBuffID, m_data.nTargetBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
            }
            // 插入表中
            m_addBuffMap.insert(ADD_BUFF_MAP::value_type(uidTarget,true));

            ++nAddBuffCount;
        }

        if (dwMonsterCount > 0)
        {
            g_EHelper.BatchAddBuff(uidMonsterAddBuff, dwMonsterCount, m_data.nTargetBuffID, m_data.nTargetBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
        }
    }

    bool checkAddBuff(UID uidTarget)
    {
        if (m_pEntity == NULL)
        {
            return false;
        }

        if (isInvalidUID(uidTarget))
        {
            return false;
        }

        // 取得自己位置
        Vector3 vLoc = m_pEntity->getLocation();
        // 取得目标位置
        Vector3 vTargetLoc = getLocation(uidTarget);
        // 不在范围内
        if (!vLoc.checkDistance(vTargetLoc, m_data.fDistance))
        {
            return false;
        }

        int nCur = 0;
        int nMax = 0;
        switch (m_data.nAddBuffType)
        {
        case CHECK_PROPERTY_ADD_BUFF_HP:
            {
                nCur = getProperty_Integer(uidTarget, PROPERTY_HP);
                nMax = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
            }
            break;
        case CHECK_PROPERTY_ADD_BUFF_MAX_HP:
            {
                nCur = getProperty_Integer(uidTarget, PROPERTY_MAX_HP); 
                nMax = nCur; 
            }
            break;
        case CHECK_PROPERTY_ADD_BUFF_MP:
            {
                nCur = getProperty_Integer(uidTarget, PROPERTY_MP);
                nMax = getProperty_Integer(uidTarget, PROPERTY_MAX_MP);
            }
            break;
        case CHECK_PROPERTY_ADD_BUFF_ENERGY:
            {
                nCur = getProperty_Integer(uidTarget, PROPERTY_ENERGY);
                nMax = getProperty_Integer(uidTarget, PROPERTY_MAX_ENERGY);
            }
            break;
        case CHECK_PROPERTY_ADD_BUFF_STAMINA:
            {
                nCur = getProperty_Integer(uidTarget, PROPERTY_STAMINA);
                nMax = getProperty_Integer(uidTarget, PROPERTY_MAX_STAMINA);
            }
            break;
        default:
            break;
        }
        if (nCur <= 0 || nMax <= 0)
        {
            return false;
        }

        bool bAdd = false;
        switch (m_data.nValueType)
        {
        case MA_VALUE_NUM:
            {
                if ((float)nCur >= m_data.fMinValue && (float)nCur <= m_data.fMaxValue)
                {
                    bAdd = true;
                }
            }
            break;
        case MA_VALUE_PER:
            {
                float fPercent = (float)nCur/nMax;
                if (fPercent >= m_data.fMinValue && fPercent <= m_data.fMaxValue)
                {
                    bAdd = true;
                }
            }
            break;
        case MA_VALUE_LOSE_PER:
            {
                float fPercent = (float)(nMax-nCur)/nMax;
                if (fPercent >= m_data.fMinValue && fPercent <= m_data.fMaxValue)
                {
                    bAdd = true;
                }
            }
            break;
        default:
            break;
        }
        if (!bAdd)
        {
            return false;
        }

        // 检测目标
        if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
        {
            return false;
        }

        return true;
    }

private:
	SCHEME_DATA               m_data;

    __IEntity                 *m_pEntity;

    // 自己增加Buff
    bool                      m_bSelfAddBuff;

    // 增加buff表
    typedef map<UID, bool>	  ADD_BUFF_MAP;
    ADD_BUFF_MAP			  m_addBuffMap;

    int                       m_nSpellID;
};
