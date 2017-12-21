/*******************************************************************
** �ļ���:	VoiceDef.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	������ (sww8@163.com)
** ��  ��:	2011-05-16
** ��  ��:	1.0
** ��  ��:	����ϵͳ��ض���,��Ҫ�������ͻ���,����������������������ϵͳ�Ķ���,��Ҫ������Ϸ�ض�ָ����ϷӦ��,�еĻ�����VoiceModule��
** Ӧ  ��:  ��VoiceModule�ֳ�,����Ķ��岻�ض�ָ����ϷӦ��,�����ж˹���
	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once

#include "View_ManagedDef.h"
#include "buffer.h"


// ��ӡ������Ϣ,����Ϊ����ģʽ
//#define VOICE_PRINT_DEBUG_INF

// ��Ϊѹ������ģʽ
//#define VOICE_BIG_TEST

//#define VOICE_USE_UDP_SEND	// ��UPD��������Ʋ�һ��,���û���̫��

// ��Base���UDP�����շ�
#define VOICE_USE_BASEUDP

// ������ģ��
#define VOICE_MODUEL_OPEN
#define VC_OS_WINDOWS

#ifndef SafeRelease
#	define SafeRelease(ptr)		if ((ptr)) {(ptr)->Release(); (ptr) = 0;}
#endif



#pragma pack(1)

/////// �����궨�� ///////////////////////////////////////////////////////////////////

// tolua_begin

#define  VOICE_INVALID_ID						0xFFFFFFFF		//DWORD �������,0xFFFFFFFF
#define  VOICE_MAX_WORD_NUM						0xFFFF			//WORD �������,0xFFFF

#define  VOICE_NAME_MAXSIZE						32				//������󳤶�,32
#define  VOICE_MSGSTR_MAXSIZE					64				//��Ϣ������󳤶�,64
#define  VOICE_TITLE_MAXSIZE					128				//�����ִ��ĳ���,128
#define  VOICE_LONGSTR_MAXSIZE					256				//���ִ��ĳ���,256
#define  VOICE_TEXTSTR_MAXSIZE					512				//���ִ��ĳ���,512

/// ��������10��
#define  VOICE_RECONNECT_INTERVAL				(10*1000)


#define  VOICE_BROADCASTALL_NUM					0xFFFF			//WORD �������,0xFFFF


#define  VOICE_KEEPALIVE_INTERVAL				2000			// ����KeepAlive̽����ʱ����
#define  VOICE_KEEPALIVE_TIMES					5000			// ��ʼ�״�KeepAlive̽��ǰ��TCP�ձ�ʱ��
#define  VOICE_KEEPALIVE_TIMEOUT				3000			// ����Ӧʱ��
#define  VOICE_KEEPALIVE_MAXTIMEOUT				120000			// �������Ӧʱ��


#define  VIEW_MAX_GATEWAYID						64				// ��Ϊ��սClientID��ֻ����8λ��ʾ����ID,��������ID���ֻ��255λ
#define  VOICE_MAX_CLIENT_COUNT					8192			// һ̨���ط����������ص����� ��� 8192 
#define  VOICE_MAX_ROOM_USERCOUNT				100000			// �����������

#define  VOICE_RC4_KEY_SIZE						64				// RC4��Կ����
#define  VOICE_RC4_MAX_SIZE						64				// RC4��Կ��󳤶�

#define  VOICE_GATEWAY_AUDIONET_MAX				80				// ��������������Ƶ��ר������ͨ����󻺴����,������������������ķ�������(Ĭ��80,���40����������)


// tolua_end


// �����ͻ���ID��Ϣ
struct VClientIDInfo
{
	DWORD dwGatewayID : 8;  // ��������ID
	DWORD dwConnIndex :24;  // �����������

	VClientIDInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// Ƶ��
struct SVoiceChannelID
{
	DWORD	dwRoomID:8;		// ����ID
	DWORD	dwType:8;		// Ƶ������
	DWORD	dwWorldID:16;	// ����ID
	DWORD	dwID:32;		// Ƶ��ID
};

// ����ѹ�������ṹ
struct SVoiceCodecInfo
{
	// speex���ͽṹ��Ϣ
	struct SpeexInfo
	{
		bool bVBREnabled;	// �Ƿ��Ƕ�̬������
		BYTE nBand;			// (SPEEX)�����������ƣ�0 - 8KHZ ��1 - 16KHZ �еȣ�2 - 32KHZ ���
		BYTE nQuality;		// (SPEEX)�����������ƣ�0 ~ 10�� 0 ��10 ���
	};
	// celt����
	struct CELTInfo
	{
		bool bVBREnabled;	// �Ƿ��Ƕ�̬������	
		UINT nSampleRate;	// (CELTѹ����)������������ 32000 ~ 96000���ӵ͵��� Ĭ��48000
		UINT nVBRTargetBitrate;//(CELTѹ����),����ı�����,��128000 Ĭ��32000
	};

	union
	{
		SpeexInfo speex;
		CELTInfo celt;
	} Info;


	BYTE nType;			// ������������ EMVoiceCodecType
	BYTE nChannelCount;	// ֧�ֵ���Ƶͨ����,������Ϊ2,������Ϊ1
};


// tolua_begin

/////// ����ö�ٶ��� ///////////////////////////////////////////////////////////////////

// ���������߳�ִ�в������붨��
enum EMVoiceServerControlCode
{
	VS_ControlCode_None = 0,		// �޲���
	VS_ControlCode_StartService,	// ��������
	VS_ControlCode_StopService,		// ֹͣ����
	VS_ControlCode_ShutDown,		// �رճ���,ֹͣ�����Ҫ�رս���
	// ���
	VS_ControlCode_Max
};

// �������б�������Ͷ���
enum EMVoiceServerListUpdateType
{
	VS_SvrListUpdateType_All = 0,		// ȫ��
	VS_SvrListUpdateType_Add,			// ����
	VS_SvrListUpdateType_Edit,			// �޸�
	VS_SvrListUpdateType_Del,			// ɾ��
	// ���
	VS_SvrListUpdateType_Max
};

// ��������
enum EMVoiceCommand
{
	VoiceCommand_First = 1,
	VoiceCommand_ChangeMaxConnection = VoiceCommand_First,	// �޸����������
	VoiceCommand_ExportUserList,							// �����û��б�
	VoiceCommand_NetSpeedMax,								// ��������������(Mbps)

	VoiceCommand_Max
};

// ����ת��ʱʹ�õ�ͨ������
enum EMVoiceSendChannelType
{
	VoiceSendChannel_Control = 1,		// ��������ͨ��
	VoiceSendChannel_Voice,				// ��������ͨ��
};


// ���������ټ��Ĺ㲥����
enum EMVoiceADBroadcastType
{
	VoiceBroadcast_None			= 0x0,		// ��Ч
	VoiceBroadcast_SelfWorld	= 0x1,		// �㲥�Լ���
	VoiceBroadcast_AllWorld		= 0x2,		// �㲥������
	VoiceBroadcast_ExceptSelfWorld	= 0x4,	// �㲥���Լ���
};

// ����������Ƶ������λ��ʶ,8λ
enum EMVoiceStreamDataFlag
{
	VoiceStreamData_None			= 0x0,		// ��Ч
	VoiceStreamData_Audio			= 0x1,		// �к���������,�����ǽ�����
	VoiceStreamData_Video			= 0x2,		// ������Ƶ����,Ҫ�ò�ͬ�������ʽ�Ͳ����ʽ,����û����Ƶʱ������Ƶ��Ϣ�Խ�ʡ������
	VoiceStreamData_KeyVideo		= 0x4,		// ��ʾ��ǰ֡�ǹؼ�֡
	//VoiceStreamData_NoUse			= 0x8,		// 
};

// ����Ȩ��
#define VOICE_USER_RIGHT_ALL		0xFFFFFFFF
// OW��Ƶ��ӵ����Ȩ��
#define VOICE_USER_RIGHT_OW			(VoiceRight_Owner|VoiceRight_Ban|VoiceRight_Kick|VoiceRight_Move|VoiceRight_Broadcast|VoiceRight_AD|VoiceRight_Mic|VoiceRight_Lock|VoiceRight_Speak|VoiceRight_Vest)
// admin������ԱȨ��
#define VOICE_USER_RIGHT_ADMIN		(VoiceRight_Kick|VoiceRight_Move|VoiceRight_AD|VoiceRight_Mic|VoiceRight_Lock|VoiceRight_Speak|VoiceRight_Vest|VoiceRight_Ban)
// Host��������Ȩ��
#define VOICE_USER_RIGHT_HOST		(VoiceRight_Kick|VoiceRight_Move|VoiceRight_AD|VoiceRight_Mic|VoiceRight_Speak|VoiceRight_Ban)	
// VIP���α�Ȩ��
#define VOICE_USER_RIGHT_VIP		(VoiceRight_Speak)

// Ƶ������״̬���
enum EMVoiceChannelRunFlag
{
    VoiceChannelRunFlag_Saving			= 0x1,	///  Ƶ������ʱ�������ڱ������ݿ�
    //VoiceChannelRunFlag_Unuse		= 0x2,	/// 
    //VoiceChannelRunFlag_Unuse		= 0x4,	/// 
    //VoiceChannelRunFlag_Unuse		= 0x8,	/// 
    //VoiceChannelRunFlag_Unuse		= 0x10,	/// 
    //VoiceChannelRunFlag_Unuse		= 0x20,	/// 
    //VoiceChannelRunFlag_Unuse		= 0x40,	/// 
    //VoiceChannelRunFlag_Unuse		= 0x80,	/// 
    //VoiceChannelRunFlag_Unuse		= 0x100, ///
    //VoiceChannelRunFlag_Unuse		= 0x200, ///
    //VoiceChannelRunFlag_Unuse		= 0x400, ///
    //VoiceChannelRunFlag_Unuse		= 0x800, /// 
    //VoiceChannelRunFlag_Unuse  		= 0x1000, /// 
    //VoiceChannelRunFlag_Unuse		= 0x2000, /// 
};


// Ƶ��Ӧ������
enum EMVoiceChannelUseType
{
	VoiceChannelUseType_None = 0,	// Ĭ��


	VoiceChannelUseType_Max,
};



// ���������Ϣ ���紫����
enum EMVoiceRoomBaseInfo
{
	VoiceRoomBaseInfo_Lock0			= 0x1,	/// 0�ŷ������
	VoiceRoomBaseInfo_Lock1			= 0x2,	/// 1�ŷ������
	VoiceRoomBaseInfo_Lock2			= 0x4,	/// 2�ŷ������
	VoiceRoomBaseInfo_Lock3			= 0x8,	/// 3�ŷ������
	VoiceRoomBaseInfo_Lock4			= 0x10,	/// 4�ŷ������
	VoiceRoomBaseInfo_Lock5			= 0x20,	/// 5�ŷ������
	VoiceRoomBaseInfo_Lock6			= 0x40,	/// 6�ŷ������
	VoiceRoomBaseInfo_Lock7			= 0x80,	/// 7�ŷ������ VoiceRoomBaseInfo_Lock��������Ҫ��VoiceRoomType_Max����һ��
	//VoiceRoomState_Unuse			= 0x100, ///
	//VoiceRoomState_Unuse			= 0x200, ///
	//VoiceRoomState_Unuse			= 0x400, ///
	//VoiceRoomState_Unuse			= 0x800, /// 
	//VoiceRoomState_Unuse  		= 0x1000, /// 
	//VoiceRoomState_Unuse			= 0x2000, /// 
};

// ���а�״̬
enum EMVoicePersonalTopState
{
	VoicePersonalTopState_Voice			= 0x1,	/// ��������
	VoicePersonalTopState_Video			= 0x2,	/// ������Ƶ
	//VoicePersonalTopState_Unuse			= 0x4,	/// 
	//VoicePersonalTopState_Unuse			= 0x8,	/// 
	//VoicePersonalTopState_Unuse			= 0x10,	/// 
	//VoicePersonalTopState_Unuse			= 0x20,	/// 
	//VoicePersonalTopState_Unuse			= 0x40,	/// 
	//VoicePersonalTopState_Unuse			= 0x80,	/// 
	//VoicePersonalTopState_Unuse			= 0x100, ///
	//VoicePersonalTopState_Unuse			= 0x200, ///
	//VoicePersonalTopState_Unuse			= 0x400, ///
	//VoicePersonalTopState_Unuse			= 0x800, /// 
	//VoicePersonalTopState_Unuse  		= 0x1000, /// 
	//VoicePersonalTopState_Unuse			= 0x2000, ///
};

// ����ģ���б��������
enum EMVoiceUpdateType
{
	VoiceUpdateType_All = 0,		// ͬ��ȫ��
	VoiceUpdateType_Add,			// ���� 
	VoiceUpdateType_Edit,			// �޸�
	VoiceUpdateType_Delete,			// ɾ��
};

// ���������������û���Ϣ���ͻ���
enum EMVoiceUpdateUserType
{
	VoiceUpdateUserType_All = 0,		// ȫ��
	VoiceUpdateUserType_Base,			// ���»�����Ϣ
	VoiceUpdateUserType_Type,			// ����ָ�����Ƶ��,Ȩ��
	VoiceUpdateUserType_AllChannel,		// ȫ��Ƶ����Ϣ
	VoiceUpdateUserType_AllRight,		// ����Ȩ��
	VoiceUpdateUserType_AllType,		// ��������
};

// �������������·�����Ϣ���ͻ���
enum EMVoiceUpdateRoomType
{
	VoiceUpdateRoomType_All = 0,		// ȫ��
	VoiceUpdateRoomType_Base,			// ������Ϣ
	VoiceUpdateRoomType_MicUser,		// ���ϵ�����Ϣ
	VoiceUpdateRoomType_Remark,			// ���¹���
};

// ��������������Ƶ����Ϣ���ͻ���
enum EMVoiceUpdateChannelType
{
	VoiceUpdateChannelType_All = 0,		// Ƶ��ȫ����Ϣ���Ժ���չ��
	VoiceUpdateChannelType_Base,		// Ƶ��������Ϣ
	VoiceUpdateChannelType_Audio,		// Ƶ���﷿���Ƿ�����������Ϣ
	VoiceUpdateChannelType_Name,		// Ƶ������
	VoiceUpdateChannelType_RoomName,	// Ƶ����������
};

// ��������������Ƶ����Ϣ���ͻ���
enum EMVoiceUpdateAllChannelType
{
	VoiceUpdateAllChannelType_All = 0,			// ����Ƶ����ȫ����Ϣ���Ժ���չ��
	VoiceUpdateAllChannelType_Audio,			// ����Ƶ���Ƿ�����������Ϣ
};


// ����һ����Ա�Ĳ�����Ϣ���ͻ���
enum EMVoiceUpdateUserPartInfoType
{
	VoiceUpdateUserPartInfoType_StateFlag = 0,	// �û�״̬���
	VoiceUpdateUserPartInfoType_FansCount,		// ��˿����
};


// �û��߳�ԭ��
enum EMVoiceKickReason
{
	VoiceKickReason_Default = 0,		// Ĭ��
	VoiceKickReason_Full,				// ��������
	VoiceKickReason_LoginTimeout,		// ��¼��ʱ
	VoiceKickReason_LoginError,			// ��¼ʧ��
	VoiceKickReason_UserExist,			// �û�����

	VoiceKickReason_Max,
};

// �ƶ��������
enum EMVoiceMoveMicOrderType
{
	VoiceMoveMicOrder_Up,		// ��������
	VoiceMoveMicOrder_Down,		// ��������
	VoiceMoveMicOrder_Jump2,	// ����2������
};


// ˽������
enum EMVoiceChatConfig
{
	VoiceChatConfig_Defualt,	// Ĭ�����ã������κ��˵�˽��
	VoiceChatConfig_Ban,		// �������κ�˽��
	VoiceChatConfig_BanStranger,// ������İ����˽��
};

// ˽���û�����
enum EMVoiceChatUserType
{
	VoiceChatUserType_None,		// ��Ч����
	VoiceChatUserType_Inviter,	// ������
	VoiceChatUserType_Acceptor,	// ������
	VoiceChatUserType_Visitor,	// �����ߣ���Ҫ��ָGM

	VoiceChatUserType_Max,
};


/// ϵͳ��Ϣ����������Ϣ��λ�ö��壬����������Ϣ���壬Ϊ�˲���ͻ������һ��
enum EMVoiceInfoPos
{
	VoiceInfoPos_ChatWindow		= 0x1,	/// �����
	VoiceInfoPos_ScreenTopAD	= 0x2,	/// �ö������
	VoiceInfoPos_ScreenTopLarge	= 0x4,	/// �ö�������Ϣ
	VoiceInfoPos_ActorAbove		= 0x8,	/// ��ɫ�Ϸ�
	VoiceInfoPos_ActorUnder		= 0x10,	/// ��ɫ�·�
	VoiceInfoPos_ActorRight		= 0x20,	/// ��ɫ�ҷ�
	VoiceInfoPos_ScreenRight	= 0x40,	/// ��Ļ�ҷ�
	VoiceInfoPos_MouseInfo		= 0x80,	/// ��������Ϣ
	VoiceInfoPos_WarTips		= 0x100, ///ս����ʾ��Ϣ
	VoiceInfoPos_ScreenCenter	= 0x200, ///��Ļ���Ϸ������ʾ��Ϣ
	VoiceInfoPos_Rosponse		= 0x400, ///��ʱ��ʾ��Ϣ
	VoiceInfoPos_ActorUnderLarge= 0x800, /// ��ɫ�·�������Ϣ
	VoiceInfoPos_ScreenAnim1  	= 0x1000, /// ��Ļ����������Ϣ
	VoiceInfoPos_ScreenAnim2	= 0x2000, /// ��Ļ����������Ϣ

	VoiceMaxInfoPosCount		= 0x3FFF,	/// ���λ����
};


/// ϵͳ��Ϣ������������Ϣ���壬Ϊ�˲���ͻ������һ��
enum EMVoiceTipType
{
	VoiceTipType_Nothing = 0,		/// �޶���0
	VoiceTipType_System,			/// ϵͳ��ʾ1
	VoiceTipType_Operate,			/// ������ʾ2
	VoiceTipType_War,				/// ս����PK��ʾ3
	VoiceTipType_Task,				/// ������ʾ4
	VoiceTipType_Error,				/// ������ʾ5
	VoiceTipType_Warning,			/// ������ʾ6
	VoiceTipType_Boss,				/// boss��Ϣ��ʾ7
	VoiceTipType_SystemTips,		/// ϵͳ���ٷ�����8
	VoiceTipType_Equip,				/// װ�����9
	VoiceTipType_Conscribe,			/// սʿ��ļ10
	VoiceTipType_Weal,				/// ��Ա����11
	VoiceTipType_Auction,			/// ������Ϣ12

	VoiceMaxTipTypeCount,			///���������
};


/////// �ͻ��˹���ö�ٶ���/////////////////////////////////////////////



// ������������
enum EMVoiceCodecType
{
	VoiceCodeType_Speex = 0,	// speex����
	VoiceCodeType_Celt,			// Celt����
	VoiceCodeType_Max,			// �������
};


/////// ����������ö�ٶ���/////////////////////////////////////////////

// �����������û�״̬����
enum EMVoiceUserState
{
	VoiceUserState_None = 0,	// ��Ч
	VoiceUserState_Offline,		// ������
	VoiceUserState_Online,		// ����δ��¼
	VoiceUserState_Login,		// �����ѵ�¼

	VoiceUserState_Max,
};

///// �������ݽṹ���� //////////////////////////////////////////////

// ͨ������������������ת��Ϣ����������Ϣ��
struct SMsgVoiceHead
{
	DWORD dwWorldID;		// ��Ϣ��Դ��Ϸ����ID
	DWORD dwServerID;		// ��Ϣ��Դ������ID
};

// ��Ϸ��������Ϣ���ݽṹ
struct SVoiceGameServerInfo
{
	DWORD	dwID;				// ������ID
	DWORD	dwWorldID;			// ������������Ϸ����ID
	WORD	wServerType;		// ����������
	WORD	wSubID;				// ��n�ŷ�����
	DWORD	dwStateFlag;		// ������״̬
	void*	pExtPart;			// ��������չ�����ӿ�

	SVoiceGameServerInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// ������ṹ��
struct SVoiceCountryFreeMic
{
	char	szNickname[VOICE_NAME_MAXSIZE];		// �ǳ�
	DWORD	dwPDBID;
	BYTE	nCountryID;			// ����ID

	SVoiceCountryFreeMic(void)
	{
		memset( this, 0, sizeof(SVoiceCountryFreeMic) );
	}
};

// Ƶ�����ýṹ
struct SVoiceChannelConfig
{
	DWORD	dwChannelType;			// Ƶ������   EMVoiceChannelType
	bool	bOpen;					// �Ƿ񿪷�
	bool	bUserList;				// �Ƿ�֧�ֳ�Ա�б�
	bool	bEnterFree;				// �Ƿ�֧�ֿ������������͵�Ƶ���������Ƶ��
	bool	bMicOrder;				// �Ƿ�֧������ģʽ
	bool	bMicFree;				// �Ƿ�֧������ģʽ
	WORD	wCallMinLevel;			// �ټ���Ҫ����С�ȼ���0Ϊ������
	DWORD	dwChatCoolDown;			// ����������ȴʱ�䣬��
	DWORD	dwVestCount;			// ��ʱ�������
	bool	bPush;					// Ƶ������
	DWORD	dwMaxCount;				// �������
	DWORD	dwVoiceQuality;			// ����Ʒ�� EMVoiceQualityType
	DWORD	dwADCoolTime;			// �����ټ�����ȴʱ��
};

// �������ýṹ
struct SVoiceRoomConfig
{
	DWORD	dwMicState;				// Ĭ������ģʽ
	DWORD	dwMicTimeLen;			// ����ģʽ�£�ʱ����
	DWORD	dwMicInvalidTime;		// ����ģʽ�£���ò�˵�����Զ�����
	BYTE	nPush;					// ��������
};



// �����������Ͽͻ��˵�¼��Ϣ
struct SVoiceClientLoginInfo
{
	DWORD	dwConnectTime;	// ����ʱ��
	DWORD	dwRemoteIP;		// �ÿͻ���IP

	SVoiceClientLoginInfo(void)
	{
		memset( this, 0, sizeof(SVoiceClientLoginInfo) );
	}
};

// ����Ƶ��������Ϣ
struct SVoiceChannelBaseInfo
{
	DWORD dwStateFlag;		// ״̬��� EMVoiceChannelStateFlag
	DWORD dwChatCoolDown;	// ����������ȴʱ�䣬��
	DWORD dwOwnerUDBID;		// Ƶ��ӵ����UDBID
	DWORD dwQuality;		// ����Ʒ��  EMVoiceQualityType
	DWORD dwDefautRoomID;	// Ĭ�Ͻ���ķ���ID
	DWORD dwUseType;		// Ӧ������
	DWORD dwSpare1;			// ����1
	DWORD dwSpare2;			// ����2
	DWORD dwSpare3;			// ����3
	DWORD dwOwnerType;		// Ƶ��ӵ�����ʺ�����

	SVoiceChannelBaseInfo(void)
	{
		memset( this, 0, sizeof(SVoiceChannelBaseInfo) );
	}
};

// �������������Ϣ
struct SVoiceRoomBaseInfo
{
	BYTE	nMicState;			// ������ EMVoiceMicState
	DWORD	dwStateFlag;		// ����״̬��� EMVoiceRoomStateFlag
	DWORD	dwUserCount;		// �����������ο�ֵ
	DWORD	dwChatCoolDown;		// ����������ȴʱ��
	DWORD	dwMicOrderTime;		// Ĭ������ʱ��

	SVoiceRoomBaseInfo(void)
	{
		memset( this, 0, sizeof(SVoiceRoomBaseInfo) );
	}
};

// ����˽����Ϣ
struct SVoiceChatInfo
{
	bool bMicOpen;		// �Ƿ���
	DWORD dwChatID;		// ˽�Ķ���ID
	DWORD dwUserType;	// EMVoiceChatUserType
	DWORD dwLastAcceptTime;	// �ϴν���˽�������ʱ��

	SVoiceChatInfo(void)
	{
		memset( this, 0, sizeof(SVoiceChatInfo) );
	}
};


///// �ͻ��˹������ݽṹ���� //////////////////////////////////////////////


// ����������
struct SVoiceRoomMicData
{
	DWORD		dwClientID;		/// �ͻ���ID
	DWORD		dwIndex;		/// ����

	SVoiceRoomMicData(void)
	{
		memset( this, 0, sizeof(SVoiceRoomMicData) );
	}
};

// ������˵�����
struct SVoiceRoomVoiceData
{
	DWORD		dwClientID;		/// �ͻ���ID
	DWORD		dwTime;			/// ��ʼ˵��ʱ��

	SVoiceRoomVoiceData(void)
	{
		memset( this, 0, sizeof(SVoiceRoomVoiceData) );
	}
};

// ������˵�������
struct SVoiceRoomVoiceDataResult
{
	DWORD		dwClientID;		/// �ͻ���ID
	bool		bTalking;		/// �Ƿ���˵��

	SVoiceRoomVoiceDataResult(void)
	{
		memset( this, 0, sizeof(SVoiceRoomVoiceDataResult) );
	}
};

// Ƶ������ʱ����
struct SVoiceVestData
{
	DWORD dwUDBID;						// dwUDBID
	BYTE nSex;
	BYTE nNewUserType;					// �û�����	
	char szName[VOICE_NAME_MAXSIZE];	// ����
};

// ��ǰ�������ϵ���
struct SVoiceCurMicUserInfo
{
	DWORD	dwMicClientID;	// ��ǰ�����ϵ���
	DWORD	dwMicEndTime;	// �����ʱ��
	DWORD	dwFansCount;	// ��˿����
	bool	bMicFans;		// �������û��ķ�˿
	DWORD	dwWorldID;		// �����û���������ID

	SVoiceCurMicUserInfo(void)
	{
		memset( this, 0, sizeof(*this) );
	}
};

// ��������
struct SVoiceWorldData
{
	DWORD dwWorldID;		// ����ID
	char szWorldName[VOICE_NAME_MAXSIZE];	// ��������

	SVoiceWorldData(void)
	{
		memset( this, 0, sizeof(*this) );
	}
};

// ����Ƶ�����а�����
struct SVoicePersonalTopData
{
	DWORD dwWorldID;		// ����ID
	DWORD dwCountryID;		// ����ID
	DWORD dwUDBID;			// ͨ��֤ID
	char szOwnerName[VOICE_NAME_MAXSIZE];	// ����
	DWORD dwUserCount;		// ��������
	DWORD dwMaxMemberCount;	// ��ʷ�����������
	BYTE nState;			// ״̬
	DWORD dwOwnerType;		// Ƶ��ӵ�����ʺ�����

	SVoicePersonalTopData(void)
	{
		memset( this, 0, sizeof(*this) );
	}
};


// Ƶ��������������
struct SVoiceChannelSortInfo
{
	LONGLONG nChannelID;	// Ƶ��ID
	DWORD dwUserCount;		// ��������
	DWORD dwUserData;		// �û�����
};

// �յ���˿�ټ�����Ϣ
struct SVoiceCallFansData
{
	DWORD dwTime;				// ����ʱ��
	DWORD dwClientID;			// �ͻ���ID
	DWORD dwChannelType;
	DWORD dwRoomID;
	DWORD dwWorldID;
	DWORD dwID;
	BYTE nSex;					// �Ա�
	BYTE nCountryID;			// ����ID
	DWORD dwFromWorldID;			// ����ID
	DWORD dwUDBID;				// UDBID
	char szNickname[VOICE_NAME_MAXSIZE];		// �ǳ�
	char szMsg[VOICE_TEXTSTR_MAXSIZE];	// �ı���Ϣ
	BYTE nParam1;			// ��������1
	BYTE nParam2;			// ��������2

};

// ���󱣴�������ȴ��Ϣ���ݽṹ
struct SVoiceSaveCoolInfo
{
	int nCounts;						// ��ȴʱ�ı������
	DWORD dwLastTimes;					// ��󱣴�ʱ��
	bool bOnTimer;						// �Ƿ��Ƕ�ʱ����,�ǾͲ���Ϊ�����󣬲������Ŷӵ��������

	SVoiceSaveCoolInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// ������˿���а�����
struct SVoiceFansTopListData
{
	DWORD dwUDBID;									// ͨ��֤UDBID
	DWORD dwUserType;								// ͨ��֤�ʺ�����
	DWORD dwRank;									// ����
	DWORD dwFansCount;								// ��˿����
	DWORD dwPDBID;									// �����¼�Ľ�ɫID
	DWORD dwFromWorldID;							// �����¼�Ľ�ɫ��Դ����ID
	DWORD dwCountryID;								// �����¼�Ľ�ɫ����ID(���ң���½����)
	char szActorName[VOICE_NAME_MAXSIZE];			// �����¼�Ľ�ɫ��ɫ��

	SVoiceFansTopListData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// ������Ϣ
struct SVoiceDiamondInfo
{
	DWORD dwUDBID;				// ͨ��֤UDBID,Ψһ
	DWORD dwBlueDiamond;		// ����������һֱ�ۼ�
	DWORD dwCurBlueDiamond;		// ��ǰ��������ֻ�������ֵ�ſɽ���ɺ���
	DWORD dwBlueDiamondOld;		// ����֮ǰ������
	DWORD dwBlueDiamondToday;	// ���ջ�����������ڶ����Զ��ѽ�������ֵ��ӵ�dwCurBlueCount�У�Ȼ������

	DWORD dwRedDiamond;			// ����������һֱ�ۼӣ��ⲿ�ֿɽ���ɽ��/RMB

	SVoiceDiamondInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


///////////////////////////////////////////////////////////////////
// �¼���ö�ٶ���: ENUM_EVENT_VOICE + ��Ϣ����
enum EMEventVoice
{
	ENUM_EVENT_VOICE_INVALID = 0,
	ENUM_EVENT_VOICE_HANDSHAKE,					// ���������������ֳɹ��¼�
	ENUM_EVENT_VOICE_SERVERLIST_ALL,			// ȫ�������������¼�
	ENUM_EVENT_VOICE_SERVERLIST_ADD,			// �����������¼�
	ENUM_EVENT_VOICE_SERVERLIST_DEL,			// ɾ���������¼�
	ENUM_EVENT_VOICE_WORLDLIST_ALL,				// ȫ����Ϸ��������¼�
	ENUM_EVENT_VOICE_WORLDLIST_ADD,				// ������Ϸ�����¼�
	ENUM_EVENT_VOICE_WORLDLIST_EDIT,			// �޸���Ϸ�����¼�

	// �ͻ���ʹ��
	ENUM_EVENT_VOICE_CLIENT_INITSPEEX,			// �첽����ѹ���� �������߳�
	ENUM_EVENT_VOICE_CLIENT_CONNECTSERVER,		// ������������
	ENUM_EVENT_VOICE_CLIENT_DISCONNECT,			// ��������Ͽ�����
	ENUM_EVENT_VOICE_CLIENT_CONNECTED,			// �����ӷ�����
	ENUM_EVENT_VOICE_CLIENT_RECONNECT,			// �������ӷ�����
	ENUM_EVENT_VOICE_CLIENT_CHATMSG,			// ����

	ENUM_EVENT_VOICE_CLIENT_USERINFOUPDATE,		// �û���Ϣ���£������û���Ƶ��������Ϣ��ӵ��Ȩ��
	ENUM_EVENT_VOICE_CLIENT_ROOMINFOUPDATE,		// ������Ϣ����
	ENUM_EVENT_VOICE_CLIENT_ISSPEAKING,	        // �����û��Ƿ���˵��
	ENUM_EVENT_VOICE_CLIENT_ENTERROOMRESULT,	// ���뷿����
	ENUM_EVENT_VOICE_CLIENT_CHANNELINFOUPDATE,	// Ƶ����Ϣ����
	ENUM_EVENT_VOICE_CLIENT_SELFSPEAKING,		// �Լ�˵���ĸ���

	ENUM_EVENT_VOICE_CLIENT_NEEDNEWIP,			// ��Ҫ��ѡIP
	ENUM_EVENT_VOICE_CLIENT_TEXTCHATMESSAGE,	// �ı�����

	ENUM_EVENT_VOICE_CLIENT_VOICELIST,			// �����б�
	ENUM_EVENT_VOICE_CLIENT_CHATLIST,			// ˽���б�

	ENUM_EVENT_VOICE_CLIENT_ALLCHANNELINFOUPDATE,	// ȫ��Ƶ����Ϣ����
	ENUM_EVENT_VOICE_CLIENT_FIRSTSPEAKING,		// ��һ��Ƶ������˵���¼�
	ENUM_EVENT_VOICE_CLIENT_VESTLIST,			// ��ף���ʱ�����б����
	ENUM_EVENT_VOICE_CLIENT_PERSONALTOPLIST,	// ����Ƶ�����а�
	ENUM_EVENT_VOICE_CHAT,						// ˽��
	ENUM_EVENT_VOICE_CLIENT_CHANNELAUDIO,		// Ƶ����������
	ENUM_EVENT_VOICE_CLIENT_CALLFANS,			// �ټ���˿
	ENUM_EVENT_VOICE_CLIENT_GIFTSENDINFO,		// ������Ϣ
	ENUM_EVENT_VOICE_CLIENT_DIAMONDINFO,		// ������Ϣ
	ENUM_EVENT_VOICE_CLIENT_FANSTOPLIST,		// ��˿���а�
	ENUM_EVENT_VOICE_CLIENT_ADMINENTERINFO,		// ����Ա������Ϣ
	ENUM_EVENT_VOICE_CLIENT_AD,					// �����ټ���
	ENUM_EVENT_VOICE_CLIENT_VIDEOFRAME,			// ��Ƶ����
	ENUM_EVENT_VOICE_CLIENT_VIDEOSHOW,			// ��ʾ��Ƶ�¼�,��������ƵԴ,�ȴ���Ƶ��,��Ƶ�ж�
	ENUM_EVENT_VOICE_CLIENT_VIDEO,				// ��Ƶ�¼�

	ENUM_EVENT_VOICE_MAXID,						// ����¼���
};

// ����¼���
#define EVENT_VOICE_MAXID						ENUM_EVENT_VOICE_MAXID




//////////////////////////////////////////////////////////////////////////

// �첽����ѹ���� �������߳�
enum EMSubEventInitSpeex
{
	EventInitSpeex_Nothing	= 0,	// ����Ϊ
	EventInitSpeex_Init,			// ������ѹ���� ������
	EventInitSpeex_UnInit,			// �ͷŽ�ѹ���� ������
};

// ͨ���¼�ö��
enum EMVoiceAudioEvent
{
	VoiceAudioEvent_NetConnect = 0,		// ��������
	VoiceAudioEvent_NetClose,			// �Ͽ�����
	VoiceAudioEvent_AddVoiceUser,		// ��������û�
	VoiceAudioEvent_AddToBlackList,		// ��ӵ�������
	VoiceAudioEvent_FireProviderEvent,	// �׳����ֲ��¼�

	VoiceAudioEvent_OnNetConnected = 0,		// ��������
	VoiceAudioEvent_OnNetClose,				// �Ͽ�����
};


// �����б����¼�
enum EMSubEventVoiceList
{
	EventVoiceList_Clear,		// �������������Ա�б�
	EventVoiceList_Add,			// �������������Ա
	EventVoiceList_Flash,		// ˢ���б�
};

// ����˽���б�
enum EMSubEventChatList
{
	EventChatList_Clear,		// ���˽���б�
	EventChatList_Add,			// ���˽��
	EventChatList_Flash,		// ˢ���б�
};

// �ټ���˿�¼�
enum EMSubEventCallFans
{
	EventCallFans_Call,				// �յ���˿�ټ�
	EventCallFans_OnlineCount,		// ��˿��������Ӧ
	EventCallFans_Success,			// �ټ���˿�����ɹ�
	EventCallFans_Fail,				// �ټ���˿����ʧ��
	EventCallFans_CallResult,		// �����ټ���˿���
	EventCallFans_Flash,			// ����ˢ�½���
	EventCallFans_UpdateTime,		// �����ϴ������˿ʱ��
};

// ˽�����¼�
enum EMSubEventVoiceChat
{	
	EventVoiceChat_Invite,			// ���յ�˽������
	EventVoiceChat_InviteSuccess,	// ����˽������ɹ�
	EventVoiceChat_RunSuccess,		// ˽�ĳɹ�����
	EventVoiceChat_End,				// ˽�Ľ���
	EventVoiceChat_OpenMic,			// ˽�Ŀ���
};

// ��ʾ��Ƶ�¼�,��������ƵԴ,�ȴ���Ƶ��,��Ƶ�ж� ���¼� dwData1
enum EMSubEventVoiceVideoShow
{	
	EventVoiceVideoShow_NewVideo,		// ���µ���Ƶ	,dwData2:ͨ��Id,dwData3:dwClientID
	EventVoiceVideoShow_EndVideo,		// ��Ƶ�ź��ж�	,dwData2:ͨ��Id,dwData3:dwClientID
	EventVoiceVideoShow_KeyFrame,		// ��Ƶ��һ���յ��ؼ�֡	,dwData2:ͨ��Id,dwData3:dwClientID
};

// ��Ƶ�¼�
enum EMSubEventVoiceVideo
{	
	EventVoiceVideo_Device,			// ��Ƶ�����豸���
	EventVoiceVideo_Camera,			// �ɼ���Ƶ����ͷ����,dwData2: EMVideoCameraControlCode
};




// tolua_end










// ���������������ֳɹ��¼�
#define EVENT_VOICE_HANDSHAKE				ENUM_EVENT_VOICE_HANDSHAKE
// �¼����ݽṹ
struct SEventVoiceHandshake
{
	DWORD	dwServerID;				// ������ID

	SEventVoiceHandshake(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// ȫ�������������¼�
#define EVENT_VOICE_SERVERLIST_ALL				ENUM_EVENT_VOICE_SERVERLIST_ALL
// �¼����ݽṹ
struct SEventVoiceServerListAll
{
	DWORD	dwServerCounts;				// �յ�����������

	SEventVoiceServerListAll(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// �����������¼�
#define EVENT_VOICE_SERVERLIST_ADD				ENUM_EVENT_VOICE_SERVERLIST_ADD
// �¼����ݽṹ
struct SEventVoiceServerListAdd
{
	DWORD	dwID;				// ������ID
	DWORD	dwWorldID;			// ������������Ϸ����ID
	WORD	wServerType;		// ����������
	WORD	wSubID;				// ��n�ŷ�����
	DWORD	dwStateFlag;		// ������״̬

	SEventVoiceServerListAdd(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// ɾ���������¼�
#define EVENT_VOICE_SERVERLIST_DEL				ENUM_EVENT_VOICE_SERVERLIST_DEL
// �¼����ݽṹ
struct SEventVoiceServerListDel
{
	DWORD dwServerID;				// ������ID

	SEventVoiceServerListDel(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
// ȫ����Ϸ��������¼�
#define EVENT_VOICE_WORLDLIST_ALL				ENUM_EVENT_VOICE_WORLDLIST_ALL
// �¼����ݽṹ
struct SEventVoiceWorldListAll
{
	DWORD	dwWorldCounts;				// �յ���Ϸ�������

	SEventVoiceWorldListAll(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// ������Ϸ�����¼�
#define EVENT_VOICE_WORLDLIST_ADD				ENUM_EVENT_VOICE_WORLDLIST_ADD
// �¼����ݽṹ
struct SEventVoiceWorldListAdd
{
	DWORD	dwWorldID;			// ��Ϸ����ID

	SEventVoiceWorldListAdd(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// �޸���Ϸ�����¼�
#define EVENT_VOICE_WORLDLIST_EDIT				ENUM_EVENT_VOICE_WORLDLIST_EDIT
// �¼����ݽṹ
struct SEventVoiceWorldListEdit
{
	DWORD dwWorldID;				// ��Ϸ����ID

	SEventVoiceWorldListEdit(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// �¼�����ͷ
struct SEventVoiceBaseHead
{
	DWORD dwData1;		// ����1
	DWORD dwData2;		// ����2
	DWORD dwData3;		// ����3
};


#define		EVENT_VOICE_CLIENT_CONNECTSERVER	ENUM_EVENT_VOICE_CLIENT_CONNECTSERVER		// ������������
#define		EVENT_VOICE_CLIENT_DISCONNECT		ENUM_EVENT_VOICE_CLIENT_DISCONNECT			// ���ӶϿ�
#define		EVENT_VOICE_CLIENT_CONNECTED		ENUM_EVENT_VOICE_CLIENT_CONNECTED			// ������
#define		EVENT_VOICE_CLIENT_RECONNECT		ENUM_EVENT_VOICE_CLIENT_RECONNECT			// ��������

#define		EVENT_VOICE_CLIENT_CHATMSG			ENUM_EVENT_VOICE_CLIENT_CHATMSG				// ����  ����������������������Ϣ��ֱ��ת������
// SVoiceSubMsgSendChatMsg_VC

#define		EVENT_VOICE_CLIENT_USERINFOUPDATE	ENUM_EVENT_VOICE_CLIENT_USERINFOUPDATE		// �û���Ϣ���£������û���Ƶ��������Ϣ��ӵ��Ȩ��
#define		EVENT_VOICE_CLIENT_ROOMINFOUPDATE	ENUM_EVENT_VOICE_CLIENT_ROOMINFOUPDATE		// ������Ϣ����
#define		EVENT_VOICE_CLIENT_ISSPEAKING       ENUM_EVENT_VOICE_CLIENT_ISSPEAKING		    // �����û��Ƿ���˵��
#define		EVENT_VOICE_CLIENT_ENTERROOMRESULT	ENUM_EVENT_VOICE_CLIENT_ENTERROOMRESULT		// ���뷿����
#define		EVENT_VOICE_CLIENT_CHANNELINFOUPDATE	ENUM_EVENT_VOICE_CLIENT_CHANNELINFOUPDATE	// Ƶ����Ϣ����
#define		EVENT_VOICE_CLIENT_ALLCHANNELINFOUPDATE	ENUM_EVENT_VOICE_CLIENT_ALLCHANNELINFOUPDATE	// ȫ��Ƶ����Ϣ����
#define		EVENT_VOICE_CLIENT_SELFSPEAKING	ENUM_EVENT_VOICE_CLIENT_SELFSPEAKING			// Ƶ����Ϣ����

#define		EVENT_VOICE_CLIENT_NEEDNEWIP		ENUM_EVENT_VOICE_CLIENT_NEEDNEWIP			// ��Ҫ��ѡIP
#define		EVENT_VOICE_CLIENT_TEXTCHATMESSAGE		ENUM_EVENT_VOICE_CLIENT_TEXTCHATMESSAGE	// �ı�����

#define		EVENT_VOICE_CLIENT_VOICELIST			ENUM_EVENT_VOICE_CLIENT_VOICELIST		// ����������Ա�б�
#define		EVENT_VOICE_CLIENT_CHATLIST				ENUM_EVENT_VOICE_CLIENT_CHATLIST		// ˽���б�

#define		EVENT_VOICE_CLIENT_FIRSTSPEAKING		ENUM_EVENT_VOICE_CLIENT_FIRSTSPEAKING	// �����һ������˵�����¼�
#define		EVENT_VOICE_CLIENT_VESTLIST				ENUM_EVENT_VOICE_CLIENT_VESTLIST		// ��ף���ʱ�����б����
#define		EVENT_VOICE_CLIENT_PERSONALTOPLIST		ENUM_EVENT_VOICE_CLIENT_PERSONALTOPLIST	// ����Ƶ�����а�
#define		EVENT_VOICE_CHAT						ENUM_EVENT_VOICE_CHAT					// ˽��
#define		EVENT_VOICE_CLIENT_CHANNELAUDIO			ENUM_EVENT_VOICE_CLIENT_CHANNELAUDIO	// Ƶ����������
#define		EVENT_VOICE_CLIENT_CALLFANS				ENUM_EVENT_VOICE_CLIENT_CALLFANS		// �ټ���˿
#define		EVENT_VOICE_CLIENT_GIFTSENDINFO			ENUM_EVENT_VOICE_CLIENT_GIFTSENDINFO	// ������Ϣ
#define		EVENT_VOICE_CLIENT_DIAMONDINFO			ENUM_EVENT_VOICE_CLIENT_DIAMONDINFO		// ������Ϣ
#define		EVENT_VOICE_CLIENT_FANSTOPLIST			ENUM_EVENT_VOICE_CLIENT_FANSTOPLIST		// ��˿���а�
#define		EVENT_VOICE_CLIENT_ADMINENTERINFO		ENUM_EVENT_VOICE_CLIENT_ADMINENTERINFO	// ����Ա������Ϣ
#define		EVENT_VOICE_CLIENT_AD					ENUM_EVENT_VOICE_CLIENT_AD				// �����ټ���
#define		EVENT_VOICE_CLIENT_VIDEOFRAME			ENUM_EVENT_VOICE_CLIENT_VIDEOFRAME		// ��Ƶ����
#define		EVENT_VOICE_CLIENT_VIDEOSHOW			ENUM_EVENT_VOICE_CLIENT_VIDEOSHOW		// ��ʾ��Ƶ�¼�,��������ƵԴ,�ȴ���Ƶ��,��Ƶ�ж�
#define		EVENT_VOICE_CLIENT_INITSPEEX			ENUM_EVENT_VOICE_CLIENT_INITSPEEX		// �첽����ѹ���� �������߳�
#define		EVENT_VOICE_CLIENT_VIDEO				ENUM_EVENT_VOICE_CLIENT_VIDEO			// ��Ƶ�¼�








//////////////////////////////////////////////////////////////////////////
// ��Ϣ�������ݽṹ

/***************************************************************/
///////////////////// ����������ģ�� ����Ϣ��/////////////////
// �ϲ����Ϣ��ض��� MSG_MODULEID_VOICE
/***************************************************************/

///////////////////////////////////////////////////////////////////
// ��Ϣ��ö�ٶ���: ENUM + ��Ϣ����
enum EMMsgVoice
{
	ENUM_MSG_VOICE_INVALID = 0,

	// ������Ϣ
	ENUM_MSG_VOICE_HANDSHAKE,						// ������Ϣ
	ENUM_MSG_VOICE_HANDSHAKE_RESPONSE,				// ��Ӧ������Ϣ
	ENUM_MSG_VOICE_KEEPALIVE,						// �������
	ENUM_MSG_VOICE_ANSWER_KEEPALIVE,				// ��Ӧ�������
	ENUM_MSG_VOICE_SENDDATA,						// �����������������ط��������ݸ������������
	ENUM_MSG_VOICE_SERVERLIST_UPDATE,				// �������б����
	ENUM_MSG_VOICE_DATA_SUBMSG,						// ��������Ϣ����
	ENUM_MSG_VOICE_SENDCHATMSG,						// ������Ϣ
	ENUM_MSG_VOICE_PERFORMANCE,						// ���ܼ����Ϣ��

	ENUM_MSG_VOICE_CHECK,							// �����Ϣ��������������������״̬
	ENUM_MSG_VOICE_CHECKBACK,						// �����Ϣ�ظ���������������������״̬

	// ����������Ϣ

	// ����������
	ENUM_MSG_VOICE_USERPARTINFO_UPDATE,				// ���������û�������Ϣ

	ENUM_MSG_VOICE_ALL_WORLD_BROADCAST,				// ���������������㲥��Ϣ��������
	ENUM_MSG_VOICE_MAXID,							// �����Ϣ��
};

// �����Ϣ��
#define MSG_VOICE_MAXID						ENUM_MSG_VOICE_MAXID



//////////////////////////////////////////////////////////////////////////
// ������Ϣ
#define MSG_VOICE_HANDSHAKE				ENUM_MSG_VOICE_HANDSHAKE
// ����������������������������������Ϣ���ݽṹ
struct SMsgVoiceHandshake_SB
{
	DWORD	dwWorldID;						// ������������Ϸ����ID
	WORD	nServerType;					// ����������
	WORD	wSubID;							// ��n�ŷ�����
	bool	bIsPublic;						// �Ƿ��ǹ�������
	char	szName[VOICE_NAME_MAXSIZE];	// ��������
	char	szWorldName[VOICE_NAME_MAXSIZE];	// ��Ϸ������

	SMsgVoiceHandshake_SB(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// �ͻ�����������������
struct SMsgVoiceHandshake_CG
{
	char	szNickname[VOICE_NAME_MAXSIZE];		// �ǳ�
	DWORD	dwPDBID;							// PDBID
	char	szMacAddress[VOICE_NAME_MAXSIZE];	// ������ַ

	SMsgVoiceHandshake_CG(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// ��������ͨ������������
struct SMsgVoiceDataHandShake_CG
{
	DWORD dwClientID;		// �ͻ���ID
	BYTE	nKeyLen;
	//char szKeyData[nKeyLen]				// ��̬����

	SMsgVoiceDataHandShake_CG(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// ������������������������
struct SMsgVoiceHandshake_GV
{
	DWORD	dwGatewayID;			// ��������
	DWORD	dwMaxCounts;			// �������

	SMsgVoiceHandshake_GV(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// ��Ӧ������Ϣ
#define MSG_VOICE_HANDSHAKE_RESPONSE		ENUM_MSG_VOICE_HANDSHAKE_RESPONSE

// ������������Ӧ������Ϣ���ݽṹ
struct SMsgVoiceHandshakeResponse	
{
	DWORD	dwID;				// ����ID
	DWORD	dwServerTime;		// ������ʱ��

	SMsgVoiceHandshakeResponse(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// �������ػ�Ӧ������Ϣ���ݽṹ
struct SMsgVoiceHandshakeResponse_GC	
{
	DWORD	dwID;				// ����ID
	DWORD	dwServerTime;		// ������ʱ��
	DWORD	dwAudioPort;		// ����ͨ���˿�
	BYTE	nKeyLen;
	//char szKeyData[nKeyLen]	// ��̬����

	SMsgVoiceHandshakeResponse_GC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// ������������Ӧ��������������Ϣ���ݽṹ
struct SMsgVoiceHandshakeResponse_VG	
{
	DWORD	dwID;				// ����ID
	DWORD	dwServerTime;		// ������ʱ��

	SMsgVoiceHandshakeResponse_VG(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// ������������Ӧ�����ͻ���������Ϣ���ݽṹ
struct SMsgVoiceHandshakeResponse_VC	
{
	DWORD	dwClientID;				// �ͻ���ID
	DWORD	dwServerTime;			// ������ʱ��
	BYTE	dwVoiceQuality;			// Ƶ��Ĭ������Ʒ������ EMVoiceQualityType
	SVoiceCodecInfo	codecInfo;		// ѹ������
	BYTE	nKeyLen;
	//char szKeyData[nKeyLen]					// ��̬����

	SMsgVoiceHandshakeResponse_VC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
// �������
#define MSG_VOICE_KEEPALIVE				ENUM_MSG_VOICE_KEEPALIVE


//////////////////////////////////////////////////////////////////////////
// ��Ӧ�������
#define MSG_VOICE_ANSWER_KEEPALIVE			ENUM_MSG_VOICE_ANSWER_KEEPALIVE

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// �����������������ط��������ݸ��������������Ϣ
#define MSG_VOICE_SENDDATA					ENUM_MSG_VOICE_SENDDATA

// �����������������ط��������ݸ���������������ݽṹ
struct SMsgVoiceSendData
{
	DWORD	nSendChennel:8;		// ��������ͨ������ EMVoiceSendChannelType
	DWORD	dwServerID:24;		// ���������ID
	DWORD	nDataLens;			// ���ݳ���
	//BYTE	pData[nDataLens];	// ����
};


//////////////////////////////////////////////////////////////////////////
// ������������Ϣ����Ϣ
#define MSG_VOICE_DATA_SUBMSG					ENUM_MSG_VOICE_DATA_SUBMSG

// ������������Ϣ����Ϣ���ݽṹ
struct SMsgVoiceDataSubMsg
{
	DWORD dwMsgCode;						// ����Ϣ����  EMVoiceDataSubMsgCode
	DWORD dwUserData1;						// �û�����1
	DWORD dwUserData2;						// �û�����2
	DWORD dwUserData3;						// �û�����3
	DWORD dwUserData4;
	DWORD dwUserData5;
	// ��������

	SMsgVoiceDataSubMsg(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// �������ܼ����Ϣ��
#define	 MSG_VOICE_PERFORMANCE			ENUM_MSG_VOICE_PERFORMANCE

// ��������������ͨ�˶���
enum EMVoicePerformanceType
{
	VoicePerformanceType_RecordAudio = 0,	// �ͻ��� ¼������
	VoicePerformanceType_EncodeFrame,		// �ͻ��� ѹ������
	VoicePerformanceType_DecodeFrame,		// �ͻ��� ��������
	VoicePerformanceType_PlayAudio,			// �ͻ��� ��������
	VoicePerformanceType_ClientPack,		// �ͻ��� �������
	VoicePerformanceType_Client,			// �ͻ���
	VoicePerformanceType_Gateway,			// ��������
	VoicePerformanceType_Server,			// ������
	VoicePerformanceType_Max				// ���
};

// ��������
struct SMsgVoicePerformanceNode
{
	LONGLONG nSendTicks;		// ����ʱ��
	LONGLONG nRecvTicks;		// �յ�ʱ��
	LONGLONG nFrequency;		// CPUƵ��
	WORD	nRecvCounts;		// �������հ�����
	WORD	nSendCounts;		// ������������

	SMsgVoicePerformanceNode(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// ���ܼ����Ϣ��
struct SMsgVoicePerformanceData
{
	SMsgVoicePerformanceNode data[VoicePerformanceType_Max];		// �������� EMVoicePerformanceType
};

// ����->���أ����ܼ����Ϣ��
struct SMsgVoicePerformance_S
{
	DWORD  dwClientID;     // �ͻ���ID
	SMsgVoicePerformanceData perData;	// ���ܼ����Ϣ��
};

#pragma pack()