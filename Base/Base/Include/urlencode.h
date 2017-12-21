//==========================================================================
/**
* @file	  : urlencode.h
* @author : PeakGao <peakgao163@163.com>
* created : 2010-1-30   16:04
* purpose : 
*/
//==========================================================================

#ifndef __urlencode_h__
#define __urlencode_h__

#include "Common.h"

#include <string>

#ifdef SUPPORT_URL

namespace rkt {

	class RKT_EXPORT Url
	{
	public:
		static std::string encode(const std::string& src);
		static std::string decode(const std::string& src);
	};

} // rkt

#endif // SUPPORT_URL

#endif // __urlencode_h__