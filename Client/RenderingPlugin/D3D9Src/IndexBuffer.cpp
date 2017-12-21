#include "StdAfx.h"
#include "IndexBuffer.h"
#include "BufferManager.h"

namespace rkt
{
	IndexBuffer::IndexBuffer(IndexType idxType, uint numIndexes,BufferUsage usage,ResPool pool) : m_Usage(usage),m_IndexType(idxType),m_ui32NumIndexes(numIndexes),m_bLocked(false),m_pIndexBuffer(0)
	{
		switch (idxType)
		{
		case IT_16BIT:
			m_ui32IndexSize = sizeof(unsigned short);
			break;
		case IT_32BIT:
			m_ui32IndexSize = sizeof(unsigned int);
			break;
		}

		D3DPOOL d3dpool = D3DPOOL_SYSTEMMEM;
		switch(pool)
		{
		case POOL_SYSTEMMEM:d3dpool = D3DPOOL_SYSTEMMEM;break;
		case POOL_MANAGED:d3dpool = D3DPOOL_MANAGED;break;
		case POOL_DEFAULT:d3dpool = D3DPOOL_DEFAULT;break;
		}

		m_pool = pool;

		uint ui32SizeInBytes = m_ui32IndexSize * m_ui32NumIndexes;

		HRESULT hr = g_pDevice->CreateIndexBuffer(ui32SizeInBytes,BufferUtils::getD3DUsage(usage),idxType == IT_16BIT ? D3DFMT_INDEX16 : D3DFMT_INDEX32,d3dpool,&m_pIndexBuffer,0);
		if(!m_pIndexBuffer)
		{
			ErrorLn("CreateIndexBuffer failed:"<<my_t2utf8(DXGetErrorString(hr))<<" POOL:"<<(int)d3dpool<<" Usage:"<<usage);
			if(d3dpool != D3DPOOL_SYSTEMMEM)
			{
				ErrorLn("Try CreateIndexBuffer to system memory");
				m_pool = POOL_SYSTEMMEM;
				d3dpool = D3DPOOL_SYSTEMMEM;
				hr = g_pDevice->CreateIndexBuffer(ui32SizeInBytes,BufferUtils::getD3DUsage(usage),idxType == IT_16BIT ? D3DFMT_INDEX16 : D3DFMT_INDEX32,D3DPOOL_SYSTEMMEM,&m_pIndexBuffer,0);
			}
		}
		if(!m_pIndexBuffer)
		{
			ErrorLn("CreateIndexBuffer failed:"<<my_t2utf8(DXGetErrorString(hr))<<" POOL:"<<(int)d3dpool<<" Usage:"<<usage);
		}
	}

	IndexBuffer::~IndexBuffer()
	{
		SAFERELEASE(m_pIndexBuffer);
	}

	void* IndexBuffer::lock(uint offset,uint length,LockOptions options)
	{
		if(!m_pIndexBuffer)return 0;
		DWORD flags = 0;

		void *data = 0;
		HRESULT hr = m_pIndexBuffer->Lock(offset,length,&data,BufferUtils::getD3DLockOptions(options));
		if (FAILED(hr))
		{
			ErrorLn("锁定索引缓冲失败！");
			return 0;
		}
		if(data)m_bLocked = true;
		return data;
	}

	void IndexBuffer::unlock()
	{
		if(!m_pIndexBuffer)return;
		m_pIndexBuffer->Unlock();
		m_bLocked = false;
	}

	bool	IndexBuffer::isLocked() { return m_bLocked; }
	IndexType IndexBuffer::getType(void) const { return m_IndexType; }
	uint IndexBuffer::getNumIndices(void) const { return m_ui32NumIndexes; }
	uint IndexBuffer::getIndexSize(void) const { return m_ui32IndexSize; }
	BufferUsage	IndexBuffer::getUsage() const { return m_Usage; }

	void IndexBuffer::release(){delete this;}

	//---------------------------------------------------------------------
	void IndexBuffer::readData(uint offset,uint length,void* pDest)
	{
		void *pData = lock(offset,length,BL_READ_ONLY);
		if(pData)
		{
			memcpy(pDest,pData,length);
			unlock();
		}
	}
	//---------------------------------------------------------------------
	void IndexBuffer::writeData(uint offset,uint length,const void* pSource,bool discardWholeBuffer)
	{
		void *pData = lock(offset,length,BL_DISCARD);
		if(pData)
		{
			memcpy(pData,pSource,length);
			unlock();
		}
	}
}