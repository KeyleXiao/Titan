/*******************************************************************
** �ļ���:	EffectClient_Roll2Target.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	ljw
** ��  ��:	6/30/2017
** ��  ��:	1.0
** ��  ��:	����Ŀ�귭��
** Ӧ  ��:  ��ҷ����ǿͻ��˷���Ȼ��ͬ��λ�õ��������� ��������λ��.
			����mode2��״̬��ֻ�ڿͻ��˿��Ƶ� �����������ʱ���ܾ�ȷ����. AI������ʱ���������Ϊ��֪��������ȷ��ѡ��ǰ�����ҷ�������
			����AI�ķ���������Ҫ������һ�� �����������������Ŀ��ص� ���ڲ���ǰ�����Ǻ������ ����ʾ���Լ����ݵ�ǰ�����Ŀ��ص��Լ�����
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IClientEntity.h"
#include "IClientGlobal.h"
#include "IFreezeClient.h "
using namespace EFFECT_CLIENT;

class CEffectRoll2Target : public IEffect
{
public:
	typedef EffectClient_Roll2Target SCHEME_DATA;

	CEffectRoll2Target(SCHEME_DATA & data) :m_data(data)
	{
	}

	// Ч������
	virtual bool			Start(EFFECT_CONTEXT * pContext)
	{
		if (pContext == 0 || pContext->pEntity == 0)
			return false;

		cmd_roll_2_target cmd;
		cmd.loc[0] = pContext->ptTargetTile.x;
		cmd.loc[1] = pContext->ptTargetTile.y;
		cmd.loc[2] = pContext->ptTargetTile.z;

		// ���ü��ܲ�
		pContext->pEntity->sendCommandToEntityView(ENTITY_TOVIEW_AI_ROLL, 0, 0, &cmd, sizeof(cmd));

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() { return new CEffectRoll2Target(m_data); }

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
};