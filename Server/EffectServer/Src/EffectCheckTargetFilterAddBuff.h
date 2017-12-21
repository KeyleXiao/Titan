/*******************************************************************
** 文件名:	EffectCheckTargetFilterAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/06/2016
** 版  本:	1.0
** 描  述:	效果-检测目标过滤增加buff
            http://172.16.0.120/redmine/issues/2062
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

class CEffectCheckTargetFilterAddBuff : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_CheckTargetFilterAddBuff    SCHEME_DATA;

	CEffectCheckTargetFilterAddBuff( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL),m_nSpellID(0)
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
		return new CEffectCheckTargetFilterAddBuff(m_data);
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
        // 取得自己位置
        Vector3 vSelfLoc = m_pEntity->getLocation();

         /** 取得实体
		*/
		int nSceneID = UID_2_SCENE(uidSelf);
		SceneServiceHelper sceneHelper(nSceneID);
		if ( sceneHelper.m_ptr==0 )
		{
			return;
		}
        
		AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
        int num = sceneHelper.m_ptr->k_nearest(vSelfLoc, m_data.fDistance, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL, true );
        if ( num<=0 )
        {
            return;
        }

        SBuffContext BuffContext;
        BuffContext.nID = m_nSpellID;

		for ( int i=0; i<num; ++i )
		{
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

			// 增加buff
			AddBuff(uidTarget, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
            break;
		}
	}

private:
	SCHEME_DATA               m_data;

	__IEntity			      *m_pEntity;

    int                       m_nSpellID;
};
