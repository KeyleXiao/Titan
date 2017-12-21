#pragma once
#include "DGlobalMessage.h"
#include "RoomEvent.h"
#include "IServiceMgr.h"
#include "IFramework.h"
#include "ISocialGlobal.h"
#include "IVoiceConnector.h"
#include "IVoiceConnectorService.h"
#include "INetMessageHandler.h"
#include "ITimerService.h"
#include "net/Reactor.h"
#include "net/net.h"
#include <WinSock.h>

using namespace Voice;

class VoiceConnectorService : public IVoiceConnectorService,public IVoiceMessageHandler,
    public EventHandler,public IMessageHandler,public ITransmitHandler,public IEventExecuteSink
{
protected:
	IVoiceConnector *    m_pConnector;
    string               m_strIp;
    int                  m_nPort;

public:
	VoiceConnectorService() : m_pConnector()
        , m_nPort(0)
	{
	}

	virtual ~VoiceConnectorService()
	{
		Stop();
	}

	bool Start( const char * ip,int port )
	{
		// 挂接网络事件
		Reactor * reactor = gSocialGlobal->getFramework()->get_reactor();
		if ( reactor==0 )
			return false;

		HANDLE hNetEvent = GetNetworkEventHandle(NetworkUserType_User5);
		reactor->AddEvent(hNetEvent);
        reactor->RegisterEventHandler( hNetEvent,this, __FUNCTION__);

        m_strIp = ip;
        m_nPort = port;

		m_pConnector = CreateVoiceConnectorProc((TimerAxis *)(gSocialGlobal->getTimerService()->getAxis()));
        m_pConnector->registerMessageHandler(MSG_MODULEID_COMMON, this);
        gSocialGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_VOICE, this);
        gSocialGlobal->getMessageDispatch()->registerTransmitHandler(MSG_MODULEID_VOICE, this);

        // 进入房间事件
        gSocialGlobal->getEventEngine()->Subscibe(this, EVENT_ROOM_ADD_ACTOR, SOURCE_TYPE_ROOM, 0, __FUNCTION__);
        // 离开房间事件
        gSocialGlobal->getEventEngine()->Subscibe(this, EVENT_ROOM_DEL_ACTOR, SOURCE_TYPE_ROOM, 0, __FUNCTION__);
        // 房间内换阵营事件
        gSocialGlobal->getEventEngine()->Subscibe(this, EVENT_ROOM_ACTOR_CHANGE_CAMP, SOURCE_TYPE_ROOM, 0, __FUNCTION__);

		if ( m_pConnector==0 )
		{
			ErrorLn(_GT("创建语音服务器连接器失败!"));
			return false;
		}
        m_pConnector->Create(getThisGameWorldID(), MSG_ENDPOINT_SOCIAL, gSocialGlobal->getServerID(), _GT("社会服"), NULL);
		//m_pConnector->RegisterServer(_GT("场景服务器"),0,MSG_ENDPOINT_SCENE );

		return true;
	}

	void Stop()
	{
		// 挂接网络事件
		Reactor * reactor = gSocialGlobal->getFramework()->get_reactor();
		if ( reactor )
		{
			HANDLE hNetEvent = GetNetworkEventHandle(NetworkUserType_User5);
			reactor->RemoveEvent( hNetEvent );
		}

		if ( m_pConnector )
		{
            m_pConnector->unregisterMessageHandler(MSG_MODULEID_COMMON);
            gSocialGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_VOICE);
			ErrorLn(_GT("正在断开与语音服务器连接!") );
			m_pConnector->Release();
			m_pConnector = 0;
		}

        // 进入房间事件
        gSocialGlobal->getEventEngine()->UnSubscibe(this, EVENT_ROOM_ADD_ACTOR, SOURCE_TYPE_ROOM, 0);
        // 离开房间事件
        gSocialGlobal->getEventEngine()->UnSubscibe(this, EVENT_ROOM_DEL_ACTOR, SOURCE_TYPE_ROOM, 0);
        // 房间内换阵营事件
        gSocialGlobal->getEventEngine()->UnSubscibe(this, EVENT_ROOM_ACTOR_CHANGE_CAMP, SOURCE_TYPE_ROOM, 0);
	}


	virtual void OnEvent( HANDLE event )
	{
		DispatchNetworkEvents(100,NetworkUserType_User5);
	}

	virtual bool  connectServer(void)
    {
        if(!m_pConnector->Connect(m_strIp.c_str(), m_nPort, NetworkUserType_User5))
        {
            ErrorLn(__FUNCTION__ << ", connect VoiceServer failed, ip=" << m_strIp.c_str() << ", port=" << m_nPort);
            return false;
        }
        TraceLn(__FUNCTION__":Connect VoiceServer success!");
        return true;
    }

	virtual void  SendData(const char * pData,DWORD dwLen)
    {
        m_pConnector->SendData(pData, dwLen);
    }

	virtual bool registerMessageHandler(DWORD moduleId, IVoiceMessageHandler* handler)
    {
        return m_pConnector->registerMessageHandler(moduleId, handler);
    }

	virtual bool unregisterMessageHandler(DWORD moduleId)
    {
        return m_pConnector->unregisterMessageHandler(moduleId);
    }

	virtual bool registerEventHandler(WORD wEventID,IVoiceEventHandler* handler)
    {
        return m_pConnector->registerEventHandler(wEventID, handler);
    }

	virtual bool unregisterEventHandler(WORD wEventID,IVoiceEventHandler* handler)
    {
        return m_pConnector->unregisterEventHandler(wEventID, handler);
    }

	virtual void SendDataToServer(DWORD dwServerID,const char * pData,DWORD dwLen,bool bZeroBroadcast=false)
    {
        m_pConnector->SendDataToServer(dwServerID, pData, dwLen, bZeroBroadcast);
    }

	virtual void BroadcastDataToServer(DWORD * pServerArray,WORD wServerNum,const char * pData,DWORD dwLen)
    {
        m_pConnector->BroadcastDataToServer(pServerArray, wServerNum, pData, dwLen);
    }

	virtual void BroadcastDataToClient( BYTE nBroadcastType, void * pData, DWORD dwLen, DWORD dwPDBID1 = 0, DWORD dwPDBID2 = 0, DWORD dwWorldID1 = 0, DWORD dwWorldID2 = 0, DWORD dwValueID1 = 0, DWORD dwValueID2 = 0, bool bRoom = false )
    {
        m_pConnector->BroadcastDataToClient(nBroadcastType, pData, dwLen, dwPDBID1, dwPDBID2, dwWorldID1, dwWorldID2, dwValueID1, dwValueID2, bRoom);
    }

    virtual void HandleVoiceMessage(unsigned long actionId,SMsgVoiceHead &voiceHead, SGameMsgHead* head, LPSTR data, size_t len)
    {
    }

    virtual void onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len)
    {
        
    }

	virtual void	onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
    {
        switch(head->byKeyAction)
        {
        case MSG_VOICE_LOGIN:
            {
                if(len < sizeof(SMsgVoiceZoneLogin_ZS))
                {
                    ErrorLn(__FUNCTION__ << ", len=" << len);
                    return;
                }

                SMsgVoiceZoneLogin_ZS* pMsg = (SMsgVoiceZoneLogin_ZS*)data;
                obuf512 obufData;

                SGameMsgHead header;
                header.SrcEndPoint = MSG_ENDPOINT_SOCIAL;
                header.DestEndPoint= MSG_ENDPOINT_VOICE;
                header.byKeyModule  = MSG_MODULEID_VOICE;
                header.byKeyAction  = MSG_VOICE_LOGIN;
                obufData.push_back( &header, sizeof(SGameMsgHead) );

                obufData.push_back(pMsg, sizeof(SMsgVoiceZoneLogin_ZS));

                m_pConnector->SendData( obufData.data(), obufData.size() );	// 消息发送给语音服务器
            }
            break;
        }
    }

    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        switch(wEventID)
        {
        case EVENT_ROOM_ADD_ACTOR: // 更新战场频道信息到语音服
            {
                if(pszContext == NULL || nLen < sizeof(SEventRoomAddActor))
                {
                    ErrorLn(__FUNCTION__ << __LINE__);
                    return;
                }
                SEventRoomAddActor* pEvent = (SEventRoomAddActor*)pszContext;

                updateWarChannelInfo2VoiceServer(pEvent->nActorID, (pEvent->dwRoomID << 2) | (pEvent->nCamp + 1));
            }
            break;
        case EVENT_ROOM_DEL_ACTOR: // 更新战场频道信息到语音服
            {
                if(pszContext == NULL || nLen < sizeof(SEventRoomDelActor))
                {
                    ErrorLn(__FUNCTION__ << __LINE__);
                    return;
                }
                SEventRoomDelActor* pEvent = (SEventRoomDelActor*)pszContext;
                updateWarChannelInfo2VoiceServer(pEvent->nActorID, 0);
            }
            break;
        case EVENT_ROOM_ACTOR_CHANGE_CAMP: // 房间内换阵营
            {
                if(pszContext == NULL || nLen < sizeof(SEventRoomActorChangeCamp))
                {
                    ErrorLn(__FUNCTION__ << __LINE__);
                    return;
                }
                SEventRoomActorChangeCamp* pEvent = (SEventRoomActorChangeCamp*)pszContext;
                updateWarChannelInfo2VoiceServer(pEvent->nActorID, (pEvent->dwRoomID << 2) | (pEvent->nCamp + 1));
            }
            break;
        }
    }

	virtual void	onServerListUpdated(){}

	virtual void	onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData){}

	virtual void release()
	{
		Stop();
		delete this;
	}

private:
    void updateWarChannelInfo2VoiceServer(int nActorID, DWORD dwChannelID)
    {
        SVoiceUserInfo_Table userInfo;
        SVoiceChannel& channelWar = userInfo.context.channelList[VoiceChannel_War];
        ((SVoiceChannelID*)&channelWar.channleID)->dwWorldID = getThisGameWorldID();
        ((SVoiceChannelID*)&channelWar.channleID)->dwType = VoiceChannel_War;
        ((SVoiceChannelID*)&channelWar.channleID)->dwID = dwChannelID;
        channelWar.nUserType = VoiceUserType_Normal;

        obuf obufData;
        SGameMsgHead header;
        header.SrcEndPoint = MSG_ENDPOINT_SCENE;
        header.DestEndPoint= MSG_ENDPOINT_VOICE;
        header.byKeyModule  = MSG_MODULEID_VOICE;
        header.byKeyAction  = MSG_VOICE_USERINFO_UPDATE;

        obufData.push_back( &header, sizeof(SGameMsgHead) );

        obufData << nActorID;

        bool bMask[VOICE_FIELD_MAX];
        for(int i = 0; i < VOICE_FIELD_MAX; ++i) bMask[i] = false;
        bMask[VOICE_FIELD_CHANNEL_War] = true;
        userInfo.getFields(obufData, bMask);

        m_pConnector->SendData(obufData.data(), obufData.size());
    }
};