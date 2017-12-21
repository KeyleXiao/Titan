/*******************************************************************
** 文件名:	EffectCheckDistanceAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭政林
** 日  期:	3/08/2016
** 版  本:	1.0
** 描  述:	效果-检测距离增加buff
            http://172.16.0.120/redmine/issues/1693
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;

class CEffectCheckDistanceAddBuff : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_CheckDistanceAddBuff    SCHEME_DATA;

	CEffectCheckDistanceAddBuff( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_vTargetTile(0,0,0),m_uidTarget(0),m_nSpellID(0)
	{
        vector<string> tempVector;
        tempVector.clear();

        m_vDistance.clear();
        StringHelper::split(tempVector, m_data.strDistance, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_vDistance.push_back(StringHelper::toFloat(tempVector[nCount]));
        }
        // 距离容器数量
        int nDistanceCount = (int)m_vDistance.size();

        tempVector.clear();
        m_vBuffID.clear();
        StringHelper::split(tempVector, m_data.strBuffID, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_vBuffID.push_back(StringHelper::toInt(tempVector[nCount]));
        }
        // BuffID容器数量
        int nBuffIDCount = (int)m_vBuffID.size();
        if (nDistanceCount != nBuffIDCount)
        {
            ErrorLn("CEffectCheckDistanceAddBuff error,nDistanceCount="<<nDistanceCount<<",nBuffIDCount="<<nBuffIDCount);
        }

        tempVector.clear();
        m_vBuffLevel.clear();
        StringHelper::split(tempVector, m_data.strBuffLevel, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_vBuffLevel.push_back(StringHelper::toInt(tempVector[nCount]));
        }
        // BuffLevel容器数量
        int nBuffLevelCount = (int)m_vBuffLevel.size();
        if (nBuffIDCount != nBuffLevelCount)
        {
            ErrorLn("CEffectCheckDistanceAddBuff error,nBuffIDCount="<<nBuffIDCount<<",nBuffLevelCount="<<nBuffLevelCount);
        }

        tempVector.clear();
        m_vSelfBuffID.clear();
        StringHelper::split(tempVector, m_data.strSelfBuffID, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_vSelfBuffID.push_back(StringHelper::toInt(tempVector[nCount]));
        }
        // 自己BuffID容器数量
        int nSelfBuffIDCount = (int)m_vSelfBuffID.size();

        tempVector.clear();
        m_vSelfBuffLevel.clear();
        StringHelper::split(tempVector, m_data.strSelfBuffLevel, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_vSelfBuffLevel.push_back(StringHelper::toInt(tempVector[nCount]));
        }
        // 自己BuffLevel容器数量
        int nSelfBuffLevelCount = (int)m_vSelfBuffLevel.size();
        if (nSelfBuffIDCount != nSelfBuffLevelCount)
        {
            ErrorLn("CEffectCheckDistanceAddBuff error,nSelfBuffIDCount="<<nSelfBuffIDCount<<",nSelfBuffLevelCount="<<nSelfBuffLevelCount);
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
        {
			return false;
        }
        m_pEntity = context->pEntity;

        // 距离容器数量
        int nDistanceCount = (int)m_vDistance.size();
        // BuffID容器数量
        int nBuffIDCount = (int)m_vBuffID.size();
        // BuffLevel容器数量
        int nBuffLevelCount = (int)m_vBuffLevel.size();
        if (nDistanceCount == 0 || nDistanceCount != nBuffIDCount || nBuffIDCount != nBuffLevelCount)
        {
            return false;
        }

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
                ErrorLn("CEffectCheckDistanceAddBuff error nType="<<m_data.nType);
                return false;
            }
            break;
        }

        m_nSpellID = context->nID;

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
        if (m_pEntity)
        {
            g_EHelper.KillTimer(0, this);

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectCheckDistanceAddBuff(m_data);
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
        // 取得自己位置
        Vector3 vLoc = m_pEntity->getLocation();

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
        int num = sceneHelper.m_ptr->k_nearestInSpell(m_vTargetTile, m_data.fDistance, m_data.fDistance, pProxyArray, 256, LAYER_ALL);
        if ( num<=0 )
        {
            return;
        }

        SBuffContext BuffContext;
        BuffContext.nID = m_nSpellID;

		// 先判断英雄后判断其他的
		bool bFirstLoop = true; 
		int  nAddBuffCount =  0;

LOOP_START:
        for ( int i=0;i<num;++i )
        {
            // 目标UID
			UID uidTarget = pProxyArray[i]->m_uid;
            if (isInvalidUID(uidTarget))
            {
                continue;
            }

			if ( bFirstLoop )
			{
				// 第一轮先判断英雄
				if (UID_2_TYPE(uidTarget) != TYPE_PLAYER_ROLE)
				{
					continue;
				}
			}else
			{
				// 第二轮判断其他的
				if (UID_2_TYPE(uidTarget) == TYPE_PLAYER_ROLE)
				{
					continue;
				}
			}

            // 检测目标
            if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
            {
                continue;
            }

            // 取得目标位置
            Vector3 vTargetLoc = getLocation(uidTarget);
            // 算出距离
            float fDistance = (vLoc - vTargetLoc).getLength();

            int nCount = (int)m_vDistance.size();
            for (int j=0; j<nCount; ++j)
            {
                if (fDistance <= m_vDistance[j])
                {
                    // 累加增加Buff数量
                    ++nAddBuffCount;

                    // 给目标增加对应buff
                    AddBuff(uidTarget, m_vBuffID[j], m_vBuffLevel[j], uidSelf, 0, &BuffContext, sizeof(BuffContext));

                    if (j < (int)m_vSelfBuffID.size() && j < (int)m_vSelfBuffLevel.size())
                    {
                        int nSelfBuffID = m_vSelfBuffID[j];
                        int nSelfBuffLevel = m_vSelfBuffLevel[j];
                        if (nSelfBuffID > 0 && nSelfBuffLevel > 0)
                        {
                            AddBuff(uidSelf, nSelfBuffID, nSelfBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
                        }
                    }
                    break;
                }
            }

            // 达到增加数量(0表示不限制数量)
            if (m_data.nAddBuffCount != 0 && nAddBuffCount >= m_data.nAddBuffCount)
            {
				bFirstLoop = false;
                break;
            }
        }

		// 判断完英雄再判断一次其他的
		if ( bFirstLoop )
		{
			bFirstLoop = false;
			goto LOOP_START;
		}
    }

private:
	SCHEME_DATA             m_data;

    // 效果信息
    __IEntity               *m_pEntity;

    // BuffID容器
    vector<float>			m_vDistance;
    // BuffID容器
    vector<int>				m_vBuffID;
    // BuffLevel容器
    vector<int>				m_vBuffLevel;

    Vector3                 m_vTargetTile;

    UID                     m_uidTarget;

    // 自己BuffID容器
    vector<int>				m_vSelfBuffID;
    // 自己BuffLevel容器
    vector<int>				m_vSelfBuffLevel;

    int                     m_nSpellID;
};
