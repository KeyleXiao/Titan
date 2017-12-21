/*******************************************************************
** 文件名:	EffectCastSpell.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/22/2015
** 版  本:	1.0
** 描  述:	效果-施法技能
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "EventDef.h"
#include "ISpell.h"
#include "EntityHelper.h"
#include "SpellContextDef.h"

class CEffectCastSpell : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_CastSpell    SCHEME_DATA;

	CEffectCastSpell( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_dwLastTick(0)
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

		// 注册选择事件
		g_EHelper.Subscibe(m_pEntity, this, (EventKey)m_data.nCastSpellType, "CEffectCastSpell");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销选择事件
			g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)m_data.nCastSpellType);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectCastSpell(m_data);
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
		case EVENT_ENTITY_ATTACK:	// 收到攻击事件
			{
				if (m_pEntity == NULL)
				{
					return;
				}
				__ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
				if (pSpellPart == NULL)
				{
					return;
				}

				if (pszContext == NULL || nLen != sizeof(event_entity_attack))
				{
					return;
				}
				event_entity_attack *pAttack = (event_entity_attack *)pszContext;

				UID uidTarget = INVALID_UID;
				switch (m_data.nTagetType)
				{
				case MA_TAGET_TO_SELF:
					{
						uidTarget = m_pEntity->getUID();
					}
					break;
				case MA_TAGET_TO_TARGET:
					{
						uidTarget = pAttack->uidTarget;
					}
					break;
				default:
					{
						ErrorLn("CEffectCastSpell EVENT_ENTITY_ATTACK error nTagetType="<<m_data.nTagetType);
						return;
					}
					break;
				}
				if (isInvalidUID(uidTarget))
				{
					return;
				}

				UID uidSrcEntity = m_pEntity->getUID();
				// 检测目标
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
				{
					return;
				}

				// 技能效果类型匹配
				if ((m_data.nSpellEffectType & pAttack->nSpellEffectType) != m_data.nSpellEffectType)
				{
					return;
				}

				// 计算概率
				int nRandNum = 0;
				rktRangeRand(nRandNum, 1, EFFECT_PROB_VALUE);
				if(nRandNum > m_data.nProb)
				{
					return;
				}

				// 取得当前时间
				DWORD dwTick = getTickCountEx();
				if (m_dwLastTick + m_data.nInterval > dwTick)
				{
					return;
				}
				// 设置上次时间
				m_dwLastTick = dwTick;

                Vector3 vLoc = m_pEntity->getLocation();
				Vector3 vTargetTile = getLocation(uidTarget);
                DWORD dwStartTick = getTickCountEx();
				// 技能施法现场
				SPELL_CONTEXT spellContext;
				spellContext.CC.pEntity = m_pEntity;
				spellContext.CC.uidOperator = uidSrcEntity;
				spellContext.CC.uidTarget = uidTarget;
                spellContext.CC.ptStartTile = vLoc;
				spellContext.CC.ptTargetTile = vTargetTile;
				spellContext.CC.nID = m_data.nSpellID;		
				spellContext.CC.dwStartTick = dwStartTick;
				spellContext.CC.dwUpdateFrame = 0;
				spellContext.EC.pEntity = m_pEntity;
				spellContext.EC.uidOperator = uidSrcEntity;
				spellContext.EC.uidTarget = uidTarget;
                spellContext.EC.ptStartTile = vLoc;
				spellContext.EC.ptTargetTile = vTargetTile;
				spellContext.EC.nID = m_data.nSpellID;
                spellContext.EC.vSelfLoc = vLoc;
                spellContext.EC.dwStartTick = dwStartTick;

				pSpellPart->castSpell(&spellContext);
			}
			break;
		case EVENT_ENTITY_DAMAGE:	// 收到伤害事件
			{
				if (m_pEntity == NULL)
				{
					return;
				}
				__ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
				if (pSpellPart == NULL)
				{
					return;
				}

				if (pszContext == NULL || nLen != sizeof(event_entity_damage))
				{
					return;
				}
				event_entity_damage *pDamage = (event_entity_damage *)pszContext;

				UID uidTarget = INVALID_UID;
				switch (m_data.nTagetType)
				{
				case MA_TAGET_TO_SELF:
					{
						uidTarget = m_pEntity->getUID();
					}
					break;
				case MA_TAGET_TO_TARGET:
					{
						uidTarget = pDamage->uidOperator;
					}
					break;
				default:
					{
						ErrorLn("CEffectCastSpell EVENT_ENTITY_DAMAGE error nTagetType="<<m_data.nTagetType);
						return;
					}
					break;
				}
				if (isInvalidUID(uidTarget))
				{
					return;
				}

				UID uidSrcEntity = m_pEntity->getUID();
				// 检测目标
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
				{
					return;
				}

				// 技能效果类型匹配
				if ((m_data.nSpellEffectType & pDamage->nSpellEffectType) != m_data.nSpellEffectType)
				{
					return;
				}

				// 计算概率
				int nRandNum = 0;
				rktRangeRand(nRandNum, 1, EFFECT_PROB_VALUE);
				if(nRandNum > m_data.nProb)
				{
					return;
				}

				// 取得当前时间
				DWORD dwTick = getTickCountEx();
				if (m_dwLastTick + m_data.nInterval > dwTick)
				{
					return;
				}
				// 设置上次时间
				m_dwLastTick = dwTick;

                Vector3 vLoc = m_pEntity->getLocation();
				Vector3 vTargetTile = getLocation(uidTarget);
                DWORD dwStartTick = getTickCountEx();
				// 技能施法现场
				SPELL_CONTEXT spellContext;
				spellContext.CC.pEntity = m_pEntity;
				spellContext.CC.uidOperator = uidSrcEntity;
				spellContext.CC.uidTarget = uidTarget;
                spellContext.CC.ptStartTile = vLoc;
				spellContext.CC.ptTargetTile = vTargetTile;
				spellContext.CC.nID = m_data.nSpellID;		
				spellContext.CC.dwStartTick = dwStartTick;
				spellContext.CC.dwUpdateFrame = 0;
				spellContext.EC.pEntity = m_pEntity;
				spellContext.EC.uidOperator = uidSrcEntity;
				spellContext.EC.uidTarget = uidTarget;
                spellContext.EC.ptStartTile = vLoc;
				spellContext.EC.ptTargetTile = vTargetTile;
				spellContext.EC.nID = m_data.nSpellID;
                spellContext.EC.vSelfLoc = vLoc;
                spellContext.EC.dwStartTick = dwStartTick;

				pSpellPart->castSpell(&spellContext);
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
