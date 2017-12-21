/*******************************************************************
** 文件名:	SceneLogicService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-10
** 版  本:	1.0
** 描  述:	本文件为根据ISceneLogicService接口定义，自动生成的SceneLogicService接口
            代理文件，该文件是为符合Framework对接口进行的包装

			
********************************************************************/

#pragma once

/**
	服务容器:
	1.框架根据配置创建容器，容器负责与框架进行底层通讯，收到调用请求再转交给应用层提供的服务存根进行处理
	2.代理/存根，从COM架构借鉴过来的名词，应用层调用某个服务的接口，实际交由代理封包成消息，再提交给框架进行转发
	            而框架里的容器收到请求消息，则需要调用存根进行解释和真正处理
*/

#include "IServiceContainer.h"
#include "ISceneLogicService.h"
#include "buffer.h"

// 这个是服务实现类，你需要自己实现
#include "SceneLogicService.h"
#include "NPCService_Stub.h"
#include "TankService_Stub.h"
#include "LuaService_Stub.h"
#include "PathFindService_Stub.h"

using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现ISceneLogicService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class SceneLogicService_Proxy : public ISceneLogicService
{
public:
	IServiceContainer*		m_pContainer;
	SceneLogicService *     m_real_service;

	BYTE					m_nServiceType;		// 游戏逻辑服务类型 GAMELOGIC_SERVICE_TYPE
	INPCService*			m_pNPCService_Proxy;
	ITankService*			m_pTankService_Proxy;
	ILuaService*			m_pLuaService_Proxy;
	IPathFindService*		m_pPathFindService_Proxy;	// 寻路


	/////// ISceneLogicService //////////////////////////////////////////////////
	// 取得NPCService对象
	virtual INPCService * getNPCService(void)
	{
		return m_pNPCService_Proxy;
	}

	// 取得TankService对象
	virtual ITankService * getTankService(void)
	{
		return m_pTankService_Proxy;
	}

	// 取得LuaService对象
	virtual ILuaService * getLuaService(void)
	{
		return m_pLuaService_Proxy;
	}

	virtual IEventEngine * getEventService(void)
	{
		return m_real_service->getEventService();
	}

	virtual IPathFindService * getPathFindService(void)
	{
		return m_pPathFindService_Proxy;
	}

    // 取得AIService
    virtual SERVICE_PTR getAIService(void)
    {
        // 调试功能需要在service外部调用这个函数，会取不到结果,直接返回。
        if(m_real_service != NULL)
        {
            return m_real_service->getAIService();
        }

        BUILD_TYPE_MSG_CONTEXT_VOID( ISceneLogicService::getAIService);

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);

        if ( resultBuf.good() && resultBuf.size() > 0  )
        {
            ibuffer t_in(resultBuf.data(),resultBuf.size());
            SERVICE_PTR vReturn;
            t_in >> vReturn;
            if ( t_in.good() ) return vReturn;
        }

        return SERVICE_PTR(0);
    }

    virtual void destroyActor(UID uid, int nReason)
    {        
        obuf t_data;
        t_data << (BYTE)GAMELOGIC_SERVICE_NONE << uid << nReason;

        BUILD_MSG_BUFFER(ISceneLogicService::destroyActor, t_data);

        m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }

	virtual void  release(  )
	{
        BUILD_TYPE_MSG_CONTEXT_VOID( ISceneLogicService::release);

        m_pContainer->post_message( pMsg,nMsgLen,0);
	};

	/////// SceneLogicService_Proxy //////////////////////////////////////////////////
	SceneLogicService_Proxy()
		: m_pContainer(0)
		, m_real_service(0)
		, m_nServiceType(GAMELOGIC_SERVICE_CENTER)
		, m_pNPCService_Proxy(0)
		, m_pTankService_Proxy(0)
		, m_pLuaService_Proxy(0)
		, m_pPathFindService_Proxy(0)
	{

	}

	~SceneLogicService_Proxy()
	{

	}

	// 取得游戏逻辑服务类型
	BYTE getServiceType()
	{
		return m_nServiceType;
	}

	bool create(INPCService* pNPCService, ITankService* pTankService, ILuaService* pLuaService, IPathFindService *pPathFindService)
	{
		// 怪物
		m_pNPCService_Proxy = pNPCService;
		// 载具
		m_pTankService_Proxy = pTankService;
		// LUA
		m_pLuaService_Proxy = pLuaService;

		m_pPathFindService_Proxy = pPathFindService;

		return true;
	}

   //////////////////////////////////////////////////////////////////////////////////

};

/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class SceneLogicService_Stub : public IServiceStub
{
	BYTE			            m_nServiceType;	
	SceneLogicService_Proxy  m_proxy;
	SceneLogicService *      m_real_service;

	// NPC存根对象
	NPCService_Stub*			m_NPCService_Stub;
	// 载具存根对象
	TankService_Stub*			m_TANKService_Stub;
	// LUA存根对象
	LuaService_Stub*			m_LUAService_Stub;
	// 寻路存根对象
	PathFindService_Stub*		m_pPathFindService_Stub;

public:
	/////// SceneLogicService_Stub /////////////////////////////////////////////////////////
	SceneLogicService_Stub(SceneLogicService * pReal)
		: m_real_service(pReal)
		, m_NPCService_Stub(0)
//		, m_TANKService_Stub(0)
		, m_LUAService_Stub(0)
		, m_pPathFindService_Stub(0)
        , m_nServiceType(0)
	{
		// NPC存根对象
		m_NPCService_Stub = new NPCService_Stub(new NPCService);
		// 载具存根对象
		m_TANKService_Stub = new TankService_Stub(new TankService);
		// LUA存根对象
		m_LUAService_Stub = new LuaService_Stub(new LuaService);
		// 寻路存根对象
		m_pPathFindService_Stub = CAST_TYPE(PathFindService_Stub*, CreatePathFindService());

		m_proxy.m_real_service = m_real_service;
	}

	~SceneLogicService_Stub()
	{
	}

	/////// IServiceStub ///////////////////////////////////////////////////////////////////
	// 启动
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len )
	{
		m_proxy.m_pContainer = pContainer;

		// NPC存根对象
		m_NPCService_Stub->on_start(pContainer, data, len);
		//// 载具存根对象
		m_TANKService_Stub->on_start(pContainer, data, len);
		// LUA存根对象
		m_LUAService_Stub->on_start(pContainer, data, len);
		// 寻路存根对象
		m_pPathFindService_Stub->on_start(pContainer, data, len);

		m_proxy.create(&(m_NPCService_Stub->m_proxy)
			,&(m_TANKService_Stub->m_proxy)
			,&(m_LUAService_Stub->m_proxy)
			,CAST_TYPE(IPathFindService*, m_pPathFindService_Stub->query_interface(IID_PROXY))
		);
		m_real_service->create(m_NPCService_Stub->m_real_service
			, m_TANKService_Stub->m_real_service
			, m_LUAService_Stub->m_real_service
			, m_pPathFindService_Stub->getServiceHandler()
		);

        // lua里面会调用m_pEventService，如果在on_start里面处理m_pEventService为空
        m_LUAService_Stub->on_init();

        ibuffer in(data,len);
        int mapid = INVALID_MAPID;
		int scene = INVALID_SCENE_ID;
        bool isCopy = false;;
		int nWarID = INVALID_WAR_ID;
        in >> mapid >> scene >> isCopy >> nWarID; 

        // 场景ID校验
        if(scene > MAX_SCENE_COUNT)
        {
            ErrorLn(__FUNCTION__": scene overflow max value. scene="<< scene);
            return false;
        }

		return m_real_service->on_start(mapid, scene, isCopy, nWarID);
	}

	// 停止
	virtual bool on_stop()
	{
		// NPC存根对象
		m_NPCService_Stub->on_stop();
		// 载具存根对象
		m_TANKService_Stub->on_stop();
		// LUA存根对象
		m_LUAService_Stub->on_stop();
		// 寻路存根对象
		m_pPathFindService_Stub->on_stop();

		bool bResult = m_real_service->on_stop();

        return bResult;
	}

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf  )
	{
		if ( pMsg->context_len<sizeof(BYTE) )
			return false;

		BYTE byType = *(BYTE*)pMsg->context;

		switch (byType)
		{
		case GAMELOGIC_SERVICE_MONSTER:
			{
				return m_NPCService_Stub->handle_message(pMsg, resultBuf);
			}
			break;
		case GAMELOGIC_SERVICE_TANK:
			{
				return m_TANKService_Stub->handle_message(pMsg,resultBuf);
			}
			break;
		case GAMELOGIC_SERVICE_LUA:
			{
				return m_LUAService_Stub->handle_message(pMsg,resultBuf);
			}
			break;
		case GAMELOGIC_SERVICE_FINDPATH:
			{
				return m_pPathFindService_Stub->handle_message(pMsg,resultBuf);
			}
			break;

		default:	// 默认都进自己的Proxy
			{
				if ( TEST_SAME_FUNCTION(ISceneLogicService::release) )
				{
					m_real_service->release();
					return true;
				}

                if ( TEST_SAME_FUNCTION(ISceneLogicService::getAIService) )
                {
                    const SERVICE_PTR& vReturn = m_real_service->getAIService();
                    if( resultBuf ) *resultBuf << vReturn;
                    return true;
                }

                if ( TEST_SAME_FUNCTION(ISceneLogicService::destroyActor) )
                {
                    ibuffer in(pMsg->context,pMsg->context_len);
                    in.skip<BYTE>();
                    UID uid; int nReason;
                    in >> uid >> nReason;
                    m_real_service->destroyActor(uid, nReason);
                    return true;
                }

                ErrorLn(__FUNCTION__":can not found function name:" << MsgFunctionName<SERVICE_MESSAGE>()(pMsg) );
			}
			break;
		}

		return false;
	}

	// 返回代理对象即可
	virtual void * query_interface( int iid )
	{
		return &m_proxy;
	}

	// 销毁
	virtual void release()
	{
        m_proxy.m_pContainer = 0;

		// NPC存根对象
		if (m_NPCService_Stub)
		{
			m_NPCService_Stub->release();
			m_NPCService_Stub = 0;
		}
		// 载具存根对象
		//if (m_TANKService_Stub)
		//{
		//	m_TANKService_Stub->release();
		//	m_TANKService_Stub = 0;
		//}
		// LUA存根对象
		if (m_LUAService_Stub)
		{
			m_LUAService_Stub->release();
			m_LUAService_Stub = 0;
		}

		if(m_pPathFindService_Stub)
		{
			m_pPathFindService_Stub->release();
			m_pPathFindService_Stub = 0;
		}
		if ( m_real_service )
		{
			m_real_service->release();
			m_real_service = 0;
		}

		delete this;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateSceneLogicService()
{
	return new SceneLogicService_Stub(new SceneLogicService);
}