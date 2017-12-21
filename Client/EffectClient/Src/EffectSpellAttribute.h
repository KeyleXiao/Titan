/*******************************************************************
** 文件名:	EffectSpellAttribute.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	10/12/2016
** 版  本:	1.0
** 描  述:	效果-技能询问
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ConditionDef.h"
#include "ISchemeCenter.h"
using namespace EFFECT_CLIENT;
using namespace CONDITION_CLIENT;

#define  EFFECT_INNER_CONDITION( typeName,obj,pContext)    gClientGlobal->getConditionFactory()->TestInnerCondition( #typeName,&obj,sizeof(obj),pContext)

class CEffectSpellAttribute : public IEffect
{
public:
	typedef EffectClient_SpellAttribute SCHEME_DATA;

	CEffectSpellAttribute( SCHEME_DATA & data ) : m_data(data), m_pEntity(NULL)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT *pContext )
	{
		if ( pContext==0 || pContext->pEntity==0 )
			return false;

        m_pEntity = pContext->pEntity;
        // 技能相关属性
        cmd_spell_attribute attribute;
        // 开启瞄准标志
        attribute.nDefaultAimFlag = 1;
        do 
        {
            IClientEntity *pClientEntity = (IClientEntity *)m_pEntity;
            // 死亡状态，不再可使用技能
            if (pClientEntity->getState() == ENTITY_STATE_DIE)
            {
                break;
            }
            ISpellPart *pSpellPart = (ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
            if (pSpellPart == NULL)
            {
                break;
            }

            ISpell *pSpell = pSpellPart->findSpell(m_data.nSpellID);
            if (pSpell == NULL)
            {
                break;
            }

            // 技能使用中
            if (pSpell->isWorking())
            {
                break;
            }

            // 技能使用类型
            int nCastType = pSpell->getSpellData(SPDATA_CAST_TYPE);

            CONDITION_CONTEXT context;
            context.pEntity = m_pEntity;

            // 检查法力
            ConditionClient_MP Mp;
            Mp.nValue = pSpell->getSpellData( SPDATA_NEED_MP );
            if ( EFFECT_INNER_CONDITION( ConditionClient_MP,Mp,&context )==false )
            {
                break;
            }

            // 检查体力
            ConditionClient_Stamina Stamina;
            Stamina.nValue = pSpell->getSpellData( SPDATA_NEED_STAMINA );
            if ( EFFECT_INNER_CONDITION( ConditionClient_Stamina,Stamina,&context )==false )
            {
                break;
            }

            // 检查能量
            ConditionClient_Energy Energy;
            Energy.nValue = pSpell->getSpellData( SPDATA_NEED_ENERGY );
            if ( EFFECT_INNER_CONDITION( ConditionClient_Energy,Energy,&context )==false )
            {
                break;
            }

            // 需要检测冷却(!技能使用客户端不检测冷却)
            if ((nCastType&SpellCastType_ClientNotCheckCool) != SpellCastType_ClientNotCheckCool)
            {
                // 检查冷却
                ConditionClient_CoolDown CoolDown;
                CoolDown.nClassID = EFreeze_ClassID_Spell;
                CoolDown.nCoolDownID = pSpell->getSpellData(SPDATA_COOLID);
                if (EFFECT_INNER_CONDITION(ConditionClient_CoolDown, CoolDown, &context)==false)
                {
                    break;
                }
            }

            // 读条时间
            int nTime = pSpell->getSpellData( SPDATA_PREPARE_TIME );
            if ((pSpell->getSpellData(SPDATA_CAST_FLAG)&SpellCastFlag_ContinueFocoEnergia) == SpellCastFlag_ContinueFocoEnergia)
            {
                // 累加持续时间
                nTime += pSpell->getSpellData( SPDATA_DURATION );
            }

            // 技能选择类型
            int nSelectType = pSpell->getSpellData(SPDATA_SELECT_TYPE);

            attribute.nSelectType = nSelectType;
            attribute.fDistance = (float)pSpell->getSpellData(SPDATA_DISTANCE)/SPELL_FLOAT_VALUE;
            attribute.fAttackRange = (float)pSpell->getSpellData(SPDATA_ATTACK_RANGE)/SPELL_FLOAT_VALUE;
            attribute.fChooseRange = (float)pSpell->getSpellData(SPDATA_CHOOSE_RANGE)/SPELL_FLOAT_VALUE;
            if ((nSelectType&SpellSelectType_SelectHeight) == SpellSelectType_SelectHeight)
            {
                ISchemeEntity *pSchemeEntity = gClientGlobal->getSchemeCenter()->getSchemeEntity();
                if (pSchemeEntity != NULL)
                {
                    attribute.fHeight = pSchemeEntity->getEntityXml()->fSpellHeight;
                }
            }
            attribute.nDrawType = pSpell->getSpellData(SPDATA_DRAW_TYPE);
            attribute.nTargetType = pSpell->getSpellData(SPDATA_SELECT_TARGET);
            attribute.nAttributeType = 1;	// 可使用
            attribute.nCastType = nCastType;
            attribute.nSlotIndex = m_data.nSlotIndex;
            attribute.nSlotType = m_data.nSlotType;
            attribute.nSpellID = m_data.nSpellID;
            attribute.nAttackType = pSpell->getSpellData(SPDATA_ATTACK_TYPE);
            attribute.nSpellType = pSpell->getSpellData(SPDATA_TYPE);
            attribute.nFocoEnergiaTime = nTime;
            attribute.nCastFlag = pSpell->getSpellData( SPDATA_CAST_FLAG );
            attribute.nSpellFlag = pSpell->getSpellData( SPDATA_FLAG );
            if ((attribute.nSpellFlag&SpellFlag_Roll) == SpellFlag_Roll)
            {
                ISlotLogicPart *pSlotLogicPart = (ISlotLogicPart *)pClientEntity->getEntityPart(PART_SLOTLOGIC);
                if (pSlotLogicPart != NULL)
                {
                    attribute.nRollSpellStartID = pSlotLogicPart->GetSlotSkillID(m_data.nSlotType, ROLL_SPELL_START_SLOT_INDEX);
                }
            }
        } while (false);

        // 设置技能相关属性
        m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SETSPELLATTRIBUTE, 0, 0, &attribute, sizeof(attribute));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            // 技能相关属性
            cmd_spell_attribute attribute;
            // 开启瞄准标志
            attribute.nDefaultAimFlag = -1;
            // 设置技能相关属性
            m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SETSPELLATTRIBUTE, 0, 0, &attribute, sizeof(attribute));

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectSpellAttribute(m_data); }

	// 取得效果ID
	virtual int				GetEffectID(){ return m_data.nID; }

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;

    IEntity                 *m_pEntity;
};
