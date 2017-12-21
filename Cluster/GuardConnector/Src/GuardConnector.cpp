/*******************************************************************
** 文件名:	GuardConnector.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	02/21/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "GuardConnector.h"
#include "netbuffer.h"
#include "ICommand.h"

using namespace rkt;
#include <strstream>
using namespace Guard;


#define      INVALID_SERVER_ID   0xFFFFFFFF 	/// 不合法的服务器ID 

/////////////////// GuardConnector ///////////////////////////////////////////
GuardConnector::GuardConnector(TimerAxis * pTimerAxis)
	: m_pConnection(0)
	, m_pTimerAxis(pTimerAxis)
	, m_wRemotePort(0)
	, m_dwWorldID(INVALID_SERVER_ID)
	, m_wSubID(USHRT_MAX)
	, m_wServerType(MSG_ENDPOINT_UNKNOW)
	, m_bIsConnected(false)
	, m_bIsAnswer(true)
	, m_dwLastAnswerTime(0)
    , m_nThreadMode(NetworkUserType_Default)
{
	memset(m_szName, 0, sizeof(m_szName)); 
	memset(m_szWorldName, 0, sizeof(m_szWorldName)); 
	memset(m_MsgHandlers, 0, sizeof(m_MsgHandlers)); 
	m_serverTest.Create(this);
}

/** 取得Connection
*/
IConnection* GuardConnector::GetConnection()
{
	return m_pConnection;
}

/** 取得TimerAxis
*/
TimerAxis* GuardConnector::GetTimerAxis()
{
	return m_pTimerAxis;
}

/** 取得ServerType
*/
WORD GuardConnector::GetServerType()
{
	return m_wServerType;
}

///////////////////IGuardConnector///////////////////////////////////////////
/** 创建
DWORD dwWorldID		: 服务器所在游戏世界ID
WORD wServerType	: 服务器类型
WORD wSubID			: 第n号服务器
char *szServerName	: 服务器名称
char *szWorldName	: 游戏世界名称
@return  
*/
bool GuardConnector::Create(DWORD dwWorldID, WORD wServerType, WORD wSubID,const char * szServerName,const char * szWorldName)
{
	// 保存参数
	m_dwWorldID = dwWorldID;
	m_wServerType = wServerType;
	m_wSubID = wSubID;
	memset(m_szName,0,sizeof(m_szName));
	memset(m_szWorldName, 0, sizeof(m_szWorldName));

	if (szServerName)
	{
		sstrcpyn(m_szName, szServerName, sizeof(m_szName));	// 软件服务器名称
	}
	if (szWorldName)
	{
		sstrcpyn(m_szWorldName, szWorldName, sizeof(m_szWorldName));
	}
	
	return true;
}

/** 释放
*/
void  GuardConnector::Release()
{
	Close();

	delete this;
}

// 关闭和主服务器之间的连接
void GuardConnector::Close()
{
	if ( m_pConnection )
	{
		m_pConnection->Release();
		m_pConnection = 0;
	}

	m_pTimerAxis->KillTimer(ETimerEventID_ReconnectNet,this);
	m_pTimerAxis->KillTimer(ETimerEventID_Keepalive,this);

	m_bIsConnected = false;
}

/** 连接数据监护服务器
@brief			: 启动服务器时连接中心服务器,以后如果连接断开,该模块内部会定时重试
@param szCSIP	: 中心服务器IP 
@param nCSPort	: 中心服务器端口
@return			: 如果没有网络错误,则返回成功
*/
bool GuardConnector::ConnectServer(const char * szCSIP,WORD nCSPort,int nThreadMode)
{
	if (NULL==szCSIP || 0==nCSPort)
	{
		return false;
	}
	m_pConnection = CreateTCPConnection(0,this,DEFAULT_PACK,nThreadMode);
	if ( m_pConnection==0 )
	{
		return false;
	}

	m_szRemoteIP = szCSIP;
	m_wRemotePort= nCSPort;
    m_nThreadMode = nThreadMode;

	m_bIsConnected = false;

	char szSetName[256] = {0};
	::_snprintf_s(szSetName, sizeof(szSetName),_TRUNCATE, _GT("数据监护服连接器:%s"),m_szName);
	m_pConnection->setName(szSetName);

	return m_pConnection->Connect(szCSIP,nCSPort);
}

/** 注册消息处理者
@param moduleId 模块ID
@param hander 消息处理者实例
*/
bool GuardConnector::registerMessageHandler(DWORD moduleId, IGuardMessageHandler* handler)
{
	Assert(moduleId >= 0 && moduleId < MSG_MODULEID_MAXID);
	if (moduleId >= 0 && moduleId < MSG_MODULEID_MAXID)
	{
		if (m_MsgHandlers[moduleId] != NULL)
		{
			WarningLn("GuardConnector registerMessageHandler check moduleId("<<moduleId<<") already registered");
		}
		m_MsgHandlers[moduleId] = handler;
		return true;
	}
	else
	{
		ErrorLn("GuardConnector registerMessageHandler check moduleId("<<moduleId<<") range error");
		return false;
	}
}

/** 注销消息处理者
@param moduleId 模块ID
*/
bool GuardConnector::unregisterMessageHandler(DWORD moduleId)
{
	Assert(moduleId >= 0 && moduleId < MSG_MODULEID_MAXID);
	if (moduleId >= 0 && moduleId < MSG_MODULEID_MAXID)
	{
		if (m_MsgHandlers[moduleId] == NULL)
			WarningLn("GuardConnector unregisterMessageHandler check moduleId("<<moduleId<<") already unregistered");
		m_MsgHandlers[moduleId] = 0;
		return true;
	}
	else
	{
		ErrorLn("GuardConnector unregisterMessageHandler check moduleId("<<moduleId<<") range error");
		return false;
	}
}

/** 取得本地服务器在中心服务器上的ID
*/
DWORD GuardConnector::GetLocalServerCSID()
{
	return m_wSubID;
}

/** 是否与服务器断开连接了
@return  
*/
bool GuardConnector::IsDisconnectToServer(void)
{
	if ( m_bIsConnected==false || m_pConnection==0 )
	{		
		return true;
	}

	return false;
}

/** 是否已启动
@return  
*/
bool GuardConnector::IsStartup()
{
	return m_serverTest.IsStartup();
}

/** 往数据监护服务器写入数据
@param nNameSpace    : 镜像数据空间命名 IMAGE_DATA_NAMESPACE
@param nKey	         : 键值
@param pData         : 数据
@param dwLen         : 数据长度
*/
bool GuardConnector::WriteImageData(IGuardMessageHandler* handler,int nNameSpace, int nKey, const char * pData, DWORD dwLen)
{
	if (nNameSpace <= IMAGE_DATA_NAMESPACE_UNKNOWN || nNameSpace >= IMAGE_DATA_NAMESPACE_MAX)
	{
		ErrorLn("GuardConnector write image data failed because namespace invalid, namespace="<<nNameSpace<<", ken="<<nKey);
		return false;
	}

	obuf obufData;

	SMsgGuardWriteData data;
	data.nNameSpace = nNameSpace;	// 镜像数据命名类型 IMAGE_DATA_NAMESPACE
	data.nKey		= nKey;			// 键值
	data.nBackPtr	= (UINT_PTR)handler;
	data.dwDataLen	= dwLen;		// 数据长度
	obufData.push_back(&data, sizeof(data));

	if(pData != NULL && dwLen >0)
	{
		obufData.push_back(pData, dwLen);	// 数据
	}

	return SendMessage(MSG_GUARD_WRITE_DATA, obufData.data(), obufData.size() );
}

/** 从数据监护服务器异步读取数据
@param nNameSpace    : 镜像数据空间命名 IMAGE_DATA_NAMESPACE
@param nKey	         : 键值
*/
bool GuardConnector::ReadImageData(IGuardMessageHandler* handler,int nNameSpace, int nKey)
{
	if (nNameSpace <= IMAGE_DATA_NAMESPACE_UNKNOWN || nNameSpace >= IMAGE_DATA_NAMESPACE_MAX)
	{
		ErrorLn("GuardConnector read image data failed because namespace invalid, namespace="<<nNameSpace<<", ken="<<nKey);
		return false;
	}

	SMsgGuardReadData data;
	data.nNameSpace = nNameSpace;	// 镜像数据命名类型 IMAGE_DATA_NAMESPACE
	data.nKey		= nKey;			// 键值
	data.nBackPtr	= (UINT_PTR)handler;

	return SendMessage(MSG_GUARD_READ_DATA, &data, sizeof(data) );
}

/** 服务器性能测试
@param   
@param   
@return  
*/
void  GuardConnector::StartCapabilityTest(int nCheckInterval)
{
	return m_serverTest.StartCapabilityTest(nCheckInterval);
}

/** 停止服务器性能测试
@param   
@param   
@return  
*/
void GuardConnector::StopCapabilityTest()
{
	return m_serverTest.StopCapabilityTest();
}

/** 取完成性能测试的结点
@param   
@param   
@return  
*/
TCAPABILITYNODE& GuardConnector::GetCapabilityNode()
{
	return m_serverTest.GetCapabilityNode();
}

/** 取最新监控结点数据
@param   
@param   
@return  
*/
TMONITORNODE& GuardConnector::GetMonitorNode()
{
	return m_serverTest.GetMonitorNode();
}

///////////////////IConnectionEventHandler///////////////////////////////////////////
void GuardConnector::OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler )
{
	assert(false);
}

void GuardConnector::OnConnected( IConnection * conn )
{
	m_bIsConnected = true;

	if (m_wServerType!=MSG_ENDPOINT_UNKNOW)
	{
		SendHandshake();
	}

	m_pTimerAxis->SetTimer(ETimerEventID_Keepalive,KEEPALIVE_INTERVAL,this);
}


void GuardConnector::OnDisconnected( IConnection * conn,DWORD reason )
{
	m_bIsConnected = false;

	OnConnectError(conn);

	ErrorLn(__FUNCTION__":The connection with guard server disconnected!" << reason );
}

void GuardConnector::OnError( IConnection * conn,DWORD dwErrorCode )
{
    m_bIsConnected = false;

	OnConnectError(conn);

	ErrorLn(__FUNCTION__":The connection with guard server occurred an error!" << dwErrorCode );
}

void GuardConnector::OnConnectError(IConnection * conn)
{
	if ( m_pConnection==conn )
	{
		Close();
		m_pTimerAxis->SetTimer(ETimerEventID_ReconnectNet,RECONNECT_INTERVAL,this,1);
	}
}

void GuardConnector::OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
{
	Assert(pData != NULL && dwDataLen >= sizeof(SGameMsgHead));
	SGameMsgHead* head = (SGameMsgHead*)pData;
	unsigned long moduleId = head->byKeyModule;
	LPSTR data = (LPSTR)pData + sizeof(SGameMsgHead);
	size_t new_len = dwDataLen-sizeof(SGameMsgHead);
	if (new_len == 0) data = 0;
	Assert(moduleId >= 0 && moduleId < MSG_MODULEID_MAXID);


	if (m_MsgHandlers[moduleId])
	{
		BYTE actionId = head->byKeyAction;

		if (moduleId == MSG_MODULEID_GUARD)
		{
			switch(actionId)
			{
			case MSG_GUARD_ANSWER_KEEPALIVE:		// 心跳检查
				{
					m_bIsAnswer = true;							// 是否有回应
					m_dwLastAnswerTime = ::GetTickCount();		// 最后收到心跳Tick时间
				}
				break;
			case MSG_GUARD_HANDSHAKE_RESPONSE:		// 回应握手消息
				{
					OnMsgHandshakeResponse(actionId,head,data,new_len);
				}
				break;
			default:
				{
					m_MsgHandlers[moduleId]->HandleGuardMessage(actionId, head, data, new_len);
				}
				break;
			}
		}
		else
		{
			m_MsgHandlers[moduleId]->HandleGuardMessage(actionId, head, data, new_len);
		}
	}
	else
	{
		static DWORD dwLastAlertTimes = 0;
		DWORD dwNowTimes = (DWORD)time(NULL);
		if (dwNowTimes>dwLastAlertTimes+60)
		{
			dwLastAlertTimes = dwNowTimes;
			Error("GuardConnector::OnRecv(): moduleId("<<moduleId<<") Handler not registered"<<endl);
		}
	}
}

///////////////////TimerHandler///////////////////////////////////////////
/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void GuardConnector::OnTimer( unsigned long dwTimerID )
{
	switch(dwTimerID)
	{
	case ETimerEventID_Keepalive:			//  检查心跳
		{
			if (m_bIsConnected)
			{
				bool bIsHandered = false;
				// 回应心跳检查
				if (!m_bIsAnswer)
				{
					DWORD dwNowTicks = ::GetTickCount();		// 最后收到心跳Tick时间
					if (dwNowTicks-m_dwLastAnswerTime>GUARD_KEEPALIVE_MAXTIMEOUT)
					{
						m_dwLastAnswerTime = dwNowTicks;
						bIsHandered = true;
						// 重连
						Close();
						m_pTimerAxis->SetTimer(ETimerEventID_ReconnectNet,RECONNECT_INTERVAL,this,1);

						WarningLn("More than "<<GUARD_KEEPALIVE_MAXTIMEOUT<<" millisecond not received guard server connection heartbeat,Reconnect GuardServer："<< m_szRemoteIP.c_str() <<":" << m_wRemotePort);
					}
				}

				if (!bIsHandered)
				{
					m_bIsAnswer = false;

					// 心跳检查
					SendMessage(MSG_GUARD_KEEPALIVE, NULL, 0);
					//TraceLn("send heartbeat pack to guard server!");				
				}
			}
			else
			{
				m_bIsAnswer = true;
				m_dwLastAnswerTime = ::GetTickCount();		// 最后收到心跳Tick时间
			}
		}
		break;
	case ETimerEventID_ReconnectNet:					// 重连定时器
		{
			WarningLn(__FUNCTION__": reconnect to guard server ip="<< m_szRemoteIP.c_str() <<", port=" << m_wRemotePort<<", ThreadMode="<<m_nThreadMode);
			ConnectServer(m_szRemoteIP.c_str(),m_wRemotePort, m_nThreadMode);
		}
		break;
	default:break;
	}

}


void GuardConnector::OnMsgHandshakeResponse(unsigned long actionId, SGameMsgHead* head, void* data, size_t len)
{
	TraceLn("connect guard server sucess.");
}



//////////////////////////////////////////////////////////////////////////////
bool GuardConnector::SendHandshake()
{
	TraceLn("Send handshake to guard server");

	SMsgGuardHandshake_SG data;
	data.dwWorldID = m_dwWorldID;			// 服务器所在游戏世界ID
	data.nServerType = m_wServerType;		// 服务器类型
	data.wSubID = m_wSubID;					// 第n号服务器
	sstrcpyn(data.szName,m_szName, sizeof(data.szName));	// 软件服务器名称
	sstrcpyn(data.szWorldName,m_szWorldName,sizeof(data.szWorldName));	// 软件服务器名称

	return SendMessage(MSG_GUARD_HANDSHAKE, &data, sizeof(data) );
}

// 发给务器数据
bool GuardConnector::SendMessage(BYTE nMessageID, void * pData, size_t nDataLen)
{
	SGameMsgHead header;
	header.SrcEndPoint	= (BYTE)m_wServerType;
	header.DestEndPoint	= MSG_ENDPOINT_GUARD;
	header.byKeyModule  = MSG_MODULEID_GUARD;
	header.byKeyAction  = nMessageID;

	obuf obufData;
	obufData.push_back(&header, sizeof(header));
	if (pData && nDataLen > 0)
	{
		obufData.push_back(pData, nDataLen);
	}

	return SendData(obufData.data(),obufData.size());
}

// 发给务器数据
bool GuardConnector::SendData(void * pData, size_t nDataLen)
{
	if ( m_bIsConnected==false || m_pConnection==0 )
	{
		ErrorLn("Not connected guard server yet,please send data later.");
		return false;
	}

	if(!pData || nDataLen==0)
	{
		ErrorLn("send guard server data is invalid. pack_len="<<nDataLen);
		return false;
	}

	m_pConnection->SendData((char*)pData, nDataLen);
	return true;
}


//////////////////////////////////////////////////////////////////////////////
API_EXPORT  Guard::IGuardConnector * RKT_CDECL CreateGuardConnector(rkt::TimerAxis * pTimerAxis)
{
	return new GuardConnector(pTimerAxis);
}