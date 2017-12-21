//==========================================================================
/**
* @file	  : MemObj.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-4-11   15:40
* purpose : 
*/
//==========================================================================

#ifndef RKT_MEMOBJ_H
#define RKT_MEMOBJ_H

#include "Common.h"

#ifdef SUPPORT_MEMORY

#include "Thread.h"
#include "Trace.h"

#pragma warning(push)
#pragma warning(disable : 4251)

namespace rkt {


	/** 内存对象使用示例

	如果类声明为内存对象，那么每次new一个类时，实际上连续new了n个类实例（n默认为64，用户可以自己权衡选择个数）
	每次释放一个对象时，没有真正的free，只是进行了回收，便于下次请求时，立即返回而避免内存分配，支持继承树上的各
	个节点类采用内存对象策略
	@code
	//test.h
	class CMyTest
	{
		DECLARE_MEMOBJ
	public:
		CMyTest();
		~CMyTest();
		void foo();
	private:
		ulong m_myVar;
	};

	// test.cpp
	IMPLEMENT_MEMOBJ(CMyTest, 64)
		CMyTest::CMyTest()
	{
	}
	CMyTest::~CMyTest()
	{
	}
	void foo()
	{
	}
	@endcode
	*/

	/// 数据链
	struct RKT_EXPORT DataChain
	{
		DataChain* prev;	/// 前一个数据链

		void* data()		{ return this + 1; }

		static DataChain* create(DataChain*& head, ulong blockCount, ulong blockSize)
		{
			assert(blockCount > 0 && blockSize > 0);

			DataChain* p = (DataChain*) new uchar[sizeof(DataChain) + blockCount * blockSize];
			p->prev = head;
			return head = p;
		}

		void close()
		{
			DataChain* p = this;
			while (p != NULL)
			{
				uchar* bytes = (uchar*)p;
				DataChain* prev = p->prev;
				delete[] bytes;
				p = prev;
			}
		}
	};

	/// 内存对象
	template<class ThreadModel>
	class MemObjT
	{
		struct Node
		{
			Node* next;	/// 仅仅在空闲内存中有效
		};

		ulong		mBlockSize;		/// 每个块的大小（即类对象的大小）
		ulong		mBlockCount;	/// 每次连续分配的块数（即对象数）
		DataChain*	mChainList;		/// 记录每次连续分配的块
		Node*		mFreeNode;		/// 当前空闲的块节点
		typename ThreadModel::mutex	mMutex;	/// 对应线程模型的互斥体
	public:
		MemObjT(ulong blockSize, ulong blockCount = 64)
		{
			assert(blockSize >= sizeof(Node));
			assert(blockCount >= 1);

			if (blockSize < sizeof(Node)) blockSize = sizeof(Node);
			if (blockCount <= 1) blockCount = 64;

			mBlockSize = blockSize;
			mBlockCount = blockCount;
			mFreeNode = 0;
			mChainList = 0;
		}

		~MemObjT()
		{
			freeAll();
		}

		ulong getBlockSize() const		{ return mBlockSize; }

	public:
		void* alloc()
		{
			ThreadModel::scopelock guard(mMutex);

			if (!mFreeNode)
			{
				DataChain* newBlock = DataChain::create(mChainList, mBlockCount, mBlockSize); // 分配新的数据链
				Node* node = (Node*)newBlock->data(); // 定位到数据区
				(uchar*&)node += (mBlockSize * mBlockCount) - mBlockSize; // 定位到最后一个节点

				// 各节点建立链接关系
				for (int i=mBlockCount-1; i>=0; i--, (uchar*&)node-=mBlockSize)
				{
					node->next = mFreeNode;
					mFreeNode = node;
				}
			}

			assert(mFreeNode);

			void* node = mFreeNode;
			mFreeNode = mFreeNode->next;
			return node;
		}

		void freeObj(void* ptr)
		{
			ThreadModel::scopelock guard(mMutex);

			if (ptr)
			{
				Node* node = (Node*)ptr;
				node->next = mFreeNode;
				mFreeNode = node;
			}
		}

		void freeAll()
		{
			ThreadModel::scopelock guard(mMutex);

			mChainList->close();
			mChainList = 0;
			mFreeNode = 0;
		}
	};

	typedef  MemObjT<SingleThread>	SingleThreadMemObj;
	typedef  MemObjT<MultiThread>	MultiThreadMemObj;

	// 导出
	template class RKT_EXPORT MemObjT<SingleThread>;
	template class RKT_EXPORT MemObjT<MultiThread>;


	/** DECLARE_MEMOBJ_SINGLETHREAD -- 非线程同步内存对象声明，在类声明中使用 */
#define DECLARE_MEMOBJ_SINGLETHREAD \
public:\
	void* operator new(size_t)						{ return sAlloc.alloc();}\
	void* operator new(size_t, void* p)				{ return p; }\
	void* operator new(size_t, const char*, int)	{ return sAlloc.alloc();}\
	void operator delete(void* p)					{ sAlloc.freeObj(p); }\
	void operator delete(void* p, void*)			{ sAlloc.freeObj(p); }\
	void operator delete(void* p, const char*, int)	{ sAlloc.freeObj(p);}\
protected:\
	static SingleThreadMemObj sAlloc;


	/** IMPLEMENT_MEMOBJ_SINGLETHREAD -- 非线程同步内存对象实现，在实现文件中使用 */
#define IMPLEMENT_MEMOBJ_SINGLETHREAD(classname, blockcount)	SingleThreadMemObj classname::sAlloc(sizeof(classname), blockcount);


	/** DECLARE_MEMOBJ -- 线程同步内存对象声明，在类声明中使用 */
#define DECLARE_MEMOBJ_MULTITHREAD \
public:\
	void* operator new(size_t)						{ return sAlloc.alloc();}\
	void* operator new(size_t, void* p)				{ return p; }\
	void* operator new(size_t, const char*, int)	{ return sAlloc.alloc(); }\
	void operator delete(void* p) 					{ sAlloc.freeObj(p); }\
	void operator delete(void* p, void*)			{ sAlloc.freeObj(p); }\
	void operator delete(void* p, const char*, int)	{ sAlloc.freeObj(p); }\
protected:\
	static MultiThreadMemObj sAlloc;


	/** IMPLEMENT_MEMOBJ -- 线程同步内存对象实现，在实现文件中使用 */
#define IMPLEMENT_MEMOBJ_MULTITHREAD(classname, blockcount)					MultiThreadMemObj classname::sAlloc(sizeof(classname), blockcount);

#define DECLARE_MEMOBJ		DECLARE_MEMOBJ_SINGLETHREAD
#define IMPLEMENT_MEMOBJ	IMPLEMENT_MEMOBJ_SINGLETHREAD

} // namespace

#pragma warning(pop)

#endif // #ifdef SUPPORT_MEMORY

#endif // RKT_MEMOBJ_H