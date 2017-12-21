/*******************************************************************
** 文件名:	PerformanceMonitor.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-10-31
** 版  本:	1.0
** 描  述:	用于简单性能监控


**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#pragma once

#ifndef __PERFORMANCEMONITOR_H__ 
#define __PERFORMANCEMONITOR_H__

#include "EntityDef.h"
#include "IClientGlobal.h"

//////////////////////////////////////////////////////////////////////////
// 实体消息Warnling报警时间阀值 ms
#	define ENTITY_MESSAGE_WARNING_TIME_INTVAL			100
// 实体消息Error报警时间阀值 ms
#	define ENTITY_MESSAGE_ERROR_TIME_INTVAL				200

// 函数执行Warnling报警时间阀值 ms
#	define FUNCTION_WARNING_TIMER_INTVAL				50
// 函数执行Error报警时间阀值 ms
#	define FUNCTION_ERROR_TIME_INTVAL					1000

// 检测并输出信息
inline void checkTimeForAlert(DWORD dwBegin, DWORD dwEnd, const char* szError, DWORD dwMinAlert, DWORD dwMaxAlert, const char* szFunction, int nFileLine)
{
	if( dwEnd-dwBegin >= ENTITY_MESSAGE_ERROR_TIME_INTVAL )
	{
		ErrorLn(szFunction << szError <<", run " <<(dwEnd-dwBegin) <<" ms!!");
	}
	else if( dwEnd-dwBegin >= ENTITY_MESSAGE_WARNING_TIME_INTVAL )
	{
		WarningLn(szFunction << szError <<", run " <<(dwEnd-dwBegin) <<" ms");
	}
}

#ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
// 实体消息执行报警
#	define CHECK_ENTITY_MSG_ALERT(begin, end, szError)	checkTimeForAlert(begin,end, szError,ENTITY_MESSAGE_WARNING_TIME_INTVAL,ENTITY_MESSAGE_ERROR_TIME_INTVAL, __FUNCTION__,__LINE__)
// 函数执行报警
#	define CHECK_FUNCTION_ALERT(begin, end, szError)	checkTimeForAlert(begin,end, szError,FUNCTION_WARNING_TIMER_INTVAL,FUNCTION_ERROR_TIME_INTVAL, __FUNCTION__,__LINE__)
#else
// 实体消息执行报警
#	define CHECK_ENTITY_MSG_ALERT(begin, end, szError)
// 函数执行报警
#	define CHECK_FUNCTION_ALERT(begin, end, szError)
#endif


// 函数执行开始
#define PERFORMANCE_FUNCTION_BEGIN()	\
	unsigned long dwMonitor_Check_Start_Tick = getTickCount();
// 函数结束
#define	PERFORMANCE_FUNCTION_END(szAlert)	\
	CHECK_FUNCTION_ALERT(dwMonitor_Check_Start_Tick,getTickCount(), szAlert);








#endif	// __PERFORMANCEMONITOR_H__


