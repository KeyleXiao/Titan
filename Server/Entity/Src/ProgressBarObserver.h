/*******************************************************************
** 文件名:	ProgressBar.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/11/23
** 版  本:	1.0
** 描  述:	进度条观察者

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IEntityEvent.h"


struct __IEntity;

class CProgressBarObserver : public IEventExecuteSink
{
public:
	CProgressBarObserver(void);

	virtual ~CProgressBarObserver(void);

public:
	///////////////////////////////// IEventExecuteSink ////////////////////////////
	/**
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return
	@note
	@warning
	@retval buffer
	*/
	virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	// 创建
	virtual bool create(__IEntity * pMaster);

	// 释放
	virtual void release(void);

	// 启动
	virtual void start(int nSpellID, int nReverse);

	// 停止
	virtual void stop();

	// 进度条完成
	virtual void onProgressOK() {};

	// 进度条被中断
	virtual void onProgressBreak() {};

private:
	__IEntity* m_pMaster;
};
