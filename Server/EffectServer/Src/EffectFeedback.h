/*******************************************************************
** 文件名:	EffectFeedback.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015-07-14
** 版  本:	1.0
** 描  述:	回赠效果
** 应  用: 

2015-07-14 第一版需求：
	攻击命中目标后，造成伤害回复自身血量（对防御塔类建筑无效）
	需要支持配置技能触发类型、固定数值配置
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "EntityHelper.h"

using namespace EFFECT_SERVER;


// 回赠效果
class CEffectFeedback : public IEffectEx
{
public:
	typedef  EffectServer_Feedback    SCHEME_DATA;

	CEffectFeedback( SCHEME_DATA & data ) : m_data(data)
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
		if( context==0 || context->pEntity==0 )
			return false;

		// 所赠数值不正确
		if (m_data.nNum == 0)
		{
			return false;
		}

		__IEntityProperty *pEntityProperty = (__IEntityProperty *)context->pEntity->getEntityPart(PART_PROPERTY);
		if (pEntityProperty == NULL)
		{
			return false;
		}

		__ISpellPart *pSpellPart = (__ISpellPart *)context->pEntity->getEntityPart(PART_SPELL);
		if (pSpellPart == NULL)
		{
			return false;
		}
		ISpell * pSpell = pSpellPart->findSpell( context->nID );
		if ( pSpell==0 )
		{
			return false;
		}		
		// 配置技能触发类型
		int nSpellType = pSpell->getSpellData(SPDATA_SPELL_EFFECT_TYPE);
		if ( (nSpellType & m_data.nSpellType) != nSpellType )
		{
			return false;
		}

		// 自己
		UID uidSource = context->pEntity->getUID();
		// 目标
		UID uidTarget = context->uidTarget;

		// 检测目标
		if (!g_EHelper.chooseTarget(context->pEntity, m_data.nTarget, uidTarget))
		{
			return false;
		}

		// 增加数值
		switch(m_data.nNumType)
		{
		case FEEDBACK_HP:		// 血量
			{
				NotifyTargetCure(uidSource, uidSource, m_data.nNum);
			}
			break;
		default:
			break;
		}
		
		return true;
	}

	/** 通知目标治疗
	@uidTarget: 治疗目标
	@uidOperator: 施加治疗者
	@nCureHP: 治疗血量
	@return  
	*/
	void NotifyTargetCure(UID uidTarget, UID uidOperator, int nCureHP)
	{
		/* 治疗消息 */
		msg_entity_cure cure; 
		cure.uidOperator = uidOperator; 
		cure.uidTarget = uidTarget;
		cure.nCureHP = nCureHP; 

		// 发送实体消息
		g_EHelper.sendEntityMessage(uidTarget, PART_DAMAGE, DAMAGE_MSG_CURE, (char *)&cure, sizeof(cure));
	}



	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectFeedback(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
};
