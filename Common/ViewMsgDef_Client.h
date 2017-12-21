/*******************************************************************
** �ļ���:	ViewMsgDef_Client.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/11/29
** ��  ��:	1.0
** ��  ��:	��սϵͳ��Ϣ�ṹ�嶨�壨�ͻ����������֮�����Ϣ��
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "IMsg.h"
#include "ViewMsgDef.h"
#include "buffer.h"
#include "ViewDef_.h"


#pragma pack(1)
/*****************************************************************/
///////////////////////// ��սģ�� ����Ϣ�ṹ��////////////////////
// �ϲ����Ϣ��ض��� MSG_MODULEID_VIEW
/*****************************************************************/
// ��Ϣ��ʽΪ SMsgView_ + ��ֹ�ڵ� + ��Ϣ��
// ��ֹ�ڵ㶨�����£�
// CG	Client��������
// CM	Client���������
// CV	Client������ս��


///////////////////////////////////////////////////////////////////
// ������Ϣ ENUM_MSG_VIEW_HANDSHAKE
// �ͻ�������������
struct SMsgView_CG_Handshake : public IFixMsg<SMsgView_CG_Handshake>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE; }

	PlayerID	dwPlayerID;						// PlayerID
	BYTE		szKey[VIEW_SECRET_KEY_SIZE];	// ������֤��Key
	char		szMacAddress[MAC_ADDRESS_SIZE];	// ������ַ
};

///////////////////////////////////////////////////////////////////
// ��Ӧ������Ϣ ENUM_MSG_VIEW_HANDSHAKE_RESPONSE
// ������ظ���ս��
struct SMsgView_GC_HandshakeResponse : public IFixMsg<SMsgView_GC_HandshakeResponse>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE_RESPONSE; }

	// ����Ϣ
};

///////////////////////////////////////////////////////////////////
// Gate�ظ�Client����½ʧ��	ENUM_MSG_VIEW_HANDSHAKE_FAIL
enum EnumHandshakeFail
{
	E_HANDSHAKE_FAIL_NO_ID = 0,		// ����û�����ID
	E_HANDSHAKE_FAIL_NOT_REACH_VIEW,// ��ViewServer�����Ӳ���(��ʹ��������Ҳ�п��ܳ���ĳ��̨����֮�����Ӳ��ϵ����)
	E_HANDSHAKE_FAIL_ERR_KEY,		// key����
	E_HANDSHAKE_FAIL_IS_FULL,		// ��������Ա

	E_HANDSHAKE_FAIL_MAX,
};
FixMsgStruct(SMsgView_GC_HandshakeFail)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE_FAIL; }

	// ����Ϣ
	EnumHandshakeFail	eFailReason;
};


///////////////////////////////////////////////////////////////////
// �������	ENUM_MSG_VIEW_KEEPALIVE
FixMsgStruct(SMsgView_CG_KeepAlive)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_KEEPALIVE; }
	// ����Ϣ
};


///////////////////////////////////////////////////////////////////
// ��Ӧ�������	ENUM_MSG_VIEW_ANSWER_KEEPALIVE
FixMsgStruct(SMsgView_GC_Answer_KeepAlive)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_ANSWER_KEEPALIVE; }
	// ����Ϣ
};

#pragma pack()