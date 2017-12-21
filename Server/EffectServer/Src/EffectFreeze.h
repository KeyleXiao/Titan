/*******************************************************************
** 文件名:	EffectFreeze.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/28/2015
** 版  本:	1.0
** 描  述:	效果-冷却相关
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"

using namespace rkt;
using namespace EFFECT_SERVER;

/* 效果： 技能连招
*/
class CEffectFreeze : public IEffectEx
{
public:
	typedef  EffectServer_Freeze    SCHEME_DATA;

	CEffectFreeze( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_bStart(false)
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
		if ( context==0)
			return false;

		m_pEntity = context->pEntity;
		if (m_pEntity == NULL)
		{
			return false;
		}
		__IFreezePart *pFreezePart = (__IFreezePart *)m_pEntity->getEntityPart(PART_FREEZE);
		if (pFreezePart == NULL)
		{
			return false;
		}
        __IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
        if (pEntityProperty == NULL)
        {
            return false;
        }

        int nAddTime = m_data.nAddTime;
        // 受冷却缩减
        if (m_data.nCoolReduce)
        {
            // 取得百分比冷却缩减
            int nPCTCoolReduce = pEntityProperty->getProperty_Integer(PROPERTY_PCT_COOL_REDUCE);
            // 实际冷却＝配置冷却*（1-冷却缩减）
            nAddTime = (int)(nAddTime/ZOOM_IN_MULTIPLE*(ZOOM_IN_MULTIPLE - nPCTCoolReduce));
        }

		switch (m_data.nType)
		{
			// 冷却设置-单个
		case FREEZE_SET:
			{
				// 增加或者减少冷却时间
				pFreezePart->AddFreezeTime(m_data.nClassID, m_data.dwFreezeID, nAddTime);
			}
			break;
			// 冷却清除-单个
		case FREEZE_CLEAN:
			{
				// 取消某个冷却
				pFreezePart->Cancel(m_data.nClassID, m_data.dwFreezeID, true);
			}
			break;
			// 冷却设置-百分比-单个
		case FREEZE_SET_RATE:
			{
				float fRate = nAddTime/EFFECT_RATE;
				// 增加或者减少冷却时间按百分比
				pFreezePart->AddRateFreezeTime(false, m_data.nClassID, m_data.dwFreezeID, fRate);
			}
			break;
		case FREEZE_SET_RATE_CLASS:
			{
				float fRate = nAddTime/EFFECT_RATE;
				// 增加或者减少冷却时间按百分比
				pFreezePart->AddRateFreezeTime(true, m_data.nClassID, m_data.dwFreezeID, fRate);
			}
			break;
        case FREEZE_ONCE_ADD:
            {
                // 打断技能增加或者减少冷却时间
                pFreezePart->AddSpellBreakFreezeTime(m_data.nClassID, m_data.dwFreezeID, nAddTime);
            }
            break;
        case FREEZE_CLASS:
            {
                // 清除某一类
                pFreezePart->Clean(m_data.nClassID);
            }
            break;
        case FREEZE_ALL:
            {
                // 清除所有冷却
                pFreezePart->Clean();
            }
            break;
		default:
			{
				ErrorLn("Effect Freeze not find nType="<<m_data.nType);
				return false;
			}
			break;
		}

		m_bStart = true;

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        do 
        {
            if (!m_bStart)
            {
                break;
            }
            m_bStart = false;

            if (m_pEntity == NULL)
            {
                break;
            }
            __IFreezePart *pFreezePart = (__IFreezePart *)m_pEntity->getEntityPart(PART_FREEZE);
            if (pFreezePart == NULL)
            {
                break;
            }

            switch (m_data.nType)
            {
                // 冷却设置
            case FREEZE_SET:
                {
                    if (m_data.nSubtractAddTime)
                    {
                        // 增加或者减少冷却时间
                        pFreezePart->AddFreezeTime(m_data.nClassID, m_data.dwFreezeID, -m_data.nAddTime);
                    }
                }
                break;
                // 冷却清除
            case FREEZE_CLEAN:
                break;
                // 冷却设置-百分比-单个
            case FREEZE_SET_RATE:
                {
                    if (m_data.nSubtractAddTime)
                    {
                        float fRate = m_data.nAddTime/EFFECT_RATE;
                        // 增加或者减少冷却时间按百分比
                        pFreezePart->AddRateFreezeTime(false, m_data.nClassID, m_data.dwFreezeID, -fRate);
                    }
                }
                break;
            case FREEZE_SET_RATE_CLASS:
                {
                    if (m_data.nSubtractAddTime)
                    {
                        float fRate = m_data.nAddTime/EFFECT_RATE;
                        // 增加或者减少冷却时间按百分比
                        pFreezePart->AddRateFreezeTime(true, m_data.nClassID, m_data.dwFreezeID, -fRate);
                    }
                }
                break;
            case FREEZE_ONCE_ADD:
                {
                    if (m_data.nSubtractAddTime)
                    {
                        // 打断技能增加或者减少冷却时间
                        pFreezePart->AddSpellBreakFreezeTime(m_data.nClassID, m_data.dwFreezeID, -m_data.nAddTime);
                    }
                }
                break;
            default:
                break;
            }
        } while (false);

		m_pEntity = NULL;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectFreeze(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;

	__IEntity				*m_pEntity;
	// 是否启用效果
	bool					m_bStart;
};