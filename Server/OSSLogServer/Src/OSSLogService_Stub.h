/*******************************************************************
** 文件名:	OSSLogService_Stub.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-7-19
** 版  本:	1.0
** 描  述:	日记服务


********************************************************************/

#pragma once;

#include "OSSLogService.h"
#include "OSSLogService_Proxy.h"


class COSSLogService_Stub : public IServiceStub
{
public:
	////////////////// IServiceStub ////////////////////////////////////////////////////////

	// 启动
	// @param data : 在目前这版本中,data里的内容是指向配置信息的TiXmlElement *指针
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len );

	// 停止
	virtual bool on_stop();

	// 处理消息
	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf );

	// 返回代理对象即可
	virtual void * query_interface( int iid );

	// 销毁
	virtual void release();

	////////////////// COSSLogService_Stub ////////////////////////////////////////////////////////

	// 构造函数
	COSSLogService_Stub(COSSLogService * pReal);

	// 析构函数
	~COSSLogService_Stub();

private:
	COSSLogService_Proxy	m_proxy;
	COSSLogService*		    m_real_service;
};