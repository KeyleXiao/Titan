#ifndef __BufferManager_H__
#define __BufferManager_H__

#include <map>
#include <hash_map>
#include "VertexBuffer.h"

namespace rkt
{

	class BufferUtils
	{
	public:
		static DWORD getD3DUsage(uint usage);
		static DWORD getD3DLockOptions(LockOptions lo);
	};

	/// 普通缓冲管理器
	class BufferManager : public IBufferManager
	{
	public:
		virtual IVertexBuffer*	createVertexBuffer(uint vertexSize,uint numVertices,BufferUsage usage,ResPool pool = POOL_MANAGED);
		virtual IIndexBuffer*	createIndexBuffer(IndexType itype,uint numIndices,BufferUsage usage,ResPool pool = POOL_MANAGED);
		virtual IVertexBuffer*	createVertexBufferUP(uint vertexSize,uint numVertices,BufferUsage usage,ResPool pool = POOL_MANAGED);
		virtual IIndexBuffer*	createIndexBufferUP(IndexType itype,uint numIndices,BufferUsage usage,ResPool pool = POOL_MANAGED);
		virtual void injectFrame();
		virtual void dump();
		virtual void intialize(){}
		virtual void finalize(){}

		/** 检查DEFAULT的顶点缓冲是否都释放掉了，因为微端在下载地图资源时，
		改变分辨率，有部分VertexBuffer是没有释放掉的，没有查出是哪里创
		建了这个VertexBuffer，先用这个方法解决。这个函数一定要在
		RenderEngine::invalidateDeviceObjects()中放在最后调用，因为只是做
		一个检查而已，切记！！
		*/
		virtual void CheckDefaultPoolVertexBufferReleased();

		/** 增加DEFAULT的顶点缓冲,此函数目前只和检查DEFAULT的顶点缓冲是否都释放掉有关，不做其他功能
		@param pVb 顶点缓冲
		*/
		virtual void AddDefaultPoolVertexBuffer(IVertexBuffer *pVb);
		/** 移除DEFAULT的顶点缓冲,此函数目前只和检查DEFAULT的顶点缓冲是否都释放掉有关，不做其他功能
		@param pVb 顶点缓冲
		*/
		virtual void RemoveDefaultPoolVertexBuffer(IVertexBuffer *pVb);

	protected:
		BufferManager();
		std::map<DWORD,IVertexBuffer*> m_DefaultVbMap;
	public:
		static bool bCheckDefaultVertexBuffer;
		static BufferManager*	Instance()
		{
			static BufferManager bm;
			return &bm;
		}

		uint	m_frame;
	};

	struct VB
	{
		IVertexBuffer *vb;
		uint lastUsedTime;
		bool nowUsing;

		VB() : vb(0),lastUsedTime(0),nowUsing(false){}
	};

	typedef std::list<VB> VBList;

	typedef std::map<VBInfo,VBList*> VBMap;

	/// 可缓存的缓冲管理器
	class BufferManagerCached : public IBufferManager
	{
	public:
		virtual IVertexBuffer*	createVertexBuffer(uint vertexSize,uint numVertices,BufferUsage usage,ResPool pool = POOL_MANAGED);
		virtual IIndexBuffer*	createIndexBuffer(IndexType itype,uint numIndices,BufferUsage usage,ResPool pool = POOL_MANAGED);
		virtual IVertexBuffer*	createVertexBufferUP(uint vertexSize,uint numVertices,BufferUsage usage,ResPool pool = POOL_MANAGED);
		virtual IIndexBuffer*	createIndexBufferUP(IndexType itype,uint numIndices,BufferUsage usage,ResPool pool = POOL_MANAGED);
		virtual void injectFrame();
		virtual void dump();
		virtual void intialize();
		virtual void finalize();

	public:
		void invalidateDeviceObjects(IVertexBuffer *pVB);
		void invalidateDeviceObjects();

		/** 检查DEFAULT的顶点缓冲是否都释放掉了，因为微端在下载地图资源时，
		改变分辨率，有部分VertexBuffer是没有释放掉的，没有查出是哪里创
		建了这个VertexBuffer，先用这个方法解决。这个函数一定要在
		RenderEngine::invalidateDeviceObjects()中放在最后调用，因为只是做
		一个检查而已，切记！！BufferManagerCached暂时不需要，所以给一个空
		*/
		virtual void CheckDefaultPoolVertexBufferReleased(){};

		/** 增加DEFAULT的顶点缓冲,BufferManagerCached暂时不需要,此函数目前只和检查DEFAULT的顶点缓冲是否都释放掉有关，不做其他功能，所以给一个空
		@param pVb 顶点缓冲
		*/
		virtual void AddDefaultPoolVertexBuffer(IVertexBuffer *pVb){};
		/** 移除DEFAULT的顶点缓冲,BufferManagerCached暂时不需要,此函数目前只和检查DEFAULT的顶点缓冲是否都释放掉有关，不做其他功能，所以给一个空
		@param pVb 顶点缓冲
		*/
		virtual void RemoveDefaultPoolVertexBuffer(IVertexBuffer *pVb){};

	protected:
		BufferManagerCached() : m_frame(0),m_lastCheckTime(0){}
	public:
		static BufferManagerCached*	Instance()
		{
			static BufferManagerCached cbm;
			return &cbm;
		}

		uint	m_frame;

	private:
		void destroyVertexBuffer(IVertexBuffer* vb);
		friend class VertexBuffer;

		VBMap	m_vbmRecycled;

		uint	m_lastCheckTime;
	};

}
#endif