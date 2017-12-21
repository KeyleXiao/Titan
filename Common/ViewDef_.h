/*******************************************************************
** �ļ���:	ViewDef_.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/11/30
** ��  ��:	1.0
** ��  ��:	��սϵͳͨ�ú궨��
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "buffer.h"
#include "Common.h"
#include "NetMessageDef.h"

#pragma pack(1)

#define MAC_ADDRESS_SIZE			32		// ���������ַ����
#define VIEW_SECRET_KEY_SIZE		64		// ��½��֤�볤��
#define VIEW_SECRET_KEY_TIME_OUT	1*60	// ��½��֤�� ��Чʱ�䣨��λ���룩

#define MAX_GATEWAY_COUNT		USHRT_MAX	// �����֧�ֵ�Gate����
#define MAX_VIEW_SERVER_COUNT	USHRT_MAX	// �����֧�ֵ�View����
#define MAX_GAME_SERVER_COUNT	USHRT_MAX	// �����֧�ֵ���Ϸ������

#define VIEW_KEEPALIVE_INTERVAL			2000			// ����KeepAlive̽����ʱ����
#define VIEW_KEEPALIVE_MAXTIMEOUT		120000			// �������Ӧʱ��
#define VIEW_LISTENER_KEEPALIVE_TIMEOUT	300*1000		// �����˿ڳ�ʱʱ��
#define	VIEW_RECONNECT_INTERVAL			(10*1000)		// ��������10��


// ���val�������false���򷵻�false
#define D_IF_FALSE_RETURN(val)	\
if (!val)						\
{								\
	return false;				\
}


typedef	DWORD	ReplayID;	// ս��ID
typedef	WORD	ViewID;		// ViewServerID
typedef	WORD	GateID;		// ����ID
typedef DWORD	PlayerID;	// ������ʶ��ս�ͻ��˵�ΨһID��Ŀǰ�������ս�ķ��������������ڿ��������DBID��

#pragma pack()