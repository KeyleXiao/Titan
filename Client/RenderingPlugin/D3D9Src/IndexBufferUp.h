#ifndef __IndexBufferUp_H__
#define __IndexBufferUp_H__

#include "IBufferManager.h"

namespace rkt
{
	class IndexBufferUp : public IIndexBuffer
	{
	public:
		/** 获得索引缓冲区类型，参考IndexType枚举类型
		@see IndexType
		@return 缓冲区类型
		*/
		virtual IndexType	getType(void ) const;							

		/** 获取索引的数量
		@return 索引的数量
		*/
		virtual uint		getNumIndices(void ) const;						

		/** 获得索引的长度(字节)
		@return 每个索引的长度，一般为2,4
		*/
		virtual uint		getIndexSize(void ) const;						

		/** 获得索引缓冲区的使用方式，参考BufferUsage
		@see BufferUsage
		@return 索引缓冲区的使用方式
		*/
		virtual BufferUsage	getUsage() const;

		/** 加锁缓冲区，从来可以直接存取数据
		@param offset 从数据开始的偏移（字节）
		@param length 要加锁的数据区的长度
		@param options 加锁的方式
		@see LockOptions
		@return 直接数据的地址
		*/
		virtual void *		lock(uint offset,uint length,LockOptions options);						

		/** 解锁缓冲区，和lock配套使用
		*/
		virtual void 		unlock();		

		/** 获取索引缓冲区是否处于加锁状态
		@return true表示处于加锁状态，false则未加锁
		*/
		virtual bool		isLocked();		

		/** 释放缓冲区对象
		*/
		virtual void 		release();		

		/** 从缓冲区读取数据，放到指定的缓存
		@param offset 从数据开始的偏移值
		@param length 要读取的数据长度
		@param pDest 目标缓冲区地址
		*/
		virtual void 	readData(uint offset,uint length,void * pDest);	

		/** 把数据写入缓冲区
		@param offset 从数据开始的偏移值
		@param length 要写入的的数据长度
		@param pSource 要写入的数据地址
		@param discardWholeBuffer 是否要抛弃整个buffer的数据(会提高速度)
		*/
		virtual void 	writeData(																			
			uint offset,																	
			uint length,																	
			const void * pSource,															
			bool discardWholeBuffer = false);

		virtual void* GetBufferPointer();

	protected:
		IndexType m_IndexType;
		uint m_ui32NumIndexes;
		uint m_ui32IndexSize;
	private:
		BufferUsage	m_Usage;
		bool	m_bLocked;
		ResPool	m_pool;
	public:
		IndexBufferUp(IndexType idxType, uint numIndexes,BufferUsage usage,ResPool pool);
		~IndexBufferUp();
	public:
		void*	m_pIndexBuffer;
	};

}
#endif