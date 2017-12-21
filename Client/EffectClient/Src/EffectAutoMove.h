/*******************************************************************
** �ļ���:	EffectAutoMove.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	ljw
** ��  ��:	7/4/2017
** ��  ��:	1.0
** ��  ��:	#4970 �Զ�ǰ��Ч��
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IClientEntity.h"
#include "IClientGlobal.h"
#include "IFreezeClient.h "
using namespace EFFECT_CLIENT;

class CEffectAutoMove : public IEffect
{
public:
	typedef EffectClient_AutoMove SCHEME_DATA;

	CEffectAutoMove(SCHEME_DATA & data) :m_data(data), m_pEntity(NULL)
	{
	}

	// Ч������
	virtual bool			Start(EFFECT_CONTEXT * pContext)
	{
		if (pContext == 0 || pContext->pEntity == 0)
			return false;

		if (!pContext->pEntity->isHero())
			return false;

		m_pEntity = pContext->pEntity;

		cmd_auto_move cmd;
		cmd.nOpenFlag = 1;
		cmd.fStartSpeed = m_data.fStartSpeed;
		cmd.fAcceleration = m_data.fAcceleration;
		cmd.fMaxSpeed = m_data.fMaxSpeed;

		m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_AUTO_MOVE, 0, 0, &cmd, sizeof(cmd));

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != NULL)
		{
			cmd_auto_move cmd;
			cmd.nOpenFlag = 0;
			cmd.fStartSpeed = m_data.fStartSpeed;
			cmd.fAcceleration = m_data.fAcceleration;
			cmd.fMaxSpeed = m_data.fMaxSpeed;

			m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_AUTO_MOVE, 0, 0, &cmd, sizeof(cmd));

			m_pEntity = NULL;
		}
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() { return new CEffectAutoMove(m_data); }

	// �ͷ�
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	// ȡ��Ч��ID
	virtual int				GetEffectID() { return m_data.nID; }
private:
	SCHEME_DATA             m_data;
	IEntity*				m_pEntity;
};