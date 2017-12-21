//==========================================================================
/**
* @file	  : StringCommon.h
* @author : PeakGao <peakgao163@163.com>
* created : 2010-1-6   12:20
* purpose : 
*/
//==========================================================================

#ifndef __StringCommon_h__
#define __StringCommon_h__

#include "Common.h"

namespace rkt {

	extern RKT_EXPORT mstring EmptyMString;
	extern RKT_EXPORT wstring EmptyWString;

#ifdef RKT_UNICODE
	#define EmptyTString	EmptyWString
#else
	#define EmptyTString	EmptyMString
#endif

	/**
		xsprintf和xswprintf不同于sprintf，仅仅支持字符串类型的参数
		
		@param buff		目标缓冲区
		@param buffSize	目标缓冲区的大小(是字符数，不是字节数)
		@param count	最大限制的输出字符个数
		@param format	格式化串

		@retval	成功返回字符串长度，否则返回－1（包括参数错误，字符串被截断，以及缓冲区太小的情况，其中字符串被截断需要count为_TRUNCATE）

		e.g.
			char buff[256];
			wchar_t wbuff[256];

			const char* s1, *s2, *s3;
			const wchar_t* ws1, *ws2, *ws3;

			%n - 表示第几个参数， 1 <= n <= 9 ,最多支持9个参数，从1开始，0不支持

			_xsprintf(buff, _countof(buff), "a:%1 b:%2 c:%3", s1, s2, s3);
			_xsnprintf(buff, _countof(buff), _TRUNCATE, "a:%1 b:%2 c:%3", s1, s2, s3);
			_xswprintf(wbuff, _countof(wbuff), L"a:%1 b:%2 c:%3", ws1, ws2, ws3);
	*/

	RKT_API int _xvscprintf(const char* format, va_list arglist);
	RKT_API int _xsprintf(char* buff, size_t buffSize, const char* format, ...);
	RKT_API int _xsnprintf(char* buff, size_t buffSize, size_t count, const char* format, ...);
	RKT_API int _xvsnprintf(char* buff, size_t buffSize, size_t count, const char* format, va_list arglist);

	RKT_API int _xvscwprintf(const wchar_t* format, va_list arglist);
	RKT_API int _xswprintf(wchar_t* buff, size_t buffSize, const wchar_t* format, ...);
	RKT_API int _xsnwprintf(wchar_t* buff, size_t buffSize, size_t count, const wchar_t* format, ...);
	RKT_API int _xvsnwprintf(wchar_t* buff, size_t buffSize, size_t count, const wchar_t* format, va_list arglist);

} // rkt

#endif // __StringCommon_h__