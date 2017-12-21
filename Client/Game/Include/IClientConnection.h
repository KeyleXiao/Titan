/*******************************************************************
** 文件名: IClientConnection.h 
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 10/16/2017
** 版  本: 1.0
** 描  述: 

********************************************************************/

#ifndef __I_CLIENT_CONNECTION_H__
#define __I_CLIENT_CONNECTION_H__


/** 
*/
struct IClientConnection
{
    /** 释放
    */
    virtual void                Release() = 0;

    /** 创建
    */
    virtual bool                Create(const char* szTcpIpAddr, WORD wTcpPort, const char* szUdpIpAddr, WORD wUdpPort, bool firstUseUDP=true) = 0;

    /** 发送消息包
    */
    virtual bool                SendData( const char * pData,DWORD dwDataLen,DWORD dwFlag = 0) = 0;

    /** 设置接收消息的处理对象
    */
    virtual void                SetHandler( IConnectionEventHandler * pHandler ) = 0;

    /**
    @name         : 获取队列里未处理的数据包个数
    @name         : 获取队列是发包,否则是收包
    @return       : 
    */
    virtual unsigned long       GetPendingCount(bool bIsSend=false) = 0;

    /** 是否游戏网络连接中
    */
    virtual bool                IsConnected() = 0;

    /** 获取网络协议类型
    */
    virtual unsigned long       GetNetworkProtocol() = 0;
};

// 创建网络连接
#define net_create(szTcpAddr, wTcpPort, szUdpAddr, wUdpPort, firstUseUDP)   gClientGlobal->getNetConnection()->Create((szTcpAddr), (wTcpPort), (szUdpAddr), (wUdpPort), (firstUseUDP))

// 网络端口并释放
#define net_release()                       gClientGlobal->getNetConnection()->Release()
#define net_isconnected()                   gClientGlobal->getNetConnection()->IsConnected()
// 
#define net_setHandler(handler)             gClientGlobal->getNetConnection()->SetHandler(handler)
// 发送数据
#define net_senddata(data, size)            gClientGlobal->getNetConnection()->SendData((data), (size))
#define net_senddata_flag(data,size,flag)   gClientGlobal->getNetConnection()->SendData((data), (size), (flag))
#endif // __I_CLIENT_CONNECTION_H__