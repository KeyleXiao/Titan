/*******************************************************************
** �ļ���:	E:\speed\Common\ChatDef.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	10/15/2015	15��50
** ��  ��:	1.0
** ��  ��:	
            ����ϵͳ��ع�������
********************************************************************/

#pragma once
#include "GatewayServiceDef.h"
#include "PackagePtr.h"

//���DEBUG��Ϣ
#ifdef VERSEION_DEBUG
#define CHAT_DEBUG_OUTPUT
#endif

#ifdef CHAT_DEBUG_OUTPUT
# define CHAT_TRACELN(x)			TraceLn(x)
# define CHAT_WARNINGLN(x)			WarningLn(x)
# define CHAT_ERRORLN(x)			ErrorLn(x)
# define CHAT_EMPHASISLN(x)			EmphasisLn(x)
#else
# define CHAT_TRACELN(x)
# define CHAT_WARNINGLN(x)
# define CHAT_ERRORLN(x)
# define CHAT_EMPHASISLN(x)
#endif

using Gateway::ClientID;

#define MAX_LOUDSPEAKER_MSG_LEN			512	   		// ������Ϣ��󳤶�
#define MAX_CHAT_MSG_LENGTH				256			// ������Ϣ����
#define SYSTEMMESSAGE_MAX_LEN			512			// ϵͳ��ʾ��󳤶�
#define CHAT_MSG_ITEM_FLAG				(char)21	// �������ͷ���
#define FLOAT_TEXT_DEFAULT_TIME			1000		// Ʈ��Ĭ��ʱ��
#define CHAT_NEARBY_DISTANCE			24			// ����������Ϣ���շ�Χ
//#define MAX_REPORT_MSG_LENGTH			256			// �ٱ�������Ϣ
//#define REPORT_COLD_TICKCOUNT			180000		// �ٱ�����ȴʱ����
#define SYSTEMMESSAGE_SCHEME_FILENAME	"Scp\\SystemMessage.csv"
#define CHATCD_SCHEME_FILENAME	"Scp\\ChatCD.csv"

#define LOUDSPEAKER_CALLTIMER_HUGE      25000    // ���ȵĹ㲥���ʱ��

// ����ö�ٶ���
#include "Chat_ManagedDef.h"

// ϵͳ��Ϣ�㲥���Ͷ���
enum EMChatBroadcastType
{
	CHAT_BROADCASTTYPE_SINGLE,		// �Ե���(��PDBID)
	CHAT_BROADCASTTYPE_TEAMMATE,	// ��ȫ��(��PDBID)
	CHAT_BROADCASTTYPE_WARSCENE,	// ��ȫ��(������ID)
	CHAT_BROADCASTTYPE_SERVER,		// ��ȫ������(��������ID)
	CHAT_BROADCASTTYPE_KIN,			// ��ս��(��ս��ID)
	CHAT_BROADCASTTYPE_CLAN,		// �԰��(�����ID)
	CHAT_BROADCASTTYPE_ROOM,		// �Է���(������ID)
	CHAT_BROADCASTTYPE_WORLD,		// ������(��PDBID)
	CHAT_BROADCASTTYPE_LOUDSPEAKER,	// ������(��0)
	CHAT_BROADCASTTYPE_ZONE,		// ������(������ID)
	CHAT_BROADCASTTYPE_CAMP,		// ����Ӫ(ս���ڴ�����ID�������ڴ�PDBID)

	CHAT_BROADCASTTYPE_MAX			// ���������
};

// SystemMessage.csv�ű���¼ʹ�ñ�ʶ
enum EMChatTextUseFlag
{
	CHAT_TEXTUSEFLAG_CLIENT    = 0x01,     // �ͻ���ʹ�� 1
	CHAT_TEXTUSEFLAG_SCENE     = 0x02,     // ������ʹ�� 2
	CHAT_TEXTUSEFLAG_SOCIAL    = 0x04,     // ����ʹ�� 4
	CHAT_TEXTUSEFLAG_LOGIN     = 0x08,     // ��½��ʹ�� 8
	CHAT_TEXTUSEFLAG_GATEWAY   = 0x10,     // ���ط�ʹ�� 16
};

// ��Ҿٱ���Ϣ��GM���Ͷ���
enum EMChatReportToGMType
{
    // δ����
    CHAT_REPORTTOGMTYPE_NULL = 0,		// δ֪
    CHAT_REPORTTOGMTYPE_REPORT,			// ��Ҿٱ���Ϣ
    CHAT_REPORTTOGMTYPE_REPORTCHAT,		// �����ظ�Gm���� dwUserData1 �ش��ٱ���ϢID,������Ϣ�����û�˵��szMsg��
    CHAT_REPORTTOGMTYPE_REPLYFAIL,		// GM�ظ�ʧ�� dwUserData1 �ش��ٱ���ϢID

    CHAT_REPORTTOGMTYPE_MAX				// ���������
};

// ��Ҿٱ���Ϣ��GMԭ���ʶ,λ��ʶ
enum EMChatReportToGMReasonFlag
{
    CHAT_REPORTTOGMREASON_NULL			= 0x0,			// 0	:��
    CHAT_REPORTTOGMREASON_OTHER			= 0x1,			// 1	:����
    CHAT_REPORTTOGMREASON_BILK			= 0x2,			// 2	:թƭ
    CHAT_REPORTTOGMREASON_HOSTILITY		= 0x4,			// 4	:����
    CHAT_REPORTTOGMREASON_BLACKMAIL		= 0x8,			// 8	:�ٱ�������ַ

    CHAT_REPORTTOGMREASON_MAX			= 0x10000		// ���
};

// ��������
enum EMLoudspeakerType
{
	LOUDSPEAKERTYPE_NULL = 0,
	LOUDSPEAKERTYPE_HUGE,					// ������
	LOUDSPEAKERTYPE_MAX					
};

// Ȩ��
enum EMPrivilegeFlag
{
	PrivilegeFlag_None              = 0,
	PrivilegeFlag_Notice            = 1 << 0,	// ����
	PrivilegeFlag_Stat_Online       = 1 << 1,	// ͳ����������
	PrivilegeFlag_Kickout           = 1 << 2,	// ����
    PrivilegeFlag_HideSelf          = 1 << 3,   // ��������
    PrivilegeFlag_VipViewBattle     = 1 << 4,   // VIP��ս 
};

// �㲥��Ϣ
struct SBroacastSysMessage
{
	int broadType;				// EMChatBroadcastType
	int broadTargetID;
	int tipID;					// EMChatTipID
	string strMsg;
	DWORD dwSendTime;

	SBroacastSysMessage()
	{
		memset(this, 0, sizeof(*this));
	}
};


// ��δ��ֲ�Ĺ��ܵĶ��壬�ȹ�����ֲ���ٿ���
/*
// �����򿪹ٷ���ĳЩ���Ӷ���
enum EMOpenUrlType
{
	OPENURLTYPE_FORUM,			// ��̳
};


// ��������
enum EMChatMuteType
{
	CHAT_MUTETYPE_GENERAL,	// ��ͨ����
	CHAT_MUTETYPE_COMPUTER,	// ���Ե���
	CHAT_MUTETYPE_AUTO,		// �Զ�����
	CHAT_MUTETYPE_OFFLINE,	// ���߽���

	CHAT_MUTETYPE_MAX		// �����������ֵ
};

// ���Թ���
enum EMChatMuteRule
{
	CHAT_MUTERULE_NULL = 0,					// �����κδ���
	CHAT_MUTERULE_NORMAL,					// ��ʾ��ұ����ԣ�����޷�������Ϣ��������ǰ�Ľ��Դ������
	CHAT_MUTERULE_NOPROMPT,					// ����ʾ��ұ����ԣ���ҿ��Է���Ϣ��������㲥�������ɼ������������ӵĻ���
	CHAT_MUTERULE_EXCLUDEPRIVATECHANNELS,	// ս�ӣ����飬˽�ģ���ᣬ�Ŷ�,�������ƣ������Ĳ�����
	
	CHAT_MUTERULE_MAX
};

// ������������ʽ
enum EMChatRudeFine
{
	CHAT_RUDEFINE_CHECK,	// ��ѯ������ֵ
	CHAT_RUDEFINE_ADD,		// ��ӷ�����ֵ

	CHAT_RUDEFINE_MAX
};
*/

// �����ṹ�嶨��
// SystemMessage�ű���Ϣ�ṹ
struct SSchemeSystemMessage
{
	int             nId;              // Ʈ��id
	std::string     strDesc;          // ��������
	int             nType;            // ��Ϣ����
	int             nPos;             // ��Ϣλ��
	int             nShowTime;        // ��ʾʱ��
	int             nFlag;            // ʹ�ñ�ʶ EMTEXT_USE_FLAG
	std::string		strTitle;		  // �������⣨�����ڵ�����Ϣ��

	SSchemeSystemMessage()
	{
		nId = 0;
		nType = 0;
		nPos = 0;
		nShowTime = 0;
		nFlag = 0;
	}
};

// ChatCD�ű���Ϣ�ṹ
struct SSchemeChatCD
{
	int nChannel;		// Ƶ��ID
	int nLimitLevel;	// �ȼ�����
	int nCDTime;		// ��ȴʱ��

	SSchemeChatCD()
	{
		memset(this, 0, sizeof(*this));
	}
};

// ����Ϊ������ṹ��,��Ҫ����ʱ������

//  ��Ҿٱ���Ϣ��GM��Ϣ���ݽṹ
struct SMsgReportToGMInfo
{
	DWORD dwMsgType;							// ��Ϣ����  EMChatReportToGMMsgType
	DWORD dwReasonFlag;							// ԭ���ʶ  ChatReportToGMReasonFlag
	BOOL  bIsVIP;								// �Ƿ���VIP
	DWORD dwReporterID;							// ��ҽ�ɫID
	DWORD dwCountryID;							// ��ҽ�ɫ����ID
	string strReporterName;						// ��ɫ��
	DWORD dwCheaterID;							// �ٱ�������ҽ�ɫID
	string strCheaterName;						// �ٱ������ɫ��
	string strTitle;							// �ٱ�֤�ݣ���󳤶Ⱥ��������ݳ���һ�£�
	string strMsg;								// ����˵��
};
/*
struct SMsgChat_KickoutToPublicWorld
{
	char szKickPersonName[ACTORNAME_MAXSIZE];	// ������
	char szSenderName[ACTORNAME_MAXSIZE];		// ������
	char szKickReason[512];						// ����ԭ��

	SMsgChat_KickoutToPublicWorld(void)
	{
		memset(this, 0, sizeof(*this));
	};
};
*/

#include "ChatMessageDef.h"

