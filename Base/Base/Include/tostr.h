//==========================================================================
/**
* @file	  : tostr.h
* @author : PeakGao <peakgao163@163.com>
* created : 2011-6-14   12:06
* purpose : 
*/
//==========================================================================

#ifndef __tostr_h__
#define __tostr_h__

#include "ostrbuf.h"
#include "char_traits_ex.h"

#include <string>

namespace rkt {

	template<class _Elem, class _Traits>
	struct _tostr_t
	{
		enum { FixedBufSize = 64 };
		_Elem	m_sz[FixedBufSize];
		_Elem*	m_ss;

		_Elem* str() const					{ return m_ss; }
		const _Elem* c_str() const			{ return m_ss; }

		_tostr_t(int v)		: m_ss(m_sz)	{ _Traits::from_int(v, m_sz, FixedBufSize, 10); }
		_tostr_t(uint v)	: m_ss(m_sz)	{ _Traits::from_uint(v, m_sz, FixedBufSize, 10);}
		_tostr_t(long v)	: m_ss(m_sz)	{ _Traits::from_long(v, m_sz, FixedBufSize, 10); }
		_tostr_t(ulong v)	: m_ss(m_sz)	{ _Traits::from_ulong(v, m_sz, FixedBufSize, 10); }
		_tostr_t(int64 v)	: m_ss(m_sz)	{ _Traits::from_int64(v, m_sz, FixedBufSize, 10); }
		_tostr_t(uint64 v)	: m_ss(m_sz)	{ _Traits::from_uint64(v, m_sz, FixedBufSize, 10); }
		_tostr_t(float v)	: m_ss(m_sz)	{ _Traits::from_float(v, m_sz, FixedBufSize); }
		_tostr_t(double v)	: m_ss(m_sz)	{ _Traits::from_double(v, m_sz, FixedBufSize); }
		_tostr_t(const _Elem* v)			{ m_ss = (_Elem*)v; }
		_tostr_t(const std::basic_string<_Elem, std::char_traits<_Elem>, std::allocator<_Elem> >& v)	{ m_ss = (_Elem*)v.c_str(); }
		_tostr_t(const basic_ostrbuf<_Elem, char_traits_ex_t<_Elem>, 256, 256 >& v)						{ m_ss = (_Elem*)v.c_str(); }
	};

	typedef _tostr_t<char, char_traits_ex_t<char> >				toastr;
	typedef _tostr_t<wchar_t, char_traits_ex_t<wchar_t> >		towstr;

#define _AS(x)	toastr((x)).c_str()
#define _WS(x)	towstr((x)).c_str()

#define _S			_AS

#ifdef RKT_UNICODE
#define _TS			_WS
#else
#define _TS			_AS
#endif

} // rkt
#endif // __tostr_h__