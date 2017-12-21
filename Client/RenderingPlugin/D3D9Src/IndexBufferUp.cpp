//==========================================================================
/**
* @file	  : IndexBufferUp.cpp
* @author : Java Zhang
* created : 2012-1-30
* 内存形式索引缓冲区
*/
//==========================================================================


#include "StdAfx.h"
#include "IndexBufferUp.h"
#include "BufferManager.h"

namespace rkt
{
	IndexBufferUp::IndexBufferUp(IndexType idxType, uint numIndexes,BufferUsage usage,ResPool pool) : m_Usage(usage),m_IndexType(idxType),m_ui32NumIndexes(numIndexes),m_bLocked(false),m_pIndexBuffer(0)
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

		HRESULT hr=S_OK;
		m_pIndexBuffer=new BYTE[ui32SizeInBytes];
		//m_pIndexBuffer=malloc(ui32SizeInBytes);
		if(!m_pIndexBuffer)
		{
			ErrorLn("CreateIndexBufferUp failed:"<<" POOL:"<<(int)d3dpool<<" Usage:"<<usage);
		}
	}

	IndexBufferUp::~IndexBufferUp()
	{
		safeDeleteArray(m_pIndexBuffer);
		//free(m_pIndexBuffer);
	}

	void* IndexBufferUp::lock(uint offset,uint length,LockOptions options)
	{
		if(!m_pIndexBuffer)return 0;

		m_bLocked = true;
		return (BYTE*)m_pIndexBuffer+offset;
	}

	void IndexBufferUp::unlock()
	{
		m_bLocked = false;
	}

	bool	IndexBufferUp::isLocked() { return m_bLocked; }
	IndexType IndexBufferUp::getType(void) const { return m_IndexType; }
	uint IndexBufferUp::getNumIndices(void) const { return m_ui32NumIndexes; }
	uint IndexBufferUp::getIndexSize(void) const { return m_ui32IndexSize; }
	BufferUsage	IndexBufferUp::getUsage() const { return m_Usage; }

	void IndexBufferUp::release()
	{
		delete this;
	}

	//---------------------------------------------------------------------
	void IndexBufferUp::readData(uint offset,uint length,void* pDest)
	{
		void *pData = lock(offset,length,BL_READ_ONLY);
		if(pData)
		{
			memcpy(pDest,pData,length);
			unlock();
		}
	}
	//---------------------------------------------------------------------
	void IndexBufferUp::writeData(uint offset,uint length,const void* pSource,bool discardWholeBuffer)
	{
		void *pData = lock(offset,length,BL_DISCARD);
		if(pData)
		{
			memcpy(pData,pSource,length);
			unlock();
		}
	}

	void* IndexBufferUp::GetBufferPointer()
	{
		return m_pIndexBuffer;
	}
}