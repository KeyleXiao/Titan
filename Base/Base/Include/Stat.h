//==========================================================================
/**
* @file	  : Stat.h
* @author : PeakGao <peakgao163@163.com>
* created : 2011-4-2   10:09
* purpose : 统计相关
*/
//==========================================================================

#ifndef __Stat_h__
#define __Stat_h__

#include "Common.h"

#ifdef SUPPORT_STAT

namespace rkt {

	struct Stat_IO
	{
		size_t openCount;	/// 打开文件次数
		size_t closeCount;	/// 关闭文件次数
		size_t readCount;	/// 读文件次数
		uint64 readSize;	/// 读文件总大小
		float readTicks;	/// 读文件总耗时

		Stat_IO()
		{
			memset(this, 0, sizeof(Stat_IO));
		}
	};

	/* 统计信息列表
		id	-	struct		-	describe
		0	-	Stat_IO		-	Sync IO	
		1	-	Stat_IO		-	Async IO
	*/

	RKT_API void* getStatInfo(int id);


	extern Stat_IO	g_SyncIOStateInfo;
	extern Stat_IO	g_AyncIOStateInfo;


} // rkt

#endif // #ifdef SUPPORT_STAT

#endif // __IOStat_h__