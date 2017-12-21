/*******************************************************************
** 文件名:	ISNSConnector.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	11/25/2016
** 版  本:	1.0
** 描  述:	SNS服务器连接器接口
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#ifndef __INTERFACE_SNS_CONNECTOR_H__
#define __INTERFACE_SNS_CONNECTOR_H__


#ifndef  SNS_API
#	ifdef SNSCONNECTOR_EXPORTS
#		define  SNS_API  extern "C" __declspec(dllexport)
#	else
#		define  SNS_API  extern "C" __declspec(dllimport)
#		pragma comment(lib, MAKE_DLL_LIB_NAME(SNSConnector))
#	endif
#endif


#pragma pack(1)
#pragma warning(disable:4200)


namespace SNS
{
	/** 处理SNS服务器发过来的消息
	*/
	struct ISNSMessageHandler
	{
		/** SNS服务器发过来的消息
		@param actionId	:消息码
		@param head		:消息头
		@param data		:消息内容
		@param len		:消息长度
		*/
		virtual void HandleSNSMessage(unsigned long msgId, void* head, void* data, size_t len) = 0;
	};

    /**
    @name : SNS服务器连接器
    @brief: 该模块负责维护和SNS服务器之间的连接,并转发各种命令
    */
    struct ISNSConnector
    {
        /** 释放
		*/
		virtual void			Release() = 0;

		/** 创建
		DWORD dwWorldID		: 服务器所在游戏世界ID
		WORD wServerType	: 服务器类型
		WORD wSubID			: 第n号服务器
		char *szServerName	: 服务器名称
		char *szWorldName	: 游戏世界名称
		@return  
		*/
		virtual bool			Create(DWORD dwWorldID, WORD wServerType, WORD wSubID, const char * szServerName) = 0;

		/** 连接服务器
		@brief			    : 启动服务器时连接SNS服务器,以后如果连接断开,该模块内部会定时重试
		@param szServerIP	: 服务器IP 
		@param wServerPort	: 服务器端口
		@param nThreadMode	: 网络通讯层多实例网络应用类型
		@return				: 如果没有网络错误,则返回成功
		*/
		virtual bool			ConnectServer(const char * szServerIP, WORD wServerPort,int nThreadMode = NetworkUserType_Default) = 0;
        
        /** 关闭和主服务器之间的连接
		*/
		virtual void			Close() = 0;

		/** 注册消息处理者
		@param dwModuleID 模块ID
		@param hander 消息处理者实例
		*/
		virtual bool			registerMessageHandler(DWORD dwModuleID, ISNSMessageHandler* handler) = 0;

		/** 注销消息处理者
		@param dwModuleID 模块ID
		*/
		virtual bool			unregisterMessageHandler(DWORD dwModuleID) = 0;

		/** 发送消息
		@param nMessageID 消息ID
		@param pData 消息
		@param nDataLen 消息大小
		@return 是否发送成功
		*/
		virtual bool			SendMessage(int nMessageID, void * pData, int nDataLen) = 0;

		/** 发送消息
		@param pData 消息
		@param nDataLen 消息大小
		@return 是否发送成功
		*/
		virtual bool			SendData(void * pData, int nDataLen) = 0;
    };

}; //namespace SNS

//////////////////////////////////////////////////////////////////////////


/// 静态库版本
#if defined(_LIB) || defined(SNS_STATIC_LIB)
#	pragma comment(lib, MAKE_LIB_NAME(SNSConnector))
extern "C" SNS::ISNSConnector * CreateSNSConnector(rkt::TimerAxis * pTimerAxis);
#	define	CreateSNSConnectorProc	CreateSNSConnector
#else 
/// 动态库版本
typedef SNS::ISNSConnector* (RKT_CDECL*procCreateSNSConnector)(rkt::TimerAxis * pTimerAxis);
#	define	CreateSNSConnectorProc	DllApi<procCreateSNSConnector>::load(MAKE_DLL_NAME(SNSConnector), "CreateSNSConnector")
#endif


#pragma pack()

#endif//__INTERFACE_SNS_CONNECTOR_H__