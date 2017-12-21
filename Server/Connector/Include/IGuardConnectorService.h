/*******************************************************************
** 文件名:	IGuardConnectorService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-18
** 版  本:	1.0
** 描  述:	数据监护服务器连接服务			
********************************************************************/

#pragma once

#include "IGuardConnector.h"
#include "IServiceContainer.h"
#include "buffer.h"
#include <vector>

using namespace Guard;

/**
	职责: 
	1.连接数据监护服务器
*/
struct IGuardConnectorService
{
	/** 关闭和主服务器之间的连接
	*/
	virtual void			Close() = 0;

	/** 注册消息处理者
	@param moduleId 模块ID
	@param hander 消息处理者实例
	*/
	virtual bool			RegisterMessageHandler(DWORD dwNameSpace, IGuardMessageHandler* handler) = 0;

	/** 注销消息处理者
	@param moduleId 模块ID
	*/
	virtual bool			UnregisterMessageHandler(DWORD dwNameSpace, IGuardMessageHandler* handler) = 0;

	/** 往数据监护服务器写入数据
	@param nNameSpace    : 镜像数据空间命名 IMAGE_DATA_NAMESPACE
	@param nKey	         : 键值
	@param pData         : 数据
	@param dwLen         : 数据长度
	@return
	*/
	virtual bool			WriteImageDataToServer(IGuardMessageHandler* handler, int nNameSpace, int nKey, const char * pData, DWORD dwLen) = 0;

	/** 从数据监护服务器异步读取数据
	@param nNameSpace    : 镜像数据空间命名 IMAGE_DATA_NAMESPACE
	@param nKey	         : 键值
	@return
	*/
	virtual bool			ReadImageDataFromServer(IGuardMessageHandler* handler, int nNameSpace, int nKey) = 0;



	/** 取得本地服务器在中心服务器上的ID
	*/
	virtual DWORD			GetLocalServerCSID() = 0;

	/** 是否与服务器断开连接了
	@param   
	@param   
	@return  
	*/
	virtual bool			IsDisconnectToServer(void) = 0;

	/** 是否已启动
	@param   
	@param   
	@return  
	*/
	virtual bool			IsStartup() = 0;
};
