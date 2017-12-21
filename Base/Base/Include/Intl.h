//==========================================================================
/**
* @file	  : Intl.h
* @author : PeakGao <peakgao163@163.com>
* created : 2010-6-29   11:34
* purpose : 
*/
//==========================================================================

#ifndef __Intl_h__
#define __Intl_h__

#include "Common.h"


#ifdef SUPPORT_INTL

namespace rkt {

	enum
	{
		FileType_Cpp = 1,
		FileType_Lua = 2,
		FileType_Csv = 4,
		FileType_Xml = 8,

		FileType_Max = 4,
	};

	/** 初始化国际化模块
	@param mo_path mo文件的路径，只需要语言字符串的父目录即可，比如 E:\Rocket20100701\Tools\gettext\i18n\zh_CN\LC_MESSAGES\demo.mo ，这里可传入E:\Rocket20100701\Tools\gettext\i18n
	@param text_domain 文本域名，就是上面例子里面的demo字符串
	*/
	//RKT_API bool init_intl(const char* mo_path, const char* text_domain);
	// for lua, csv, xml
	RKT_API const char* gettext_utf8(const char* utf8_msgid, size_t utf8_len, ulong msgType);

	// for c++
	RKT_API const char* gettext_ucs2(const wchar_t* ucs2_msgid);

} // rkt

#endif // #ifdef SUPPORT_INTL

#endif // __Intl_h__