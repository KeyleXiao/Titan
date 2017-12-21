/*******************************************************************
** 文件名:	AOI_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	10/13/2014
** 版  本:	1.0
** 描  述:	

			
********************************************************************/

#pragma once

#include <list>
#include "AOI_Define.h"
#include "Broadcast_Package.h"
#include "Ref_Count_Obj.h"
#include "cache_queue.h"
#include "cache_policy.h"
#include "buffer.h"

using namespace rkt;
using namespace std;

class AOI_Proxy;



// 视野逻辑处理器具，由应用层实现
struct IAOI_Proxy_Handler
{
	// 取得实体对象 
	virtual __IEntity * getEntity() = 0;

	// 取得ID
	virtual int getID() = 0;

	// 取得视野
	virtual int getAOISight() = 0;

	// 取得位置
	virtual void getLocation(int *loc) = 0;

	// 取得广播包缓冲时间
	virtual int  getCacheDelay( BROADCAST_ID id,AOI_Proxy * obj_1,AOI_Proxy * obj_2,Broadcast_Package * package) = 0;

	// 发送一个数据包
	virtual void sendPackage(Broadcast_Package * package) = 0;

	// 打包出现数据包,用来发送给各客户端
	virtual void getAppearancePackage(obuf & data) = 0;

	// 打包消失数据包,用来发送给各客户端
	virtual void getDisappearancePackage(obuf & data) = 0;
};

/**
	AOI代理对象
	1.实现每个对象的视野订阅机制
	2.实现每个对象的多线程广播机制
	3.实现每个对象的广播缓存机制
	*/

class AOI_Proxy : public Ref_Count_Obj
{
protected:
	IAOI_Proxy_Handler * m_pHandler;    // 处理器

	typedef std::list<AOI_Proxy *>     OBSERVER_LIST;	   // 观察者队列
	typedef std::list<AOI_Proxy *>     CLEAN_LIST;         // 释放队列

	OBSERVER_LIST   m_observerList;
	CLEAN_LIST      m_cleanList;

	typedef cache_queue<Broadcast_Package *,unsigned __int64,int>     CACHE_QUEUE;
	CACHE_QUEUE     m_package_cache;			// 数据包缓冲池

	Broadcast_Package  *  m_appearance_package;		// 外观数据包，用于广播给其他用户
	Broadcast_Package  *  m_disappearance_package;	// 消失数据包，用于广播给其他用户

public:
	AOI_Proxy() : m_appearance_package(0),m_disappearance_package(0),m_pHandler(0){
	}

	// 安全释放，避免野指针
	virtual ~AOI_Proxy()
	{
		while( m_observerList.size()>0 )
		{
			removeObserver( *(m_observerList.begin()) );
		}

		while( m_cleanList.size()>0 )
		{
			AOI_Proxy *proxy = *(m_observerList.begin());
			proxy->removeObserver(this);
		}
	}

	// 设置处理器
	void setHandler( IAOI_Proxy_Handler * pHandler)
	{
		m_pHandler = pHandler;
	}

	IAOI_Proxy_Handler * getHandler()
	{
		return m_pHandler;
	}

	// 取得视野
	int getAOISight()
	{
		return m_pHandler->getAOISight();
	}

	// 取得位置
	virtual void getLocation(int *loc)
	{
		return m_pHandler->getLocation(loc);
	}

	// 取得某个对象的外观数据，用于广播给别人
	virtual Broadcast_Package * getAppearance()
	{
		// 这里有一个BUG:玩家坐标等数据更新时package没有实时更新, 暂时每次都重新获取一次
		if (m_appearance_package)
		{
			delete m_appearance_package;
			m_appearance_package = 0;
		}
		if ( m_appearance_package==0 )
		{
			m_appearance_package = new Broadcast_Package;
			obuf data;
			m_pHandler->getAppearancePackage(data);
			m_appearance_package->setContext(data.data(),data.size());
		}

		return m_appearance_package;
	}

	// 获得消失数据包，用于广播给其它人
	virtual Broadcast_Package * getDisappearance()
	{
		if ( m_disappearance_package==0 )
		{
			m_disappearance_package = new Broadcast_Package;
			obuf data;
			m_pHandler->getDisappearancePackage(data);
			m_disappearance_package->setContext(data.data(),data.size());
		}

		return m_disappearance_package;
	}

	// 添加观察者
	bool addObserver(AOI_Proxy * pObserver)
	{
		removeObserver(pObserver);
		m_observerList.push_back(pObserver);
		pObserver->m_cleanList.push_back(this);
		return true;
	}

	// 移除观察者
	bool removeObserver(AOI_Proxy * pObserver)
	{
		m_observerList.remove(pObserver);
		pObserver->m_cleanList.remove(this);
		return true;
	}

	// 收到广播,type取值见AOI_Define.h
	virtual void onBroadcast( BROADCAST_ID id,AOI_Proxy * obj,int nCheckDist,Broadcast_Package * package )
	{
		// 方便外部调用，取进出视野的2种默认数据包,并且这些包都是引用的，只保存一份，可大大减少内存占用
		if ( package==0 )
		{
			switch(id)
			{
			case BROADCAST_ENTER_VIEWPORT:
				package = obj->getAppearance();
				break;
			case BROADCAST_LEAVE_VIEWPORT:
				package = obj->getDisappearance();
				break;
			default:
				return;		// 空包，错误!!
			}
		}

		// 检查距离
		if ( nCheckDist!=0 )
		{
			if ( nCheckDist<0 )   // 传入-1表示检查是否在视野内
				nCheckDist = m_pHandler->getAOISight();

			int myLoc[2];
			m_pHandler->getLocation(myLoc);
			int objLoc[2];
			m_pHandler->getLocation(objLoc);

			for ( int i=0;i<2;++i )
			{
				if ( abs(myLoc[i]-objLoc[i])>nCheckDist )
					return;
			}
		}

		// 给观察者转发
		OBSERVER_LIST::iterator it = m_observerList.begin();
		OBSERVER_LIST::iterator end= m_observerList.end();
		for ( ;it!=end;++it )
		{
			(*it)->onBroadcast( id,obj,0,package );
		}

		// 根据缓冲策略塞到缓冲列表中
		int delay = m_pHandler->getCacheDelay( id,this,obj,package );
		package->AddRef();	// 插入队列就要增加引用计数

		Broadcast_Package * replaced =0;
		if ( m_package_cache.push( BROADCAST_ID_BUILD(id,obj),delay + rkt::getTickCountEx(),package,replaced )==false )
		{
			replaced->Release(false);	// 冗余的广播包，释放引用计数
		}
	}

	// 处理广播数据包
	virtual void doBroadcast()
	{
		unsigned long tick = rkt::getTickCountEx();

		CACHE_QUEUE::ELEMENT * element = m_package_cache.front();
		while( element )
		{
			if ( element->s <= tick )
			{
				// 发送该数据包
				m_pHandler->sendPackage(element->data);
				
				// 释放一下引用计数
				element->data->Release();

				// 取出头节点
				m_package_cache.pop();
				element = m_package_cache.front();
			}else
			{
				break;
			}
		}
	}
};