/*******************************************************************
** 文件名:	EffectSwitchBulletPanel.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	7/21/2017
** 版  本:	1.0
** 描  述:	效果-开关子弹面板
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

	// 效果启用
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

	// 效果停止
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

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectSwitchBulletPanel(m_data);
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
    bool                      m_bStart;
};
