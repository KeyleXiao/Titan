//==========================================================================
/**
* @file	  : Handle.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-1-12   11:09
* purpose : 句柄管理，默认配置为支持线程安全
*/
//==========================================================================

#ifndef __HANDLE_H__
#define __HANDLE_H__

#include "Common.h"

#ifdef SUPPORT_HANDLE

namespace rkt {

/** 
 工作原理：句柄的原理有点类似于小内存分配的原理，首先他分配一定数量的句柄系
 列供使用（这个序列一旦用完，会重新realloc），并充分利用空闲的用户数据域，给
 他们初始化成链表关系，这样就几乎不存在遍历整个数组来查找空闲的句柄节点，系
 统总是记录一个当前空闲的节点索引（当一个句柄关闭后，他就是当前的空闲节点的索
 引），而每个空闲节点总是指向下一个空闲节点，所以基于这种思路，句柄的创建和关
 闭操作是非常快的，时间复杂度为O(1)。再看看句柄的有效性问题，前面讲到，每个句
 柄都包含有创建次数（Create Count），当一个空闲句柄第一次创建时，Count=1，这
 时，如果关闭该句柄，Count还是等于1，当下一个句柄创建请求过来，那么Count就递
 增为2，每个句柄域里面的Count自从初始化后就会一直递增，当递增到最大值后在下次
 创建时，会将Count进行回绕。
 */

#define INVALID_HANDLE					(handle)0 /// 无效句柄定义，用于判断返回的句柄是否是有效的,它跟isValidHandle不同
//#define RKT_HANDLE_DEBUG

	typedef ulong handle;


#ifdef RKT_HANDLE_DEBUG
#	define rktCreateHandle(userData)	createHandle((ulong)(userData), __FILE__, __LINE__)	/// 带调试信息的创建
#	define rktCloseHandle(h)			closeHandle(h, __FILE__, __LINE__)					/// 带调试信息的关闭
#else
#	define rktCreateHandle(userData)	createHandle((ulong)(userData))	/// 带调试信息的创建
#	define rktCloseHandle(h)			closeHandle(h)					/// 带调试信息的关闭
#endif


	/** 创建句柄
	 @param userData 句柄上附加的用户数据
	 @return 成功返回创建的句柄，否则返回NULL
	 @note 创建了句柄要记得关闭该句柄，这是一个好习惯，请采用 @c fclCreateHandle宏创建句柄，这样可以带上调试信息
	 @see closeHandle
	 */
	#ifdef RKT_HANDLE_DEBUG
	RKT_API handle createHandle(ulong userData, const char* file = 0, uint line = 0);
	#else
	RKT_API handle createHandle(ulong userData);
	#endif


	/** 关闭句柄
	 @param h 要关闭的句柄
	 @return 如果句柄有效则返回true，否则返回false
	 @note 请采用 @c fclCloseHandle宏关闭句柄，这样可以带上调试信息
	 @see createHandle
	 */
	#ifdef RKT_HANDLE_DEBUG
	RKT_API bool closeHandle(handle h, const char* file = 0, uint line = 0);
	#else
	RKT_API bool closeHandle(handle h);
	#endif


	/** 获得句柄上附加的用户数据
	 @param h 传入的句柄
	 @return 如果句柄有效则返回该句柄的用户数据，否则返回0
	 */
	RKT_API ulong getHandleData(handle h);


	/** 设置句柄上附加的用户数据
	 @param h 要附加用户数据的句柄
	 @param userData 要设置的用户数据
	 @return 如果句柄有效返回true，否则返回false
	 */
	RKT_API bool setHandleData(handle h, ulong userData);


	/** 判断句柄是否有效
	 @param h 要判断有效性的句柄
	 @return 有效返回true，否则返回false
	 */
	RKT_API bool isValidHandle(handle h);


	/** dump句柄表中的句柄信息
	 */
	RKT_API void dumpHandleTable();


	/// 轻量级的句柄模板，用于指针的安全存储
	template<typename T>
	class Handle
	{
	public:
		handle h;	/// 绑定指针的句柄

		Handle()
		{ h = createHandle(0); }
/*
		explicit Handle(handle _h) : h(_h)
		{ }*/
		
		Handle(const T* ptr)
		{ h = createHandle((ulong)ptr); }

		~Handle()
		{ closeHandle(h); }


		/** 为句柄绑定一个指针
		 @param ptr 传入的指针，可以为NULL
		 @remark 该函数一般用于默认构造下绑定一个指针或将句柄绑定的指针进行更换
		 */
		Handle& operator = (const T* ptr)
		{ setHandleData(h, (ulong)ptr); return *this; }


		/// 指针操作辅助操作
		T* operator->() const
		{ return (T*)getHandleData(h); }


		/// 直接获取句柄绑定的指针
		T* getPointer() const
		{ return (T*)getHandleData(h); }


		/// 直接获取句柄
		operator handle() const
		{ return h; }


		/// 判断句柄是否有效
		inline operator bool() const
		{ return isValidHandle(h); }
	};



} // namesapce

#endif // #ifdef SUPPORT_HANDLE

#endif // __HANDLE_H__
