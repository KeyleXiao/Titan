/*******************************************************************
** 文件名:	GatewayServiceDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	02/15/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __COMMON_GATEWAY_SERVICE_DEF_H__
#define __COMMON_GATEWAY_SERVICE_DEF_H__
 
#pragma pack(1)
#pragma warning(disable:4200)

/************************************************************************/
/* 调试信息输出宏定义                                                   */
/************************************************************************/
// 输出DEBUG信息
#ifdef VERSEION_DEBUG
#   define GATEWAY_DEBUG_OUTPUT
#endif
#ifdef GATEWAY_DEBUG_OUTPUT
#	define GATEWAY_TRACELN(x)			TraceLn(x)
#	define GATEWAY_EMPHASISLN(x)		EmphasisLn(x)
#	define GATEWAY_ERRORLN(x)			ErrorLn(x)
#	define GATEWAY_WARNINGLN(x)			WarningLn(x)
#else
#	define GATEWAY_TRACELN(x)
#	define GATEWAY_EMPHASISLN(x)
#	define GATEWAY_ERRORLN(x)
#	define GATEWAY_WARNINGLN(x)
#endif


#define  INVALID_CLIENT (ClientID)0xFFFFFFFF
#define  MAX_GATEWAYID      32                      // 因为ClientID中只用了8位表示网关ID,所以网关ID最大只能255位
#define  MAX_MACADDR_LEN    24
#define  MAX_BCLIENT_COUNT  8192					// 一台网关服务器最多承载的人数,广播数据用
#define  UDP_OFFSET         0                       // 网关服接收客户端的UDP接口在TCP接口的偏移量
/**
@name : rkt网关协议
@brief: 通常网关完成两项基本功能:
		1.数据交换 (Data Switching) : 把收到的数据转发给内网不同的机器,同时帮他们把数据发出去
		2.路由选择 (Routing) : 维护网关与网关之间的路由表,常见的协议有OSPF(链路状态-最优路径),RIP(距离-向量)等等
		我们这里实现的网关服务器主要用于转发游戏服务器和游戏客户端之间的数据
		所以主要实现的是数据交换协议部分的内容
*/
namespace Gateway
{
	///////////////////////// Kickout客户端源因 /////////////////////
	enum
	{
		EKickoutClient_Reason_PersonDataInvalid = 0,	// 角色数据非法
		EKickoutClient_Reason_ShootFailed,				// 切换进程失败
		EKickoutClient_Reason_UserLocked,				// 帐号被锁
		EKickoutClient_Reason_StopZoneSvr,				// 停止场景服
		EKickoutClient_Reason_GMKickout,				// GM强制踢人
		EKickoutClient_Reason_StarTrekFailed,			// 跨区切换失败
		EKickoutClient_Reason_SecretiveError,			// 诡异的错误
		EKickoutClient_Reason_KickoutSelf,				// 自己踢掉自己
		EKickoutClient_Reason_Hack,                     // 使用非法程序
		EKickoutClient_Reason_Matrix,                   // 矩阵卡尝试太多
		EKickoutClient_Reason_LimitLogin,               // 公共区限制登陆
		EKickoutClient_Reason_Speed,                    // 检测到加速
		EKickoutClient_Reason_NoPing,                   // 没有回复ping
		EKickoutClient_Reason_RoleVended,               // 角色出售
		EKickoutClient_Reason_TryPlay,					// 试玩
		EKickoutClient_Reason_KinChangeLand,			// 家族移民执行成功后系统强制踢人

        EKickoutClient_Reason_RepeateLogin,             // 重复登录

		EMMSG_COMMON_CLIENT_ZONE_PERFORMANCE,			// 客户端与服务器性能检测

        
	};

	///////////////////////// 消息对话框类型 ////////////////////////
	enum
	{
		EMsgBox_Type_LoginKickout = 401,				// 登录服Kickout
		EMsgBox_Type_ZoneKickout,						// 场景服Kickout
		EMsgBox_Type_PersonRelive,						// 人物复活	
		EMsgBox_Type_AskAgreeRelive,					// 询问是否复活
		EMsgBox_Type_MarryMessageBox,					// 婚姻
		EMsgBox_Type_MarryConfirmMessageBox,			// 婚姻确认
		EMsgBox_Type_TansmitConfirm,					// 传送确认
		EMsgBox_Type_TeachStudConfirm,					// 师徒确认
		EMsgBox_Type_TeachStudDida,						// 师徒Dida
		EMsgBox_Type_TeachStudTips,						// 师徒Tips
		EMsgBox_Type_TeachStudCommon,					// 师徒通用
		EMsgBox_Type_CreditChange,						// 声望兑换
		EMsgBox_Type_SLoginLock,						// 登陆安全锁
		EMsgBox_Type_WealthProtect,						// 财产保护
		EMsgBox_Type_Stock,								// 金币交易
		EMsgBox_Type_CreditConvert,						// 声望转换
		EMsgBox_Type_TSCreditTaken,						// 师徒声望提成
		EMsgBox_Type_EquipmentForging,					// 装备打造
		EMsgBox_Type_UpgradeLingHun,					// 经验打折升级
		EMsgBox_Type_RoleVended,						// 角色出售
		EMsgBox_Type_Drop,								// 物品掉落
		EMsgBox_Type_CardCollect,						// 卡片收集
		EMsgBox_Type_WingConstructor,					// 翅膀打造
		EMsgBox_Type_TPrivilege,						// 金币特权
		EMsgBox_Type_Plaza,								// 商城购买
		EMsgBox_Type_RookieCamp,						// 新手训练营
		EMsgBox_Type_PublicExit,						// 公共区退出
        EMsgBox_Type_AskIfNeedForceLogin,               // 是否强制登录
        EMsgBox_Type_LoginButNotEnterZone,              // 登录后不能正常进入场景服
	};

	//////////////////////////////////////////////////////////////////////////
	/***************** 数据交换部分(Data Switching) *************************/

	/**
	@name         : 连接到网关的客户端ID
	@brief        : 可以使用ClientID指明向哪台机发送消息
	@note         : ClientID由两部分组成 网关ID(4bits) + 连接序号(28bits)
	*/ 
	typedef unsigned long  ClientID;  
    typedef ClientID       ServerID;

	struct ClientIDInfo
	{
		unsigned int nGatewayID : 8;  // 网关ID
		unsigned int nConnIndex :24;  // 连接序号
		ClientIDInfo(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

    // clientID以后改成64位的，一直累加就用不完，就不用考虑处理因为ID复用造成的问题了。
    typedef unsigned long long SessionID;

    struct SessionIDInfo
    {
        unsigned long long ullSerial : 56;     // sessionID
        unsigned long long nGatewayID : 8; // 网关ID

        SessionIDInfo(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

	//////////////////////////////////////////////////////////////////////////
	// 网关消息标识定义
	enum EMGATEWAYMESSAGE
	{
		MSG_GATEWAY_RECVDATA=1,						// 通知:收到客户端发来的数据
		MSG_GATEWAY_SENDDATA,						// 请求网关服务器发送数据给客户端
		MSG_GATEWAY_BROADCAST,						// 请求网关服务器广播消息
		MSG_GATEWAY_DISCONNECTED,					// 通知:某客户端与网关服务器端开连接
		MSG_GATEWAY_CLOSE_CONN,						// 请求:和某个客户端断开连接
		MSG_GATEWAY_REDIRECT,						// 请求:让网关服务器通知客户端连接另外一台服务器
		MSG_GATEWAY_REQUEST_MIGRATE,				// 告诉网关服务器,某玩家已经在场景中迁徙,其区域相关的数据需要发到新的区域服务器
		MSG_GATEWAY_MIGRATE_RESULT,					// 回复服务器角色迁出结果
		MSG_GATEWAY_REGISTER_SERVER,				// 内网的游戏服务器连接上网关后向服务器注册自己
		MSG_GATEWAY_HANDSHAKE_RESPONSE,				// 网关服务器回应握手信息
		MSG_GATEWAY_CLIENT_INCOMING,				// 通知:有新客户端连入
		MSG_GATEWAY_SERVER_LOST_CONNECTION,			// 指定服务器失去连接
		MSG_GATEWAY_PING,							// 服务器PING
		MSG_GATEWAY_PING_REPLY,						// 客户端回复PING
		MSG_GATEWAY_HANDSHAKE,						// 客户度发给网关服务器的握手包
		MSG_GATEWAY_P2PBROADCAST,					// P2P广播请求
		MSG_GATEWAY_P2PBROADCAST_FAILD,				// P2P广播失败,服务器请给失败的用户重发一次
		MSG_P2PBROADCAST_DATA,						// 收到广播数据
		MSG_P2PBROADCAST_ACK,						// 确认收到广播包
		MSG_GATEWAY_UCODE,							// 网关服务器发送唯一编码给客户端
		MSG_GATEWAY_CLOSE,							// 网关服务器关闭了，通知客户端不要重连
		MSG_CLIENT_REDIRECT,						// 通知客户端重定向
		MSG_CLIENT_TPLAYERNOTCONN,					// 通知客户端停机维护不要断线续完
		MSG_GATEWAY_CONNECTSUCCESS,					// 网关服务器发送唯一编码给客户端
		MSG_GATEWAY_CLIENT_ZONE_PERFORMANCE,		// 客户端，网关，场景服，性能检测消息包
		MSG_COMMON_MESSAGEBOX,						// 系统消息，客户端会弹出对话框

        MSG_GATEWAY_UPDATE_CLIENT_NODE_LOC,         //  网关服通知其他服务器，客户端场景节点位置改变
	};

	/**
	@name         : 网关服务器通信格式
	@brief        : 网关服务器和内网的其他服务器的通信格式为: 消息头结构 + 实际数据
	@note         : 消息头结构是所有服务器通用的: 消息头 = 目标服务器ID(2bytes) + 模块ID(2bytes) + 消息码(2bytes)
	@note         : 网关服务器转发消息时: 目标服务器ID为(MSG_SERVERID_UNKNOW)+模块ID为(MSG_MODULEID_GATEWAY)+消息码为以下列出的消息码中的一个
	*/

	/** 通知:收到客户端发来的数据
	@brief: 数据内容参考SMsgGatewayRecvData结构
	*/
	// MSG_GATEWAY_RECVDATA
	struct SMsgGatewayRecvData
	{
		ClientID     client;         // 客户端ID
        WORD         flag;           // SEND_DATA_FLAG
		WORD         wDataLen;       // 数据长度
		//char         DataBuff[];     // 数据缓冲区,缓冲区长度为wDataLen的值,所以这是一个不定长结构
	};

	/**	请求网关服务器发送数据给客户端
	@brief: 数据包格式参考SMsgGatewaySendData结构
	*/
	// MSG_GATEWAY_SENDDATA
	struct SMsgGatewaySendData
	{
		ClientID     client;         // 客户端ID
		WORD         wDataLen;       // 数据长度
		//char         DataBuff[];     // 数据缓冲区,缓冲区长度为wDataLen的值,所以这是一个不定长结构
	};

	/** 请求网关服务器广播消息
	@brief: 数据包格式为SMsgGatewayBroadcastUser结构+SMsgGatewayBroadcastData结构,注意这两个结构都是不定长的
	@note: 用途举例: 广播聊天消息
	*/
	// MSG_GATEWAY_BROADCAST
	struct SMsgGatewayBroadcastData
	{
		WORD         wClientNum;    // 要广播给多少个客户端
		WORD         wDataLen;      // 要广播的数据长度
		//ClientID     ClientTable[]; // 接收数据的客户端列表,数组的元素个数为wClientNum的值,所以这是个不定长结构
		//char         DataBuff[];    // 要广播的数据内容,长度为wDataLen的值,所以这是一个不定长结构
	};

	/**
	@name         : 通知:某客户端与网关服务器端开连接
	@brief        : 数据包结构参考SMsgGatewayDisconnected结构
	*/
	// MSG_GATEWAY_DISCONNECTED
	struct SMsgGatewayDisconnected
	{
		ClientID    client;        // 断开连接的客户端
		DWORD       dwReason;      // 错误码
	};

    /**
    @name         : 请求: 和某个客户端断开连接
    @brief        : 数据包结构参考SMsgGatewayCloseConn结构
	@note         : 用途举例: 剔除作弊玩家
    */
	//  MSG_GATEWAY_CLOSE_CONN
	struct SMsgGatewayCloseConn
	{
		ClientID    client;        // 要断开连接的客户端
		DWORD       dwReason;      // 断开原因
	};

	/**
	@name         : 请求: 让网关服务器通知客户端连接另外一台服务器
	@brief        : 数据包结构参考SMsgGatewayRedirect
	@note         : 用途举例 : 由于某台网关服务器出现错误或者其他部署上的原因,可以把用户重定向到其他网关
	*/
	// MSG_GATEWAY_REDIRECT
	struct SMsgGatewayRedirect
	{
		ClientID    client;        // 客户端ID
		DWORD       dwServerIP;    // 要重定向到的服务器IP,网络字节序
		WORD        wServerPort;   // 服务器监听的端口,网络字节序
		WORD        dwReason;      // 断开原因
	};

	/**
	@name         : 告诉网关服务器,某玩家已经在场景中迁徙,其区域相关的数据需要发到新的区域服务器
	@brief        : 数据包结构参考SMsgGatewayMigrate结构
	@note         : 登陆成功后及用户在场景中移动时会触发该消息
	*/
	// MSG_GATEWAY_REQUEST_MIGRATE
    struct SMsgGatewayMigrate
	{
		ClientID    client;             // 迁移的客户端
        SessionID   session;            // session 用于校验client是否被复用
        DWORD       dwSceneServerID;    // 场景服ID
        int         nSceneID;           // 目标场景ID
        SMsgGatewayMigrate()
        {
            memset(this, 0, sizeof(*this));
        }
	};

	/**
	@name         : 回复服务器角色迁出结果
	@brief        : 数据包结构参考SMsgGatewayMigrateResult结构
	@note         : 
	*/
	// MSG_GATEWAY_MIGRATE_RESULT
	struct SMsgGatewayMigrateResult
	{
		ClientID    client;        // 迁移的客户端
		DWORD       dwError;       // 是否迁移失败,不为0则表示迁移失败
        DWORD       dwSceneServerID; // 迁移后的场景服ID
        int         nSceneID;        // 迁移后的场景ID
        DWORD       dwOldSceneServerID; // 原来的场景服ID
        int         nOldSceneID;      // 原来的场景ID
	};

	/**
	@name         : 内网的游戏服务器连接上网关后向服务器注册自己
	@brief        : 这个其实是握手信息.数据包结构见SMsgGatewayRegisterServer
	@param        :
	@return       :
	*/
	// MSG_GATEWAY_REGISTER_SERVER
	struct SMsgGatewayRegisterServer
	{
		WORD        wServerType;   // 服务器类型
        DWORD       dwServerKey;   // 该服务器在中心服务器上的ID
	};

	/**
	@name         : 网关服务器回应握手信息
	@brief        : 数据包结构见SMsgGatewayHandshakeRes
	@param        :
	@return       :
	*/
	// MSG_GATEWAY_HANDSHAKE_RESPONSE
	struct SMsgGatewayHandshakeRes
	{
		DWORD       dwGatewayID;   // 网关服务器在网关集群中的ID
		ServerID    SID;           // 本服务器在网关上的ID     
	};

	/**
	@name         : 通知:有新客户端连入
	@brief        : 数据包结构参考SMsgGatewayClientInComing结构
	*/
	// MSG_GATEWAY_CLIENT_INCOMING
	struct SMsgGatewayClientInComing
	{
		ClientID    client;        // 客户端ID
	};

	/**
	@name        : 指定服务器失去连接
	@brief       : 服务器重启后会将客户端自动转接
	*/
	// MSG_GATEWAY_SERVER_LOST_CONNECTION

	/**
	@name        : 服务器PING
	@brief       : 当突然拔掉网卡或者其他原因导致连接意外断开时(没有发送断开连接的通知),TCP连接默认会经过2小时才能发现连接断开
	@brief       : SOCKET允许设置Keepalive参数来检测断线,但是这个方法在网络上应用时参数非常难以设置,设置得不好就会导致玩家无故断线甚至连接不上服务器
	@brief       : 这里定义一个PING命令,客户端每隔一段时间发送该命令给服务器,这样服务器和客户端都能非常即时的发现连接断开,并且也能很好的统计出网络延时
	@brief       : 数据,网络延时数据对于游戏运营以及用户体验都有很重要的意义.
	*/
	// MSG_GATEWAY_PING
	struct SMsgGatewayPing
	{
		DWORD      dwLatency;         // 网络延时
		bool       bEnableCompress;   // 是否启用通讯压缩
		DWORD      dwHackIP;          // 检测外挂连接的IP是否存在,如果未0就不检测
	};

	/**
	@name        : 客户端回复PING
	@note        : PING工作的机制
	@note        : 1.服务器发一个PING包给客户端,同时告诉客户端网络延时数据
	@note        : 2.客户端回复服务器一个PING包,服务器根据发送和收到PING的时间间隔算出延时数据
	@note        : 3.客户端回复时附带一个客户端的TICK数,该数据可作为客户端是否加速检测的一个参考数据
	*/
	// MSG_GATEWAY_PING_REPLY
	struct SMsgGatewayPingReply
	{
		DWORD      dwClientTick;      // 客户端TICK数
	};

	/**
	@name        : 客户度发给网关服务器的握手包
	@note        : 用于交换一下双方的状态数据
	*/
	// MSG_GATEWAY_HANDSHAKE
	struct SMsgGatewayHandshake
	{
		bool       bIsnated;          // 是否是内网用户
		unsigned short nUDPPort;      // 该客户端的UDP监听端口 
		char   szMacAddress[MAX_MACADDR_LEN]; // 网卡地址
		DWORD		dwUCode;
	};

	/**
	@name        : P2P广播请求
	@note        : 网关服务器发给客户端，请求客户端帮助广播
	@note        : 包格式: 请求ID(4字节) + 客户端数目 + 客户端IP(1) + UDP端口(1) + ... +  客户端IP(n) + UDP端口(n) + 真正的数据
	*/
	// MSG_GATEWAY_P2PBROADCAST
	struct SMsgGatewayP2PBroadcast              
	{
		unsigned long    nRequestID;	// 请求ID
		unsigned char    nClientNum;
		unsigned long    nClientID;     //
		unsigned long    nClientIP;
		unsigned short   nClientPort;   // 0....nClientNum + data
	};

	/**
	@name        : P2P广播失败,服务器请给失败的用户重发一次
	@note        : 包格式同SMsgGatewayP2PBroadcast
	*/
	// MSG_GATEWAY_P2PBROADCAST_FAILD
	struct SMsgGatewayP2PBroadcastFaild              
	{
		unsigned long    nRequestID;	// 请求ID
		unsigned char    nClientNum;
		unsigned long    nClientID;     //
	};;

	/**
	@name        : 收到广播数据
	@note        : 客户端收到另外一个客户端发来的广播数据
	@note        : 数据标识(4字节) + 客户端IP(4字节) + 数据长度(2字节) + 真正数据
	*/
	// MSG_P2PBROADCAST_DATA
	struct SMsgP2PBroadcastData
	{
		unsigned long   nRequestID;    // 数据标识
		unsigned long   nClientIP;     // 客户端IP
		unsigned short  nDataLen;      // 数据长度
	};

	/**
	@name        : 确认收到广播包
	@noete       : 每个P2P之间的广播包将被重发3遍，直到收到为止，如果未收到则通知服务器继续发或者直接丢弃
	*/
	// MSG_P2PBROADCAST_ACK
	struct SMsgP2PBroadcastAck
	{
		unsigned long   nRequestID;    // 数据标识
		unsigned long   nClientIP;     // 客户端IP
	};

	// 网关服务器发送唯一编码给客户端			MSG_GATEWAY_UCODE
	struct SMsgGatewayUCode
	{
		DWORD dwUCode;
		DWORD dwNetworkType;	// 网络类型：1为默认值电信，2为网通
	};

	// 网关服务器关闭了，通知客户端不要重连		MSG_GATEWAY_CLOSE
	struct SMsgGatewayClose
	{
		BYTE byteClose;
	};
	// 通知客户端重定向							MSG_CLIENT_REDIRECT
	struct SMsgClientRedirect
	{
		DWORD dwServerIP;		// 要重定向到的服务器IP,网络字节序
		WORD wClientPort;        // 服务器监听的端口,网络字节序
	};

	// 通知客户端停机维护不要断线续完			MSG_CLIENT_TPLAYERNOTCONN
	struct SMsgClientTPlayerNotconn
	{
	};

	// 网关服务器发送唯一编码给客户端			MSG_GATEWAY_CONNECTSUCCESS
	struct SMsgGatewayConnectSuccess
	{
	};


	///////////////////////////////////////////////////////////////////
	// 客户端，网关，场景服，性能检测消息包 MSG_GATEWAY_CLIENT_ZONE_PERFORMANCE
	struct SMsgGatewayClientZonePerformanceNode
	{
		LONGLONG nSendTicks;		// 发包时间
		LONGLONG nRecvTicks;		// 收到时间
		LONGLONG nFrequency;		// CPU频率

		SMsgGatewayClientZonePerformanceNode(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 客户端，网关，场景服，性能检测消息包
	struct SMsgGatewayClientZonePerformanceData
	{
		SMsgGatewayClientZonePerformanceNode client;		// 客户端性能数据
		SMsgGatewayClientZonePerformanceNode toZone;		// 发场景服性能数据
		SMsgGatewayClientZonePerformanceNode toClient;		// 发客户端性能数据
		SMsgGatewayClientZonePerformanceNode zone;			// 场景服性能数据
		DWORD	nToZoneNetCounts;							// 网关发场景服网络队列包数量
		DWORD	nZoneNetCounts;								// 场景服网关连接器网络队列包数量
	};

	// 场景服->网关，性能检测消息包
	struct SMsgGatewayClientZonePerformance_S
	{
		ClientID     client;         // 客户端ID
		WORD         wDataLen;       // 数据长度
		SMsgGatewayClientZonePerformanceData perData;	// 性能检测消息包
	};
	///////////////////////////////////////////////////////////////////
	// 不登陆的玩家，供GM聊天客户端用		MSG_GATEWAY_HANDSHAKE_GMChatClient

	///////////////////////////////////////////////////////////////////
	// 不登陆的玩家，供GM聊天客户端用	MSG_GATEWAY_CLIENT_INCOMING_GMChatClient
	struct SMsgGatewayHandshakeGMChatClient
	{
		ClientID	client;
		SMsgGatewayHandshakeGMChatClient()
		{
			memset(this, 0, sizeof(*this));
		}
		SMsgGatewayHandshakeGMChatClient(const SMsgGatewayHandshakeGMChatClient& mp)
		{
			memcpy(this, &mp, sizeof(*this));
		}
	};

	// 客户端与服务器性能检测
#define MSG_COMMON_CLIENT_ZONE_PERFORMANCE			EMMSG_COMMON_CLIENT_ZONE_PERFORMANCE
	// 系统消息，客户端会弹出对话框				MSG_COMMON_MESSAGEBOX
	struct SMsgCommonMessageBox_SC
	{	
		int		nMsgBoxType;		// 消息对话框类型
		bool	bShowFlag;			// 显示还是隐藏
		int		nHtmlLen;			// HTML信息长度
		// ...............			// HTML信息
	};

	// 网关通知某个用户重连			MSG_LOGINSERVER_RECONNECT
	struct SMsgLoginServer_Reconnect
	{
		DWORD    dwClientID;              // 客户端ID(网关)
		DWORD    dwServerID;              // 服务器ID
	};
};

#pragma pack()

#endif	//__COMMON_GATEWAY_SERVICE_DEF_H__