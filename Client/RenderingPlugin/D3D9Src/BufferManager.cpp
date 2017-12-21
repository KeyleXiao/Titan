#include "StdAfx.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferUp.h"
#include "IndexBufferUp.h"
#include "BufferManager.h"

namespace rkt
{
	bool  BufferManager::bCheckDefaultVertexBuffer = false;
	//---------------------------------------------------------------------
	DWORD BufferUtils::getD3DUsage(uint usage)
	{
		switch(usage)
		{
		case BU_STATIC:
			return 0;
        case BU_WRITE_ONLY:
            return D3DUSAGE_WRITEONLY;
		case BU_STATIC_WRITE_ONLY:
			return D3DUSAGE_WRITEONLY;
		case BU_DYNAMIC:
			return D3DUSAGE_DYNAMIC;
		case BU_DYNAMIC_WRITE_ONLY:
			return D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
		case BU_DYNAMIC_WRITE_ONLY_DISCARDABLE:
			return D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
		default:
			return D3DUSAGE_DYNAMIC;
		};
	}
	//---------------------------------------------------------------------
	DWORD BufferUtils::getD3DLockOptions(LockOptions lo)
	{
		DWORD flags = D3DLOCK_DISCARD;
		switch(lo)
		{
		case BL_NORMAL:
			flags = 0;
			break;
		case BL_DISCARD:
			break;
		case BL_READ_ONLY:
			flags = D3DLOCK_READONLY;
			break;
		case BL_NO_OVERWRITE:
			flags = D3DLOCK_NOOVERWRITE;
			break;
		}

		return flags;
	}	
	
	
	IVertexBuffer*	BufferManager::createVertexBuffer(uint vertexSize,uint numVertices,BufferUsage usage,ResPool pool)
	{
		IVertexBuffer *pVertexBuffer = new VertexBuffer(this,vertexSize,numVertices,usage,pool);

		return pVertexBuffer;
	}

	IIndexBuffer*	BufferManager::createIndexBuffer(IndexType itype,uint numIndexes,BufferUsage usage,ResPool pool)
	{
		IIndexBuffer *pIndexBuffer = new IndexBuffer(itype,numIndexes,usage,pool);

		return pIndexBuffer;
	}

	IVertexBuffer*	BufferManager::createVertexBufferUP(uint vertexSize,uint numVertices,BufferUsage usage,ResPool pool)
	{
		IVertexBuffer *pVertexBuffer = new VertexBufferUp(this,vertexSize,numVertices,usage,pool);

		return pVertexBuffer;
	}

	IIndexBuffer*	BufferManager::createIndexBufferUP(IndexType itype,uint numIndexes,BufferUsage usage,ResPool pool)
	{
		IIndexBuffer *pIndexBuffer = new IndexBufferUp(itype,numIndexes,usage,pool);

		return pIndexBuffer;
	}

	void BufferManager::injectFrame()
	{
		m_frame++;
	}

	void BufferManager::dump()
	{
#ifdef STATISTICS_BUFFER
		VertexBuffer::dump();
#endif
	}
	void BufferManager::CheckDefaultPoolVertexBufferReleased()
	{
		
		if(!bCheckDefaultVertexBuffer)
			return;
		if(!m_DefaultVbMap.empty())
		{
			std::vector<IVertexBuffer*> TempVector;
			std::map<DWORD,IVertexBuffer*>::iterator ite = m_DefaultVbMap.begin();
			//这里根据Map里面的数量来初始化数组大小，因为vector内部实现是数组，动态增长数组会导致性能损耗，
			//而用list的也不行，因为调用IVertexBuffer::release会删除自身，导致迭代器破坏，只能是用vector用下标访问
			TempVector.resize(m_DefaultVbMap.size());
			int index = 0;//vector索引
			while(ite != m_DefaultVbMap.end())
			{
				TempVector[index] = ite->second;
				ite++;
				index++;
			}
			m_DefaultVbMap.clear();

			for (int i = 0;i < TempVector.size();i++)
			{
				safeRelease(TempVector[i]);
			}
			TempVector.clear();
		}
	}

	void BufferManager::AddDefaultPoolVertexBuffer( IVertexBuffer *pVb )
	{
		if(NULL != pVb)
		{
			//m_DefaultVbMap.insert(pair<DWORD,IVertexBuffer*>((DWORD)pVb  ,pVb));
			m_DefaultVbMap[(DWORD)pVb]=pVb;
		}
	}

	void BufferManager::RemoveDefaultPoolVertexBuffer( IVertexBuffer *pVb )
	{
		if(NULL != pVb)
		{
			m_DefaultVbMap.erase((DWORD)pVb);
		}
	}

	BufferManager::BufferManager() : m_frame(0)
	{
		//Ini conf("config.ini");
		bCheckDefaultVertexBuffer = false;//conf.getInt("App","CheckDefaultVertexBuffer",0) == 0 ?false:true;
	}
}