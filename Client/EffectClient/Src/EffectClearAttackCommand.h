/*******************************************************************
** 文件名:	EffectClearAttackCommand.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭政林
** 日  期:	5/31/2016
** 版  本:	1.0
** 描  述:	效果-清除攻击命令
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IClientEntity.h"
#include "IHandleCommand.h"
using namespace EFFECT_CLIENT;

class CEffectClearAttackCommand : public IEffect
{
public:
	typedef EffectClient_ClearAttackCommand SCHEME_DATA;

	CEffectClearAttackCommand( SCHEME_DATA & data ):m_data(data)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT *pContext )
	{
		if ( pContext==0 || pContext->pEntity==0 )
			return false;

        IHandleCommandManager *pHandleCommandManager = gClientGlobal->getHandleCommandManager();
        if (pHandleCommandManager == NULL)
        {
            return false;
        }
        // 获取命令工厂接口
        IHandleCommandFactory *pHandleCommandFactory = pHandleCommandManager->getHandleCommandFactory();
        if (pHandleCommandFactory == NULL)
        {
            return false;
        }
        // 清除攻击命令
        pHandleCommandFactory->clearAttackCommand();
        // 清除命令队列
        pHandleCommandManager->clear();

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectClearAttackCommand(m_data); }

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
