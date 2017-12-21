/*******************************************************************
** 文件名:	SimpleTimerAxis.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	7/12/2016
** 版  本:	1.0
** 描  述:	简单时间轴

			
********************************************************************/

#pragma once

#include "ITimerService.h"
#include "timerAxis.h"
#include "IFramework.h"

/**
    简单版时间轴
	这个时间轴不需要分配大量内存，当然检索效率会比base里的timerAxis要低
*/

class SimpleTimerAxis : public ITimerService,public TimerHandler
{
		TimerAxis * m_pAxis;
public:
		bool Create();

		bool Close();

		SimpleTimerAxis() : m_pAxis(0){}

		virtual ~SimpleTimerAxis()
		{
			Close();
		}

		/**
		@purpose          : 设置一个定时器
		@param	 timerID  : 定时器ID
		@param   interval : 定时器调用间隔
		@param   handler  : 处理接口
		@param   callTimes: 调用次数,默认调用无穷次
		@param   debugInfo: 调试信息
		@return		      : 如果设置成功则返回true
		*/
		virtual bool SetTimer( DWORD timerID,DWORD interval,ITimerHandler * handler,DWORD callTimes=INFINITY_CALL,const char * debugInfo=0 )
		{
			return m_pAxis->SetTimer( timerID,interval,handler,callTimes,debugInfo );
		}

		/**
		@purpose          : 删除定时器
		@param	 timerID  : 定时器ID
		@param   handler  : 处理接口
		@return		      : 返回是否删除成功
		*/
		virtual bool KillTimer( DWORD timerID,ITimerHandler * handler )
		{
			return m_pAxis->KillTimer( timerID,handler );
		}

		/**
		@purpose          : 检查定时器是否需要触发
		@param timeout    : 单次执行时间片
		*/
		virtual void CheckTimer(ulong timeout = 0)
		{
			m_pAxis->CheckTimer(timeout);
		}

		/**
		取得时间轴对象
		@desc : 有些老系统，例如网关连接器里用的是老时间轴对象，所以还是暴露一个接口允许传出去
		*/
		virtual void * getAxis()
		{
			return m_pAxis;
		}

		virtual void OnTimer( unsigned long dwTimerID )
		{
			CheckTimer(100);
		}
};