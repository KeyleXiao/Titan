/*******************************************************************
** 文件名:	EffectGetTalent.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	ljw
** 日  期:	05/22/2015
** 版  本:	1.0
** 描  述:	获得天赋
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"

class CEffectActiveTalent : public IEffectEx
{
public:
	typedef  EffectServer_ActiveTalent    SCHEME_DATA;

	CEffectActiveTalent( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL)
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

        m_pEntity = context->pEntity;

        __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
        if (pSpellPart == NULL)
        {
            return false;
        }

        switch (m_data.nTalentType)
        {
        case EFFECT_ACTIVE_TALENT:
            {
                pSpellPart->activeTalent(m_data.nTalentID);
            }
            break;
        case EFFECT_DE_ACTIVE_TALENT:
            {
                pSpellPart->deActiveTalent(m_data.nTalentID);
            }
            break;
        default:
            break;
        }
		
		return false;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            do 
            {
                // 结束保留
                if (m_data.nReserve)
                {
                    break;
                }

                __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
                if (pSpellPart == NULL)
                {
                    break;
                }

                // 结束操作要反一下
                switch (m_data.nTalentType)
                {
                case EFFECT_ACTIVE_TALENT:
                    {
                        pSpellPart->deActiveTalent(m_data.nTalentID);
                    }
                    break;
                case EFFECT_DE_ACTIVE_TALENT:
                    {
                        pSpellPart->activeTalent(m_data.nTalentID);
                    }
                    break;
                default:
                    break;
                }
            } while (false);

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectActiveTalent(m_data);
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
};
