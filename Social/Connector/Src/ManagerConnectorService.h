/*******************************************************************
** 文件名:	ManagerConnectorService.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-07-09
** 版  本:	1.0
** 描  述:	管理器服务器连接服务			
********************************************************************/

#pragma once

#include "IManagerConnector.h"

#include "IServiceMgr.h"
#include "IFramework.h"
#include "ISocialGlobal.h"

#include "IMessageDispatch.h"
#include "net/Reactor.h"
#include "net/net.h"

#include "SocialGlobalHelper.h"


using namespace Manager;


/**
	职责: 
	1.管理器服务器连接服务
*/
class ManagerConnectorService : public IManagerConnectorService, public IManagerMessageHandler, public IManagerEventHandler, public EventHandler
{
#undef	CONNECTOR_NETWORK_USERTYPE
#define CONNECTOR_NETWORK_USERTYPE		NetworkUserType_User4

protected:
	IManagerConnector *  m_pConnector;

public:
	ManagerConnectorService() : m_pConnector()
	{
	}

	virtual ~ManagerConnectorService()
	{
		Stop();
	}

	bool Start(const char * ip, int port)
	{
		// 挂接网络事件
		Reactor * reactor = gSocialGlobal->getFramework()->get_reactor();
		if ( reactor==0 )
			return false;

		HANDLE hNetEvent = GetNetworkEventHandle(CONNECTOR_NETWORK_USERTYPE);
		reactor->AddEvent(hNetEvent);
		reactor->RegisterEventHandler( hNetEvent,this);

		m_pConnector = CreateManagerConnectorProc(this, (TimerAxis *)gSocialGlobal->getTimerService()->getAxis());
		if ( m_pConnector==0 )
		{
			ErrorLn(_GT("创建管理器连接器失败!"));
			return false;
		}

		m_pConnector->Create(MSG_ENDPOINT_SOCIAL, gSocialGlobal->getServerID(),_GT("社会服务器"));
		if(!m_pConnector->Connect(ip,port, CONNECTOR_NETWORK_USERTYPE))
		{
			TraceLn(_GT("连接子服务器管理器失败!"));
			return false;
		}
		TraceLn(_GT("连接子服务器管理器成功!"));


		// 订阅 要求更新实时数据事件
		m_pConnector->registerEventHandler(EVENT_MG_REAL_TIME_DATA,static_cast<IManagerEventHandler *>(this));
		
		// 管理器连接器服务已启动
		IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
		if(pEventEngine)
		{
			pEventEngine->FireExecute(EVENT_SYSTEM_MANAGECONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
		}

		return true;
	}

	void Stop()
	{
		// 管理器连接器服务已关闭
		IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
		if(pEventEngine)
		{
			pEventEngine->FireExecute(EVENT_SYSTEM_MANAGECONNECTOR_STOP, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
		}

		// 挂接网络事件
		Reactor * reactor = gSocialGlobal->getFramework()->get_reactor();
		if ( reactor )
		{
			HANDLE hNetEvent = GetNetworkEventHandle(CONNECTOR_NETWORK_USERTYPE);
			reactor->RemoveEvent( hNetEvent );
		}

		// 关闭连接器
		if (m_pConnector)
		{
			m_pConnector->SetServerState(MG_SERVERSTATE_STOPING);// 正在关闭

			// 取消订阅 要求更新实时数据事件
			m_pConnector->unregisterEventHandler(EVENT_MG_REAL_TIME_DATA,static_cast<IManagerEventHandler *>(this));

			EmphasisLn(_GT("关闭子服务器管理器连接!"));
			m_pConnector->Release();
			m_pConnector = NULL;
		}
	}

	virtual void release()
	{
		Stop();
		delete this;
	}

	virtual void OnEvent( HANDLE event )
	{
		DispatchNetworkEvents(100, CONNECTOR_NETWORK_USERTYPE);
	}
	

	////////////// IManagerConnectorService ////////////////////////////////////////////////////////////
	/** 取得本地服务器在子服务器上的ID
	*/
	virtual DWORD  getLocalServerID()
	{
		return m_pConnector->GetLocalServerID();
	}

	/** 直接发送数据给子服务器
	@param pData         : 要发送的数据
	@param dwLen         : 数据长度
	*/
	virtual void  sendData(ibuffer & msg)
	{
		return m_pConnector->SendData(msg.data(), msg.size());
	}

	/** 发给主服务器数据
	@param pData         : 要发送的数据
	@param dwLen         : 数据长度
	*/
	virtual void  sendDataToMain(ibuffer & msg)
	{
		return m_pConnector->SendDataToMain(msg.data(), msg.size());
	}

	/** 发给根服务器数据
	@param pData         : 要发送的数据
	@param dwLen         : 数据长度
	*/
	virtual void  sendDataToRoot(ibuffer & msg)
	{
		return m_pConnector->SendDataToRoot(msg.data(), msg.size());
	}

	/** 请求启动
	@param dwStartFlag   : 启动标识 EMManagerStartServerFlag
	@param dwUserData    : 用户数据
	*/
	virtual bool requestStart(DWORD dwStartFlag,DWORD dwUserData)
	{
		return m_pConnector->RequestStart(dwStartFlag, dwUserData);
	}

	/** 请求关闭,如果没有请求，就关了服务器，会当作当机处理
	@param dwCloseFlag   : 关闭标识 EMManagerCloseServerFlag
	@param dwUserData    : 用户数据
	*/
	virtual bool requestClose(DWORD dwCloseFlag,DWORD dwUserData)
	{
		return m_pConnector->RequestClose(dwCloseFlag, dwUserData);
	}

	/** 取得在主服务器上的UID
	*/
	virtual DWORD  getServerUID()
	{
		return m_pConnector->GetServerUID();
	}

	/** 取得游戏世界UID
	*/
	virtual DWORD  getWorldID()
	{
		return m_pConnector->GetWorldID();
	}

	/** 取得游戏世界信息
	*/
	virtual SGameWorldConnectorInfo getGameWorldInfo()
	{
		return (*m_pConnector->GetGameWorldInfo());
	}

	/** 注册事件处理者
	@param wEventID 事件ID	参考 EMEventManager
	@param hander 事件处理者实例
	*/
	virtual bool registerEventHandler(WORD wEventID,IManagerEventHandler* handler)
	{
		return m_pConnector->registerEventHandler(wEventID, handler);
	}

	/** 注销事件处理者
	@param wEventID 事件ID	参考 EMEventManager
	@param  hander 事件处理者实例
	*/
	virtual bool unregisterEventHandler(WORD wEventID,IManagerEventHandler* handler)
	{
		return m_pConnector->unregisterEventHandler(wEventID, handler);
	}

	/** 设置实时数据
	DWORD dwDataID		: 实时数据类型ID,不同的服务器类型用不用的ID 参考 EMManagerRealTimeDataLogin
	int nValue			: 数值
	@return  是否成功设置
	*/
	virtual void setRealTimeData(DWORD dwDataID,int nValue)
	{
		m_pConnector->SetRealTimeData(dwDataID, nValue);
	}

	/** 取得实时数据
	DWORD dwDataID		: 实时数据类型ID
	@return  int nValue	: 数值
	*/
	virtual int getRealTimeData(DWORD dwDataID)
	{
		return m_pConnector->GetRealTimeData(dwDataID);
	}

	/** 设定服务器状态
	DWORD dwStateFlag:服务器状态类型定义 EMManagerServerStateFlag
	*/
    virtual void setServerState(DWORD dwStateFlag)
    {
        m_pConnector->SetServerState(dwStateFlag);
    }

	///////// IManagerMessageHandler /////////////////////////////////////////////////////////////////

	/**
	@name         : 处理其他服务器通过子服务器转发的消息
	@brief        : 具体内容不清楚
	@param actionId  : 消息码
	@param head  : 消息头
	@param data  : 消息内容
	@param len:消息长度
	*/
	virtual void HandleManagerMessage(rkt::ulong actionId, SManagerMsgHead* head, LPSTR data, size_t len)
	{
		switch(actionId)
		{
		case MSG_MG_HANDSHAKE_RESPONSE:		//  子服务器连接成功回应握手消息
			{
				if (m_pConnector)
				{
					m_pConnector->SetServerState(MG_SERVERSTATE_STARTING);// 正在启动
				}
			}
			break;

        case MSG_MG_REQUESTSTART_RESULT:	// 软件服务器向主服务器请求启动结果消息,调用 RequestStart()后收到
            {
                if(data==NULL|| len != sizeof(SMsgManagerRequestStartResult_M))
                {
                    ErrorLn(_GT("向主服务器请求启动结果数据出错,数据有误！"));
                    return;
                }

                gSocialGlobal->getNetMessageHandler()->handleManagerResourceResult(data, len);
            }
            break;

        case MSG_MG_SENDSCRIPTDATA:		// 收到服务器启动脚本文件数据
            {
		        gSocialGlobal->getNetMessageHandler()->handleManagerResourceData(data, len);
            }
            break;

		case MSG_MG_CONTROLSOFTSERVER:			// 操作软件服务器消息
			{
				if(len < sizeof(SMsgManagerControlSoftServer_C))
				{
					break;
				}

				SMsgManagerControlSoftServer_C * pMsg = (SMsgManagerControlSoftServer_C *)(data);
				switch(pMsg->dwControlCode)
				{
				case MG_SoftControl_SetServerUID:	// 设定软件服务器的UID
					{
		                gSocialGlobal->getNetMessageHandler()->handleManagerSetGameWorldID();
					}
					break;
				}
			}
			break;

        default:break;
		}
	}

	///////// IManagerEventHandler /////////////////////////////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@retval buffer 
	*/
	virtual void OnManagerEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen)
	{

	}


};