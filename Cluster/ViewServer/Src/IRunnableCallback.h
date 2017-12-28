/*******************************************************************
** 文件名:	IRunnableCallback.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/23
** 版  本:	1.0
** 描  述:	继承此类的对象的创建和销毁，都应该在逻辑线程中
** 应  用:  往返Redis线程的基类
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "ICallback.h"


struct IRunnableCallback : public ICallback, public IRunnable
{
	IRunnableCallback() : isDone(false)
	{}

	bool	IsDone() { return isDone; }
	void	SetDone() { isDone = true; }

	virtual	bool NeedRecovery() = 0;
	// 是否需要再次投递到Redis线程
	virtual bool NeedSpawnAgain() { return false; }

	void	OnSpawn()
	{
		UnDone();

		_OnSpawn();
	}

protected:
	virtual	void	_OnSpawn() = 0;

private:
	void	UnDone() { isDone = false; }

protected:
	bool isDone;
};
