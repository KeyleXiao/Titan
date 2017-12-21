/*******************************************************************
** 文件名:	IMainManagerConnector.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2009/8/28
** 版  本:	1.0
** 描  述:	主服务器管理器连接模块
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __INTERFACE_MAINMANAGER_CONNECTOR_H__
#define __INTERFACE_MAINMANAGER_CONNECTOR_H__

#pragma once

#include "DManagerModule.h"

#include "TimerAxis.h"

namespace MainManager{


	/**
	@name : 主服务器管理器数据处理器
	@brief: 处理主服务器管理器发来的数据
	*/
	struct IMainManagerMessageHandler
	{

		/**
		@name         : 主服务器管理器服务器通知游戏服务器有新的数据包到达
		@brief        : 该数据包是客户端发给该游戏服务器的,通过数据包包头的dwServerID指定
		@param pData  : 数据缓冲区位置
		@param wDataLen:数据长度
		*/
		virtual void HandleReceivedData(LPVOID pData,DWORD dwDataLen) = 0;

	};

	/**
	@name : 维护游戏服务器与主服务器管理器服务器之间的联系
	@brief: 具体负责的事物有:
			1.维护和主服务器管理器服务器之间的连接
			2.发送和处理与主服务器管理器之间交互的数据
	*/
	struct IMainManagerConnector
	{
		/**
		@name         : 连接主服务器管理器服务器
		@brief        : 这个函数调用一次,因为只能连到一个主服务器管理器服务器
		@note         : 
		@param   ip   : 主服务器管理器服务器ip
		@param port   : 主服务器管理器服务器端口
		@return       : 连接过程中是否出错
		*/
		virtual bool Connect(const char * ip,WORD port) = 0;

		/**
		@name         : 向主服务器管理器服务器注册本地服务器的信息 
		@brief        : 主服务器管理器会根据该注册信息进行数据转发.
		@note         : (这个函数最好一开始就调用.紧随SetMessageHandler)
		@param wServerType: 服务器的类型
		@param dwServerKey: 该服务器对应的服务器ID
		@return       : 
		*/
		virtual bool RegisterServer(WORD wServerType,DWORD dwServerKey) = 0;

		/**
		@name         : 发送数据给客户端
		@brief        : 
		@param pData  : 数据缓冲区地址
		@param wDataLen:数据长度
		@return       : 如果该客户端不存在或者对应的主服务器管理器服务器已断开则返回false,其他返回true
		*/
		virtual bool SendData(LPVOID pData,DWORD dwDataLen) = 0;

		virtual void  Close() = 0;

		virtual DWORD GetServerID() const  = 0;

		virtual void SetServerID(DWORD dwServerID) = 0;

		virtual DWORD GetServerUID() const  = 0;

		virtual void SetServerUID(DWORD dwServerUID) = 0;

		virtual void  Release() = 0;

		//  硬件服务器向主服务器请求重起,如果没有请求，就关了服务器，会当作当机处理
		// DWORD dwRestartFlag:重起标识 EMManagerRestartServerFlag ,DWORD dwUserData: 用户数据
		virtual bool RequestRestart(DWORD dwRestartFlag=0,DWORD dwUserData=0) = 0;

		virtual void SetServerInstallVersion(DWORD dwPackVer,DWORD dwPackType,DWORD dwPackVer0,DWORD dwPackType0) = 0;
		// 主动重连
		virtual bool ReConnect() = 0;
		// 是否已连上
		virtual bool IsConnected() = 0;
	};

	/**
	@name            : 创建一个主服务器管理器连接管理对象
	@brief           :
	@param pHandler  : 数据处理接口
	@param pTimerAxis: 时间轴接口,主要用来定时重试
	@return          : 返回主服务器管理器管理接口指针
	*/
	//IMainManagerConnector * CreateMainManagerConnector(IMainManagerMessageHandler * pHandler,rkt::TimerAxis * pTimerAxis);
};

/// 静态库版本
#if defined(_LIB) || defined(MAINMANAGER_STATIC_LIB)
#	pragma comment(lib, MAKE_LIB_NAME(MainManagerConnector))
extern "C" MainManager::IMainManagerConnector * CreateMainManagerConnector(MainManager::IMainManagerMessageHandler * pHandler,rkt::TimerAxis * pTimerAxis);
#	define	CreateMainManagerConnectorProc  CreateMainManagerConnector
#else 
/// 动态库版本
typedef MainManager::IMainManagerConnector* (RKT_CDECL*procCreateMainManagerConnector)(MainManager::IMainManagerMessageHandler * pHandler,rkt::TimerAxis * pTimerAxis);
#	define	CreateMainManagerConnectorProc	DllApi<procCreateMainManagerConnector>::load(MAKE_DLL_NAME(MainManagerConnector), "CreateMainManagerConnector")
#endif

#endif//__INTERFACE_MAINMANAGER_CONNECTOR_H__