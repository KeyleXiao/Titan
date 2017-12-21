/*******************************************************************
** 文件名:	IManagerConnectorService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-07-21
** 版  本:	1.0
** 描  述:	管理器服务器连接服务			
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "buffer.h"
#include <vector>

struct SGameWorldConnectorInfo;

/**
	职责: 
	1.管理器服务器连接服务
*/
struct IManagerConnectorService
{
		/** 取得本地服务器在子服务器上的ID
		*/
		virtual DWORD                       getLocalServerID() = 0;

		/** 直接发送数据给子服务器
		@param pData         : 要发送的数据
		@param dwLen         : 数据长度
		*/
		virtual void                        sendData(ibuffer & msg) = 0;

		/** 发给主服务器数据
		@param pData         : 要发送的数据
		@param dwLen         : 数据长度
		*/
		virtual void                        sendDataToMain(ibuffer & msg) = 0;

		/** 发给根服务器数据
		@param pData         : 要发送的数据
		@param dwLen         : 数据长度
		*/
		virtual void                        sendDataToRoot(ibuffer & msg) = 0;

		/** 请求启动
		@param dwStartFlag   : 启动标识 EMManagerStartServerFlag
		@param dwUserData    : 用户数据
		*/
		virtual bool                        requestStart(DWORD dwStartFlag=0,DWORD dwUserData=0) = 0;

		/** 请求关闭,如果没有请求，就关了服务器，会当作当机处理
		@param dwCloseFlag   : 关闭标识 EMManagerCloseServerFlag
		@param dwUserData    : 用户数据
		*/
		virtual bool                        requestClose(DWORD dwCloseFlag=0,DWORD dwUserData=0) = 0;

		/** 取得在主服务器上的UID
		*/
		virtual DWORD                       getServerUID() = 0;

		/** 取得游戏世界UID
		*/
		virtual DWORD                       getWorldID() = 0;

		/** 取得游戏世界信息
		*/
		virtual SGameWorldConnectorInfo     getGameWorldInfo() = 0;

		///** 注册事件处理者
		//@param wEventID 事件ID	参考 EMEventManager
		//@param hander 事件处理者实例
		//*/
		//virtual bool                        registerEventHandler(WORD wEventID,IManagerEventHandler* handler) = 0;

		///** 注销事件处理者
		//@param wEventID 事件ID	参考 EMEventManager
		//@param  hander 事件处理者实例
		//*/
		//virtual bool                        unregisterEventHandler(WORD wEventID,IManagerEventHandler* handler) = 0;

		/** 设置实时数据
		DWORD dwDataID		: 实时数据类型ID,不同的服务器类型用不用的ID 参考 EMManagerRealTimeDataLogin
		int nValue			: 数值
		@return  是否成功设置
		*/
		virtual void                        setRealTimeData(DWORD dwDataID,int nValue) = 0;

		/** 取得实时数据
		DWORD dwDataID		: 实时数据类型ID
		@return  int nValue	: 数值
		*/
		virtual int                         getRealTimeData(DWORD dwDataID) = 0;

	    /** 设定服务器状态
	    DWORD dwStateFlag:服务器状态类型定义 EMManagerServerStateFlag
	    */
        virtual void                        setServerState(DWORD dwStateFlag) = 0;
};
