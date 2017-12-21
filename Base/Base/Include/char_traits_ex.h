//==========================================================================
/**
* @file	  : char_traits_ex.h
* @author : PeakGao <peakgao163@163.com>
* created : 2010-5-18   14:31
* purpose : 字符特性模板
*/
//==========================================================================

#ifndef __char_traits_ex_h__
#define __char_traits_ex_h__

#include "Common.h"

#include <crtdefs.h> // for __CLRCALL_OR_CDECL, _TRUNCATE
#include <iosfwd> // for char_traits
#include <mbstring.h> // for _mbctolower...

#include "StringCommon.h" // for xvsnprintf...

namespace rkt {

	template<class _Elem>
	struct char_traits_ex_t : public std::char_traits<_Elem>
	{
		static const _Elem* __cdecl get_def_space() {
			return " \t";
		}
		static const _Elem* __cdecl get_def_sep() {
			return ",";
		}
		static _Elem __cdecl to_lower(const _Elem& _Ch) {
			return ::_mbctolower(_Ch);
		}
		static _Elem __cdecl to_upper(const _Elem& _Ch) {
			return ::_mbctoupper(_Ch);
		}
		static void __cdecl to_lower(_Elem* _First, size_t _Size) {
			::_strlwr_s(_First, _Size);
		}
		static void __cdecl to_upper(_Elem* _First, size_t _Size) {
			::_strupr_s(_First, _Size);
		}
		static int __cdecl compare_nocase(const _Elem* _First1, const _Elem* _First2) {
			return ::_mbsicmp((const unsigned char*)_First1, (const unsigned char*)_First2);
		}
		static int __cdecl compare_nocase_n(const _Elem* _First1, const _Elem* _First2, size_t _Count) {
			return ::_mbsnicmp((const unsigned char*)_First1, (const unsigned char*)_First2, _Count);
		}
		static void __cdecl reverse(_Elem* _First) {
			::_mbsrev((unsigned char*)_First);
		}
		static int __cdecl to_int(const _Elem* _First) {
			return (int)::strtoul(_First, 0, 10);
		}
		static long __cdecl to_long(const _Elem* _First) {
			return (long)::strtoul(_First, 0, 10);
		}
		static __int64 __cdecl to_int64(const _Elem* _First) {
			return (__int64)::_strtoui64(_First, 0, 10);
		}
		static float __cdecl to_float(const _Elem* _First) {
			return (float)::strtod(_First, 0);
		}
		static double __cdecl to_double(const _Elem* _First) {
			return ::strtod(_First, 0);
		}
		static void __cdecl from_int(int _Val, _Elem* _First, size_t _Size, int _Radix) {
			::_ltoa_s((long)_Val, _First, _Size, _Radix);
		}
		static void __cdecl from_uint(unsigned int _Val, _Elem* _First, size_t _Size, int _Radix) {
			::_ultoa_s((unsigned long)_Val, _First, _Size, _Radix);
		}
		static void __cdecl from_long(long _Val, _Elem* _First, size_t _Size, int _Radix) {
			::_ltoa_s(_Val, _First, _Size, _Radix);
		}
		static void __cdecl from_ulong(unsigned long _Val, _Elem* _First, size_t _Size, int _Radix) {
			::_ultoa_s(_Val, _First, _Size, _Radix);
		}
		static void __cdecl from_int64(__int64 _Val, _Elem* _First, size_t _Size, int _Radix) {
			::_i64toa_s(_Val, _First, _Size, _Radix);
		}
		static void __cdecl from_uint64(unsigned __int64 _Val, _Elem* _First, size_t _Size, int _Radix) {
			::_ui64toa_s(_Val, _First, _Size, _Radix);
		}
		static void __cdecl from_float(float _Val, _Elem* _First, size_t _Size) {
			//::gcvt(_Val, _First); // ecvt, fcvt...
			::_snprintf_s(_First, _Size, _TRUNCATE, "%f", _Val);
		}
		static void __cdecl from_double(double _Val, _Elem* _First, size_t _Size) {
			::_snprintf_s(_First, _Size, _TRUNCATE, "%lf", _Val);
		}
		static int __cdecl vscprintf(const _Elem* _Format, va_list _List) {
			// doesn't count terminating '\0'
			return ::_vscprintf(_Format, _List);
		}
		static int __cdecl vsnprintf(_Elem* _First, size_t _Size, size_t _Count, const _Elem* _Format, va_list _List) {
			return ::_vsnprintf_s(_First, _Size, _Count, _Format, _List);
		}
		static int __cdecl xvscprintf(const _Elem* _Format, va_list _List) {
			// doesn't count terminating '\0'
			return _xvscprintf(_Format, _List);
		}
		static int __cdecl xvsnprintf(_Elem* _First, size_t _Size, size_t _Count, const _Elem* _Format, va_list _List) {
			return _xvsnprintf(_First, _Size, _Count, _Format, _List);
		}
	};



	template<>
	struct char_traits_ex_t<wchar_t> : public std::char_traits<wchar_t>
	{
		typedef wchar_t	_Elem;
		static const _Elem* __cdecl get_def_space() {
			return L" \t";
		}
		static const _Elem* __cdecl get_def_sep() {
			return L",";
		}
		static _Elem __cdecl to_lower(const _Elem& _Ch) {
			return towlower(_Ch);
		}
		static _Elem __cdecl to_upper(const _Elem& _Ch) {
			return towupper(_Ch);
		}
		static void __cdecl to_lower(_Elem* _First, size_t _Size) {
			::_wcslwr_s(_First, _Size);
		}
		static void __cdecl to_upper(_Elem* _First, size_t _Size) {
			::_wcsupr_s(_First, _Size);
		}
		static int __cdecl compare_nocase(const _Elem* _First1, const _Elem* _First2) {
			return ::_wcsicmp(_First1, _First2);
		}
		static int __cdecl compare_nocase_n(const _Elem* _First1, const _Elem* _First2, size_t _Count) {
			return ::_wcsnicmp(_First1, _First2, _Count);
		}
		static void __cdecl reverse(_Elem* _First) {
			::_wcsrev(_First);
		}
		static int __cdecl to_int(const _Elem* _First) {
			return (int)::_wtol(_First);
		}
		static long __cdecl to_long(const _Elem* _First) {
			return ::_wtol(_First);
		}
		static __int64 __cdecl to_int64(const _Elem* _First) {
			return ::_wtoi64(_First);
		}
		static float __cdecl to_float(const _Elem* _First) {
			return (float)::wcstod(_First, 0);
		}
		static double __cdecl to_double(const _Elem* _First) {
			return ::wcstod(_First, 0);
		}
		static void __cdecl from_int(int _Val, _Elem* _First, size_t _Size, int _Radix) {
			::_ltow_s((long)_Val, _First, _Size, _Radix);
		}
		static void __cdecl from_uint(unsigned int _Val, _Elem* _First, size_t _Size, int _Radix) {
			::_ultow_s((unsigned long)_Val, _First, _Size, _Radix);
		}
		static void __cdecl from_long(long _Val, _Elem* _First, size_t _Size, int _Radix) {
			::_ltow_s(_Val, _First, _Size, _Radix);
		}
		static void __cdecl from_ulong(unsigned long _Val, _Elem* _First, size_t _Size, int _Radix) {
			::_ultow_s(_Val, _First, _Size, _Radix);
		}
		static void __cdecl from_int64(__int64 _Val, _Elem* _First, size_t _Size, int _Radix) {
			::_i64tow_s(_Val, _First, _Size, _Radix);
		}
		static void __cdecl from_uint64(unsigned __int64 _Val, _Elem* _First, size_t _Size, int _Radix) {
			::_ui64tow_s(_Val, _First, _Size, _Radix);
		}
		static void __cdecl from_float(float _Val, _Elem* _First, size_t _Size) {
			::_snwprintf_s(_First, _Size, _TRUNCATE, L"%f", _Val);
		}
		static void __cdecl from_double(double _Val, _Elem* _First, size_t _Size) {
			::_snwprintf_s(_First, _Size, _TRUNCATE, L"%lf", _Val);
		}
		static int __cdecl vscprintf(const _Elem* _Format, va_list _List) {
			// doesn't count terminating '\0'
			return ::_vscwprintf(_Format, _List);
		}
		static int __cdecl vsnprintf(_Elem* _First, size_t _Size, size_t _Count, const _Elem* _Format, va_list _List) {
			return ::_vsnwprintf_s(_First, _Size, _Count, _Format, _List);
		}
		static int __cdecl xvscprintf(const _Elem* _Format, va_list _List) {
			// doesn't count terminating '\0'
			return _xvscwprintf(_Format, _List);
		}
		static int __cdecl xvsnprintf(_Elem* _First, size_t _Size, size_t _Count, const _Elem* _Format, va_list _List) {
			return _xvsnwprintf(_First, _Size, _Count, _Format, _List);
		}

	};

	typedef char_traits_ex_t<char>			mchar_traits_ex;
	typedef char_traits_ex_t<wchar_t>		wchar_traits_ex;

	//template struct RKT_EXPORT char_traits_ex_t<char>;
	//template struct RKT_EXPORT char_traits_ex_t<wchar_t>;

#ifdef RKT_UNICODE
	typedef wchar_traits_ex		char_traits_ex;
#else
	typedef mchar_traits_ex		char_traits_ex;
#endif

} // rkt


#endif // __char_traits_ex_h__