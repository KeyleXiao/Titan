/*******************************************************************
** 文件名:	EffectShowCommonMessageBox.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	10/11/2016
** 版  本:	1.0
** 描  述:	效果-显示公共消息框
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IClientEntity.h"
#include "IClientGlobal.h"
#include "ISpell.h"
#include "GameViewDef.h"
using namespace EFFECT_CLIENT;

class CEffectShowCommonMessageBox : public IEffect
{
public:
	typedef EffectClient_ShowCommonMessageBox SCHEME_DATA;

	CEffectShowCommonMessageBox( SCHEME_DATA & data ):m_data(data)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT *pContext )
	{
		if ( pContext==0 || pContext->pEntity==0 )
        {
			return false;
        }

        IRenderViewProxy *pRenderView = gClientGlobal->getRenderView();
        if (pRenderView == NULL)
        {
            return false;
        }

		cmd_show_common_message_box box;
        box.nType = m_data.nType;
        sstrcpyn(box.szCaption, a2utf8(m_data.szCaption), sizeof(box.szCaption));
        sstrcpyn(box.szText, a2utf8(m_data.szText), sizeof(box.szText));
		pRenderView->sendControllerCommand(GVIEWCMD_SHOW_COMMON_MESSAGEBOX, 0, 0, &box, sizeof(box));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectShowCommonMessageBox(m_data); }

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
};
