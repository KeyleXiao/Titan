/**
* Created by PeakGao 2008.01.04
*/

#ifndef RKT_BASE_H
#define RKT_BASE_H

#pragma warning(disable : 4996) // strcpy等非安全警告

#include <Windows.h>

#include "Common.h"

#include "char_traits_ex.h"
#include "ostrbuf.h"
#include "tostr.h"
#include "buffer.h"
#include "strconv.h"
#include "StringCommon.h"
#include "MyTime.h"

#ifdef SUPPORT_TRACE
#include "Trace.h"
#endif

#ifdef SUPPORT_API
#include "Api.h"
#endif

#ifdef SUPPORT_SINGLETON
#include "Singleton.h"
#endif

#ifdef SUPPORT_AUTOSTRING
#include "autostring.h"
#endif

#ifdef SUPPORT_STRINGHASH
#include "StringHash.h"
#endif

#ifdef SUPPORT_STRINGHELPER
#include "StringHelper.h"
#endif

#ifdef SUPPORT_MEMORY
#include "Mem.h"
#include "MemObj.h"
#endif

#ifdef SUPPORT_NET
#include "StlHelper.h"
#include "Net/IAsynfile.h"
#include "Net/net.h"
#endif

#ifdef SUPPORT_STREAM
#include "FileSystem.h"
#endif

#ifdef SUPPORT_OBJECTPOOL
#include "ICacheManager.h"
#endif

#ifdef SUPPORT_RLE
#include "rle.h"
#endif

#ifdef SUPPORT_SAFELIST
#include "safelist.h"
#endif

#ifdef SUPPORT_LOADLIB
#include "LoadLib.h"
#endif

#ifdef SUPPORT_HANDLE
#include "Handle.h"
#endif

#ifdef SUPPORT_PATHFINDER
#include "PathFinder.h"
#endif

#ifdef SUPPORT_IME
#include "Ime.h"
#endif

#ifdef SUPPORT_RANDGENERATOR
#include "MyRand.h"
#endif

#ifdef SUPPORT_APP
#include "App.h"
#endif

#ifdef SUPPORT_TINYXML
#include "tinyxml.h"
#endif

#ifdef SUPPORT_OBJECT
#include "Object.h"
#endif

#ifdef SUPPORT_THREAD
#include "LockFree.h"
#include "Thread.h"
//#include "TPool.h"
#endif

#ifdef SUPPORT_TIMER
#include "Timer.h"
#include "TimerAxis.h"
#endif

#ifdef SUPPORT_EVENTENGINE
#include "IEventEngine.h"
#endif

#ifdef SUPPORT_SCHEMEENGINE
#include "ISchemeEngine.h"
#include "XlsValue.h"
#endif


//#ifdef SUPPORT_PROFILE
#include "IPerfProf.h"
//#endif

#ifdef SUPPORT_STACKWALK
#include "IStackWalk.h"
#endif

#ifdef SUPPORT_LUAENGINE
#include "ILuaEngine.h"
#endif

#ifdef SUPPORT_BASE64
#include "base64.h"
#endif

#ifdef SUPPORT_URL
#include "urlencode.h"
#endif

#ifdef SUPPORT_ALERT
#include "Alert.h"
#endif

#ifdef SUPPORT_INTL
#include "Intl.h"
#endif

#ifdef SUPPORT_LOCALECONFIG
#include "LocaleConfig.h"
#endif

#ifdef SUPPORT_STAT
#include "Stat.h"
#endif

#ifdef SUPPORT_DES
#include "DES.h"
#endif

#ifdef SUPPORT_CRC
#include "CRC.h"
#endif

#endif // RKT_BASE_H