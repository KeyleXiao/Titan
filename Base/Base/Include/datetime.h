//==========================================================================
/**
* @file	  : datetime.h
* @author : PeakGao <peakgao163@163.com>
* created : 2009-12-7   11:27
* purpose : 日期时间相关
*/
//==========================================================================

#ifndef __datetime_h__
#define __datetime_h__

#include "Common.h"
#include <time.h>


namespace rkt {

	/// 时间片段
	class RKT_EXPORT TimeSpan
	{
	public:
		TimeSpan();
		TimeSpan(__time64_t time);
		TimeSpan(int days, int hours, int mins, int secs);

		int64 getDays() const;

		int64 getTotalHours() const;
		int getHours() const;

		int64 getTotalMinutes() const;
		int getMinutes() const;

		int64 getTotalSeconds() const;
		int getSeconds() const;

		__time64_t getTimeSpan() const;

		TimeSpan operator+(TimeSpan span) const;
		TimeSpan operator-(TimeSpan span) const;
		TimeSpan& operator+=(TimeSpan span);
		TimeSpan& operator-=(TimeSpan span);
		bool operator==(TimeSpan span) const;
		bool operator!=(TimeSpan span) const;
		bool operator<(TimeSpan span) const;
		bool operator>(TimeSpan span) const;
		bool operator<=(TimeSpan span) const;
		bool operator>=(TimeSpan span) const;

	private:
		__time64_t m_timeSpan;
	};

	/// 时间
	class RKT_EXPORT Time
	{
	public:
		static Time getCurrentTime();

		Time();
		Time(__time64_t time);

		/** 构造指定的日期时间
		@param year		年(如 2009)
		@param month	月(1-12)
		@param day		日(1-31)
		@param hour		时(0-23)
		@param minute	分(0-59)
		@param second	秒(0-59)
		*/
		Time(int year, int month, int day, int hour, int minute, int second);

		Time& operator=(__time64_t time);

		Time& operator+=(TimeSpan span);
		Time& operator-=(TimeSpan span);

		TimeSpan operator-(Time time) const;
		Time operator-(TimeSpan span) const;
		Time operator+(TimeSpan span) const;

		bool operator==(Time time) const;
		bool operator!=(Time time) const;
		bool operator<(Time time) const;
		bool operator>(Time time) const;
		bool operator<=(Time time) const;
		bool operator>=(Time time) const;

		struct tm* getGmtTm(struct tm* ptm) const;
		struct tm* getLocalTm(struct tm* ptm) const;

		__time64_t getTime() const;

		int getYear() const;
		int getMonth() const;
		int getDay() const;
		int getHour() const;
		int getMinute() const;
		int getSecond() const;
		int getDayOfWeek() const;

	private:
		__time64_t m_time;
	};


	/// 获取虚拟时间
	RKT_API time_t getVirtualTime(time_t*);

	/// 获取本地虚拟时间
	RKT_API struct tm* getLocalVirtualTime();


	/** 设置虚拟的日期时间(虚拟时间，实际不修改系统的时间)
	@param year		年(如 2009)
	@param month	月(1-12)
	@param day		日(1-31)
	@param hour		时(0-23)
	@param minute	分(0-59)
	@param second	秒(0-59)
	*/
	RKT_API void setVirtualTime(int year, int month, int day, int hour, int minute, int second);

	/// 清除本地虚拟时间
	RKT_API void clearVirtualTime();

} // rkt


#endif // __datetime_h__