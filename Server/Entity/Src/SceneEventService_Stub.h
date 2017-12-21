/*******************************************************************
** 文件名:	SceneEventService_Stub.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	
            单个场景事件存根
********************************************************************/

#pragma once;

#include "SceneEventService.h"
#include "SceneEventService_Proxy.h"


// 单个场景事件存根
class SceneEventService_Stub : public IServiceStub
{
public:
	////////////////// IServiceStub ////////////////////////////////////////////////////////

	// 启动
	// @param data : 在目前这版本中,data里的内容是指向配置信息的TiXmlElement *指针
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len );

	// 停止
	virtual bool on_stop();

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf & resultBuf);

	// 返回代理对象即可
	virtual void * query_interface( int iid );

	// 销毁
	virtual void release();

	////////////////// SceneEventServiceService ////////////////////////////////////////////////////////

	// 构造函数
	SceneEventService_Stub(SceneEventService * pReal);

	// 析构函数
	~SceneEventService_Stub();

public:
	SceneEventService_Proxy		m_proxy;
	SceneEventService *			m_real_service;
};

extern "C" __declspec(dllexport) IServiceStub * CreateSceneEventService();