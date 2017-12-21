/*******************************************************************
** �ļ���:	ViewMsgDef_Server.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/11/29
** ��  ��:	1.0
** ��  ��:	��սϵͳ��Ϣ�ṹ�嶨�壨�������������֮�����Ϣ��
** Ӧ  ��:
**************************** �޸ļ�¼ ******************************
** �޸���:
** ��  ��:
** ��  ��:
********************************************************************/
#pragma once
#include "ViewMsgDef.h"
#include "IByteRecord.h"
#include "IMsg.h"
#include "ViewDef_.h"
#include <vector>
#include "ISerializableData.h"

#pragma pack(1)

/*****************************************************************/
///////////////////////// ��սģ�� ����Ϣ�ṹ��////////////////////
// �ϲ����Ϣ��ض��� MSG_MODULEID_VIEW
/*****************************************************************/
// ��Ϣ��ʽΪ SMsgView_ + ��ֹ�ڵ� + ��Ϣ��
// ��ֹ�ڵ㶨�����£�
// GM	���ء��������
// GV	���ء�����ս��
// VG	��ս����������
// VM	��ս�����������
// MG	�������������
// MV	�����������ս��
// SM	��Ϸ�����������
// MS	�����������Ϸ��

///////////////////////////////////////////////////////////////////
// ������Ϣ ENUM_MSG_VIEW_HANDSHAKE
// ��ս������������
struct SMsgView_VM_Handshake : public IFixMsg<SMsgView_VM_Handshake>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE; }

	DWORD	dwServerID;		// ServerID
	DWORD	dwMaxCount;		// �ܳ��ص��������
	DWORD	dwCurrCount;	// ��ǰ����
};
struct SMsgView_GM_Handshake : public IFixMsg<SMsgView_GM_Handshake>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE; }

	DWORD	dwServerID;		// ServerID
	WORD	wPort;			// ��Gate����ViewServer�Ķ˿�
	DWORD	dwMaxCount;		// �ܳ��ص��������
	DWORD	dwCurrCount;	// ��ǰ����
};
struct SMsgView_VG_Handshake : public IFixMsg<SMsgView_VG_Handshake>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE; }

	DWORD	dwServerID;	// ServerID
	DWORD	dwID;		// MngΪ�����ɵĶ�̬ID
};
struct SMsgView_SM_Handshake : public IFixMsg<SMsgView_SM_Handshake>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE; }

	// ����Ϣ
};


///////////////////////////////////////////////////////////////////
// ��Ӧ������Ϣ ENUM_MSG_VIEW_HANDSHAKE_RESPONSE
// ������ظ���ս��
struct SMsgView_MV_HandshakeResponse : public IFixMsg<SMsgView_MV_HandshakeResponse>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE_RESPONSE; }

	DWORD	dwID;	// MngΪ�����ɵĶ�̬ID
};
struct SMsgView_MG_HandshakeResponse : public IFixMsg<SMsgView_MG_HandshakeResponse>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE_RESPONSE; }

	DWORD	dwID;	// MngΪ�����ɵĶ�̬ID
};
struct SMsgView_GV_HandshakeResponse : public IFixMsg<SMsgView_GV_HandshakeResponse>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE_RESPONSE; }

	DWORD	dwID;	// MngΪGate���ɵĶ�̬ID
};
struct SMsgView_MS_HandshakeResponse : public IFixMsg<SMsgView_MS_HandshakeResponse>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE_RESPONSE; }

	DWORD	dwID;	// MngΪ�����ɵĶ�̬ID
};


///////////////////////////////////////////////////////////////////
// �������	ENUM_MSG_VIEW_KEEPALIVE
struct SMsgView_VM_KeepAlive : public IFixMsg<SMsgView_VM_KeepAlive>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_KEEPALIVE; }

	// ����Ϣ
};
struct SMsgView_GM_KeepAlive : public IFixMsg<SMsgView_GM_KeepAlive>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_KEEPALIVE; }

	// ����Ϣ
};
struct SMsgView_VG_KeepAlive : public IFixMsg<SMsgView_VG_KeepAlive>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_KEEPALIVE; }

	// ����Ϣ
};
FixMsgStruct(SMsgView_SM_KeepAlive)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_KEEPALIVE; }

	// ����Ϣ
};


///////////////////////////////////////////////////////////////////
// ��Ӧ�������	ENUM_MSG_VIEW_ANSWER_KEEPALIVE
struct SMsgView_MV_Answer_KeepAlive : public IFixMsg<SMsgView_MV_Answer_KeepAlive>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_ANSWER_KEEPALIVE; }
	// ����Ϣ
};
struct SMsgView_MG_Answer_KeepAlive : public IFixMsg<SMsgView_MG_Answer_KeepAlive>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_ANSWER_KEEPALIVE; }
	// ����Ϣ
};
struct SMsgView_GV_Answer_KeepAlive : public IFixMsg<SMsgView_GV_Answer_KeepAlive>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_ANSWER_KEEPALIVE; }
	// ����Ϣ
};
FixMsgStruct(SMsgView_MS_Answer_KeepAlive)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_ANSWER_KEEPALIVE; }
	// ����Ϣ
};


///////////////////////////////////////////////////////////////////
// �������ص���Ϣ ENUM_MSG_VIEW_GATES_INFO
struct SGateInfo
{
	std::string	strIp;
	WORD		wPort;
	WORD		wID;
};
typedef	std::vector<SGateInfo*> VecGateInfo;
struct SMsgView_MV_GatesInfo : public ISerializableData
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_GATES_INFO; }

	VecGateInfo		vecGates;

	~SMsgView_MV_GatesInfo()
	{
		auto count = vecGates.size();
		for (int i = 0; i < count; ++i)
		{
			SGateInfo*& pInfo = vecGates.at(i);
			safeDelete(pInfo);
		}
		vecGates.clear();
	}

	// �����л�
	virtual void fromBytes(const char* p, size_t len) override
	{
		ibuffer ibufData(p, len);
		WORD count;
		ibufData >> count;

		for (int i = 0; i < count; ++i)
		{
			SGateInfo* pInfo = new SGateInfo;
			ibufData >> pInfo->strIp >> pInfo->wPort >> pInfo->wID;
			vecGates.push_back(pInfo);
		}
	}

	// ���л�
	virtual void toBytes(obuf& obufData) const override
	{
		WORD count = (WORD)vecGates.size();
		obufData << count;

		for (int i = 0; i < count; ++i)
		{
			SGateInfo* pInfo = vecGates.at(i);
			obufData << pInfo->strIp << pInfo->wPort << pInfo->wID;
		}
	}
};


///////////////////////////////////////////////////////////////////
// ����ۿ�ĳ��ս��	ENUM_MSG_VIEW_REQUEST_REPLAY
// �ظ���Game���Ľ��ö��
FixMsgStruct(SMsgView_SM_RequestReplay)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_REQUEST_REPLAY; }

	PlayerID	dwPlayerID;	// ��ʶ��ս�ͻ��˵�ΨһID
	ReplayID	rID;		// ս��ID
	WORD		wRightID;	// Ȩ��ID
};


///////////////////////////////////////////////////////////////////
// ����ۿ�ĳ��ս�� �Ļظ�	ENUM_MSG_VIEW_REQUEST_REPLAY_FAIL
enum EnumResultRequestReplay
{
	E_RESULT_RR_SUCCESS = 0,		// �ɹ�
	E_RESULT_RR_ERR_NO_VIEW_SERVER,	// û��ViewServer����
	E_RESULT_RR_ERR_VIEW_IS_FULL,	// ����ViewServer����
	E_RESULT_RR_ERR_GATE_IS_FULL,	// ����Gate����

	E_RESULT_RR_MAX,
};
FixMsgStruct(SMsgView_SM_RequestReplay_Fail)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_REQUEST_REPLAY_FAIL; }

	EnumResultRequestReplay		eResult;	// ���ö�٣�ֻ�����Ǹ��ִ�����
	SMsgView_SM_RequestReplay	reqMsg;		// ֮ǰGame������������Ϣ
};


///////////////////////////////////////////////////////////////////
// �뿪ʼ����ĳ��ս��	ENUM_MSG_VIEW_START_VIEW
FixMsgStruct(SMsgView_MV_StartView)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_START_VIEW; }

	ReplayID	rID;		// ս��ID
};


///////////////////////////////////////////////////////////////////
// IDΪXX����ҽ�Ҫ����ĳ�ֱ�������ĳ̨View�ϣ�����Ӵ���������Կ...����ʱʱ��ΪT	ENUM_MSG_VIEW_REQUEST_ADD_PLAYER
FixMsgStruct(SMsgView_MG_RequestAddPlayer)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_REQUEST_ADD_PLAYER; }

	SMsgView_SM_RequestReplay	reqMsg;		// ֮ǰGame������������Ϣ
	ViewID	wViewID;						// ViewServerID
	BYTE	szKeyData[VIEW_SECRET_KEY_SIZE];// ��Կ
	DWORD	dwTimeOut;						// ��ʱʱ��
};


///////////////////////////////////////////////////////////////////
// Gate�ظ�Mng�������ҽ��	ENUM_MSG_VIEW_REQUEST_ADD_PLAYER_REPLY
enum EnumResultRequestAddPlayer
{
	E_RESULT_RAP_SUCCESS = 0,		// �ɹ�
	E_RESULT_RAP_ERR_PLAYER_EXIST,	// ��ID����Ѵ����ڱ�Gate�����ѵ�½
	E_RESULT_RAP_ERR_AUTH_EXIST,	// ��ID��ҵĵ�½��Ϣ�Ѵ����ڱ�Gate������δ��½
	E_RESULT_RAP_ERR_NOT_REACH_VIEW,// ��ViewServer�����Ӳ���(��ʹ��������Ҳ�п��ܳ���ĳ��̨����֮�����Ӳ��ϵ����)

	E_RESULT_RAP_MAX,
};
FixMsgStruct(SMsgView_GM_RequestAddPlayerReply)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_REQUEST_ADD_PLAYER_REPLY; }

	EnumResultRequestAddPlayer	eResult;	// ���ö��
	PlayerID					dwPlayerID;	// ��ʶ��ս�ͻ��˵�ΨһID
};


///////////////////////////////////////////////////////////////////
// Gate֪ͨMng��ĳ��ҵ���֤��Ϣɾ��	ENUM_MSG_VIEW_AUTH_DEL
enum EnumAuthDelReason
{
	E_AuthDelR_HANDSHAKE,		// ���ֵ�½�ɹ�
	E_AuthDelR_TIME_OUT,		// ��ʱ��
	E_AuthDelR_NOT_REACH_VIEW,	// ��ViewServer�����Ӳ����ˣ���Ȼ֮ǰ�����ϣ�

	E_AuthDelR_MAX,
};
FixMsgStruct(SMsgView_GM_AuthDel)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_AUTH_DEL; }

	PlayerID			dwPlayerID;	// ��ʶ��ս�ͻ��˵�ΨһID
	EnumAuthDelReason	eReason;	// ԭ��
};


///////////////////////////////////////////////////////////////////
//֪ͨView��ĳClientҪ�ۿ�ĳս������ĳ��Ȩ��	ENUM_MSG_VIEW_C_REQUEST_REPLAY
FixMsgStruct(SMsgView_GV_CRequestReplay)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_C_REQUEST_REPLAY; }

	SMsgView_SM_RequestReplay	reqMsg;		// ֮ǰGame������������Ϣ
};


#pragma pack()