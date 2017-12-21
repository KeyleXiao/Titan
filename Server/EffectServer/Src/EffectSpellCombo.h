/*******************************************************************
** 文件名:	EffectSpellCombo.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/02/2015
** 版  本:	1.0
** 描  述:	效果-技能连招(技能标志位0号位置存储技能连招)
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ExternalHelper.h"
#include "SpellContextDef.h"

using namespace rkt;
using namespace EFFECT_SERVER;

/* 效果： 技能连招
*/
class CEffectSpellCombo : public IEffectEx
{
	// 技能连招索引
	#define SPELL_COMBO_INDEX 0
public:
	typedef  EffectServer_SpellCombo    SCHEME_DATA;

	CEffectSpellCombo( SCHEME_DATA & data ) : m_data(data)
	{
		vector<string> tempVector;
		tempVector.clear();

		m_ComboIdVector.clear();
		StringHelper::split(tempVector, m_data.strComboID, ';', " ");
		for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
		{
			m_ComboIdVector.push_back(StringHelper::toInt(tempVector[nCount]));
		}
		// 取得技能ID数量
		int nComboIdCount = (int)m_ComboIdVector.size();

		tempVector.clear();
		m_ResetTimeVector.clear();
		StringHelper::split(tempVector, m_data.strResetTime, ';', " ");
		for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
		{
			m_ResetTimeVector.push_back(StringHelper::toInt(tempVector[nCount]));
		}
		// 重置时间数量
		int nResetTimeCount = (int)m_ResetTimeVector.size();
		if (m_data.nComboCount != nComboIdCount || m_data.nComboCount != nResetTimeCount)
		{
			ErrorLn("LianZhao number does not match,ID="<< GetID() <<",nComboCount="<<m_data.nComboCount<<",IDCounts="<<nComboIdCount<<",Reset the number of time="<<nResetTimeCount);
		}

		// 设置连招个数为最小的一个
		m_data.nComboCount = min(m_data.nComboCount, min(nComboIdCount, nResetTimeCount));
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

		// 没有连招
		if (m_data.nComboCount <= 0)
			return false;

		__ISpellPart *pSpellPart = (__ISpellPart *)context->pEntity->getEntityPart(PART_SPELL);
		if (pSpellPart == NULL)
			return false;

		// 0号位置是连招开始ID
		ISpell *pSpell = pSpellPart->findSpell(m_ComboIdVector[0]);
		if ( pSpell==0 )
			return false;

		// 取得当前的连招序号
		int nCurCombo = pSpell->getFlag(SPELL_COMBO_INDEX);
		if ( nCurCombo>=m_data.nComboCount)
			nCurCombo = 0;

		int nLastSpellID = m_ComboIdVector[nCurCombo];
		ISpell *pLastSpell = pSpellPart->findSpell(nLastSpellID);
		if ( pLastSpell==0 )
		{
			return false;
		}

		// 取得冷却部件
		__IFreezePart *pFreezePart = (__IFreezePart*)context->pEntity->getEntityPart(PART_FREEZE);
		if (pFreezePart == NULL)
		{
			return false;
		}

		//  上次连招还在冷却
		if (!pFreezePart->CanWork(EFreeze_ClassID_Spell, pLastSpell->getSpellData( SPDATA_COOLID )))
		{
			// 改变技能ID,设置此标志表示还不可用
			context->nID = 0;
            if (pConditionContext != NULL)
            {
                pConditionContext->nID = 0;
            }
			return false;
		}

		// 取得上次释放时间
		DWORD nLastSpellTick = pSpellPart->getLastCastTime();
		DWORD nCurTick = getTickCountEx();

		// 如果超时就重置
		if ( nCurTick-m_ResetTimeVector[nCurCombo] > nLastSpellTick )
		{
			nCurCombo = 0;
		}
		else
		{
			++nCurCombo;
			if ( nCurCombo>=m_data.nComboCount)
				nCurCombo = 0;
		}

		int nCurSpellID = m_ComboIdVector[nCurCombo];

		// 保存连招序号
		pSpell->setFlag(SPELL_COMBO_INDEX, nCurCombo);

		// 改变技能ID
        context->nID = nCurSpellID;
        if (pConditionContext != NULL)
        {
            pConditionContext->nID = nCurSpellID;
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
		return new CEffectSpellCombo(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;
	// 连招ID容器
	vector<int>				m_ComboIdVector;
	// 连招ID容器
	vector<int>				m_ResetTimeVector;
};