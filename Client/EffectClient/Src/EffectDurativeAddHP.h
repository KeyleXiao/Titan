/*******************************************************************
** 文件名:	EffectDurativeAddHP.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	penglu
** 日  期:	2014/12/29
** 版  本:	1.0
** 描  述:	持续加减血效果类声明
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "TimerHandler.h"
using namespace EFFECT_DATA;

class CEffectDurativeAddHP : public IEffect, public TimerHandler
{
public:
	typedef  EffectData_DurativeAddHP    SCHEME_DATA;

	CEffectDurativeAddHP( SCHEME_DATA & data ) : m_data(data),m_uid(INVALID_UID){}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context );

	// 效果停止
	virtual void			Stop();

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectDurativeAddHP(m_data); }

	// 释放
	virtual void			Release();

	// ITimerHandler
	virtual void OnTimer( unsigned long dwTimerID );

private:
	SCHEME_DATA               m_data;
	UID                       m_uid;
};
