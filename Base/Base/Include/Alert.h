/*******************************************************************
** 文件名:      Alert.h 
** 版  权:      (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:      宋文武 (sww8@163.com)
** 日  期:      2010-6-3
** 版  本:      1.0
** 描  述:      
** 应  用:      实时报警

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef RKT_ALERT_H
#define RKT_ALERT_H

#include "Common.h"

#ifdef SUPPORT_ALERT

#include "ostrbuf.h"


//#define ALERT_LIMIT_FREQUENCY

#ifdef ALERT_LIMIT_FREQUENCY
#include <windows.h>
#endif

// 服务器管理器性能报警时长
#define MANAGER_PERFORMANCEALERT_LAPSE 30000

namespace rkt {

	//  报警消息级别 
	enum EMAlertLevel
	{
		AlertLevel_None		= 0x0,			// 0	:无
		AlertLevel_Info		= 0x1,			// 1	:信息
		AlertLevel_Prompt	= 0x2,			// 2	:提示
		AlertLevel_Emphasis	= 0x4,			// 4	:强调
		AlertLevel_Warning	= 0x8,			// 8	:警告
		AlertLevel_Error	= 0x10,			// 16	:错误
		AlertLevel_Bad		= 0x20,			// 32	:严重
		AlertLevel_All		= 0xFF,			// 255	:全部

		AlertLevel_MAXID	= 7,			// 最大个数
	};

	// 报警源类型，有新类型加到后面，不要加入中间，否则以前保存的数据显示不对
	// 以下类型为内定类型
	enum EMAlertSrcType
	{
		MG_AlertSrcType_None = 0,				// 未知
		MG_AlertSrcType_DataBase,				// 数据库
		MG_AlertSrcType_Performance,			// 性能
		MG_AlertSrcType_Memory,				// 内存
		MG_AlertSrcType_Network,				// 网络
		MG_AlertSrcType_Lua,					// Lua
		MG_AlertSrcType_Script,				// 脚本
		MG_AlertSrcType_Crash,					// 崩溃
		// 最大
		MG_AlertSrcType_MaxID
	};

	/// 报警消息接收器
	class RKT_EXPORT AlertListener
	{
	protected:
		int	m_nAlertlevel;	// 报警消息级别 
	public:
		AlertListener() : m_nAlertlevel(AlertLevel_All) {}
		virtual ~AlertListener() {}

		void setAlertLevel(int nlevel)		{ m_nAlertlevel = nlevel; }
		int getAlertLevel() const		{ return m_nAlertlevel; }
		virtual void onAlert(int nlevel,int nSrcType,const char* szMsg,  const char* file, int line) {}
	};

	RKT_API void createAlert(int nlevel = AlertLevel_All);
	RKT_API int setAlertLevel(int nlevel);
	RKT_API int getAlertLevel();
	RKT_API bool setAlertHasLimitFrequency(bool bLimitFrequency);
	RKT_API bool AlertHasLimitFrequency();
	RKT_API void registerAlert(AlertListener* pSink);
	RKT_API void unregisterAlert(AlertListener* pSink);
	RKT_API void closeAlert();

	RKT_API void outputAlert(int nlevel,int nSrcType,const char* szMsg,  const char* file = 0, int line = 0);

} // namespace


#ifdef ALERT_LIMIT_FREQUENCY // 控制每个输出点的最快频率是1秒
#define ALERT_LIMIT_FREQ(e)	static rkt::ulong _last_alert_tick = GetTickCount(); if (!rkt::AlertHasLimitFrequency() || (GetTickCount() - _last_alert_tick) > 3000) { _last_alert_tick = GetTickCount(); e}
#else
#define ALERT_LIMIT_FREQ(e)	e
#endif

// 报警常用宏定义
// 信息
#define AlertInfo(nType,x)		{ if(rkt::getAlertLevel() & rkt::AlertLevel_Info)	{ ALERT_LIMIT_FREQ(rkt::ostrbuf o_s_b; o_s_b<<x;rkt::outputAlert(AlertLevel_Info,nType,o_s_b.c_str(),__FILE__, __LINE__);)} }
// 提示
#define AlertPrompt(nType,x)	{ if(rkt::getAlertLevel() & rkt::AlertLevel_Prompt)	{ ALERT_LIMIT_FREQ(rkt::ostrbuf o_s_b; o_s_b<<x;rkt::outputAlert(AlertLevel_Prompt,nType,o_s_b.c_str(),__FILE__, __LINE__);)} }
// 强调
#define AlertEmphasis(nType,x)	{ if(rkt::getAlertLevel() & rkt::AlertLevel_Emphasis)	{ ALERT_LIMIT_FREQ(rkt::ostrbuf o_s_b; o_s_b<<x;rkt::outputAlert(AlertLevel_Emphasis,nType,o_s_b.c_str(),__FILE__, __LINE__);)} }
// 警告
#define AlertWarning(nType,x)	{ if(rkt::getAlertLevel() & rkt::AlertLevel_Warning)	{ ALERT_LIMIT_FREQ(rkt::ostrbuf o_s_b; o_s_b<<x;rkt::outputAlert(AlertLevel_Warning,nType,o_s_b.c_str(),__FILE__, __LINE__);)} }
// 错误
#define AlertError(nType,x)		{ if(rkt::getAlertLevel() & rkt::AlertLevel_Error)	{ ALERT_LIMIT_FREQ(rkt::ostrbuf o_s_b; o_s_b<<x;rkt::outputAlert(AlertLevel_Error,nType,o_s_b.c_str(),__FILE__, __LINE__);)} }
// 严重
#define AlertBad(nType,x)		{ if(rkt::getAlertLevel() & rkt::AlertLevel_Bad)	{ ALERT_LIMIT_FREQ(rkt::ostrbuf o_s_b; o_s_b<<x;rkt::outputAlert(AlertLevel_Bad,nType,o_s_b.c_str(),__FILE__, __LINE__);)} }

// 只报警一次的宏定义
#define AlertOnceInfo(nType,x)			{static bool f=true;if(f){f=!f;AlertInfo(nType,x);}}
#define AlertOncePrompt(nType,x)		{static bool f=true;if(f){f=!f;AlertPrompt(nType,x);}}
#define AlertOnceEmphasis(nType,x)		{static bool f=true;if(f){f=!f;AlertEmphasis(nType,x);}}
#define AlertOnceWarning(nType,x)		{static bool f=true;if(f){f=!f;AlertWarning(nType,x);}}
#define AlertOnceError(nType,x)			{static bool f=true;if(f){f=!f;AlertError(nType,x);}}
#define AlertOnceBad(nType,x)			{static bool f=true;if(f){f=!f;AlertBad(nType,x);}}


#else // #ifdef SUPPORT_ALERT

#define AlertInfo(nType,x)
#define AlertPrompt(nType,x)
#define AlertEmphasis(nType,x)
#define AlertWarning(nType,x)
#define AlertError(nType,x)	
#define AlertBad(nType,x)

#define AlertOnceInfo(nType,x)
#define AlertOncePrompt(nType,x)
#define AlertOnceEmphasis(nType,x)
#define AlertOnceWarning(nType,x)
#define AlertOnceError(nType,x)
#define AlertOnceBad(nType,x)

#endif // #ifndef SUPPORT_ALERT

#endif // RKT_ALERT_H