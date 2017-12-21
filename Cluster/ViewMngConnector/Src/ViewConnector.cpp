/*******************************************************************
** 文件名:	ViewConnector.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2009/8/25
** 版  本:	1.0
** 描  述:	语音服务器连接器
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include <strstream>
#include "ostrbuf.h"
#include "time.h"
#include "ViewConnector.h"
#include "ViewCommonPart.h"

// 全局管理连接器接口
ViewConnector* g_hViewConnector = NULL;


//////////////////////////////////////////////////////////////////////////////
API_EXPORT  IViewConnector * RKT_CDECL CreateViewConnector(rkt::TimerAxis * pTimerAxis)
{
	ViewConnector * pConn = new ViewConnector(pTimerAxis);
	return (IViewConnector *)pConn;
}

//////////////////////////////////////////////////////////////////////////
ViewConnector::ViewConnector(TimerAxis * pTimerAxis):
	 m_pConnection(0)
	, m_pTimerAxis(pTimerAxis)
	, m_wRemotePort(0)
	, m_dwPublicWorldID(0)
	, m_dwWorldID(0)
	, m_ID(0)
	, m_wSubID(0)
	, m_bIsAnswer(true)
	, m_dwLastAnswerTime(0)
	, m_wServerType(MSG_ENDPOINT_UNKNOW)
	, m_bIsConnected(false)
    , m_nThreadMode(NetworkUserType_Default)
{

	memset(m_szName, 0, sizeof(m_szName));
	memset(m_szWorldName, 0, sizeof(m_szWorldName));

	memset(&m_MsgHandlers, 0, sizeof(m_MsgHandlers));

	if (NULL!=g_hViewConnector)
	{
		ErrorLn(_GT("语音服务器连接器被重复创建，会导致服务器管理功能出问题，请检查相关代码并修正！"));
	}
	g_hViewConnector = this;
}

ViewConnector::~ViewConnector()
{
	g_hViewConnector = NULL;
}

bool ViewConnector::Connect(const char * szCSIP,WORD nCSPort,int nThreadMode)
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

	//TCP_KEEPALIVE inKeepAlive;
	////设置socket的keep alive为5秒，并且发送次数为3次
	//inKeepAlive.onoff = 1; 
	//inKeepAlive.keepaliveinterval = MANAGER_KEEPALIVE_INTERVAL;	//两次KeepAlive探测间的时间间隔
	//inKeepAlive.keepalivetime = MANAGER_KEEPALIVE_TIMES;		//开始首次KeepAlive探测前的TCP空闭时间
	//m_pConnection->SetSocketOption(SOL_SOCKET,SO_KEEPALIVE,(char*)&inKeepAlive,sizeof(TCP_KEEPALIVE));

	m_szRemoteIP = szCSIP;
	m_wRemotePort= nCSPort;
    m_nThreadMode = nThreadMode;

	m_bIsConnected = false;

	char szSetName[256] = {0};
	::_snprintf_s(szSetName, sizeof(szSetName),_TRUNCATE, _GT("语音连接器:%d.%s:%d:%d"),m_dwWorldID,m_szWorldName,m_wServerType,m_wSubID);
	m_pConnection->setName(szSetName);

	return m_pConnection->Connect(szCSIP,nCSPort);
}
bool ViewConnector::Create(DWORD dwGameWorldID,WORD wServerType,WORD wSubID,const char * szServerName,const char * szWorldName)
{
	// 保存参数
	m_dwWorldID = dwGameWorldID;
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

	// 注册模块消息处理
	registerMessageHandler(MSG_MODULEID_VOICE, this);

	return true;
}

// 关闭和主服务器之间的连接
void ViewConnector::Close()
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


void ViewConnector::Release()
{

	// 取消注册模块消息处理
	unregisterMessageHandler(MSG_MODULEID_VOICE);

	Close();

	ClearAllList();

	delete this;
}


bool ViewConnector::IsConnected()
{
	return m_bIsConnected;
}

bool ViewConnector::RegisterServer(DWORD dwGameWorldID,WORD wServerType,WORD wSubID)
{
	m_dwWorldID = dwGameWorldID;
	m_wSubID = wSubID;
	m_wServerType = wServerType;

	if (m_bIsConnected)
	{
		return SendHandshake();
	}

	return false;
}


DWORD ViewConnector::GetServerID()
{
	return m_ID;
}


// 发给语音服务器数据
void  ViewConnector::SendData(const char * pData,DWORD dwLen)
{
	if ( m_pConnection )
	{
		m_pConnection->SendData(pData,dwLen);
	}
}

//////////////////////////////////////////////////////////////////////////
void ViewConnector::OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler )
{
	assert(false);
}

void ViewConnector::OnConnected( IConnection * conn )
{
	m_bIsConnected = true;

	if (m_wServerType!=MSG_ENDPOINT_UNKNOW)
	{
		SendHandshake();
	}

	m_pTimerAxis->SetTimer(ETimerEventID_Keepalive,VOICE_KEEPALIVE_INTERVAL,this);
}


void ViewConnector::OnDisconnected( IConnection * conn,DWORD reason )
{
	if ( m_pConnection==conn )
	{
		Close();
		m_pTimerAxis->SetTimer(ETimerEventID_ReconnectNet,VOICE_RECONNECT_INTERVAL,this,1);
	}

	WarningLn("The connection with VoiceServer disconnected!" << reason );
}

void ViewConnector::OnError( IConnection * conn,DWORD dwErrorCode )
{
	if ( m_pConnection==conn )
	{
		Close();
		m_pTimerAxis->SetTimer(ETimerEventID_ReconnectNet,VOICE_RECONNECT_INTERVAL,this,1);
	}

	WarningLn("The connection with VoiceServer occurred an error!" << dwErrorCode );
}


// 主动重连
bool ViewConnector::ReConnect()
{
	Close();
	WarningLn(_GT("主动重连 语音服务器 ") << m_szRemoteIP.c_str() <<":" << m_wRemotePort);

	return Connect(m_szRemoteIP.c_str(),m_wRemotePort, m_nThreadMode);
}

//////////////////////////////////////////////////////////////////////////
void ViewConnector::OnTimer( unsigned long dwTimerID )
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
					if (dwNowTicks-m_dwLastAnswerTime>VOICE_KEEPALIVE_MAXTIMEOUT)
					{
						m_dwLastAnswerTime = dwNowTicks;
						bIsHandered = true;
						// 重连
						Close();
						m_pTimerAxis->SetTimer(ETimerEventID_ReconnectNet,VOICE_RECONNECT_INTERVAL,this,1);

						WarningLn(_GT("与语音服务器连接超过")<<VOICE_KEEPALIVE_MAXTIMEOUT<<_GT("毫秒没有心跳,重连语音服务器：  ") << m_szRemoteIP.c_str() <<":" << m_wRemotePort);
					}
				}

				if (!bIsHandered)
				{
					m_bIsAnswer = false;
					// 心跳检查
					SGameMsgHead header;
					header.SrcEndPoint = (BYTE)m_wServerType;
					header.DestEndPoint= MSG_ENDPOINT_VOICE;
					header.byKeyModule  = MSG_MODULEID_VOICE;
					header.byKeyAction  = MSG_VOICE_KEEPALIVE;

					obuf obufData;
					obufData.push_back(&header, sizeof(SGameMsgHead));

					SendData(obufData.data(),obufData.size());

					//TraceLn("发送心跳检查!");
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
			TraceLn("Reconnect to VoiceServer " << m_szRemoteIP.c_str() <<":" << m_wRemotePort);
			Connect(m_szRemoteIP.c_str(),m_wRemotePort,m_nThreadMode);
		}
		break;
	default:break;
	}

}

//////////////////////////////////////////////////////////////////////////


/** 注册事件处理者
@param wEventID 事件ID	参考 VoiceDef.h   EMEventVoice
@param hander 事件处理者实例
*/
bool ViewConnector::registerEventHandler(WORD wEventID,IVoiceEventHandler* handler)
{
	if (NULL==handler || wEventID>=EVENT_VOICE_MAXID)
	{
		Error(_GT("注册语音服务器连接器事件处理者无效,wEventID=")<<wEventID<<" "<<endl);
		return false;
	}
	DWORD dwHandlerID = (DWORD)handler;
	m_mapEventHandlers[wEventID][dwHandlerID] = handler;	// 事件处理者列表
	return true;
}

/** 注销事件处理者
@param wEventID 事件ID	参考 VoiceDef.h   EMEventVoice
@param  hander 事件处理者实例
*/
bool ViewConnector::unregisterEventHandler(WORD wEventID,IVoiceEventHandler* handler)
{
	if (NULL==handler|| wEventID>=EVENT_VOICE_MAXID)
	{
		Error(_GT("注销语音服务器连接器事件处理者无效,wEventID=")<<wEventID<<" "<<endl);
		return false;
	}
	DWORD dwHandlerID = (DWORD)handler;
	m_mapEventHandlers[wEventID].erase(dwHandlerID);
	return true;
}


/** 触发事件
@param wEventID 事件ID	参考 VoiceDef.h   EMEventVoice
*/
bool ViewConnector::FireEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen)
{
	if (wEventID>=EVENT_VOICE_MAXID)
	{
		Error(_GT("触发语音服务器连接器事件ID无效,wEventID=")<<wEventID<<" "<<endl);
		return false;
	}
	TMAP_VoiceEventHandlerInfo &mapEventHandler = m_mapEventHandlers[wEventID];		// 事件处理者列表
	for(TMAP_VoiceEventHandlerInfo::iterator iter = mapEventHandler.begin(); iter!= mapEventHandler.end();++iter)
	{
		IVoiceEventHandler* pEventHandler = iter->second;
		if (NULL!=pEventHandler)
		{
			pEventHandler->OnVoiceEventExecute(wEventID,pszContext,nLen);
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
/** 注册消息处理者
@param moduleId 模块ID
@param hander 消息处理者实例
*/
bool ViewConnector::registerMessageHandler(DWORD moduleId, IVoiceMessageHandler* handler)
{
	Assert(moduleId >= 0 && moduleId < MSG_MODULEID_MAXID);
	if (moduleId >= 0 && moduleId < MSG_MODULEID_MAXID)
	{
		if (m_MsgHandlers[moduleId] != NULL)
		{
			Warning(_GT("语音服务器连接器 moduleId(")<<moduleId<<") already registered"<<endl);
		}
		m_MsgHandlers[moduleId] = handler;
		return true;
	}
	else
	{
		Error(_GT("语音服务器连接器 moduleId(")<<moduleId<<") range error"<<endl);
		return false;
	}
}

/** 注销消息处理者
@param moduleId 模块ID
*/
bool ViewConnector::unregisterMessageHandler(DWORD moduleId)
{
	Assert(moduleId >= 0 && moduleId < MSG_MODULEID_MAXID);
	if (moduleId >= 0 && moduleId < MSG_MODULEID_MAXID)
	{
		if (m_MsgHandlers[moduleId] == NULL)
			Warning(_GT("语音服务器连接器 moduleId(")<<moduleId<<") already unregistered"<<endl);
		m_MsgHandlers[moduleId] = 0;
		return true;
	}
	else
	{
		Error(_GT("语音服务器连接器 moduleId(")<<moduleId<<") range error"<<endl);
		return false;
	}
}

void ViewConnector::OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
{
	Assert(pData != NULL && dwDataLen >= sizeof(SGameMsgHead));
	SGameMsgHead* head = (SGameMsgHead*)pData;
	unsigned long moduleId = head->byKeyModule;
	LPSTR data = (LPSTR)pData + sizeof(SGameMsgHead);
	size_t new_len = dwDataLen-sizeof(SGameMsgHead);
	if (new_len == 0) data = 0;
	Assert(moduleId >= 0 && moduleId < MSG_MODULEID_MAXID);

	if (moduleId >= 0 && moduleId < MSG_MODULEID_MAXID)
	{
		// 通过跨区语音服务器中转消息必须加入此消息体
		SMsgVoiceHead voiceHead;
		// 处理中转数据,把附加数据去掉
		if (MSG_MODULEID_VOICE==moduleId && MSG_VOICE_SENDDATA==head->byKeyAction)
		{
			SMsgVoiceHead * pBHead = (SMsgVoiceHead *)data;
			voiceHead.dwWorldID	= pBHead->dwWorldID;		// 游戏世界ID
			voiceHead.dwServerID	= pBHead->dwServerID;		// 服务器ID
			head = (SGameMsgHead*)(data+sizeof(SMsgVoiceHead));
			moduleId = head->byKeyModule;
			Assert(data != NULL && new_len >= (sizeof(SGameMsgHead)+sizeof(SMsgVoiceHead)));
			Assert(moduleId >= 0 && moduleId < MSG_MODULEID_MAXID);
			data = data+sizeof(SGameMsgHead)+sizeof(SMsgVoiceHead);
			new_len = new_len-sizeof(SGameMsgHead)-sizeof(SMsgVoiceHead);
			if (new_len == 0) data = 0;

			if (moduleId >= MSG_MODULEID_MAXID)
			{
				Error(_GT("跨区语音服务器连接器:中转消息moduleId(")<<moduleId<<_GT(") 超出范围！")<<endl);
				return;
			}
		}
		else
		{
			voiceHead.dwWorldID	= 0;		// 游戏世界ID
			voiceHead.dwServerID	= 0;		// 服务器ID
		}

		if (m_MsgHandlers[moduleId])
			m_MsgHandlers[moduleId]->HandleVoiceMessage(head->byKeyAction,voiceHead,head, data, new_len);
		else
		{
			static DWORD dwLastAlertTimes = 0;
			DWORD dwNowTimes = (DWORD)time(NULL);
			if (dwNowTimes>dwLastAlertTimes+60)
			{
				dwLastAlertTimes = dwNowTimes;
				Error(_GT("语音服务器连接器 moduleId(")<<moduleId<<") Handler not registered"<<endl);
			}
		}

	}
	else
	{
		Error(_GT("语音服务器连接器:消息moduleId(")<<moduleId<<_GT(") 超出范围！")<<endl);
	}
}

//////////////////////////////////////////////////////////////////////////////
bool ViewConnector::SendHandshake()
{
	if ( m_bIsConnected==false || m_pConnection==0 )
	{
		WarningLn("Not connected VoiceServer yet,please send handshake a later.");
		return false;
	}

	SGameMsgHead header;
	header.SrcEndPoint = (BYTE)m_wServerType;
	header.DestEndPoint= MSG_ENDPOINT_VOICE;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_HANDSHAKE;

	// 语音服务器连接器与语音服务器握手消息数据结构
	SMsgVoiceHandshake_SB sendData;
	sendData.dwWorldID		= m_dwWorldID;		// 服务器所在游戏世界ID
	sendData.nServerType	= m_wServerType;	// 服务器类型
	sendData.wSubID			= m_wSubID;			// 第n号服务器
	sendData.bIsPublic		= isPublicGameWorld();	// 是否是公共区的

	sstrcpyn(sendData.szName,m_szName, sizeof(sendData.szName));		// 服务器名
	sstrcpyn(sendData.szWorldName,m_szWorldName, sizeof(sendData.szWorldName));		// 游戏世界名

	obuf obufData;
	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoiceHandshake_SB));

	SendData(obufData.data(),obufData.size());

	TraceLn(_GT("发送握手信息给 ")<<ToString().data());

	return true;
}

std::string ViewConnector::ToString()
{
	ostrstream buffer;
	buffer << _GT(" 语音服务器[") << m_szRemoteIP <<":" << m_wRemotePort <<"][ID=" << m_ID << ",dwWorldID="<<m_dwWorldID<<",ServerType="<<m_wServerType<<",SubID="<<m_wSubID<<":"<<m_szName<<"]" << std::ends;
	return buffer.str();
}

// 语音服务器连接器接口
IViewConnector* GetGlobalViewConnector()
{
	return g_hViewConnector;
}


////// 常用发数据接口 ////////////////////////////////////////////////////////////////////

// 发数据给指定游戏服务器 dwServerID:服务器ID 为0时广播给所有服务器,bZeroBroadcast:为true时打开为0广播,否则无效
void ViewConnector::SendDataToServer(DWORD dwServerID,const char * pData,DWORD dwLen,bool bZeroBroadcast)
{
	if ((bZeroBroadcast==false && 0==dwServerID) || NULL==pData || 0==dwLen)
	{
		return;
	}
	DWORD dwMaxSendLens = dwLen+sizeof(SGameMsgHead)+sizeof(SMsgVoiceSendData)+128;	
	BYTE szDataBuf[32768];			// 32K的临时内存
	BYTE * pBufData = szDataBuf;	// 默认用32K
	bool bIsNewMemory = false;		// 是否是new的内存，用来用完释放
	if (dwMaxSendLens>=32768)		// 如果大于32K,就New内存
	{
		bIsNewMemory = true;
		// 使用外部new的内存发数据,用完释放
		pBufData = (BYTE*)new char[dwMaxSendLens];
	}
	ofixbuf obufData((char *)pBufData, dwMaxSendLens);

	SGameMsgHead header;
	header.SrcEndPoint = (BYTE)m_wServerType;
	header.DestEndPoint= MSG_ENDPOINT_VOICE;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_SENDDATA;

	// 请求语音服务器网关服务发送数据给各软件服务器数据结构
	SMsgVoiceSendData sendData;
	sendData.dwServerID = dwServerID;		// 软件服务器ID
	sendData.nDataLens = dwLen;				// 数据长度

	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoiceSendData));
	obufData.push_back(pData, dwLen);

	SendData(obufData.data(),obufData.size());

	// 是new的内存,用完释放
	if (bIsNewMemory)
	{
		delete [](pBufData);
		pBufData=NULL;
	}

}
// 广播数据给指定列表游戏服务器 pServerArray:服务器ID列表,wServerNum:列表个数
void ViewConnector::BroadcastDataToServer(DWORD * pServerArray,WORD wServerNum,const char * pData,DWORD dwLen)
{
	if (NULL==pServerArray || 0==wServerNum|| 1024<wServerNum || NULL==pData || 0==dwLen)
	{
		return;
	}
	DWORD dwMaxSendLens =  dwLen+sizeof(SGameMsgHead)+sizeof(SMsgVoiceBroadcastData)+wServerNum*sizeof(DWORD)+128;
	BYTE szDataBuf[32768];			// 32K的临时内存
	BYTE * pBufData = szDataBuf;	// 默认用32K
	bool bIsNewMemory = false;		// 是否是new的内存，用来用完释放
	if (dwMaxSendLens>=32768)		// 如果大于32K,就New内存
	{
		bIsNewMemory = true;
		// 使用外部new的内存发数据,用完释放
		pBufData = (BYTE*)new char[dwMaxSendLens];
	}
	ofixbuf obufData((char *)pBufData, dwMaxSendLens);

	SGameMsgHead header;
	header.SrcEndPoint = (BYTE)m_wServerType;
	header.DestEndPoint= MSG_ENDPOINT_VOICE;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_BROADCASTDATA;

	// 请求语音服务器网关服务广播数据给各软件服务器数据结构
	SMsgVoiceBroadcastData sendData;
	sendData.dwServerCounts = wServerNum;			// 软件服务器ID列表个数
	sendData.nDataLens		= dwLen;				// 数据长度

	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoiceBroadcastData));
	obufData.push_back(pServerArray, wServerNum*sizeof(DWORD));			// 软件服务器ID列表
	obufData.push_back(pData, dwLen);

	SendData(obufData.data(),obufData.size());

	// 是new的内存,用完释放
	if (bIsNewMemory)
	{
		delete [](pBufData);
		pBufData=NULL;
	}

}



////// 常用服务器列表相关接口 ////////////////////////////////////////////////////////////////////

/**  取得服务器ID列表
@param   DWORD* pServerArray	: 保存找到的ID数组
@param   WORD dwArrayLen		: 数组长度
@param   WORD &dwGetCounts		: 找到的个数
@param   DWORD dwWorldID		: 游戏世界ID，0为全部
@param   WORD wServerType		: 服务器类型,0为全部
@return  找到的个数
*/
DWORD ViewConnector::GetGameServerIDList(DWORD * pServerArray,DWORD dwArrayLen,DWORD &dwGetCounts,DWORD dwWorldID,WORD wServerType)
{
	if (pServerArray==NULL || dwArrayLen==0)
	{
		dwGetCounts = 0;
		return 0;
	}
	DWORD dwFindCounts = 0;
	DWORD * pServerID = pServerArray;
	// 游戏世界ID为0用遍历
	if (0==dwWorldID)
	{
		for(TMAP_VoicePartList::iterator iter = m_mapPartList.begin(); iter!= m_mapPartList.end();++iter)
		{
			if (NULL==iter->second)
			{
				continue;
			}
			if ( (0==dwWorldID|| dwWorldID==iter->second->GetWorldID()) && (0==wServerType|| wServerType==iter->second->GetServerType()))
			{
				if (dwFindCounts>=dwArrayLen)
				{
					break;
				}
				(*pServerID) = iter->second->GetServerID();
				pServerID++;
				dwFindCounts++;
			}
		}
	} 
	else
	{
		// 游戏世界ID分类列表
		TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.find(dwWorldID);
		if ( iter!=m_mapWorldList.end())
		{
			// 合成ID
			WORD wTypeSubData[2];
			DWORD dwTypeSubID = 0;
			for(TMAP_VoiceID2IDInfo::iterator iter1 = iter->second.mapList.begin(); iter1!= iter->second.mapList.end();++iter1)
			{
				dwTypeSubID = iter1->first;
				memcpy(wTypeSubData,&dwTypeSubID,sizeof(DWORD));
				if (0==wServerType || wTypeSubData[0]==wServerType)
				{
					if (dwFindCounts>=dwArrayLen)
					{
						break;
					}
					(*pServerID) = iter1->second;
					pServerID++;
					dwFindCounts++;
				}
			}
		}
	}
	dwGetCounts = dwFindCounts;
	return dwFindCounts;
}


/**  取得指定游戏世界的指定类型的服务器ID
@param   DWORD dwWorldID		: 游戏世界ID
@param   WORD wServerType		: 服务器类型
@param   WORD wSubID			: 服务器序号
@return  找到的服务器ID,为0表示没找到
*/
DWORD ViewConnector::GetGameServerIDByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID)
{
	// 处理 游戏世界ID分类列表
	TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.find(dwWorldID);
	if ( iter==m_mapWorldList.end())
	{
		return 0;
	}

	// 合成ID
	WORD wTypeSubData[2];
	wTypeSubData[0] = wServerType;
	wTypeSubData[1] = wSubID;
	DWORD dwTypeSubID = 0;
	memcpy(&dwTypeSubID,wTypeSubData,sizeof(DWORD));

	TMAP_VoiceID2IDInfo::iterator iter1 = iter->second.mapList.find(dwTypeSubID);
	if ( iter1==iter->second.mapList.end())
	{
		return 0;
	}
	return iter1->second;
}

/** 列出所有服务器信息
@param   DWORD dwWorldID		: 游戏世界ID
@param   WORD wServerType		: 服务器类型
@return 
*/
void ViewConnector::PrintGameServerListInfo(DWORD dwWorldID,WORD wServerType)
{
	for(TMAP_VoicePartList::iterator iter = m_mapPartList.begin(); iter!= m_mapPartList.end();++iter)
	{
		if ( (0==dwWorldID|| dwWorldID==iter->second->GetWorldID()) && (0==wServerType|| wServerType==iter->second->GetServerType()))
		{
			TraceLn("ID="<<iter->second->GetServerID()<<_GT(",游戏世界ID=")<<iter->second->GetWorldID()<<_GT(",服务器类型=")<<GetServerTypeString(iter->second->GetServerType())<<_GT(",序号=")<<iter->second->GetServerSubID());
		}
	}
}

////// 常用游戏世界列表相关接口 ////////////////////////////////////////////////////////////////////

/**  取得游戏世界ID列表
@param   DWORD* pWorldArray	: 保存找到的ID数组
@param   WORD dwArrayLen		: 数组长度
@param   WORD &dwGetCounts		: 找到的个数
@return  找到的个数
*/
DWORD ViewConnector::GetGameWorldIDList(DWORD * pWorldArray,DWORD dwArrayLen,DWORD &dwGetCounts)
{
	if (pWorldArray==NULL || dwArrayLen==0)
	{
		dwGetCounts = 0;
		return 0;
	}
	DWORD dwFindCounts = 0;
	DWORD * pWorldID = pWorldArray;
	// 游戏世界ID分类列表,以dwWorldID为关键字
	for(TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.begin(); iter!= m_mapWorldList.end();++iter)
	{
		(*pWorldID) = iter->first;
		pWorldID++;
		dwFindCounts++;
	}
	dwGetCounts = dwFindCounts;
	return dwFindCounts;
}

/**  取得指定游戏世界信息
@param   DWORD dwWorldID		: 游戏世界ID
@return  找到的服务器ID,为0表示没找到
*/
IVoiceWorldInfo * ViewConnector::GetGameWorldInfo(DWORD dwWorldID)
{
	// 游戏世界ID分类列表
	TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.find(dwWorldID);
	if ( iter==m_mapWorldList.end())
	{
		return NULL;
	}
	return &(iter->second);
}

/** 列出所有游戏世界信息
@return 
*/
void ViewConnector::PrintGameWorldListInfo()
{
	// 游戏世界ID分类列表,以dwWorldID为关键字
	for(TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.begin(); iter!= m_mapWorldList.end();++iter)
	{
		TraceLn(_GT("游戏世界ID=")<<iter->second.dwWorldID<<_GT(",名称=")<<iter->second.strWorldName.c_str());
	}
}

////// 常用扩展信息类列表相关接口 ////////////////////////////////////////////////////////////////////

/** 用服务器ID 取得扩展信息类
@param   DWORD dwServerID		: 服务器ID
@return  IVoicePart*	:服务器信息
*/
IVoicePart* ViewConnector::GetGameServerPart(DWORD dwServerID)
{
	// 语音服务器上的服务器扩展信息类列表,以服务器ID为关键字
	TMAP_VoicePartList::iterator iter = m_mapPartList.find(dwServerID);
	if ( iter==m_mapPartList.end())
		return NULL;
	else
		return iter->second;
}

/** 用TypeSubID取得扩展信息类
@param   DWORD dwWorldID		: 游戏世界ID
@param   WORD wServerType		: 服务器类型
@param   WORD wSubID			: 服务器序号
@return  const SVoiceGameServerInfo*	:服务器信息
*/
IVoicePart* ViewConnector::GetGameServerPartByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID)
{
	DWORD dwFindServerID = GetGameServerIDByTypeSubID(dwWorldID,wServerType,wSubID);
	if (0==dwFindServerID)
	{
		return NULL;
	}
	// 语音服务器上的服务器扩展信息类列表,以服务器ID为关键字
	TMAP_VoicePartList::iterator iter = m_mapPartList.find(dwFindServerID);
	if ( iter==m_mapPartList.end())
		return NULL;
	else
		return iter->second;
}

// 清除所有服务器信息列表
void ViewConnector::ClearAllList(void)
{
	// 语音服务器上的服务器扩展信息类列表,以服务器ID为关键字
	for(TMAP_VoicePartList::iterator iter = m_mapPartList.begin(); iter!= m_mapPartList.end();++iter)
	{
		if (NULL!=iter->second)
		{
			iter->second->Release();
		}
	}
	// 清除
	m_mapPartList.clear();

}

////// 语音功能类相关接口 ////////////////////////////////////////////////////////////////////
/** 添加蓝钻
@param   DWORD dwUDBID		: 用户UDBID
@return  DWORD dwCount		: 蓝钻数
@param	 DWORD dwPDBID		: 用户PDBID 可以为0
@param	 DWORD dwPDBIDSend	: 赠送者PDBID 可以为0
@param	 DWORD dwSpare1
@param	 DWORD dwSpare2
@param	 DWORD dwSpare3 备用
@param	 const char * szReason : 备注
*/
bool ViewConnector::AddVoiceBlueDiamond( DWORD dwUDBID, DWORD dwCount, DWORD dwPDBID, DWORD dwPDBIDSend, DWORD dwSpare1, DWORD dwSpare2, DWORD dwSpare3, const char * szReason )
{
	if( dwUDBID == 0 || dwCount == 0 )
	{
		return false;
	}

	DWORD dwReasonLen = 0;
	char szBuff[128] = {0};
	if( szReason != NULL && strlen(szReason) > 0 )
	{
		sstrcpyn( szBuff, szReason, sizeof(szBuff) );
		dwReasonLen = strlen(szBuff);
	}

	obuf obufData;
	SGameMsgHead header;
	header.SrcEndPoint = (BYTE)m_wServerType;
	header.DestEndPoint= MSG_ENDPOINT_VOICE;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_ADDVOICEBLUEDIAMOND;
	obufData.push_back( &header, sizeof(SGameMsgHead) );

	SMsgVoiceAddVoiceBlueDiamond_VV addData;
	addData.dwUDBID = dwUDBID;		// 添加者UDBID
	addData.dwPDBID = dwPDBID;		// 添加者PDBID
	addData.dwCount = dwCount;		// 添加蓝钻数量
	addData.dwPDBIDSend = dwPDBIDSend;
	addData.dwSpare1 = dwSpare1;
	addData.dwSpare2 = dwSpare2;
	addData.dwSpare3 = dwSpare3;
	addData.nReasonLen = (WORD)dwReasonLen;	// 备注长度
	obufData.push_back( &addData, sizeof(SMsgVoiceAddVoiceBlueDiamond_VV) );

	if( addData.nReasonLen > 0 )
	{
		obufData.push_back( szBuff, dwReasonLen );
	}
	
	SendData(obufData.data(),obufData.size());

	return true;
}

/** 广播数据到客户端
@param	pData		: 数据
@param	dwLen		: 数据长度
@param	nBroadcastType : 广播类型 EMVoiceChannelType
@param	dwPDBID1/dwPDBID2 : 用户PDBID
@param	dwWorldID1/dwWorldID2 : 世界ID，
@param  dwValueID1/dwValueID2 : ID值,参考SVoiceChannelID
@param  bRoom : 是否广播用户dwPDBID1所在房间
*/
bool ViewConnector::BroadcastDataToClient( BYTE nBroadcastType, void * pData, DWORD dwLen, DWORD dwPDBID1, DWORD dwPDBID2, DWORD dwWorldID1, DWORD dwWorldID2, DWORD dwValueID1, DWORD dwValueID2, bool bRoom )
{
	if( pData == NULL || dwLen <= 0 || nBroadcastType > VoiceChannel_Max )
	{
		return false;
	}

	DWORD dwMaxSendLens = dwLen+sizeof(SGameMsgHead)+sizeof(SMsgVoiceBroadcastDataToClient)+128;	
	BYTE szDataBuf[32768];			// 32K的临时内存
	BYTE * pBufData = szDataBuf;	// 默认用32K
	bool bIsNewMemory = false;		// 是否是new的内存，用来用完释放
	if (dwMaxSendLens>=32768)		// 如果大于32K,就New内存
	{
		bIsNewMemory = true;
		// 使用外部new的内存发数据,用完释放
		pBufData = (BYTE*)new char[dwMaxSendLens];
	}
	ofixbuf obufData((char *)pBufData, dwMaxSendLens);

	SGameMsgHead header;
	header.SrcEndPoint = (BYTE)m_wServerType;
	header.DestEndPoint= MSG_ENDPOINT_VOICE;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_BROADCASTDATATOCLIENT;
	obufData.push_back(&header, sizeof(SGameMsgHead));

	SMsgVoiceBroadcastDataToClient broadcastData;
	broadcastData.nType = nBroadcastType;				// 广播类型	EMVoiceBroadcastDataToClientType
	broadcastData.dwPDBID1 = dwPDBID1;					// 角色ID1
	broadcastData.dwPDBID2 = dwPDBID2;					// 角色ID2
	broadcastData.nWorldID1 = (WORD)dwWorldID1;			// 世界ID1
	broadcastData.nWorldID2 = (WORD)dwWorldID2;			// 世界ID2
	broadcastData.dwValueID1 = dwValueID1;		// ID值1
	broadcastData.dwValueID2 = dwValueID2;		// ID值2
	broadcastData.bRoom = bRoom;
	broadcastData.dwDataLen = dwLen;					// 数据长度
	obufData.push_back( &broadcastData, sizeof(SMsgVoiceBroadcastDataToClient) );

	obufData.push_back( pData, dwLen );

	SendData(obufData.data(),obufData.size());

	// 是new的内存,用完释放
	if (bIsNewMemory)
	{
		delete [](pBufData);
		pBufData=NULL;
	}

	return true;
}

///////////////////////// IVoiceMessageHandler 处理语音服务器发过来的消息 /////////////////////////
void ViewConnector::HandleVoiceMessage(ulong actionId,SMsgVoiceHead &voiceHead,SGameMsgHead* head, LPSTR data, size_t len)
{
	switch(actionId)
	{
	case MSG_VOICE_ANSWER_KEEPALIVE:		// 心跳检查
		{
			m_bIsAnswer = true;							// 是否有回应
			m_dwLastAnswerTime = ::GetTickCount();		// 最后收到心跳Tick时间
			//TraceLn("收到心跳回应!");
		}
		break;
	case MSG_VOICE_SERVERLIST_UPDATE:		// 服务器列表更新
		{
			OnMsgServerListUpdate(actionId,voiceHead,head,data,len);
		}
		break;
	case MSG_VOICE_WORLDLIST_UPDATE:		// 游戏世界列表更新
		{
			OnMsgWorldListUpdate(actionId,voiceHead,head,data,len);
		}
		break;
	case MSG_VOICE_HANDSHAKE_RESPONSE:		// 回应握手消息
		{
			OnMsgHandshakeResponse(actionId,voiceHead,head,data,len);
		}
		break;
	default:
		{
			ErrorLn("尚有一个语音服务器发过来的消息未处理，actionId = "<<actionId);
		}
		break;
	}
}


///////////////////////// 处理语音服务器发过来的消息 /////////////////////////


// 回应握手消息
void ViewConnector::OnMsgHandshakeResponse(unsigned long actionId, SMsgVoiceHead &voiceHead,SGameMsgHead* head, LPSTR data, size_t len)
{

	if( len != sizeof(SMsgVoiceHandshakeResponse))
	{
		ErrorLn(_GT("语音服务器回应握手消息数据大小非法!"));
		return;
	}

	SMsgVoiceHandshakeResponse * pMsg = (SMsgVoiceHandshakeResponse *)data;

	m_ID = pMsg->dwID;

	TraceLn(_GT("与语音服务器握手成功! ID=")<<m_ID<<_GT(",公共游戏世界ID=")<<m_dwPublicWorldID);

	// 触发与语音服务器握手成功事件
	SEventVoiceHandshake evenData;
	evenData.dwServerID = m_ID;
	FireEventExecute(EVENT_VOICE_HANDSHAKE,(LPSTR)&evenData,sizeof(evenData));

}

// 服务器列表更新消息
void ViewConnector::OnMsgServerListUpdate(unsigned long actionId, SMsgVoiceHead &voiceHead, SGameMsgHead* head, LPSTR data, size_t len)
{

	if(data==NULL|| len < sizeof(SMsgVoiceServerListUpdate))
	{
		return;
	}

	SMsgVoiceServerListUpdate * pMsg = (SMsgVoiceServerListUpdate *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf, _countof(szBuf),_NGT"ViewConnector::OnMsgServerListUpdate()  服务器列表更新消息:来源游戏世界ID=%d,来源服务器ID=%d,更新类型=%d,软件服务器个数=%d",
		voiceHead.dwWorldID,voiceHead.dwServerID,pMsg->nUpdateType,pMsg->dwServerCounts);
	TraceLn(szBuf);
#endif
	// 软件服务器信息列表
	SMsgVoiceGameServerNode * pServerNode = (SMsgVoiceGameServerNode *)(data+ sizeof(SMsgVoiceServerListUpdate));

	// 更新类型,EMVoiceServerListUpdateType
	switch(pMsg->nUpdateType)
	{
	case VS_SvrListUpdateType_All:		// 全部
		{
			// 清除所有服务器信息列表
			ClearAllList();

			for (DWORD i=0;i<pMsg->dwServerCounts;i++ )
			{
				// 加入扩展信息类
				TMAP_VoicePartList::iterator iter = m_mapPartList.find(pServerNode->dwID);
				if ( iter==m_mapPartList.end())
				{
					CVoiceCommonPart * pNewPart = new CVoiceCommonPart();
					pNewPart->Create(pServerNode->dwID,pServerNode->dwWorldID,pServerNode->wServerType,pServerNode->wSubID);
					m_mapPartList[pServerNode->dwID] = pNewPart;
					
					// 处理 游戏世界ID分类列表
					TMAP_VoiceWorldIDInfo::iterator iter1 = m_mapWorldList.find(pServerNode->dwWorldID);
					if ( iter1==m_mapWorldList.end())
					{
						SVoiceWorldNode addNode;
						addNode.dwWorldID = pServerNode->dwWorldID;
						// 登录管理员帐号信息,以dwID为关键字,pair<TMAP_LoginUserInfo::iterator,bool> retPair=
						pair<TMAP_VoiceWorldIDInfo::iterator,bool> retPair = m_mapWorldList.insert(TMAP_VoiceWorldIDInfo::value_type(pServerNode->dwWorldID,addNode));
						iter1 = retPair.first;
					}
					// 合成ID
					WORD wTypeSubData[2];
					wTypeSubData[0] = pServerNode->wServerType;
					wTypeSubData[1] = pServerNode->wSubID;
					DWORD dwTypeSubID = 0;
					memcpy(&dwTypeSubID,wTypeSubData,sizeof(DWORD));

					iter1->second.mapList[dwTypeSubID] = pServerNode->dwID;

					// 触发新增服务器事件
					SEventVoiceServerListAdd evenData;
					evenData.dwID			= pServerNode->dwID;		// 服务器ID
					evenData.dwWorldID		= pServerNode->dwWorldID;	// 服务器所在游戏世界ID
					evenData.wServerType	= pServerNode->wServerType;	// 服务器类型
					evenData.wSubID			= pServerNode->wSubID;		// 第n号服务器
					evenData.dwStateFlag	= pServerNode->dwStateFlag;	// 服务器状态
					FireEventExecute(EVENT_VOICE_SERVERLIST_ADD,(LPSTR)&evenData,sizeof(evenData));
				}
				pServerNode++;
			}

			// 触发全部服务器加入事件
			SEventVoiceServerListAll evenData;
			evenData.dwServerCounts = pMsg->dwServerCounts;		// 收到服务器个数
			FireEventExecute(EVENT_VOICE_SERVERLIST_ALL,(LPSTR)&evenData,sizeof(evenData));
		}
		break;
	case VS_SvrListUpdateType_Add:		// 新增
	case VS_SvrListUpdateType_Edit:		// 修改
		{
			for (DWORD i=0;i<pMsg->dwServerCounts;i++ )
			{
				// 加入扩展信息类
				TMAP_VoicePartList::iterator iter = m_mapPartList.find(pServerNode->dwID);
				if ( iter==m_mapPartList.end())
				{
					CVoiceCommonPart * pNewPart = new CVoiceCommonPart();
					pNewPart->Create(pServerNode->dwID,pServerNode->dwWorldID,pServerNode->wServerType,pServerNode->wSubID);
					m_mapPartList[pServerNode->dwID] = pNewPart;

					// 处理 游戏世界ID分类列表
					TMAP_VoiceWorldIDInfo::iterator iter1 = m_mapWorldList.find(pServerNode->dwWorldID);
					if ( iter1==m_mapWorldList.end())
					{
						SVoiceWorldNode addNode;
						addNode.dwWorldID = pServerNode->dwWorldID;
						// 登录管理员帐号信息,以dwID为关键字,pair<TMAP_LoginUserInfo::iterator,bool> retPair=
						pair<TMAP_VoiceWorldIDInfo::iterator,bool> retPair = m_mapWorldList.insert(TMAP_VoiceWorldIDInfo::value_type(pServerNode->dwWorldID,addNode));
						iter1 = retPair.first;
					}
					// 合成ID
					WORD wTypeSubData[2];
					wTypeSubData[0] = pServerNode->wServerType;
					wTypeSubData[1] = pServerNode->wSubID;
					DWORD dwTypeSubID = 0;
					memcpy(&dwTypeSubID,wTypeSubData,sizeof(DWORD));

					iter1->second.mapList[dwTypeSubID] = pServerNode->dwID;

					// 触发新增服务器事件
					SEventVoiceServerListAdd evenData;
					evenData.dwID			= pServerNode->dwID;		// 服务器ID
					evenData.dwWorldID		= pServerNode->dwWorldID;	// 服务器所在游戏世界ID
					evenData.wServerType	= pServerNode->wServerType;	// 服务器类型
					evenData.wSubID			= pServerNode->wSubID;		// 第n号服务器
					evenData.dwStateFlag	= pServerNode->dwStateFlag;	// 服务器状态
					FireEventExecute(EVENT_VOICE_SERVERLIST_ADD,(LPSTR)&evenData,sizeof(evenData));
				}

				pServerNode++;
			}
		}
		break;
	case VS_SvrListUpdateType_Del:		// 删除
		{
			// 软件服务器ID列表
			DWORD * pServerID = (DWORD *)(data+ sizeof(SMsgVoiceServerListUpdate));

			for (DWORD i=0;i<pMsg->dwServerCounts;i++ )
			{
				// 删除扩展信息类
				TMAP_VoicePartList::iterator iter = m_mapPartList.find((*pServerID));
				if ( iter!=m_mapPartList.end())
				{
					// 触发删除服务器事件
					SEventVoiceServerListDel evenData;
					evenData.dwServerID	= (*pServerID);				// 服务器ID
					FireEventExecute(EVENT_VOICE_SERVERLIST_DEL,(LPSTR)&evenData,sizeof(evenData));

					if (NULL!=iter->second)
					{
						IVoicePart* pVoicePart = iter->second;
						// 处理 游戏世界ID分类列表
						TMAP_VoiceWorldIDInfo::iterator iter1 = m_mapWorldList.find(pVoicePart->GetWorldID());
						if ( iter1!=m_mapWorldList.end())
						{
							// 合成ID
							WORD wTypeSubData[2];
							wTypeSubData[0] = pVoicePart->GetServerType();
							wTypeSubData[1] = pVoicePart->GetServerSubID();
							DWORD dwTypeSubID = 0;
							memcpy(&dwTypeSubID,wTypeSubData,sizeof(DWORD));
							iter1->second.mapList.erase(dwTypeSubID);
						}
						pVoicePart->Release();
					}
					m_mapPartList.erase(iter);
				}

				pServerID++;
			}
		}
		break;
	}

#ifdef VOICE_PRINT_DEBUG_INF
	// 列出所有服务器信息
	PrintGameServerListInfo();
#endif

}

// 游戏世界列表更新消息
void ViewConnector::OnMsgWorldListUpdate(unsigned long actionId, SMsgVoiceHead &voiceHead, SGameMsgHead* head, LPSTR data, size_t len)
{

	if(data==NULL|| len < sizeof(SMsgVoiceWorldListUpdate))
	{
		return;
	}

	SMsgVoiceWorldListUpdate * pMsg = (SMsgVoiceWorldListUpdate *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf, _countof(szBuf),_NGT"ViewConnector::OnMsgWorldListUpdate()  游戏世界列表更新消息:来源游戏世界ID=%d,来源服务器ID=%d,更新类型=%d,游戏世界个数=%d",
		voiceHead.dwWorldID,voiceHead.dwServerID,pMsg->nUpdateType,pMsg->dwWorldCounts);
	TraceLn(szBuf);
#endif
	// 游戏世界信息列表
	SMsgVoiceGameWorldNode * pServerNode = (SMsgVoiceGameWorldNode *)(data+ sizeof(SMsgVoiceWorldListUpdate));

	DWORD nListCounts = pMsg->dwWorldCounts;

	DWORD dwNameStrOffset = nListCounts * sizeof(SMsgVoiceGameWorldNode) + sizeof(SMsgVoiceWorldListUpdate);
	LPSTR pNameStr = (LPSTR)data + dwNameStrOffset;

	char szWorldName[VOICE_NAME_MAXSIZE] = {0};

	// 更新类型,EMVoiceServerListUpdateType
	switch(pMsg->nUpdateType)
	{
	case VS_SvrListUpdateType_All:		// 全部
		{
			for (DWORD i=0;i<pMsg->dwWorldCounts;i++ )
			{
				pNameStr = (LPSTR)data + dwNameStrOffset;

				if (pServerNode->nNameStrlens>0 && pServerNode->nNameStrlens<sizeof(szWorldName))
				{
					sstrcpyn(szWorldName,pNameStr,pServerNode->nNameStrlens+1);
				}
				else
				{
					memset(szWorldName,0,sizeof(szWorldName));
				}

				// 处理 游戏世界ID分类列表
				TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.find(pServerNode->dwWorldID);
				if ( iter==m_mapWorldList.end())
				{
					SVoiceWorldNode addNode;
					addNode.dwWorldID = pServerNode->dwWorldID;
					addNode.strWorldName = szWorldName;
					//信息,以dwID为关键字,pair<TMAP_LoginUserInfo::iterator,bool> retPair=
					m_mapWorldList.insert(TMAP_VoiceWorldIDInfo::value_type(pServerNode->dwWorldID,addNode));
				}
				else
				{
					iter->second.strWorldName = szWorldName;
				}

				dwNameStrOffset += pServerNode->nNameStrlens;
				pServerNode++;
			}

			// 触发全部游戏世界加入事件
			SEventVoiceWorldListAll evenData;
			evenData.dwWorldCounts = pMsg->dwWorldCounts;		// 收到游戏世界个数
			FireEventExecute(EVENT_VOICE_WORLDLIST_ALL,(LPSTR)&evenData,sizeof(evenData));
		}
		break;
	case VS_SvrListUpdateType_Add:		// 新增
	case VS_SvrListUpdateType_Edit:		// 修改
		{
			for (DWORD i=0;i<pMsg->dwWorldCounts;i++ )
			{
				pNameStr = (LPSTR)data + dwNameStrOffset;

				if (pServerNode->nNameStrlens>0 && pServerNode->nNameStrlens<sizeof(szWorldName))
				{
					sstrcpyn(szWorldName,pNameStr,pServerNode->nNameStrlens+1);
				}
				else
				{
					memset(szWorldName,0,sizeof(szWorldName));
				}

				// 处理 游戏世界ID分类列表
				TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.find(pServerNode->dwWorldID);
				if ( iter==m_mapWorldList.end())
				{
					SVoiceWorldNode addNode;
					addNode.dwWorldID = pServerNode->dwWorldID;
					addNode.strWorldName = szWorldName;
					//信息,以dwID为关键字,pair<TMAP_LoginUserInfo::iterator,bool> retPair=
					m_mapWorldList.insert(TMAP_VoiceWorldIDInfo::value_type(pServerNode->dwWorldID,addNode));

					// 触发新增游戏世界事件
					SEventVoiceWorldListAdd evenData;
					evenData.dwWorldID		= pServerNode->dwWorldID;	// 游戏世界ID
					FireEventExecute(EVENT_VOICE_WORLDLIST_ADD,(LPSTR)&evenData,sizeof(evenData));
				}
				else
				{
					iter->second.strWorldName = szWorldName;

					// 触发改游戏世界事件
					SEventVoiceWorldListEdit evenData;
					evenData.dwWorldID		= pServerNode->dwWorldID;	// 游戏世界ID
					FireEventExecute(EVENT_VOICE_WORLDLIST_EDIT,(LPSTR)&evenData,sizeof(evenData));
				}

				dwNameStrOffset += pServerNode->nNameStrlens;
				pServerNode++;
			}
		}
		break;
	}

#ifdef VOICE_PRINT_DEBUG_INF
	// 列出所有游戏世界信息
	PrintGameWorldListInfo();
#endif

}

/** 取定时器
@param   
@param   
@return  
*/
TimerAxis* ViewConnector::GetTimerAxis()
{
	return m_pTimerAxis;
}


// 取得服务器类型名称
const char* GetServerTypeString(DWORD dwServerType)
{
	//游戏软件服务器
	static const char *_gszGameSoftServerName[MSG_SERVERID_MAXID] =
	{
		_GT("未知游戏服务器"),       // 未定义
		_GT("场景服务器"),           // 场景服务器	    ［简称：S］
		_GT("网关服务器"),           // 网关服务器	    ［简称：G］
		_GT("登录服务器"),           // 登录服务器	    ［简称：L］
		_GT("世界服务器"),           // 世界服务器	    ［简称：W］
		_GT("游戏客户端"),           // 客户端			［简称：C］
		_GT("中心服务器"),           // 中心服务器	    ［简称：E］
		_GT("社会服务器"),           // 社会服务器	    ［简称：O］
		_GT("跨区桥服务器"),         // 跨区桥服务器	［简称：B］
		_GT("DB应用服务器"),         // DB应用服务器	［简称：D］
		_GT("通行证服务器"),         // 通行证服务器	［简称：P］
		_GT("数据监护服务器"),       // 数据监护服务器	［简称：G］
		_GT("语音服务器"),           // 语音服务器	    ［简称：V］	
		_GT("语音网关服"),           // 语音网关		［简称：VG］
	};
	if (dwServerType<MSG_SERVERID_MAXID)
	{
		return _gszGameSoftServerName[dwServerType];
	}
	return _gszGameSoftServerName[0];
}
