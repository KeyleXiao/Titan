//==========================================================================
/**
* @file	  : IGameRPC.h
* @author : CHEN.TAO! <101251683@qq.com>
* created : 2008-11-7   16:55
* purpose : 游戏服务器远程过程调用
*/
//==========================================================================

#ifndef __INTERFACE_GAME_SERVER_RPC_H__
#define __INTERFACE_GAME_SERVER_RPC_H__

#ifdef SUPPORT_RPC

#include "VariantData.h"

#define    MSG_RPC_REQUEST   123                // 命令码: 请求远程调用一个函数

#define    MSG_RPC_SUBSCRIBE 234                // 命令码: 请求远程订阅一个事件

#define    MSG_RPC_BRIDGEREQUEST 235            // 命令码: 通过桥服务器请求远程调用一个函数

#define    RPC_PARAM_1      "1"                 // 远程过程调用的第一个参数
#define    RPC_PARAM_2      "2"                 // 远程过程调用的第二个参数
#define    RPC_PARAM_3      "3"                 // ""
#define    RPC_PARAM_4      "4"
#define    RPC_PARAM_5      "5"
#define    RPC_PARAM_6      "6"
#define    RPC_PARAM_7      "7"
#define    RPC_PARAM_8      "8"
#define    PRC_FUNC_NAME    "func"              // 请求调用的函数
#define    RPC_RETURN_FUNC  "ret_func"          // 返回结果时调用的函数
#define    RPC_RETURN       "return"            // 远程过程调用时返回的结果

/**
@ brief : 可以被用来远程调用的函数约定
@ param params : 传过来的参数列表,可以从里面依次取出参数1 ~ 参数n,另外函数返回的结果也设在这个参数里面
@              : 例如: 依次取参数
@              : params.getInt(RPC_PARAM_1)
@              : params.getString(RPC_PARAM_2)
@              : 返回结果
@              : params.setValue(RPC_RETURN,(int)1);
*/
typedef void (__stdcall *RPC_HOST_FUNCTION)( Variant_Dic & params );


/**
@ brief    : 游戏服务器远程过程调用管理器
@          : 用于实现游戏服务器之间的远程过程调用
*/
struct IGameRPC
{
	/**
	@ brief        : 注册一个可以被用来远程调用的函数
	@ param szFuncName : 函数名
	@ param funcPtr    : 函数指针
	*/
	virtual void RegisterRPCFunction( const char * szFuncName,RPC_HOST_FUNCTION funcPtr ) = 0;

	/**
	@ brief : 远程调用一个函数,最多支持8个参数
	@ param nRemoteServerID : 远程服务器ID
	@ param szFuncName      : 要调用的函数名
	@ param szReturnFunc    : 如果需要获取返回值,则填写返回时回调的函数名,如果该函数名为NULL,则表示不获取返回值
	@ param param1          : 传递给要调用的函数的第一个参数,这是一个混合类型,可以自由传入整型,字符串,或bool型数据,不传则表示该参数无效
	@ param param2          : 传递给要调用的函数的第二个参数,这是一个混合类型,可以自由传入整型,字符串,或bool型数据,不传则表示该参数无效
	@ param param3          : " "
	@ param param4          : " "
	@ note                  : 使用方法:
							: unsigned long nSocialServerID = 0; // 通过中心服务器接口获取社会服务器ID,为简便这里直接填0
							: RemoteCall( nSocialServerID,"GetFriendList","FriendListReturn",2344); // 从社会服务器上获取2344号玩家的好友列表,在FriendListReturn中接收
	*/
	virtual bool RemoteCall( unsigned long nRemoteServerID,
							  const char * szFuncName,
							  const char * szReturnFunc,
							  const Variant_Data & param1 = Variant_Data(),
							  const Variant_Data & param2 = Variant_Data(),
							  const Variant_Data & param3 = Variant_Data(),
							  const Variant_Data & param4 = Variant_Data(),
							  const Variant_Data & param5 = Variant_Data(),
							  const Variant_Data & param6 = Variant_Data(),
							  const Variant_Data & param7 = Variant_Data(),
							  const Variant_Data & param8 = Variant_Data()
							  ) = 0;

	/**
	@ brief  : 远程订阅一个事件
	@ param nRemoteServerID : 远程服务器ID
	@ param nEventType    : 要订阅的事件类型
	@ param nEventSource  : 要订阅的事件源
	@ param nEventID      : 要订阅的事件ID
	@ param szHandlerFunc : 事件处理函数名
	*/
	virtual bool RemoteSubscribe( unsigned long nRemoteServerID,int nEventType,int nEventSource,int nEventID,const char * szHandlerFunc ) = 0;

	/**
	@ brief  : 取消远程订阅一个事件
	@ param nRemoteServerID : 远程服务器ID
	@ param nEventType    : 要订阅的事件类型
	@ param nEventSource  : 要订阅的事件源
	@ param nEventID      : 要订阅的事件ID
	@ param szHandlerFunc : 事件处理函数名
	*/
	virtual bool RemoteUnSubscribe( unsigned long nRemoteServerID,int nEventType,int nEventSource,int nEventID,const char * szHandlerFunc ) = 0;

	/**
	@ brief               : 收到一个远程事件订阅请求
	@ param nFromServerID : 从哪个服务器发过来的请求
	@ param request       : 传过来的调用参数,包括请求的函数名以及调用的参数列表,可通过RPC_PARAM_1等宏做为键值取出来
	@ note                : RPC_PARAM_1 (int) 要订阅的事件类型
	@ note                : RPC_PARAM_2 (int) 要订阅的事件源
	@ note				  : RPC_PARAM_3 (int) 要订阅的事件ID
	@ note                : RPC_PARAM_4 (string) 事件处理函数名
	*/
	virtual bool RemoteSubscribe_Request( unsigned long nFromServerID,Variant_Dic & request) = 0;

	/**
	@ brief        : 收到一个远程过程调用请求
	@ param nFromServerID : 从哪个服务器发过来的请求
	@ param params        : 传过来的调用参数,包括请求的函数名以及调用的参数列表,可通过RPC_PARAM_1等宏做为键值取出来
	*/
	virtual void RPC_Request( unsigned long nFromServerID,Variant_Dic & request ) = 0;

	/**
	@ brief : 远程调用一个函数,最多支持8个参数（通过桥服务器）
	@ param nBridgeServerID : 远程服务器ID（桥服务器分配）
	@ param szFuncName      : 要调用的函数名
	@ param szReturnFunc    : 如果需要获取返回值,则填写返回时回调的函数名,如果该函数名为NULL,则表示不获取返回值
	@ param param1          : 传递给要调用的函数的第一个参数,这是一个混合类型,可以自由传入整型,字符串,或bool型数据,不传则表示该参数无效
	@ param param2          : 传递给要调用的函数的第二个参数,这是一个混合类型,可以自由传入整型,字符串,或bool型数据,不传则表示该参数无效
	@ param param3          : " "
	@ param param4          : " "
	@ note                  : 使用方法:
	: unsigned long nSocialServerID = 0; // 通过中心服务器接口获取社会服务器ID,为简便这里直接填0
	: RemoteCall( nSocialServerID,"GetFriendList","FriendListReturn",2344); // 从社会服务器上获取2344号玩家的好友列表,在FriendListReturn中接收
	*/
	virtual bool BridgeCall( unsigned long nBridgeServerID,
		const char * szFuncName,
		const Variant_Data & param1 = Variant_Data(),
		const Variant_Data & param2 = Variant_Data(),
		const Variant_Data & param3 = Variant_Data(),
		const Variant_Data & param4 = Variant_Data(),
		const Variant_Data & param5 = Variant_Data(),
		const Variant_Data & param6 = Variant_Data(),
		const Variant_Data & param7 = Variant_Data(),
		const Variant_Data & param8 = Variant_Data()
		) = 0;
};

#endif // #ifdef SUPPORT_RPC

#endif//__INTERFACE_GAME_SERVER_RPC_H__