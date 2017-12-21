/*******************************************************************
** �ļ���:	EffectShowDebuffProgressBar.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	ljw
** ��  ��:	7/24/2017
** ��  ��:	1.0
** ��  ��:	Ч��-��ʾdebuff������
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

	// Ч������
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

	// Ч��ֹͣ
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

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectShowDebuffProgressBar(m_data);
	}

	// �ͷ�
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	// ȡ��Ч��ID
	virtual int				GetEffectID()
	{
		return m_data.nID;
	}

private:
	SCHEME_DATA               m_data;

	IEntity                   *m_pEntity;
};
