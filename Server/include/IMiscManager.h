/*******************************************************************
** 文件名:	IMiscManager.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-12-9
** 版  本:	1.0
** 描  述:	辅助全局对象的一个杂项管理器

********************************************************************/

#pragma once

#ifndef __I_MISC_MANAGER_H__
#define __I_MISC_MANAGER_H__

#include "IServiceContainer.h"

// 辅助全局对象的一个杂项管理器接口
struct IMiscManager
{
	/** 加载
	@return  
	*/
    virtual bool	        load() = 0;

	/** 执行Service的on_stop
	@return  
	*/
    virtual void            onStop(void) = 0;

	/** 释放
	@note     释放所有资源，并且销毁此对像
	@warning 
	*/
	virtual void			release(void) = 0;

	/* 用Service配置名称创建Service
    @param pszServiceSchemeName: Service配置名称
    */
	virtual SERVICE_PTR     create_service(const char * pszServiceSchemeName) = 0;

	/* 停止服务
    @param pszServiceSchemeName: Service配置名称
    */
    virtual void            stop_service(const char * szServiceSchemeName) = 0;

	/* 停止服务
    @param serviceID:服务ID
    */
    virtual void            stop_service(SERVICE_ID  serviceID) = 0;

    /* 注册到DBEngineService，这个有点尴尬*/
    virtual bool            registerDBEngineService() = 0;

    /* 校验游戏世界ID有效性*/
    virtual bool            checkWorldID(void) = 0;

    /* 校验账号有效性*/
    virtual bool            checkAccount(const char * pszUserName, const char * pszPassword) = 0;
};


#endif // __I_MISC_MANAGER_H__
