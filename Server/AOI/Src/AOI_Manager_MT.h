/*******************************************************************
** 文件名:	AOI_Manager_MT.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	10/18/2014
** 版  本:	1.0
** 描  述:	

			
********************************************************************/

#pragma once

#include "AOI_Manager.h"
#include "Thread.h"
#include "net/producer_consumer_queue.h"
#include <set>

using namespace std;
using namespace rkt;

// 定义基类
#define AOI_MANAGE  AOI_Manager<OBJECT_TYPE,COORD_TYPE,DIM,AOI_GRID_SIZE,LAYER_COUNT>

/**
	多线程版本的AOI视野管理
*/
template< typename OBJECT_TYPE,		/*对象类型*/
	typename COORD_TYPE,      /*坐标数据类型*/
	int DIM,					/*几维空间*/
	int AOI_GRID_SIZE,	    /*每个广播单元格的大小*/
	int LAYER_COUNT			/*有几层AOI对象*/
>
class AOI_Manager_MT : public AOI_MANAGE,public IRunnable
{
private:
	Thread  * m_pBroadcastThread;			// 广播线程
	bool      m_isAsyncMode;                // 是否是异步模式

	typedef std::set<OBJECT_TYPE>  OBJECT_MAP;
	OBJECT_MAP   m_objMap;					// 对象列表

	enum
	{
		AOI_CMD_INSERT = 1,					// 异步命令，插入对象
		AOI_CMD_REMOVE = 2,					// 异步命令，移除对象
		AOI_CMD_MOVE   = 3,                 // 异步命令，移动对象
		AOI_CMD_BROADCAST= 4,               // 异步命令，视野广播
		AOI_CMD_ADD_OBSERVER  = 5,          // 异步命令, 观察某个对象的视野
		AOI_CMD_DEL_OBSERVER  = 6,          // 异步命令, 观察某个对象的视野
	};

	struct AOI_COMMAND						// 异步命令，给另外一个线程用的
	{
		int   type;
		OBJECT_TYPE obj;
		COORD_TYPE loc[DIM];
		int   layer;

		union
		{
			struct MoveData
			{
				COORD_TYPE new_loc[DIM];
			}move_data;						// 移动数据

			struct BroadcastData
			{
				int dist;
				BROADCAST_ID id;
				Broadcast_Package * package;
			}broadcast_data;				// 广播数据

			OBJECT_TYPE observer;           // 观察者
		};
	};

	producer_consumer_queue<AOI_COMMAND>  m_commands;	// 异步命令列表,逻辑线程插入，广播线程取出

public:
	AOI_Manager_MT() : m_isAsyncMode(true),m_pBroadcastThread(0)
	{}

	virtual ~AOI_Manager_MT()
	{
		if(m_pBroadcastThread)
		{
			m_pBroadcastThread->terminate();
			delete m_pBroadcastThread;
			m_pBroadcastThread = 0;
		}
	}

	void initialize( COORD_TYPE loc[DIM],COORD_TYPE grid_num[DIM],bool isAsync )
	{
		AOI_MANAGE::initialize(loc,grid_num);
		switchMode(isAsync);
		return;
	}

	bool insertEntity( OBJECT_TYPE obj,COORD_TYPE loc[DIM],int layer )
	{
		AOI_COMMAND cmd;
		cmd.type = AOI_CMD_INSERT;
		cmd.obj  = obj;
		memcpy( cmd.loc,loc,sizeof(cmd.loc) );
		cmd.layer = layer;

		// 增加一下引用计数，防止obj对象释放后引起的非法
		obj->AddRef();

		if ( m_isAsyncMode==true )
		{
			m_commands.push(cmd);
		}else
		{
			do_command(cmd);
		}

		return true;
	}

	bool removeEntity( OBJECT_TYPE obj,COORD_TYPE loc[DIM],int layer )
	{
		AOI_COMMAND cmd;
		cmd.type = AOI_CMD_REMOVE;
		cmd.obj  = obj;
		memcpy( cmd.loc,loc,sizeof(cmd.loc) );
		cmd.layer = layer;

		// 增加一下引用计数，防止obj对象释放后引起的非法
		obj->AddRef();

		if ( m_isAsyncMode==true )
		{
			m_commands.push(cmd);
		}else
		{
			do_command(cmd);
		}
		return true;
	}

	bool moveEntity( OBJECT_TYPE obj,COORD_TYPE old_loc[DIM],COORD_TYPE new_loc[DIM],int layer )
	{
		AOI_COMMAND cmd;
		cmd.type = AOI_CMD_MOVE;
		cmd.obj  = obj;
		memcpy( cmd.loc,old_loc,sizeof(cmd.loc) );
		cmd.layer = layer;

		memcpy( cmd.move_data.new_loc,new_loc,sizeof(cmd.move_data.new_loc) );

		// 增加一下引用计数，防止obj对象释放后引起的非法
		obj->AddRef();

		if ( m_isAsyncMode==true )
		{
			m_commands.push(cmd);
		}else
		{
			do_command(cmd);
		}
		return true;
	}

	// 添加观察者
	bool addObserver(AOI_Proxy * pHost,AOI_Proxy * pObserver)
	{
		AOI_COMMAND cmd;
		cmd.type = AOI_CMD_ADD_OBSERVER;
		cmd.obj  = pHost;

		cmd.observer = pObserver;

		// 增加一下引用计数，防止obj对象释放后引起的非法
		pHost->AddRef();
		pObserver->AddRef();

		if ( m_isAsyncMode==true )
		{
			m_commands.push(cmd);
		}else
		{
			do_command(cmd);
		}

		return true;
	}

	// 移除观察者
	bool removeObserver(AOI_Proxy * pHost,AOI_Proxy * pObserver )
	{
		AOI_COMMAND cmd;
		cmd.type = AOI_CMD_DEL_OBSERVER;
		cmd.obj  = pHost;

		cmd.observer = pObserver;

		// 增加一下引用计数，防止obj对象释放后引起的非法
		pHost->AddRef();
		pObserver->AddRef();

		if ( m_isAsyncMode==true )
		{
			m_commands.push(cmd);
		}else
		{
			do_command(cmd);
		}

		return true;
	}

	bool broadcast( COORD_TYPE loc[DIM],int dist,BROADCAST_ID id,OBJECT_TYPE obj,void *pContext,int len,int layerMask=LAYER_WATCHER)
	{
		AOI_COMMAND cmd;
		cmd.type = AOI_CMD_BROADCAST;
		cmd.obj  = obj;
		memcpy( cmd.loc,loc,sizeof(cmd.loc) );
		cmd.layer = layerMask;

		cmd.broadcast_data.dist = dist;
		cmd.broadcast_data.id = id;

		if ( pContext!=0 && len >0 )
		{
			cmd.broadcast_data.package = new Broadcast_Package;
			cmd.broadcast_data.package->setContext(pContext,len);
		}else
		{
			cmd.broadcast_data.package = 0;
		}

		// 增加一下引用计数，防止obj对象释放后引起的非法
		if ( obj ) obj->AddRef();

		if ( m_isAsyncMode==true )
		{
			m_commands.push(cmd);
		}else
		{
			do_command(cmd);
		}
		return true;
	}

	void do_command( AOI_COMMAND & cmd )
	{
		switch(cmd.type)
		{
		case AOI_CMD_INSERT:
			m_objMap.insert(cmd.obj);
			AOI_MANAGE::insertEntity( cmd.obj,cmd.loc,cmd.layer );
			break;
		case AOI_CMD_REMOVE:
			m_objMap.erase(cmd.obj);
			AOI_MANAGE::removeEntity( cmd.obj,cmd.loc,cmd.layer );
			break;
		case AOI_CMD_MOVE:
			AOI_MANAGE::moveEntity( cmd.obj,cmd.loc,cmd.move_data.new_loc,cmd.layer );
			break;
		case AOI_CMD_BROADCAST:
			AOI_MANAGE::broadcast( cmd.loc,cmd.broadcast_data.dist,cmd.broadcast_data.id,cmd.obj,cmd.broadcast_data.package);
			if ( cmd.broadcast_data.package )
			{
				cmd.broadcast_data.package->Release();
			}
			break;
		case AOI_CMD_ADD_OBSERVER:
			cmd.obj->addObserver(cmd.observer);
			cmd.observer->Release();
			break;
		case AOI_CMD_DEL_OBSERVER:
			cmd.obj->removeObserver(cmd.observer);
			cmd.observer->Release();
			break;
		}

		if( cmd.obj )
		{
			cmd.obj->Release();
		}
	}

	bool switchMode( bool isAsync )
	{
		m_isAsyncMode = isAsync;

		if ( m_isAsyncMode==true )
		{
			if ( m_pBroadcastThread==0 )
			{
				m_pBroadcastThread = new Thread;
				m_pBroadcastThread->spawn(this);
			}
		}else
		{
			//AOI_COMMAND cmd;				// 异步切换到同步,要把之前的命令都执行完 
			//while( m_commands.pop(cmd) )
			//{
			//	do_command(cmd);
			//}
		}

		return true;
	}

	void sync_run()
	{
		if ( m_isAsyncMode==false )	// 只在同步模式调用
		{
			// 发送广播数据包
			OBJECT_MAP::iterator it = m_objMap.begin();
			for ( ;it!=m_objMap.end();++it )
			{
				(*it)->doBroadcast();
			}
		}
	}

	virtual void run()
	{
		while(1)
		{
			// 如果是同步模式，不用我做什么 
			if ( m_isAsyncMode==false )
			{
				Sleep(1000);
				continue;
			}

			// 执行命令
			DWORD tick = rkt::getTickCountEx();

			AOI_COMMAND cmd;
			while( m_commands.pop(cmd) )
			{
				do_command(cmd);
				if ( rkt::getTickCountEx()-tick>100 )
					break;
			}

			// 发送广播数据包
			OBJECT_MAP::iterator it = m_objMap.begin();
			for ( ;it!=m_objMap.end();++it )
			{
				(*it)->doBroadcast();
			}

		}
	}

	virtual void release() {}
};