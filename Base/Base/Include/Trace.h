/**
* Created by PeakGao 2008.01.02
*/

#ifndef RKT_TRACE_H
#define RKT_TRACE_H

#include "Common.h"

#ifdef SUPPORT_TRACE

#include "ostrbuf.h"

/**
	Trace设计要点：
	1、方便，仅几个宏直接使用；
	2、基于类型安全，取消诸如 printf 的不安全格式；
	3、输出信息可重定向；
	4、线程安全；
	5、支持输出级别；
*/

//#define LIMIT_FREQUENCY

#ifdef LIMIT_FREQUENCY
#include <windows.h>
#endif

namespace rkt {

	/// 消息级别
	enum TraceLevel
	{
		levelInfo		= 0x1,	/// 输出详细信息
		levelTrace		= 0x2,	/// 输出Trace信息
		levelWarning	= 0x4,	/// 输出警告信息
		levelError		= 0x8,	/// 输出错误信息
		levelEmphasis	= 0x10,	/// 输出用于强调的信息

		levelAll = levelInfo|levelTrace|levelWarning|levelError|levelEmphasis,
	};

	/// 消息接收器
	class RKT_EXPORT TraceListener
	{
	protected:
		int		level;
		bool	has_time;
	public:
		TraceListener() : level(levelAll), has_time(false) {}
		virtual ~TraceListener() {}

		void setTraceLevel(int lvl)		{ level = lvl; }
		int getTraceLevel() const		{ return level; }
		void setHasTime(bool hasTime)	{ has_time = hasTime; }
		bool hasTime() const			{ return has_time; }
		virtual void onTrace(const char* msg, const char* time, TraceLevel level) {}
	};

	RKT_API void createTrace(int level = levelAll, bool hasTime = true, bool threadSafe = true);
	RKT_API int setTraceLevel(int level);
	RKT_API int getTraceLevel();
	RKT_API int trackTraceContextMenu(int level);
	RKT_API bool setTraceHasTime(bool hasTime);
	RKT_API bool setTraceHasFileLine(bool hasFileLine);
	RKT_API bool setTraceHasLimitFrequency(bool limitFrequency);
	RKT_API bool hasLimitFrequency();
	RKT_API size_t setTraceMessageThreshold(size_t threshold);
	RKT_API size_t getTraceMessageThreshold();
	RKT_API void registerTrace(TraceListener* sink);
	RKT_API void unregisterTrace(TraceListener* sink);
	RKT_API void closeTrace();

	RKT_API TraceListener* output2Console(int level = levelAll, bool hasTime = false);
	RKT_API TraceListener* output2File(const char* filename = NULL, int level = levelAll, bool hasTime = false);
	RKT_API TraceListener* output2RichEdit(void* hwnd, int level = levelAll, bool hasTime = false);
	RKT_API TraceListener* output2Socket(const char* host = "127.0.0.1", ushort port = 1234, bool hasTime = false);

	RKT_API void addTraceToRichEdit(void* hWndRichEdit, const char* msg, TraceLevel level);
	RKT_API void dispatch2RichEdit(TraceListener* tl);

	RKT_API void output(const char* msg, TraceLevel level = levelTrace, const char* file = 0, int line = 0);

	// crash_开头的方法都是特殊的trace，只有很少模块用得到，只为了记录服务器非法用的，为了这些信息不被普通信息淹没
	RKT_API TraceListener* crash_output2File(const char* filename);
	RKT_API void crash_output(const char* msg, TraceLevel level = levelTrace, const char* file = 0, int line = 0);

	// 设置lua的路径
	RKT_API void setTraceLuaPath(const char* path);

} // namespace

#ifdef LIMIT_FREQUENCY // 控制每个输出点的最快频率是1秒
#define LIMIT_FREQ(e)	static rkt::ulong _last_trace_tick = GetTickCount(); if (!rkt::hasLimitFrequency() || (GetTickCount() - _last_trace_tick) > 1000) { _last_trace_tick = GetTickCount(); e}
#else
#define LIMIT_FREQ(e)	e
#endif


#define Trace(x)			{ if(rkt::getTraceLevel() & rkt::levelTrace)	{ LIMIT_FREQ(rkt::ostrbuf o_s_b;o_s_b<<x;rkt::output(o_s_b.c_str(), rkt::levelTrace, __FILE__, __LINE__);)} }
#define Warning(x)			{ if(rkt::getTraceLevel() & rkt::levelWarning)	{ LIMIT_FREQ(rkt::ostrbuf o_s_b;o_s_b<<"WARNING: "<<x;rkt::output(o_s_b.c_str(), rkt::levelWarning, __FILE__, __LINE__);)} }
#define Error(x)			{ if(rkt::getTraceLevel() & rkt::levelError)	{ LIMIT_FREQ(rkt::ostrbuf o_s_b;o_s_b<<"ERROR: "<<x;rkt::output(o_s_b.c_str(), rkt::levelError, __FILE__, __LINE__);/*debugBreakEx(o_s_b.c_str());*/)} }
#define Emphasis(x)			{ if(rkt::getTraceLevel() & rkt::levelEmphasis)	{ LIMIT_FREQ(rkt::ostrbuf o_s_b;o_s_b<<x;rkt::output(o_s_b.c_str(), rkt::levelEmphasis, __FILE__, __LINE__);)} }

#define TraceLn(x)			{ if(rkt::getTraceLevel() & rkt::levelTrace)	{ LIMIT_FREQ(rkt::ostrbuf o_s_b;o_s_b<<x<<rkt::endl;rkt::output(o_s_b.c_str(), rkt::levelTrace, __FILE__, __LINE__);)} }
#define WarningLn(x)		{ if(rkt::getTraceLevel() & rkt::levelWarning)	{ LIMIT_FREQ(rkt::ostrbuf o_s_b;o_s_b<<"WARNING: "<<x<<rkt::endl;rkt::output(o_s_b.c_str(), rkt::levelWarning, __FILE__, __LINE__);)} }
#define ErrorLn(x)			{ if(rkt::getTraceLevel() & rkt::levelError)	{ LIMIT_FREQ(rkt::ostrbuf o_s_b;o_s_b<<"ERROR: "<<x<<rkt::endl;rkt::output(o_s_b.c_str(), rkt::levelError, __FILE__, __LINE__);/*debugBreakEx(o_s_b.c_str());*/)} }
#define EmphasisLn(x)		{ if(rkt::getTraceLevel() & rkt::levelEmphasis)	{ LIMIT_FREQ(rkt::ostrbuf o_s_b;o_s_b<<x<<rkt::endl;rkt::output(o_s_b.c_str(), rkt::levelEmphasis, __FILE__, __LINE__);)} }

#define TraceOnce(x)		{static bool f=true;if(f){f=!f;Trace(x);}}
#define WarningOnce(x)		{static bool f=true;if(f){f=!f;Warning(x);}}
#define ErrorOnce(x)		{static bool f=true;if(f){f=!f;Error(x);}}
#define EmphasisOnce(x)		{static bool f=true;if(f){f=!f;Emphasis(x);}}

#define TraceOnceLn(x)		{static bool f=true;if(f){f=!f;TraceLn(x);}}
#define WarningOnceLn(x)	{static bool f=true;if(f){f=!f;WarningLn(x);}}
#define ErrorOnceLn(x)		{static bool f=true;if(f){f=!f;ErrorLn(x);}}
#define EmphasisOnceLn(x)	{static bool f=true;if(f){f=!f;EmphasisLn(x);}}

#define crash_ErrorLn(x)	{ if(rkt::getTraceLevel() & rkt::levelError){ rkt::ostrbuf o_s_b;o_s_b<<"ERROR: "<<x<<rkt::endl;rkt::crash_output(o_s_b.c_str(), rkt::levelError, __FILE__, __LINE__);} }

#ifdef OPEN_PROFILE
#	define Info(x)			{ if(rkt::getTraceLevel() & rkt::levelInfo)	{ LIMIT_FREQ(rkt::ostrbuf o_s_b;o_s_b<<x;rkt::output(o_s_b.c_str(), rkt::levelInfo, __FILE__, __LINE__);)} }
#	define InfoLn(x)		{ if(rkt::getTraceLevel() & rkt::levelInfo)	{ LIMIT_FREQ(rkt::ostrbuf o_s_b;o_s_b<<x<<rkt::endl;rkt::output(o_s_b.c_str(), rkt::levelInfo, __FILE__, __LINE__);)} }
#	define InfoOnce(x)		{static bool f=true;if(f){f=!f;Info(x);}}
#	define InfoOnceLn(x)	{static bool f=true;if(f){f=!f;InfoLn(x);}}
#	define Assert(x)		{if(!(x)){LIMIT_FREQ(rkt::ostrbuf o_s_b;o_s_b<<"ASSERT: "<<#x<<rkt::endl;rkt::output(o_s_b.c_str(), rkt::levelError, __FILE__, __LINE__);) debugBreakEx(#x);}}
#	define Verify(x)		{if(!(x)){LIMIT_FREQ(rkt::ostrbuf o_s_b;o_s_b<<"VERIFY: "<<#x<<rkt::endl;rkt::output(o_s_b.c_str(), rkt::levelError, __FILE__, __LINE__);)}}
#else
#	define Info(x)
#	define InfoLn(x)
#	define InfoOnce(x)
#	define InfoOnceLn(x)
#	define Assert(x)
#	define Verify(x)		(x)
#endif


#ifdef ASSERT
#undef ASSERT
#endif

#ifdef VERIFY
#undef VERIFY
#endif

#define ASSERT Assert
#define VERIFY Verify

#else // #ifdef SUPPORT_TRACE

#define Trace(x)
#define Warning(x)
#define Error(x)
#define Emphasis(x)

#define TraceLn(x)
#define WarningLn(x)
#define ErrorLn(x)
#define EmphasisLn(x)

#define TraceOnce(x)
#define WarningOnce(x)
#define ErrorOnce(x)
#define EmphasisOnce(x)

#define TraceOnceLn(x)
#define WarningOnceLn(x)
#define ErrorOnceLn(x)
#define EmphasisOnceLn(x)

#define Info(x)
#define InfoLn(x)
#define InfoOnce(x)
#define InfoOnceLn(x)
#define Assert(x)
#define Verify(x)		(x)

#endif // #ifndef SUPPORT_TRACE

#define InitDebugInfo  Info
#define InitDebugInfoLn  InfoLn

#endif // RKT_TRACE_H