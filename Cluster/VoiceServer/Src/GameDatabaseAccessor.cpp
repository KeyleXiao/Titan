/*******************************************************************
** 文件名:	GameDatabaseAccessor.cpp 
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

#include "stdafx.h"
#include "GameDatabaseAccessor.h"
#include "IVoiceDataBaseDef.h"

bool GameDatabaseAccessor::Create(const char * szDBConfigFile,const char * szDBProcedureConfigFile)
{	
	// trace文件地址	
	time_t	long_time; struct tm * local_time;
	time(&long_time); local_time = localtime(&long_time);

	char szDBLogPath[512] = { 0 };
	sprintf_s(szDBLogPath, "%s\\Log\\VoiceServer\\Voice%d%.2d%.2d%.2d%.2d%.2d_DBRunLog.html", getWorkDir(),
		local_time->tm_year+1900,
		local_time->tm_mon + 1,
		local_time->tm_mday,
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec);

	char szPerforLog[512] = { 0 };
	sprintf_s(szPerforLog, "%s\\Log\\VoiceServer\\Voice%d%.2d%.2d%.2d%.2d%.2d_PerforRunLog.html", getWorkDir(),
		local_time->tm_year+1900,
		local_time->tm_mon + 1,
		local_time->tm_mday,
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec);
	m_pDBEngine = CreateDBEngineProc(&m_ThreadPool,this,szDBConfigFile,szDBProcedureConfigFile,VOICE_SERVER_DBID, szDBLogPath, szPerforLog,0,false);
	if ( m_pDBEngine==0 )
	{
		ErrorLn("Create database engine failed! config file: " << szDBConfigFile <<","<< szDBProcedureConfigFile );
		return false;
	}

	TraceLn("Database engine initialize ok!");
	return true;
}

bool GameDatabaseAccessor::Create(int nServerType, LPVOID pDataBuff, size_t nDataSize, const char * szDBProcedureConfigFile)
{
    if(pDataBuff == NULL || nDataSize == 0)
    {
        ErrorLn(__FUNCTION__": Load DBEngine From Memory Data Failure!!!");
        return false;
    }

    // trace文件地址	
    time_t	long_time; struct tm * local_time;
    time(&long_time); local_time = localtime(&long_time);

    char szDBLogPath[512] = { 0 };
    sprintf_s(szDBLogPath, "%s\\Log\\VoiceServer\\Voice%d%.2d%.2d%.2d%.2d%.2d_DBRunLog.html", getWorkDir(),
        local_time->tm_year+1900,
        local_time->tm_mon + 1,
        local_time->tm_mday,
        local_time->tm_hour,
        local_time->tm_min,
        local_time->tm_sec);

    char szPerforLog[512] = { 0 };
    sprintf_s(szPerforLog, "%s\\Log\\VoiceServer\\Voice%d%.2d%.2d%.2d%.2d%.2d_PerforRunLog.html", getWorkDir(),
        local_time->tm_year+1900,
        local_time->tm_mon + 1,
        local_time->tm_mday,
        local_time->tm_hour,
        local_time->tm_min,
        local_time->tm_sec);


    m_pDBEngine = CreateDBEngineExProc(&m_ThreadPool
        ,this
        ,pDataBuff
        ,nDataSize
        ,szDBProcedureConfigFile
        ,nServerType
        ,szDBLogPath
        ,szPerforLog
        ,getThisGameWorldID()
        ,isPublicGameWorld()
        );
    if ( m_pDBEngine==0 )
    {
        ErrorLn("Create database engine failed!" );
        return false;
    }

    TraceLn("Database engine initialize ok!");
    return true;
}

void GameDatabaseAccessor::Close()
{
	if ( m_pDBEngine )
	{
		m_pDBEngine->Release();
		m_pDBEngine = 0;
	}
}


void GameDatabaseAccessor::OnState(int nInfoID, LPCSTR pszDesc)
{
	if(nInfoID == DBENGINE_STATEINFO_OK)
	{
		TraceLn(_GT("数据库引擎连接正常！！！INFOID ＝ ") <<nInfoID << _GT("描述：")<<pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_DISCONNECT)
	{
		ErrorLn(_GT("数据库引擎断开连接！！！INFOID ＝ ")<<nInfoID<<_GT("描述：")<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_RENEWCONNECT)
	{
		TraceLn(_GT("数据库引擎重连通过！！！INFOID ＝ ")<<nInfoID<<_GT("描述：")<<pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_THROW)
	{
		ErrorLn(_GT("数据库引擎抛掉请求！！！INFOID ＝ ")<<nInfoID<<_GT("描述：")<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_EXCEPTION)
	{
		ErrorLn(_GT("数据库引擎抛出异常！！！INFOID ＝ ")<<nInfoID<<_GT("描述：")<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_PERFORMANCE)
	{
		WarningLn(_GT("数据库引擎性能警告！！！INFOID ＝ ")<<nInfoID<<_GT("描述：")<<pszDesc);
		AlertBad(MG_AlertSrcType_Performance, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_GAMEWORLDERROR)
	{
		ErrorLn(_GT("数据库验证调错数据库！倒！INFOID ＝ ")<<nInfoID<<_GT("描述：")<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
	else
	{
		ErrorLn(_GT("数据库引擎返回信息！！！INFOID ＝ ")<<nInfoID<<_GT("描述：")<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
}

/// 默认的返回处理逻辑
void GameDatabaseAccessor::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if ( nDBRetCode!=DBRET_CODE_SUCCEED )
	{
		ErrorLn("nCmdID=" << nCmdID << " nDBRetCode=" << nDBRetCode);
		if ( pszDBRetDesc )
		{
			ErrorLn(pszDBRetDesc);
		}

		return;
	}
}

bool GameDatabaseAccessor::ExecuteSP(DWORD dwCmdID, DWORD dwQueueIndex, LPCSTR pszInData, int nInDataLen, IDBRetSink * pDBRetSink)
{
	if ( m_pDBEngine )
	{
		bool bNewProxy = false;
		// 默认处理
		if ( pDBRetSink==0 )
		{
			pDBRetSink = this;
		}
		else
		{
			// 标识指针有效
			m_mapDBRetSink[pDBRetSink]=true;

			bNewProxy = true;

			pDBRetSink = new DBRequestHandlerProxy(pDBRetSink);
			if (pDBRetSink == NULL)
			{
				return false;
			}
		}

		if( m_pDBEngine->ExecuteSP(dwCmdID,dwQueueIndex,pszInData,nInDataLen, pDBRetSink))
		{
			return true;
		}
		else
		{
			if (bNewProxy)
			{
				delete pDBRetSink;
			}
			return false;
		}
	}

	return false;


}


bool GameDatabaseAccessor::AttachTask(ILogicThreadTask * pTask, LPCSTR pszDebug)
{
	m_TaskList.insert(pTask);
	return true;
}

void GameDatabaseAccessor::DetachTask(ILogicThreadTask * pTask)
{
	m_TaskList.erase(pTask);
}

void GameDatabaseAccessor::FetchDatabaseResult()
{
	TASK_LIST::iterator it = m_TaskList.begin();
	for ( ;it!=m_TaskList.end();++it )
	{
		(*it)->OnDo();
	}
}

// 数据库引擎队列是否为空
bool GameDatabaseAccessor::IsDBEngineQueueEmpty()
{
	SAsynQueueStateInfo pInfoArray[64];
	int nArrayCount = 64;

	if( !m_pDBEngine->GetAsynQueueRunState( pInfoArray, nArrayCount ) )
	{
		ErrorLn( _GT("IsDBEngineQueueEmpty pInfoArray空间太小 nArrayCount=") << nArrayCount );
		return true;
	}

	for( int i = 0; i < nArrayCount; i ++ )
	{
		if( pInfoArray[i].nRunRequestQty + pInfoArray[i].nRetryRequestQty > 0 )
		{
			return false;
		}
	}

	return true;
}


/** 取消数据库返回回调
@param   
@param   
@return  
*/
void GameDatabaseAccessor::RemoveDBRetSink(IDBRetSink * pDBRetSink)
{
	m_mapDBRetSink.erase(pDBRetSink);
}

/** 判断数据库返回回调是否存在
@param   
@param   
@return  
*/
bool GameDatabaseAccessor::IsDBRetSinkExist(IDBRetSink * pDBRetSink)
{
	TMAP_DBSINK::iterator Iter = m_mapDBRetSink.find(pDBRetSink);
	if (Iter == m_mapDBRetSink.end())
	{
		return false;
	}

	return true;
}
