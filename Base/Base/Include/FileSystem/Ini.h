//==========================================================================
/**
* @file	  : Ini.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-11-21   14:06
* purpose : ini文件读写
*/
//==========================================================================

#ifndef __Ini_h__
#define __Ini_h__

#include "Common.h"

#ifdef SUPPORT_STREAM

#include "path.h"

#include <string>
#include <vector>

#define USE_WIN32_INI	// 先一直打开吧，因为新的ini解析存在跟内存的冲突，因为内存分配类型要读取ini后才决定，而ini的读取会分配内存

/**
	说明：
	1、section,key等字符串是大小写不敏感的形式
	2、获取的字符串值，其两边被系统进行空格裁剪

*/

namespace rkt {

	class RKT_EXPORT Ini
	{
	public:

		Path	mFileName; /// ini文件名

	public:    
		Ini();
		Ini(const char* filename);

		/// 设置ini文件名
		void setPath(const char* filename);
		const char* getPath() const;

		/// 获取整数    
		uint getInt(const char* section, const char* key, int def = 0) const;
		/// 设置整数    
		bool setInt(const char* section, const char* key, int value);

		/// 获取字符串
		std::string getString(const char* section, const char* key, const char* def = 0) const;
		size_t getString(const char* section, const char* key, char* return_string, size_t size, const char* def = 0) const;
		/// 设置字符串
		bool setString(const char* section, const char* key, const char* value);
/*
		/// 获取固定长度结构(内存存为16进制)
		bool getStruct(const char* section, const char* key, void* struct_ptr, size_t struct_size) const;
		/// 设置固定长度结构(内存存为16进制)
		bool setStruct(const char* section, const char* key, void* struct_ptr, size_t struct_size);

		/// 获取节(返回的串中，每一行是一个以'\0'结尾的缓冲，我们以^标识字符串中止符号，返回的字符串格式如：a=1^b=3^^ 返回值为8)
		ulong getSection(const char* section, char* return_string, size_t size) const;
		//void getSection(const char* section, std::vector<std::string>& strlist) const;
		/// 设置节
		ulong setSection(const char* section, const char* str);
		//void setSection(const char* section, const std::vector<std::string>& strlist);

		/// 获取节名(返回的字符串类似getSection的格式)
		ulong getSectionNames(char* return_string, size_t size) const;
		//void getSectionNames(std::vector<std::string>& strlist) const;
		*/
	};

} // namespace

#endif // #ifdef SUPPORT_STREAM

#endif // __Ini_h__