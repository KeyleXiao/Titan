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

#include "Voice_ManagedDef.h"
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


#define  VOICE_MAX_GATEWAYID					64				// ��Ϊ����ClientID��ֻ����8λ��ʾ����ID,��������ID���ֻ��255λ
#define  VOICE_MAX_CLIENT_COUNT					8192			// һ̨���ط����������ص����� ��� 8192 
#define  VOICE_MAX_ROOM_USERCOUNT				100000			// �����������

#define  VOICE_RC4_KEY_SIZE						64				// RC4��Կ����
#define  VOICE_RC4_MAX_SIZE						64				// RC4��Կ��󳤶�

#define  VOICE_SERVICE_UPDATE_INFO_INTERVAL		1200000			// ����ȫ����Ϸ�û�������Ϣ��������������20����ͬ��һ��
#define  VOICE_CHANNEL_OBSERVER_TIMEOUT			120000			// Ƶ���۲��߳�ʱʱ��120��
#define	 VOICE_ROOM_MIN_BUFF_SIZE				16				// �����û��б���С�����ڴ����
#define	 VOICE_ROOM_MAX_MICORDER				16				// �����������
#define  VOICE_ACTIVE_CHANNEL_TIME				30000			// ��ԾƵ��ʱ��
#define  VOICE_CALL_FANS_COOL_TIME				3600			// �ټ���˿��ȴʱ�䣨�룩 һ��Сʱ
#define  VOICE_AD_COOL_TIME						3600			// �����ټ�����ȴʱ�䣨�룩 һ��Сʱ

#define  VOICE_COOL_SAVE_TIMES					600000			// ���ݱ�����ȴʱ��,����(Ĭ��10����)
#define  VOICE_COOL_SAVE_COUNTS					100				// ���󱣴�������ȴ����������,���ڴ����ͻᱣ��

#define  VOICE_SAVE_DB_INTERVAL					600000			// ���ݱ�����ʱ��,����(Ĭ��10����)

#define  VOICE_MAX_DIAMOND_COUNT				500000			// ���������

#define  VOICE_CLIENT_AUDIONET_MAX				40				// �ͻ���������Ƶ��ר������ͨ����󻺴����,������������������ķ�������(Ĭ��40,���20����������)
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
	VoiceCommand_ExportUserList = 1,	// �����û��б�
	VoiceCommand_ChangeMaxConnection,	// �޸����������
	VoiceCommand_ExportChannelList,		// ����Ƶ���б�
	VoiceCommand_NetSpeedMax,			// ��������������(Mbps)

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
	ENUM_MSG_VOICE_BROADCASTDATA,					// �����������������ط���㲥���ݸ������������
	ENUM_MSG_VOICE_BROADCASTDATATOCLIENT,			// ���������������㲥���ݸ��ͻ���
	ENUM_MSG_VOICE_WORLDLIST_UPDATE,				// ��Ϸ�����б����
	ENUM_MSG_VOICE_SERVERLIST_UPDATE,				// �������б����
	ENUM_MSG_VOICE_DATA_SUBMSG,						// ��������Ϣ����
	ENUM_MSG_VOICE_VOICEDATA,						// ��������
	ENUM_MSG_VOICE_SENDCHATMSG,						// ������Ϣ
	ENUM_MSG_VOICE_PERFORMANCE,						// ���ܼ����Ϣ��
	ENUM_MSG_VOICE_VOICELIST,						// ˵���б�
	ENUM_MSG_VOICE_CHATLIST,						// ˽���б�
	ENUM_MSG_VOICE_WORLDLIST,						// �����б�
	ENUM_MSG_VOICE_AD,								// Ƶ������
	ENUM_MSG_VOICE_VESTLIST,						// ��ף���ʱ�����б�
	ENUM_MSG_VOICE_PERSONALTOPLIST,					// �������а�
	ENUM_MSG_VOICE_CHATINVITE,						// ˽������
	ENUM_MSG_VOICE_ADDVOICEBLUEDIAMOND,				// �������

	ENUM_MSG_VOICE_BRIDGEDATA,						// ������Ϣ���������������ɿ���
	ENUM_MSG_VOICE_CHECK,							// �����Ϣ��������������������״̬
	ENUM_MSG_VOICE_CHECKBACK,						// �����Ϣ�ظ���������������������״̬

	// ����������Ϣ
	ENUM_MSG_VOICE_LOGIN,							// �����¼����������
	ENUM_MSG_VOICE_CLIENTLIST_UPDATE,				// ��������ͬ���ͻ�����Ϣ������������

	// ����������
	ENUM_MSG_VOICE_USERINFO_UPDATE,					// ���������û���Ϣ
	ENUM_MSG_VOICE_CHANNELUSERLIST_UPDATE,			// ����Ƶ����Ա�б�
	ENUM_MSG_VOICE_ROOMINFO_UPDATE,					// ���·�����Ϣ
	ENUM_MSG_VOICE_USERPARTINFO_UPDATE,				// ���������û�������Ϣ
	ENUM_MSG_VOICE_CHANNELINFO_UPDATE,				// ����Ƶ����
	ENUM_MSG_VOICE_TEXTCHATMESSAGE,					// ����������Ϣ
	ENUM_MSG_VOICE_ALLCHANNELINFO_UPDATE,			// ��������Ƶ����Ϣ��ͨ�õĻ�����Ϣ
	ENUM_MSG_VOICE_CHATUSERINFO_UPDATE,				// ˽���û���Ϣ����
	ENUM_MSG_VOICE_CALLFANS,						// �ټ���˿
	ENUM_MSG_VOICE_GIFTSENDINFO,					// ������Ϣ
	ENUM_MSG_VOICE_DIAMONDINFO,						// ���������Ϣ
	ENUM_MSG_VOICE_FANSTOPLIST,						// ��˿���а�
	ENUM_MSG_VOICE_ADMINENTERINFO,					// ����Ա������Ϣ

	// ���󷿼�����
	ENUM_MSG_VOICE_REQ_ROOM_ACTOR_LIST,				// ���󷿼���Ա�б�
	ENUM_MSG_VOICE_SEND_ROOM_ACTOR_LIST,			// ���󷿼���Ա�б���
	ENUM_MSG_VOICE_UB_SET_TEMP_RIGHT,               // ͳս��ʼ������ʱȨ��
	ENUM_MSG_VOICE_UBINFO_UPDATE,					// �������������������ͳս��Ϣ

	ENUM_MSG_VOICE_SOCAIL_NOTIFY_USER_LOGOUT,		// ����֪ͨ�������
	
	
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
// �����������������ط���㲥���ݸ������������
#define MSG_VOICE_BROADCASTDATA					ENUM_MSG_VOICE_BROADCASTDATA

// �����������������ط���㲥���ݸ���������������ݽṹ
struct SMsgVoiceBroadcastData
{
	BYTE	nSendChennel;		// ��������ͨ������ EMVoiceSendChannelType
	DWORD	dwServerCounts;		// ���������ID�б����
	DWORD	nDataLens;			// ���ݳ���
	//DWORD	dwServerID[dwServerCounts];			// ���������ID�б�
	//BYTE	pData[nDataLens];					// ����
};


//////////////////////////////////////////////////////////////////////////
// ���������������㲥���ݸ��ͻ���
#define MSG_VOICE_BROADCASTDATATOCLIENT ENUM_MSG_VOICE_BROADCASTDATATOCLIENT
struct SMsgVoiceBroadcastDataToClient
{
	BYTE nType;				// �㲥����		EMVoiceChannelType
	DWORD dwPDBID1;			// ��ɫID1
	DWORD dwPDBID2;			// ��ɫID2

	WORD nWorldID1;			// ����ID1
	WORD nWorldID2;			// ����ID2

	DWORD dwValueID1;		// IDֵ1
	DWORD dwValueID2;		// IDֵ2

	bool bRoom;				// �Ƿ�㲥�û�dwPDBID1���ڷ���

	DWORD dwDataLen;		// ���ݳ���
	// char pData[nDataLen];
};

//////////////////////////////////////////////////////////////////////////
// ��Ϸ�����б����
#define MSG_VOICE_WORLDLIST_UPDATE					ENUM_MSG_VOICE_WORLDLIST_UPDATE

// ��Ϣ�� ��Ϸ�������ݽṹ
struct SMsgVoiceGameWorldNode
{
	DWORD	dwWorldID;			// ��Ϸ����ID
	BYTE	nNameStrlens;		// ��Ϸ���������ִ�����

	SMsgVoiceGameWorldNode(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// ��Ϸ�����б�������ݽṹ
struct SMsgVoiceWorldListUpdate
{
	BYTE	nUpdateType;		// ��������,EMVoiceServerListUpdateType
	DWORD	dwWorldCounts;		// ��Ϸ�������
	//dwWorldCounts* SMsgVoiceGameWorldNode;	// �����������Ϣ�б�
	// dwWorldCounts* char szWorldName[nNameStrlens];	// ��Ϸ���������б�

	SMsgVoiceWorldListUpdate(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// �������б����
#define MSG_VOICE_SERVERLIST_UPDATE					ENUM_MSG_VOICE_SERVERLIST_UPDATE

// ��Ϣ�� ��Ϸ��������Ϣ���ݽṹ
struct SMsgVoiceGameServerNode
{
	DWORD	dwID;				// ������ID
	DWORD	dwWorldID:16;		// ������������Ϸ����ID
	WORD	wServerType:8;		// ����������
	WORD	wSubID:8;			// ��n�ŷ�����
	DWORD	dwStateFlag:8;		// ������״̬

	SMsgVoiceGameServerNode(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// �������б�������ݽṹ
struct SMsgVoiceServerListUpdate
{
	BYTE	nUpdateType;		// ��������,EMVoiceServerListUpdateType
	DWORD	dwServerCounts;		// �������������
	//dwServerCounts* SMsgVoiceGameServerNode;	// �����������Ϣ�б�

	SMsgVoiceServerListUpdate(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// ��������ͬ���ͻ�����Ϣ������������
#define		MSG_VOICE_CLIENTLIST_UPDATE		ENUM_MSG_VOICE_CLIENTLIST_UPDATE
// ��Ϣ�� �ͻ�����Ϣ���ݽṹ
struct SMsgVoiceGatewayClientNode
{
	DWORD	dwClientID;			// �ͻ���ID
	DWORD	dwRemoteIP;			// �ͻ���IP
	//DWORD	dwPDBID;			// PDBID
};

// �ͻ����б�������ݽṹ
struct SMsgVoiceGatewayClientListUpdate
{
	BYTE	nUpdateType;		// ��������
	DWORD	dwCounts;		// ��Ϸ�������
	// dwCounts * SMsgVoiceGatewayClientNode;	// �ͻ�����Ϣ�б�
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

// ������������Ϣ��Ϣ����Ϣ���붨��
enum EMVoiceDataSubMsgCode
{
	////////////////////	����������   //////////////////////////////////////////////////////
	VoiceSubMsg_SendChatMsg = 0,					// ������Ϣ
	VoiceSubMsg_LoginResponse,						// ��¼��Ӧ���յ�����Ϣ�ͱ�ʾ��¼�Ѿ��ɹ�
	VoiceSubMsg_KickUser,							// �߳��û�
	VoiceSubMsg_AddVoiceTime,						// ��������ʱ��
	VoiceSubMsg_VoiceCheck,							// �����Ϣ��������������������״̬
	VoiceSubMsg_VoiceCheckBack,						// �����Ϣ�ظ���������������������״̬
	VoiceSubMsg_UpdateNumValue,						// ������ֵ����Ϸ������

	////////////////////	�����߼��ͻ���   //////////////////////////////////////////////////////
	VoiceSubMsg_RequestLogin,					// ��������������¼
	VoiceSubMsg_OpenMic,						// ��Ͳ����	0 �ر� 1 ��
	VoiceSubMsg_OpenHeadset,					// ��Ͳ����	0 �ر� 1 ��
	VoiceSubMsg_RequestVisitor,					// ����۲�
	VoiceSubMsg_EnterRoom,						// ���뷿��
	VoiceSubMsg_EnterRoomResult,				// ���뷿����
	VoiceSubMsg_ChangeMicState,					// ���÷�������ģʽ
	VoiceSubMsg_SetRoomStateFlag,				// ���÷���״̬���
	VoiceSubMsg_SetRoomLock,					// ���÷�����
	VoiceSubMsg_AddMicOrder,					// ��/����
	VoiceSubMsg_AddMicOrderTime,				// ����/����������˵��ʱ��
	VoiceSubMsg_SetMicTime,						// ��������ʱ��
	VoiceSubMsg_AddClientToMicOrder,			// ��һ���û���ӵ����򣬻���������Ƴ�
	VoiceSubMsg_MoveMicOrder,					// �ƶ�����
	VoiceSubMsg_Move,							// ��Ա����
	VoiceSubMsg_Broadcast,						// Ƶ���㲥
	VoiceSubMsg_AD,								// Ƶ������
	VoiceSubMsg_Ban,							// ��PDBID ����
	VoiceSubMsg_Kick,							// ����
	VoiceSubMsg_SetRemark,						// ����Ƶ����ע
	VoiceSubMsg_SetTitleName,					// ���óƺ�
	VoiceSubMsg_GMEnterRoom,					// GM����ָ������
	VoiceSubMsg_GMEnterRoomByPDBID,				// GM����������ڷ���
	VoiceSubMsg_ClearAllMicOrder,				// �������
	VoiceSubMsg_OpenUserList,					// �����û��б�
	VoiceSubMsg_OpenChannel,					// ����Ƶ��
	VoiceSubMsg_Chat,							// ��������
	VoiceSubMsg_VoiceList,						// ���������б�
	VoiceSubMsg_ChatList,						// ˽���б�
	VoiceSubMsg_GetLastAudioInfo,				// ��ȡ����Ƶ���Ƿ�����������Ϣ
	VoiceSubMsg_GetVoiceTimeLen,				// ��ȡ��������ʱ��
	VoiceSubMsg_EnterRoomByPDBID,				// ���������ڵ�Ƶ��
	VoiceSubMsg_AddVest,						// �����ף���ʱ����
	VoiceSubMsg_GetVestList,					// ��ȡ��ף���ʱ�����б�
	VoiceSubMsg_SetPersonalChannelName,			// �޸ĸ���Ƶ������
	VoiceSubMsg_SetPersonalChannelPublic,		// ��������Ƶ�����μӸ������а�
	VoiceSubMsg_SendPersonalConfig,				// ���͸���Ƶ������
	VoiceSubMsg_RequestPersonalTopList,			// ��ȡ����Ƶ�����а�
	VoiceSubMsg_ChatInvite,						// ˽������
	VoiceSubMsg_ChatInviteResponse,				// ˽�������Ӧ
	VoiceSubMsg_ChatInviteSuccess,				// ����˽������ɹ�
	VoiceSubMsg_ChatRunSuccess,					// ˽�ĳɹ�����
	VoiceSubMsg_ChatClose,						// ˽�Ĺر�
	VoiceSubMsg_ChatEnd,						// ˽�Ľ���
	VoiceSubMsg_OpenChatMic,					// ˽�Ŀ���
	VoiceSubMsg_SetChatConfig,					// ˽������
	VoiceSubMsg_GMEnterChat,					// GM����˽��
	VoiceSubMsg_ShareMic,						// ����
	VoiceSubMsg_ChannelAudio,					// Ƶ��������
	VoiceSubMsg_MicFans,						// �������û��ķ�˿
	VoiceSubMsg_AddAtten,						// ���/ȡ����ע
	VoiceSubMsg_CallFans,						// �ټ���˿
	VoiceSubMsg_RequestFansOnlineCount,			// �������߷�˿����
	VoiceSubMsg_AcceptFansCall,					// ���ܷ�˿�ټ�
	VoiceSubMsg_RequestFansCallResult,			// �����ټ���˿���
	VoiceSubMsg_SetRoomName,					// ���÷�������
	VoiceSubMsg_SetChannelMaxCount,				// ����Ƶ����������
	VoiceSubMsg_SetRoomChatCoolDown,			// ���÷�������������ȴʱ��
	VoiceSubMsg_CloseRoomChat,					// �رշ�����������
	VoiceSubMsg_SetChannelQuality,				// ����Ƶ������Ʒ��
	VoiceSubMsg_SendGift,						// ��������
	VoiceSubMsg_AcceptGift,						// ��������
	VoiceSubMsg_AddBlueDiamond,					// �������
	VoiceSubMsg_GiftSendInfo,					// ������Ϣ
	VoiceSubMsg_RequestDiamondInfo,				// �����������Ϣ
	VoiceSubMsg_RequestFansTopList,				// ��ȡ��˿���а�
	VoiceSubMsg_OpenVideo,						// ��Ƶ����	0 �ر� 1 ��

	// ���
	VoiceSubMsg_Max
};

// VoiceSubMsg_RequestLogin

// ������Ϣ VoiceSubMsg_SendChatMsg
struct SVoiceSubMsgSendChatMsg_VC
{
	bool bTrace;		// �Ƿ�trace��LOG
	WORD nTipsType;		// ��ʾ��Ϣ����
	WORD nPos;			// ϵͳ��Ϣ��ʾλ��
	WORD dwTextLen;
	//char text[dwTextLen]
};

// VoiceSubMsg_EnterRoom,						// ���뷿��
struct SVoiceSubMsgEnterRoom_CV
{
	LONGLONG nRoomID;						// ����ID						
	char szPassword[VOICE_NAME_MAXSIZE];	// ���� ����Ǽ������䣬����

	SVoiceSubMsgEnterRoom_CV(void)
	{
		memset( this, 0, sizeof(*this) );
	}
};


// VoiceSubMsg_SendGift				// ��������
struct SVoiceSubMsgSendGift_CS
{
	DWORD dwWorldID;	// ����������ID
	DWORD dwPDBID;		// ������
	DWORD dwGiftID;		// ����ID
	DWORD dwFlag;		// λ��ʶ	EMVoiceSendGiftFlag
	DWORD dwData1;		// �����Զ���1
	DWORD dwData2;		// �����Զ���2
	DWORD dwData3;		// �����Զ���3
	WORD nMessageLen;	// ���Գ���
	// char szMessage[nMessageLen];
};


//////////////////////////////////////////////////////////////////////////
// �����Ϣ��������������������״̬
// VoiceSubMsg_VoiceCheck,							
// VoiceSubMsg_VoiceCheckBack
// CHECKִ����Ϣ��
enum EMVoiceCheckActionID
{
	VoiceCheckActionID_SendGift = 0,	// ����

	VoiceCheckActionID_Max,
};

struct SVoiceSubMsgCheck
{
	BYTE nSrcPoint;		// ��Դ��
	WORD wSrcWorldID;	// ��Դ����ID
	WORD wDestWorldID;	// Ŀ������ID
	WORD wActionId;		// ִ����Ϣ��		EMVoiceCheckActionID
	DWORD dwPDBIDSend;	// ���ͷ�PDBID
	DWORD dwPDBID;		// ���շ�PDBID
	DWORD dwSpare1;		// ����1
	DWORD dwSpare2;		// ����2
	DWORD dwSpare3;		// ����3
	DWORD dwDataLen;	// �������ݳ���
};

// VoiceSubMsg_AcceptGift ��������
struct SVoiceSubMsgAcceptGift_ZZ
{
	DWORD dwPDBIDSend;	// ������
	WORD nWorldID;		// ����������ID
	BYTE nCountry;		// �����˹���ID
	DWORD dwPDBID;		// ������
	WORD nGiftID;		// ����ID
	DWORD dwFlag;		// λ��ʶ	EMVoiceSendGiftFlag
	DWORD dwData1;		// �����Զ���1
	DWORD dwData2;		// �����Զ���2
	DWORD dwData3;		// �����Զ���3
	BYTE nNameLen;		// ���ֳ���
	WORD nMessageLen;	// ���Գ���
	// char szName[nNameLen];
	// char szMessage[nMessageLen];
};

// VoiceSubMsg_AddBlueDiamond
struct SVoiceSubMsgAddBlueDiamond_ZV
{
	DWORD dwUDBID;			// ���ӵ�ͨ��֤
	DWORD dwPDBID;			// �õ������ͨ��֤�¶�Ӧ�Ľ�ɫID��������¼��־��
	DWORD dwCount;			// ���ӵ���������һ����󲻳���500000�������ĵ�����Ч������
	DWORD dwPDBIDSend;		// �����������PDBID��������¼��־��
	DWORD dwSpare1;			// ����1����Ž��յ�����ID
	DWORD dwSpare2;			// ����2
	DWORD dwSpare3;			// ����2
	WORD nReasonLen;		// ��ע����
	// char szReason[nReasonLen];
};

// VoiceSubMsg_UpdateNumValue
enum EMVoiceUpdateNumValueID
{
	VoiceUpdateNumValueID_UserType = 0,		// ��ݱ��
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

//////////////////////////////////////////////////////////////////////////


// �����¼����������
#define		MSG_VOICE_LOGIN			ENUM_MSG_VOICE_LOGIN

struct SVoiceChannel
{
    LONGLONG channleID;
    BYTE nUserType;

    SVoiceChannel()
    {
        Clear();
    }

    void Clear()
    {
        memset(this, 0, sizeof(*this));
    }
};

// �����û���Ϣ
struct SVoiceUserInfo
{
    DWORD dwFromWorldID;
    DWORD dwClientID;		// �ͻ���ID
    DWORD dwPDBID;			// PDBID
    DWORD dwUDBID;			// �ʺ�ID
    char  szName[VOICE_NAME_MAXSIZE];   
    int   nSex;             // �Ա�
    int   nRankLevel;      // ��λ
    char  szKinName[32];   // ��������
    SVoiceChannel channelList[VoiceChannel_Max];  // �Լ�Ĭ��Ƶ���б�
    SVoiceChannel exChannel;  // ��չƵ�� ����������(����)�ĸ���Ƶ��

    // ���ݿ�����
    DWORD		dwDBUserType;	// DB�û�����
    DWORD		dwFansCount;	// ���з�˿��
    DWORD       dwYesterdayFansCount; // ���շ�˿��
    
    // ��ǰ��һЩ״̬
    BYTE		nVestUserType;        // ��ǰƵ����׵��û����ͣ�ΪƵ��ӵ������Ȩ�Ĺ���Ա����
    DWORD       dwStateFlag;          // ״̬���
    int         nMicIndex;            // �������� 0��ʾ������

    SVoiceUserInfo()
    {
        Clear();
    }

    void Clear()
    {
        memset(this, 0, sizeof(*this));
    }
};

// �����û���Ϣ�ֶζ���
enum SVoiceUserInfoFieldID
{
    VOICE_FIELD_FROMWORLDID,
    VOICE_FIELD_CLIENTID,
    VOICE_FIELD_PDBID,
    VOICE_FIELD_UDBID,
    VOICE_FIELD_NAME,
    VOICE_FIELD_SEX,
    VOICE_FIELD_RANK_LEVEL,
    VOICE_FIELD_KINNAME,
    
    // Ƶ���б� �����ӦEMVoiceChannelType
    VOICE_FIELD_CHANNEL_None,
    VOICE_FIELD_CHANNEL_Gm,
    VOICE_FIELD_CHANNEL_World,
    VOICE_FIELD_CHANNEL_Country,
    VOICE_FIELD_CHANNEL_Clan,
    VOICE_FIELD_CHANNEL_Kin,
    VOICE_FIELD_CHANNEL_Group,
    VOICE_FIELD_CHANNEL_Team,
    VOICE_FIELD_CHANNEL_War,
    VOICE_FIELD_CHANNEL_Personal,

    VOICE_FIELD_EX_CHANNEL,

    VOICE_FIELD_DBUSER_TYPE,
    VOICE_FIELD_FANSCOUNT,
    VOICE_FIELD_FANSCOUNT_YESTERDAY,

    VOICE_FIELD_VESTTYPE,
    VOICE_FIELD_STATE_FLAG,
    VOICE_FIELD_MIC_INDEX,

    VOICE_FIELD_MAX,  // ���ɳ��� BYTE����
};

// ��װ�����û���Ϣ ��������ֶ�
struct SVoiceUserInfo_Table
{
    struct FieldInfo
    {
        int nOffset;
        int nLen;

        FieldInfo()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    SVoiceUserInfo context;
    FieldInfo fields[VOICE_FIELD_MAX];

    SVoiceUserInfo_Table()
    {
#define CreateField(id, member) \
    fields[id].nOffset = (int)(&((SVoiceUserInfo*)0)->member); \
    fields[id].nLen = sizeof(context.member);

        CreateField(VOICE_FIELD_FROMWORLDID, dwFromWorldID);
        CreateField(VOICE_FIELD_CLIENTID, dwClientID);
        CreateField(VOICE_FIELD_PDBID, dwPDBID);
        CreateField(VOICE_FIELD_UDBID, dwUDBID);
        CreateField(VOICE_FIELD_NAME, szName);
        CreateField(VOICE_FIELD_SEX, nSex);
        CreateField(VOICE_FIELD_RANK_LEVEL, nRankLevel);
        CreateField(VOICE_FIELD_KINNAME, szKinName);

        for(int i = VOICE_FIELD_CHANNEL_Gm; i <= VOICE_FIELD_CHANNEL_Personal; ++i)
        {
            CreateField(i, channelList[i - VOICE_FIELD_CHANNEL_None]);
        }

        CreateField(VOICE_FIELD_EX_CHANNEL, exChannel);
       
        CreateField(VOICE_FIELD_DBUSER_TYPE, dwDBUserType);
        CreateField(VOICE_FIELD_FANSCOUNT, dwFansCount);
        CreateField(VOICE_FIELD_FANSCOUNT_YESTERDAY, dwYesterdayFansCount);

        CreateField(VOICE_FIELD_VESTTYPE, nVestUserType);
        CreateField(VOICE_FIELD_STATE_FLAG, dwStateFlag);
        CreateField(VOICE_FIELD_MIC_INDEX, nMicIndex);
    }

    void getFields(rkt::obuf& ob, bool bMask[VOICE_FIELD_MAX])
    {
        int nFieldNumOffset = ob.size();
        
        // �ֶ�����
        ob << (BYTE)0;

        BYTE byUpdateNum = 0;
        for(int i = 0; i < VOICE_FIELD_MAX; ++i)
        {
            if(bMask[i])
            {
                ob << (BYTE)i;
                ob.push_back((char*)&context + fields[i].nOffset, fields[i].nLen);
                ++byUpdateNum;
            }
        }

        *(BYTE*)(ob.data() + nFieldNumOffset) = byUpdateNum;
    }

    bool updateFields(rkt::ibuffer& in, bool bMask[VOICE_FIELD_MAX] = NULL)
    {
        BYTE byFieldsNum = 0;
        in >> byFieldsNum;
        BYTE byFieldID = 0;
        for(int i = 0; i < byFieldsNum; ++i)
        {
            in >> byFieldID;
            if(in.fail())
            {
                return false;
            }

            if(byFieldID < 0 || byFieldID >= VOICE_FIELD_MAX)
            {
                return false;
            }

            const FieldInfo& field = fields[byFieldID];
            if(in.space() < field.nLen)
            {
                return false;
            }
            memcpy(((char*)&context) + field.nOffset, in.current(), field.nLen);
            in.offset(field.nLen);

            if(bMask) bMask[byFieldID] = true;
        }

        return true;
    }
};


// ����������������������¼
struct SMsgVoiceZoneLogin_ZS
{
	SVoiceUserInfo userInfo;
    char   key[VOICE_RC4_KEY_SIZE];

    SMsgVoiceZoneLogin_ZS()
    {
        memset(this, 0, sizeof(*this));
    }
};


//////////////////////////////////////////////////////////////////////////
// ��������
#define		MSG_VOICE_VOICEDATA			ENUM_MSG_VOICE_VOICEDATA

// ��֡��������Ϣ
struct SMsgVoiceAudioFrame
{
	WORD	nSize;		// ���ݴ�С
};
// ��֡������Ƶ����Ϣ
struct SMsgVoiceVideoFrame
{
	WORD	nAudioSize;		// �������ݴ�С
	WORD	nVideoSize;		// ��Ƶ���ݴ�С
};

// �������ṹ
struct SMsgVoiceAudioDataHead
{
	DWORD dwClientID;	// �ͻ���ID
	BYTE nCounts;		// ����
	BYTE nParam;		// ���� ������Դ�û����õ�������ϵ�� ���������Լ���������������

	// SMsgVoiceAudioFrame * nCounts;	// ÿ֡��Ϣ
	// char Frame[nSize] * nCounts;
};


//////////////////////////////////////////////////////////////////////////
// ���������������û����ͻ���
#define		MSG_VOICE_USERINFO_UPDATE			ENUM_MSG_VOICE_USERINFO_UPDATE


//////////////////////////////////////////////////////////////////////////
// Ƶ����Ա�б����
#define		MSG_VOICE_CHANNELUSERLIST_UPDATE		ENUM_MSG_VOICE_CHANNELUSERLIST_UPDATE


//////////////////////////////////////////////////////////////////////////
// ���·�����Ϣ
#define		MSG_VOICE_ROOMINFO_UPDATE		ENUM_MSG_VOICE_ROOMINFO_UPDATE
// ����ͷ
struct SMsgVoiceUpdateRoomHead_VC
{
	BYTE nUpdateType;	// ��������  EMVoiceUpdateRoomType
	BYTE nRoomID;		// ����ID
	// ��ͬ�಻ͬ����
};

// VoiceUpdateRoomType_All  ȫ��
// SMsgVoiceUpdateRoomBase_VC baseInfo;		// ���������Ϣ
// SMsgVoiceUpdateRoomMicUser_VC micUser;	// ���ϵ�����Ϣ ���������ģʽ���ŷ���


// VoiceUpdateRoomType_Base ������Ϣ
struct SMsgVoiceUpdateRoomBase_VC
{
	BYTE	nMicState;			// ������ EMVoiceMicState
	DWORD	dwStateFlag;		// ����״̬��� EMVoiceRoomStateFlag
	DWORD	dwUserCount;		// �����������ο�ֵ
	WORD	dwChatCoolDown;		// ����������ȴʱ��
	WORD	dwMicOrderTime;		// Ĭ������ʱ��
};

// VoiceUpdateRoomType_MicUser ���ϵ�����Ϣ
struct SMsgVoiceUpdateRoomMicUser_VC
{
	DWORD	dwClientID;				// ��ǰ�����ϵ��ˣ�ClientID
	DWORD	dwMicEndTime;			// �����ʱ��
	DWORD	dwFansCount;			// ��˿��
	WORD	dwWorldID;				// ���ϵ�����������ID
};

// VoiceUpdateRoomType_Remark ������Ϣ
struct SMsgVoiceUpdateRoomRemark_VC
{
	WORD nRemarkLen;					// ��ע��Ϣ����
	//char szRemarkLen[nRemarkLen]		// ��ע��Ϣ
};


//////////////////////////////////////////////////////////////////////////
// ����Ƶ����
#define		MSG_VOICE_CHANNELINFO_UPDATE	ENUM_MSG_VOICE_CHANNELINFO_UPDATE
// ����ͷ
struct SMsgVoiceUpdateChannelInfoHead_VC
{
	BYTE nUpdateType;		// ��������
};

// VoiceUpdateChannelType_All   ������Ϣ
struct SMsgVoiceUpdateChannelAll_VC
{
	BYTE nOwnerNameLen;						// Ƶ���������ֳ���
	SVoiceChannelBaseInfo baseInfo;			// Ƶ��������Ϣ
	WORD nRoomLockInfo;						// ���������Ϣ
	WORD nAudioInfo;						// �������������Ϣ
	BYTE nRoomNameLen[VoiceRoomType_Max];	// ������
	// char szOwnerName[nOwnerNameLen];
	// char szRoomName[VoiceRoomType_Max][VOICE_NAME_MAXSIZE]
};

// VoiceUpdateChannelType_Base  Ƶ��������Ϣ
struct SMsgVoiceUpdateChannelBase_VC
{
	SVoiceChannelBaseInfo baseInfo;			// Ƶ��������Ϣ
	BYTE nRoomLockInfo;						// ���������Ϣ
	WORD nAudioInfo;						// �������������Ϣ
};

// VoiceUpdateChannelType_Audio
struct SMsgVoiceUpdateChannelAudio_VC
{
	WORD nAudioInfo;		// �������������Ϣ
};

// VoiceUpdateChannelType_Name 
struct SMsgVoiceUpdateChannelName_VC
{
	char szChannelName[VOICE_NAME_MAXSIZE];		// Ƶ������
};

// VoiceUpdateChannelType_RoomName
struct SMsgVoiceUpdateRoomName_VC
{
	BYTE nRoomID;		// ����ID
	char szRoomName[VOICE_NAME_MAXSIZE];		// ��������
};

//////////////////////////////////////////////////////////////////////////
// ����������Ϣ
#define		MSG_VOICE_TEXTCHATMESSAGE	ENUM_MSG_VOICE_TEXTCHATMESSAGE
struct SMsgVoiceTextChatMessage_VC
{
	BYTE nChannel;				// ����Ƶ��
	DWORD dwWorldID;			// ����������ID
	BYTE nCountry;				// ����ID
	DWORD dwPDBID;				// ������PDBID
	DWORD dwTarget;				// ������PDBID
	BYTE nNameLen;				// ���������ֳ���
	BYTE nTextLen;				// ��������
	// char szName[nNameLen]
	// char szText[nTextLen]
};


//////////////////////////////////////////////////////////////////////////
// ˵���б�
#define		MSG_VOICE_VOICELIST		ENUM_MSG_VOICE_VOICELIST
struct SMsgVoiceVoiceList_VC
{
	DWORD	dwCount;		// ˵���˵ĸ���
};

struct SMsgVoiceVoiceNode_VC
{
	DWORD		dwPDBID;			// ˵���˵�PDBID
	BYTE		nChannelType;		// Ƶ������
	DWORD		dwWorldID;			// ����ID
	DWORD		dwID;				// IDֵ
	BYTE		nRoomID;			// �ӷ���ID
	DWORD		dwCount;			// ����
	WORD		nTime;				// ������ǰ˵��
	BYTE		nNameLen;			// ˵��������
	// char szName[nNameLen]
};

#define		MSG_VOICE_CHATLIST		ENUM_MSG_VOICE_CHATLIST						// ˽���б�
struct SMsgVoiceChatList_VC
{
	DWORD dwCount;		// ˽�ĸ���
};

struct SMsgVoiceChatNode_VC
{
	DWORD dwChatID;				// ˽��ID
	DWORD dwWorldIDInviter;		// ����������ID
	DWORD dwWorldIDAcceptor;	// ����������ID
	BYTE nNameLenInviter;	
	BYTE nNameLenAcceptor;
	DWORD dwInviterUDBID;		/// �������û��ʺ�ID
	DWORD dwAcceptorUDBID;		/// �������û��ʺ�ID
	// char szNameI[nNameLenInviter];
	// char szNameA[nNameLenAcceptor];
};


//////////////////////////////////////////////////////////////////////////
// �����б�
#define		MSG_VOICE_WORLDLIST		ENUM_MSG_VOICE_WORLDLIST
struct SMsgVoiceWorldList_VC
{
	DWORD	dwCount;		// ��Ϸ�������
};

struct SMsgVoiceWorldNode_VC
{
	DWORD	dwID;			// ��Ϸ����ID
	BYTE	nNameLen;		// ��Ϸ�������ֳ���
	// char szName[nNameLen];
};


//////////////////////////////////////////////////////////////////////////
// ��������Ƶ����Ϣ��ͨ�õĻ�����Ϣ
#define		MSG_VOICE_ALLCHANNELINFO_UPDATE		ENUM_MSG_VOICE_ALLCHANNELINFO_UPDATE	

// ����ͷ
struct SMsgVoiceUpdateAllChannelInfoHead_VC
{
	BYTE nUpdateType;		// ��������
};

// VoiceUpdateAllChannelType_Audio,	 ����Ƶ���Ƿ�����������Ϣ
struct SMsgVoiceUpdateAllChannelAudio_VC
{
	WORD nChannelAudioInfo;		// ����ʱ����Ϣ��λ����
};


//////////////////////////////////////////////////////////////////////////
// Ƶ������
#define		MSG_VOICE_AD		ENUM_MSG_VOICE_AD
struct SMsgVoiceAD_SC
{
	DWORD dwTime;		// �ټ�ʱ��
	DWORD dwPDBID;		// ������
	DWORD dwUDBID;		// UDBID
	BYTE nSex;			// �Ա�
	BYTE nChannelType;	// Ƶ������
	BYTE nRoomID;		// �ӷ���ID
	DWORD dwValueID;	// ֵID
	BYTE nADType;		// ��������Ч�� ��DIDA��λ��
	WORD nTextLen;		// �����ı�����
	// char szInviterName[nNameLen];
	// char szText[dwTextLen]	// �����ı�
};

struct SMsgVoiceAD_SV
{
	DWORD dwPDBID;			// ������
	BYTE nBroadcastType;	// EMVoiceADBroadcastType
	BYTE nADType;			// ��������Ч�� ��DIDA��λ��
	WORD nTextLen;			// �����ı�����
	// char szText[nTextLen]	// �����ı�
};

//////////////////////////////////////////////////////////////////////////
// ��ף���ʱ�����б�
#define		MSG_VOICE_VESTLIST		ENUM_MSG_VOICE_VESTLIST

// ����ͷ
struct SVoiceVestList_VC
{
	BYTE nUpdateType;		// ��������	EMVoiceUpdateType
	BYTE nCount;			// ���¸���
};

// ���½��
struct SVoiceVestNode_VC
{
	DWORD dwUDBID;			// dwUDBID
	BYTE nSex;
	BYTE nNewUserType;		// �û�����	
	BYTE nNameLen;			// ���Ƴ���
	// char szName[nNameLen];
};

//////////////////////////////////////////////////////////////////////////
// �������а�
#define MSG_VOICE_PERSONALTOPLIST	ENUM_MSG_VOICE_PERSONALTOPLIST
struct SVoicePersonalTopList_VC
{
	DWORD dwUpdateTime;
	BYTE nCount;	// ����
};

struct SVoicePersonalTopData_VC
{
	DWORD dwWorldID;
	BYTE nCountryID;
	DWORD dwUDBID;			// ͨ��֤ID
	DWORD dwUserCount;		// ��������
	DWORD dwMaxMemberCount;	// ��ʷ�����������
	BYTE nState;			// ״̬
	BYTE nOwnerType;		// ӵ�����ʺ�����
	BYTE nNameLen;			// ���Ƴ���
	//char szName[nNameLen];	// ����
};


//////////////////////////////////////////////////////////////////////////
// ˽������
#define MSG_VOICE_CHATINVITE	ENUM_MSG_VOICE_CHATINVITE
struct SVoiceChatInvite_SV
{
	DWORD dwInviter;		// ������PDBID
	DWORD dwAcceptor;		// ������PDBID
	bool bStranger;			// �Ƿ���İ����
};


//////////////////////////////////////////////////////////////////////////
// ˽���û���Ϣ����
#define MSG_VOICE_CHATUSERINFO_UPDATE ENUM_MSG_VOICE_CHATUSERINFO_UPDATE
struct SVoiceChatUserInfo_VC
{
	BYTE nUserType;			// �û����ͣ�0 ������ 1 ������
	SVoiceUserInfo userInfo;	// �û���Ϣ
};


//////////////////////////////////////////////////////////////////////////
// �ټ���˿
#define MSG_VOICE_CALLFANS ENUM_MSG_VOICE_CALLFANS
struct SVoiceCallFans_VC
{
	DWORD dwTime;				// ����ʱ��
	DWORD dwClientID;			// �ͻ���ID
	LONGLONG nID;				// �ټ�����ID
	BYTE nSex;
	BYTE nCountryID;			// ����ID
	DWORD dwFromWorldID;			// ����ID
	DWORD dwUDBID;				// UDBID
	char szNickname[VOICE_NAME_MAXSIZE];		// �ǳ�
	WORD nMsgLen;
	BYTE nParam1;			// ��������1
	BYTE nParam2;			// ��������2
	// char szMsg[nMsgLen];
};


//////////////////////////////////////////////////////////////////////////
// ������Ϣ���������������ɿ���
#define MSG_VOICE_BRIDGEDATA ENUM_MSG_VOICE_BRIDGEDATA
struct SMsgVoiceBridgeData
{
	BYTE nSrcEndPoint;		// Ŀ�ĵ�
	WORD wDestWorldID;		// Ŀ������ID
	DWORD dwPDBID;			// Ŀ�Ľ�ɫ
	DWORD dwDataLen;		// ���ݳ���
};


//////////////////////////////////////////////////////////////////////////
// �������
#define MSG_VOICE_ADDVOICEBLUEDIAMOND ENUM_MSG_VOICE_ADDVOICEBLUEDIAMOND
struct SMsgVoiceAddVoiceBlueDiamond_VV
{
	DWORD dwUDBID;		// �����UDBID
	DWORD dwPDBID;		// �����PDBID
	DWORD dwCount;		// �����������
	DWORD dwPDBIDSend;	// ������PDBID
	DWORD dwSpare1;
	DWORD dwSpare2;
	DWORD dwSpare3;		// ����
	WORD nReasonLen;	// ��ע����
	// char szReason[nReasonLen];
};


//////////////////////////////////////////////////////////////////////////
// ������Ϣ
#define MSG_VOICE_GIFTSENDINFO ENUM_MSG_VOICE_GIFTSENDINFO
struct SMsgVoiceGiftSendInfo
{
	DWORD dwTime;			// ����ʱ��
	DWORD dwPDBIDSend;		// ������
	WORD nWorldIDSend;		// ����������ID
	BYTE nCountrySend;		// �����˹���ID

	DWORD dwPDBID;		// ������
	WORD nWorldID;		// ����������ID
	BYTE nCountry;		// �����˹���ID

	WORD nGiftID;		// ����ID
	BYTE nGiftLevel;		// ����ȼ�
	BYTE nBroadcast;	// ����㲥��Χ

	DWORD dwFlag;		// λ��ʶ	EMVoiceSendGiftFlag
	DWORD dwData1;		// �����Զ���1
	DWORD dwData2;		// �����Զ���2
	DWORD dwData3;		// �����Զ���3

	BYTE nNameLenSend;	// ���������ֳ���
	BYTE nNameLen;		// ���������ֳ���
	WORD nMessageLen;	// ���Գ���
	//char szNameSender[nNameSendLen];
	//char szName[nNameLen];
	//char szMessage[nMessageLen];
};


//////////////////////////////////////////////////////////////////////////
// ���������Ϣ
#define MSG_VOICE_DIAMONDINFO ENUM_MSG_VOICE_DIAMONDINFO
struct SMsgVoiceDiamondInfo_VC
{
	DWORD dwUDBID;				// ͨ��֤UDBID,Ψһ
	DWORD dwBlueDiamond;		// ����������һֱ�ۼ�
	DWORD dwCurBlueDiamond;		// ��ǰ��������ֻ�������ֵ�ſɽ���ɺ���
	DWORD dwBlueDiamondOld;		// ����֮ǰ������
	DWORD dwBlueDiamondToday;	// ���ջ�����������ڶ����Զ��ѽ�������ֵ��ӵ�dwCurBlueCount�У�Ȼ������

	DWORD dwRedDiamond;			// ����������һֱ�ۼӣ��ⲿ�ֿɽ���ɽ��/RMB
};


//////////////////////////////////////////////////////////////////////////
// ��˿���а�
#define MSG_VOICE_FANSTOPLIST ENUM_MSG_VOICE_FANSTOPLIST
struct SMsgVoiceFansTopList_VC
{
	DWORD dwUpdateTime;		// ���а�����ʱ��
	BYTE nCount;			// ����
};

struct SMsgVoiceFansTopNode_VC
{
	DWORD dwUDBID;						// ͨ��֤UDBID
	BYTE nUserType;						// ͨ��֤�ʺ�����
	BYTE nRank;							// ����
	DWORD dwFansCount;					// ��˿����
	WORD dwFromWorldID;					// �����¼�Ľ�ɫ��Դ����ID
	BYTE nCountryID;					// �����¼�Ľ�ɫ����ID(���ң���½����)
	BYTE nNameLen;
	//char szActorName[nNameLen];		// �����¼�Ľ�ɫ��ɫ��
};


//////////////////////////////////////////////////////////////////////////
// ����Ա������Ϣ
#define MSG_VOICE_ADMINENTERINFO ENUM_MSG_VOICE_ADMINENTERINFO
struct SMsgVoiceAdminEnterInfo_VC
{
	bool bEnter;		// �Ƿ���� true����false ��
	DWORD dwClientID;	// �ͻ���ID
	WORD nWorldID;		// ����ID
	DWORD dwPDBID;
	BYTE nRoomID;		// �����ķ���ID
	BYTE nNameLen;		// ��ɫ����
	// char szActorName[nNameLen];
};

///////////////////////////////////////////////////////////////////////
// ���󷿼���Ա�б�
#define MSG_VOICE_REQ_ROOM_ACTOR_LIST ENUM_MSG_VOICE_REQ_ROOM_ACTOR_LIST				
struct SMsgVoiceReqRoomActorList_OV
{
	DWORD dwDBID;	// ��������DBID
	DWORD dwWorldID; 
	int	nNation;	// ����ID
	SVoiceChannelID roomInfo;  // ������Ϣ  ע:��ʼͳսʱ����ֶκ���
	BYTE byFlag;		// ����ͳս����0����ʼͳս����1

	SMsgVoiceReqRoomActorList_OV()
	{
		memset(this, 0, sizeof(*this));
	}
};

// ���ͷ�����Ա�б�
#define MSG_VOICE_SEND_ROOM_ACTOR_LIST ENUM_MSG_VOICE_SEND_ROOM_ACTOR_LIST			
struct SMsgVoiceSendRoomActorList_VO
{
	DWORD dwDBID; // �����ɫID
	DWORD dwWorldID; 
	int	nNation;	// ����ID
	SVoiceChannelID roomInfo;  // ������Ϣ
	DWORD		dwStatsMaxMemberNum; // ͳ�Ƶķ���������ֵ
	DWORD		dwActorCount;
	BYTE		byFlag;		// ����ͳս����0����ʼͳս����1
	//����������Ϣ
	//DWORD arrActorDBIDList[];

	SMsgVoiceSendRoomActorList_VO()
	{
		memset(this, 0, sizeof(*this));
	}
};

// ��������ͳս��ʱȨ��
#define MSG_VOICE_UB_SET_TEMP_RIGHT ENUM_MSG_VOICE_UB_SET_TEMP_RIGHT
struct SMsgVoiceUBSetTempRight_OV
{
	DWORD dwDBID;
	SVoiceChannelID roomInfo;  // ������Ϣ
	bool bOn; // true -- ����Ȩ�� false -- �ջ�Ȩ��
	int nStatsFlag; // 0--�رշ�ֵͳ�� 1--��ʼ��ֵͳ�� 2--ά�ַ�ֵͳ�� 
};

// ����������������ͳս��Ϣ
#define MSG_VOICE_UBINFO_UPDATE ENUM_MSG_VOICE_UBINFO_UPDATE 
struct SMsgVoiceUBUpdateActorInfo_OV
{
	DWORD dwDBID;
	DWORD dwTotalCredit; // �ۻ�����
	bool bIsUBCommander; // �Ƿ���ͳս
};

// ����֪ͨ�������
#define MSG_VOICE_SOCIAL_NOTIFY_USER_LOGOUT ENUM_MSG_VOICE_SOCAIL_NOTIFY_USER_LOGOUT
struct SMsgVoiceSocialNotifyActorLogout_OV
{
	DWORD dwPDBID;  // ���DBID
};

#pragma pack()