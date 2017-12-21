#ifndef __VertexBuffer_H__
#define __VertexBuffer_H__

#include "IBufferManager.h"

namespace rkt
{
	struct IBufferManager;

	
	struct VBInfo
	{
		uint vertexSize;
		uint numVertices;
		BufferUsage usage;
		ResPool pool;
		VBInfo() : vertexSize(0),numVertices(0),usage(BU_STATIC),pool(POOL_SYSTEMMEM){}

		bool operator<(const VBInfo &vbi) const
		{
			if(vertexSize == vbi.vertexSize && numVertices == vbi.numVertices && usage == vbi.usage && pool == vbi.pool)return false;
			if(vertexSize < vbi.vertexSize)return true;
			else if(vertexSize == vbi.vertexSize)
			{
				if(numVertices < vbi.numVertices)return true;
				else if(numVertices == vbi.numVertices)
				{
					if(usage < vbi.usage)return true;
					else if(usage == vbi.usage)
					{
						if(pool < vbi.pool)return true;
					}
				}
			}
			return false;
		}
	};

	class VertexBuffer : public IVertexBuffer
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

		/** 克隆顶点缓冲区的数据，用户负责返回的顶点缓冲区对象的生命周期
		@return 克隆出来的顶点缓冲区
		*/
		virtual IVertexBuffer*	clone();
	virtual void invalidateDeviceObjects();

	protected:
		uint m_ui32NumVertices;
		uint m_ui32VertexSize;
	private:
		BufferUsage	m_Usage;
		bool	m_bLocked;
		IBufferManager*	m_pBufferManager;
		ResPool	m_pool;
		BufferUsage	m_usage;
	public:
		VertexBuffer(IBufferManager *pBufferManager,uint vertexSize,uint numVertices,BufferUsage usage,ResPool pool);
		~VertexBuffer();
	public:
		IDirect3DVertexBuffer9*	m_pVertexBuffer;
		VBInfo	m_vbi;

		//uint	m_frame;
#ifdef STATISTICS_BUFFER
		union BufferId
		{
			ulong id;
			struct 
			{
				ulong pool:3;
				ulong usage:5;
				ulong size:8;
				ulong num:16;
			};
		};
		struct BufferStatInfo
		{
			size_t create_count;
			size_t release_count;
			size_t peak_count; // create_count - release_count
			size_t lock_count;
			size_t unlock_count;
			float create_time;
			float release_time;
			float lock_time;
			float unlock_time;

		};
		typedef HashMap<ulong, BufferStatInfo>	BufferStatMap;
		static BufferStatMap sBufferStatMap;

		static void dump();
#endif

		public:
			void _release();
	};

}

#endif
