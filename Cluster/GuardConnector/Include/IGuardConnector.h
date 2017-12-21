/*******************************************************************
** 文件名:	IGuardConnector.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-19
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __INTERFACE_GUARD_CONNECTOR_H__
#define __INTERFACE_GUARD_CONNECTOR_H__


#include "GuardDef.h"


#pragma pack(1)
#pragma warning(disable:4200)

namespace Guard
{
	/** 处理数据监护服务器发过来的消息
	*/
	struct IGuardMessageHandler
	{
		/** 数据监护服务器发过来的消息
		@param actionId	:消息码
		@param head		:消息头
		@param data		:消息内容
		@param len		:消息长度
		*/
		virtual void HandleGuardMessage(unsigned long dwMsgID, void* head, void* data, size_t len) = 0;
	};


	/** 数据监护服务器连接器
	*/
	struct IGuardConnector
	{
		/** 创建
		DWORD dwWorldID		: 服务器所在游戏世界ID
		WORD wServerType	: 服务器类型
		WORD wSubID			: 第n号服务器
		char *szServerName	: 服务器名称
		char *szWorldName	: 游戏世界名称
		@return  
		*/
		virtual bool			Create(DWORD dwWorldID, WORD wServerType, WORD wSubID,const char * szServerName,const char * szWorldName) = 0;

		/** 释放
		*/
		virtual void			Release() = 0;

		/** 关闭和主服务器之间的连接
		*/
		virtual void			Close() = 0;

		/** 连接数据监护服务器
		@brief			: 启动服务器时连接中心服务器,以后如果连接断开,该模块内部会定时重试
		@param szCSIP	: 中心服务器IP 
		@param nCSPort	: 中心服务器端口
		@return			: 如果没有网络错误,则返回成功
		*/
		virtual bool			ConnectServer(const char * szCSIP,WORD nCSPort,int nThreadMode = NetworkUserType_Default) = 0;

		/** 注册消息处理者
		@param moduleId 模块ID
		@param hander 消息处理者实例
		*/
		virtual bool			registerMessageHandler(DWORD moduleId, IGuardMessageHandler* handler) = 0;

		/** 注销消息处理者
		@param moduleId 模块ID
		*/
		virtual bool			unregisterMessageHandler(DWORD moduleId) = 0;

		/** 往数据监护服务器写入数据
		@param nNameSpace    : 镜像数据空间命名 IMAGE_DATA_NAMESPACE
		@param nKey	         : 键值
		@param pData         : 数据
		@param dwLen         : 数据长度
		@return
		*/
		virtual bool			WriteImageData(IGuardMessageHandler* handler, int nNameSpace, int nKey, const char * pData, DWORD dwLen) = 0;

		/** 从数据监护服务器异步读取数据
		@param nNameSpace    : 镜像数据空间命名 IMAGE_DATA_NAMESPACE
		@param nKey	         : 键值
		@return
		*/
		virtual bool			ReadImageData(IGuardMessageHandler* handler, int nNameSpace, int nKey) = 0;

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
};

/// 静态库版本
#if defined(_LIB) || defined(CENTER_STATIC_LIB)
#	pragma comment(lib, MAKE_LIB_NAME(GuardConnector))
extern "C" Guard::IGuardConnector * CreateGuardConnector(rkt::TimerAxis * pTimerAxis);
#	define	CreateGuardConnectorProc	CreateGuardConnector
#else 
/// 动态库版本
typedef Guard::IGuardConnector* (RKT_CDECL*procCreateGuardConnector)(rkt::TimerAxis * pTimerAxis);
#	define	CreateGuardConnectorProc	DllApi<procCreateGuardConnector>::load(MAKE_DLL_NAME(GuardConnector), "CreateGuardConnector")
#endif

#pragma pack()

#endif	//__INTERFACE_GUARD_CONNECTOR_H__