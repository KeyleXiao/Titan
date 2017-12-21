/*******************************************************************
** 文件名:	netdef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2013-03-07
** 版  本:	1.0
** 描  述:	网络通讯接口定义
** 应  用:  与net.h分开,方便用

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#ifndef __NETDEF_H__
#define __NETDEF_H__

namespace rkt
{

	#define MAX_NET_PACKAGE_SIZE  (1024*16)	// 最大数据块大小(数据包大小不能大于16k)
	#define MAX_UDP_PACKAGE_SIZE  (1024*2)	// UDP最大数据块大小(UDP MTU理论不会超过1600,安全大小是576)

	#define CONN_ERROR_PACKER  -1         // 解包失败
	#define CONN_ERROR_DNS     -2         // 域名解释失败
	#define CONN_ERROR_PROXY   -3         // 代理服务器连接失败

	/// 发送数据时的标志位
	enum SEND_DATA_FLAG
	{
		NO_COPY      = 0X01 ,     // 发送数据时不要额外拷贝内存,这个标志主要是为了提高性能
		// 因为发送数据是一个异步重叠IO,所以传递给操作系统SOCKET
		// 实现的数据BUFF不能指向栈上,所以通常没发送一次数据内部
		// 都要重新new一段内存,并把数据拷贝过去再发.如果SendData
		// 函数中传递的数据地址已经是分配在堆上的,用户调用时可以
		// 指定就用这块内存发送,这样可以减少一次内存拷贝的开销.
		NO_ENCRYPT   = (0X01<<1), // 发送数据时通常会使用默认的加密算法进行加密.为了取消加密
		// 或者应用层已经加过密了可以设置该标志取消内部的加密过程。
		NO_PACKAGE   = (0X01<<2), // 取消内部的组包过程.因为TCP是流传输的,所以发数据时默认会组包
		OOB_DATA     = (0X01<<3), // 紧急数据,暂时不支持
		FLAG_COMBIN  = (0X01<<4), // 当数据包超过最大值时合并发送(这个标志仅供内部使用)
		NO_ACK       = (0X01<<5), // 不需要确认的包，即可以被丢弃的
	};


	// 网络通讯层多实例网络应用类型,用来实现多个实例以支持多个处理线程收发
	// CallbackDispatcher由CDispatcherManager ,post_complete_simulation由CPostCompleteSimulationManager来管理多实例
	enum EMNetworkUserType
	{
		NetworkUserType_Default = 0,	// 系统默认
		NetworkUserType_User1,			// 扩展应用1
		NetworkUserType_User2,			// 扩展应用2
		NetworkUserType_User3,			// 扩展应用3
		NetworkUserType_User4,			// 扩展应用4
        NetworkUserType_User5,          // 扩展应用5
		NetworkUserType_Max,
	};


	// 网络连接位标记,用来扩展功能,并占用内存少
	enum EMNetworkFlag
	{
		NetworkFlag_NoTrace		= 0x1,	///  不显示一般Trace信息,如http连接
		//NetworkFlag_Unuse		= 0x2,	/// 
		//NetworkFlag_Unuse		= 0x4,	/// 
		//NetworkFlag_Unuse		= 0x8,	/// 
		//NetworkFlag_Unuse		= 0x10,	/// 
		//NetworkFlag_Unuse		= 0x20,	/// 
		//NetworkFlag_Unuse		= 0x40,	/// 
		//NetworkFlag_Unuse		= 0x80,	/// 
		//NetworkFlag_Unuse		= 0x100, ///
		//NetworkFlag_Unuse		= 0x200, ///
		//NetworkFlag_Unuse		= 0x400, ///
		//NetworkFlag_Unuse		= 0x800, /// 
		//NetworkFlag_Unuse  	= 0x1000, /// 
		//NetworkFlag_Unuse		= 0x2000, /// 
	};

	/// 一次发送最大Buff数目，不能超过MAX_ASYN_IO_BUFF_COUNT
#	define	MAX_ASYN_IO_SEND_BUFF_COUNT			128

};

#endif//__NETDEF_H__