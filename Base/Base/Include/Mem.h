//==========================================================================
/**
* @f	  : Mem.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-4-11   10:25
* purpose : 简单内存管理
*/
//==========================================================================

#ifndef RKT_MEM_H
#define RKT_MEM_H

#include "Common.h"

#ifdef SUPPORT_MEMORY

#ifdef __cplusplus
#ifndef USE_SYSTEM_MEMORY
		// 下面使用定制的内存分配
		//////////////////////////////////////////////////////////////////////////
		// 分配，释放相关
		RKT_API void*	mem_alloc(size_t s);
		RKT_API void*	mem_realloc(void* p, size_t s);
		RKT_API void	mem_free(void* p);

		/// 获取内存分配总量
		RKT_API size_t	mem_size();

		/// 统计内存(目前仅仅mdl实现了)
		RKT_API void	mem_stat();

#endif // !USE_SYSTEM_MEMORY

#endif // __cplusplus



//////////////////////////////////////////////////////////////////////////
// 下面为重载 new, delete 操作符
#ifndef USE_SYSTEM_MEMORY

	inline void* _cdecl operator new(size_t s)								{ return mem_alloc(s); }
	inline void  _cdecl operator delete(void* p)							{ mem_free(p); }
	inline void* _cdecl operator new[](size_t s)							{ return mem_alloc(s); }
	inline void  _cdecl operator delete[](void* p)							{ mem_free(p); }
	
	#define malloc(s)			mem_alloc(s)
	#define calloc(n, s)		mem_alloc(s*n)
	#define realloc(p, s)		mem_realloc(p, s)
	#define _recalloc(p, n, s)	mem_realloc(p, s*n)
	#define free(p)				mem_free(p)

#endif // !USE_SYSTEM_MEMORY


#endif // #ifndef SUPPORT_MEMORY

#define RKT_NEW	new

#endif // RKT_MEM_H