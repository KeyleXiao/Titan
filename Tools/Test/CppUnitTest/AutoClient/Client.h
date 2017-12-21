/*******************************************************************
** 文件名:	Client.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	1/26/2015
** 版  本:	1.0
** 描  述:	客户端类声明

********************************************************************/
#pragma once

#include "Scheme.h"

class CAutoClientDlg;

class CClient : public IConnectionEventHandler, public TimerHandler
{
    enum
    {
        TIMER_CHECK_ROOM_CREATED    = 0x100,
        TIMER_CHECK_PLAYERS_ENTERED,
        TIMER_CLOSE_WAR,
    };

public:
	CClient(const SchemeAccount& account, CAutoClientDlg* pDlg);
	~CClient();

	bool ConnectServer(string strIP, WORD wPort);

	void CreateAction();

	inline Vector3&		GetLoc()		{ return m_loc; }
	inline UID			GetUID()		{ return m_uid; }
    inline int          GetAccountID()  { return m_account.a_id; }
    inline T_MDRoomID   GetRoomID()     { return m_nRoomID; }

    void JoinRoom(T_MDRoomID nRoomID);

    void SetTimer(int nTimerID, DWORD dwInterval);
    void KillTimer(int nTimerID);

private:
	virtual void OnAccept(IConnection * pIncomingConn,IConnectionEventHandler** ppHandler);

	virtual void OnConnected(IConnection* conn);

	virtual void OnDisconnected(IConnection* conn, DWORD reason);

	virtual void OnRecv(IConnection* conn, LPVOID pData, DWORD dwDataLen);

	virtual void OnError(IConnection* conn, DWORD dwErrorCode);

	virtual void OnTimer(unsigned long dwTimerID);

    void Move(int* params);

    void UseSkill(int* params);

    bool CheckRoomCreated(T_MDRoomID& nRoomID);

    bool CheckPlayersEntered();

    void EnterMMO();

    void EnterHall();

    void CreateRoom();

    void StartWar();

    void CloseWar();

private:
    static int      s_nRoomNum;

private:
	SchemeAccount	m_account;
	IConnection*	m_pNetConn;
	CAutoClientDlg*	m_pMainDlg;

    bool            m_bRoomCreator;

	Vector3			m_loc;
	UID				m_uid;
    T_MDRoomID      m_nRoomID;
};
