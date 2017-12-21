/*******************************************************************
** 文件名:	SystemMonitor.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华 (Carl Chen)
** 日  期:	2016-7-4
** 版  本:	1.0
** 描  述:  记录日志
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "SystemMonitor.h"
#include "OssLogDef.h"
#include "time.h"
#include "IDBEngine.h"
#include "OSSLogServer.h"


CSystemMonitor::CSystemMonitor()
{
	m_mapTask.clear();
	m_nMonitorTaskLastSaveTime = 0;
}

CSystemMonitor::~CSystemMonitor()
{

}

bool CSystemMonitor::Create()
{
	return true;
}

void CSystemMonitor::Release()
{

}

// 任务事件触发后填写信息以便写日志
void CSystemMonitor::AddTaskInfo(int nTaskID, int nAcceptNum, int nFinishNum, int nGiveUpNum)
{
	DBREQ_OSS_MONITOR_TASK& node = m_mapTask[nTaskID];
	node.nTaskID = nTaskID;
	node.nAcceptNum += nAcceptNum;
	node.nFinishNum += nFinishNum;
	node.nGiveUpNum += nGiveUpNum;

	if ((m_mapTask.size() + 2) * sizeof(DBREQ_OSS_MONITOR_TASK) + sizeof(DBREQ_PARAM_OSS_MONITOR_TASK) >= 7*1024)
	{
		BatchSaveTaskData();
	}
}

// 定时器触发后回调,你可以在这里编写处理代码
void CSystemMonitor::OnTimer()
{
	time_t long_time;
	time(&long_time);
	int curTime = (int)long_time;

	// 监控 任务数据
	if (curTime - m_nMonitorTaskLastSaveTime >= OSS_SAVE_MONITOR_TASK_INTERVAL)
	{
		BatchSaveTaskData();
		m_nMonitorTaskLastSaveTime = curTime;
	}
}

// 保存数据
void CSystemMonitor::SaveAllData()
{
    TraceLn("All Logs Save: task.....");
	BatchSaveTaskData();
}

// 批量存储任务数据
void CSystemMonitor::BatchSaveTaskData()
{
	if (m_mapTask.empty())
	{
		return;
	}

	time_t long_time; time(&long_time); int nCurTime = (int)long_time;

	obuf obLog;
	DBREQ_PARAM_OSS_MONITOR_TASK log;
	obuf obData;
	size_t nDataLen = 0;

	for (TMAP_TASK::iterator iter = m_mapTask.begin(); iter != m_mapTask.end(); ++iter)
	{
		if (nDataLen >= 7*1024)
		{
			int nLogLength = log.nNum * sizeof(DBREQ_OSS_MONITOR_TASK);
			log.nServerID = gServerGlobal->getServerID();
			GetTimeString(log.szTime, sizeof(log.szTime));
			obLog.push_back(&log, sizeof(log));
			obLog.push_back(&nLogLength, sizeof(nLogLength));
			obLog.push_back(obData.data(), obData.size());
			SaveToVan(GAMEDB_REQUEST_MONITOR_TASK, obLog.data(), obLog.size());

			obLog.clear();
			obData.clear();
			log.nNum = 0;
			nDataLen = 0;
		}

		DBREQ_OSS_MONITOR_TASK & item = iter->second;
		obData.push_back(&item, sizeof(item));
		nDataLen += sizeof(item);
		++log.nNum;
	}

	if (log.nNum > 0)
	{
		int nLogLength = log.nNum * sizeof(DBREQ_OSS_MONITOR_TASK);
		log.nServerID = gServerGlobal->getServerID();
		GetTimeString(log.szTime, sizeof(log.szTime));
		obLog.push_back(&log, sizeof(log));
		obLog.push_back(&nLogLength, sizeof(nLogLength));
		obLog.push_back(obData.data(), obData.size());
		SaveToVan(GAMEDB_REQUEST_MONITOR_TASK, obLog.data(), obLog.size());

		obLog.clear();
		obData.clear();
		log.nNum = 0;
		nDataLen = 0;
	}

	// 清空当前对象数值数据
	m_mapTask.clear();
}

// 保存日志
bool CSystemMonitor::SaveToVan(int nDbDmdID, void * pLog, size_t nLen)
{
	return g_pOssLogServer->executeSP(nDbDmdID, (LPSTR)pLog, (int)nLen);
}

// 取上一小时的整点值
void CSystemMonitor::GetTimeString(char* strTime, int nLen)
{
	if (nLen<=12)
	{
		ErrorLn("CSystemMonitor::GetTimeString strTime太小！");
		return;
	}
	time_t t;
	time(&t);

	t = t-3600;
	tm local;
	memset(&local,0,sizeof(local));
	localtime_s(&local,&t);

	sprintf_s(strTime, nLen, "%d%.2d%.2d%.2d%.2d",1900+local.tm_year,local.tm_mon+1,local.tm_mday,local.tm_hour,0);
}