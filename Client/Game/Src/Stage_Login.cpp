/*******************************************************************
** 文件名:	Stage_Login.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/21/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/

#include "stdafx.h"
#include "Stage_Login.h"
#include "net/net.h"
#include "IClientGlobal.h"
#include "ActorClient.h"
#include "IRenderView.h"
#include "NetMessageDef.h"
#include "Stage_Manager.h"
#include "TimerAxis.h"
#include "TimeSyncClient.h"
#include "gatewayclient.h"
#include "ITimerService.h"
#include "ILoginServerProtocol.h"
#include "net/characterencrypt.h"
#include "net/encryption_rotate.h"
#include "encrypt_rc4.h"
#include "IChatClient.h"
#include "EventDef.h"
#include "Api.h"
#include "DPassportModule.h"

Stage_Login::Stage_Login()
	: m_bHandshake(false)
	, m_nEncryptMethod(0)
	, m_nKeyLen(0)
	, m_pKeyData(0)
	, m_wEncryptKey(0)
    , m_bForceLogin(false)
{
    m_IpList.clear();
    m_PortList.clear();
    m_currentIP = "";
    m_currentPort = "";
}

Stage_Login::~Stage_Login()
{
    if ( m_pKeyData )
    {
        free( m_pKeyData );
        m_pKeyData = 0;
    }

    gClientGlobal->getTimerAxis()->KillTimer(FORACELOING_TIMER, this);
    gClientGlobal->getTimerAxis()->KillTimer(LOGIN_VERTFY_TIMER, this);
}

void Stage_Login::onEnter( void * context,int len )
{
	// 通知UI加载登陆界面
	gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LOAD_LOGIN,0,0,0,0 );

    IEntityClient * pEntityClient = gClientGlobal->getEntityClient();
    if(pEntityClient)
    {
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_USER_LOGIN,this );
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_PROGRESSING,this );
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_FORCE_LOGIN,this );
    }

    IMessageDispatch * pMessageDispatch = gClientGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        pMessageDispatch->registerMessageHandler(MSG_MODULEID_LOGIN, this);
    }

    /// 获取事件服务器
    IEventEngine * pEventEngine = gClientGlobal->getEventEngine();
    if (pEventEngine != NULL)
    {
        // 默认区变化了,新服务器IP,端口可从config.ini中取
        pEventEngine->Subscibe(this,EVENT_SYSTEM_DEFAULTSERVER_CHANGED,SOURCE_TYPE_SYSTEM,0,__FUNCTION__);
    }

    TraceLn("GatewayClient start ...");
    GatewayClient::getInstance().Start();

    // 清空IP和端口数据
    m_IpList.clear();
    m_PortList.clear();
    Ini ini("config.ini");
    std::string ServerIP = ini.getString("Network", "ip",",");
    std::string Port = ini.getString("Network", "port","5678,");

    StringHelper::split(m_IpList, ServerIP.c_str());
    StringHelper::split(m_PortList, Port.c_str());

    initializeNetSetting();

    if(context != NULL && len > 0)
    {
        ibuffer in(context, len);
        bool bAutoLogin = false;
        in >> bAutoLogin;

        if(bAutoLogin) 
        {
            // 重新登录
            GatewayClient::getInstance().ResetUCode();
            net_release();

            gameview_login& login = ActorClient::getInstance().getLoginInfo();
            onViewLogin(0, 0, &login);
        }
    }
}

void Stage_Login::update()
{

}

void Stage_Login::onLeave()
{
     gClientGlobal->getTimerAxis()->KillTimer(this);

    IEntityClient * pEntityClient = gClientGlobal->getEntityClient();
    if(pEntityClient)
    {
        pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_USER_LOGIN);
        pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_PROGRESSING);
        pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_FORCE_LOGIN); 
    }

	gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_LOGIN, this);

    /// 获取事件服务器
    IEventEngine * pEventEngine = gClientGlobal->getEventEngine();
    if (pEventEngine != NULL)
    {
        // 默认区变化了,新服务器IP,端口可从config.ini中取
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink *>(this), EVENT_SYSTEM_DEFAULTSERVER_CHANGED, SOURCE_TYPE_SYSTEM, 0);
    }

	if ( m_pKeyData )
    {
        free( m_pKeyData );
        m_pKeyData = 0;
    }
}

bool Stage_Login::onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam )
{
	// 登录
    switch ( eventID )
    {
    case GVIEWCMD_USER_LOGIN:           // 登陆账号
        {
            onViewLogin( nParam,strParam,ptrParam ); 
        }
        return true;

    case GVIEWCMD_PROGRESSING:          // 进入到读条阶段 
        {
            if ( nParam<100 )
                gClientGlobal->getMessageDispatch()->choke(true);
            else
                gClientGlobal->getMessageDispatch()->choke(false);
        }
        return true;

    case GVIEWCMD_FORCE_LOGIN:
        {
            m_bForceLogin = true;
            OnTimer(FORACELOING_TIMER);
            gClientGlobal->getTimerAxis()->SetTimer(FORACELOING_TIMER, 3000, this);
        }
        return true;

    default:
        ErrorLn( "Stage_Login::onViewEvent unhandled eventID=" << eventID );
        return false;
    }
}

void Stage_Login::OnTimer( unsigned long dwTimerID )
{
    switch(dwTimerID)
    {
    case FORACELOING_TIMER:
        {
            gClientGlobal->getChatClient()->showSystemMessage(CHAT_TIP_LOGIN_FORCE_LOGINING);

            sendLoginMsg();
        }
        break;

    case LOGIN_VERTFY_TIMER:             // 登录服务器校验账号
        {
            // 还没连上服务器
            if(!m_bHandshake)
            {
                break;
            }

            // 杀掉定时器
            gClientGlobal->getTimerAxis()->KillTimer(LOGIN_VERTFY_TIMER, this);

            // 发送登录信息
            sendLoginMsg();
        }
        break;

    default:
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void Stage_Login::OnConnected( IConnection * conn )
{

    Stage_Base::OnConnected(conn);
}

void Stage_Login::OnDisconnected( IConnection * conn,DWORD reason )
{
	//gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LOGIN_DISCONNECTED,reason,0,0, 0);
    Stage_Base::OnDisconnected(conn, reason);
}

void Stage_Login::OnError( IConnection * conn,DWORD dwErrorCode )
{
	//gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LOGIN_DISCONNECTED,dwErrorCode,0,0, 0);
    Stage_Base::OnError(conn, dwErrorCode);
}

void Stage_Login::onMessage(SNetMsgHead* head, void* data, size_t len)
{
	if ( head == NULL || data == NULL || len < 1)
	{
		return;
	}

	// 登录成功消息
    switch ( head->byKeyAction )
    {
    case MSG_LOGINSERVER_MESSAGE:
        {
			if ( len < sizeof(SMsgLoginServer_Message) )
			{
				ErrorLn(__FUNCTION__ << ": message length is error! len="<< len <<", size="<< sizeof(SMsgLoginServer_Message)  );
				return;
			}
			SMsgLoginServer_Message* pMsg = (SMsgLoginServer_Message*)data;
			if ( len != sizeof(SMsgLoginServer_Message) + pMsg->wMsgLen)
			{
				ErrorLn(__FUNCTION__ << ": message length is error! len="<< len << ",total_size="<< (sizeof(SMsgLoginServer_Message) + pMsg->wMsgLen) );
				return;
			}

			char* pContent = (char*)(pMsg+1);
			gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LOGIN_CONNECTING,0,0,0, 0);

            if (NoError == pMsg->dwErrorCode)
            {
                gClientGlobal->getChatClient()->showSystemMessage(CHAT_TIP_LOGIN_SERVER_MESSAGE, pContent);
            }
            else
            {
                cmd_open_servermsg_wnd cmdData;
                sstrcpyn(cmdData.szMsg,  pContent, sizeof(cmdData.szMsg));
                gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LOGIN_OPEN_SERVERMSG_VIEW,0,0,&cmdData, sizeof(cmdData));
            }
        }
        break;

    case MSG_LOGINSERVER_HANDSHAKE:
        {
            TraceLn("loginserver handshake ok...");
            onMsgHandshake( data,len );
        }
        break;

    case MSG_LOGINSERVER_SWITCH_STATE:
        {
            if(data == NULL || len < sizeof(SMsgLoginServer_SwitchState))
            {
                ErrorLn("login state receive invalid message, len="<< len <<", sizeof="<< sizeof(SMsgLoginServer_SwitchState)); 
                break;
            }
            SMsgLoginServer_SwitchState* pMsg = (SMsgLoginServer_SwitchState*)data;
            Stage_Manager *  pStageManager = gClientGlobal->getStageManager();
            if(pStageManager)
            {
                pStageManager->gotoState(pStageManager->loginServerStateSwitchLocal(pMsg->state));
            }
        }
        break;
    case MSG_LOGINSERVER_SERVER_MSG_INFO:
        {
            if(data == NULL || len < sizeof(SMsgLoginServer_ServerMsgInfo))
            {
                ErrorLn("login state receive invalid message, len="<< len <<", sizeof="<< sizeof(SMsgLoginServer_ServerMsgInfo)); 
                break;
            }
            SMsgLoginServer_ServerMsgInfo* pMsg = (SMsgLoginServer_ServerMsgInfo*)data;
            cmd_open_servermsg_wnd cmdData;
            sstrcpyn(cmdData.szMsg, pMsg->szMessage, sizeof(cmdData.szMsg));
            gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LOGIN_OPEN_SERVERMSG_VIEW,0,0,&cmdData, sizeof(cmdData));
        }
        break;
    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Stage_Login::sendLoginMsg()
{
    gameview_login & login = ActorClient::getInstance().getLoginInfo();
    if( strlen(login.access_token) > 0 )
    {
        executeWebLogin(login.access_token);
    }
    else
    {
        executeLogin();
    }

    startNetAssist();
}

void Stage_Login::executeLogin()
{
    obuf256 ob;

    Ini ini("config.ini");
    gameview_login & login = ActorClient::getInstance().getLoginInfo();

    SMsgLoginServer_Login data;

    strcpy( data.szUserName,login.account );
    strcpy( data.szPassword,login.password );
    data.dwGameWorldID  = login.worldid;
    data.bForceLogin    = m_bForceLogin;
    data.dwClientVer    = login.nClientVersion;
    // 硬件服务器 MAC地址
    sstrcpyn(data.szMacAddress, GetMacAddress(), sizeof(data.szMacAddress));
    data.dwSerialNO     = getComputerID();  // 客户端硬盘序列号信息
	data.dwPartnerID    = ini.getInt("Login","PartnerID",0);    // 合作伙伴ID

    // 账号名和密码进行加密
    CharacterEncrypt::getInstance().Encrypt( data.szUserName,m_wEncryptKey );
    CharacterEncrypt::getInstance().Encrypt( data.szPassword,m_wEncryptKey );

    const char * encrypt_data = 0;
    unsigned long encrypt_len = 0;

    // 对登录数据进行加密
    static encrypt_RC4				g_encryptor_rc4;			// RC4全局加密器
    static encryption_rotate_stream	g_encryptor_custom;         // 自定义加密器
    switch( m_nEncryptMethod )
    {
    case LOGIN_ENCRYPT_NO:
        {
            encrypt_data = (const char *)&data;
            encrypt_len  = sizeof(data);
        }
        break;
    case LOGIN_ENCRYPT_RC4:
        {
            void * pKey = 0;
            unsigned long nKeyLen = 0;
            g_encryptor_rc4.create_key( (const unsigned char *)m_pKeyData,m_nKeyLen,&pKey,nKeyLen );
            g_encryptor_rc4.encrypt( (const char *)&data,sizeof(data),&encrypt_data,encrypt_len,pKey );
            g_encryptor_rc4.release_key( pKey );
        }
        break;
    case LOGIN_ENCRYPT_CUSTOM:
        {
            void * pKey = 0;
            unsigned long nKeyLen = 0;
            g_encryptor_custom.create_key( (const unsigned char *)m_pKeyData,m_nKeyLen,&pKey,nKeyLen );
            g_encryptor_custom.encrypt( (const char *)&data,sizeof(data),&encrypt_data,encrypt_len,pKey );
            g_encryptor_custom.release_key( pKey );
        }
        break;
    case LOGIN_ENCRYPT_RSA:
        {

        }
        break;
    default:
        {
            ErrorLn("send invalid parameters for login request:"<< m_nEncryptMethod);
        }
        return;
    }

    SNetMsgHead head;
    head.byDestEndPoint = MSG_ENDPOINT_LOGIN;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byKeyModule    = MSG_MODULEID_LOGIN;
    head.byKeyAction    = MSG_LOGINSERVER_LOGIN;

    ob << head << data;

    net_senddata(ob.data(), ob.size());
}
//////////////////////////////////////////////////////////////////////////

void Stage_Login::executeWebLogin(std::string key)
{
    SNetMsgHead head;
    head.byDestEndPoint = MSG_ENDPOINT_LOGIN;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byKeyModule    = MSG_MODULEID_LOGIN;
    head.byKeyAction    = MSG_LOGINSERVER_CLIENT_LYCHECKINFO;

    // 取得当前版本号
    gameview_login & login = ActorClient::getInstance().getLoginInfo();
    int nClientVer = login.nClientVersion;

    SMsgLoginServer_LoginEx data;
    memset(&data,0,sizeof(SMsgLoginServer_LoginEx));
    data.dwClientVer = nClientVer;
    memset(data.szUserName,0,sizeof(data.szUserName));
    memset(data.szPassword,0,sizeof(data.szPassword));
    data.bForceLogin = m_bForceLogin;
    // 硬件服务器 MAC地址
    sstrcpyn(data.szMacAddress, GetMacAddress(), sizeof(data.szMacAddress));

    data.dwPartnerID = PASSPORT_ID_PARTNER_WEB; //web
    data.dwSerialNO = getComputerID();
    data.dwSize = key.length()+1;

    obuf1024 ob;
    ob.push_back(&head,sizeof(head));
    // 压入LoginEx
    ob.push_back(&data,sizeof(data));
    // 压入KEY
    ob<< key.c_str();

    net_senddata( ob.data(), ob.size() );
}

//////////////////////////////////////////////////////////////////////////
void Stage_Login::onViewLogin( int nParam,const char * strParam,void * ptrParam )
{
    if ( ptrParam==0 )
    {
        ErrorLn( "Stage_Login::onViewLogin ptr=0!" );
        return;
    }

    gameview_login & login = ActorClient::getInstance().getLoginInfo();
    login = *(gameview_login*)ptrParam;

    // 不是本地登陆用当前选择区IP登陆
    if (login.isLocalLogin == 0)
    {
        sstrcpyn(login.svrip, m_currentIP.c_str(),sizeof(login.svrip));
    }
    else
    {
        sstrcpyn(login.svrip, getLocalIpAddress().c_str(),sizeof(login.svrip));
    }

    if( net_isconnected() && strcmp(login.svrip, m_serverData.strLoginAddr.c_str()) )
    {
        // 服务器ip有改动的话 释放旧的连接
        net_release();
    }

    // 创建连接
    if( !net_isconnected() )
    {
        m_bHandshake = false;
        rkt::Ini config("config.ini");
        // 用当前的端口
        int nPort = StringHelper::toInt(m_currentPort);
        int nUdpPort = nPort+UDP_OFFSET;
        if ( net_create(login.svrip, nPort, login.svrip, nUdpPort, m_serverData.isOpenUDP) )
        {
            net_setHandler(this);

            m_serverData.strLoginAddr = login.svrip;
            m_serverData.nLoginPort = nPort;

            gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LOGIN_CONNECTING,0,0,0, 0);
            TraceLn( __FUNCTION__": connect to ip:" << login.svrip);
        }
        else
        {
            ErrorLn( __FUNCTION__": connect to server failed! ip=" << login.svrip);
            return;
        }
    }

    // 如果已经服务器连接好将直接发信息校验
    if (m_bHandshake)
    {
        sendLoginMsg();
    }
    else
    {
        // 开启定时器来登录
        gClientGlobal->getTimerAxis()->KillTimer(LOGIN_VERTFY_TIMER, this);
        gClientGlobal->getTimerAxis()->SetTimer(LOGIN_VERTFY_TIMER, 1000, this, INFINITY_CALL, __FILE__);
    }

    //TraceLn(__FUNCTION__": name="<< login.account<<" m_bHandshake ="<<m_bHandshake);
}

//////////////////////////////////////////////////////////////////////////
// 连接同步时间服务器
void Stage_Login::connectSyncTimerServer()
{
    TimeSyncClient * pTimeSyncClient = gClientGlobal->getTimeSyncer();
    if(!pTimeSyncClient)
    {
        return;
    }

    pTimeSyncClient->Stop();

    // 启动对时器
    rkt::Ini config("config.ini");
    gameview_login & login = ActorClient::getInstance().getLoginInfo();
    m_serverData.strSyncTimeAddr= config.getString( "Login","timeServerIp", login.svrip );
    m_serverData.nSyncTimePort  = config.getInt( "Login","timeServerPort",9700 );
    pTimeSyncClient->Start( m_serverData.strSyncTimeAddr.c_str(), m_serverData.nSyncTimePort, m_serverData.nSyncTimePort+1, gClientGlobal->getTimerAxis() );
    TraceLn("Connect SyncTimer Server...");
}
//////////////////////////////////////////////////////////////////////////
bool Stage_Login::startNetAssist()
{
    // 启动对时器
    connectSyncTimerServer();

    return true;
}
//////////////////////////////////////////////////////////////////////////
void Stage_Login::onMsgHandshake( void * data,size_t len )
{
    if ( data==NULL || len!=sizeof(SMsgLoginServer_Handshake) )
        return;

    SMsgLoginServer_Handshake * handshake = (SMsgLoginServer_Handshake *)data;

    // 登录服是否已发送加密数据
    m_bHandshake = true;

    if ( m_pKeyData )
    {
        free( m_pKeyData );
        m_pKeyData = 0;
    }

    m_wEncryptKey    = handshake->wEncryptKey;
    m_nEncryptMethod = handshake->byEncryptMethod;
    m_nKeyLen        = ENCRYPT_KEY_LEN;

    if ( m_nKeyLen > 0 )
    {
        m_pKeyData = (char *)malloc( m_nKeyLen );
        memcpy( m_pKeyData,handshake->szKeyData,m_nKeyLen );
    }

    /// 设置本进程为公共区
    if (handshake->bPublicGameWorld)
    {
        setPublicGameWorld();
    }
    else
    {
        setGeneralGameWorld();
    }

    /// 设置当前游戏世界ID
    setThisGameWorldID(handshake->nThisGameWorldID, true);

    /// 设置公共区游戏世界ID
    setPublicGameWorldID(handshake->nPublicGameWorldID, true);
}

bool Stage_Login::initializeNetSetting()
{
    // 读取服务器IP和地址
    Ini ini("config.ini");

    int nServerCount = min(m_IpList.size(), m_PortList.size());
    if ( nServerCount<=0 )  
    {
        std::string ServerIP = ini.getString("Network", "ip",",");
        std::string Port = ini.getString("Network", "port","5678,");
        StringHelper::split(m_IpList, ServerIP.c_str());
        StringHelper::split(m_PortList, Port.c_str());	
        nServerCount = min(m_IpList.size(), m_PortList.size());
    }

    // 开启UDP方式
    bool isOpenUDP = (ini.getInt("Network", "support_udp", 0)==1);
   // 回写INI文件
    ini.setInt("Network", "support_udp", isOpenUDP);
    if( m_serverData.isOpenUDP = isOpenUDP ) {
        TraceLn("client open udp connect server.");
    }

    int index = 0; 
    rktRangeRand(index, 0, nServerCount-1);
    m_currentIP = *(m_IpList.begin() + index);
    m_currentPort = *(m_PortList.begin() + index);

    // 修改到登陆
    m_IpList.erase( m_IpList.begin()+index );
    m_PortList.erase( m_PortList.begin()+index );
    return true;
}


bool Stage_Login::changeServer()
{
    initializeNetSetting();

    // 登录服是否已发送加密数据
    m_bHandshake = false;

    const char *   szIP  = m_currentIP.c_str();
    unsigned short nPort = StringHelper::toInt(m_currentPort);

    // 释放老的连接
    if(net_isconnected())
    {
        net_release();
    }

    if( !net_create(szIP, nPort, szIP, nPort+UDP_OFFSET, m_serverData.isOpenUDP) )
    {
        ErrorLn("client connect to server failed! ip=" << szIP);
        return false;
    }
    
    net_setHandler(this);

    m_serverData.strLoginAddr = szIP;
    m_serverData.nLoginPort = nPort;

    gameview_login & login = ActorClient::getInstance().getLoginInfo();
    // 不是本地登陆用当前选择区IP登陆
    if (login.isLocalLogin == 0)
    {
        sstrcpyn(login.svrip, szIP,sizeof(login.svrip));
    }
    else
    {
        sstrcpyn(login.svrip, getLocalIpAddress().c_str(),sizeof(login.svrip));
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LOGIN_CONNECTING,0,0,0, 0);
    TraceLn("client connect to ip:" << m_serverData.strLoginAddr.c_str());

    return true;
}


bool Stage_Login::onGameServerChanged(LPCSTR pData, int nLen)
{
    if (nLen != sizeof(event_system_default_server_changed))
    {
        ErrorLn(__FUNCTION__": error lenth!!");
        return false;
    }

    // 换区数据结构信息
    event_system_default_server_changed * pResult = (event_system_default_server_changed *)pData;

    // 从当前登录服下线，和网关断线
    net_release();

    // 把登陆检测握手信息的定时器关闭
    gClientGlobal->getTimerAxis()->KillTimer(LOGIN_VERTFY_TIMER, this);

    // 换区不变化版本，此时需要重连网关服务器
    if (!pResult->bIsVerChange)
    {
        // 清空IP和端口数据
        m_IpList.clear();
        m_PortList.clear();

        // 重连指定目标网关服务器
        return changeServer();
    }

    return true;
}

void Stage_Login::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch(wEventID)
    {
    case EVENT_SYSTEM_DEFAULTSERVER_CHANGED:			// 默认区变化了,新服务器IP,端口可从config.ini中取
        {
            if (bSrcType==SOURCE_TYPE_SYSTEM )
            {
                onGameServerChanged(pszContext, nLen);
            }
        }
        break;
    default:
        {

        }
        break;
    }
}

string Stage_Login::getLocalIpAddress()
{
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
        return "";
    char local[255] = {0};
    gethostname(local, sizeof(local));
    hostent* ph = gethostbyname(local);
    if (ph == NULL)
        return "";
    in_addr addr;
    memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); // 这里仅获取第一个ip
    string localIP;
    localIP.assign(inet_ntoa(addr));
    WSACleanup();
    return localIP;
}