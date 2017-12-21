/*******************************************************************
** 文件名:	Stage_Login.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/24/2014
** 版  本:	1.0
** 描  述:	登陆状态管理

********************************************************************/

#pragma once

#include "Stage_Base.h"
#include "IMessageDispatch.h"
#include "TimerHandler.h"
#include "MatchDef.h"
#include "GameViewDef.h"

/**
	游戏状态 - 登陆状态，负责发起服务器连接，并发送用户的登陆信息
*/
class Stage_Login : public Stage_Base,public IGameViewEventHandler,public IMessageHandler, public TimerHandler, public IEventExecuteSink
{
    enum
    {
        FORACELOING_TIMER,              // 强制登录
        LOGIN_VERTFY_TIMER,             // 登录服务器校验账号
    };
public:
    Stage_Login();

    ~Stage_Login();

	// 进入状态
	virtual void onEnter( void * context,int len );

	// 逻辑更新
	virtual void update();

	// 离开状态
	virtual void onLeave();

    virtual bool onGameServerChanged(LPCSTR pData, int nLen);

    //////////////////// IEventExecuteSink ////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void OnConnected( IConnection * conn );

	virtual void OnDisconnected( IConnection * conn,DWORD reason );

	virtual void OnError( IConnection * conn,DWORD dwErrorCode );

	virtual bool onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );

	////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void onMessage( SNetMsgHead* head, void* data, size_t len);

    virtual void OnTimer( unsigned long dwTimerID );

protected:
    void onViewLogin( int nParam,const char * strParam,void * ptrParam );

    void onMsgHandshake( void * data,size_t len );

private:
	// 登录
	void sendLoginMsg();

    void executeLogin();
    void executeWebLogin(std::string key);

    // 连接同步时间服务器
    void connectSyncTimerServer();

    bool startNetAssist();

    bool initializeNetSetting();

    bool changeServer();

    string getLocalIpAddress();
private:
    struct SLoginServerData
    {
        string  strLoginAddr;
        int     nLoginPort;
        string  strSyncTimeAddr;
        int     nSyncTimePort;
        bool    isOpenUDP;
        SLoginServerData()
        {
            nLoginPort = nSyncTimePort = 0;
            isOpenUDP = false;
        }
    };
    SLoginServerData   m_serverData;

    bool        m_bHandshake;
    int         m_nEncryptMethod;	// 登陆服务器要求的加密方式
    BYTE        m_nKeyLen;			// 登陆服务器提供的密钥长度
    char   *    m_pKeyData;			// 登陆服务器提供的密钥内容
    int         m_wEncryptKey;      // 加密密钥

    bool        m_bForceLogin;      // 是否强制登录

    std::vector<std::string> m_IpList;
    std::vector<std::string> m_PortList;
    std::string		m_currentIP;
    std::string		m_currentPort;
};
