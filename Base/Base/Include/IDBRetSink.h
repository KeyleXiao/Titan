/*******************************************************************
** 文件名:	IDBRetSink.h  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-25
** 版  本:	1.0
** 描  述:	DBEngine服务			
********************************************************************/
#pragma once

#ifndef __BASE_BASE_INCLUDE_IDBRETSINK_H__
#define __BASE_BASE_INCLUDE_IDBRETSINK_H__


/// 数据库请求返回回调接口
struct IDBRetSink
{
	/** 数据库请求返回回调方法
	@param   pDBRetSink		：结果返回接口	
	@param   nCmdID ：命令ＩＤ
	@param   nDBRetCode：数据库请求返回值，参考上面定义
	@param   pszDBRetDesc：数据库请求返回描述，由ＳＰ返回
	@param   nQueueIndex：队列定义
	@param   pOutData：输出数据
	@param   nOutLen：输出数据长度
	@return  
	@note     
	@warning 此对像千万不能在数据库返回前释放，否则会非法！
	@retval buffer 
	*/	
	virtual void			OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen) = 0;

	/** 设置此次执行的游戏世界ID,只允许数据库引擎调用
	@param   
	@param   
	@return  本应该当作OnReturn参数给调用者，若现在加个参数，改动太大
	*/
	virtual void			SetExecuteGameWorldID(int nGameWorldID) { m_nExecuteGameWorldID =  nGameWorldID; }

	/** 取得对象创建时间
	@return
	*/
	virtual DWORD			GetCreateTime(void) { return m_dwCreateTime; }

	/** 校验是否合法，供应用层可以依据自身需要来进行特殊处理
	@param dwCmdID： 存储过程命令ID
	@param dwQueueIndex： 异步队列索引号
	@return
	*/
	virtual bool			IsValid(const DWORD	dwCmdID, const DWORD dwQueueIndex)		{ return true; }

    // 这个应用层不用实现，事件引擎内部的优化逻辑
    virtual void **		    GetProxyInfoPtr() { return &m_InfoPtr; }  

	IDBRetSink() : m_nExecuteGameWorldID(0), m_InfoPtr(0)
	{ 
		m_dwCreateTime = rkt::getTickCount();
	}

protected:
	int						m_nExecuteGameWorldID;
	// 对象创建时间
	DWORD					m_dwCreateTime;
    // 指向临时数据的指针
    void *                  m_InfoPtr; 
};


#endif  // __BASE_BASE_INCLUDE_IDBRETSINK_H__

