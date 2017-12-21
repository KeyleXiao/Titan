/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\GlobalVoiceServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音服务器
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "net.h"
#include "IDBEngine.h"
#include "TimerAxis.h"
#include "ClientList.h"
#include "ServerList.h"
#include "GatewayList.h"
#include "ChannelList.h"
#include "ChatList.h"
#include "Acceptor.h"
#include "Reactor.h"
#include "Trace.h"
#include "AsynIoFrame.h"
#include "net/producer_consumer_queue.h"
using namespace rkt;

#include "IVoiceDataBaseDef.h"
#include "IGlobalVoiceServer.h"
#include "VoiceServerSetting.h"
#include "GameDatabaseAccessor.h"
#include <string>

#include "IManagerConnector.h"
using namespace Manager;

// 使用用户列表缓冲机制
//#define VOICE_CLIENTID_BUFFLIST


//相关统计数据结构
struct SVoiceTotalInfo	
{
	DWORD dwRecvSpeed;		// 收包速度
	DWORD dwSendSpeed;		// 发包速度

	DWORD dwRecvSpeedServer;		// 服务器收包速度
	DWORD dwSendSpeedServer;		// 服务器发包速度
	DWORD dwRecvSpeedGateway;		// 网关收包速度
	DWORD dwSendSpeedGateway;		// 网关发包速度
	DWORD dwRecvSpeedClient;		// 客户端收包速度
	DWORD dwSendSpeedClient;		// 客户端发包速度
	DWORD dwSendAvgSpeedClient;		// 客户端平均发包速度

	DWORD dwClientPort;		// 客户端监听端口
	DWORD dwClients;		// 客户端数
	DWORD dwServers;		// 软件服务器数
	DWORD dwGateways;		// 网关数
	DWORD dwWorldCounts;	// 游戏世界数
	DWORD dwServerPort;		// 服务器监听端口
	DWORD dwStartTime;		// 启动时间
	DWORD dwGameID;			// 游戏ID
	DWORD dwAreaID;			// 游戏区域ID
	DWORD dwWorldID;		// 游戏世界ID
	bool bIsPublic;			// 是否公共游戏世界
	char szGameName[MANAGER_NAME_MAXSIZE];		// 游戏名称
	char szAreaName[MANAGER_NAME_MAXSIZE];		// 游戏区域名称
	char szWorldName[MANAGER_NAME_MAXSIZE];		// 游戏世界名称

	SVoiceTotalInfo()
	{
		memset(this, 0, sizeof(*this));
	};

	void clear(void)
	{
		memset(this, 0, sizeof(*this));
	};
};


// 本地序号ID与ID对应表,以dwID为关键字
typedef map<DWORD,DWORD> TMAP_VoiceID2IDInfo;

//游戏世界信息数据结构
struct SVoiceWorldNode : public IVoiceServerWorldInfo
{
	DWORD dwWorldID;				// 游戏世界ID
	string strWorldName;			// 游戏世界名称
	TMAP_VoiceID2IDInfo mapList;	// 服务器ID对应列表

	SVoiceWorldNode(void){dwWorldID = 0;}
	virtual ~SVoiceWorldNode(void){ }
	// 游戏世界ID
	virtual DWORD GetWorldID(void){ return dwWorldID;}
	// 游戏世界名称
	virtual const char * GetWorldName(void)	{ return strWorldName.c_str();}
	// 服务器个数
	virtual DWORD GetServerCounts(void)	{ return mapList.size();}
};


// 游戏世界ID分类列表,以dwWorldID为关键字
typedef map<DWORD,SVoiceWorldNode> TMAP_VoiceWorldIDInfo;


/////////////////////// 频道封PDBID 结构 ///////////////////////////////////////////////////
#define VOICE_BAN_PDBID_TIME	3600		// 被封时间 60分钟

// 频道I禁言信息
struct SVoiceChannelBanInfo
{
	DWORD dwBanTime;
};

// 频道ID为键值，禁言失效时间
typedef map<LONGLONG, SVoiceChannelBanInfo> TMAP_VoiceChannelBanList;

// 被禁PDBID列表, PDBID为键值
typedef map<DWORD, TMAP_VoiceChannelBanList> TMAP_VoiceBanPBID;

// 召集粉丝信息
struct SVoiceCallFansInfo
{
	DWORD	dwLastCallFansTime;
	DWORD	dwFansAcceptCount;		// 召集之后，粉丝接受邀请的个数
	DWORD	dwFansCancelCount;		// 召集之后，粉丝取消关注的个数
};

// 邀请粉丝记录 UDBID为键，邀请粉丝时间为值
typedef map<DWORD, SVoiceCallFansInfo> TMAP_VoiceCallFans;


// 语音召集令信息
struct SVoiceADInfo
{
	DWORD dwLastADTime;	// 邀请时间
};

// 召集信息，频道类型为键
typedef map<BYTE, SVoiceADInfo> TMAP_ADInfo;

// 语音召集令记录 UDBID为键, 邀请粉丝时间为值
typedef map<DWORD, TMAP_ADInfo> TMAP_VoiceAD;


// 自动流量控制中的频道列表
typedef map<LONGLONG, DWORD> TMAP_VoiceAutoNetSpeedList;


class GlobalVoiceServer : public IGlobalVoiceServer
						, public TimerHandler
						, public EventHandler
						, public IManagerMessageHandler
						, public IManagerEventHandler
						, public IAcceptorHandler
						, public IDBRetSink
{
	enum
	{
		ETimerEventID_ReconnectNet = 0,		// 重连
		ETimerEventID_CheckTimeOut,			// 检测超时
		ETimerEventID_Keepalive,			// 检查心跳

		ETimerEventID_VoiceNetCount,		// 统计网络流量
		ETimerEventID_VoiceControl,			// 运行窗口控制命令
		ETimerEventID_CheckBanPDBID,		// 检测PDBID禁言
		ETimerEventID_DBStart,				// 启动时数据库处理

		ETimerEventID_WaitShutDown,			// 等待数据库保存完成后关闭
		ETimerEventID_WindowClose,			// 窗口关闭定时器

		ETimerEventID_CheckCallFansInfo,	// 检测粉丝邀请信息

		ETimerEventID_GetFansTopList,		// 获取粉丝排行榜
		ETimerEventID_CheclAD,				// 检测语音召集令信息
		
		ETimerEventID_CheckUBCommanderSpeak, // 检测统战发言时间
	};

public:
	GlobalVoiceServer();

	virtual ~GlobalVoiceServer();

	// 初始化启动环境
	bool InitEnvironment(void);

	// 反初始化环境
	bool UnInitEnvironment(void);

	/**
	@name                    : 初始化网络
	@brief                   :
	@param wServerListenPort : 服务器监听端口
	@param wClientListenPort : 客户端监听端口
	@return                  :
	*/
	bool Create(WORD wServerListenPort,WORD wClientListenPort);

	bool Close();

	// 真正关闭服务器进程
	void ShutDownApp(void);

	// 是否初始化
	bool IsInit(void);

	// 是否已启动
	bool IsStart(void);

	/// 关闭服务器程序
	void CloseServerApp(void);

	/// 启动管理连接器
	void CreateManagerConnector(void);

	/// 关闭管理连接器
	void ReleaseManagerConnector(void);

	// 设置当前操作代码  DWORD dwCode 参考 EMManagerControlCode
	virtual void SetControlCode(DWORD dwCode);

	// 执行操作代码
	bool DoControlCodeTask(DWORD dwCode);

	/** 载入配置config.xml信息,在程序启动时加载
	@param   
	@return  
	@note     
	@retval  
	*/ 
	bool LoadConfigInfo();

	
	//////////////////////////////////////////////////////////////////////////
	// 封PDBID
	bool BanPDBID( LONGLONG nChannelID, DWORD dwPDBID, DWORD dwTime = 0 );

	// 取消封PDBID
	bool UnbanPDBID( LONGLONG nChannelID, DWORD dwPDBID );

	// 是否被封，dwTimeLeft 返回剩余时间
	bool IsPDBIDBan( LONGLONG nChannelID, DWORD dwPDBID, DWORD & dwTimeLeft );

	// 增加一条邀请粉丝记录
	bool AddCallFansInfo( DWORD dwUDBID, DWORD dwCallTime );

	// 获取邀请粉丝记录
	SVoiceCallFansInfo * GetCallFansInfo( DWORD dwUDBID );

	// 更新邀请粉丝信息
	bool UpdateCallFansInfo( DWORD dwUDBID, DWORD dwFansAcceptCount, DWORD dwFansCancelCount );

	// 增加一条语音召集令记录
	bool AddADInfo( DWORD dwUDBID, DWORD dwChannelType, DWORD dwCallTime );

	// 设置召集信息给用户
	void SetADInfoToUser( CClientUser & user );

	////////// IManagerEventHandler 处理子服务器连接器执行事件 ///////////////////////////////

	/** 处理子服务器连接器执行事件
	@param   wEventID ：事件ID
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@retval buffer 
	*/
	virtual void OnManagerEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen);

	////////// IManagerMessageHandler 处理子服务器发过来的消息 //////////////////
	/**
	@name         : 处理其他服务器通过子服务器转发的消息
	@brief        : 具体内容不清楚
	@param actionId  : 消息码
	@param head  : 消息头
	@param data  : 消息内容
	@param len:消息长度
	*/
	virtual void HandleManagerMessage(ulong actionId, SManagerMsgHead* head, LPSTR data, size_t len);


	///////////// IAcceptorHandler /////////////////////////////////////////////////////////////
	virtual void OnAccept( WORD wListenPort,IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler );


	///////////// EventHandler /////////////////////////////////////////////////////////////
	virtual void OnEvent( HANDLE event );

	//////////////////////////////////IDBRetSink///////////////////////
	/** 数据库请求返回回调方法
	@param   nCmdID ：命令ＩＤ
	@param   nDBRetCode：数据库请求返回值，参考上面定义
	@param   pszDBRetDesc：数据库请求返回描述，由ＳＰ返回
	@param   nQueueIndex：队列定义
	@param   pOutData：输出数据
	@param   nOutLen：输出数据长度
	@return  
	@note     
	@warning 此对像千万不能在数据库返回前释放，否则会非法！
	@retval buffer 
	*/	
	virtual void	OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	////////////////////////// TimerHandler ///////////////////////////
	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	virtual void OnTimer(unsigned long dwTimerID);

	/// purpose: 获得时间轴
	virtual TimerAxis* getTimerAxis() { return &m_TimerAxis; }

	/// 获取事件引擎
	virtual IEventEngine* getEventEngine()  { return m_EventEngine; }

	/** 启用定时器
	@param   
	@param   
	@return  
	*/ 
	virtual bool SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo, DWORD callTimes = INFINITY_CALL);

	/** 销毁定时器
	@param   
	@param   
	@return  
	*/
	virtual bool KillTimer(DWORD timerID, ITimerHandler * handler);

	/// 服务器网络消息处理
	virtual void onServerMessage(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	/// 语音网关消息处理
	virtual void onGatewayMessage(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	/// 客户端网络消息处理
	virtual void onClientMessage(GatewayUser &server, CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len);
	

	// 发送数据给指定服务器
	virtual bool SendDataToServer(DWORD dwServerID,LPVOID pData,DWORD dwDataLen);

	// 广播数据给服务器 pServerArray==NULL && wServerNum==VOICE_BROADCASTALL_NUM 广播所有
	virtual bool BroadcastToServer(DWORD * pServerArray,WORD wServerNum,LPVOID pData,DWORD dwDataLen);


	// 发数据给指定语音网关 dwGatewayID:语音网关ID 
	virtual bool SendDataToGateway(DWORD dwGatewayID,const char * pData,DWORD dwLen,bool bZeroBroadcast=false);

	// 广播数据给指定列表语音网关 pServerArray:语音网关ID列表,wClientNum:列表个数 pServerArray==NULL && wGatewayNum==VOICE_BROADCASTALL_NUM 广播所有
	virtual bool BroadcastDataToGateway(DWORD * pGatewayArray,WORD wGatewayNum,const char * pData,DWORD dwLen);

	// 发数据给指定游戏客户端 dwClientID:客户端ID
	virtual bool SendDataToClient( DWORD dwClientID,const char * pData,DWORD dwLen, BYTE nSendChennel = VoiceSendChannel_Control );

	// 广播数据给指定列表游戏客户端 pServerArray:客户端ID列表,wClientNum:列表个数, dwBanClientID 要排除的
	virtual bool BroadcastDataToClient(DWORD * pClientArray,DWORD wClientNum,const char * pData,DWORD dwLen, DWORD dwBanClientID=0, BYTE nSendChennel = VoiceSendChannel_Control);


	// 游戏服务器断线
	virtual void OnServerUserLeave(ServerUser *pServer);

	// 语音网关断线
	virtual void OnGatewayUserLeave(GatewayUser *pServer, DWORD dwReason = 0);

	// 客户端连入
	virtual void OnClientUserEnter(CClientUser *pUser, DWORD dwReason = 0);

	// 客户端断线
	virtual void OnClientUserLeave(CClientUser * pUser, DWORD dwReason = 0);

	// 客户端登录
	virtual void OnClientUserLogin(CClientUser *pUser, DWORD dwReason = 0);

	// 客户端登出
	virtual void OnClientUserLogout(CClientUser * pUser, DWORD dwReason = 0);

	// 频道创建
	virtual void OnChannelCreate( CChannel *pChannel, DWORD dwReason = 0);

	// 频道销毁
	virtual void OnChannelDestroy( CChannel *pChannel, DWORD dwReason = 0);

	// 用户进入房间
	virtual void OnUserEnterRoom( CRoom *pRoom, CClientUser* pUser, DWORD dwReason = 0);

	// 用户退出房间
	virtual void OnUserLeaveRoom( CRoom *pRoom, CClientUser* pUser, DWORD dwReason = 0);



	// 发送语音模块数据给指定游戏客户端 dwClientID:客户端ID 
	virtual bool SendVoiceDataToClient(DWORD dwClientID,WORD byKeyAction,DWORD dwMsgCode,DWORD dwUserData1=0,DWORD dwUserData2=0,DWORD dwUserData3=0,const char * pData=0,DWORD dwLen=0, BYTE nSendChennel = VoiceSendChannel_Control);

	// 广播语音模块数据到客户端 pClientArray: 要发送的客户端ID数组,wClientNum  : 客户端数目
	virtual bool BroadcastVoiceDataToClient(DWORD * pClientArray,DWORD wClientNum,WORD byKeyAction,DWORD dwMsgCode,DWORD dwUserData1=0,DWORD dwUserData2=0,DWORD dwUserData3=0,const char * pData=0,DWORD dwLen=0, BYTE nSendChennel = VoiceSendChannel_Control);

	// 发送聊天信息到客户端
	virtual bool BroadcastChatMsgToClient(DWORD * pClientArray,DWORD wClientNum, WORD nPos, WORD nTipsType, const char * pMsg, DWORD dwUserData1=0,DWORD dwUserData2=0,DWORD dwUserData3=0 );



	/// 发送数据到指定的游戏服务器
	virtual bool SendDataToGameServer(DWORD dwServerID, BYTE nDestEndPoint,WORD byKeyModule,WORD byKeyAction,DWORD dwMsgCode,DWORD dwUserData1=0,DWORD dwUserData2=0,DWORD dwUserData3=0,const char * pData=0,DWORD dwLen=0);

	// 广播数据到指定的游戏服务器
	virtual bool BroadcastDataToGameServer(DWORD * pServerArray, WORD wServerNum,BYTE nDestEndPoint,WORD byKeyModule,WORD byKeyAction,DWORD dwMsgCode,DWORD dwUserData1=0,DWORD dwUserData2=0,DWORD dwUserData3=0,const char * pData=0,DWORD dwLen=0);


	
	// 发送语音模块数据给指定语音网关
	virtual bool SendVoiceDataToGateway(DWORD dwGatewayID,WORD byKeyAction,DWORD dwMsgCode,DWORD dwUserData1=0,DWORD dwUserData2=0,DWORD dwUserData3=0,const char * pData=0,DWORD dwLen=0);




	////// 常用服务器列表相关接口 ////////////////////////////////////////////////////////////////////

	/**  取得服务器ID列表
	@param   DWORD* pServerArray	: 保存找到的ID数组
	@param   WORD dwArrayLen		: 数组长度
	@param   WORD &dwGetCounts		: 找到的个数
	@param   DWORD dwWorldID		: 游戏世界ID，0为全部
	@param   WORD wServerType		: 服务器类型,0为全部
	@return  找到的个数
	*/
	virtual DWORD GetGameServerIDList(DWORD * pServerArray,DWORD dwArrayLen,DWORD &dwGetCounts,DWORD dwWorldID=0,WORD wServerType=0);

	/**  取得指定游戏世界的指定类型的服务器ID
	@param   DWORD dwWorldID		: 游戏世界ID
	@param   WORD wServerType		: 服务器类型
	@param   WORD wSubID			: 服务器序号
	@return  找到的服务器ID,为0表示没找到
	*/
	virtual DWORD GetGameServerIDByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID);

	/** 列出所有服务器信息
	@param   DWORD dwWorldID		: 游戏世界ID
	@param   WORD wServerType		: 服务器类型
	@return 
	*/
	virtual void PrintGameServerListInfo(DWORD dwWorldID=0,WORD wServerType=0);

	////// 常用游戏世界列表相关接口 ////////////////////////////////////////////////////////////////////

	/**  取得游戏世界ID列表
	@param   DWORD* pWorldArray	: 保存找到的ID数组
	@param   WORD dwArrayLen		: 数组长度
	@param   WORD &dwGetCounts		: 找到的个数
	@return  找到的个数
	*/
	virtual DWORD GetGameWorldIDList(DWORD * pWorldArray,DWORD dwArrayLen,DWORD &dwGetCounts);

	/**  取得指定游戏世界信息
	@param   DWORD dwWorldID		: 游戏世界ID
	@return  找到的服务器ID,为0表示没找到
	*/
	virtual IVoiceServerWorldInfo * GetGameWorldInfo(DWORD dwWorldID);

	/** 列出所有游戏世界信息
	@return 
	*/
	virtual void PrintGameWorldListInfo();

	
	// 踢掉用户
	virtual void KickUser( CClientUser *pClientUser, DWORD dwReason = 0 );


	////////// 内部公用 ////////////////////////////////////////////////////////////////

	LPCSTR GetKickReasonText( DWORD dwReason );


private:

	// 命令操作
	void ExcutCommand( int nNum1, int nNum2, const char * szTxt );

	////////////// Server ////////////////////////////////////////////////////////////

	// 握手消息
	void OnMsgServerHandshake(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 请求桥服务器网关服务发送数据给各软件服务器消息
	void OnMsgServerSendData(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 请求桥服务器网关服务广播数据给各软件服务器
	void OnMsgServerBroadcastData(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 添加蓝钻
	void OnMsgServerAddVoiceBlueDiamond( ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len );

	// 服务器请求广播数据到客户端
	void OnMsgServerBroadcastDataToClient( ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len );

	///////////////// SocialServer /////////////////////////////////////////////////////////

	// 服务器请求登录
	void OnMsgSocialServerLogin( ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len );

    // 游戏服更新语音用户信息
    void OnMsgUserInfoUpdate(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 社会服发送私聊邀请
	void OnMsgSocialServerChatInvite( ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len );

	// 玩家从社会服下线
	void OnMsgSocialNotifyActorLogout(ServerUser& server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	///////////////// Gateway /////////////////////////////////////////////////////////

	// 握手消息
	void OnMsgGatewayHandshake(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 语音网关转发而来的客户端消息
	void OnMsgGatewaySendData(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 语音网关同步客户端信息到语音服务器
	void OnMsgGatewayClientListUpdate(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 请求性能检测消息包
	void OnMsgGatewayPerformance(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	
	///////////////// Client /////////////////////////////////////////////////////////

	/// 客户端登录
	void OnMsgClientLogin(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 语音客户端子消息
	void OnMsgClientVoiceSubMsg(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 客户端发来语音数据
	void OnMsgClientVoiceData( CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len );

	//////////////////////////////////////////////////////////////////////////

	// 更新服务器列表,dwServerID:发向的服务器ID,为0时广播到所有服务器,nUpdateType:更新类型 EMVoiceServerListUpdateType,dwUpdateServerID:要更新信息的服务器ID
	void UpdateServerList(DWORD dwServerID,BYTE nUpdateType,DWORD dwUpdateServerID);

	// 更新游戏世界列表,dwServerID:发向的服务器ID,为0时广播到所有服务器,nUpdateType:更新类型 EMVoiceServerListUpdateType,dwUpdateWorldID:要更新信息的游戏世界ID
	void UpdateWorldList(DWORD dwServerID,BYTE nUpdateType,DWORD dwUpdateWorldID);

	// 更新世界列表到客户端
	void UpdateWorldListToClient( DWORD * pClientArray, DWORD wClientNum );

	// 更新粉丝排行榜到客户端
	void UpdateFansTopListToClient( DWORD dwClientID );

	// 定时检测是否可以关闭语音服务器
	void OnTimerCheckShutDown(void);

    // 加载DB
    void LoadMemoryDB(void* pDataBuff, size_t nSize);


public:

	IManagerConnector * m_ManagerConnector;		// 服务器管理连接器
	IEventEngine *	m_EventEngine;				// 事件引擎

	TimerAxis                   m_TimerAxis;				// 时间轴
	Acceptor					m_ServerListener;			// 监听服务器连接
	Acceptor					m_ClientListener;			// 网关连接

	HANDLE                      m_hTimer;					// 定时触发的事件

	// 是否已加载数据
	bool	m_bLoaded;

	// 关闭程序标识
	bool	m_bCoseAppFlag;

	DWORD m_dwLastErrorEndPoint;
	DWORD m_dwLastErrormoduleId;

	DWORD m_dwTotalUseTimes;
	DWORD m_dwStartTime;
	DWORD m_dwEndTime;
	DWORD m_dwTotalAdd;

	char szTraceFileName[MAX_PATH];
	char szCrashFileName[MAX_PATH];
	char szDumpFileName[MAX_PATH];
	// 取得本地路径
	char m_szWorkDir[MAX_PATH];
	// 取得服务器程序Bin路径
	char m_szServerBinWorkDir[MAX_PATH];

	DWORD m_dwMaxServerID;

	// 服务器监听端口
	WORD m_wServerListenPort;
	// 公共游戏世界ID
	DWORD m_dwPublicWorldID;

	DWORD m_dwMaxConnection;		// 最大连接数

	//相关统计数据结构
	SVoiceTotalInfo	m_TotalInfo;

	DWORD m_dwRecvSpeedServer;		// 服务器收包速度
	DWORD m_dwSendSpeedServer;		// 服务器发包速度

	DWORD m_dwRecvSpeedGateway;		// 网关收包速度
	DWORD m_dwSendSpeedGateway;		// 网关发包速度

	DWORD m_dwRecvSpeedClient;		// 客户端收包速度
	DWORD m_dwSendSpeedClient;		// 客户端发包速度

	// 游戏世界ID分类列表,以dwWorldID为关键字
	TMAP_VoiceWorldIDInfo m_mapWorldList;

	bool		m_bUpdate;		// 数据更新锁
	bool		m_bNewDataCome;	// 是否有更新
	int			m_nNum1;		// 参数1
	int			m_nNum2;		// 参数2
	std::string m_strTxt;		// 参数3

	char m_szSendBuf[MAX_NET_PACKAGE_SIZE];			// 16K的发送临时内存 外部用
    char m_szDataBuf[MAX_NET_PACKAGE_SIZE];
    char m_szNameBuf[MAX_NET_PACKAGE_SIZE];

	DWORD m_dwClientIDBuff[VOICE_MAX_ROOM_USERCOUNT];	// 临时用户ID列表，必须用VOICE_MAX_ROOM_USERCOUNT做下标 转发语音专用
	DWORD m_dwMergeIDBuff[VOICE_MAX_ROOM_USERCOUNT];	// 临时用户ID列表，必须用VOICE_MAX_ROOM_USERCOUNT做下标 合并用

	// 粉丝排行榜
	list<SVoiceFansTopListData> m_FansTopList;

	// 粉丝排行榜更新时间
	DWORD m_dwFansTopListUpdateTime;

private:


	// 当前操作代码
	DWORD				m_dwControlCode;

	// 是否初始化
	bool m_bInit;
	// 是否启动
	bool m_bStart;

	bool m_bIsAnswer;						// 是否有回应
	bool m_bIsSendDeConnectAlert;			// 是否已发断线报警
	DWORD m_dwLastAnswerTime;				// 最后收到心跳Tick时间

	// 用数组处理，性能比原来map加vector提高 14倍
	DWORD m_SendClientArray[VOICE_MAX_GATEWAYID][VOICE_MAX_CLIENT_COUNT];	// 分类的客户端ID
	DWORD m_SendCountsArray[VOICE_MAX_GATEWAYID+1];	// 各分类客户端ID数

	// 被禁PDBID列表
	TMAP_VoiceBanPBID m_mapBanPDBID;

	// 数据库状态 0:未初始化,1:启动,2:已关闭
	DWORD m_dwDBInitState;

	// 正在关服中
	bool m_bShutDowning;

	// 邀请粉丝信息列表
	TMAP_VoiceCallFans m_mapCallFans;

	// 语音召集令信息列表
	TMAP_VoiceAD m_mapAD;

	// 自动流量控制中的频道列表
	TMAP_VoiceAutoNetSpeedList m_mapAutoNetSpeed;

	// 是否启动了自动流量控制
	bool m_bAutoNetSpeed;

};


std::string GetLastErrorString(DWORD dwErrorCode);

/** 取得字节大小显示字串
@param  ULONGLONG dwSize:字节大小
@return  LPCSTR
*/	
LPCSTR GetFileSizeString(ULONGLONG dwSize);

/** 取得剩余时间字串
@param  char * szBuf:字串buffer
@param  int nBufSize:字串buffer长度
@param  DWORD nRemainSeconds:剩余时间(秒)
@return  
*/	
void GetRemainTimeString(char * szBuf,int nBufSize,DWORD nRemainSeconds);

/** 取得剩余时间字串
@param  DWORD nRemainSeconds:剩余时间(秒)
@return  LPCSTR
*/	
LPCSTR GetRemainTimeString(DWORD nRemainSeconds);

// 取得时间字串
LPCSTR GetTimeString(DWORD dwTime);

// 取得服务器类型名称
LPCSTR GetServerTypeString(DWORD dwServerType);

// 获取一个频道的配置
const SVoiceChannelConfig & GetChannelConfig( DWORD dwChannelType );

// 获取一个房间的配置
const SVoiceRoomConfig & GetRoomConfig( DWORD dwChannelType, DWORD dwRoomID );

// 根据用户类型获取名称
LPCSTR GetNameByUserType( DWORD dwType );

// 把一个ClientID列表合并到一个数组中，去掉重复的
// 把pClientIDIn里的ID合并到pClientIDArray中
// dwIDArrayLen pClientIDArray的值个数
// 此处借用TMAP_VoiceID2IDInfo定义
// 返回合并后的长度
DWORD MergeClientIDList( DWORD * pClientIDArray, DWORD dwIDArraySize, DWORD dwIDArrayCount, TMAP_VoiceID2IDInfo & TmpCheckList, DWORD * pClientIDIn, DWORD dwIDCountIn, bool bCheck = false );
