/*******************************************************************
** 文件名:	EffectShowPromptUI.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/05/2016
** 版  本:	1.0
** 描  述:	效果-显示提示UI
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;

class CEffectShowPromptUI : public IEffect
{
public:
	typedef  EffectClient_ShowPromptUI    SCHEME_DATA;

	CEffectShowPromptUI( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

        m_pEntity = context->pEntity;

        cmd_creature_ShowPromptUI ShowPromptUI;
        ShowPromptUI.nType = 1;
        ShowPromptUI.nPromptID = m_data.nPromptID;
        ShowPromptUI.uid = m_pEntity->getUID();
		m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SHOW_PROMPT_UI, 0, 0, &ShowPromptUI, sizeof(ShowPromptUI));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            cmd_creature_ShowPromptUI ShowPromptUI;
            ShowPromptUI.nType = 0;
            ShowPromptUI.nPromptID = m_data.nPromptID;
            ShowPromptUI.uid = m_pEntity->getUID();
            m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SHOW_PROMPT_UI, 0, 0, &ShowPromptUI, sizeof(ShowPromptUI));

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectShowPromptUI(m_data);
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
