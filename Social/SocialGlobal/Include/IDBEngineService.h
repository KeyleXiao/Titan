/*******************************************************************
** 文件名:	IDBEngineService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-25
** 版  本:	1.0
** 描  述:	DBEngine服务			
********************************************************************/

#pragma once

#ifndef __SOCIAL_SOCIALGLOBAL_INCLUDE_IDBENGINESERVICE_H__
#define __SOCIAL_SOCIALGLOBAL_INCLUDE_IDBENGINESERVICE_H__

#include "IDBEngine.h"


// DBEngine服务
struct IDBEngineService
{
	/** 注册处理者
	@param pSink 数据库请求返回回调接口
	*/
	virtual bool			registerHandler(IDBRetSink* pSink, const char* pszDesc = "") = 0;

	/** 注销注册处理者
	@param pSink 数据库请求返回回调接口
	*/
	virtual bool			unregisterHandler(IDBRetSink* pSink) = 0;

	/** 执行一个存储过程
	@param   dwCmdID		：cmdid
	@param   dwQueueIndex	: 异步队列索引号
	@param   pszInData		：输入数据
	@param   nInDataLen		：输入数据长度
	@param   pDBRetSink		：结果返回接口	
	@param   nGameWorldID	: 向何个游戏世界数据库执行，若为公共区,必填游戏世界id
	@return  		
	@retval buffer 
	*/
	virtual bool			executeSP(DWORD dwCmdID, DWORD dwQueueIndex, LPCSTR pszInData, int nInDataLen, IDBRetSink * pDBRetSink, int nGameWorldID = 0) = 0;

	/** 输出队列
	*/
    virtual void            outputQueue() = 0;

};


#endif  // __SOCIAL_SOCIALGLOBAL_INCLUDE_IDBENGINESERVICE_H__

