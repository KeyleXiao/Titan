/*******************************************************************
** �ļ���:	EffectSwitchBulletPanel.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	ljw
** ��  ��:	7/21/2017
** ��  ��:	1.0
** ��  ��:	Ч��-�����ӵ����
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;

class CEffectSwitchBulletPanel : public IEffect
{
public:
	typedef  EffectClient_SwitchBulletPanel    SCHEME_DATA;

	CEffectSwitchBulletPanel( SCHEME_DATA & data ) : m_data(data), m_bStart(false)
	{
	}

	// Ч������
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

        if(!context->pEntity->isHero())
            return false;

        m_bStart = true;

		cmd_switch_bullet_count_panel cmd;
		cmd.nOpen = /*m_data.eSwitch*/OPEN_BULLET_PANEL;
		cmd.nMaxCount = m_data.nMaxBulletNum;

		gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_SWITCH_BULLET_PANEL, 0,0,&cmd,sizeof(cmd));

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
        if(!m_bStart)
            return;

        cmd_switch_bullet_count_panel cmd;
        cmd.nOpen = CLOSE_BULLET_PANEL;
        cmd.nMaxCount = m_data.nMaxBulletNum;

        gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_SWITCH_BULLET_PANEL, 0,0,&cmd,sizeof(cmd));

        m_bStart = false;
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectSwitchBulletPanel(m_data);
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
    bool                      m_bStart;
};
