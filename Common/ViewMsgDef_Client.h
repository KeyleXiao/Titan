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
#include "ViewRedisData.h"


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
// VC	��ս������Client


///////////////////////////////////////////////////////////////////
// ������Ϣ ENUM_MSG_VIEW_HANDSHAKE
// �ͻ�������������
struct SMsgView_CG_Handshake : public IFixMsg<SMsgView_CG_Handshake>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_CLIENT; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
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
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_CLIENT; };
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
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_CLIENT; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE_FAIL; }

	// ����Ϣ
	EnumHandshakeFail	eFailReason;
};


///////////////////////////////////////////////////////////////////
// �������	ENUM_MSG_VIEW_KEEPALIVE
FixMsgStruct(SMsgView_CG_KeepAlive)
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_CLIENT; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_KEEPALIVE; }
	// ����Ϣ
};


///////////////////////////////////////////////////////////////////
// ��Ӧ�������	ENUM_MSG_VIEW_ANSWER_KEEPALIVE
FixMsgStruct(SMsgView_GC_Answer_KeepAlive)
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_CLIENT; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_ANSWER_KEEPALIVE; }
	// ����Ϣ
};


///////////////////////////////////////////////////////////////////
// ս��Ԥ����Ϣ	ENUM_MSG_VIEW_BATTLE_PREP
FixMsgStruct(SMsgView_VC_BattlePrep)
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEW; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_CLIENT; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_BATTLE_PREP; }

	ReplayID						rID;		// ս��ID
	ViewRedisData::BattlePrepInfo	sPrepInfo;	// ս��Ԥ����Ϣ
};

///////////////////////////////////////////////////////////////////
// ս����Ϣ	ENUM_MSG_VIEW_BATTLE_INFO
struct SMsgView_VC_BattleInfo : public ISerializableData
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEW; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_CLIENT; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_BATTLE_INFO; }

	ReplayID						rID;		// ս��ID
	ViewRedisData::BattleInfo		sBattleInfo;// ս����Ϣ(ĳһʱ�̵�)

	SMsgView_VC_BattleInfo() : rID(0)
	{}

	SMsgView_VC_BattleInfo(const ReplayID& id, const ViewRedisData::BattleInfo& info)
		: rID(id), sBattleInfo(info)
	{}

	SMsgView_VC_BattleInfo(const ReplayID& id, ViewRedisData::BattleInfo&& info)
		: rID(id), sBattleInfo(info)
	{}

	virtual void fromBytes(const char* p, size_t len) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}


	virtual void toBytes(obuf& obufData) const override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

};


#pragma pack()