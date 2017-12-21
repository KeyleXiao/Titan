/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MapPathFind\Src\PathFindService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/12/2015	16:25
** 版  本:	1.0
** 描  述:	
            寻路服务存根
********************************************************************/

#pragma once

#include "IPathFindService.h"
#include "PathFindService_Proxy.h"
#include "PathFindService.h"
#include "DynamicObstacleService_Proxy.h"
#include "IServiceContainer.h"

struct IServiceStub;
// 存根接口
struct IPathFindService_Stub
{
	/* 取得实际Service处理对象
	@name				:
	*/
	virtual IPathFindService*   getServiceHandler() = 0;
};

class PathFindService_Stub : public IPathFindService_Stub
{
public:
    ///////////////IServiceStub///////////////////////////////////////////////////////////
	virtual bool on_start( IServiceContainer* pContainer,void * data,int len );
	virtual bool on_stop();
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf );	
	virtual void * query_interface( int iid );
	virtual void release();

    ///////////////IPathFindService_Stub///////////////////////////////////////////////////
    /* 取得实际Service处理对象
    @name				: 
    @return             : 开启动态障碍代理成功或者失败
    */
    virtual IPathFindService*   getServiceHandler();
    
public:
	BYTE			  m_nServiceType;		// 游戏逻辑服务类型 GAMELOGIC_SERVICE_TYPE
	PathFindService_Stub(PathFindService * pReal); 
	virtual ~PathFindService_Stub();

	bool startDynamicObstacleProxy();

	//动态障碍服务引用
	DynamicObstacleService_Proxy m_pDynamicObstacleProxy;
	PathFindService_Proxy	m_proxy;
	PathFindService*		m_real_service;
};


////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IPathFindService_Stub * CreatePathFindService();
