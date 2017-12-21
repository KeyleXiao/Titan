#ifndef __VertexBufferUp_H__
#define __VertexBufferUp_H__

#include "IBufferManager.h"

namespace rkt
{
	struct IBufferManager;



	class VertexBufferUp : public IVertexBuffer
	{
	public:
		/** 释放定点缓冲区对象
		*/
		virtual void 	release();

		/** 获得顶点大小，每个顶点占用的字节数
		@return 顶点大小
		*/
		virtual uint	getVertexSize() const;

		/** 获得顶点的数量
		@return 顶点数量
		*/
		virtual uint	getNumVertices() const;

		/** Buffer的使用方式
		@return Buffer的使用方式
		@see BufferUsage
		*/
		virtual BufferUsage	getUsage() const;

		/** 加锁缓冲区，获得数据的指针，从而可以操作数据
		@param offset 从数据开始的偏移值
		@param length 加锁的区域长度
		@param options 加锁的方式
		@see LockOptions
		@return 数据区指针
		*/
		virtual void *	lock(uint offset,uint length,LockOptions options);

		/** 解锁缓冲区，和lock配套使用
		*/
		virtual void 	unlock();

		/** 获取缓冲区是否处于被加锁状态
		@return true缓冲区被加锁，false未锁定
		*/
		virtual bool	isLocked();

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

		/** 克隆顶点缓冲区的数据，用户负责返回的顶点缓冲区对象的生命周期
		@return 克隆出来的顶点缓冲区
		*/
		virtual IVertexBuffer*	clone();
	virtual void invalidateDeviceObjects();

	protected:
		uint m_ui32NumVertices;
		uint m_ui32VertexSize;
	private:
		bool	m_bLocked;
		IBufferManager*	m_pBufferManager;
		ResPool	m_pool;
		BufferUsage	m_usage;
	public:
		VertexBufferUp(IBufferManager *pBufferManager,uint vertexSize,uint numVertices,BufferUsage usage,ResPool pool);
		~VertexBufferUp();
	public:
		void*	m_pVertexBuffer;

		public:
			void _release();
	};

}

#endif
