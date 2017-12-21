/*******************************************************************
** 文件名:	IServiceMgr.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/10/2014
** 版  本:	1.0
** 描  述:	服务管理器

			
********************************************************************/

#pragma once

#include "share_ptr.h"
#include "IServiceContainer.h"

/**
	服务管理器:
	1.根据名字查找服务（Service)信息
	2.通过根服务器同步各节点的服务（Service)注册信息
	3.服务器管理本身也可以是个服务
*/

struct IServiceMgr
{
	/** 
	取得服务(Service)对象指针
	@desc     : 调用这个函数是线程安全的，不论在哪个服务都可以调用
	@param id : 服务ID
	*/
	virtual SERVICE_PTR get_service( SERVICE_ID id ) = 0;

	/**
	根据名字查找服务指针
	@desc       : 这个速度会比较慢
	@param name : 服务名称
	*/
	virtual SERVICE_PTR find_service( const char * name ) = 0;

	/** 
	注册服务(Service)
	@param id  : 服务ID,如果服务ID为INVALID_SERVICE_ID,则会自动生成一个唯一ID
	*/
	virtual bool register_service( SERVICE_PTR pService,SERVICE_ID id=INVALID_SERVICE_ID ) = 0;

	/** 
	注销服务(Service)
	@desc      : 注销服务时会将之前的ID收回
	*/
	virtual bool unregister_service( SERVICE_PTR pService ) = 0;

	// 停止所有服务的运行
	virtual void stop_all() = 0;

	// 是否一个服务对象都没有了
	virtual bool empty() = 0;

    // 获取所有服务的ID
    virtual int get_all_service_ids(SERVICE_ID* arrayID, size_t nArraySize) = 0;
};