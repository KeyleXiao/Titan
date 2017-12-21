//==========================================================================
/**
* @file	  : strconv.h
* @author : PeakGao <peakgao163@163.com>
* created : 2010-5-19   11:35
* purpose : 字符串编码转换
*/
//==========================================================================

#ifndef __strconv_h__
#define __strconv_h__

#include "Common.h"

namespace rkt {

	enum { FIXED_CONV_BUF_LEN	= 256 }; /// 字符串转换的固定缓冲大小

	class RKT_EXPORT strconv_a2w
	{
		wchar_t*	m_psz;
		wchar_t		m_szBuffer[FIXED_CONV_BUF_LEN];

	public:
		strconv_a2w(const char* sz, unsigned int codePage = -1);
		strconv_a2w(char* sz, unsigned int codePage = -1);
		strconv_a2w(const std::string& str, unsigned int codePage = -1);
		strconv_a2w(std::string& str, unsigned int codePage = -1);
		~strconv_a2w();

		operator wchar_t*() const	{ return m_psz; }
		wchar_t* str() const		{ return m_psz; }

	private:
		void conv(const char* sz, unsigned int codePage);

		strconv_a2w();
		strconv_a2w(const strconv_a2w&);
		strconv_a2w& operator=(const strconv_a2w&);
	};


	class RKT_EXPORT strconv_w2a
	{
		char*	m_psz;
		char	m_szBuffer[FIXED_CONV_BUF_LEN*4];

	public:
		strconv_w2a(const wchar_t* sz, unsigned int codePage = -1);
		strconv_w2a(wchar_t* sz, unsigned int codePage = -1);
		strconv_w2a(const std::wstring& str, unsigned int codePage = -1);
		strconv_w2a(std::wstring& str, unsigned int codePage = -1);
		~strconv_w2a();

		operator char*() const	{ return m_psz; }
		char* str() const		{ return m_psz; }

	private:
		void conv(const wchar_t* sz, unsigned int codePage);

		strconv_w2a();
		strconv_w2a(const strconv_w2a&);
		strconv_w2a& operator=(const strconv_w2a&);
	};

	RKT_API wchar_t* conv_a2w(const char* sz, unsigned int codePage = -1);
	RKT_API char* conv_w2a(const wchar_t* sz, unsigned int codePage = -1);
	RKT_API void conv_free(void* sz);

} // rkt


#ifndef CP_UTF8 // for winnls.h
#define CP_UTF8 65001 
#endif

#define a2w(s)			(rkt::strconv_a2w((s)).str())
#define w2a(ws)			(rkt::strconv_w2a((ws)).str())
#define a2utf8(s)		(rkt::strconv_w2a(rkt::strconv_a2w((s)).str(), CP_UTF8).str())
#define w2utf8(ws)		(rkt::strconv_w2a((ws), CP_UTF8).str())
#define utf82a(s)		(rkt::strconv_w2a(rkt::strconv_a2w((s), CP_UTF8).str()).str())
#define utf82w(s)		(rkt::strconv_a2w((s), CP_UTF8).str())

#define a2w_ex(s)		(rkt::conv_a2w((s)))
#define w2a_ex(ws)		(rkt::conv_w2a((ws)))
#define a2utf8_ex(s)	(rkt::conv_w2a(a2w(s), CP_UTF8))
#define w2utf8_ex(s)	(rkt::conv_w2a((s), CP_UTF8))
#define utf82a_ex(s)	(rkt::conv_w2a(utf82w( (s) ) ))
#define utf82w_ex(s)	(rkt::conv_a2w((s), CP_UTF8))

#ifdef _UNICODE

#define a2t				a2w
#define w2t
#define t2a				w2a
#define t2w
#define utf82t			utf82w
#define t2utf8			w2utf8

#define a2t_ex			a2w_ex
#define w2t_ex			
#define t2a_ex			w2a_ex
#define t2w_ex
#define utf82t_ex		utf82w_ex
#define t2utf8_ex		w2utf8_ex

#ifdef RKT_UTF8
#define my_utf82t		utf82w
#define my_t2utf8		w2utf8
#define my_utf82t_ex	utf82w_ex
#define my_t2utf8_ex	w2utf8_ex
#define my_conv_free	conv_free
#else
#define my_utf82t		a2w
#define my_t2utf8		w2a
#define my_utf82t_ex	utf82a_ex
#define my_t2utf8_ex	w2a_ex
#define my_conv_free	conv_free
#endif

#else

#define a2t
#define w2t				w2a
#define t2a
#define t2w				a2w
#define utf82t			utf82a
#define t2utf8			a2utf8

#define a2t_ex
#define w2t_ex			w2a_ex
#define t2a_ex
#define t2w_ex			a2w_ex
#define utf82t_ex		utf82a_ex
#define t2utf8_ex		a2utf8_ex

#ifdef RKT_UTF8
#define my_utf82t		utf82a	
#define my_t2utf8		a2utf8
#define my_utf82t_ex	utf82a_ex
#define my_t2utf8_ex	a2utf8_ex
#define my_conv_free	conv_free
#else
#define my_utf82t		
#define my_t2utf8		
#define my_utf82t_ex
#define my_t2utf8_ex
#define my_conv_free
#endif

#endif

#endif // __strconv_h__