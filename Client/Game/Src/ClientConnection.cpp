/*******************************************************************
** 文件名: ClientConnection.cpp 
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 10/16/2017
** 版  本: 1.0
** 描  述: 客户端连接


1、net.h里新增了CreateKCPConnection方法创建KCP连接，返回的接口和TCP的一样，可以完全替换使用
2、要改下网关服务器，增加KCP端口的配置
3、客户端默认实用KCP连接，如果连接不上，则使用TCP，最好是在登陆界面可以勾选
4、netdef.h里新增了一个SEND_DATA_FLAG,NO_ACK,这种包可以丢包也不保证顺序
移动数据包有2种，一种是启停，这种不能丢，一种是定时发送的同步，这种包可以设置NO_ACK让他可以丢失，
服务器广播这种包时，也把标志位设置为NO_ACK，可节省大量带宽。但是因为不保证顺序，
客户端收到时需要判断一下TICK数，过期的同步包直接丢弃
********************************************************************/

#include "stdafx.h"
#include "IClientGlobal.h"
#include "ClientConnection.h"
#include "IMessageDispatch.h"
#include "gatewayclient.h"

/////////////////////////////////////////ClientConnect/////////////////////////////////////////

// 构造函数
ClientConnection::ClientConnection()
    : m_wTcpPort(0)
    , m_wUdpPort(0)
    , m_pTcpConnection(0)
    , m_pUdpConnection(0)
    , m_isRelease(false)
{

}

// 析构函数
ClientConnection::~ClientConnection()
{

}

/** 释放
*/
void ClientConnection::Release()
{
    m_isRelease = true;
    if(m_pTcpConnection)
    {
        m_pTcpConnection->Disconnect();
        m_pTcpConnection->Release();
        m_pTcpConnection = NULL;
    }

    if(m_pUdpConnection)
    {
        m_pUdpConnection->Release();
        m_pUdpConnection = NULL;
    }
}

/** 创建
*/
bool ClientConnection::Create(const char* szTcpIpAddr, WORD wTcpPort, const char* szUdpIpAddr, WORD wUdpPort, bool firstUseUDP)
{
    m_isRelease = false;

    if(szTcpIpAddr && wTcpPort > 0 || szUdpIpAddr && wUdpPort > 0)
    {
        if(szTcpIpAddr) {
            m_szTcpAddr = szTcpIpAddr;
            m_wTcpPort = wTcpPort;
        }

        if(firstUseUDP) {
            if(!szUdpIpAddr) {
                firstUseUDP = false;
            }else {
                m_szUdpAddr = szUdpIpAddr;
                m_wUdpPort = wUdpPort;
            }
        }

        // 客户端关掉压缩,只支持加密
        EnableNetCompress(false);

        if(firstUseUDP)
        {
            if( CreateUDPConnection(szUdpIpAddr, wUdpPort) || CreateTCPConnection(szTcpIpAddr, wTcpPort))
            {
                return true;
            }
        }
        else
        {
            if( CreateTCPConnection(szTcpIpAddr, wTcpPort) )
                return true;
        }
    }

    return false;
}

bool ClientConnection::SendData(const char * pData,DWORD dwDataLen,DWORD dwFlag)
{
    if(m_pTcpConnection)
    {
        return m_pTcpConnection->SendData(pData, dwDataLen, dwFlag);
    }

    if(m_pUdpConnection)
    {
        return m_pUdpConnection->SendData(pData, dwDataLen, dwFlag);
    }

    return false;
}

void ClientConnection::SetHandler(IConnectionEventHandler * pHandler)
{
    m_pHandler = pHandler;
}

/**
@name         : 获取队列里未处理的数据包个数
@name         : 获取队列是发包,否则是收包
@return       : 
*/
unsigned long ClientConnection::GetPendingCount(bool bIsSend)
{
    if(m_pTcpConnection)
    {
        return m_pTcpConnection->GetPendingCount(bIsSend);
    }

    if(m_pUdpConnection)
    {
        m_pUdpConnection->GetPendingCount(bIsSend);
    }

    return 0;
}

/** 是否游戏网络连接中
*/
bool ClientConnection::IsConnected()
{
    if(m_pTcpConnection)
    {
        return m_pTcpConnection->IsConnected();
    }

    if(m_pUdpConnection)
    {
        return m_pUdpConnection->IsConnected();
    }

    return false;
}

unsigned long ClientConnection::GetNetworkProtocol()
{
    return (m_pTcpConnection) ? rkt::PROTOCOL_TCP : rkt::PROTOCOL_UDP;
}

// 创建TCP链接
IConnection* ClientConnection::CreateTCPConnection(const char* szIpAddr, WORD wPort)
{
    rkt::IConnection * pConn = rkt::CreateTCPConnection( 0, this, LUA_PACK );
    if ( !pConn )
    {
        return NULL;
    }

    m_pTcpConnection = pConn;
    pConn->setName("Client");

    // 连接服务器
    if(pConn->Connect(szIpAddr, wPort))
    {
        TraceLn("tcp connect ip="<<szIpAddr <<", ip="<<wPort);
    }

    return m_pTcpConnection;
}

// 创建Udp链接
IConnection* ClientConnection::CreateUDPConnection(const char* szIpAddr, WORD wPort)
{
    rkt::IConnection * pConn = rkt::CreateKCPConnection(false, 0, this, LUA_PACK );
    if ( !pConn )
    {
        return NULL;
    }

    pConn->setName("Client");

    // 连接服务器
    if( pConn->Connect(szIpAddr, wPort) )
    {
        m_pUdpConnection = pConn;
        TraceLn("udp connect ip="<<szIpAddr <<", ip="<<wPort);
    }

    return m_pUdpConnection;
}

void ClientConnection::SetEventHandler(IConnectionEventHandler * pHandler)
{
    m_pHandler = pHandler;
}

/////////////////////////////////////////IConnectionEventHandler/////////////////////////////////////////
/**
@brief              : 收到被动连接
@param pIncomingConn: 客户端连入的新连接
@param ppHandler    : 处理该连接的Handler
@warning            : 收到连接后一定要设置Handler
*/
void ClientConnection::OnAccept(IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler)
{

}

/**
@brief      : 连接成功
@param conn : 连接对象指针
*/
void ClientConnection::OnConnected(IConnection * conn)
{
    if(m_pHandler)
    {
        m_pHandler ->OnConnected(conn);
    }
}

/**
@brief      : 连接断开
@param conn : 连接对象指针
@param reason:断开原因(保留) 
*/
void ClientConnection::OnDisconnected(IConnection * conn,DWORD reason)
{
    if(m_pHandler)
    {
        m_pHandler ->OnDisconnected(conn, reason);
    }
}

/**
@brief        : 收到数据
@param conn   : 连接对象指针
@param pData  : 数据地址
@pram dwDataLen:数据长度
@return       :
*/
void ClientConnection::OnRecv(IConnection * conn,LPVOID pData,DWORD dwDataLen)
{
    // 派发消息
    IMessageDispatch * pMessageDispatch  = gClientGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        pMessageDispatch->dispatchMessage( pData,dwDataLen );
    }
}

/**
@name             : 网络错误事件
@brief            :
@param conn       : 出现错误的套接字
@param dwErrorCode: 错误码
*/
void ClientConnection::OnError(IConnection * conn,DWORD dwErrorCode)
{
    if(m_pHandler)
    {
        m_pHandler ->OnError(conn, dwErrorCode);

        // 客户端在释放中了
        if(m_isRelease)
        {
            return;
        }

        // 如果网关服已关闭不处理
        if(GatewayClient::getInstance().IsGatewayClosed())
        {
            return;
        }

        // 如果UDP连接有问题将启用TCP连接
        if(m_pUdpConnection)
        {
            m_pUdpConnection = NULL;

            CreateTCPConnection(m_szTcpAddr.c_str(), m_wTcpPort);
        }

    }
}


