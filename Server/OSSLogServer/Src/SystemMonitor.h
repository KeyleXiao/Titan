/*******************************************************************
** 文件名:	SystemMonitor.h 
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


#ifndef __OSS_SRC_SYSTEMMONITOR_H__
#define __OSS_SRC_SYSTEMMONITOR_H__


#include <map>
#include <list>
#include "OssLogDef.h"
#include "IGameDatabaseDef.h"

class CSystemMonitor
{
	typedef map<int, DBREQ_OSS_MONITOR_TASK> TMAP_TASK;

public:
	// 任务事件触发后填写信息以便写日志
	void AddTaskInfo(int nTaskID, int nAcceptNum, int nFinishNum, int nGiveUpNum);

	// 定时器触发后回调,你可以在这里编写处理代码
	void OnTimer();

	// 保存数据
	void SaveAllData();

private:
	// 批量存储任务数据
	void BatchSaveTaskData(); 

	// 保存日志
	bool SaveToVan(int nDbDmdID, void * pLog, size_t nLen);

	// 取上一小时的整点值
	void GetTimeString(char* strTime, int nLen);

public:
	CSystemMonitor();

	~CSystemMonitor();

	// 创建
	bool Create();

	// 释放
	void Release();

private:
	// 监控 任务数据
	TMAP_TASK m_mapTask;

	int m_nMonitorTaskLastSaveTime;
};


#endif// __OSS_SRC_SYSTEMMONITOR_H__