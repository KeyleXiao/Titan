/*******************************************************************
** 文件名:	ISceneEventService.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-10
** 版  本:	1.0
** 描  述:	

		单个场景事件接口
********************************************************************/

#pragma once

#include "IEventEngine.h"
using namespace rkt;

struct SEventServiceKey
{
	explicit SEventServiceKey(WORD	wEventID, BYTE byType)
	{
		m_dwValue = (wEventID << 16) + byType;
	}

	operator DWORD()
	{
		return m_dwValue;
	}
private:
	DWORD	m_dwValue;
};

// 单个场景事件接口
struct ISceneEventService: public rkt::IEventEngine
{
	// 是否可调度到场景逻辑执行
	virtual bool canDispatch(WORD wEventID, BYTE byType) = 0;

	// 释放
	virtual void release() = 0;
};