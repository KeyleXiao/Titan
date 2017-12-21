//==========================================================================
/**
* @file	  : mdl.h
* @author : PeakGao <peakgao163@163.com>
* created : 2009-6-19   18:03
* purpose : Memory Debug Library
*/
//==========================================================================

#ifndef __mdl_h__
#define __mdl_h__

#ifdef _DEBUG
#pragma comment(lib, "mdl_d.lib")
#else
#pragma comment(lib, "mdl.lib")
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	__declspec(dllimport) void*		mdl_alloc(size_t size);
	__declspec(dllimport) void*		mdl_realloc(void* p, size_t size);
	__declspec(dllimport) void		mdl_free(void* p);
	__declspec(dllimport) size_t	mdl_memsize(void* p); /// 根据地址获取内存大小
	__declspec(dllimport) size_t	mdl_size(); /// 总分配尺寸
	__declspec(dllimport) void		mdl_stat();	/// 统计内存数据

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // __mdl_h__
