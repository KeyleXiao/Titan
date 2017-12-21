/*******************************************************************
** 文件名:	GatewayUser.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音网关用户
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "GatewayUser.h"
#include "GatewayList.h"
#include "GlobalVoiceServer.h"
#include "VoiceServerSetting.h"
#include <strstream>

// 打印该服务器信息
#define  DUMP_THIS_SERVER  ToString()

/// purpose: 构造函数
GatewayUser::GatewayUser(IConnection * conn) : m_pConnection(conn),m_dwID(0)
{
	const char * ip = 0;
	conn->GetRemoteAddr(&ip,m_dwRemotePort);
	assert(ip!=0);
	m_szRemoteIP = ip;

	m_bIsPublic = false;						// 是否是公共区的
	m_bIsAlive	= true;							// 是否有心跳
	m_bIsAnswer	= true;							// 是否有回应
	m_dwLastAnswerTime	= GetTickCount();		// 最后收到心跳Tick时间

	m_dwAddTime = (DWORD)time(NULL);			// 连入时间
	m_dwWorldID	= 0;			// 服务器所在游戏世界ID
	m_wServerType = 0;			// 服务器类型,会根据参见消息结构头第一个字段转发消息给不同的服务器
	m_wSubID = 0;				// 第n号服务器
	m_dwStateFlag = 0;			// 服务器状态

	memset(m_szName,0,sizeof(m_szName));

    string strX = DUMP_THIS_SERVER; //  pszThisServer =  DUMP_THIS_SERVER.c_str();
	TraceLn(_GT("连入网关：") << a2utf8(strX.c_str()));

}

/// purpose: 析构函数
GatewayUser::~GatewayUser()
{

}

/// purpose: 连接断开
void GatewayUser::OnDisconnected( IConnection * conn,DWORD reason )
{
	TraceLn(_GT("网关连接断开：") << DUMP_THIS_SERVER);
	Release();
}

/// purpose: 网关用户收包
void GatewayUser::OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
{
	assert(conn==m_pConnection);
	
	((GlobalVoiceServer*)gGlobalServer)->m_dwRecvSpeedGateway += dwDataLen;		// 收包速度

	if ( dwDataLen<sizeof(SGameMsgHead))
	{
		WarningLn(_GT("网关收包长度太小"));
		return;
	}

	Assert(pData != NULL);
	SGameMsgHead* head = (SGameMsgHead*)pData;
	ulong moduleId = head->byKeyModule;
	uchar* data = (uchar*)pData + sizeof(SGameMsgHead);
	size_t new_len =(size_t)(dwDataLen-sizeof(SGameMsgHead));
	if (new_len == 0) data = 0;

	gGlobalServer->onGatewayMessage((*this), head->byKeyAction, head, data, new_len);
}

/// purpose: 连接出错
void GatewayUser::OnError( IConnection * conn,DWORD dwErrorCode )
{
    string strServer = DUMP_THIS_SERVER;
	TraceLn(_GT("网关连接出错：ErrorCode[") << dwErrorCode << _GT("]，")<< a2utf8(strServer.c_str()));
	Release();
}

/// purpose: 发送数据到网关服务器
bool GatewayUser::SendData( const char * pData,DWORD dwDataLen,DWORD dwFlag)
{
	Assert(m_pConnection);
	if (!m_pConnection)	{	return false;	}

	((GlobalVoiceServer*)gGlobalServer)->m_dwSendSpeedGateway += dwDataLen;		// 发包速度
	
	return m_pConnection->SendData(pData,dwDataLen,dwFlag);
}

/// purpose: 发送数据到客户端
bool GatewayUser::SendDataToClient( DWORD dwClientID, const char * pData, DWORD dwLen, BYTE nSendChennel )
{
	if ( 0==dwClientID || NULL==pData || 0==dwLen)
	{
		return false;
	}

	// 网关ID无效
	DWORD dwGatewayID = ((VClientIDInfo *)&dwClientID)->dwGatewayID;
	if( dwGatewayID != m_dwGatewayID )
	{
		return false;
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
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_VOICEGATE;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_SENDDATA;

	// 接受数据的客户端ID
	SMsgVoiceSendData sendData;
	sendData.nSendChennel = nSendChennel;	// 发送数据通道类型 EMVoiceSendChannelType
	sendData.dwServerID = dwClientID;		// 客户端ID
	sendData.nDataLens = dwLen;				// 数据长度

	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoiceSendData));
	obufData.push_back(pData, dwLen);

	((GlobalVoiceServer*)gGlobalServer)->m_dwSendSpeedClient += dwLen;		// 发包速度

	bool bResult = SendData(obufData.data(),obufData.size());

	// 是new的内存,用完释放
	if (bIsNewMemory)
	{
		delete [](pBufData);
		pBufData=NULL;
	}

	return bResult;
}

/// purpose: 广播数据到客户端
bool GatewayUser::BroadcastDataToClient( DWORD * pClientArray, WORD wClientNum, const char * pData, DWORD dwLen, BYTE nSendChennel )
{
	DWORD dwMaxSendLens =  dwLen+sizeof(SGameMsgHead)+sizeof(SMsgVoiceBroadcastData)+wClientNum*sizeof(DWORD)+128;
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
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_VOICEGATE;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_BROADCASTDATA;

	// 接受数据的客户端个数
	SMsgVoiceBroadcastData sendData;
	sendData.nSendChennel = nSendChennel;			// 发送数据通道类型 EMVoiceSendChannelType
	sendData.dwServerCounts = wClientNum;			// 软件服务器ID列表个数
	sendData.nDataLens		= dwLen;				// 数据长度

	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoiceBroadcastData));
	obufData.push_back(pClientArray, wClientNum*sizeof(DWORD));			// 软件服务器ID列表
	obufData.push_back(pData, dwLen);

	((GlobalVoiceServer*)gGlobalServer)->m_dwSendSpeedClient += ((DWORD)wClientNum*dwLen);		// 发包速度

	bool bResult = SendData(obufData.data(),obufData.size());

	// 是new的内存,用完释放
	if (bIsNewMemory)
	{
		delete [](pBufData);
		pBufData=NULL;
	}

	return bResult;
}

/// purpose: 返回网关的连接ID
DWORD GatewayUser::GetServerID()
{
	return m_dwID;
}

/// purpose: 设置网关连接ID
void GatewayUser::SetServerID(DWORD dwID)
{
	m_dwID = dwID;
}

/// purpose: 获得网关ID
DWORD GatewayUser::GetGatewayID()
{
	return m_dwGatewayID;
}

/// purpose: 设置网关ID
void GatewayUser::SetGatewayID(DWORD dwGatewayID)
{
	m_dwGatewayID = dwGatewayID;
}

/// purpose: 获取服务器类型
WORD GatewayUser::GetServerType()
{
	return m_wServerType;
}

/// purpose: 获得网关服务器IP
const char * GatewayUser::GetRemoteIP()
{
	return m_szRemoteIP.c_str();
}

/// purpose: 获得网关服务器端口
DWORD GatewayUser::GetRemotePort()
{
	return m_dwRemotePort;
}

/// purpose: 网关信息
std::string GatewayUser::ToString()
{
	ostrstream buffer;
	buffer << " 网关["<<m_szName<<",ID="<< m_dwID << "][" << m_szRemoteIP <<":" << m_dwRemotePort <<"]" << std::ends;
	return buffer.str();
}

/// purpose: 释放操作
void GatewayUser::Release()
{
	IGlobalVoiceServer* pGlobalServer = gGlobalServer;
	if (pGlobalServer!=NULL)
	{
		pGlobalServer->OnGatewayUserLeave(this);
	}

	// 从列表删除用户
	GatewayList::getInstance().DelUser(this);

	// 释放连接
	if ( m_pConnection )
	{
		m_pConnection->Release();
		m_pConnection=0;
	}

	delete this;
}


IConnection * GatewayUser::GetConnection()
{
	return m_pConnection;
}
