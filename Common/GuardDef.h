/*******************************************************************
** �ļ���:	GuardDef.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��绪
** ��  ��:	2015-08-14
** ��  ��:	1.0
** ��  ��:	���ݼ໤ͨ�ýṹ����

********************************************************************/

#pragma once

// ���DEBUG��Ϣ
#define GUARD_PRINT_DEBUG_INF

#define SERVER_NAME_MAXSIZE				32			// ������󳤶�,32
#define GUARD_KEEPALIVE_MAXTIMEOUT		120000		// �������Ӧʱ��
#define GUARD_BROADCASTALL_NUM			0xFFFF		// WORD �������,0xFFFF


/////// ����ö�ٶ��� ///////////////////////////////////////////////////////////////////
// �Ƿ�֧��Int����
//#define GUARD_SUPPORT_IMAGE_INT_UNIT

// ����������������
enum IMAGE_DATA_NAMESPACE
{
	IMAGE_DATA_NAMESPACE_UNKNOWN = 0,		// �������ݣ�δ֪
    IMAGE_DATA_NAMESPACE_ACTOR_WAR,			// ��ɫս������
	//IMAGE_DATA_NAMESPACE_ACCOUNT,			// �ʺ�����
	//IMAGE_DATA_NAMESPACE_ACTOR,				// ��ɫ����
	IMAGE_DATA_NAMESPACE_MAX,				// �����
};


// ���������߳�ִ�в������붨��
enum EMGuardServerControlCode
{
	GS_ControlCode_None = 0,		// �޲���
	GS_ControlCode_StartService,	// ��������
	GS_ControlCode_StopService,		// ֹͣ����
	GS_ControlCode_ShutDown,		// �رճ���,ֹͣ�����Ҫ�رս���
	// ���
	GS_ControlCode_Max
};

/***************************************************************/
///////////////////// ���ݼ໤������ģ�� ����Ϣ��////////////////
// �ϲ����Ϣ��ض��� MSG_MODULEID_GUARD
/***************************************************************/

///////////////////////////////////////////////////////////////////
// ��Ϣ��ö�ٶ���: ENUM + ��Ϣ����
enum EMMsgGuard
{
	MSG_GUARD_INVALID = 0,
	MSG_GUARD_HANDSHAKE,						// ������Ϣ
	MSG_GUARD_HANDSHAKE_RESPONSE,				// ��Ӧ������Ϣ
	MSG_GUARD_KEEPALIVE,						// �������
	MSG_GUARD_ANSWER_KEEPALIVE,					// ��Ӧ�������
	MSG_GUARD_SERVERCHECKREQ,					// ���ͷ��������ܼ������
	MSG_GUARD_SERVERCHECKRES,					// ��Ӧ���������ܼ������

	MSG_GUARD_READ_DATA,						// �ӷ������ж���������
	MSG_GUARD_READ_DATA_RESULT,					// �ӷ������ж��������ݽ��
	MSG_GUARD_WRITE_DATA,						// д�������ݵ�������
	MSG_GUARD_WRITE_DATA_RESULT,				// д�������ݵ����������

	MSG_GUARD_MAXID,							// �����Ϣ��
};

// MSG_GUARD_HANDSHAKE		���ݼ໤�����������������ݼ໤������������Ϣ���ݽṹ
struct SMsgGuardHandshake_SG
{
	DWORD	dwWorldID;							// ������������Ϸ����ID
	WORD	nServerType;						// ����������
	WORD	wSubID;								// ��n�ŷ�����
	char	szName[SERVER_NAME_MAXSIZE];		// ��������
	char	szWorldName[SERVER_NAME_MAXSIZE];	// ��Ϸ������

	SMsgGuardHandshake_SG(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_GUARD_HANDSHAKE_RESPONSE		���ݼ໤��������Ӧ������Ϣ���ݽṹ
struct SMsgGuardHandshakeResponse	
{
	DWORD	dwID;								// ����ID
	DWORD	dwServerTime;						// ������ʱ��

	SMsgGuardHandshakeResponse(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// MSG_GUARD_SERVERCHECKREQ			���ͷ��������ܼ���������ݽṹ
struct SMsgGuardServerCheck
{
	int nCheckTime;		// ��ʼ���ɼ���ʱ���
	int nSerial;		// ���
	int nGameWorldID;	// ��Ϸ����ID
	int nBeginTick;		// ��⿪ʼ��TICK��
	int nEndTick;		// ��������TICK��
	WORD wServerType;	// ����������
	WORD wServerNo;		// ��������ţ�ͬ����ʱ��
	int nFromServerID;	// ��Դ������ID
	int nToServerID;	// ���������ID

	SMsgGuardServerCheck()
	{
		memset(this, 0, sizeof(*this));
	}
};


// MSG_GUARD_READ_DATA,				�ӷ������ж���������
struct SMsgGuardReadData
{
	int			nNameSpace;
	int			nKey;
	UINT_PTR	nBackPtr;
	SMsgGuardReadData()
	{
		memset(this, 0, sizeof(*this));
	}
};
//MSG_GUARD_READ_DATA_RESULT,		�ӷ������ж��������ݽ��
struct SMsgGuardReadDataResult
{
	int			nNameSpace;			// ���������������� IMAGE_DATA_NAMESPACE
	int			nKey;				// ��ֵ
	UINT_PTR	nBackPtr;
	int			nDataLen;			// ���ݳ���
	// char* data;				// ����
	SMsgGuardReadDataResult()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_GUARD_WRITE_DATA,			д�������ݵ�������
struct SMsgGuardWriteData
{
	int			nNameSpace;
	int			nKey;
	UINT_PTR	nBackPtr;
	DWORD		dwDataLen;
	// char*  data
	SMsgGuardWriteData()
	{
		memset(this, 0, sizeof(*this));
	}
};

//MSG_GUARD_WRITE_DATA_RESULT,		д�������ݵ����������
struct SMsgGuardWriteDataResult
{
	int			nNameSpace;
	int			nKey;
	UINT_PTR	nBackPtr;
	bool		bResult;

	SMsgGuardWriteDataResult()
	{
		memset(this, 0, sizeof(*this));
	}
};
