/*******************************************************************
** 文件名:	EffectTimerSpellOverlay.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/25/2015
** 版  本:	1.0
** 描  述:	效果-定时技能累积
			(技能标志位0号位置记录累积技能数量)
			使用此效果，技能标志位0位置被占用
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ExternalHelper.h"
#include "SpellMessageDef.h"

using namespace rkt;
using namespace EFFECT_SERVER;

/* 效果： 定时技能累积
*/
class CEffectTimerSpellOverlay : public IEffectEx
{
	// 累积技能数量索引
	#define SPELL_OVERLAY_COUNT_INDEX	0
public:
	typedef  EffectServer_TimerSpellOverlay    SCHEME_DATA;

	CEffectTimerSpellOverlay( SCHEME_DATA & data ) : m_data(data)
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
		if (context==0 || context->pEntity==0)
        {
			return false;
        }
        __IEntity *pEntity = context->pEntity;

		__ISpellPart *pSpellPart = (__ISpellPart *)pEntity->getEntityPart(PART_SPELL);
		if (pSpellPart == NULL)
        {
			return false;
        }

        __IFreezePart *pFreezePart = (__IFreezePart *)pEntity->getEntityPart(PART_FREEZE);
        if (pFreezePart == NULL)
        {
            return false;
        }

        int nSpellID = context->nID;
		ISpell *pSpell = pSpellPart->findSpell(nSpellID);
		if (pSpell == 0)
        {
			return false;
        }

        bool bSend = false;

        // 技能累积类型 SPELLOVERLAY_TYPE
        switch (m_data.nType)
        {
        case SPELLOVERLAY_SET:      // 设置
            {
                // 设置累积技能数量
                pSpell->setFlag(SPELL_OVERLAY_COUNT_INDEX, m_data.nUpperLimit);

                bSend = true;
            }
            break;

        case SPELLOVERLAY_ADD:      // 增加,不能超过上限值
            {
                // 取得累积技能数量
                int nOverlayCount = pSpell->getFlag(SPELL_OVERLAY_COUNT_INDEX);
                // 如果大于上限
                if (nOverlayCount >= m_data.nUpperLimit)
                {
                    return false;
                }

                if (!pFreezePart->CanWork(m_data.nClassID, m_data.dwFreezeID))
                {
                    return false;
                }

                // 设置累积技能数量
                pSpell->setFlag(SPELL_OVERLAY_COUNT_INDEX, ++nOverlayCount);

                // 进行冷却
                pFreezePart->Work(m_data.nClassID, m_data.dwFreezeID);

                //// 受冷却缩减影响
                if (m_data.isActiveByFreeze)
                {
                    IFreezeServer *pFreezeServer = gServerGlobal->getFreezeServer();
                    if (pFreezeServer )
                    {
                        // 配置冷却时间
                        int nFreezeTime = pFreezeServer->GetSchemeFreezeTime(m_data.nClassID, m_data.dwFreezeID );
                        // 取得百分比冷却缩减
                        int nPCTCoolReduce = pEntity->getIntProperty(PROPERTY_PCT_COOL_REDUCE);
                        // 要缩减冷却时间
                        int nChangeTime = (int)((float)nFreezeTime*( (float)nPCTCoolReduce)/ ZOOM_IN_MULTIPLE); 

                        //WarningLn(__FUNCTION__": ConfigFreezeTime="<< nFreezeTime <<", PCTCool="<< nPCTCoolReduce <<", ChangeTime="<< nChangeTime);
                        // 改变冷却时间
                        pFreezePart->AddSpellBreakFreezeTime(m_data.nClassID, m_data.dwFreezeID, -nChangeTime);
                    }
                }

                bSend = true;
            }
            break;

        case SPELLOVERLAY_LESSEN:   // 减少,不能超过下限值
            {
                // 取得累积技能数量
                int nOverlayCount = pSpell->getFlag(SPELL_OVERLAY_COUNT_INDEX);
                // 取得减少前累积技能数量
                int nTempOverlayCount = nOverlayCount;
                // 减少累积次数
                --nOverlayCount;
                // 如果小于下限
                if (nOverlayCount < m_data.nLowerLimit)
                {
                    nOverlayCount = m_data.nLowerLimit;
                }

                // 设置累积技能数量
                pSpell->setFlag(SPELL_OVERLAY_COUNT_INDEX, nOverlayCount);

                // 如果是最大累积的时候减少，要重新开始计时，即清除累积技能累积时间,否则不清
                if (nTempOverlayCount == m_data.nUpperLimit)
                {
                    // 进行冷却
                    pFreezePart->Work(m_data.nClassID, m_data.dwFreezeID);
                }

                bSend = true;
            }
            break;
        default:
            {
                ErrorLn("Effect Spell Overlay not find nType="<<m_data.nType);
                return false;
            }
            break;
        }

        if (!bSend)
        {
            return true;
        }

        // 取得累积技能数量
        int nOverlayCount = pSpell->getFlag(SPELL_OVERLAY_COUNT_INDEX);
        // 取得冷却总时间
        int nTotalTime = pFreezePart->GetFreezeTime(m_data.nClassID, m_data.dwFreezeID);
        // 取得冷却剩余时间
        int nFreezeLeftTime = pFreezePart->GetFreezeLeftTime(m_data.nClassID, m_data.dwFreezeID);

        SSpellOverlay_SC spellOverlay;
        spellOverlay.nID = nSpellID;
        spellOverlay.nSlotType = m_data.nSlotType;
        spellOverlay.nSlotIndex = m_data.nSlotIndex;
        spellOverlay.nFlagIndex = SPELL_OVERLAY_COUNT_INDEX;
        spellOverlay.nFlag = nOverlayCount;
        spellOverlay.nType = nOverlayCount >= m_data.nUpperLimit == true ? 0 : 1;
        spellOverlay.nLeftTime = nTotalTime - nFreezeLeftTime;
        spellOverlay.nTotalTime = nTotalTime;
        spellOverlay.nFreezeLeftTime = nFreezeLeftTime;
        g_EHelper.playerSend2Client(pEntity, PART_SPELL, SPELL_MSG_SPELL_OVERLAY, (char *)&spellOverlay, sizeof(spellOverlay));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectTimerSpellOverlay(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;
};