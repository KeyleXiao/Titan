/*******************************************************************
** 文件名:	EffectSetSingleSpellData.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/23/2015
** 版  本:	1.0
** 描  述:	效果-设置技能单个数据(int)
            http://172.16.0.120/redmine/issues/1464
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;

class CEffectSetSingleSpellData : public IEffectEx
{
public:
	typedef  EffectServer_SetSingleSpellData    SCHEME_DATA;

	CEffectSetSingleSpellData( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_bSetData(0),m_nOldSpellData(0)
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

        if (getSpellIndexType(m_data.nIndex) != SpellIndex_Int)
        {
            ErrorLn("CEffectSetSingleSpellData error nID="<<GetID()<<",nIndex="<<m_data.nIndex);
            return false;
        }

        __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
        if (pSpellPart == NULL)
        {
            return false;
        }

        ISpell *pSpell = pSpellPart->findSpell(m_data.nSpellID);
        if (pSpell == NULL)
        {
            return false;
        }
        // 设置老数据
        m_nOldSpellData = pSpell->getSpellData((SPELL_DATA_ID)m_data.nIndex);
        
        // 已设置数据
        m_bSetData = true;

        // 设置某个技能数值
        pSpell->setSpellData((SPELL_DATA_ID)m_data.nIndex, m_data.nIndexData, NULL);

        return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        do 
        {
            if (!m_bSetData || m_pEntity == NULL || !m_data.nRecovery)
            {
                break;
            }

            __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
            if (pSpellPart == NULL)
            {
                break;
            }

            ISpell *pSpell = pSpellPart->findSpell(m_data.nSpellID);
            if (pSpell == NULL)
            {
                break;
            }

            // 设置某个技能数值
            pSpell->setSpellData((SPELL_DATA_ID)m_data.nIndex, m_nOldSpellData, NULL);
        } while (false);

        m_bSetData = false;
        m_pEntity = NULL;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectSetSingleSpellData(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;

    __IEntity                 *m_pEntity;

    // 是否已设置数据
    bool                      m_bSetData;
    // 老技能数据
    int                       m_nOldSpellData;
};
