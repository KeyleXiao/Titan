//==========================================================================
/**
* @file	  : VertexBufferUp.cpp
* @author : Java Zhang
* created : 2012-1-30
* 内存形式顶点缓冲区
*/
//==========================================================================

#include "StdAfx.h"
#include "VertexBufferUp.h"
#include "BufferManager.h"

namespace rkt
{


	VertexBufferUp::VertexBufferUp(IBufferManager *pBufferManager,uint vertexSize,uint numVertices,BufferUsage usage,ResPool pool) 
		: m_ui32VertexSize(vertexSize),m_ui32NumVertices(numVertices),m_bLocked(false), m_pVertexBuffer(0),m_pBufferManager(pBufferManager), m_pool(pool),m_usage(usage)
	{

		//PP_BY_NAME("VertexBufferUp::VertexBufferUp");

		D3DPOOL d3dpool = D3DPOOL_SYSTEMMEM;
		switch(pool)
		{
		case POOL_SYSTEMMEM:d3dpool = D3DPOOL_SYSTEMMEM;break;
		case POOL_MANAGED:d3dpool = D3DPOOL_MANAGED;break;
		case POOL_DEFAULT:d3dpool = D3DPOOL_DEFAULT;break;
		}

		HRESULT hr=S_OK;
		//HRESULT hr = g_pDevice->CreateVertexBuffer(vertexSize * numVertices,BufferUtils::getD3DUsage(usage),0,d3dpool,&m_pVertexBuffer,0);
		m_pVertexBuffer=new BYTE[vertexSize * numVertices];
		//m_pVertexBuffer=malloc(vertexSize * numVertices);

		if(!m_pVertexBuffer)
		{
			ErrorLn("CreateVertexBufferUp failed:"<<" vertexSize = "<<vertexSize<<"num = "<<numVertices);
		}
	}

	VertexBufferUp::~VertexBufferUp()
	{
		//PP_BY_NAME("VertexBufferUp::~VertexBufferUp");
		safeDeleteArray(m_pVertexBuffer);
	}


	void* VertexBufferUp::lock(uint offset,uint length,LockOptions options)
	{
		//PP_BY_NAME("VertexBufferUp::lock");
		if(!m_pVertexBuffer)return 0;

		m_bLocked = true;
		return (BYTE*)m_pVertexBuffer+offset;
	}

	void VertexBufferUp::unlock()
	{
		m_bLocked = false;
	}

	bool VertexBufferUp::isLocked() { return m_bLocked; }
	uint VertexBufferUp::getVertexSize() const { return m_ui32VertexSize; }
	uint VertexBufferUp::getNumVertices() const { return m_ui32NumVertices; }
	BufferUsage	VertexBufferUp::getUsage() const { return m_usage; }

	void VertexBufferUp::release()
	{
		delete this;
	}
	
	void VertexBufferUp::_release()
	{
		delete this;
	}

	void VertexBufferUp::invalidateDeviceObjects()
	{
		delete this;
	}

	void	VertexBufferUp::readData(uint offset,uint length,void* pDest)
	{
		void *pData = lock(offset,length,BL_READ_ONLY);
		if(pData)
		{
			memcpy(pDest,pData,length);
			unlock();
		}
	}

	void	VertexBufferUp::writeData(uint offset,uint length,const void* pSource,bool discardWholeBuffer)
	{
		void *pData = lock(offset,length,BL_DISCARD);
		if(pData)
		{
			memcpy(pData,pSource,length);
			unlock();
		}
	}

	void* VertexBufferUp::GetBufferPointer()
	{
		return m_pVertexBuffer;
	}

	IVertexBuffer*	VertexBufferUp::clone()
	{
		IVertexBuffer *pVertexBuffer = m_pBufferManager->createVertexBufferUP(m_ui32VertexSize,m_ui32NumVertices,m_usage);
		if(pVertexBuffer)
		{
			void  *pDest = pVertexBuffer->lock(0,0,BL_DISCARD);
			void  *pSrc = 0;
			if(m_usage & BU_WRITE_ONLY)
				pSrc = this->lock(0,0,BL_NORMAL);
			else
				pSrc = this->lock(0,0,BL_READ_ONLY);

			if(pSrc && pDest)
			{
				memcpy(pDest,pSrc,m_ui32VertexSize * m_ui32NumVertices);
			}
			if(pDest)pVertexBuffer->unlock();
			if(pSrc)this->unlock();
		}

		return pVertexBuffer;
	}

}


