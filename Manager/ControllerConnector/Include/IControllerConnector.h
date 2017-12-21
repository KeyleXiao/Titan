/*******************************************************************
** 文件名:	IControllerConnector.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	11/25/2016
** 版  本:	1.0
** 描  述:	中控服务器连接器接口
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#ifndef __INTERFACE_CONTROLLER_CONNECTOR_H__
#define __INTERFACE_CONTROLLER_CONNECTOR_H__


#ifndef  CONTROLLER_API
#	ifdef CONTROLLERCONNECTOR_EXPORTS
#		define  CONTROLLER_API  extern "C" __declspec(dllexport)
#	else
#		define  CONTROLLER_API  extern "C" __declspec(dllimport)
#		pragma comment(lib, MAKE_DLL_LIB_NAME(ControllerConnector))
#	endif
#endif


#pragma pack(1)
#pragma warning(disable:4200)

struct SControlMsgHead;
namespace Controller
{
	/** 处理中控服务器发过来的消息
	*/
	struct IControllerMessageHandler
	{
		/**
		@name         : 处理其他服务器通过子服务器转发的消息
		@brief        : 具体内容不清楚
		@param actionId  : 消息码
		@param head  : 消息头
		@param data  : 消息内容
		@param len:消息长度
		*/
		virtual void HandleControlMessage(unsigned long actionId, SControlMsgHead* head, void* data, size_t len) = 0;
	};

    /**
    @name : 子服务器连接器
    @brief: 该模块负责维护和子服务器之间的连接,并转发各种命令
    */
    struct IControllerConnector
    {
        /** 释放
		*/
		virtual void			Release() = 0;

		/** 创建
        @param nGameType: 游戏类型
        @param nEndPoint
        @param szServerName	: 服务器名称
		@return  
		*/
		virtual bool			Create(BYTE nEndPoint,const char * szServerName) = 0;

		/** 连接服务器
		@brief			    : 启动服务器时连接中心服务器,以后如果连接断开,该模块内部会定时重试
		@param szServerIP	: 服务器IP 
		@param wServerPort	: 服务器端口
		@param nThreadMode	: 网络通讯层多实例网络应用类型
		@return			: 如果没有网络错误,则返回成功
		*/
		virtual bool			ConnectServer(const char * szServerIP, WORD wServerPort,int nThreadMode = NetworkUserType_Default) = 0;
        
        /** 关闭和主服务器之间的连接
		*/
		virtual void			Close() = 0;

        /** 是否已连上服务器
		*/
        virtual bool            IsConnected() = 0;

        /** 请求服务器下发资源
		*/
        virtual bool            RequestResource(DWORD dwGameID, DWORD dwWorldID, BYTE nSourceServerType, DWORD dwResourceType) = 0;

        /** 请求服务器重加载资源
		*/
        virtual bool            RequestReloadResource(DWORD dwGameID, DWORD dwWorldID, BYTE nSourceServerType, DWORD dwResourceType) = 0;

        /** 请求服务器下发用本地资源启服游戏世界ID列表
		*/
        virtual bool            RequestLocalResourceWorldList(DWORD dwGameID) = 0;

        /** 获取用本地资源启服的游戏世界ID列表
		*/
        virtual DWORD           GetLocalResourceWorldList(DWORD dwGameID, DWORD * dwArray, DWORD dwCount) = 0;
    };

}; //namespace Controller

//////////////////////////////////////////////////////////////////////////


/// 静态库版本
#if defined(_LIB) || defined(CONTROLLER_STATIC_LIB)
#	pragma comment(lib, MAKE_LIB_NAME(ControllerConnector))
extern "C" Controller::IControllerConnector * CreateControllerConnector(Controller::IControllerMessageHandler * pHandler, rkt::TimerAxis * pTimerAxis);
#	define	CreateControllerConnectorProc	CreateControllerConnector
#else 
/// 动态库版本
typedef Controller::IControllerConnector* (RKT_CDECL*procCreateControllerConnector)(Controller::IControllerMessageHandler * pHandler, rkt::TimerAxis * pTimerAxis);
#	define	CreateControllerConnectorProc	DllApi<procCreateControllerConnector>::load(MAKE_DLL_NAME(ControllerConnector), "CreateControllerConnector")
#endif


#pragma pack()

#endif//__INTERFACE_CONTROLLER_CONNECTOR_H__