//==========================================================================
/**
* @file	  : ICacheManager.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-10-23   12:42
* purpose : 资源请求及对象缓存管理
*/
//==========================================================================

#ifndef RKT_ICACHEMANAGER_H
#define RKT_ICACHEMANAGER_H

#ifdef SUPPORT_OBJECTPOOL

#include <string>
#include "ostrbuf.h"

namespace rkt {

	//////////////////////////////////////////////////////////////////////////
	// 资源请求管理器
	//////////////////////////////////////////////////////////////////////////
	struct IAsynStreamSink;

	enum
	{
		IoResType_Unknow,	// 未知
		IoResType_Texture,	// 贴图
		IoResType_Model,	// 模型
		IoResType_Mpw,		// 物件
		IoResType_Fb,		// 动画
		IoResType_DiBiao,	// 地表
		IoResType_Mpg,		// 地图
		IoResType_Adorn,	// 装饰
		IoResType_Magic,	// 光效
		IoResType_Max
	};

#ifdef USE_STAT_INFO
	struct RequestStat
	{
		size_t	completeCount;	/// 完成的请求数
		size_t	requestCount;	/// 提交的IO请求数
		size_t	requestPeak;	/// 提交的请求峰值
		size_t  cancelCount;		/// 取消数
		size_t	needRequestCount;	/// 需要请求的数目
	};
#endif

	/// IO请求管理器，负责资源的请求
	struct IIoRequestManager
	{
#ifdef USE_STAT_INFO
		/// 获取统计信息
		virtual const RequestStat& getStatInfo() const = 0;
#endif

		/** 设置采用强制同步模式，当为true时，任何的异步加载都当作同步处理
		@param setting	是否同步请求
		@retval			返回先前的是否同步请求
		*/
		virtual bool setForceSynchronous(bool setting) = 0;

		/** 请求资源(本来直接可以通过文件系统发起IO请求，这里包装一下，主要是为了能够在两次主循环之间的请求列表使之具有优先级顺序)
		@param resName		资源的名字
		@param sink			资源回调接口
		@param userData		用户数据
		@param asynchronous	是否异步加载
		@param priority		资源优先级（当为同步时，忽略优先级，异步时，优先级仅仅是局部，也就是在submitRequestList调用前具有请求优先级）
		@note 应用层自己有必要保证同样的resName和sink不应该request多次！！
		*/
		virtual bool request(const char* resName, IAsynStreamSink* sink, ulong userData = 0, bool asynchronous = true, int priority = 0, uint resType = IoResType_Unknow) = 0;

		/** 取消资源的请求
		@param resName		资源的名字
		@param sink			回调接口，如果设置为0，则取消该资源的所有回调
		@note				一般，如果资源还没有加载完毕时就需要放弃该资源的加载，则必须调用该方法，否则sink可能无效了，导致非法
		*/
		virtual void cancel(const char* resName, IAsynStreamSink* sink = 0) = 0;

		/** 向主循环提交请求列表（用于异步请求，从请求列表中提交给系统去执行异步IO）
		该函数一般在app::onIdle()里面被调用
		@param count		提交的个数，如果为0，则提交队列中的所有请求
		*/
		virtual void submitRequestList(ulong count = 0) = 0;

		/**
			输出监控信息，用于调试优化
		*/
		virtual void printMonitorInfo() = 0;

		/// 释放
		virtual void release() = 0;
	};




	//////////////////////////////////////////////////////////////////////////
	// 缓存管理器
	//////////////////////////////////////////////////////////////////////////
	typedef std::string	CacheId;	/// 缓存唯一ID

	enum CacheType{CacheType_Mpw, CacheType_Model, CacheType_Texture, CacheType_CubeTexture};

	/// 可缓存对象接口
	struct ICache
	{
		/// 获取缓存的Id
		virtual const CacheId& getId() const = 0;

		/// 获取缓存占用内存的大小
		virtual size_t getSize() const = 0;

		/// 被缓存管理器遗弃(实现类可以在该回调中调用如delete this语句)
		virtual void onDiscard() = 0;

		/// 获取缓存内含的用户数据
		virtual void* getUserData() = 0;

		/// 被扔入垃圾筒时回调
		virtual void onRecycled() = 0;

		/// 获取缓冲区的类型
		virtual CacheType getType() const = 0;
	};

#ifdef USE_STAT_INFO
	struct CacheStat
	{
		size_t	curNodeCount;		/// 完成的请求数
		size_t	recycleNodeCount;	/// 提交的IO请求数
		size_t	cachePeak;			/// 缓存的峰值
		size_t	curMemorySize;		/// 当前内存占用量
		size_t	maxPoolSize;		/// 最大池内存大小
		size_t	requestCount;		/// 请求数
		size_t	hitCount;			/// 命中数
	};
#endif

	/// 缓存管理器接口，负责对象的缓存
	struct ICacheManager
	{
#ifdef USE_STAT_INFO
		/// 获取统计信息
		virtual const CacheStat& getStatInfo() const = 0;
#endif

		/** 设置缓冲池的大小
		@param maxPoolSize	最大缓冲池大小
		@retval	返回先前的最大缓冲池大小
		*/
		virtual size_t setMaxPoolSize(size_t maxPoolSize) = 0;

		/// 获取最大缓冲池大小
		virtual size_t getMaxPoolSize() const = 0;

		/// 获取当前缓冲池大小
		virtual size_t getPoolSize() const = 0;

		/** 判断缓存是否存在
		@param id	缓存的Id
		@retval		存在返回true，否则返回0，不改变引用计数
		*/
		virtual bool isCacheExist(const CacheId& id) const = 0;

		/** 获取缓存
		@param id	缓存的Id
		@retval		成功返回缓存对象，否则返回0，如果发现会递增引用计数
		*/
		virtual ICache* getCache(const CacheId& id) = 0;

		/** 当有的缓存的内存大小发生改变时，通知管理器作处理
		@param id	缓存的Id
		*/
		virtual void onCacheSizeChanged(const CacheId& id) = 0;

		/** 添加缓存
		@param cache	缓存对象
		@retval			成功返回true，否则返回false
		@remark			如果在缓存列表中发现有跟加入的缓存的Id相同的话，将返回false
		*/
		virtual bool addCache(ICache* cache) = 0;

		/** 移除缓存，当引用计数为0时，自动加入到垃圾箱
		@param id		缓存的Id
		@retval			当该缓存被立即清理后返回true(此时不应该在removeCache后面做其他事情，而应该立即返回)，否则还处于缓存状态则返回false
		*/
		virtual bool removeCache(const CacheId& id) = 0;

		/// 清空缓存数据
		virtual void clear() = 0;

		/// 释放
		virtual void release() = 0;

		/// 清空指定类型的缓存数据
		virtual void clearByType(CacheType type) = 0;

		/// 打印缓存信息
		virtual void dump(ostrbuf& osb) const = 0;

		/// 更新缓存
		virtual void update(int timeout) = 0;
	};

	/// 获取系统的IO请求管理器
	RKT_API IIoRequestManager* getIoRequestManager();

	/// 获取系统的缓存管理器
	RKT_API ICacheManager* getCacheManager();

} // namespace

#endif // #ifdef SUPPORT_OBJECTPOOL

#endif // RKT_ICACHEMANAGER_H