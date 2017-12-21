/*******************************************************************
** 文件名:	DBEngineService.h  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-25
** 版  本:	1.0
** 描  述:	DBEngine服务			
********************************************************************/

#pragma once

#ifndef __SOCIAL_SOCIALGLOBAL_SRC_DBENGINESERVICE_H__
#define __SOCIAL_SOCIALGLOBAL_SRC_DBENGINESERVICE_H__


#include "IServiceMgr.h"
#include "IFramework.h"
#include "ISocialGlobal.h"
#include "IDBEngineService.h"
#include "DBEngineStateSink.h"
#include "LogicThread.h"
#include "IResourceManager.h"
#include "IServerController.h"


#include "Simple_Atom_Lock.h"

// 数据库服务器配置
#define DBSERVER_SCHEME_FILENAME		"Scp\\DBServer.dbs"

// 数据库存储过程配置
#define DBPROCEDURE_SCHEME_FILENAME		"Scp\\DBProcedure.dbs"


// DBEngine服务	
class CDBEngineService : public IDBEngineService
{
private:
	volatile	LONG			m_mutex;
	IDBEngine*					m_pDBEngine;
	CDBEngineStateSink			m_DBEngineStateSink;
	ThreadPool					m_ThreadPool;
	CLogicThread				m_LogicThread;

public:
	CDBEngineService() : m_mutex(0), m_pDBEngine(0)
	{

	}

	virtual ~CDBEngineService()
	{

	}

	bool Start()
	{
		char szLogAbsolutePath[512] = { 0 };
		sprintf_s(szLogAbsolutePath, "%s\\Log\\", getWorkDir());
		char szDBLogPath[512] = { 0 };sprintf_s(szDBLogPath, "%s\\SocialDBRunLog.html", szLogAbsolutePath);
		char szPerforLog[512] = { 0 };sprintf_s(szPerforLog, "%s\\SocialDBPerforLog.html", szLogAbsolutePath);
		string stringDBServerPath = DBSERVER_SCHEME_FILENAME; string stringProcedurePath = DBPROCEDURE_SCHEME_FILENAME;

#ifdef USE_DBSERVER_FILE
        m_pDBEngine = CreateDBEngineProc(&m_ThreadPool
            ,&m_LogicThread
            ,stringDBServerPath.c_str()
            ,stringProcedurePath.c_str()
            ,MSG_ENDPOINT_SOCIAL
            ,szDBLogPath
            ,szPerforLog
            ,getThisGameWorldID()
            ,isPublicGameWorld()
            );
#else
        void * pDataBuff = 0;
        int nDataSize = gSocialGlobal->getResourceManager()->getData(RESOURCE_TYPE_DBSERVER, &pDataBuff);
        if(nDataSize == 0)
        {
            return false;
        }

        m_pDBEngine = CreateDBEngineExProc(&m_ThreadPool
            ,&m_LogicThread
            ,pDataBuff
            ,nDataSize
            ,stringProcedurePath.c_str()
            ,MSG_ENDPOINT_SOCIAL
            ,szDBLogPath
            ,szPerforLog
            ,getThisGameWorldID()
            ,isPublicGameWorld()
            );
                
        pDataBuff = 0;
#endif


		if(m_pDBEngine == NULL)
		{
			ErrorLn("create DBEngine fail!");
			return false;
		}

		// 添加状态信息回调
		m_pDBEngine->AddStateSink(&m_DBEngineStateSink);

		// 创建逻辑线程
		m_ThreadPool.add(static_cast<IRunnable *>(&m_LogicThread), THREAD_PRIORITY_HIGHEST);

		// DB引擎服务已启动
		IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
		if(pEventEngine)
		{
			pEventEngine->FireExecute(EVENT_SYSTEM_DBENGINE_STARTED, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
		}

		TraceLn("create DBEngine success.");
		return true;
	}

	void Stop()
	{
		// DB引擎服务已关闭
		IEventEngine * pEventEngine = gSocialGlobal->getEventEngine();
		if (pEventEngine != NULL)
		{
			pEventEngine->FireExecute(EVENT_SYSTEM_DBENGINE_STOP, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
		}

        if(m_pDBEngine)
        {
		    m_pDBEngine->RemoveStateSink(&m_DBEngineStateSink);
        }
		
		// 停止逻辑线程
		m_LogicThread.StopThread();
		EmphasisLn("Stop DBEngine logic thread..");

		// 数据库引擎
		SAFE_RELEASE(m_pDBEngine);
	}

	virtual void release()
	{
		Stop();
		delete this;
	}


	////////////// IDBEngineService ////////////////////////////////////////////////////////////
	/** 注册处理者
	@param pSink 数据库请求返回回调接口
	*/
	virtual bool registerHandler(IDBRetSink* pSink, const char* pszDesc)
	{
		return true;
	}

	/** 注销注册处理者
	@param pSink 数据库请求返回回调接口
	*/
	virtual bool unregisterHandler(IDBRetSink* pSink)
	{
		return true;
	}


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
	virtual bool executeSP(DWORD dwCmdID, DWORD dwQueueIndex, LPCSTR pszInData, int nInDataLen, IDBRetSink * pDBRetSink, int nGameWorldID)
	{
		return m_pDBEngine ? m_pDBEngine->ExecuteSP(dwCmdID, dwQueueIndex, pszInData, nInDataLen, pDBRetSink, pDBRetSink, nGameWorldID) : false;
	}

    
	/** 输出队列
	*/
    virtual void  outputQueue()
    {
        TraceLn("-------------------- BEGIN OUTPUT DB QUEUE INFO ----------------------");
        if(m_pDBEngine != NULL)
        {
            SAsynQueueStateInfo tempStateInfo[256]; int nArrayCount = 256;
            if(!m_pDBEngine->GetAsynQueueRunState(tempStateInfo, nArrayCount))
            {
                ErrorLn(__FUNCTION__": ARRAY IS SHORT, ONLY GET MANY QUEUE INFO.");
            }

            char szTrace[256] = { 0 };
            for(int i = 0; i < nArrayCount; i++)
            {                
                sprintf_s(szTrace, "%s, DB_ID=%d, QUEUE_ID=%d, RUN_QUEUE_QTY=%d, RETRY_QUEUE_QTY=%d.", a2utf8(tempStateInfo[i].szDataBaseNickName),
                    tempStateInfo[i].dwDataBaseID, tempStateInfo[i].dwQueueID, tempStateInfo[i].nRunRequestQty, tempStateInfo[i].nRetryRequestQty);

                TraceLn(szTrace);
            }
        }
        TraceLn("-------------------- END OUTPUT DB QUEUE INFO ----------------------");
    }
};


#endif  // __SOCIAL_SOCIALGLOBAL_SRC_DBENGINESERVICE_H__
