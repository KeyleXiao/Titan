/**
* Created by PeakGao 2008.01.04
*/

#ifndef RKT_STRINGHELPER_H
#define RKT_STRINGHELPER_H

#include "Common.h"

#include <string>
#include <algorithm>
#include <stdarg.h>
#include <vector>

#include "char_traits_ex.h"

#ifdef SUPPORT_STRINGHELPER

/**测试例子
	std::string s1;
	StringHelper::format(s1, "ab中文c%04d", 789);
	StringHelper::toUpper(s1);
	StringHelper::toLower(s1);

	int n = StringHelper::toInt("1234567890");
	int64 n64 = StringHelper::toInt64("1234567890123");
	float ff = StringHelper::toFloat("3.141592600");

	std::string sss;
	StringHelper::fromInt(sss, n);
	StringHelper::fromInt64(sss, n64);
	StringHelper::fromFloat(sss, ff);

	const char* str = " a ,,	b	, c	,	d ,e,f,g ";
	std::vector<std::string> v;
	StringHelper::split(v, str, ',', " \t");
*/

namespace rkt {

	/**
		字符串辅助类，主要用于对一些常用的字符串的操作
		该类基本都是静态方法
	*/
	template<class _Elem, class _Traits>
	class StringHelperT
	{
		enum { TmpBufSize = 64 };
	public:
		typedef std::basic_string<_Elem, std::char_traits<_Elem>, std::allocator<_Elem> >		String;

		/// 替换字符
		static void replace(String& str, _Elem _Old, _Elem _New)
		{
			size_t _Pos = String::npos;
			size_t _Len = str.length();
			while (++_Pos < _Len)
			{
				if (str[_Pos] == _Old)
					str[_Pos] = _New;
			}
		}

		/// 替换字符串
		static void replace(String& str, const String& _Old, const String& _New)
		{
			size_t _Oldlen = _Old.length();
			if (_Oldlen == 0)
				return;

			size_t _Newlen = _New.length();
			size_t _Pos = str.find(_Old.c_str(), 0);
			while (_Pos != String::npos)
			{
				str.replace(_Pos, _Oldlen, _New.c_str(), _Newlen);
				_Pos += _Newlen;
				_Pos = str.find(_Old.c_str(), _Pos);
			}
		}

		/// 去除左侧的空格
		static void  trimLeft(String& str,const String& space = _Traits::get_def_space())
		{
			str.erase(0,str.find_first_not_of(space));
		}

		/// 去除右侧的空格
		static void  trimRight(String& str,const String& space = _Traits::get_def_space())
		{
			str.erase(str.find_last_not_of(space) + 1);
		}

		/// 去除头尾的空格
		static void  trim(String& str,const String& space = _Traits::get_def_space())
		{
			trimLeft(str, space);
			trimRight(str, space);
		}

		/// 大小写不敏感的字符串比较
		static int casecmp(const String& str1, const String& str2)
		{
			return _Traits::compare_nocase(str1.c_str(), str2.c_str());
		}

		/// 大小写不敏感的字符串比较(含长度)
		static int casecmpn(const String& str1, const String& str2, size_t n)
		{
			return _Traits::compare_nocase_n(str1.c_str(), str2.c_str(), n);
		}

		/// 格式化一个字符串
		/*
		static String& format(String& str, const _Elem* _Format, ...)
		{
			const size_t _InitSize = 512;
			_Elem _Tempbuf[_InitSize];

			va_list _Args;
			va_start(_Args, _Format);

			int ret = 0;
			ret = _Traits::vsnprintf(_Tempbuf, _InitSize, _TRUNCATE, _Format, _Args);
			if (ret != -1)
			{
				str.assign(_Tempbuf, ret);
			}
			else
			{
				const size_t _IncSize = 256;
				size_t _Cap = _InitSize + _IncSize;
				while (ret < 0)
				{
					str.reserve(_Cap);
					ret = _Traits::vsnprintf((_Elem*)str.c_str(), str.capacity(), _TRUNCATE, _Format, _Args);
					_Cap = str.capacity() + _IncSize;
				} 

				str.assign(str.c_str(), ret);
			}

			va_end(_Args);

			return str;
		}*/
		static String& format(String& str, const _Elem* _Format, ...)
		{
			if (_Format)
			{
				va_list _Args;
				va_start(_Args, _Format);

				int len = _Traits::vscprintf(_Format, _Args);
				if (len>0)
				{
					str.reserve(len);
					_Traits::vsnprintf((_Elem*)str.c_str(), str.capacity() + 1, _TRUNCATE, _Format, _Args);
					str.assign(str.c_str(), len);
				}
				va_end(_Args);
			}

			return str;
		}

		static String& add_format(String& str, const _Elem* _Format, ...)
		{
			if (_Format)
			{
				va_list _Args;
				va_start(_Args, _Format);

				size_t sz = str.length();
				int len = _Traits::vscprintf(_Format, _Args);
				str.reserve(sz + len);
				_Traits::vsnprintf((_Elem*)str.c_str() + sz, str.capacity() + 1 - sz, _TRUNCATE, _Format, _Args);
				str.append(str.c_str() + sz, len);
				va_end(_Args);
			}

			return str;
		}

		/// 格式化一个字符串
		/*
		static String& xformat(String& str, const _Elem* _Format, ...)
		{
			const size_t _InitSize = 512;
			_Elem _Tempbuf[_InitSize];

			va_list _Args;
			va_start(_Args, _Format);

			int ret = 0;
			ret = _Traits::xvsnprintf(_Tempbuf, _InitSize, _TRUNCATE, _Format, _Args);
			if (ret != -1)
			{
				str.assign(_Tempbuf, ret);
			}
			else
			{
				const size_t _IncSize = 256;
				size_t _Cap = _InitSize + _IncSize;
				while (ret < 0)
				{
					str.reserve(_Cap);
					ret = _Traits::xvsnprintf((_Elem*)str.c_str(), str.capacity(), _TRUNCATE, _Format, _Args);
					_Cap = str.capacity() + _IncSize;
				} 

				str.assign(str.c_str(), ret);
			}

			va_end(_Args);

			return str;
		}*/
		static String& xformat(String& str, const _Elem* _Format, ...)
		{
			if (_Format)
			{
				va_list _Args;
				va_start(_Args, _Format);

				int len = _Traits::xvscprintf(_Format, _Args);
				str.reserve(len);
				_Traits::xvsnprintf((_Elem*)str.c_str(), str.capacity() + 1, _TRUNCATE, _Format, _Args);
				str.assign(str.c_str(), len);
				va_end(_Args);
			}

			return str;
		}
		static String& add_xformat(String& str, const _Elem* _Format, ...)
		{
			if (_Format)
			{
				va_list _Args;
				va_start(_Args, _Format);

				size_t sz = str.length();
				int len = _Traits::xvscprintf(_Format, _Args);
				str.reserve(sz + len);
				_Traits::xvsnprintf((_Elem*)str.c_str() + sz, str.capacity() + 1 - sz, _TRUNCATE, _Format, _Args);
				str.append(str.c_str() + sz, len);
				va_end(_Args);
			}

			return str;
		}

		/// 转换成整数
		static int toInt(const String& str)
		{
			return _Traits::to_int(str.c_str());
		}

		/// 转换成整数
		static long toLong(const String& str)
		{
			return _Traits::to_long(str.c_str());
		}

		/// 转换成int64
		static int64 toInt64(const String& str)
		{
			return _Traits::to_int64(str.c_str());
		}

		/// 转换成浮点
		static float toFloat(const String& str)
		{
			return _Traits::to_float(str.c_str());
		}

		/// 从整数转换成字符串
		static String& fromInt(String& str, int _Val, int _Radix = 10)
		{
			_Elem _Buf[TmpBufSize];
			_Traits::from_int(_Val, _Buf, TmpBufSize, _Radix);
			str = _Buf;
			return str;
		}

		/// 从整数转换成字符串
		static String& fromLong(String& str, long _Val, int _Radix = 10)
		{
			_Elem _Buf[TmpBufSize];
			_Traits::from_long(_Val, _Buf, TmpBufSize, _Radix);
			str = _Buf;
			return str;
		}

		/// 从无符号整数转换成字符串
		static String& fromUlong(String& str, ulong _Val, int _Radix = 10)
		{
			_Elem _Buf[TmpBufSize];
			_Traits::from_ulong(_Val, _Buf, TmpBufSize, _Radix);
			str = _Buf;
			return str;
		}

		/// 从int64转换成字符串
		static String& fromInt64(String& str, int64 _Val, int _Radix = 10)
		{
			_Elem _Buf[TmpBufSize];
			_Traits::from_int64(_Val, _Buf, TmpBufSize, _Radix);
			str = _Buf;
			return str;
		}

		/// 从int64转换成字符串
		static String& fromUint64(String& str, uint64 _Val, int _Radix = 10)
		{
			_Elem _Buf[TmpBufSize];
			_Traits::from_uint64(_Val, _Buf, TmpBufSize, _Radix);
			str = _Buf;
			return str;
		}

		/// 从float转换成字符串
		static String& fromFloat(String& str, float _Val)
		{
			_Elem _Buf[TmpBufSize];
			_Traits::from_float(_Val, _Buf, TmpBufSize);
			str = _Buf;
			return str;
		}

		/// 转换成小写
		static String& toLower(String& str)
		{
			//std::transform(str.begin(), str.end(), str.begin(), _Traits::to_lower);
			_Traits::to_lower(const_cast<_Elem*>(str.c_str()), str.length() + 1);
			return str;
		}

		/// 转换成大写
		static String& toUpper(String& str)
		{
			//std::transform(str.begin(), str.end(), str.begin(), _Traits::to_upper);
			_Traits::to_upper(const_cast<_Elem*>(str.c_str()), str.length() + 1);
			return str;
		}

		/// 将一个字符串分割(支持自定义分隔符，和裁剪的空白字符集)
		static void split(std::vector<String>& strList, const _Elem* source, const _Elem sep = (_Elem)',', const _Elem* space = 0)
		{
			splitEx(strList, source, _Traits::length(source), sep, space);
		}
		static void splitEx(std::vector<String>& strList, const _Elem* source, size_t len, const _Elem sep = (_Elem)',', const _Elem* space = 0)
		{
			if (!source || *source == 0)
				return;

			String sp;
			if (space)
				sp = space;

			const _Elem* s = source;
			const _Elem* e = source;
			const _Elem* end = source + len;
			while (e < end)
			{
				if (*e == sep) // 发现分隔符
				{
					String str;
					str.assign(s, e - s);
					if (space)
						trim(str, sp);
					strList.push_back(str);
					e++;
					s = e;
				}
				else
					e++;
			}

			// 尾部有
			if (e > s)
			{
				String str;
				str.assign(s, e - s);
				if (space)
					trim(str, sp);
				strList.push_back(str);
			}
		}

		/// 将一个字符串分割(支持自定义分隔符，和裁剪的空白字符集)
		static void split(std::vector<String>& strList, const String& source, const String& sep = _Traits::get_def_sep(), const String& space = _Traits::get_def_space())
		{
			if (source.empty())
				return;

			String::size_type s = 0;
			String::size_type e = source.find(sep, s);
			while (e != String::npos)
			{
				strList.push_back(String());
				String& str = strList.back();
				str = source.substr(s, e - s);
				if (!space.empty())
					trim(str, space);

				s = e + sep.length();
				e = source.find(sep, s);
			}

			// 尾部有
			if (s < source.length())
			{
				strList.push_back(String());
				String& str = strList.back();
				str = source.substr(s);
				if (!space.empty())
					trim(str, space);
			}
		}
	};

	typedef StringHelperT<char,		char_traits_ex_t<char>	>		MStringHelper,StringHelper;
	typedef StringHelperT<wchar_t,	char_traits_ex_t<wchar_t> >		WStringHelper;

#ifdef RKT_UNICODE
	typedef WStringHelper	TStringHelper;
	//template class RKT_EXPORT StringHelperT<wchar_t,	char_traits_ex_t<wchar_t> >;
#else
	typedef MStringHelper	TStringHelper;
	//template class RKT_EXPORT StringHelperT<char,		char_traits_ex_t<char>	>;
#endif

} // namespace rkt

#endif // #ifdef SUPPORT_STRINGHELPER

#endif // RKT_STRINGHELPER_H