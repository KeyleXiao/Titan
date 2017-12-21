/*******************************************************************
** 文件名:	GameDatabaseAccessor.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2013-03-20
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#ifndef __GAME_DATABASE_ACCESSOR_H__
#define __GAME_DATABASE_ACCESSOR_H__

#include "Thread.h"
#include "IDBEngine.h"
#include "Singleton.h"
#include "IGlobalVoiceServer.h"
#include "DBRequester.h"
using namespace rkt;

#include <set>
#include <map>


/**
@name : 数据库存取器
@brief: 对IDBEngine进行包装,负责初始化IDBEngine
*/

class GameDatabaseAccessor : public IDBEngineStateSink,public IDBRetSink,public ILogicThread,public SingletonEx<GameDatabaseAccessor>
{
	typedef map<IDBRetSink*, bool> TMAP_DBSINK;
public:
	//////////////////////////////////////////////////////////////////////////
	virtual bool AttachTask(ILogicThreadTask * pTask, LPCSTR pszDebug);

	virtual void DetachTask(ILogicThreadTask * pTask);

	virtual void OnState(int nInfoID, LPCSTR pszDesc);

	/// 默认的返回处理逻辑
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	/** 执行一个存储过程
	@param   dwCmdID：cmdid
	@param   dwQueueIndex:异步队列索引号
	@param   pszInData：输入数据
	@param   nInDataLen：输入数据长度
	@param   pDBRetSink：结果返回接口,如果你不关心执行结果,可以传入NULL,这样就会调用默认的OnReturn进行处理
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	bool	ExecuteSP(DWORD dwCmdID, DWORD dwQueueIndex, LPCSTR pszInData, int nInDataLen, IDBRetSink * pDBRetSink);

	bool    Create(const char * szDBConfigFile,const char * szDBProcedureConfigFile);
    // 用内存数据来创建
    bool    Create(int nServerType, LPVOID pDataBuff, size_t nDataSize, const char * szDBProcedureConfigFile);


	void    Close();

	/** 取消数据库返回回调
	@param   
	@param   
	@return  
	*/
	void	RemoveDBRetSink(IDBRetSink * pDBRetSink);

	/** 判断数据库返回回调是否存在
	@param   
	@param   
	@return  
	*/
	bool	IsDBRetSinkExist(IDBRetSink * pDBRetSink);

	///     提取数据库存取结果
	void    FetchDatabaseResult();

	// 数据库引擎队列是否为空
	bool IsDBEngineQueueEmpty();

	GameDatabaseAccessor() : m_pDBEngine(0){}

	virtual ~GameDatabaseAccessor() { Close();}

protected:

	typedef std::set<ILogicThreadTask *>  TASK_LIST;
	TASK_LIST						m_TaskList;

	ThreadPool						m_ThreadPool;
	IDBEngine  *					m_pDBEngine;
	TMAP_DBSINK					  m_mapDBRetSink;
};


/**
@name : 数据库请求代理处理器
@brief: 实现这个类的原因是:
        传给数据库引擎的处理接口(IDBRetSink)在数据库请求返回之前必须保证不能被销毁
		这在很多时候会对程序的结构有很大限制,
		所以这里实现一个代理类,该类在实际的请求类释放之后会自己销毁自己,使得实际请求类的逻辑更自由
		相关的协作约定是:
        1.每次调用OnReturn后,Proxy自己销毁自己,所以实际请求类在收到OnReturn后不能再记录Proxy的指针
		2.实际请求类释构时调用Proxy的Clear方法,这样当OnReturn到来时Proxy就可以安全的释放
*/
class DBRequestHandlerProxy : public IDBRetSink
{
public:
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
	{
		if ( m_pRealHandler)
		{
			DWORD dwNowTick = GetTickCount();

			if (GameDatabaseAccessor::getInstance().IsDBRetSinkExist(m_pRealHandler))
			{
				// 为同一个对象
				if (m_pRealHandler->GetCreateTime() == m_dwHandleCreateTime)
				{
					m_pRealHandler->OnReturn(pRealDBRetSink, nCmdID,nDBRetCode,pszDBRetDesc,nQueueIndex,pOutData,nOutLen);

					// 超过5秒报警
					if (dwNowTick > m_dwCallTick + 5000)
					{
						string strDBRetDesc;
						if (pszDBRetDesc != NULL)
						{
							strDBRetDesc = pszDBRetDesc;
						}
						WarningLn(_GT("调用数据返回时间过长,时长=")<< (dwNowTick-m_dwCallTick) << ", nCmdID="<< nCmdID <<", nQueueIndex="<<nQueueIndex <<",nDBRetCode="<< nDBRetCode << ", nOutLen="<<nOutLen << ", DBRetDesc="<< strDBRetDesc.c_str());
					}
				}
				else
				{
					string strDBRetDesc;
					if (pszDBRetDesc != NULL)
					{
						strDBRetDesc = pszDBRetDesc;
					}
					ErrorLn(_GT("数据库请求代理发现同一个地址要处理的不是同一个对象,nCmdID=")<< nCmdID <<", nQueueIndex="<<nQueueIndex <<",nDBRetCode="<< nDBRetCode << ", nOutLen="<<nOutLen << ", DBRetDesc="<< strDBRetDesc.c_str()<<_GT(",回调时间=")<<GetCreateTime()<<_GT(",对象时间=")<<m_pRealHandler->GetCreateTime() );
				}
			}
			else
			{
				string strDBRetDesc;
				if (pszDBRetDesc != NULL)
				{
					strDBRetDesc = pszDBRetDesc;
				}
				WarningLn(_GT("数据库请求代理没找到地址对象,时长=")<< (dwNowTick-m_dwCallTick) << ", nCmdID="<< nCmdID <<", nQueueIndex="<<nQueueIndex <<",nDBRetCode="<< nDBRetCode << ", nOutLen="<<nOutLen << ", DBRetDesc="<< strDBRetDesc.c_str());
			}
		}
		
		delete this;
	}

	void Clear(){ m_pRealHandler = 0; m_dwCallTick = 0;}

	DBRequestHandlerProxy(IDBRetSink * pHandler) : m_pRealHandler(pHandler)
	{
		m_dwCallTick = GetTickCount();
		if (m_pRealHandler!=NULL)
		{
			m_dwHandleCreateTime = m_pRealHandler->GetCreateTime();
		}
		else
		{
			m_dwHandleCreateTime = m_dwCallTick;
		}
	}

private:
	IDBRetSink   *    m_pRealHandler;
	DWORD			  m_dwCallTick;
	DWORD			m_dwHandleCreateTime;
};


#endif//__GAME_DATABASE_ACCESSOR_H__