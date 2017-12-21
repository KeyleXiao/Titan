/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceGateway\Src\GlobalVoiceGateway.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音网关
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "net.h"
#include "TimerAxis.h"
#include "ClientList.h"
#include "AudioList.h"
#include "Acceptor.h"
#include "Reactor.h"
#include "Trace.h"
#include "AsynIoFrame.h"
#include "net/producer_consumer_queue.h"
using namespace rkt;

#include "IGlobalVoiceGateway.h"
#include "VoiceGatewaySetting.h"
#include <string>

#include "IManagerConnector.h"
using namespace Manager;

#include "VoiceConnector.h"


//相关统计数据结构
struct SVoiceTotalInfo	
{
	DWORD dwRecvSpeedServer;		// 服务器收包速度
	DWORD dwSendSpeedServer;		// 服务器发包速度
	DWORD dwRecvSpeedClient;		// 客户端收包速度
	DWORD dwSendSpeedClient;		// 客户端发包速度

	DWORD dwAudioPort;		// 语音端监听端口
	DWORD dwClientPort;		// 客户端监听端口
	DWORD dwClients;		// 客户端数
	DWORD dwVoiceClients;	// 语音通道客户端数
	DWORD dwServers;		// 软件服务器数
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

	DWORD dwGatewayID;		// 网关ID

	SVoiceTotalInfo()
	{
		memset(this, 0, sizeof(*this));
	};

	void clear(void)
	{
		memset(this, 0, sizeof(*this));
	};
};


class GlobalVoiceGateway : public IGlobalVoiceGateway
						 , public TimerHandler
						 , public EventHandler
						 , public IManagerMessageHandler
						 , public IManagerEventHandler
						 , public IAcceptorHandler
{
	enum
	{
		ETimerEventID_ReconnectNet = 0,
		ETimerEventID_CheckTimeOut,
		ETimerEventID_Keepalive,

		ETimerEventID_VoiceNetCount,
		ETimerEventID_VoiceControl,
	};

public:
	GlobalVoiceGateway();

	virtual ~GlobalVoiceGateway();

	// 初始化启动环境
	bool InitEnvironment(void);

	// 反初始化环境
	bool UnInitEnvironment(void);

	/**
	@name                    : 初始化网络
	@brief                   :
	@param dwID				 : 网关ID
	@param szServerIP		 : 服务器IP
	@param wServerPort		 : 服务器端口
	@param wClientListenPort : 客户端监听端口
	@return                  :
	*/
	bool Create( DWORD dwID, const char * szServerIP, WORD wServerPort, WORD wClientListenPort );

	bool Close();

	// 是否初始化
	bool IsInit(void);

	// 是否启动
	bool IsStart(void);

	/// 关闭服务器程序
	void CloseServerApp(void);

	/// 启动管理连接器
	void CreateManagerConnector(void);

	/// 关闭管理连接器
	void ReleaseManagerConnector(void);

	// 设置当前操作代码  DWORD dwCode 参考 EMManagerControlCode
	virtual void SetControlCode(DWORD dwCode);

	// 取得网关ID
	virtual DWORD GetGatewayID(void);

	// 执行操作代码
	bool DoControlCodeTask(DWORD dwCode);

	/** 载入配置config.xml信息,在程序启动时加载
	@param   
	@return  
	@note     
	@retval  
	*/ 
	bool LoadConfigInfo();

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

	
	///////////////////////// IAcceptorHandler ///////////////////////////////////
	virtual void OnAccept( WORD wListenPort,IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler );


	////////////////////////// EventHandler ///////////////////////////
	virtual void OnEvent( HANDLE event );

	
	//////////////////////////TimerHandler///////////////////////////
	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	virtual void OnTimer(unsigned long dwTimerID);

	virtual TimerAxis* getTimerAxis() { return &m_TimerAxis; }

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


	/// 客户端网络消息处理
	virtual void onClientMessage(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	/// 服务器网络消息处理
	virtual void onServerMessage(ulong actionId, SGameMsgHead* head, void* data, size_t len);

	/// 转发网络消息到语音服务器
	virtual void onTransVoiceServer(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	/// 语音通道客户端网络消息处理
	virtual void onAudioClientMessage(CAudioUser &audio, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	/// 语音通道转发网络消息到语音服务器
	virtual void onAudioTransVoiceServer(CAudioUser &audio, ulong actionId, SGameMsgHead* head, void* data, size_t len);


	// 客户端连入
	virtual void OnClientUserEnter(CClientUser *pUser, DWORD dwReason = 0);

	// 用户断线
	virtual void OnClientUserLeave(CClientUser * pUser, DWORD dwReason = 0);


	// 更新语音网关客户端到语音服务器，nUpdateType:更新类型 EMVoiceClientUpdateType，dwClientID:要更新信息的客户端ID
	void UpdateClientInfoToServer( BYTE nUpdateType, DWORD dwClientID );

private:

	// 命令操作
	void ExcutCommand( int nNum1, int nNum2, const char * szTxt );

	/////////////  Client 网络消息 /////////////////////////////////////////////////////////////

	// 接收客户端握手消息
	void OnMsgClientHandshake(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// purpose: 语音通道客户端握手消息
	void OnMsgAudioClientHandshake(CAudioUser &audio, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 请求性能检测消息包
	void OnMsgClientPerformance(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 请求桥服务器网关服务发送数据给各软件服务器消息
	void OnMsgClientSendData(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 请求桥服务器网关服务广播数据给各软件服务器
	void OnMsgClientBroadcastData(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	/////////// VoiceServer 网络消息  ///////////////////////////////////////////////////////////////
	
	// 握手消息
	void OnMsgServerHandshakeResponse(ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 请求桥服务器网关服务发送数据给各软件服务器消息
	void OnMsgServerSendData(ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 请求桥服务器网关服务广播数据给各软件服务器
	void OnMsgServerBroadcastData(ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 语音网关子消息
	void OnMsgServerSubMsg(ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 请求性能检测消息包
	void OnMsgServerPerformance(ulong actionId, SGameMsgHead* head, void* data, size_t len);

	//////////////////////////////////////////////////////////////////////////

public:

	IManagerConnector			*m_ManagerConnector;			// 服务器管理连接器

	VoiceConnector				m_VoiceConnector;				// 语音服务器连接器

	TimerAxis                   m_TimerAxis;				// 时间轴

	Acceptor					m_ClientListener;			// 客户端监听服务器连接

	Acceptor					m_AudioListener;			// 语音通道监听连接
	DWORD						m_dwAudioListenPort;		// 语音通道监听端口
	bool						m_bOpenAudioConnect;		// 是否开放语音通道

	DWORD						m_dwMaxConnection;			// 最大连接数

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

	//相关统计数据结构
	SVoiceTotalInfo	m_TotalInfo;

	DWORD m_dwRecvSpeedServer;		// 服务器收包速度
	DWORD m_dwSendSpeedServer;		// 服务器发包速度
	DWORD m_dwRecvSpeedClient;		// 客户端收包速度
	DWORD m_dwSendSpeedClient;		// 客户端发包速度

	// 游戏世界ID分类列表,以dwWorldID为关键字
	//TMAP_VoiceWorldIDInfo m_mapWorldList;

	bool		m_bUpdate;		// 数据更新锁
	bool		m_bNewDataCome;	// 是否有更新
	int			m_nNum1;		// 参数1
	int			m_nNum2;		// 参数2
	std::string m_strTxt;		// 参数3
private:

	DWORD				m_dwGetwayID;	//  网关ID
	// 当前操作代码
	DWORD				m_dwControlCode;

	// 是否初始化
	bool m_bInit;
	// 是否启动
	bool m_bStart;

	bool m_bIsAnswer;						// 是否有回应
	bool m_bIsSendDeConnectAlert;			// 是否已发断线报警
	DWORD m_dwLastAnswerTime;				// 最后收到心跳Tick时间

	bool m_bReady;							// 语音网关是否与语音服务器已经握手完毕


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
_TCHAR * GetServerTypeString(DWORD dwServerType);