using System;
using System.Reflection;
using System.IO;
using System.Text;

struct ITimerHandler
{
};

interface TimerService
{
		/**
		@purpose          : 设置一个定时器
		@param	 timerID  : 定时器ID
		@param   interval : 定时器调用间隔
		@param   handler  : 处理接口
		@param   callTimes: 调用次数,默认调用无穷次
		@param   debugInfo: 调试信息
		@return		      : 如果设置成功则返回true
		*/
		unsafe bool SetTimer( UInt32 timerID,UInt32  interval,ITimerHandler * handler,UInt32 callTimes,string debugInfo );

		/**
		@purpose          : 删除定时器
		@param	 timerID  : 定时器ID
		@param   handler  : 处理接口
		@return		      : 返回是否删除成功
		*/
		unsafe bool KillTimer( UInt32  timerID,ITimerHandler * handler );

		void release();
};