/*******************************************************************
** 文件名:	EffectCheckMpOrDistanceAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/24/2016
** 版  本:	1.0
** 描  述:	效果-检测魔法或位置增加Buff
            http://172.16.0.120/redmine/issues/1651
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;

class CEffectCheckMpOrDistanceAddBuff : public IEffectEx, public rkt::TimerHandler
{
public:
	typedef  EffectServer_CheckMpOrDistanceAddBuff    SCHEME_DATA;

	CEffectCheckMpOrDistanceAddBuff( SCHEME_DATA & data )
		:m_data(data)
		,m_pEntity(NULL)
		,m_vTargetTile(0,0,0)
		,m_uidTarget(0)
		,m_nSpellID(0)
		,m_AddedMp(0)
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
                ErrorLn("CEffectCheckMpOrDistanceAddBuff error nType="<<m_data.nType);
                return false;
            }
            break;
        }

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
        g_EHelper.KillTimer(0, this);
        m_pEntity = NULL;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectCheckMpOrDistanceAddBuff(m_data);
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

        __IEntityProperty *pProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
        if (pProperty == NULL)
        {
            return;
        }

		// 取得当前值
		int nMp = pProperty->getProperty_Integer(PROPERTY_MP);

		// 计算本次需要增加的魔法
		int nNewAdd = m_data.nValue;

		// 是否开启叠加功能
		if (m_data.bAccumulateAddMP)
		{
			m_AddedMp += m_data.nValue;
			nNewAdd = m_AddedMp + m_data.nAccumulateBaseMP;
		}

        bool bEnd = false;
		int nNewMp = nMp + nNewAdd;
		if (nNewMp < 0)	// 新的魔法小于0，则直接结束，执行buff
		{
			bEnd = true;
		}
		else if (nNewMp == 0)	// 新的魔法等于0，则直接结束，执行buff，同时设置加上新加的魔法值
		{
			// 增加魔法
			pProperty->addProperty_Integer(PROPERTY_MP, nNewAdd);
			bEnd = true;
		}
		else   // 新的当前魔法大于0，则加上新加的魔法值，如果新的当前魔法小于结束值，则结束
		{
			// 增加魔法
			pProperty->addProperty_Integer(PROPERTY_MP, nNewAdd);

			if (nNewMp <= m_data.nEndValue)
			{
				bEnd = true;
			}
		}

        // 不是技能点，取目标实时位置
        if (m_data.nType != USE_SPELL_POS_TILE)
        {
            m_vTargetTile = getLocation(m_uidTarget);
        }

        // 取得自己位置
        Vector3 vLoc = m_pEntity->getLocation();
        if (!equalto0(m_data.fDistance) && !vLoc.checkDistance(m_vTargetTile,m_data.fDistance))
        {
            bEnd = true;
        }

        // 结束
        if (bEnd)
        {
            // 杀掉定时器
            g_EHelper.KillTimer(0, this);
            SBuffContext BuffContext;
            BuffContext.nID = m_nSpellID;
            // 增加Buff
            AddBuff(uidSelf, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
        }
    }

private:
	SCHEME_DATA               m_data;

    __IEntity                 *m_pEntity;

    Vector3                   m_vTargetTile;

    UID                       m_uidTarget;

    int                       m_nSpellID;

	// 已经添加的魔法
	int						  m_AddedMp;
};
