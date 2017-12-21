#pragma once
#include "DGlobalMessage.h"
#include "IServiceMgr.h"
#include "IFramework.h"
#include "IServerGlobal.h"
#include "IVoiceConnector.h"
#include "IVoiceConnectorService.h"
#include "INetMessageHandler.h"
#include "ITimerService.h"
#include "EntityHelper.h"
#include "KinHelper.h"
#include "ClanHelper.h"
#include "ClanDef.h"
#include "MatchSceneHelper.h"
#include "net/Reactor.h"
#include "net/net.h"
#include <WinSock.h>

using namespace Voice;

class VoiceConnectorService : public IVoiceConnectorService,
    public IVoiceMessageHandler,public EventHandler,public IMessageHandler,public IEventExecuteSink
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
	}

	bool Start( const char * ip,int port )
	{
		// 挂接网络事件
		Reactor * reactor = gServerGlobal->getFramework()->get_reactor();
		if ( reactor==0 )
			return false;

		HANDLE hNetEvent = GetNetworkEventHandle(NetworkUserType_User5);
		reactor->AddEvent(hNetEvent);
        reactor->RegisterEventHandler( hNetEvent,this, __FUNCTION__);

        m_strIp = ip;
        m_nPort = port;

		m_pConnector = CreateVoiceConnectorProc((TimerAxis *)(gServerGlobal->getTimerService()->getAxis()));
		if ( m_pConnector==0 )
		{
			ErrorLn(_GT("创建语音服务器连接器失败!"));
			return false;
		}

        m_pConnector->registerMessageHandler(MSG_MODULEID_COMMON, this);
        gServerGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_VOICE, this);
        m_pConnector->Create(getThisGameWorldID(), MSG_ENDPOINT_SCENE, gServerGlobal->getServerID(), _GT("场景服务器"), NULL);

        rkt::IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
        if (pEventEngine)
        {
            pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_LOGIN, SOURCE_TYPE_PERSON, 0, __FUNCTION__);
            pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_SYNC_PROPERTY, SOURCE_TYPE_PERSON, 0, __FUNCTION__);
            pEventEngine->Subscibe(this, EVENT_KIN_SHAIKHID_CHANGED, SOURCE_TYPE_KIN, 0, __FUNCTION__);
            pEventEngine->Subscibe(this, EVENT_CLAN_SHAIKHID_CHANGED, SOURCE_TYPE_CLAN, 0, __FUNCTION__);
        }


		return true;
	}

	void Stop()
	{
		// 挂接网络事件
		Reactor * reactor = gServerGlobal->getFramework()->get_reactor();
		if ( reactor )
		{
			HANDLE hNetEvent = GetNetworkEventHandle(NetworkUserType_User5);
			reactor->RemoveEvent( hNetEvent );
		}

		if ( m_pConnector )
		{
            m_pConnector->unregisterMessageHandler(MSG_MODULEID_COMMON);
            gServerGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_VOICE);
			ErrorLn(_GT("正在断开与语音服务器连接!") );
			m_pConnector->Release();
			m_pConnector = 0;
		}

        rkt::IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
        if (pEventEngine)
        {
            pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_LOGIN, SOURCE_TYPE_PERSON, 0);
            pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_SYNC_PROPERTY, SOURCE_TYPE_PERSON, 0);
            pEventEngine->UnSubscibe(this, EVENT_KIN_SHAIKHID_CHANGED, SOURCE_TYPE_KIN, 0);
            pEventEngine->UnSubscibe(this, EVENT_CLAN_SHAIKHID_CHANGED, SOURCE_TYPE_CLAN, 0);
        }
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
        switch(head->byKeyAction)
        {
        case MSG_VOICE_DATA_SUBMSG:
            {
                PDBID pdbid = gServerGlobal->getActorMgr()->ClientID2PDBID(clientID);
                if(pdbid == INVALID_PDBID)
                {
                    WarningLn(__FUNCTION__ << ", MSG_VOICE_DATA_SUBMSG , INVALID_PDBID");
                    return;
                }

                onClientVoiceSubMsg(pdbid, data, len);
            }
            break;
        }
    }

    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        switch(wEventID)
        {
        case EVENT_GAME_ACTOR_LOGIN:
            {
                if(pszContext == NULL || nLen < sizeof(event_actor_login))
                {
                    ErrorLn(__FUNCTION__ << ", EVENT_GAME_ACTOR_LOGIN, nLen=" << nLen);
                    return;
                }
    
                // 上线不用更新频道列表 因为语音登录消息会带过去
                event_actor_login* pEvent = (event_actor_login*)pszContext;
                if(IsComputerPDBID(pEvent->dwDBID) || pEvent->eMode == elogin_online)
                {
                    return;
                }

                // 现在除了上线  切场景战场频道肯定会变
                updateUserInfo2VoiceServer(pEvent->dwDBID, VOICE_FIELD_CHANNEL_War);
            }
            break;
        case EVENT_GAME_ACTOR_SYNC_PROPERTY:
            {
                if(pszContext == NULL || nLen < sizeof(event_entity_sync_share_property))
                {
                    ErrorLn(__FUNCTION__ << ", EVENT_GAME_ACTOR_SYNC_PROPERTY, nLen=" << nLen);
                    return;
                }

                event_entity_sync_share_property* pEvent = (event_entity_sync_share_property*)pszContext;

                switch(pEvent->nPropertyID)
                {
                case PROPERTY_TEAMID:
                    {
                        updateUserInfo2VoiceServer(pEvent->dwDBID, VOICE_FIELD_CHANNEL_Team);
                    }
                    break;
                case PROPERTY_CLAN:
                    {
                        updateUserInfo2VoiceServer(pEvent->dwDBID, VOICE_FIELD_CHANNEL_Clan);
                    }
                    break;
                case PROPERTY_KIN:
                    {
                        updateUserInfo2VoiceServer(pEvent->dwDBID, VOICE_FIELD_CHANNEL_Kin);
                    }
                    break;
                }
            }
            break;
        case EVENT_KIN_SHAIKHID_CHANGED:
            {
                if(pszContext == NULL || nLen < sizeof(event_kin_shaikhid_changed))
                {
                    ErrorLn(__FUNCTION__ << ", EVENT_KIN_SHAIKHID_CHANGED, nLen=" << nLen);
                    return;
                }

                event_kin_shaikhid_changed* pEvent = (event_kin_shaikhid_changed*)pszContext;

                ActorHelper hOld(pEvent->nOldID, ACTOR_DBID);
                if(hOld.m_ptr)
                {
                    updateUserInfo2VoiceServer(pEvent->nOldID, VOICE_FIELD_CHANNEL_Kin);
                }

                ActorHelper hNew(pEvent->nNewID, ACTOR_DBID);
                if(hNew.m_ptr)
                {
                    updateUserInfo2VoiceServer(pEvent->nNewID, VOICE_FIELD_CHANNEL_Kin);
                }
            }
            break;
        case EVENT_CLAN_SHAIKHID_CHANGED:
            {
                if(pszContext == NULL || nLen < sizeof(event_clan_shaikhid_changed))
                {
                    ErrorLn(__FUNCTION__ << ", EVENT_CLAN_SHAIKHID_CHANGED, nLen=" << nLen);
                    return;
                }

                event_clan_shaikhid_changed* pEvent = (event_clan_shaikhid_changed*)pszContext;

                ActorHelper hOld(pEvent->nOldID, ACTOR_DBID);
                if(hOld.m_ptr)
                {
                    updateUserInfo2VoiceServer(pEvent->nOldID, VOICE_FIELD_CHANNEL_Clan);
                }

                ActorHelper hNew(pEvent->nNewID, ACTOR_DBID);
                if(hNew.m_ptr)
                {
                    updateUserInfo2VoiceServer(pEvent->nNewID, VOICE_FIELD_CHANNEL_Clan);
                }
            }
            break;
        }
    }

    void onClientVoiceSubMsg(PDBID dbid, void* data, size_t len)
    {
        if(len < sizeof(SMsgVoiceDataSubMsg))
        {
            ErrorLn(__FUNCTION__ << ", message format error");
            return;
        }
        SMsgVoiceDataSubMsg* pSubMsg = (SMsgVoiceDataSubMsg*)data;
        switch(pSubMsg->dwMsgCode)
        {
        case VoiceSubMsg_RequestLogin:  // 语音登录
            {
                DWORD dwVoiceClientID = pSubMsg->dwUserData1;
                DWORD dwKeyLen = pSubMsg->dwUserData2;
                void* pKeyData = pSubMsg + 1;

                ActorHelper helper(dbid, ACTOR_DBID);
                if(!helper.m_ptr)
                {
                    WarningLn(__FUNCTION__ << ", dbid=" << dbid);
                    return;
                }

                SMsgVoiceZoneLogin_ZS msg;

                if(dwKeyLen > sizeof(msg.key))
                {
                    WarningLn(__FUNCTION__ << ", key len dismatch");
                    return;
                }
                memcpy(msg.key, pKeyData, dwKeyLen);
                msg.userInfo.dwFromWorldID = getThisGameWorldID(); // todo.
                msg.userInfo.dwClientID = dwVoiceClientID;
                msg.userInfo.dwPDBID = dbid;
                msg.userInfo.dwUDBID = helper.m_ptr->getUserID();
                msg.userInfo.nSex = (helper.m_ptr->getPlayerInfo().bSex ? 1 : 0);
                string strName = helper.m_ptr->getName();
                strncpy(msg.userInfo.szName, strName.c_str(), sizeof(msg.userInfo.szName));

                // 初始频道信息
                makeVoiceChannel(dbid, msg.userInfo, VOICE_FIELD_CHANNEL_Gm);
                makeVoiceChannel(dbid, msg.userInfo, VOICE_FIELD_CHANNEL_Clan);
                makeVoiceChannel(dbid, msg.userInfo, VOICE_FIELD_CHANNEL_Kin);
                makeVoiceChannel(dbid, msg.userInfo, VOICE_FIELD_CHANNEL_Team);
                makeVoiceChannel(dbid, msg.userInfo, VOICE_FIELD_CHANNEL_War);
                makeVoiceChannel(dbid, msg.userInfo, VOICE_FIELD_CHANNEL_Personal);

                // 战队名
                int nKinID = helper.m_ptr->getProperty_Integer(PROPERTY_KIN); 
                KinHelper kinHelper;
                if(kinHelper.m_ptr)
                {
                    SKinInfo kinInfo = kinHelper.m_ptr->getKinInfo(nKinID);
                    int nKinNameLen = min(sizeof(msg.userInfo.szKinName), sizeof(kinInfo.szName));
                    memcpy(msg.userInfo.szKinName, kinInfo.szName, nKinNameLen);
                }
                
                // 段位 todo.

                // 发到社会服
                gServerGlobal->getCenterConnectorService()->sendToSocial(MSG_MODULEID_VOICE, MSG_VOICE_LOGIN, (const char*)&msg, sizeof(msg));
            }
            break;
        }
    }

    // 更新语音用户信息
    bool updateUserInfo2VoiceServer(PDBID dbid, SVoiceUserInfoFieldID eField)
    {
        if(m_pConnector == NULL)
        {
            return false;
        }

        if(eField < 0 || eField >= VOICE_FIELD_MAX)
        {
            return false;
        }

        SVoiceUserInfo_Table userInfo;
        if(eField > VOICE_FIELD_CHANNEL_None && eField <= VOICE_FIELD_CHANNEL_Personal)
        {
            makeVoiceChannel(dbid, userInfo.context, eField);
        }
        // ... 其他一些字段 todo

        obuf obufData;
        SGameMsgHead header;
        header.SrcEndPoint = MSG_ENDPOINT_SCENE;
        header.DestEndPoint= MSG_ENDPOINT_VOICE;
        header.byKeyModule  = MSG_MODULEID_VOICE;
        header.byKeyAction  = MSG_VOICE_USERINFO_UPDATE;

        obufData.push_back( &header, sizeof(SGameMsgHead) );
        
        obufData << dbid;
        
        bool bMask[VOICE_FIELD_MAX];
        for(int i = 0; i < VOICE_FIELD_MAX; ++i) bMask[i] = false;
        bMask[eField] = true;
        userInfo.getFields(obufData, bMask);

        m_pConnector->SendData(obufData.data(), obufData.size());
        return true;
    }

    // 生成玩家的频道信息
    bool makeVoiceChannel(PDBID dbid, SVoiceUserInfo& userInfo, SVoiceUserInfoFieldID eChannel)
    {
        ActorHelper helper(dbid, ACTOR_DBID);
        if(!helper.m_ptr)
        {
            WarningLn(__FUNCTION__ << ", dbid=" << dbid);
            return false;
        }

        SPlayerInfo playerInfo = helper.m_ptr->getPlayerInfo();
        DWORD dwFromWorld = playerInfo.dwWorldID;

        switch(eChannel)
        {
        case VOICE_FIELD_CHANNEL_Gm:
            {
                // 用户官方频道信息
                SVoiceChannel& channelGM = userInfo.channelList[VoiceChannel_GM];
                ((SVoiceChannelID*)&channelGM.channleID)->dwType = VoiceChannel_GM;
                ((SVoiceChannelID*)&channelGM.channleID)->dwID = VoiceChannel_GM; 
                channelGM.nUserType = VoiceUserType_Normal;
            }
            break;
        case VOICE_FIELD_CHANNEL_Clan:
            {
                DWORD dwClanID = helper.m_ptr->getProperty_Integer(PROPERTY_CLAN);
                
                // 是否是盟主
                bool bClanOwner = false;
                ClanHelper clanHelper;
                if(clanHelper.m_ptr)
                {
                    bClanOwner = (clanHelper.m_ptr->getNumProp(dwClanID, emClanProp_ShaikhID) == dbid);
                }

                // 用户联盟频道信息
                SVoiceChannel& channelClan = userInfo.channelList[VoiceChannel_Clan];
                ((SVoiceChannelID*)&channelClan.channleID)->dwWorldID = dwFromWorld;
                ((SVoiceChannelID*)&channelClan.channleID)->dwType = VoiceChannel_Clan;
                ((SVoiceChannelID*)&channelClan.channleID)->dwID = dwClanID;

                if(bClanOwner)
                {
                    channelClan.nUserType = VoiceUserType_OW;
                }
                else
                {
                    channelClan.nUserType = VoiceUserType_Normal;
                }
            }
            break;
        case VOICE_FIELD_CHANNEL_Kin:
            {
                DWORD dwKinID = helper.m_ptr->getProperty_Integer(PROPERTY_KIN);

                // 是否是战队长
                bool bKinOwner = false;
                KinHelper kinHelper;
                if(kinHelper.m_ptr)
                {
                    SKinInfo kinInfo = kinHelper.m_ptr->getKinInfo(dwKinID);
                    bKinOwner = (kinInfo.nKinProp_ShaikhID == dbid);
                }

                // 用户战队频道信息
                SVoiceChannel& channelKin = userInfo.channelList[VoiceChannel_Kin];
                ((SVoiceChannelID*)&channelKin.channleID)->dwWorldID = dwFromWorld;
                ((SVoiceChannelID*)&channelKin.channleID)->dwType = VoiceChannel_Kin;
                ((SVoiceChannelID*)&channelKin.channleID)->dwID = dwKinID; 
                if(bKinOwner)
                {
                    channelKin.nUserType = VoiceUserType_OW;
                }
                else
                {
                    channelKin.nUserType = VoiceUserType_Normal;
                }
            }
            break;
        case VOICE_FIELD_CHANNEL_Team:
            {
                // 用户队伍频道信息
                SVoiceChannel& channelTeam = userInfo.channelList[VoiceChannel_Team];
                ((SVoiceChannelID*)&channelTeam.channleID)->dwWorldID = dwFromWorld;
                ((SVoiceChannelID*)&channelTeam.channleID)->dwType = VoiceChannel_Team;
                ((SVoiceChannelID*)&channelTeam.channleID)->dwID = helper.m_ptr->getProperty_Integer(PROPERTY_TEAMID);
                channelTeam.nUserType = VoiceUserType_Normal;
            }
            break;
        case VOICE_FIELD_CHANNEL_War:
            {
                // 用户战场频道信息
                SVoiceChannel& channelWar = userInfo.channelList[VoiceChannel_War];
                ((SVoiceChannelID*)&channelWar.channleID)->dwWorldID = getThisGameWorldID();
                ((SVoiceChannelID*)&channelWar.channleID)->dwType = VoiceChannel_War;
                channelWar.nUserType = VoiceUserType_Normal;
                MatchSceneHelper matchHelper;
                int nRoomID = matchHelper.m_ptr->getRoomID(dbid); 
                DWORD dwWarChannelID = 0;
                if(nRoomID != 0)
                {
                    int nCamp = helper.m_ptr->getProperty_Integer(PROPERTY_CAMP);
                    dwWarChannelID = (nRoomID << 2) | nCamp;
                }
                ((SVoiceChannelID*)&channelWar.channleID)->dwID = dwWarChannelID; 
            }
            break;
        case VOICE_FIELD_CHANNEL_Personal:
            {
                // 用户个人频道信息
                SVoiceChannel& channelSelf = userInfo.channelList[VoiceChannel_Personal];
                ((SVoiceChannelID*)&channelSelf.channleID)->dwWorldID = dwFromWorld;
                ((SVoiceChannelID*)&channelSelf.channleID)->dwType = VoiceChannel_Personal;
                ((SVoiceChannelID*)&channelSelf.channleID)->dwID = dbid;
                channelSelf.nUserType = VoiceUserType_OW;
            }
            break;
        }
       
        return true;
    }

	virtual void release()
	{
		delete this;
    }
};