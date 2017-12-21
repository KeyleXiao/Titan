/*******************************************************************
** 文件名:	EffectSetSingleStrSpellData.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/23/2015
** 版  本:	1.0
** 描  述:	效果-设置技能单个字符串数据(str)
            http://172.16.0.120/redmine/issues/1464
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;

class CEffectSetSingleStrSpellData : public IEffect
{
public:
	typedef  EffectClient_SetSingleStrSpellData    SCHEME_DATA;

	CEffectSetSingleStrSpellData( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_bSetData(0)
	{
        memset(m_nOldSpellData, 0, sizeof(m_nOldSpellData));
	}

    // 获取效果ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

        m_pEntity = context->pEntity;

        if (getSpellIndexType(m_data.nIndex) != SpellIndex_Str)
        {
            ErrorLn("CEffectSetSingleStrSpellData error nID="<<GetID()<<",nIndex="<<m_data.nIndex);
            return false;
        }

        ISpellPart *pSpellPart = (ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
        if (pSpellPart == NULL)
        {
            return false;
        }

        ISpell *pSpell = pSpellPart->findSpell(m_data.nSpellID);
        if (pSpell == NULL)
        {
            // 之前优化技能进视野包 把其他人的技能去掉了 延时到其他人使用技能时再学习
            pSpellPart->addSpell(m_data.nSpellID);
            pSpell = pSpellPart->findSpell(m_data.nSpellID);
            if (pSpell == NULL)
            {
                return false;
            }
        }
        // 设置老数据
        sstrcpyn(m_nOldSpellData, pSpell->getSpellDataStr((SPELL_DATA_ID)m_data.nIndex), sizeof(m_nOldSpellData));
        
        // 已设置数据
        m_bSetData = true;

        // 设置某个技能数值
        pSpell->setSpellData((SPELL_DATA_ID)m_data.nIndex, 0, m_data.strIndex);

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

            // 设置某个技能数值
            pSpell->setSpellData((SPELL_DATA_ID)m_data.nIndex, 0, m_nOldSpellData);
        } while (false);

        m_bSetData = false;
        m_pEntity = NULL;
        
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectSetSingleStrSpellData(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	// 取得效果ID
	virtual int				GetEffectID()
	{
		return m_data.nID;
	}

	// 
	virtual bool			IsBusying()
	{
		return false;
	}

	// 
	virtual bool			OnThink()
	{
		return false;
	}

	// 
	virtual bool			IsAbleStart()
	{
		return true;
	}

private:
	SCHEME_DATA               m_data;

    IEntity                 *m_pEntity;

    // 是否已设置数据
    bool                      m_bSetData;
    // 老技能数据
    char                      m_nOldSpellData[SINGLE_STR_SPELL_DATA_LEN];
};
