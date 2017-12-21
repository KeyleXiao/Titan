/*******************************************************************
** 文件名:	GuardConnector.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-17
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __GUARD_CONNECTOR_H__
#define __GUARD_CONNECTOR_H__

#include "netbuffer.h"
#include "IGuardConnector.h"
using namespace Guard;

#include "net.h"
using namespace rkt;

#include <string>
#include <map>
#include "ServerCapabilityTest.h"

// 最大统计消息码
#define MSG_STAT_MAXMSGID	500

class GuardConnector : public IGuardConnector,public IConnectionEventHandler,public TimerHandler
{
	enum
	{
		ETimerEventID_ReconnectNet = 0,
		ETimerEventID_Keepalive,
	};

	/// 重连周期10秒
#	define KEEPALIVE_INTERVAL	(2*1000)
#	define RECONNECT_INTERVAL	(10*1000)	

public:
	/////////////////// GuardConnector ///////////////////////////////////////////
	/** 构造函数
	*/
	GuardConnector(TimerAxis * pTimerAxis);

	/** 取得Connection
	*/
	IConnection*					GetConnection();

	/** 取得TimerAxis
	*/
	TimerAxis*						GetTimerAxis();

	/** 取得ServerType
	*/
	WORD							GetServerType();

public:
	/////////////////// IGuardConnector ///////////////////////////////////////////
	/** 创建
	DWORD dwWorldID		: 服务器所在游戏世界ID
	WORD wServerType	: 服务器类型
	WORD wSubID			: 第n号服务器
	char *szServerName	: 服务器名称
	char *szWorldName	: 游戏世界名称
	@return  
	*/
	virtual bool					Create(DWORD dwWorldID, WORD wServerType, WORD wSubID,const char * szServerName,const char * szWorldName);

	/** 释放
	*/
	virtual void					Release();

	/** 关闭和主服务器之间的连接
	*/
	virtual void					Close();

	/** 连接数据监护服务器
	@brief			: 启动服务器时连接中心服务器,以后如果连接断开,该模块内部会定时重试
	@param szCSIP	: 中心服务器IP 
	@param nCSPort	: 中心服务器端口
	@return			: 如果没有网络错误,则返回成功
	*/
	virtual bool					ConnectServer(const char * szCSIP,WORD nCSPort,int nThreadMode);

	/** 注册消息处理者
	@param moduleId 模块ID
	@param hander 消息处理者实例
	*/
	virtual bool					registerMessageHandler(DWORD moduleId, IGuardMessageHandler* handler);

	/** 注销消息处理者
	@param moduleId 模块ID
	*/
	virtual bool					unregisterMessageHandler(DWORD moduleId);

	/** 往数据监护服务器写入数据
	@param nNameSpace    : 镜像数据空间命名 IMAGE_DATA_NAMESPACE
	@param nKey	         : 键值
	@param pData         : 数据
	@param dwLen         : 数据长度
	@return
	*/
	virtual bool					WriteImageData(IGuardMessageHandler* handler, int nNameSpace, int nKey, const char * pData, DWORD dwLen);

	/** 从数据监护服务器异步读取数据
	@param nNameSpace    : 镜像数据空间命名 IMAGE_DATA_NAMESPACE
	@param nKey	         : 键值
	@return
	*/
	virtual bool					ReadImageData(IGuardMessageHandler* handler, int nNameSpace, int nKey);


	/** 取得本地服务器在中心服务器上的ID
	*/
	virtual DWORD					GetLocalServerCSID();

	/** 是否与服务器断开连接了
	@param   
	@param   
	@return  
	*/
	virtual bool					IsDisconnectToServer(void);

	/** 是否已启动
	@param   
	@param   
	@return  
	*/
	virtual bool					IsStartup();


	/** 启动服务器性能测试
	@param   
	@param   
	@return  
	*/
	virtual void					StartCapabilityTest(int nCheckInterval);

	/** 停止服务器性能测试
	@param   
	@param   
	@return  
	*/
	virtual void					StopCapabilityTest();

	/** 取完成性能测试的结点
	@param   
	@param   
	@return  
	*/
	virtual TCAPABILITYNODE&		GetCapabilityNode();

	/** 取最新监控结点数据
	@param   
	@param   
	@return  
	*/
	virtual TMONITORNODE&			GetMonitorNode();

	///////////////////IConnectionEventHandler///////////////////////////////////////////
	/**
	@brief              : 收到被动连接
	@param pIncomingConn: 客户端连入的新连接
	@param ppHandler    : 处理该连接的Handler
	@warning            : 收到连接后一定要设置Handler
	*/
	virtual void					OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler );

	/**
	@brief      : 连接成功
	@param conn : 连接对象指针
	*/
	virtual void					OnConnected( IConnection * conn );

	/**
	@brief      : 连接断开
	@param conn : 连接对象指针
	@param reason:断开原因(保留) 
	*/
	virtual void					OnDisconnected( IConnection * conn,DWORD reason );

	/**
	@brief        : 收到数据
	@param conn   : 连接对象指针
	@param pData  : 数据地址
	@pram dwDataLen:数据长度
	@return       :
	*/
	virtual void					OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen );

	/**
	@name             : 网络错误事件
	@brief            :
	@param conn       : 出现错误的套接字
	@param dwErrorCode: 错误码
	*/
	virtual void					OnError( IConnection * conn,DWORD dwErrorCode );

	///////////////////TimerHandler///////////////////////////////////////////
	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	virtual void					OnTimer( unsigned long dwTimerID );

protected:
	// Connection出现异常
	void							OnConnectError(IConnection * conn);

	// 发给务器数据
	inline bool						SendMessage(BYTE nMessageID, void * pData, size_t nDataLen);

	// 发给务器数据
	inline bool						SendData(void * pData, size_t nDataLen);

	bool							SendHandshake();
	void							OnMsgHandshakeResponse(unsigned long actionId, SGameMsgHead* head, void* data, size_t len);


protected:
	IConnection   *			m_pConnection;
	TimerAxis     *			m_pTimerAxis;
	std::string				m_szRemoteIP;
	WORD					m_wRemotePort;
	DWORD					m_dwWorldID;
	WORD					m_wSubID;
	WORD					m_wServerType;
	char					m_szName[SERVER_NAME_MAXSIZE];
	char					m_szWorldName[SERVER_NAME_MAXSIZE];
	bool					m_bIsConnected;
    BYTE                    m_nThreadMode;

	bool m_bIsAnswer;				// 是否有回应
	DWORD m_dwLastAnswerTime;		// 最后收到心跳Tick时间

	// 消息处理者列表
	IGuardMessageHandler*	m_MsgHandlers[MSG_MODULEID_MAXID+1];

	// 服务器性能测试对象
	CServerCapabilityTest	m_serverTest;
};

#endif	//__GUARD_CONNECTOR_H__