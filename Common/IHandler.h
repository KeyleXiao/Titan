/*******************************************************************
** 文件名:	IHandler.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/27
** 版  本:	1.0
** 描  述:	消息处理接口
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "DGlobalMessage.h"

template <class TYPE>
class IHandler
{
public:
	// 模块号
	virtual	BYTE	GetModuleId() = 0;
	// 消息号
	virtual	BYTE	GetActionId() = 0;
	// 分发框架中使用的接口，使用者不需要使用
	virtual	void	_Handle(TYPE* pSession, SGameMsgHead* head, void* data, size_t len) = 0;
};
