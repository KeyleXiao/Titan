/*******************************************************************
** 文件名:	IResourceManager.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-12-17
** 版  本:	1.0
** 描  述:	资源管理接口

		
********************************************************************/

#ifndef __I_SOURCE_MANAGER_H__ 
#define __I_SOURCE_MANAGER_H__

#include "buffer.h"


// 资源管理接口
struct IResourceManager
{
public:
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

    /** 执行收到的消息
	*/
	virtual void            onRecvMessage(const char * pData, size_t nLen) = 0;

    /** 获取指定类型的资源
	*/
    virtual int             getData(int nResourceType, void **ppOutBuff) = 0;

    /* 是否加载好DLL
    */
    virtual bool            isLoadDLL(void) = 0;

    /* 设置是否从PACK中加载LUA
    */
    virtual void            setFromPackLoadLua(bool bFromPack ) = 0;

    /* 是否从PACK包中加载LUA
    */
    virtual bool            isLoadLuaFromPack(void) = 0;

    /** 释放指定资源
	*/
    virtual void            releaseData(int nResourceType) = 0;
};

#endif	// __I_SOURCE_MANAGER_H__
