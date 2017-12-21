#include "StdAfx.h"
#include "VertexBuffer.h"
#include "BufferManager.h"

namespace rkt
{

#ifdef STATISTICS_BUFFER
	VertexBuffer::BufferStatMap VertexBuffer::sBufferStatMap;
#endif

	VertexBuffer::VertexBuffer(IBufferManager *pBufferManager,uint vertexSize,uint numVertices,BufferUsage usage,ResPool pool) 
		: m_ui32VertexSize(vertexSize),m_ui32NumVertices(numVertices),m_Usage(usage),m_bLocked(false), m_pVertexBuffer(0),m_pBufferManager(pBufferManager), m_pool(pool),m_usage(usage)
	{
		m_vbi.vertexSize = vertexSize;
		m_vbi.numVertices = numVertices;
		m_vbi.usage = usage;
		m_vbi.pool = pool;

		//PP_BY_NAME("VertexBuffer::VertexBuffer");

#ifdef STATISTICS_BUFFER
		TickCounter t;
#endif

		D3DPOOL d3dpool = D3DPOOL_SYSTEMMEM;
		switch(pool)
		{
		case POOL_SYSTEMMEM:d3dpool = D3DPOOL_SYSTEMMEM;break;
		case POOL_MANAGED:d3dpool = D3DPOOL_MANAGED;break;
		case POOL_DEFAULT:d3dpool = D3DPOOL_DEFAULT;break;
		}

		HRESULT hr = g_pDevice->CreateVertexBuffer(vertexSize * numVertices,BufferUtils::getD3DUsage(usage),0,d3dpool,&m_pVertexBuffer,0);
		if(d3dpool == D3DPOOL_DEFAULT&& BufferManager::bCheckDefaultVertexBuffer)
		{
			m_pBufferManager->AddDefaultPoolVertexBuffer(this);
		}
		if(!m_pVertexBuffer)
		{
			ErrorLn("CreateVertexBuffer failed:"<<my_t2utf8(DXGetErrorString(hr))<<" POOL:"<<(int)d3dpool<<" usage:"<<usage<<" vertexSize = "<<vertexSize<<"num = "<<numVertices);
			if(d3dpool != D3DPOOL_SYSTEMMEM)
			{
				ErrorLn("Try CreateVertexBuffer to system memory");
				hr = g_pDevice->CreateVertexBuffer(vertexSize * numVertices,BufferUtils::getD3DUsage(usage),0,D3DPOOL_SYSTEMMEM,&m_pVertexBuffer,0);
				m_pool = POOL_SYSTEMMEM;
				d3dpool = D3DPOOL_SYSTEMMEM;
			}
		}
		if(!m_pVertexBuffer)
		{
			ErrorLn("CreateVertexBuffer failed:"<<my_t2utf8(DXGetErrorString(hr))<<" POOL:"<<(int)d3dpool<<" usage:"<<usage<<" vertexSize = "<<vertexSize<<"num = "<<numVertices);
		}

#ifdef STATISTICS_BUFFER
		if (m_pVertexBuffer)
		{
			float tt = t.getTimeByDouble();
			BufferId id;
			id.size = m_ui32VertexSize;
			id.num = m_ui32NumVertices;
			id.usage = m_Usage;
			id.pool = m_pool;

			BufferStatMap::iterator it = sBufferStatMap.find(id.id);
			if (it == sBufferStatMap.end())
			{
				BufferStatInfo bsi;
				memset(&bsi, 0, sizeof(BufferStatInfo));
				it = sBufferStatMap.insert(BufferStatMap::value_type(id.id, bsi)).first;
			}
			
			BufferStatInfo& info = (*it).second;
			info.create_count++;
			info.create_time += tt;
			size_t delta = info.create_count - info.release_count;
			if (delta > info.peak_count) info.peak_count = delta;
		}
#endif
	}

	VertexBuffer::~VertexBuffer()
	{
		//PP_BY_NAME("VertexBuffer::~VertexBuffer");

#ifdef STATISTICS_BUFFER
		TickCounter t;
#endif

		if(m_pool == POOL_DEFAULT && BufferManager::bCheckDefaultVertexBuffer)
		{
			m_pBufferManager->RemoveDefaultPoolVertexBuffer(this);
		}
		SAFERELEASE(m_pVertexBuffer);

#ifdef STATISTICS_BUFFER
		{
			float tt = t.getTimeByDouble();
			BufferId id;
			id.size = m_ui32VertexSize;
			id.num = m_ui32NumVertices;
			id.usage = m_Usage;
			id.pool = m_pool;

			BufferStatMap::iterator it = sBufferStatMap.find(id.id);
			if (it != sBufferStatMap.end())
			{
				BufferStatInfo& info = (*it).second;
				info.release_count++;
				info.release_time += tt;
			}
		}
#endif
	}


	void* VertexBuffer::lock(uint offset,uint length,LockOptions options)
	{
		//PP_BY_NAME("VertexBuffer::lock");
		if(!m_pVertexBuffer)return 0;
		if(m_bLocked)
		{
			ErrorLn("locked vb locked again. offset = "<<offset<<" length = "<<length<<" options = "<<options<<"locked = "<<m_bLocked<<"pool = "<<m_pool<<"num = "<<m_ui32NumVertices<<"vsize = "<<m_ui32VertexSize);
			unlock();
		}

#ifdef STATISTICS_BUFFER
		TickCounter t;
#endif

		DWORD flags = BufferUtils::getD3DLockOptions(options);

		void *data = 0;
		//Info("numVertices = "<<m_ui32NumVertices<<endl);
		HRESULT hr = m_pVertexBuffer->Lock(offset,length,&data,flags);
		if (FAILED(hr))
		{
			ErrorLn("锁定顶点缓冲失败！！");
			return 0;
		}

		if(data)
		{
			m_bLocked = true;
		}
		else
		{
			ErrorLn("VertexBuffer::lock failed. offset = "<<offset<<" length = "<<length<<" options = "<<options<<"locked = "<<m_bLocked<<"pool = "<<m_pool<<"num = "<<m_ui32NumVertices<<"vsize = "<<m_ui32VertexSize);
		}

#ifdef STATISTICS_BUFFER
		{
			float tt = t.getTimeByDouble();
			BufferId id;
			id.size = m_ui32VertexSize;
			id.num = m_ui32NumVertices;
			id.usage = m_Usage;
			id.pool = m_pool;

			BufferStatMap::iterator it = sBufferStatMap.find(id.id);
			if (it != sBufferStatMap.end())
			{
				BufferStatInfo& info = (*it).second;
				info.lock_count++;
				info.lock_time += tt;
			}
		}
#endif

		return data;
	}

	void VertexBuffer::unlock()
	{
		if(!m_bLocked)return;
		if(!m_pVertexBuffer)return;

#ifdef STATISTICS_BUFFER
		TickCounter t;
#endif

		m_pVertexBuffer->Unlock();
		m_bLocked = false;

#ifdef STATISTICS_BUFFER
		{
			float tt = t.getTimeByDouble();
			BufferId id;
			id.size = m_ui32VertexSize;
			id.num = m_ui32NumVertices;
			id.usage = m_Usage;
			id.pool = m_pool;

			BufferStatMap::iterator it = sBufferStatMap.find(id.id);
			if (it != sBufferStatMap.end())
			{
				BufferStatInfo& info = (*it).second;
				info.unlock_count++;
				info.unlock_time += tt;
			}
		}
#endif
	}

	bool VertexBuffer::isLocked() { return m_bLocked; }
	uint VertexBuffer::getVertexSize() const { return m_ui32VertexSize; }
	uint VertexBuffer::getNumVertices() const { return m_ui32NumVertices; }
	BufferUsage	VertexBuffer::getUsage() const { return m_Usage; }

	void VertexBuffer::release()
	{
#ifdef CACHE_BUFFER
		BufferManagerCached::Instance()->destroyVertexBuffer(this);
#else
		delete this;
#endif
	}

	
	void VertexBuffer::_release()
	{
		delete this;
	}

	void VertexBuffer::invalidateDeviceObjects()
	{
#ifdef CACHE_BUFFER
		BufferManagerCached::Instance()->invalidateDeviceObjects(this);
#else
		delete this;
#endif
	}

	void	VertexBuffer::readData(uint offset,uint length,void* pDest)
	{
		void *pData = lock(offset,length,BL_READ_ONLY);
		if(pData)
		{
			memcpy(pDest,pData,length);
			unlock();
		}
	}

	void	VertexBuffer::writeData(uint offset,uint length,const void* pSource,bool discardWholeBuffer)
	{
		void *pData = lock(offset,length,BL_DISCARD);
		if(pData)
		{
			memcpy(pData,pSource,length);
			unlock();
		}
	}

	IVertexBuffer*	VertexBuffer::clone()
	{
		IVertexBuffer *pVertexBuffer = m_pBufferManager->createVertexBuffer(m_ui32VertexSize,m_ui32NumVertices,m_Usage);
		if(pVertexBuffer)
		{
			void  *pDest = pVertexBuffer->lock(0,0,BL_DISCARD);
			void  *pSrc = 0;
			if(m_Usage & BU_WRITE_ONLY)
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

#ifdef STATISTICS_BUFFER
	void VertexBuffer::dump()
	{
		if (sBufferStatMap.empty())
			return;

		ostrbuf osb;
		osb<<"ID,POOL,USAGE,SIZE,NUM,Create Times,Free Times,Peak Times,Create Ticks,Free Ticks,Lock Times,Unlock Times,Lock Ticks,Unlock Ticks,Avg Create Ticks,Avg Free Ticks,Avg Lock Ticks, Avg Unlock Ticks"<<endl;

		for (BufferStatMap::const_iterator it=sBufferStatMap.begin(); it!=sBufferStatMap.end(); ++it)
		{
			const BufferId& id = (BufferId&)(*it).first;
			const BufferStatInfo& info = (*it).second;

			osb<<id.id<<","
				<<(int)id.pool<<","
				<<(int)id.usage<<","
				<<(int)id.size<<","
				<<(int)id.num<<","
				<<info.create_count<<","
				<<info.release_count<<","
				<<info.peak_count<<","
				<<info.create_time<<","
				<<info.release_time<<","
				<<info.lock_count<<","
				<<info.unlock_count<<","
				<<info.lock_time<<","
				<<info.unlock_time<<","
				<<float(info.create_count ? (info.create_time/info.create_count) : 0.f)<<","
				<<float(info.release_count ? (info.release_time/info.release_count) : 0.f)<<","
				<<float(info.lock_count ? (info.lock_time/info.lock_count) : 0.f)<<","
				<<float(info.unlock_count ? (info.unlock_time/info.unlock_count) : 0.f)<<endl;
		}

		std::string path = getWorkDir();
		path += "\\StatVB.csv";
		FileStream fs(path.c_str());
		if (fs.open(Stream::modeWrite|Stream::shareRead|Stream::modeCreate))
		{
			fs.write(osb.c_str(), osb.length());
			fs.close();
		}
	}
#endif
}


