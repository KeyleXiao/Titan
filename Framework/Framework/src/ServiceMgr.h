/*******************************************************************
** 文件名:	ServiceMgr.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/10/2014
** 版  本:	1.0
** 描  述:	服务管理器(实现)

			
********************************************************************/

#pragma once

#include <map>
#include <string>

#include "IServiceMgr.h"
#include "WorkThreadScheduler.h"
#include "Simple_Atom_Lock.h"
#include "fast_id_map.h"
#include "Singleton.h"
#include "CloudService.h"

using namespace std;
using namespace rkt;

#define MAX_SERVICE_COUNT  10240

/**
	服务管理器:
	1.根据名字查找服务（Service)信息
	2.通过根服务器同步各节点的服务（Service)注册信息
	3.服务器管理本身也可以是个服务
*/

class ServiceMgr : public IServiceMgr,public Singleton<ServiceMgr>
{
protected:
	//typedef fast_id_map<unsigned long,SERVICE_PTR>      SERVICE_MAP;
	//typedef std::map<string,SERVICE_ID>					NAMING_MAP;

	//SERVICE_MAP                m_service_map;
	//NAMING_MAP                 m_naming_map;
	//volatile	LONG           m_mutex;

	SERVICE_PTR      m_serviesList[MAX_SERVICE_COUNT];            // 因为share_ptr并不是线程安全的，所以这里实际并不能保证多线程安全，未来还要重构...
	volatile LONG    m_nAllocIndex;

public:
	//ServiceMgr() : m_mutex(0),m_service_map(SERVICE_PTR(0)){}

	ServiceMgr() : m_nAllocIndex(0){}

	/** 
	取得服务(Service)对象指针
	@desc     : 调用这个函数是线程安全的，不论在哪个服务都可以调用
	@param id : 服务ID
	*/
	virtual SERVICE_PTR get_service( SERVICE_ID id )
	{
		//Simple_Atom_Lock lock(&m_mutex);

		//return m_service_map.getData(id.index);
		if ( id.cloud!=CloudService::getInstance().get_id() )
		{
			return SERVICE_PTR(0);
		}

		if ( id.index<0 || id.index>=MAX_SERVICE_COUNT )
		{
			return SERVICE_PTR(0);
		}

		return m_serviesList[id.index];
	}

	/**
	根据名字获取服务指针
	@param name : 服务名称
	*/
	virtual SERVICE_PTR find_service( const char * name )
	{
		for ( int i=0;i<MAX_SERVICE_COUNT;++i )
		{
			if ( m_serviesList[i]!= 0 && stricmp( m_serviesList[i]->get_scheme().scheme->name,name)==0 )
			{
				return m_serviesList[i];
			}
		}

		return SERVICE_PTR(0);
		//Simple_Atom_Lock lock(&m_mutex);

		//NAMING_MAP::iterator it = m_naming_map.find(name);
		//if ( it==m_naming_map.end() )
		//	return SERVICE_PTR(0);

		//return m_service_map.getData(it->second.index);
	}

	// 分配一个新的服务ID
	SERVICE_ID allot_service_id()
	{
		SERVICE_ID id = INVALID_SERVICE_ID;
		id.cloud = CloudService::getInstance().get_id();

		for ( int i=0;i<MAX_SERVICE_COUNT;++i )
		{
			id.index = InterlockedIncrement(&m_nAllocIndex) % MAX_SERVICE_COUNT;
			if ( m_serviesList[id.index]==0 )
			{
				return id;
			}
		}

		return INVALID_SERVICE_ID;
	}

	/** 
	注册服务(Service)
	@param id  : 服务ID,如果服务ID为INVALID_SERVICE_ID,则会自动生成一个唯一ID
	*/
	virtual bool register_service( SERVICE_PTR  pService,SERVICE_ID id=INVALID_SERVICE_ID )
	{
		//Simple_Atom_Lock lock(&m_mutex);

		if ( !WorkThreadScheduler::getInstance().schedule(pService,pService->get_scheme().thread_id) )
			return false;

		if ( INVALID_SERVICE_ID==id )
		{
			//id.cloud = CloudService::getInstance().get_id();
			//id.index = m_service_map.allotID();
			//id.index = allot_service_id();
            id = allot_service_id();
		}

		pService->set_id(id);

		m_serviesList[id.index] = pService;
		//m_service_map.setData(id.index,pService);
		//m_naming_map[pService->get_scheme().scheme->name] = id;
		return true;
	}

	/** 
	注销服务(Service)
	@desc      : 注销服务时会将之前的ID收回
	*/
	virtual bool unregister_service( SERVICE_PTR  pService )
	{
		//Simple_Atom_Lock lock(&m_mutex);

		//m_naming_map.erase(pService->get_scheme().scheme->name);
		SERVICE_ID id = pService->get_id();
		//m_service_map.revertID(id.index);
		
		if ( get_service(id)==pService )
		{
			m_serviesList[id.index] = SERVICE_PTR(0);
		}else
		{
			ErrorLn("unregister_service failed! can not found service " << pService->get_scheme().scheme->name );
		}

		return WorkThreadScheduler::getInstance().unschedule(pService);
	}

	// 停止所有服务的运行
	virtual void stop_all()
	{
		//Simple_Atom_Lock lock(&m_mutex);
		//SERVICE_PTR services[4096];
		//size_t count = m_service_map.getAllDatas(services, _countof(services));
		//for (size_t i = 0; i < count; ++i)
		//{
		//	services[i]->stop();
		//}
		for ( int i=0;i<MAX_SERVICE_COUNT;++i )
		{
			if ( m_serviesList[i]!= 0  )
			{
				m_serviesList[i]->stop();
			}
		}
	}

	// 是否一个服务对象都没有了
	virtual bool empty()
	{
		//Simple_Atom_Lock lock(&m_mutex);
		//return m_service_map.empty();

		for ( int i=0;i<MAX_SERVICE_COUNT;++i )
		{
			if ( m_serviesList[i]!= 0  )
			{
				return false;
			}
		}

		return true;
	}

	virtual void release()
	{
		releaseInstance();
	}

    // 获取所有服务的ID
    virtual int get_all_service_ids(SERVICE_ID* arrayID, size_t nArraySize)
    {
        int nCount = 0;

        //for (NAMING_MAP::iterator iter = m_naming_map.begin(); iter != m_naming_map.end(); ++iter)
        //{
        //    if(nCount >= nArraySize)
        //        break;

        //    arrayID[nCount++]=iter->second;
        //}
		for ( int i=0;i<MAX_SERVICE_COUNT;++i )
		{
			if ( m_serviesList[i]!= 0  )
			{
				if(nCount >= nArraySize)
					break;

				arrayID[nCount++]=m_serviesList[i]->get_id();
			}
		}

        return nCount;
    }
};
