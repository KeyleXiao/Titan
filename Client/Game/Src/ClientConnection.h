/*******************************************************************
** 文件名: ClientConnection.h 
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
#pragma once

#include "IClientConnection.h "

#include <string>
using namespace std;

class ClientConnection : public IClientConnection, public IConnectionEventHandler
{
public:
    /////////////////////////////////////////IClientConnection/////////////////////////////////////////

    /** 释放
    */
    virtual void        Release();

    /** 创建
    */
    virtual bool        Create(const char* szTcpIpAddr, WORD wTcpPort, const char* szUdpIpAddr, WORD wUdpPort, bool firstUseUDP);

    /** 发送消息包
    */
    virtual bool        SendData( const char * pData,DWORD dwDataLen,DWORD dwFlag);

    /** 设置接收消息的处理对象
    */
    virtual void        SetHandler( IConnectionEventHandler * pHandler );

    /**
    @name         : 获取队列里未处理的数据包个数
    @name         : 获取队列是发包,否则是收包
    @return       : 
    */
    virtual unsigned long       GetPendingCount(bool bIsSend);

    /** 是否游戏网络连接中
    */
    virtual bool        IsConnected();

    /** 获取网络协议类型
    */
    virtual unsigned long       GetNetworkProtocol();
	/////////////////////////////////////////IConnectionEventHandler/////////////////////////////////////////
	/**
	@brief              : 收到被动连接
	@param pIncomingConn: 客户端连入的新连接
	@param ppHandler    : 处理该连接的Handler
	@warning            : 收到连接后一定要设置Handler
	*/
	virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler );

	/**
	@brief      : 连接成功
	@param conn : 连接对象指针
	*/
	virtual void OnConnected( IConnection * conn );

	/**
	@brief      : 连接断开
	@param conn : 连接对象指针
	@param reason:断开原因(保留) 
	*/
	virtual void OnDisconnected( IConnection * conn,DWORD reason );

	/**
	@brief        : 收到数据
	@param conn   : 连接对象指针
	@param pData  : 数据地址
	@pram dwDataLen:数据长度
	@return       :
	*/
	virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen );

	/**
	@name             : 网络错误事件
	@brief            :
	@param conn       : 出现错误的套接字
	@param dwErrorCode: 错误码
	*/
	virtual void OnError( IConnection * conn,DWORD dwErrorCode );

    /////////////////////////////////////////ClientConnect/////////////////////////////////////////
    // 构造函数
    ClientConnection();

    // 析构函数
    ~ClientConnection();


    
    // 创建TCP链接
    IConnection* CreateTCPConnection(const char* szIpAddr, WORD wPort);

    // 创建Udp链接
    IConnection* CreateUDPConnection(const char* szIpAddr, WORD wPort);

    virtual void SetEventHandler( IConnectionEventHandler * pHandler );

private:
    WORD                        m_wTcpPort;
    string                      m_szTcpAddr;
    IConnection*                m_pTcpConnection;

    WORD                        m_wUdpPort;
    string                      m_szUdpAddr;
    IConnection*                m_pUdpConnection;

    IConnectionEventHandler *   m_pHandler;
    bool                        m_isRelease;
};