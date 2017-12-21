/*******************************************************************
** 文件名:	EffectStopLighting.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	5/04/2015
** 版  本:	1.0
** 描  述:	效果-停止光效
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;

class CEffectStopLighting : public IEffect
{
public:
	typedef EffectClient_StopLighting SCHEME_DATA;

	CEffectStopLighting( SCHEME_DATA & data ) : m_data(data)
	{
		m_splitVector.clear();
		StringHelper::split(m_splitVector, m_data.strName, ';', " ");
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT *pContext )
	{
		if ( pContext==0 || pContext->pEntity==0 )
			return false;

		if (m_splitVector.empty())
			return false;

		int nCount = m_splitVector.size();
		for (int i=0; i<nCount; ++i)
		{
			int nLightingID = StringHelper::toInt(m_splitVector[i]);
			if (nLightingID == 0)
			{
				continue;
			}
			// 告诉显示层停止某个光效
			pContext->pEntity->sendCommandToEntityView(ENTITY_TOVIEW_REMOVE_LIGHTING, nLightingID, 0, 0, 0);
		}

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectStopLighting(m_data); }

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
	vector<string>			m_splitVector;
};
