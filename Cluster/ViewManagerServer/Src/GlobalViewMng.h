/*******************************************************************
** 文件名:	GlobalViewMng.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/22
** 版  本:	1.0
** 描  述:	观战管理服
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "net.h"
#include "TimerAxis.h"
#include "Acceptor.h"
#include "Reactor.h"
#include "Trace.h"
#include "AsynIoFrame.h"
#include "net/producer_consumer_queue.h"
using namespace rkt;

#include "IGlobalViewMng.h"
#include "ViewMngSetting.h"
#include <string>

#include "IManagerConnector.h"
using namespace Manager;


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
	DWORD dwClients;		// ViewServer数
	DWORD dwServers;		// 游戏服务器数
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



class GlobalViewMng : public IGlobalViewMng
						, public TimerHandler
						, public EventHandler
						, public IManagerMessageHandler
						, public IManagerEventHandler
						, public IAcceptorHandler
{
	enum
	{
		ETimerEventID_ReconnectNet = 0,		// 重连
		ETimerEventID_CheckTimeOut,			// 检测超时
		ETimerEventID_Keepalive,			// 检查心跳

		ETimerEventID_VoiceNetCount,		// 统计网络流量
		ETimerEventID_VoiceControl,			// 运行窗口控制命令
		ETimerEventID_CheckBanPDBID,		// 检测PDBID禁言

		ETimerEventID_WaitShutDown,			// 等待数据库保存完成后关闭
		ETimerEventID_WindowClose,			// 窗口关闭定时器

		ETimerEventID_CheckCallFansInfo,	// 检测粉丝邀请信息

		ETimerEventID_GetFansTopList,		// 获取粉丝排行榜
		ETimerEventID_CheclAD,				// 检测语音召集令信息
		
		ETimerEventID_CheckUBCommanderSpeak, // 检测统战发言时间
	};

public:
	GlobalViewMng();

	virtual ~GlobalViewMng();

	// 初始化启动环境
	bool InitEnvironment(void);

	// 反初始化环境
	bool UnInitEnvironment(void);

	/**
	@name               : 初始化网络
	@brief              :
	@param wPortForGame	: 监听游戏服务器的端口
	@param wPortForGate : 监听Gate的端口
	@param wPortForView	: 监听ViewServer的端口
	@return             :
	*/
	bool Create(WORD wPortForGame,WORD wPortForGate, WORD wPortForView);

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

	///// 语音网关消息处理
	//virtual void onGatewayMessage(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	///// 客户端网络消息处理
	//virtual void onClientMessage(GatewayUser &server, CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len);
	

	//// 发数据给指定语音网关 dwGatewayID:语音网关ID 
	//virtual bool SendDataToGateway(DWORD dwGatewayID,const char * pData,DWORD dwLen,bool bZeroBroadcast=false);

	// 广播数据给指定列表语音网关 pServerArray:语音网关ID列表,wClientNum:列表个数 pServerArray==NULL && wGatewayNum==VOICE_BROADCASTALL_NUM 广播所有
	virtual bool BroadcastDataToGateway(DWORD * pGatewayArray,WORD wGatewayNum,const char * pData,DWORD dwLen);

	//// 发数据给指定游戏客户端 dwClientID:客户端ID
	//virtual bool SendDataToClient( DWORD dwClientID,const char * pData,DWORD dwLen, BYTE nSendChennel = VoiceSendChannel_Control );

	// 网关断线
	virtual void OnGatewayUserLeave(GatewayUser *pServer, DWORD dwReason = 0);


	////// 常用服务器列表相关接口 ////////////////////////////////////////////////////////////////////

	/**  取得指定游戏世界的指定类型的服务器ID
	@param   DWORD dwWorldID		: 游戏世界ID
	@param   WORD wServerType		: 服务器类型
	@param   WORD wSubID			: 服务器序号
	@return  找到的服务器ID,为0表示没找到
	*/
	virtual DWORD GetGameServerIDByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID);

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

	
	////////// 内部公用 ////////////////////////////////////////////////////////////////

	LPCSTR GetKickReasonText( DWORD dwReason );


private:

	// 释放所有Listener
	void ReleaseListeners();

	// 命令操作
	void ExcutCommand( int nNum1, int nNum2, const char * szTxt );

	////////////// Server ////////////////////////////////////////////////////////////

	// 握手消息
	void OnMsgServerHandshake(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	///////////////// SocialServer /////////////////////////////////////////////////////////


	///////////////// Gateway /////////////////////////////////////////////////////////

	// 握手消息
	void OnMsgGatewayHandshake(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 网关转发而来的客户端消息
	void OnMsgGatewaySendData(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	// 请求性能检测消息包
	void OnMsgGatewayPerformance(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

	
	//////////////////////////////////////////////////////////////////////////

	// 定时检测是否可以关闭语音服务器
	void OnTimerCheckShutDown(void);

public:

	IManagerConnector * m_ManagerConnector;		// 服务器管理连接器
	IEventEngine *	m_EventEngine;				// 事件引擎	// 最终如果无用，考虑删掉 TODO

	TimerAxis                   m_TimerAxis;				// 时间轴
	Acceptor					m_ServerListener;			// 监听Game服务器连接
	Acceptor					m_ViewServerListener;		// 监听View服务器连接
	Acceptor					m_ClientListener;			// 网关连接

	HANDLE                      m_hTimer;					// 定时触发的事件

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

private:
	// 当前操作代码
	DWORD				m_dwControlCode;

	// 是否初始化
	bool m_bInit;
	// 是否启动
	bool m_bStart;

	// 用数组处理，性能比原来map加vector提高 14倍
	DWORD m_SendClientArray[VIEW_MAX_GATEWAYID][VOICE_MAX_CLIENT_COUNT];	// 分类的客户端ID
	DWORD m_SendCountsArray[VIEW_MAX_GATEWAYID+1];	// 各分类客户端ID数

	// 正在关服中
	bool m_bShutDowning;

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


// 把一个ClientID列表合并到一个数组中，去掉重复的
// 把pClientIDIn里的ID合并到pClientIDArray中
// dwIDArrayLen pClientIDArray的值个数
// 此处借用TMAP_VoiceID2IDInfo定义
// 返回合并后的长度
DWORD MergeClientIDList( DWORD * pClientIDArray, DWORD dwIDArraySize, DWORD dwIDArrayCount, TMAP_VoiceID2IDInfo & TmpCheckList, DWORD * pClientIDIn, DWORD dwIDCountIn, bool bCheck = false );
