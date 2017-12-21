/*******************************************************************
** 文件名:	Client.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	1/26/2015
** 版  本:	1.0
** 描  述:	客户端类实现

********************************************************************/
#include "stdafx.h"
#include "Client.h"
#include "AutoClientDlg.h"
#include "MatchDef.h"
//#include "SpellMessageDef.h"

int CClient::s_nRoomNum = 0;

CClient::CClient(const SchemeAccount& account, CAutoClientDlg* pDlg)
	: m_account(account)
	, m_pMainDlg(pDlg)
    , m_uid(0)
    , m_nRoomID(0)
{
	m_pNetConn = CreateTCPConnection(0, this, LUA_PACK);

	// 如果是MMO模式的话则随机生成坐标
    if (pDlg->GetGameMode() == MODE_MMO)
    {
	    m_loc.x = 45+rand()%10;
	    m_loc.y = 10;
	    m_loc.z = 45+rand()%10;
    }

    m_bRoomCreator = (m_account.a_id%10 == 1);
}

CClient::~CClient()
{
    for (CCSVReader<int, SchemeAction>::iterator itr = g_actionReader.begin();
        itr != g_actionReader.end();
        ++itr)
    {
        SchemeAction& action = itr->second;
        KillTimer(action.a_id);
    }
    KillTimer(TIMER_CHECK_ROOM_CREATED);

	m_pNetConn->Release();
}

bool CClient::ConnectServer(string strIP, WORD wPort)
{
	return m_pNetConn->Connect(strIP.c_str(), wPort);
}

void CClient::CreateAction()
{
	for (CCSVReader<int, SchemeAction>::iterator itr = g_actionReader.begin();
		 itr != g_actionReader.end();
		 ++itr)
	{
		SchemeAction& action = itr->second;
		SetTimer(action.a_id, action.a_secs*1000);
	}
}

void CClient::JoinRoom(T_MDRoomID nRoomID)
{
    m_nRoomID = nRoomID;

    obuf256 ob;

    SNetMsgHead head;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byDestEndPoint = MSG_ENDPOINT_SCENE;
    head.byKeyModule    = MSG_MODULEID_MATCHSERVER;
    head.byKeyAction    = C2MS_MSG_ENTER_ROOM;

    SMsg_C2MS_EnterRoom msg;
    msg.m_RoomID = nRoomID;

    ob << head << msg;

    m_pNetConn->SendData(ob.data(), ob.size());
}

void CClient::SetTimer(int nTimerID, DWORD dwInterval)
{
    m_pMainDlg->m_timerAxis.SetTimer(nTimerID, dwInterval, this);
}

void CClient::KillTimer(int nTimerID)
{
    m_pMainDlg->m_timerAxis.KillTimer(nTimerID, this);
}

void CClient::OnAccept(IConnection* pIncomingConn, IConnectionEventHandler** ppHandler)
{
	assert(0);
}

void CClient::OnConnected(IConnection* conn)
{
	TraceLn("Client ID = " << m_account.a_id << " connect success");

	// 登陆账号
	obuf256 ob;
	CS_LoginMsg data;
	memset(&data,0,sizeof(CS_LoginMsg));

	lstrcpyn(data.szUserName, m_account.user_name.c_str(),sizeof(data.szUserName));
	lstrcpyn(data.szPassword, m_account.a_pwd.c_str(),sizeof(data.szPassword));

	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_SCENE;
	head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
	head.byKeyModule    = MSG_MODULEID_LOGIN;
	head.byKeyAction    = CS_MSG_USER_LOGIN;

	ob << head << data;

	m_pNetConn->SendData(ob.data(), ob.size());
}

void CClient::OnDisconnected(IConnection* conn, DWORD reason)
{
	ErrorLn("Client ID = " << m_account.a_id << " connect failed!");
	m_pMainDlg->ReleaseClient(this);
}

void CClient::OnRecv(IConnection* conn, LPVOID pData, DWORD dwDataLen)
{
    assert(m_pNetConn == conn);

    ibuffer in(pData, dwDataLen);

	SNetMsgHead head;
    in >> head;

	switch (MAKEWORD(head.byKeyModule, head.byKeyAction))
	{
	case MAKEWORD(MSG_MODULEID_LOGIN, SC_MSG_USER_LOGIN):
		{
            SC_LoginMsg msg;
            in >> msg;
			if (msg.bLoginOK)
			{
				TraceLn("Client ID=" << m_account.a_id << " Login Success");

                if (m_pMainDlg->GetGameMode() == MODE_MMO)
                {
                    EnterMMO();
                }
                else
                {
                    EnterHall();
                }
			}
			else
			{
				ErrorLn("Client ID = " << m_account.a_id << " Login failed!");
			}
		}
		break;

	case MAKEWORD(MSG_MODULEID_LOGIN, SC_MSG_USER_ENTER_GAME):
		{
            SC_EnterGameMsg msg;
            in >> msg;
			if (msg.nErrorCode == 0)
			{
				TraceLn("Client ID=" << m_account.a_id << " Enter MMO Game Success");
				m_pMainDlg->ClientLogin(m_account.a_id);
			}
			else
			{
				ErrorLn("Client ID=" << m_account.a_id << "Enter MMO Game failed!");
			}
		}
		break;

    case MAKEWORD(MSG_MODULEID_MATCHSERVER, MS2C_MSG_ENTER_HALL):
        {
            SMsg_MS2C_EnterHall msg;
            in >> msg;

            if (!msg.m_bOK)
            {
                ErrorLn("Client ID=" << m_account.a_id << "enter hall failed");
            }
            else
            {
                TraceLn("Client ID=" << m_account.a_id << "enter hall success");
                // 根据ID来决定谁创建房间
                if (m_bRoomCreator)
                {
                    CreateRoom();
                }
                else
                {
                    // 定时轮询房间是否已经创建好了
                    SetTimer(TIMER_CHECK_ROOM_CREATED, 200);
                }
            }
        }
        break;

    case MAKEWORD(MSG_MODULEID_MATCHSERVER, MS2C_MSG_CREATE_ROOM):
        {
            SMsg_MS2C_CreateRoom msg;
            in >> msg;
            if (!msg.m_Result)
            {
                ErrorLn("Client ID=" << m_account.a_id << "create room failed");
                break;
            }
            in >> m_nRoomID;
            TraceLn("Client ID=" << m_account.a_id << "create room success, roomID = " << (int)m_nRoomID);
            // 开启定时器轮询其它账号是否已成功加入此房间
            SetTimer(TIMER_CHECK_PLAYERS_ENTERED, 200);
        }
        break;

    case MAKEWORD(MSG_MODULEID_MATCHSERVER, MS2C_MSG_ENTER_ROOM):
        {
            SMsg_MS2C_EnterRoom msg;
            in >> msg;
            if (!msg.m_Result)
            {
                ErrorLn("Client ID=" << m_account.a_id << " enter room ID = " << (int)m_nRoomID << " failed!");
                break;
            }
            SMsg_MS2C_EnterRoom_Ex room;
            in >> room;
            m_nRoomID = room.m_RoomID;
            TraceLn("Client ID=" << m_account.a_id << "enter room success, roomID = " << (int)m_nRoomID);
        }
        break;

    case MAKEWORD(MSG_MODULEID_MATCHSCENE, MSS2C_MSG_LOAD_SCENE):
        {
            T_MDSceneID nSceneID;
            in >> nSceneID;
            TraceLn("Client ID=" << m_account.a_id << "load scene = " << (int)nSceneID);
        }
        break;

    case MAKEWORD(MSG_MODULEID_MATCHSCENE, MSS2C_MSG_ENTER_SCENE):
        {
            T_MDSceneID nSceneID;
            in >> nSceneID;
            TraceLn("Client ID=" << m_account.a_id << "enter scene");
            if (m_bRoomCreator)
            {
                SetTimer(TIMER_CLOSE_WAR, 1200*1000 );
            }
        }
        break;

	case MAKEWORD(MSG_MODULEID_ENTITY, SC_MSG_ENTITY_CREATE):
		{
            SMsgEntityHead entityHead;

			unsigned char tp = 0;
			unsigned char flag = TO_CLIENT_OTHER;

			in >> entityHead >> tp >> flag;
			if (flag & TO_CLIENT_SELF)
			{
				m_uid = entityHead.uidMaster;
				int params[3] = { 0 };
				params[0] = m_account.user_loc[0];
				params[1] = m_account.user_loc[1];
				params[2] = m_account.user_loc[2];
				Move(params);
				CreateAction();
			}
		}
		break;

	case MAKEWORD(MSG_MODULEID_ENTITY, SC_MSG_ENTITY_MOVE):
	case MAKEWORD(MSG_MODULEID_ENTITY, SC_MSG_ENTITY_UPDATEPROP):
		break;

	default: 
		{

		}
		break;
	}
}

void CClient::OnError(IConnection* conn, DWORD dwErrorCode)
{
	ErrorLn("Client ID=" << m_account.a_id << "connect error code=" << dwErrorCode);
	m_pMainDlg->ReleaseClient(this);
}

void CClient::OnTimer(unsigned long dwTimerID)
{
	int action_id = (int)dwTimerID;
    int* params = NULL;

	CCSVReader<int, SchemeAction>::iterator itr = g_actionReader.find(action_id);
	if (itr != g_actionReader.end())
    {
        params = itr->second.params;
    }

	switch (dwTimerID)
	{
	case ACTION_MOVE:
		{
			Move(params);
		}
		break;

	case ACTION_USESKILL:
		{
			UseSkill(params);
		}
		break;

    case TIMER_CHECK_ROOM_CREATED:
        {
            T_MDRoomID nRoomID = 0;
            if (CheckRoomCreated(nRoomID))
            {
                KillTimer(TIMER_CHECK_ROOM_CREATED);
                JoinRoom(nRoomID);
            }
        }
        break;

    case TIMER_CHECK_PLAYERS_ENTERED:
        {
            if (CheckPlayersEntered())
            {
                KillTimer(TIMER_CHECK_PLAYERS_ENTERED);
                StartWar();
            }
        }
        break;

    case TIMER_CLOSE_WAR:
        {
            KillTimer(TIMER_CLOSE_WAR);
            CloseWar();
        }

	default:
		break;
	}
}

void CClient::Move(int* params)
{
    msg_creature_rigidbody_sync data;
    data.nActorID = 0;

    m_loc.x = params[0];
    m_loc.y = params[1];
    m_loc.z = params[2];

    data.move.vPosition = m_loc;
    data.nTick = m_pMainDlg->m_TimeSyncer.GetTick();

    SNetMsgHead head;
    head.bySrcEndPoint	= MSG_ENDPOINT_CLIENT;
    head.byDestEndPoint	= MSG_ENDPOINT_SCENE;
    head.byKeyModule	= MSG_MODULEID_ENTITY;
    head.byKeyAction	= CS_MSG_CREATURE_SYNC;

    SMsgEntityHead entityHead;
    entityHead.uidMaster = m_uid;
    entityHead.byPartID = PART_MOVE;

    obuf256 ob;
    ob << head << entityHead << data;
    m_pNetConn->SendData(ob.data(), ob.size());
}

void CClient::UseSkill(int* params)
{
    CClient* pTargetClient = m_pMainDlg->GetAroundClient(this, 4.0f);
    //if (pTargetClient == NULL)
    //	return;

    SSpellContextImp_CS sendData;
    sendData.uidOperator = m_uid;
    sendData.uidTarget = this->GetUID();
    sendData.ptTargetTile = this->GetLoc();
    sendData.nID = params[0];

    SNetMsgHead head;
    head.bySrcEndPoint	= MSG_ENDPOINT_CLIENT;
    head.byDestEndPoint	= MSG_ENDPOINT_SCENE;
    head.byKeyModule	= MSG_MODULEID_ENTITY;
    head.byKeyAction	= SPELL_MSG_PREPARE;

    SMsgEntityHead entityHead;
    entityHead.uidMaster = m_uid;
    entityHead.byPartID = PART_SPELL;

    obuf256 ob;
    ob << head << entityHead << sendData;
    m_pNetConn->SendData(ob.data(), ob.size());

    ErrorLn("Account "<< m_account.a_id << " use skill!");
}

bool CClient::CheckRoomCreated(T_MDRoomID& nRoomID)
{
    int nCreatorAccount = (m_account.a_id-1)/10 + 1;
    CClient* pCreatorClient = m_pMainDlg->GetClient(nCreatorAccount);
    assert(pCreatorClient != NULL);

    nRoomID = pCreatorClient->GetRoomID();
    return (nRoomID != 0);
}

bool CClient::CheckPlayersEntered()
{
    for (int id = m_account.a_id+1; id < m_account.a_id+10; ++id)
    {
        CClient* pClient = m_pMainDlg->GetClient(id);
        if (pClient != NULL)
        {
            if (pClient->GetRoomID() != GetRoomID())
            {
                return false;
            }
        }
    }

    return true;
}

void CClient::EnterMMO()
{
    obuf256 ob;
    CS_EnterGameMsg msg;

    strcpy_s(msg.szUserName, sizeof(msg.szUserName), m_account.user_name.c_str());
    msg.nPrebHero = 1;

    SNetMsgHead head;
    head.byDestEndPoint = MSG_ENDPOINT_SCENE;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byKeyModule    = MSG_MODULEID_LOGIN;
    head.byKeyAction    = CS_MSG_USER_ENTER_GAME;

    ob << head << msg;

    m_pNetConn->SendData(ob.data(), ob.size());
}

void CClient::EnterHall()
{
    obuf256 ob;

    SNetMsgHead head;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byDestEndPoint = MSG_ENDPOINT_SCENE;
    head.byKeyModule    = MSG_MODULEID_MATCHSERVER;
    head.byKeyAction    = C2MS_MSG_ENTER_HALL;

    SMsg_C2MS_EnterHall msg;
    msg.m_HeroID = 1;

    ob << head << msg;

    m_pNetConn->SendData(ob.data(), ob.size());
}

void CClient::CreateRoom()
{
    obuf256 ob;

    SNetMsgHead head;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byDestEndPoint = MSG_ENDPOINT_SCENE;
    head.byKeyModule    = MSG_MODULEID_MATCHSERVER;
    head.byKeyAction    = C2MS_MSG_CREATE_ROOM;

    SMsg_C2MS_CreateRoom msg;
    msg.m_MapID		    = 1;
    msg.m_ModeID	    = EGM_Moba;
    sprintf_s(msg.m_RoomName, sizeof(msg.m_RoomName), "Test__Room %d", ++s_nRoomNum);

    ob << head << msg;

    m_pNetConn->SendData(ob.data(), ob.size());
}

void CClient::StartWar()
{
    obuf256 ob;

    SNetMsgHead head;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byDestEndPoint = MSG_ENDPOINT_SCENE;
    head.byKeyModule    = MSG_MODULEID_MATCHSERVER;
    head.byKeyAction    = C2MS_MSG_START_GAME;

    ob << head;

    m_pNetConn->SendData(ob.data(), ob.size());
};

void CClient::CloseWar()
{
    obuf256 ob;

    SNetMsgHead head;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byDestEndPoint = MSG_ENDPOINT_SCENE;
    head.byKeyModule    = MSG_MODULEID_ENTITY;
    head.byKeyAction    = CS_MSG_ENTITY_TEST;

    SMsgEntityHead ehead;
    ehead.uidMaster = m_uid;
    ehead.byPartID = PART_DRAGONBALL;

    msg_entity_smart_test msg;

    string function("SupperStone_EndtheWar()");
    msg.nStrLen = function.size()+1;

    ob << head << ehead << msg;
    ob.push_back(function.c_str(), msg.nStrLen);

    m_pNetConn->SendData(ob.data(), ob.size());
}