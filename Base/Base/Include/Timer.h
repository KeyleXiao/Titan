/**
* Created by PeakGao 2008.01.03
*/

#ifndef RKT_TIMER_H
#define RKT_TIMER_H

#include "Common.h"

#ifdef SUPPORT_TIMER

#include "Api.h"

namespace rkt {
/*
	/// 高性能时钟
	class RKT_EXPORT CTimer
	{
		int64 mBeginTime;	/// 时钟开始时间点
		int64 mEndTime;		/// 时钟结束时间点

	public:
		CTimer();

		/// 启动时钟
		void begin();

		/// 停止时钟
		void end();

		/// 返回毫秒数（整数型）
		ulong getTime() const;

		/// 返回毫秒数（Double型）
		double getTimeByDouble() const;
	};*/

	class RKT_EXPORT Timer
	{
		int64 mBeginCount;	/// 时钟开始时的计数

	public:
		Timer();

		/// 重置时钟
		void reset();

		/// 返回毫秒数（整数型）
		ulong getTime() const;

		/// 返回毫秒数（Double型）
		double getTimeByDouble() const;
	};

	class RKT_EXPORT TickCounter
	{
	public:
		TickCounter()
		{
			m_begin = getTickCount();
		}

		void reset() { m_begin = getTickCount();}

		ulong getTime() const
		{
			return getTickCount() - m_begin;
		}

		/// 返回毫秒数（Double型）
		double getTimeByDouble() const
		{
			return (double)(getTickCount() - m_begin);
		}

	private:
		ulong m_begin;
	};

}

#endif // #ifdef SUPPORT_TIMER

#endif // RKT_TIMER_H