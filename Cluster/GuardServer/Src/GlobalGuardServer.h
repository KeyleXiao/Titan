/*******************************************************************
** 文件名:	GlobalGuardServer.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015-8-17
** 版  本:	1.0
** 描  述:	数据监护服务器
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "net.h"
#include "TimerAxis.h"
#include "ServerList.h"
#include "Acceptor.h"
#include "Reactor.h"
#include "Trace.h"
#include "AsynIoFrame.h"
using namespace rkt;


#include "GuardSetting.h"
#include <string>

#include "IManagerConnector.h"
using namespace Manager;



class GlobalGuardServer :  public IGuardServer,public TimerHandler,public EventHandler,public IManagerMessageHandler,public IAcceptorHandler
{
	enum
	{
		ETimerEventID_ReconnectNet = 0,
		ETimerEventID_CheckTimeOut,
		ETimerEventID_Keepalive,
	};

public:
	GlobalGuardServer();
	virtual ~GlobalGuardServer();

	// 初始化启动环境
	bool InitEnvironment(void);

	// 反初始化环境
	bool UnInitEnvironment(void);

	/**
	@name                    : 初始化网络
	@brief                   :
	@param wServerListenPort : 服务器监听端口
	@return                  :
	*/
	bool Create(WORD wServerListenPort);

	bool Close();

	// 是否初始化
	bool IsInit(void);
	// 是否初始化
	bool IsStart(void);

	/// 关闭服务器程序
	void CloseServerApp(void);

	/// 启动管理连接器
	void CreateManagerConnector(void);
	/// 关闭管理连接器
	void ReleaseManagerConnector(void);

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

	//////////////////////////////////////////////////////////////////////////

	virtual TimerAxis* getTimerAxis()
	{ return &m_TimerAxis; }


	//////////////////////////////////////////////////////////////////////////
	virtual void OnAccept( WORD wListenPort,IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler );

	virtual void OnEvent( HANDLE event );
	
	/// 服务器网络消息处理
	virtual void onServerMessage(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);
	
	//////////////////////////TimerHandler///////////////////////////
	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	virtual void OnTimer(unsigned long dwTimerID);

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

	// 发送数据给指定服务器
	virtual bool SendDataToServer(DWORD dwServerID,LPVOID pData,DWORD dwDataLen);

	// 广播数据给服务器 pServerArray==NULL && wServerNum==BRIDGE_BROADCASTALL_NUM 广播所有
	virtual bool BroadcastToServer(DWORD * pServerArray,WORD wServerNum,LPVOID pData,DWORD dwDataLen);

	// 服务器断线
	virtual void OnServerUserLeave(ServerUser *pServer);

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

private:
	///////////// 服务器网络消息处理 /////////////////////////////////////////////////////////////
	// 握手消息
	void OnMsgServerHandshake(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len);

public:


	IManagerConnector *			m_ManagerConnector;			// 服务器管理连接器
	TimerAxis                   m_TimerAxis;				// 时间轴
	Acceptor					m_ServerListener;			// 监听服务器连接
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
};

std::string GetLastErrorString(DWORD dwErrorCode);

