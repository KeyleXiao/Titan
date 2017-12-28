/*******************************************************************
** 文件名:	EffectRangeDamageTransmit.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/05/2017
** 版  本:	1.0
** 描  述:	效果-区域伤害传递效果
            http://172.16.0.120/redmine/issues/3241
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"

class CEffectRangeDamageTransmit : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_RangeDamageTransmit    SCHEME_DATA;

	CEffectRangeDamageTransmit( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0)
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

		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE, "CEffectRangeDamageTransmit");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectRangeDamageTransmit(m_data);
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
        case EVENT_ENTITY_PREPARE_DAMAGE:
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

                if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
                {
                    break;
                }
                DamageCalcContext *pDamage = (DamageCalcContext*)pszContext;

                // 如果是效果伤害不再传递
                if ((pDamage->nDamageType&DAMAGE_TYPE_EFFECT) == DAMAGE_TYPE_EFFECT)
                {
                    break;
                }

                Vector3 vTargetTile;
                switch (m_data.nType)
                {
                case TURN_TO_POS_POS_SELF:
                    {
                        vTargetTile = m_pEntity->getLocation();
                    }
                    break;
                case TURN_TO_POS_TARGET:
                    {
                        UID uidTarget = pDamage->uidTarget;
                        if (isInvalidUID(uidTarget))
                        {
                            return;
                        }

                        vTargetTile = getLocation(uidTarget);
                    }
                    break;
                case TURN_TO_POS_OPERATOR:
                    {
                        UID uidTarget = pDamage->uidOperator;
                        if (isInvalidUID(uidTarget))
                        {
                            return;
                        }

                        vTargetTile = getLocation(uidTarget);
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectRangeDamageTransmit error nType="<<m_data.nType);
                        return;
                    }
                    break;
                }

                // 自己UID
                UID uidSelf = m_pEntity->getUID();
                /** 取得实体
		        */
		        int nSceneID = UID_2_SCENE(uidSelf);
		        SceneServiceHelper sceneHelper(nSceneID);
		        if ( sceneHelper.m_ptr==0 )
		        {
			        break;
		        }
        
		        AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
		        int num = sceneHelper.m_ptr->k_nearest(vTargetTile, m_data.fDistance, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL, true);
		        if ( num <= 0 )
                {
                    break;
                }

                UID uidOperator = pDamage->uidOperator;
                for ( int i=0; i<num; ++i )
                {
                    // 目标UID
                    UID uidTarget = pProxyArray[i]->m_uid;
                    if (isInvalidUID(uidTarget))
                    {
                        continue;
                    }

                    // 检测Buff
                    if (!IsExistBuff(uidTarget, m_data.nCheckBuffID, m_data.nCheckBuffLevel, INVALID_UID))
                    {
                        continue;
                    }

                    int nAttack = (int)(pDamage->nAttack*m_data.fValue);
                    int nSpellPower = (int)(pDamage->nSpellPower*m_data.fValue);

                    // 伤害消息
                    msg_entity_damage damage;
                    damage.nID = pDamage->nID;
                    damage.uidOperator = uidOperator;
                    damage.nDamageType = (pDamage->nDamageType|DAMAGE_TYPE_EFFECT);
                    damage.nAttack = nAttack;
                    damage.nPenetration = pDamage->nPenetration;
                    damage.nPenetrationPCT = pDamage->nPenetrationPCT;
                    damage.nSpellPower= nSpellPower;
                    damage.nLevel = pDamage->nLevel;
                    damage.nLevelFactor = pDamage->nLevelFactor;
                    damage.nSpellAttackPCT = pDamage->nSpellAttackPCT;
                    damage.nDeadType = pDamage->nDeadType;
                    damage.nSpellEffectType = pDamage->nSpellEffectType;
                    damage.nCRD = pDamage->nCRD;
                    damage.nAttackCount = pDamage->nAttackCount;
                    damage.nDamageLimitType = pDamage->nDamageLimitType;
                    damage.nDamageLimit = pDamage->nDamageLimit;
                    damage.nDamageBonus = pDamage->nDamageBonus;
                    damage.fDamageBonusPCT = pDamage->fDamageBonusPCT;
                    damage.fAttackFactor = pDamage->fAttackFactor;
                    damage.nPASD         = pDamage->nPASD;
                    damage.nDamageBonus = getProperty_Integer(uidOperator, PROPERTY_DAMAGE_BONUS);
                    damage.fDamageBonusPCT = getProperty_Integer(uidOperator, PROPERTY_DAMAGE_BONUS_PCT)/ZOOM_IN_MULTIPLE;
                    damage.nUseFlag = m_pEntity->getUseFlag()->getAll();    // 用途标识

                    // 发送实体消息
                    g_EHelper.sendEntityMessage(uidTarget, PART_DAMAGE, DAMAGE_MSG_DAMAGE, (char *)&damage, sizeof(damage));
                }

                // 设置冷却
                m_dwLastTick = dwTick;
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

    // 上次时间
    DWORD					m_dwLastTick;
};
