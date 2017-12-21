/*******************************************************************
** 文件名:	EffectSetSpellFlag.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/23/2015
** 版  本:	1.0
** 描  述:	效果-设置技能标志
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"
#include "ISpell.h"

class CEffectSetSpellFlag : public IEffectEx
{
public:
	typedef  EffectServer_SetSpellFlag    SCHEME_DATA;

	CEffectSetSpellFlag( SCHEME_DATA & data ) : m_data(data)
	{
		if (m_data.nIndex < 0 || m_data.nIndex >= SPELL_FLAG_MAX)
		{
			ErrorLn("effect setSpellFlag error m_data.nIndex="<<m_data.nIndex);
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
		if( context==0 || context->pEntity==0 )
			return false;

		__ISpellPart *pSpellPart = (__ISpellPart *)context->pEntity->getEntityPart(PART_SPELL);
		if (pSpellPart == NULL)
			return false;

        int nSpellID = m_data.nSpellID;
        if (nSpellID == 0)
        {
            nSpellID = context->nID;
        }
        if (nSpellID <= 0)
        {
            return false;
        }

		ISpell * pSpell = pSpellPart->findSpell(nSpellID);
		if ( pSpell==0 )
			return false;

		if (m_data.nIndex < 0 || m_data.nIndex >= SPELL_FLAG_MAX)
			return false;

		if (m_data.nType == SPELLFLAG_SET)
		{
			pSpell->setFlag( m_data.nIndex, m_data.nValue );
		}
		else if (m_data.nType == SPELLFLAG_ADD)
		{
			int nCurValue = pSpell->getFlag(m_data.nIndex) + m_data.nValue;
			if (m_data.nValue >= 0)
			{
				// 不能超过设置最大值
				if (nCurValue > m_data.nMaxValue)
				{
					nCurValue = m_data.nMaxValue;
				}
			}
			else
			{
				// 不能超过设置最小值
				if (nCurValue < m_data.nMaxValue)
				{
					nCurValue = m_data.nMaxValue;
				}
			}
			
			pSpell->setFlag(m_data.nIndex, nCurValue);
		}
		else if (m_data.nType == SPELLFLAG_ADD_INFINITE)
		{
			int nCurValue = pSpell->getFlag(m_data.nIndex) + m_data.nValue;

			pSpell->setFlag(m_data.nIndex, nCurValue);
		}
		else
		{
			ErrorLn("Effect Set SpellFlag not find nType="<<m_data.nType);
			return false;
		}

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectSetSpellFlag(m_data);
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
