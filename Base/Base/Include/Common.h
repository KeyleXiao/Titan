//==========================================================================
/**
* @file	  : Common.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-1-2   16:08
* purpose : 公共头文件
*/
//==========================================================================


#ifndef RKT_COMMON_H
#define RKT_COMMON_H

#include "Config.h" // 包含用户的配置

// 包含常用的stl库
#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <set>
#include <hash_map>

#include <assert.h>
#include <tchar.h>

//namespace rkt {
/*
MSVC++ 9.0  _MSC_VER = 1500 VS2008
MSVC++ 8.0  _MSC_VER = 1400 VS2005
MSVC++ 7.1  _MSC_VER = 1310 VS2003
MSVC++ 7.0  _MSC_VER = 1300 
MSVC++ 6.0  _MSC_VER = 1200 
MSVC++ 5.0  _MSC_VER = 1100
*/
// 编译器定义
#if defined( _MSC_VER )
#	define RKT_COMPILER_MSVC
#	define RKT_COMPILER_VER		_MSC_VER
#elif defined( __GNUC__ )
#   define RKT_COMPILER_GNUC
#   define RKT_COMPILER_VER		(((__GNUC__)*100) + (__GNUC_MINOR__*10) + __GNUC_PATCHLEVEL__)
#else
#   pragma error "Unknown compiler. Abort!"
#endif


// 平台定义
#if defined( __WIN32__ ) || defined( _WIN32 ) || defined( WIN32 ) || defined(_WIN64) || defined(WINDOWS) || defined(_WIN32_CE)
#	define RKT_WIN32
#else
#	define RKT_LINUX
#endif

// 系统调试版本,和编译器中DEBUG宏一致
# ifdef RKT_COMPILER_MSVC
// 临时将64位调试打开
#    if defined(_DEBUG) 
#       define VERSEION_DEBUG   1
#    endif
#endif


	/// 模块的配置后缀
#if (defined(_DEBUG) && !defined(RKT_WIN32))
#	define CFG_SUFFIX		"_d"
#else
#	define CFG_SUFFIX		""
#endif

	/// 动态库前后缀
#ifdef RKT_WIN32
#	define DLL_PREFIX	""
#	define DLL_SUFFIX	".dll"
#else
#	define DLL_PREFIX	"lib"
#	define DLL_SUFFIX	".so"
#endif


	/// 组合库文件名
#define MAKE_DLL_NAME(module)			"./"DLL_PREFIX#module""CFG_SUFFIX""DLL_SUFFIX
#define	MAKE_LIB_NAME(module)			"lib"#module""CFG_SUFFIX
#define MAKE_DLL_LIB_NAME(module)		#module""CFG_SUFFIX
// 按CPU位数加载LIB
#ifdef _WIN64
#	define MAKE_DLL_LIB_NAME_BY_ARCH(module)	MAKE_DLL_LIB_NAME(module##64)
#else
#	define MAKE_DLL_LIB_NAME_BY_ARCH(module)	MAKE_DLL_LIB_NAME(module)
#endif



/// UNICODE相关
#if defined(_UNICODE) || defined(UNICODE)
#define RKT_UNICODE

/// 这个宏主要是考虑在国际化修改过程中，兼容多字节方式下的资源文件(UNICODE模式下，文本文件启用MBCS编码格式，关闭该宏的话，文本文件应该保存为utf8编码格式)
#define UNICODE_MODE_TEXTFILE_USE_MBCS

#endif

#ifdef SUPPORT_INTL
#define RKT_UTF8
#endif

// 文件BOM
#ifdef RKT_UTF8
#define FILE_BOM	"{ 0xEF, 0xBB, 0xBF}"
#else
#define FILE_BOM	""
#endif


// 导入导出标志
#ifdef RKT_WIN32
#	ifdef _LIB
#		define CLASS_EXPORT
#		define CLASS_IMPORT
#		define API_EXPORT		extern "C"
#		define API_IMPORT		extern "C"
#	else
#		define CLASS_EXPORT		__declspec(dllexport)
#		define CLASS_IMPORT		__declspec(dllimport)
#		define API_EXPORT		extern "C" __declspec(dllexport)
#		define API_IMPORT		extern "C" __declspec(dllimport)
#	endif
#else
#	define CLASS_EXPORT
#	define CLASS_IMPORT
#	define API_EXPORT
#	define API_IMPORT
#endif

#ifdef RKT_WIN32
#	if defined(BASE_STATIC_LIB)
#		define RKT_EXPORT
#		define RKT_API		extern "C"
#		pragma comment(lib, MAKE_LIB_NAME(Base))
#	elif defined(BASE_STATIC_LIB_MD)
#		define RKT_EXPORT
#		define RKT_API		extern "C"
#		pragma comment(lib, "libBase_md.lib")
#	elif defined(BASE_EXPORTS)
#		define RKT_EXPORT	__declspec(dllexport)
#		define RKT_API		extern "C" __declspec(dllexport)
#	else
#		define RKT_EXPORT	__declspec(dllimport)
#		define RKT_API		extern "C" __declspec(dllimport)
#ifndef _WIN64
#		pragma comment(lib, MAKE_DLL_LIB_NAME(Base))
#endif
#	endif
#elif defined(RKT_LINUX)
#	define RKT_EXPORT
#	define RKT_API
#endif	// RKT_LINUX

#ifdef RKT_WIN32
#elif defined(RKT_LINUX)
#	ifndef __cdecl
#		define __cdecl __attribute__((__cdecl__))
#	endif
#endif
#define RKT_CDECL				__cdecl

#ifndef RKT_STDCALL
#	ifdef RKT_WIN32
#		define RKT_STDCALL		__stdcall
#	elif defined(RKT_LINUX)
#		define RKT_STDCALL		__attribute__((__stdcall__))
#	endif
#endif

/*
// 微软的安全警告
#if defined(RKT_WIN32) && defined(RKT_COMPILER_MSVC) && RKT_COMPILER_VER > 1300
#	ifndef _CRT_NONSTDC_NO_WARNINGS
#		define _CRT_NONSTDC_NO_WARNINGS
#	endif
#	ifndef _CRT_SECURE_NO_WARNINGS
#		define _CRT_SECURE_NO_WARNINGS
#	endif
#	ifndef _CRT_SECURE_NO_DEPRECATE
#		define _CRT_SECURE_NO_DEPRECATE
#	endif
#	ifndef _CRT_NONSTDC_NO_DEPRECATE
#		define _CRT_NONSTDC_NO_DEPRECATE
#	endif
#endif
*/

#define _FILE_LINE_DESC				const char* file, int line,
#define _FILE_LINE_VARIANT			file, line,
#define _FILE_LINE_MACRO				__FILE__,__LINE__,
#define _FILE_LINE_FORMAT			"%s[%d]:"

#ifdef SUPPORT_FILE_LINE_DEBUG
#define FILE_LINE_DESC		_FILE_LINE_DESC
#define FILE_LINE_VARIANT	_FILE_LINE_VARIANT
#define FILE_LINE_MACRO		_FILE_LINE_MACRO
#define FILE_LINE_FORMAT	_FILE_LINE_FORMAT
#else
#define FILE_LINE_DESC
#define FILE_LINE_VARIANT
#define FILE_LINE_MACRO
#define FILE_LINE_FORMAT
#endif

#ifndef RKT_CALLBACK
#	define RKT_CALLBACK		RKT_STDCALL
#endif


// 只定义无符号的简单类型，便于书写
namespace rkt {

typedef char					int8,int08;
typedef unsigned char			uchar,uint8,uint08,utf8/*,byte*/;

typedef short					int16;
typedef unsigned short			ushort,uint16;

typedef int						int32;
typedef unsigned int			uint,uint32;

typedef unsigned long			ulong;

#ifdef RKT_WIN32
typedef __int64					int64;
typedef unsigned __int64		uint64;
#elif RKT_LINUX
typedef long long int			int64;
typedef unsigned long long int	uint64;
#endif

typedef float					f32;
typedef double					f64;



// 字符和字符串的定义
typedef char						mchar;
typedef wchar_t						wchar;

typedef std::string					mstring;
typedef std::wstring				wstring;

// 程序中，需要转换文本(翻译)的地方均使用 _GT，否则可直接使用 _T
#ifdef SUPPORT_INTL
#define _GT(x)						((gettext_ucs2(L##x)))
#else
#define _GT(x)						(x)
#endif
#define _NGT//(x)						(x)	// 用于防止mo.exe提取字符串


#ifdef RKT_UNICODE

typedef wchar						tchar;
typedef wstring						tstring;

#else // MBCS

typedef mchar						tchar;
typedef mstring						tstring;

#endif

}

// 安全删除和释放
#ifndef safeDelete
#	define safeDelete(ptr)		if ((ptr)) {delete (ptr); (ptr) = 0;}
#endif
#ifndef safeDeleteArray
#	define safeDeleteArray(ptr)	if ((ptr)) {delete[] (ptr); (ptr) = 0;}
#endif
#ifndef safeRelease
#	define safeRelease(ptr)		if ((ptr)) {(ptr)->release(); (ptr) = 0;}
#endif
#ifndef SAFERELEASE
#	define SAFERELEASE(ptr)		if ((ptr)) {(ptr)->Release(); (ptr) = 0;}
#endif
/** 编译期Assert,参考boost库,如：
STATIC_ASSERT(sizeof(long) > 8);
*/
namespace COMPILE_ASSERT
{
	template <bool x> struct COMPILE_ASSERT_FAILURE;
	template <> struct COMPILE_ASSERT_FAILURE<true> { enum { value = 1 }; };
	template<int x> struct compile_assert_test{};
}
#define STATIC_ASSERT(exp) \
	typedef COMPILE_ASSERT::compile_assert_test< \
	sizeof(COMPILE_ASSERT::COMPILE_ASSERT_FAILURE< (bool)(exp) >) \
	> compile_assert_typedef_

//} // namespace


// hash_map的定义，为了不关心命名空间的差异，可直接用 HashMap 宏
#if defined(RKT_COMPILER_GNUC) && RKT_COMPILER_VER >= 310
#   define HashMap __gnu_cxx::hash_map
#else
#   if defined(RKT_COMPILER_MSVC)
#       if RKT_COMPILER_VER > 1300
#           define HashMap stdext::hash_map
#       else
#           define HashMap std::hash_map
#       endif
#   else
#       define HashMap std::hash_map
#   endif
#endif

#define PROFILE(x)

#ifdef RKT_TEST
#	define SEGMENT(x)	x
#else
#	define SEGMENT(x)
#endif


// 下面这个宏用于方便设置断点
#define	breakable		switch(1) case 1: default:

#define isValidString(s)	((s) && (*(s) != 0))

//#ifdef _INC_TIME_INL
//#error 请在包含base.h前，不要包含 time.h
//#endif

/// 调试器中断
namespace rkt {

RKT_API char* sstrcpyn(char* dest, const char* src, size_t destSize);

RKT_API int ssprintf_s(char* string, size_t sizeInBytes, const char* format, ...);

template <size_t _Size> inline int ssprintf(char (&_Dest)[_Size], const char * _Format, ...)
{
	va_list _ArgList; _crt_va_start(_ArgList, _Format);
	return rkt_vsprintf_s(FILE_LINE_VARIANT _Dest, _Size, _Format, _ArgList);
}

RKT_API void debuggerBreak(const char* exp, const char* file, unsigned int line, bool* ignore = 0);

// 获取UTF8字符串字节数
RKT_API int getUTF8StringSize(const char* src);
// 获取UTF8字符串个数
RKT_API int getUTF8StringNum(const char* src);
//RKT_API void freeAtExit(void* p);

#define _time64 getTime

}

#ifdef RKT_DEBUG
#	define debugBreak			{static bool __s_ignore__ = false; rkt::debuggerBreak(0, __FILE__, __LINE__, &__s_ignore__);}
#	define debugBreakEx(str)	{static bool __s_ignore__ = false; rkt::debuggerBreak(str, __FILE__, __LINE__, &__s_ignore__);}
#else
#	define debugBreak			(void)0
#	define debugBreakEx(str)	(void)0
#endif


#endif // RKT_COMMON_H
