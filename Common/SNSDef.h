/*******************************************************************
** �ļ���:	SNSDef.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	��绪
** ��  ��:	11/24/2016
** ��  ��:	1.0
** ��  ��:	�罻����Social Network Service Server)ģ�鶨��	
** Ӧ  ��:  	
	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/

#ifndef __SNS_DEF_H__
#define __SNS_DEF_H__

#pragma pack(1)
#pragma warning(push) // warning C4200: ʹ���˷Ǳ�׼��չ : �ṹ/�����е����С����
#pragma warning(disable : 4200)

#include "NameLenDef.h"
#include "SNS_ManagedDef.h"

/************************************************************************/
/* ������Ϣ����궨��                                                   */
/************************************************************************/
// ���DEBUG��Ϣ
#ifdef VERSEION_DEBUG
#   define SNS_DEBUG_OUTPUT
#endif
#ifdef SNS_DEBUG_OUTPUT
#	define SNS_TRACELN(x)			TraceLn(x)
#	define SNS_EMPHASISLN(x)		EmphasisLn(x)
#	define SNS_ERRORLN(x)			ErrorLn(x)
#	define SNS_WARNINGLN(x)			WarningLn(x)
#else
#	define SNS_TRACELN(x)
#	define SNS_EMPHASISLN(x)
#	define SNS_ERRORLN(x)
#	define SNS_WARNINGLN(x)
#endif


#define		SNS_CHAT_MSG_MAXSIZE			512			// ������Ϣ����
#define		ACCOUNT_NAME_MAXSIZE			32			// �˺����ֳ���
#define		ACTOR_NAME_MAXSIZE				32			// ��ɫ���ֳ���
#define		BUDDYGROUP_NAME_MAXSIZE			32			// ���ѷ������Ƴ���
#define     SERVER_NAME_MAXSIZE             32
#define     FONT_NAME_MAXSIZE				32			// �������ֳ���
#define     REMARK_NAME_MAXSIZE				32			// ��ע���ֳ���
#define     MOOD_CONTENT_MAXSIZE			128			// ��ע���ֳ���
#define     SNS_DEFAULT_LISTEN_PORT         8600		// SNS������Ĭ�ϼ����˿�
#define		STATUS_REVERSE_STR_MAXSIZE		32			// Ĭ���ַ�����󳤶�
#define		GAME_WORLD_NAME_MAXSIZE			32			// ��Ϸ�����������ֵ
#define		SNS_NEARBY__MAXSIZE				15			// ������������

/***************************************************************/
// �ϲ����Ϣ��ض��� MSG_MODULEID_SNS
/***************************************************************/

///////////////////////////////////////////////////////////////////
// ��Ϣ��ö�ٶ���: ENUM + ��Ϣ����
enum EM_MSG_SNS
{
    MSG_SNS_INVALID = 0,
    MSG_SNS_HANDSHAKE,						// ������Ϣ��������������
    MSG_SNS_HANDSHAKE_RESPONSE,				// ��Ӧ������Ϣ
    MSG_SNS_CONNECTED,						// ��SNS����������������
    MSG_SNS_DISCONNECTED,					// ��SNS�������Ͽ�����
    MSG_SNS_KEEPALIVE,						// ������飨������������
    MSG_SNS_ANSWER_KEEPALIVE,				// ��Ӧ�������
	MSG_SNS_TRANSMIT2CLIENT,				// ����Ϣ����������ͨ����ת�����ͻ���
	MSG_SNS_TRANSMIT2SNS,					// ����Ϣ����������ͨ����ת�����ͻ���
	MSG_SNS_USER_LOGIN,						// ��ҵ�½
	MSG_SNS_USER_LOGOUT,					// ��ҵǳ�

	MSG_SNS_UPDATE_BUDDY,					// ��ӡ�ɾ�����ѣ��ͻ��˷��ͣ�
	MSG_SNS_REQ_RECOMMAND_BUDDY,			// �����Ƽ����ѣ��ͻ��˷��ͣ�
	MSG_SNS_REQ_BUDDY_INFO,					// ���������Ϣ���ͻ��˷��ͣ�
	MSG_SNS_REQ_BUDDYRELATION,				// ������ѹ�ϵ���ͻ��˷��ͣ�
	MSG_SNS_SEND_MSG_DATA,					// ����������Ϣ���ͻ��˷��ͣ�
	MSG_SNS_UPDATE_REMARK,					// �޸ĺ��ѱ�ע���ͻ��˷��ͣ�
	MSG_SNS_RSP_ADD_BUDDY,					// ��Ӧ��Ӻ������󣨿ͻ��˷��ͣ�
	MSG_SNS_UPDATE_BUDDYGROUP,				// ���º��ѷ�����Ϣ���ͻ��˷��ͣ�
	MSG_SNS_CHANGE_BUDDYGROUP,				// �ı���ѷ��飨�ͻ��˷��ͣ�
	MSG_SNS_REQ_ADD_BUDDY_BY_NAME,			// ͨ��������Ӻ��ѣ��ͻ��˷��ͣ�
	MSG_SNS_REQ_USERINFO,					// �����û���Ϣ���ͻ��˷��ͣ�
	MSG_SNS_REQ_CHANGE_PERMISSION,			// �����޸�Ȩ�ޣ��ͻ��˷��ͣ�
	MSG_SNS_REQ_CHANGE_MOOD,				// �����޸����飨�ͻ��˷��ͣ�
	MSG_SNS_REQ_UPDATE_BUDDY_STATUS,		// �����������״̬���ͻ��˷��ͣ�
	MSG_SNS_REQ_INVITE_PLAYGAME,			// ��������һ����Ϸ���ͻ��˷��ͣ�
	MSG_SNS_SEND_DATA_TO_SOCIAL,			// �ͻ���C#ֱ�ӷ����ݵ��������ͻ��˷��ͣ�

	MSG_SNS_MSG_DATA,						// ����������Ϣ�������ͻ��ˣ�
	MSG_SNS_UPDATE_RELATIONSHIP,			// ���ѹ�ϵ״̬���£������ͻ��ˣ�
	MSG_SNS_BUDDY_STATUS,					// ��������״̬�������ͻ��ˣ�
	MSG_SNS_BUDDY_LIST,						// ���к�����Ϣ�������ͻ��ˣ�
	MSG_SNS_BUDDY_GROUP_LIST,				// ���к��ѷ�����Ϣ�������ͻ��ˣ�
	MSG_SNS_BUDDY_RELATION_LIST,			// ���к��ѹ�ϵ�б������ͻ��ˣ�
	MSG_SNS_SYSTEM_MESSAGE,					// ����ʾ��Ϣ�������ͻ��ˣ�
	MSG_SNS_BUDDY_INFO_SELF,				// �Լ��Ľ�ɫ��Ϣ�������ͻ��ˣ�
	MSG_SNS_BUDDY_INFO,						// ���غ�����Ϣ�������ͻ��ˣ�
	MSG_SNS_BUDDY_RELATIONSHIP,				// ���غ��ѹ�ϵ��Ϣ�������ͻ��ˣ�
	MSG_SNS_BUDDY_REMARK,					// ���غ��ѱ�ע�������ͻ��ˣ�
	MSG_SNS_USERINFO_ACK,					// �����û���Ϣ�������ͻ��ˣ�
	MSG_SNS_BUDDYINFO_CHANGE_NOTIFY,		// ֪ͨ�û�ĳ�����ѵ���Ϣ�����ı䣨�����ͻ��ˣ�
	MSG_SNS_USERSTATUS_NOTIFY,				// ֪ͨ�û�����״̬�������ͻ��ˣ�
	MSG_SNS_INVITE_PLAY_GAME_NOTIFY,		// ֪ͨ����һ����Ϸ�������ͻ��ˣ�
	MSG_SNS_UPDATE_SCENE_NOTIFY,			// ������ҵĳ�����Ϣ(�������к��ѿͻ��ˣ�
	MSG_SNS_RELATION_CHANGE_NOTIFY,			// ���ѹ�ϵ�ı�֪ͨ(�����ͻ��ˣ�

	MSG_SNS_REQ_DIRECT_ADD_BUDDY,			// ֱ������������Ϊ���ѣ�������SNS����
	MSG_SNS_UPDATE_SCENE,					// ������ҵĳ�����Ϣ��������SNS����
	MSG_SNS_UPDATE_ACTOR_INFO,				// ������ҵĽ�ɫ��Ϣ��������SNS����

	MSG_SNS_REQ_INVITE_MATCH_TEAM,			// ��������ƥ����ӣ��ͻ��˷��ͣ�
	MSG_SNS_INVITE_MATCH_TEAM_NOTIFY,		// ֪ͨ����ƥ����ӣ������ͻ��ˣ�

    MSG_SNS_MAXID,							// �����Ϣ��
};



// DB����״̬
enum EMSNS_DB_OPT_TYPE_UPDATE_BUDDY
{
	EMSNS_DB_STATUS_NONE,						
	EMSNS_DB_STATUS_ADD_BLACKLIST,				// ���������
	EMSNS_DB_STATUS_ADD_BUDDY,					// ��Ӻ���
	EMSNS_DB_STATUS_DEL_BUDDY,					// ɾ������
	EMSNS_DB_STATUS_AGREE_ADDBUDDY,				// ͬ����Ӻ���
	EMSNS_DB_STATUS_REFUSE_ADDBUDDY,			// �ܾ���Ӻ���
	EMSNS_DB_STATUS_ADD_BUDDY_DIRECT,			// ֱ����Ӻ���
};

// DB����״̬����ѯ������Ϣ
enum EMSNS_DB_OPT_TYPE_READ_BUDDYINFO
{
	EMSNS_DB_OPT_READ_BUDDYINFO_NORMAL,			// ������ѯ������Ϣ
	EMSNS_DB_OPT_READ_BUDDYINFO_ADDBUDDY,		// ���Ҳ���Ӻ���
};

// DB����״̬����ѯ�����Ϣ
enum EMSNS_DB_OPT_TYPE_READ_PLAYERINFO
{
	EMSNS_DB_OPT_TYPE_READ_PLAYERINFO_NORMAL,		// ������ѯ�����Ϣ
	EMSNS_DB_OPT_TYPE_READ_PLAYERINFO_ADDBUDDY,		// ���Ҳ���Ӻ���
};
enum EMSNS_GAME_ID
{
	EMSNS_GAME_ID_NONE,
	EMSNS_GAME_ID_TITAN,
};

// ��ɫ��Ϣ��������
enum ESNSActorInfoUpdateType
{
	ESNSActorInfoUpdateType_Unkown,
	ESNSActorInfoUpdateType_Name,
	ESNSActorInfoUpdateType_GameState,
	ESNSActorInfoUpdateType_ClanName,
	ESNSActorInfoUpdateType_MatchRankInfo,
};

struct SSNSGameWorldInfo
{
	DWORD	dwGameID;							// ��ϷID�������Ŀ���Ϸ����TITAN��TITAN���ε�
	DWORD	dwWorldID;							// ������������Ϸ����ID
	WORD	nServerType;						// ����������
	WORD	wSubID;								// ��n�ŷ�����
	char	szName[SERVER_NAME_MAXSIZE];		// ��������


	SSNSGameWorldInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	bool operator ==(const SSNSGameWorldInfo& info) const
	{
		return info.dwGameID == dwGameID && info.dwWorldID == dwWorldID;
	}
};


// ��ɫ��Ϣ
struct SSNSActorInfo
{
	DWORD	dwUserID;						// �˺�ID
	DWORD	dwGameID;						// ��ϷID�������ĸ���Ϸ
	DWORD	dwWorldID;						// ����ID
	DWORD	dwServerID;						// ������ID
	DWORD	dwSceneID;						// ����ID
	DWORD	dwPdbID;						// pdbid��������ڷ��Ľ�ɫid
	DWORD	dwHeadID;						// ͷ��ID
	char	szName[ACTOR_NAME_MAXSIZE];		// ��ɫ����
	int		nLevel;							// ��ɫ�ȼ�
	DWORD	dwLastLogout;					// �������ʱ��
	int		nMatchNum;						// �ܳ���
	int		nWinNum;						// ʤ����
	char	szClan[CLAN_NAME_SIZE];			// ��������
	char	szTitle[TITLE_NAME_SIZE];		// �ƺ�
    int     nRankGrade;                      // ��λ�ȼ�
	int		nRankScore;						// ��λ��
	char	szRankName[RANK_NAME_MAXSIZE];	// ��λ����
	BYTE    nSex;									// �Ա� 0�� 1Ů 2������
	char    szWorldName[GAME_WORLD_NAME_MAXSIZE];	// ��������
	DWORD	dwUpdateTime;							// ��ɫ��Ϣ����ʱ��

	SSNSActorInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// ���ѹ�ϵ
struct SSNSRelationship
{
	DWORD	dwUserID1;									// �û�1���˺�id
	DWORD	dwUserID2;									// �û�2���˺�id
	BYTE	nStatus;									// ��ϵ״̬
	DWORD	dwBuddyGroupID;								// ���ѷ���ID
	BYTE	nIntimacy;									// ���ܶ�
	char	szRemark[REMARK_NAME_MAXSIZE];				// �û�2���û�1�еı�ע��
	DWORD	nUpdated;									// ����ʱ��
	
	SSNSRelationship()
	{
		memset(this, 0, sizeof(*this));
	}
};

// �û���Ϣ
struct SSNSUserInfo
{
	DWORD dwUserID;							// �û�ID
	int nPermission;						// Ȩ��
	char szMood[MOOD_CONTENT_MAXSIZE];		// ����
	DWORD dwUpdateTime;						// ������ʱ��

	SSNSUserInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// ��������״̬��Ϣ
struct SSNSBuddyStatusInfo
{
	BYTE nStatus;							// ״̬�����ߡ���Ϸ�С������
	char szReverse[STATUS_REVERSE_STR_MAXSIZE];	// �����ַ���

	SSNSBuddyStatusInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};


// ������Ϣ
struct SSNSBuddyInfo
{
	SSNSBuddyStatusInfo statusInfo;
	SSNSActorInfo actorInfo;			// ��ɫ��Ϣ
	SSNSUserInfo userInfo;				// �û���Ϣ

	SSNSBuddyInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SSNSBuddyGroupInfo
{
	BYTE nRelationStatus;						// ���ѷ����ϵ״̬��EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP
	DWORD dwBuddyGroupID;						// ���ѷ���ID
	char szName[BUDDYGROUP_NAME_MAXSIZE];		// ���ѷ�������

	SSNSBuddyGroupInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SSNSBuddyRemark
{
	DWORD	dwUserID1;							// ���1ID
	DWORD	dwUserID2;							// ���2ID
	char	szRemark[REMARK_NAME_MAXSIZE];		// ���2�ı�ע��

	SSNSBuddyRemark()
	{
		memset(this, 0, sizeof(*this));
	}
};

// ��Ϸ���緢��SNS������ʱ��Я����ͷ����Ϣ
struct SONMsgSNSHead
{
	DWORD dwGameID;			// ��ϷID
	DWORD dwGameWorldID;	// ����ID
	DWORD dwUserID;			// �˺�ID

	SONMsgSNSHead()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SNS����������Ϸ���緢����ʱ�����ͷ����Ϣ
struct SNOMsgSNSHead
{
	DWORD dwUserID;		// �˺�ID
	DWORD dwPdbID;			// ��ɫID
	SNOMsgSNSHead()
	{
		memset(this, 0, sizeof(*this));
	}
};

//  SNS�����������ͻ�����Ϣʱ��Я����ͷ����Ϣ��ͨ������ת����
struct SNCMsgSNSHead
{
	DWORD dwSNSMsgID;
	SNCMsgSNSHead()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////��Ϸ���緢��SNS������///////////////////////////
// MSG_SNS_HANDSHAKE		            �������������������Ϣ���ݽṹ
struct SMsgSNSHandshake
{
	DWORD	dwGameID;							// ��ϷID�������ĸ���Ϸ
    DWORD	dwWorldID;							// ������������Ϸ����ID
    WORD	nServerType;						// ����������
    WORD	wSubID;								// ��n�ŷ�����
    char	szName[SERVER_NAME_MAXSIZE];		// ��������

    SMsgSNSHandshake(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// MSG_SNS_USER_LOGIN		��½��Ϣ
struct SMsgSNSUserLogin
{
	SSNSGameWorldInfo gameWorldInfo;			// ��Ϸ������Ϣ
	SSNSActorInfo actorInfo;					// ��ɫ��Ϣ
	char clientIP[16];								// �ͻ���IP

    SMsgSNSUserLogin(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// MSG_SNS_UPDATE_BUDDY	���º��Ѳ���
struct SMsgSNSUpdateBuddy
{
	BYTE nOptType;									// �������ͣ�EMSNS_BUDDY_UPDATE_TYPE
	DWORD dwOtherAccountID;							// ����Ӻ��ѵ��˺�ID
	char szActorName[ACTOR_NAME_MAXSIZE];			// ��ɫ��

	SMsgSNSUpdateBuddy(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_RSP_ADD_BUDDY ��Ӧ��Ӻ�������
struct SMsgSNSRspAddBuddy
{
	DWORD dwOtherAccountID;							// ����Ӻ���ID
	BYTE nFlag;										// �Ƿ�ͬ�⣺EMSNS_RSP_ADDBUDDY_TYPEE_AGGRE
	char szActorName[ACTOR_NAME_MAXSIZE];			// ��ɫ��

	SMsgSNSRspAddBuddy()
	{
		memset(this, 0, sizeof(*this));
	}
};


// ON_MSG_SNS_USER_LOGOUT ע��
struct SONMsgSNSLogout
{
	SSNSActorInfo actorInfo;					// ��ɫ��Ϣ

	SONMsgSNSLogout()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_UPDATE_BUDDYGROUP ���º��ѷ�����Ϣ
struct SMsgSNSUpdateBuddyGroup
{
	BYTE	nOptType;			// �������ͣ�EMSNS_BUDDYGROUP_UPDATE_TYPE
	DWORD	dwBuddyGroupID;		// ����ID
	char	szName[32];			// �µķ�������
	SMsgSNSUpdateBuddyGroup()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_RECOMMAND_BUDDY �����Ƽ�����
struct SMsgSNSReqRecommandBuddy
{
	
	SMsgSNSReqRecommandBuddy()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_UPDATE_REMARK �޸ĺ��ѱ�ע
struct SMsgSNSUpdateRemark
{
	DWORD dwUserID;			// �����˺�ID
	char szActorName[32];	// ��ע��
	char szName[32];		// ��ע��
	SMsgSNSUpdateRemark()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_BUDDY_INFO // ��ȡ������Ϣ
struct SMsgSNSReqBuddyInfo
{
	DWORD dwUserID;		// �����˺�ID
	SMsgSNSReqBuddyInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_BUDDYRELATION ��ȡ���ѹ�ϵ
struct SMsgSNSReqBuddyRelation
{
	DWORD dwUserID;		// �����˺�ID
	
	SMsgSNSReqBuddyRelation()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_CHANGE_BUDDYGROUP    �ı���ѷ���
struct SMsgSNSReqChangeBuddyGroup	
{
	DWORD dwUserID;				// �����û�ID
	DWORD dwBuddyGroupID;		// �µĺ��ѷ���ID

	SMsgSNSReqChangeBuddyGroup()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_ADD_BUDDY_BY_NAME  ͨ��������Ӻ���
struct SMsgSNSReqAddBuddyByName
{
	char szActorName[32];		// ��ɫ��

	SMsgSNSReqAddBuddyByName()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_USERINFO �����û���Ϣ
struct SMsgSNSReqUserInfo
{
	DWORD dwUserID;			// �û�ID

	SMsgSNSReqUserInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_CHANGE_PERMISSION �����޸�Ȩ��
struct SMsgSNSReqChangePermission
{
	int nPermission;		// Ȩ��

	SMsgSNSReqChangePermission()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_CHANGE_MOOD �����޸�����
struct SMsgSNSReqChangeMood
{
	char szMood[MOOD_CONTENT_MAXSIZE];	// ����

	SMsgSNSReqChangeMood()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_UPDATE_BUDDY_STATUS �����������״̬
struct SMsgSNSReqUpdateBuddyStatus
{
	BYTE nStatus;									// ����״̬�����ߡ����ߡ���Ϸ�еȵȣ�
	char szReverse[STATUS_REVERSE_STR_MAXSIZE];			// �����ֶ�

	SMsgSNSReqUpdateBuddyStatus()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_INVITE_PLAYGAME �����������һ����Ϸ
struct SMsgSNSReqInvitePlayGame
{
	int nUserID;
	int nRoomID;
	char szInviterName[ACTORNAME_MAXSIZE];

	SMsgSNSReqInvitePlayGame()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_REQ_INVITE_PLAYGAME �����������һ����Ϸ
struct SMsgSNSReqInviteMatchTeam
{
	int nUserID;
	char szInviterName[ACTORNAME_MAXSIZE];

	SMsgSNSReqInviteMatchTeam()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_SEND_DATA_TO_SOCIAL C#ֱ�ӷ����ݵ�����
struct SMsgSNSSendDataToSocial
{
	int nMsgType;

	// data...

	SMsgSNSSendDataToSocial()
	{
		memset(this, 0, sizeof(*this));
	}
};
///////////////////////////SNS������������Ϸ����/////////////////////////////////////////////
// MSG_SNS_HANDSHAKE_RESPONSE		 ��������Ӧ������Ϣ���ݽṹ
struct SMsgSNSHandshakeResponse	
{
	DWORD	dwID;								// ����ID
	DWORD	dwServerTime;						// ������ʱ��

	SMsgSNSHandshakeResponse(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_MSG_DATA		������Ϣ
struct SMsgSNSMsgData
{
	DWORD dwMsgID;					// ��ϢID����������д��
	DWORD dwTime;					// ����ʱ�䣨��������д��
	DWORD dwFromAccountID;			// �������˺�ID
	char szFromName[32];			// ���������֣���������д��
	DWORD dwToAccountID;			// Ŀ���˺�ID
	char szFont[FONT_NAME_MAXSIZE];	// ����
	int nFontSize;					// �����С
	int nFontColor;					// ������ɫ
	int nContentLen;				// �������ݴ�С

	SMsgSNSMsgData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_RELATIONSHIP_STATUS	// ���º��ѹ�ϵ״̬
struct SMsgSNSUpdateRelationship
{
	DWORD	dwUserID;		// ���ID
	BYTE	nStatus;			// ��ϵ״̬
	char	szActorName[32];		// ��ҵı�ע��

	SMsgSNSUpdateRelationship(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_BUDDY_STATUS		// ��������״̬
struct SMsgSNSBuddyStatus
{
	DWORD dwUserID;							// �����˺�ID
	BYTE nStatus;							// ����״̬�����ߡ����ߡ���Ϸ�еȵȣ�
	char szReverse[STATUS_REVERSE_STR_MAXSIZE];	// ����״̬�����ַ���
	DWORD dwLastLogoutTime;					// ���һ������ʱ��

	SMsgSNSBuddyStatus()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_SYSTEM_MESSAGE  // ����ʾ��Ϣ���ͻ���
struct SMsgSNSSystemMsg
{
	int nTipID;				// ��ʾID
	int nLen;				// ��ʾ��Ϣ����
	
	SMsgSNSSystemMsg()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_BUDDY_INFO_LIST
struct SMsgSNSBuddyList
{
	int nBuddyCount;		// ���Ѹ���
	
	// ������ nBuddyCount * SSNSBuddyInfo

	SMsgSNSBuddyList()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_BUDDY_GROUP_LIST
struct SMsgSNSBuddyGroupList
{
	int nBuddyGroupCount;		// ���ѷ������

	// ������ nBuddyGroupCount * SSNSBuddyGroupInfo
	

	SMsgSNSBuddyGroupList()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_BUDDY_RELATION_LIST
struct SMsgSNSBuddyRelationList
{
	int nBuddyRelationCount;		// ���ѷ������

	// ������ nBuddyRelationCount * SSNSRelationship

	SMsgSNSBuddyRelationList()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_BUDDY_INFO
struct SMsgSNSBuddyInfo
{
	SSNSBuddyInfo buddyInfo;		// ������Ϣ

	SMsgSNSBuddyInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_BUDDY_RELATIONSHIP
struct SMsgSNSBuddyRelationship
{
	SSNSRelationship buddyRelation;
};

// MSG_SNS_BUDDY_REMARK
struct SMsgSNSBuddyRemark
{
	SSNSBuddyRemark remark;
};

// MSG_SNS_BUDDY_INFO_SELF
struct SMsgSNSBuddyInfoSelf
{
	SSNSBuddyInfo buddyInfo;		// �Լ�����Ϣ

	SMsgSNSBuddyInfoSelf()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_USERINFO_ACK
struct SMsgSNSUserInfo_Ack
{
	SSNSUserInfo userInfo;
	SMsgSNSUserInfo_Ack()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_ADD_BUDDY
struct SONMsgSNSReqDirectAddBudy
{
	DWORD dwUserID1;
	DWORD dwUserID2;
	char szActorName1[ACTOR_NAME_MAXSIZE];
	char szActorName2[ACTOR_NAME_MAXSIZE];

	SONMsgSNSReqDirectAddBudy()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_BUDDYINFO_CHANGE_NOTIFY
struct SMsgSNSBuddyInfoChange_Notify
{
	DWORD dwBuddyUserID;		// ���ѵ��û�ID
	int type;					// ������Ϣ��������

	SMsgSNSBuddyInfoChange_Notify()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_USERSTATUS_NOTIFY
struct SMsgSNSUserStatus_Notify
{
	DWORD dwBuddyUserID;		// ���ѵ��û�ID
	int nStatus;				// ��������״̬

	SMsgSNSUserStatus_Notify()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_INVITE_PLAY_GAME_NOTIFY
struct SMsgSNSInvitePlayGame_Notify
{
	int nRoomID;
    DWORD dwInviteActorID;
	char szInviterName[ACTORNAME_MAXSIZE];

	SMsgSNSInvitePlayGame_Notify()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_INVITE_MATCH_TEAM_NOTIFY,		// ֪ͨ����ƥ����ӣ������ͻ��ˣ�
struct SMsgSNSInviteMatchTeam_Notify
{
	DWORD dwTargetActorID;

	SMsgSNSInviteMatchTeam_Notify()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_UPDATE_SCENE			����л��˳���
struct SMsgSNSUpdateScene
{
	DWORD dwUserID;
	DWORD dwWorldID;
	DWORD dwSceneID;

	SMsgSNSUpdateScene()
	{
		memset(this, 0, sizeof(*this));
	}
};


// MSG_SNS_UPDATE_ACTOR_INFO  ��ҽ�ɫ��Ϣ�����ı�
struct SMsgSNSUpdateActorInfo
{
	int nUpdateReson;		// ����ԭ��(ESNSActorInfoUpdateType)
	// buff...

	SMsgSNSUpdateActorInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_SNS_RELATION_CHANGE_NOTIFY ���ѹ�ϵ�ı�֪ͨ
struct SMsgSNSRelationChangeNotify
{
	int nRelation;
	DWORD	dwUserID1;		// ���1ID
	DWORD	dwUserID2;		// ���2ID
	BYTE	nStatus;			// ��ϵ״̬
	char	szRemark[32];		// ���2�ı�ע��
};
///////////////////////////////////////////////////////////////////////////

#pragma warning(pop)
#pragma pack()

#endif //__SNS_DEF_H__
