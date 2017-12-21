/**
	Created 2007.6.7 by PeakGao
	Email: peakgao163@163.com
	Modified for support UNICODE by PeakGao 2010.5.18
*/

#ifndef RKT_OSTRBUF_H
#define RKT_OSTRBUF_H

#include "Common.h"

#include <stdarg.h> // for va_start

#include "char_traits_ex.h"
#include "StringCommon.h"

namespace rkt {

/**
	示例
	
	@code

	int dosomething(TCHAR* dest_buf, int max_size)
	{
		TCHAR buf[] = "some thing some thing some thing some thing ";
		int actualCpySize = max_size / 2 + 1;
		std::char_traits<TCHAR>::_Copy_s(dest_buf, max_size, buf, actualCpySize);
		return actualCpySize;
	}

	void test_ostrbuf()
	{
		ostrbuf osb;

		// 如果怕缓冲区不够，先直接设置一下
		//osb.reserve(1024);

		// 这些操作类似ostream的功能
		osb<<(unsigned int)0xffffffff<<","<<(int)0x80000001<<endl;// endl即换行符
		osb<<99<<","<<(short)14<<","<<-15<<endl;
		osb<<(__int64)0x8000000000000001<<","<<(unsigned __int64)0xffffffffffffffff<<endl;
		osb<<15.1367891024f<<","<<9999.123456789<<endl;

		// 将一个字符串的子串写入
		osb<<ostrbuf::substr("我们你们他们abcdefg", 12)<<endl;

		// 定制的格式化
		osb<<ostrbuf::formatf("%02.6f", 3.1415926f)<<endl;
		osb<<_fi("%04d", 25)<<endl; // 这种写法更简洁
		osb<<_fi("%X", 255)<<endl;


		// 直接操作缓冲区示例（在不知情的情况下不推荐这样操作，知情的情况下，也不错，很方便，至少少了字符串拷贝操作）
		osb.grow(64);	// 可能需要128字节空间，这个数值必须正确才能防止缓存越界
		int off = dosomething(osb.cur(), 64); // dosomething 已经完全抛开了 ostrbuf 类，不受任何拘束，危险！
		osb.seek(off);	// 手动将位置seek到正确位置
		osb<<ends;		// 因上面的 dosomething 没有在缓冲区最后加入'\0'，这里不能忘记，除了像 dosomething 这样直接对缓冲区进行写外，其他的操作都会自动加'\0'

		std::cout<<WtoA(osb.c_str());
	}

	//结果:
	NO UNICODE:
	4294967295,-2147483647
	99,14,-15
	-9223372036854775807,18446744073709551615
	15.136789,9999.123457
	我们你们他们
	3.141593
	0025
	FF
	some thing some thing some thing 

	UNICODE:
	4294967295,-2147483647
	99,14,-15
	-9223372036854775807,18446744073709551615
	15.136789,9999.123457
	我们你们他们abcdef
	3.141593
	0025
	FF
	some thing some thing some thing

	@endcode
*/

/**
	字符串输出缓冲，功能类似ostream，但更加简洁

	@param _Elem			字符类型
	@param _Traits			字符特性
	@param _GrowElems		缓冲区的递增大小
	@param _FixedArrSize	内部的字符数组的大小，当该字符数组容量不够时，才new出真正的缓冲区
*/
template <class _Elem, class _Traits, int _GrowElems, int _FixedArrSize = 256>
class basic_ostrbuf
{
private:
	enum { _MyGrowElems = (_GrowElems > 0 ? ((_GrowElems + 7) & ~7) : 256) };
	enum { _MyFixedArrSize = (_FixedArrSize > 0 ? ((_FixedArrSize + 7) & ~7) : 256) };
	enum { _LongMaxLength = 16-1, _Int64MaxLength = 24-1, _DoubleMaxLength = 48-1 }; // 8*n-1

	_Elem*	_buf;					/// 字符串缓冲区
	size_t	_pos;					/// 当前写入位置
	size_t	_cap;					/// 缓冲区容量
	_Elem	_arr[_MyFixedArrSize];	/// 字符数组，当操作小缓冲区时，可以利用他，这样就没必要new内存


	/// 用于用户自定义的格式化，基于sprintf函数的格式化功能
public:
	/*
	template<class T>
	struct _Format 
	{
		const _Elem* _f;
		T _v;
		_Format(const _Elem* f, const T& v) : _f(f), _v(v) { }
	};

	typedef _Format<float>						formatf;
	typedef _Format<double>						formatd;
	typedef _Format<int>						formati;
	typedef _Format<__int64>					formati64;*/
	typedef std::pair<const _Elem*, size_t>		substr;

	// 构造（析构）
public:
	basic_ostrbuf() : _buf(_arr), _pos(0), _cap(_MyFixedArrSize)
	{
		_arr[0] = 0;
	}
	~basic_ostrbuf()
	{
		if (_buf != _arr) delete[] _buf;
	}

	// 基本方法
public:
	void clear()
	{
		grow(1);
		_pos = 0;
		_buf[_pos] = 0;
	}

	_Elem* str() const				{ return _buf; }
	const _Elem* c_str() const		{ return _buf; }
	size_t length() const			{ return _pos; }
	size_t size() const				{ return _pos; }
	size_t capacity() const			{ return _cap; }
	_Elem* cur() const				{ return _buf + _pos;}

	basic_ostrbuf& seek(int offset)
	{
		int new_pos = (int)_pos + offset;
		if (new_pos >= (int)_cap) new_pos = (int)_cap - 1;
		if (new_pos < 0) new_pos = 0;
		_pos = (size_t)new_pos;
		return (*this);
	}

	/// 预定一个用户设定的缓冲大小（对于大缓冲或已知大小的缓冲，用这种方法可以减少new/delete的次数）
	void reserve(size_t cap)
	{
		if (cap >= _cap)
			grow(cap - _pos);
	}

	// operator<<
public:
	/// 用于endl和ends
	basic_ostrbuf& operator<<(basic_ostrbuf& (__cdecl * fn)(basic_ostrbuf&)) 
	{
		return ((*fn)(*this));
	}

	/// 输入另一个对象
	basic_ostrbuf& operator<<(const basic_ostrbuf& other)
	{
		return push(other.c_str(), other.length());
	}


	/// 输入字符串
	basic_ostrbuf& operator<<(const _Elem* _Ptr)
	{
		return (_Ptr ? push(_Ptr, _Traits::length(_Ptr)) : (*this));
	}
	/// 输入一个子串
	basic_ostrbuf& operator<<(const substr& _str)
	{
		return push(_str.first, _str.second);
	}
	/// 输入std::string对象
	basic_ostrbuf& operator<<(const std::basic_string<_Elem, std::char_traits<_Elem>, std::allocator<_Elem> >& _Str)
	{
		return push(_Str.c_str(), _Str.length());
	}
	/// 输入一个字符
	/// 注：这里c当字符处理，不当0-255的数字处理，0-255的数字，需要转换为short,int,long等进行处理
	basic_ostrbuf& operator<<(_Elem _Ch)
	{
		return put(_Ch);
	}

	/// 导入数字
	basic_ostrbuf& operator<<(short int si)
	{
		return operator<<(long(si));
	}
	basic_ostrbuf& operator<<(unsigned short int usi)
	{
		return operator<<(unsigned long(usi));
	}
	basic_ostrbuf& operator<<(int __w64 i)
	{
#ifdef INT_64_BITS
		return operator<<(__int64(i));
#else
		return operator<<(long(i));
#endif
	}
	basic_ostrbuf& operator<<(unsigned int __w64 ui)
	{
#ifdef INT_64_BITS
		return operator<<(unsigned __int64(ui));
#else
		return operator<<(unsigned long(ui));
#endif
	}
	basic_ostrbuf& operator<<(long l)
	{
		grow(_LongMaxLength);
		_Traits::from_long(l, cur(), _LongMaxLength, 10);
		_pos += _Traits::length(cur());
		return (*this);
	}
	basic_ostrbuf& operator<<(unsigned long ul)
	{
		grow(_LongMaxLength);
		_Traits::from_ulong(ul, cur(), _LongMaxLength, 10);
		_pos += _Traits::length(cur());
		return (*this);
	}
	basic_ostrbuf& operator<<(__int64 i64)
	{
		grow(_Int64MaxLength);
		_Traits::from_int64(i64, cur(), _Int64MaxLength, 10);
		_pos += _Traits::length(cur());
		return (*this);
	}
	basic_ostrbuf& operator<<(unsigned __int64 ui64)
	{
		grow(_Int64MaxLength);
		_Traits::from_uint64(ui64, cur(), _Int64MaxLength, 10);
		_pos += _Traits::length(cur());
		return (*this);
	}
	basic_ostrbuf& operator<<(float f)
	{
		grow(_DoubleMaxLength);
		_Traits::from_float(f, cur(), _DoubleMaxLength);
		_pos += _Traits::length(cur());
		return (*this);
	}
	basic_ostrbuf& operator<<(double d)
	{
		grow(_DoubleMaxLength);
		_Traits::from_double(d, cur(), _DoubleMaxLength);
		_pos += _Traits::length(cur());
		return (*this);
	}


	template<int _Size = 256>
	struct _Format
	{
		_Elem		_Buff[_Size];

		_Format(const _Elem* _Fmt, ...)
		{
			va_list _List;
			va_start(_List, _Fmt);
			int ret = _Traits::vsnprintf(_Buff, _Size, _TRUNCATE, _Fmt, _List);
			if (ret < 0) _Buff[0] = 0;
			va_end(_List);
		}
	};

	template<int _Size = 256>
	struct _XFormat
	{
		_Elem		_Buff[_Size];

		_XFormat(const _Elem* _Fmt, ...)
		{
			va_list _List;
			va_start(_List, _Fmt);
			int ret = _Traits::xvsnprintf(_Buff, _Size, _TRUNCATE, _Fmt, _List);
			//if (ret < 0) _Buff[0] = 0;
			va_end(_List);
		}
	};

	template<int _Size>
	basic_ostrbuf& operator<<(const _Format<_Size>& val)
	{
		return push(val._Buff, _Traits::length(val._Buff));
	}

	template<int _Size>
	basic_ostrbuf& operator<<(const _XFormat<_Size>& val)
	{
		return push(val._Buff, _Traits::length(val._Buff));
	}

	basic_ostrbuf& add_format(const _Elem* _Format, ...)
	{
		if (_Format)
		{
			va_list _List;
			va_start(_List, _Format);

			int len = _Traits::vscprintf(_Format, _List);
			grow(len + 1);
			_Traits::vsnprintf(cur(), _cap - _pos, _TRUNCATE, _Format, _List);
			_pos += len;
			va_end(_List);
		}
		return (*this);
	}

	basic_ostrbuf& add_xformat(const _Elem* _Format, ...)
	{
		if (_Format)
		{
			va_list _List;
			va_start(_List, _Format);

			int len = _Traits::xvscprintf(_Format, _List);
			grow(len + 1);
			_Traits::xvsnprintf(cur(), _cap - _pos, _TRUNCATE, _Format, _List);
			_pos += len;
			va_end(_List);
		}
		return (*this);
	}
/*
	/// 定制的格式化
	template<class T>
	basic_ostrbuf& operator<<(const _Format<T>& val)
	{
		add_format(_DoubleMaxLength, val._f, val._v);
		return (*this);
	}*/

public:
	void grow(size_t len)
	{
		if (_pos + len >= _cap)
		{
			// 计算新的尺寸，并8字节对齐
			size_t new_size = _pos + len + 1;
			size_t new_cap = _cap ? _cap : _MyGrowElems;
			while (new_cap < new_size)
				new_cap += _MyGrowElems;
			new_cap = (new_cap + 7) & ~7;

			_Elem* new_buf = new _Elem[new_cap];
			if (_pos > 0)
			{
				_Traits::_Copy_s(new_buf, new_cap, _buf, _pos + 1);
				if (_buf != _arr)
					delete[] _buf;
			}
			else
			{
				_Traits::assign(new_buf[0], _Elem());
			}

			_buf = new_buf;
			_cap = new_cap;
		}
	}

	basic_ostrbuf& put(_Elem c)
	{
		grow(1);
		_buf[_pos] = c;
		if (c != 0)
			_Traits::assign(_buf[++_pos], _Elem());
		return (*this);
	}

	basic_ostrbuf& push(const _Elem* _Ptr, size_t len) // 外部保证 len <= strlen(_Ptr)
	{
		if (_Ptr && len > 0)
		{
			if (len == (size_t)-1)
				len = _Traits::length(_Ptr);

			if (len > 0)
			{
				grow(len);
				_Traits::_Copy_s(_buf + _pos, len, _Ptr, len);
				_Traits::assign(_buf[_pos += len], _Elem());
			}
		}
		return (*this);
	}
};



/// 字符串终止
template<class _Elem, class _Traits, int _GrowBytes, int _ArrSize>
inline basic_ostrbuf<_Elem, _Traits, _GrowBytes, _ArrSize>& _cdecl ends(basic_ostrbuf<_Elem, _Traits, _GrowBytes, _ArrSize>& osb)
{
	return osb.put((_Elem)'\0');
}

/// 换行
template<class _Elem, class _Traits, int _GrowBytes, int _ArrSize>
inline basic_ostrbuf<_Elem, _Traits, _GrowBytes, _ArrSize>& _cdecl endl(basic_ostrbuf<_Elem, _Traits, _GrowBytes, _ArrSize>& osb)
{
	return osb.put((_Elem)'\n');
}


typedef basic_ostrbuf<char,		char_traits_ex_t<char>,		256, 256>		mostrbuf,ostrbuf;
typedef basic_ostrbuf<wchar_t,	char_traits_ex_t<wchar_t>,	256, 256>		wostrbuf;

/*
#ifdef RKT_UNICODE
typedef wostrbuf	ostrbuf;
template class RKT_EXPORT basic_ostrbuf<wchar_t,	char_traits_ex_t<wchar_t>,	256, 256>;
#else
typedef mostrbuf	ostrbuf;
template class RKT_EXPORT basic_ostrbuf<char,		char_traits_ex_t<char>,		256, 256>;
#endif
*/

// 为了方便使用，作这样的 typedef
//typedef ostrbuf::formatf	_ff;
//typedef ostrbuf::formatd	_fd;
//typedef ostrbuf::formati	_fi;
//typedef ostrbuf::formati64	_fi64;
typedef ostrbuf::substr			_ss;

typedef ostrbuf::_Format<256>	_f,_ff,_fd,_fi,_fi64; // 后面几个用于兼容
typedef ostrbuf::_Format<512>	_f512;
typedef ostrbuf::_Format<1024>	_f1024;
typedef ostrbuf::_Format<2048>	_f2048;

typedef ostrbuf::_XFormat<256>	_xf;
typedef ostrbuf::_XFormat<512>	_xf512;
typedef ostrbuf::_XFormat<1024>	_xf1024;
typedef ostrbuf::_XFormat<2048>	_xf2048;

} // namespace rkt




/** 
示例：用户自定义类型的定制方法，定义后可以这样处理你的自定义类型：
YourType  yt;
ostrbuf<<yt;
*/
/*
template<class _Elem, class _Traits, int _GrowBytes, int _ArrSize>
inline rkt::basic_ostrbuf<_Elem, _Traits, _GrowBytes, _ArrSize>& _cdecl operator<<(rkt::basic_ostrbuf<_Elem, _Traits, _GrowBytes, _ArrSize>& osb, const YourType& val)
{
	if (val.c_str())
		osb.push(val.c_str(), val.length());
	return osb;
}
*/

#endif // RKT_OSTRBUF_H