/*******************************************************************
** 文件名:	EffectShowDebuffProgressBar.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	7/24/2017
** 版  本:	1.0
** 描  述:	效果-显示debuff进度条
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;

class CEffectShowDebuffProgressBar : public IEffect
{
public:
	typedef  EffectClient_ShowDebuffProgressBar    SCHEME_DATA;

	CEffectShowDebuffProgressBar( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

		m_pEntity = context->pEntity;

		cmd_show_debuff_progress_bar cmd;
		cmd.nOpenFlag = 1;
		cmd.nType = m_data.eType;
		cmd.nTime = m_data.nTime;

		m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SHOW_DEBUFF_PROGRESS_BAR, 0, 0, &cmd, sizeof(cmd));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != NULL)
		{
			cmd_show_debuff_progress_bar cmd;
			cmd.nOpenFlag = 0;
			cmd.nType = m_data.eType;
			cmd.nTime = m_data.nTime;

			m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SHOW_DEBUFF_PROGRESS_BAR, 0, 0, &cmd, sizeof(cmd));

			m_pEntity = NULL;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectShowDebuffProgressBar(m_data);
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

private:
	SCHEME_DATA               m_data;

	IEntity                   *m_pEntity;
};
