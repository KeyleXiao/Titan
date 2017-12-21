/*******************************************************************
** �ļ���:	E:\speed\Common\TeamDef.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/15/2015	15��50
** ��  ��:	1.0
** ��  ��:	�����ض���
         
********************************************************************/

#pragma once
#include "PackagePtr.h"
#include "DMirrorServer.h"
#include "NameLenDef.h"

// ���DEBUG��Ϣ
#ifdef VERSEION_DEBUG
#   define TEAM_DEBUG_OUTPUT
#endif
#ifdef TEAM_DEBUG_OUTPUT
#	define	TEAM_TRACELN(x)			TraceLn(x)
#	define	TEAM_WARNINGLN(x)			WarningLn(x)
#	define	TEAM_ERRORLN(x)			ErrorLn(x)
#	define	TEAM_EMPHASISLN(x)			EmphasisLn(x)
#else
#	define	TEAM_TRACELN(x)
#	define	TEAM_WARNINGLN(x)
#	define	TEAM_ERRORLN(x)
#	define	TEAM_EMPHASISLN(x)
#endif

// ��ӡ������Ϣ,����Ϊ����ģʽ
//#define TEAM_PRINT_DEBUG_INF



// ���ͬ������λ�õĶ�ʱ�����
#define TEAM_POSTPLACETIMER_INTERVAL			5000

// tolua_begin

// ��ӷֳɵ������Ч����
#define TEAM_DROPPRIZE_VALIDDISTANCE			128

// ��������󣬶��鲻�����ˣ�����ɢ����
#define TEAMCREATE_ENDLIFE_INTERVAL				300000

#pragma pack(1)



/******************************************************************/
/////////////////// �ŵĹ����ṹ�嶨�� ////////////////////////////
/******************************************************************/


// �ŵĹ��ö��� ( ��ͷ: GROUP )

#define GROUP_MAX_TEAM_COUNTS			100			// �����������
#define GROUP_MAX_GROUP_INDEX			8			// ���������
#define GROUP_SELF_TEAM_INDEX			9			// �Լ��ӵ����
#define GROUP_CUSTOM_TEAM_INDEX			10			// �Զ���ӵ����
#define GROUP_MEMBER_TEAM_INDEX			11			// �Զ����Ա�����
#define GROUP_ALL_TEAM_INDEX			0			// ���жӵ����
#define GROUP_REQUEST_TIMEOUT			60000		// ������� 60000 ���룬Ҳ��Ϊû���ظ����룬��ֹ����bug
#define GROUP_COMMAND_MAX_ID			255			// �������ID
#define GROUP_COMMANDLIST_MAX			8			// �����б���ౣ������
#define GROUP_EFFECTLIST_MAX			8			// ��ʾ�б���ౣ������
#define GROUP_SENDLIST_MAXNUM			1024		// ���н�����������б����������
#define GROUP_CMD_COOL_TIMES			1000		// ʹ���������ȴ 1000 ����
#define GROUP_REQUEST_COOL_TIMES		5000		// ������ȴ 5000 ����
#define GROUP_CHECK_LIFETIME			5000		// ������Ч������ 5000 ����
#define GROUP_MAX_CMDLIFETIMES			1800		// ��������� 1800 ��,30����,����Ϊ0ʱ��
#define GROUP_TARGET_RESID				1709		// λ��ʵ����ͼ������ԴID
#define GROUP_TARGETFLAG_COUNT			10			// λ��ʵ����ͼ�������
#define GROUP_FOLLOW_CHECK_TIMES		1000		// ���涨ʱ�� 1000 ����
#define GROUP_DPSSEND_COOL_TIMES		10000		// ����DPS������ȴ 10000 ����
#define GROUP_DEFAUT_ARROW_CMDID		13			// Ĭ�ϼ�ͷ����ID

// �ƺ���Ч����
#define GROUP_TEAMCAP_TEFFECTID			6			// �ӳ�ͼ��,�ƺ���Ч����ID
#define GROUP_TEAMOTHERCAP_TEFFECTID	40			// �����ӳ�ͼ��,�ƺ���Ч����ID
#define GROUP_TEAMGROUPCAP_TEFFECTID	41			// ���������ӳ�ͼ��,�ƺ���Ч����ID
#define GROUP_GROUPTEAMCAP_TEFFECTID	7			// �ų��ӳ�ͼ��,�ƺ���Ч����ID
#define GROUP_GROUPCAP_TEFFECTID		7			// �ų�ͼ��,�ƺ���Ч����ID
#define GROUP_TEAMER_TEFFECTID			8			// �����Աͼ��,�ƺ���Ч����ID
#define GROUP_GROUPER_TEFFECTID			9			// �ų�Աͼ��,�ƺ���Ч����ID
#define GROUP_NOTEAMER_TEFFECTID		21			// �������ͼ��,�ƺ���Ч����ID
#define GROUP_KILLCOOL_TEFFECTID		22			// ɱ����ȴ,�ƺ���Ч����ID
#define GROUP_PROTECT1_TEFFECTID		23			// �ͼ�����,�ƺ���Ч����ID
#define GROUP_PROTECT2_TEFFECTID		24			// �߼�����,�ƺ���Ч����ID
#define GROUP_SPEAKING_TEFFECTID		42			// ��������,�ƺ���Ч����ID


#define GROUP_FOLLOW_EFFECTID			4005		// �����Ч��Ч����ID





// ��Ч�Ķ���ID
#define INVALID_TEAM_ID						0

// ��Ч����ID
#define INVALID_GROUP_ID					0

// �����Ա������
#define TEAM_MEMBER_MAXCOUNT                5

#define TEAM_MAX_ITEM                       200

// ���в�����Ϣ����,Ϊint����,0��ʾ��,1��ʾ��
enum EMGroupConfigType
{
    // ���ͷ���ID�������GROUP_CONFIG + ���ܺ��壩
    GROUP_CONFIG_INVALID = 0,			// ��ЧID
    GROUP_CONFIG_POPUP,					// ���յ�����ʱ�Ƿ񵯳�
    GROUP_CONFIG_AUTOMOVE,				// ���յ�����ʱ���Զ�����Ŀ��λ��
    GROUP_CONFIG_EFFECT,				// ��ʾ����Ч��
    GROUP_CONFIG_LEFTMENU,				// �����Աͷ������˵�
    GROUP_CONFIG_MAXID,					// ������ID
};

// �����ݷ���
enum EMGroupRequestDataType
{
    // �����ݷ���ID�������GROUP_REQUEST + ���ܺ��壩
    GROUP_REQUESTDATA_BASE = 0,		// �Ż�����Ϣ
    GROUP_REQUESTDATA_TEAM,			// �ŵĳ�Ա������Ϣ
    GROUP_REQUESTDATA_TEAMMEMBER,	// ���ж���Ͷ�Ա��Ϣ
    GROUP_REQUESTDATA_GROUP,		// �ŵ�����Ϣ
    GROUP_REQUESTDATA_TOTAL,		// �ŵ�ͳ����Ϣ
    GROUP_REQUESTDATA_TEAMTOTAL,	// �����ͳ����Ϣ
    GROUP_REQUESTDATA_MAXID,		// ������ID
};


// Ȩ�ޱ�ʶ����
enum EMGroupCommandRightFlag
{
    // Ȩ�ޱ�ʶID�������GROUP_RIGHT + ���ܺ��壩
    GROUP_RIGHT_NONE		= 0x0,			// 0	:��
    GROUP_RIGHT_MEMBER		= 0x1,			// 1	:��Ա��Ȩ��
    GROUP_RIGHT_TEAM		= 0x2,			// 2	:�ӳ���Ȩ��
    GROUP_RIGHT_GROUP		= 0x4,			// 4	:�ų���Ȩ��
};


// ���������ͷ���
enum EMGroupSenderType
{
    // ���������ͷ���ID�������GROUP_SENDTYPE + ���ܺ��壩
    GROUP_SENDTYPE_INVALID = 0,		// ��ЧID
    GROUP_SENDTYPE_MEMBER,			// ��Ա
    GROUP_SENDTYPE_TEAM,			// �ӳ�
    GROUP_SENDTYPE_GROUP,			// �ų�
    GROUP_SENDTYPE_MAXID,			// ���Ŀ�����ID
};


// �����������Ŀ�����
enum EMGroupRequestType
{
    // �������Ŀ�����ID�������GROUP_REQUEST + ���ܺ��壩
    GROUP_REQUEST_PERSON = 0,		// ���ID
    GROUP_REQUEST_GROUP,			// ��ID
    GROUP_REQUEST_TEAM,				// ����ID
    GROUP_REQUEST_MAXID,			// ���Ŀ�����ID
};

// ѡ������,Ŀ������,0.��ѡĿ����Ŀ���,1.ѡĿ��,2.ѡĿ���,3.Ŀ��Ϊ����
enum EMGroupSelectType
{
    // ѡ������ID�������GROUP_SELECT + ���ܺ��壩
    GROUP_SELECT_INVALID = -1,		// ��ЧID
    GROUP_SELECT_NO = 0,			// ��ѡĿ����Ŀ���
    GROUP_SELECT_ENTITY,			// ѡ��ʵ��
    GROUP_SELECT_TILE,				// ѡĿ���TILE
    GROUP_SELECT_MYSELF,			// �Լ�
    GROUP_SELECT_MAXID,				// ���Ŀ�����ID
};

// ����ͳ�ƴ�������
enum EMGroupSendTotalType
{
    // ����ͳ�ƴ�������ID�������GROUP_SENDTOTAL + ���ܺ��壩 ����,0,ɱ��,1:ɱ��,2:ɱ�ؾ�
    GROUP_SENDTOTAL_MONSTER = 0,	// ɱ��
    GROUP_SENDTOTAL_PERSON,			// ɱ��
    GROUP_SENDTOTAL_TANK,			// ɱ�ؾ�
    GROUP_SENDTOTAL_DAMAGE,			// ���˺���
    GROUP_SENDTOTAL_HDAMAGE,		// ����˺�
    GROUP_SENDTOTAL_CURE,			// ��������
    GROUP_SENDTOTAL_HCURE,			// �������
    GROUP_SENDTOTAL_MAXID,			// ���ʽID
};

// ���ͳ����������
enum EMGroupTotalType
{
    // ���ͳ����������ID�������GROUP_TOTAL + ���ܺ��壩
    GROUP_TOTAL_DEAD = 0,		// ������
    GROUP_TOTAL_PERSON,			// ɱ����
    GROUP_TOTAL_MONSTER,		// ɱ����
    GROUP_TOTAL_DAMAGE,			// ���˺���
    GROUP_TOTAL_HDAMAGE,		// ����˺�
    GROUP_TOTAL_CURE,			// ��������
    GROUP_TOTAL_HCURE,			// �������
    GROUP_TOTAL_MAXID,			// ���ʽID
};

// ͳ��λ��ʶ����
enum EMGroupTotalInfoFlag
{
    // ͳ��λ��ʶ�������GROUP_TOTALFLAG + ���ܺ��壩
    GROUP_TOTALFLAG_NONE		= 0x0,			// 0	:��
    GROUP_TOTALFLAG_ONLINE		= 0x1,			// 1	:����
    GROUP_TOTALFLAG_INGROUP		= 0x2,			// 2	:������
    //GROUP_TOTALFLAG_GROUP		= 0x4,			// 4	:
    //GROUP_TOTALFLAG_GROUP		= 0x8,			// 8	:
};

// ͳ��λ��ʶĬ��
#define GROUP_TOTALFLAG_DEFAULTFLAG		3

// ����������ų�Աλ����Ϣ��ʶ
enum EMGroupRequestMapLocType
{
    // λ��ʶ�������GROUP_MAPLOCTYPE + ���ܺ��壩
    GROUP_MAPLOCTYPE_NONE			= 0x0,			// 0	:��
    GROUP_MAPLOCTYPE_GROUP			= 0x1,			// 1	:���ų�Ա,����������
    GROUP_MAPLOCTYPE_MYTEMA			= 0x2,			// 2	:���ӳ�Ա
    GROUP_MAPLOCTYPE_ALLMAP			= 0x4,			// 4	:�������е�ͼ,����ֻ�Ǳ���ͼ
    GROUP_MAPLOCTYPE_MYNATION		= 0x8,			// 8	:������������
    GROUP_MAPLOCTYPE_FRIENDNATION	= 0x10,			// 16	:�˹���������
    GROUP_MAPLOCTYPE_ENEMYNATION	= 0x20,			// 32	:�й���������
    //GROUP_MAPLOCTYPE_UNUSE3		= 0x40,			// 64	:δ��3
};




// ������������־,λ��ʶ,�������������
enum EMGroupFollowBreakFlag
{
    // ������������־�������FOLLOWBREAKFLAG + ���ܺ��壩
    FOLLOWBREAKFLAG_NONE			= 0x0,			// 0	:��
    FOLLOWBREAKFLAG_USESKILL		= 0x1,			// 1	:ʹ�ü���
    FOLLOWBREAKFLAG_DIE				= 0x2,			// 2	:����
    FOLLOWBREAKFLAG_RIDE			= 0x4,			// 4	:����
    FOLLOWBREAKFLAG_DRIVER			= 0x8,			// 8	:���ؾ�
    FOLLOWBREAKFLAG_STARTMOVE		= 0x10,			// 16	:����
    FOLLOWBREAKFLAG_CHANGELOC		= 0x20,			// 32	:����
    FOLLOWBREAKFLAG_ADDBUFF			= 0x40,			// 64	:��������״̬������ѣ��
    FOLLOWBREAKFLAG_RIDESTEED		= 0x80,			// 128	:��ս��
    //	FOLLOWBREAKFLAG_NOUSE3			= 0x100,		// 256	:δ��
    //	FOLLOWBREAKFLAG_NOUSE4			= 0x200,		// 512	:δ��
    //	FOLLOWBREAKFLAG_NOUSE5			= 0x400,		// 1024	:δ��
    //	FOLLOWBREAKFLAG_NOUSE6			= 0x800,		// 2048	:δ��
    //	FOLLOWBREAKFLAG_NOUSE7			= 0x1000,		// 4096	:δ��
    //	FOLLOWBREAKFLAG_NOUSE8			= 0x2000,		// 8192	:δ��
    //	FOLLOWBREAKFLAG_NOUSE9			= 0x4000,		// 16384:δ��
    //	FOLLOWBREAKFLAG_NOUSE10			= 0x8000,		// 32768:δ��
    //	FOLLOWBREAKFLAG_NOUSE11			= 0x10000,		// 65536:δ��

};

// ������ԭ���־
enum EMGroupFollowBreakReason
{
    // ������ԭ�������FOLLOWBREAKREASON + ���ܺ��壩
    FOLLOWBREAKREASON_CANCEL	= 0,	// ���ȡ��
    FOLLOWBREAKREASON_USESKILL,			// ʹ�ü���
    FOLLOWBREAKREASON_DIE,				// ����
    FOLLOWBREAKREASON_RIDE,				// ����
    FOLLOWBREAKREASON_DRIVER,			// ���ؾ�
    FOLLOWBREAKREASON_STARTMOVE,		// ����
    FOLLOWBREAKREASON_CHANGELOC,		// ����
    FOLLOWBREAKREASON_ADDBUFF,			// ���쳣��״̬Ӱ�죬����Ρ����š�����
    FOLLOWBREAKREASON_LOGOUT,			// �������
    FOLLOWBREAKREASON_RIDESTEED,		// ��ս��
    FOLLOWBREAKREASON_OTHER,			// ����
};



enum
{
    // �����޶��飬�Է��޶�����������
    EJoinFlowType_Request_One = 0,

    // �����޶��飬�Է��ж�����������
    EJoinFlowType_Request_Two,

    // �����޶��飬�Է��޶�����������
    EJoinFlowType_Invited_Three,

    // �����ж��飬�Է��޶�����������
    EJoinFlowType_Invited_Four
};

enum
{
    // �뿪����ԭ�������˳�����
    ELeaveTeamReason_Leave = 0,

    // �뿪����ԭ�򣺱��ӳ�����
    ELeaveTeamReason_Kick,

    // �뿪����ԭ����������
    ELeaveTeamReason_Logout,

    // �뿪����ԭ�򣺶����ɢ
    ELeaveTeamReason_Disband,

    // �뿪����ԭ�򣺶��ѿ��ȥ��
    ELeaveTeamReason_StarTrek,

    // �뿪����ԭ��ϵͳǿ��
    ELeaveTeamReason_SysKick,
};

enum
{
    // �ӳ�ת��ԭ�򣺶��鴴��֮��
    ECaptainShiftReason_Init = 0,

    // �ӳ�ת��ԭ�������˳�����
    ECaptainShiftReason_Leave,

    // �ӳ�ת��ԭ����������
    ECaptainShiftReason_Logout,

    // �ӳ�ת��ԭ�򣺱�����
    ECaptainShiftReason_Appoint,

    // �ӳ�ת��ԭ�򣺵����˳�
    ECaptainShiftReason_OffLine,
};



enum
{
    tTeamSetting_Flag_ForbidKick = 0x0001,		// ��ֹ����
    tTeamSetting_Flag_ForbidCaptain = 0x0002,	// ���öӳ�ʰȡ
    tTeamSetting_Flag_ForbidFree = 0x0004,		// ��������ʰȡ
    tTeamSetting_Flag_ForbidTurn = 0x0008,		// ��������ʰȡ	
};

enum EMGroupConveneCostType
{
    GroupConveneCostType_None = 0,		// ��Ч
    GroupConveneCostType_1,				// ��ֹ����
    GroupConveneCostType_2,				// ���öӳ�ʰȡ
    GroupConveneCostType_3,				// ��������ʰȡ
    GroupConveneCostType_Max,			// ��������ʰȡ	
};

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// �������¼����ͻ����¼�����
enum EMGroupEventType
{
    // �¼�����ID�������GROUP_EVENT + ���ܺ��壩
    GROUP_EVENT_INVALID = 0,	// ��Ч��ʽID
    GROUP_EVENT_UPDATE,			// �����Ż�������
    GROUP_EVENT_UPDATETEAM,		// ���¶����������
    GROUP_EVENT_TEAMINFO,		// �����Ŷ�����Ϣ����
    GROUP_EVENT_TEAMMEMBERINFO,	// �������ж���Ͷ�Ա��Ϣ����
    GROUP_EVENT_GROUPINFO,		// �����ŵ�����Ϣ����
    GROUP_EVENT_TOTALINFO,		// ������ͳ����Ϣ����
    GROUP_EVENT_TEAMTOTAL,		// ���¶�ͳ����Ϣ����
    GROUP_EVENT_ENTER,			// ������
    GROUP_EVENT_EXIT,			// �˳���
    GROUP_EVENT_CAPTAIN,		// ���ų�
    GROUP_EVENT_CONFIRM,		// Ҫ��ȷ��
    GROUP_EVENT_CMD,			// �յ�����
    GROUP_EVENT_ADDDIDA,		// ���͵��ͻ������Dida
    GROUP_EVENT_CONVENERESPONSE,// ���е��ټ���Ϣ

    // ������������ 
    GROUP_EVENT_SERVER,			// ������������
    GROUP_EVENT_ADD,			// ������
    GROUP_EVENT_REMOVE,			// ���Ƴ�
    GROUP_EVENT_SENDINFO,		// ��������
    GROUP_EVENT_CHANGECAPTAIN,	// �����ų�
    GROUP_EVENT_INITGROUP,		// ��ʼ��
    GROUP_EVENT_UPDATESVR,		// �����Ż�������
    GROUP_EVENT_CLEARTOTAL,		// ���ͳ������
    GROUP_EVENT_CHANGEPICKMODE,	// �޸�ʰȡģʽ
    GROUP_EVNET_COSTMONEY,		// ��Ǯ�¼�

    GROUP_EVENT_MAXID,			// ���ʽID
};


// �ͻ��˷������¼����������¼�����
enum EMGroupClientEventType
{
    // �¼�����ID�������GROUP_CLIENT + ���ܺ��壩
    GROUP_CLIENT_INVALID = 0,	// ��Ч��ʽID
    GROUP_CLIENT_ENTER,			// �������ָ����
    GROUP_CLIENT_EXIT,			// �����˳���ǰ��
    GROUP_CLIENT_ENTER1,		// �������ָ����
    GROUP_CLIENT_EXIT1,			// �����˳���ǰ��
    GROUP_CLIENT_ADD,			// ����С�ӵ�ָ����
    GROUP_CLIENT_REMOVE,		// ��С�Ӵ�ָ�����Ƴ�
    GROUP_CLIENT_CON_ENTER,		// �ų�ȷ���������
    GROUP_CLIENT_CON_ENTER1,	// �ӳ�ȷ���������
    GROUP_CLIENT_CHANGECAPTAIN,	// �����ų�
    GROUP_CLIENT_CMD,			// ʹ��ָ������
    GROUP_CLIENT_KILLSELF,		// ��ɢ��
    GROUP_CLIENT_REQUESTDATA,	// ��������
    GROUP_CLIENT_SETTOTAL,		// �趨ͳ�Ʋ���
    GROUP_CLIENT_CHANGETEAM,	// ��������
    GROUP_CLIENT_CHANGEPICKMODE,// ����ʰȡģʽ
    GROUP_CLIENT_SETLEVELLIST,	// ���õȼ�����
    GROUP_CLIENT_STARTCONVENE,	// ��ʼ���ټ�
    GROUP_CLIENT_PERSONENTER,	// ������Ӧ���ټ�
    GROUP_CLIENT_CONVENEQUERY,	// ��ȡ�ټ���Ϣ
    GROUP_CLIENT_CHANGEACTMODE,	// ��������ģʽ
    GROUP_CLIENT_MAXID,			// ���ʽID
};


// �������������¼�����
enum EMGroupSvrEventType
{
    // �¼�����ID�������GROUP_SVR + ���ܺ��壩
    GROUP_SVR_INVALID = 0,	// ��Ч��ʽID
    GROUP_SVR_ADD,			// ������
    GROUP_SVR_CONVENE,		// ���������͵��������ټ�
    GROUP_SVR_MAXID,		// ���ʽID
};

// tolua_begin

// ��ʰȡģʽ
enum EMGroupPickMode
{
    GROUP_PICKMODE_CAPTION = 0,		// �ų�ʰȡ
    GROUP_PICKMODE_DICE,			// �Ŷӷ���
    GROUP_PICKMODE_AUCTION,			// ���ž���
};

// ����ģʽ
enum EMGroupActionMode
{
    GROUP_ENTER_GROUP = 0,		// �������
    GROUP_BAN_GROUP,			// ��ֹ����
    GROUP_ACTION_MAX,			// ���ֵ
};


// �Ŷ��ټ���ѯ����
enum EMGroupConvenQueryType
{
    GROUP_CONVENE_INVALID,			// ��Ч���ټ���ѯ����
    GROUP_CONVENE_RESPONSE_INFO,	// ���е��Ŷ��ټ���Ϣ
    GROUP_CONVENE_SHOW_INFO,		// ��ѯ�����ټ�����
    GROUP_CONVENE_MAX,				// ����ѯ����
};



// �����ṹ�嶨��




/******************************************************************/
///////////////////// ��ӵĹ����ṹ�嶨�� /////////////////////////
/******************************************************************/
////////////////////////////////////////////////////////////////////

struct STeamMemberInfo
{
    DWORD		clientID;					// �ͻ���

    DWORD		dwPDBID;					// PDBID
    char		szName[GAME_NAME_MAXSIZE];	// ����	
    int			nFaceID;					// ͷ��ID
    int			nLevel;						// ��ǰ�ȼ�

    int			nSex;						// �Ա�
    int			nVocation;					// ְҵ
    int			nFromGameWorldID;			// ������Ϸ����ID
    int         nServerID;                  // ������ID

    DWORD		nSceneID;					// ����ID
    int			nMapID;						// ��ͼID

    bool		bCaptainFlag;				// �Ƿ�ӳ�
    int			nState;						// ״̬(0:���ߣ�1:����)

    bool		bDieState;					// �Ƿ�����״̬

    POINT		ptTile;						// ��ǰλ��
    POINT		ptMoveTarget;				// �ƶ�Ŀ���

    LONGLONG	uid;						// uid	

    int			nIndex;						// �������

    int			nBattle;					// ս��ֵ

    bool		bSpeaking;					// ������˵��

	DWORD		dwUserId;					// userId

	BYTE        byMatchType;                 // ��λ��λ��Ϣ ����nMatchTypeID = 8  
	int         nRankScore;
    int         nRankGrade;
    STeamMemberInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};
typedef list<STeamMemberInfo>	TLIST_MEMBERINFO;

struct SDiceModeInfo
{
    int			nEquipBaseLevel;			// װ�����ٵ�
    bool		bForgetWhite;				// �Ƿ��ų���װ
    int			nElsePickMode;				// ���ʰȡģʽ

    SDiceModeInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };

    void Clean(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// ���ǵ�ǰ��ͼ��Ϣ
struct SGroupHeroCurrentMapInfo
{
    DWORD	dwMapID;			// ���ڵ�ͼID
    DWORD	dwZoneID;			// ���ڳ���ID
    POINT	ptTile;				// λ��

    SGroupHeroCurrentMapInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// �ų�Ա��Ϣ
struct SGroupMemberInfo
{
    int		nTeamID;					// ����ID
    DWORD	dwTime;						// ����ʱ��

    SGroupMemberInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// �ͻ����ų�Ա��Ϣ
struct SClientGroupMemberInfo
{
    int		nTeamID;					// ����ID
    DWORD	dwCaptain;					// �ų�ID
    DWORD	dwTime;						// ����ʱ��
    BYTE	nCount;						// ��Ա��
    char	szName[32];					// ����,�õ��ǾͶӳ���

    SClientGroupMemberInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// �ͻ������ж����Ա��Ϣ
struct SClientGroupTeamMemberInfo
{
    int			nTeamID;					// ����ID
    DWORD		clientID;					// �ͻ���
    DWORD		dwPDBID;					// PDBID
    char		szName[GAME_NAME_MAXSIZE];	// ����	
    int			nFaceID;					// ͷ��ID
    int			nLevel;						// ��ǰ�ȼ�

    int			nSex;						// �Ա�
    int			nVocation;					// ְҵ
    int			nNation;					// ����

    DWORD		nZoneID;					// ����ID
    int			nMapID;						// ��ͼID
    POINT		ptTile;						// λ��
    POINT		ptMove;						// �ƶ���Ŀ���

    bool		bCaptainFlag;				// �Ƿ�ӳ�
    int			nState;						// ״̬(0:���ߣ�1:����)

    bool		bDieState;					// �Ƿ�����״̬

    int			nIndex;						// �������

    int			nDataFlag;					// ͬ������ 0.Ҫɾ��,1.����,2.�¼�

    SClientGroupTeamMemberInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// �ͻ�������λ����Ϣ
struct SClientGroupPersonMapLocInfo
{
    DWORD		dwPDBID;					// PDBID
    char		szName[GAME_NAME_MAXSIZE];	// ����	
    int			nCountryID;					// ����
    int			nRelation;					// ���ҹ�ϵ,0:����,1:�˹�,2:�й�

    bool		bCaptainFlag;				// �Ƿ�ӳ�
    bool		bInGroup;					// �Ƿ�������

    DWORD		nZoneID;					// ����ID
    int			nMapID;						// ��ͼID
    POINT		ptTile;						// λ��
    POINT		ptMove;						// �ƶ���Ŀ���

    bool		bOnTracek;					// �Ƿ�̬�ƶ�
    int			nDataFlag;					// ͬ������ 0.Ҫɾ��,1.����,2.�¼�


    SClientGroupPersonMapLocInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SClientGroupPersonMapLocNode
{
    SClientGroupPersonMapLocInfo locData;	// ����λ����Ϣ

    list<POINT>	listMoveTrack;				// ·��
};


/// ʹ������ʱ��������
struct SGroupCommandContext
{
    DWORD	dwMapID;			// ʹ������Ĳ��������ڵ�ͼID
    DWORD	dwZoneID;			// ʹ������Ĳ��������ڳ���ID
    DWORD	dwSender;			// ʹ������Ĳ�����PDBID
    BYTE	nSenderSex;			// ʹ������Ĳ������Ա�
    BYTE	nGroupIndex;		// �������������
    DWORD	dwTeamID;			// �����������ID
    DWORD	dwCommandID;		// ����ID
    POINT	ptTarget;			// λ��
    UID		TargetUID;			// ʵ�����uid
    BYTE	nSendType;			// ���������� EMGroupSenderType
    char	szCommandName[64];	// �Զ�����������
    char	szTargetName[32];	// Ŀ����
    char	szPlayerName[32];	// �����߽�ɫ��
    DWORD	dwTime;				// ʱ��
    DWORD	dwSnIndex;			// ���,�������б��в���ָ����������

    SGroupCommandContext()
    { 
        memset(this, 0, sizeof(*this));
    };
    void clear(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

/// ʹ������ʱ��������
struct SMsgGroupCommandContext
{
    DWORD	dwMapID;		// ʹ������Ĳ��������ڵ�ͼID
    DWORD	dwZoneID;		// ʹ������Ĳ��������ڳ���ID
    DWORD	dwSender;		// ʹ������Ĳ�����PDBID
    BYTE	nSenderSex;		// ʹ������Ĳ������Ա�
    BYTE	nGroupIndex;	// �������������,ΪGROUP_CUSTOM_TEAM_INDEX��ʾ�Զ������,dwTeamIDΪ����ID,ΪGROUP_MEMBER_TEAM_INDEX��ʾ�Զ����Ա,dwTeamIDΪ��ԱPDBID
    DWORD	dwTeamID;		// �����������ID
    UID		uidTarget;		// ʹ������ʱ�������λ���ϵ�ʵ�����
    POINT	ptTargetTile;	// ʹ������ʱ�������λ��
    DWORD	dwCommandID;	// ����ID 1~255
    BYTE	nSendType;		// ���������� EMGroupSenderType
    BYTE	nStrLens;		// ��ʾ�ִ�����
    BYTE	nTargetNameLens;// Ŀ��������
    BYTE	nNameLens;		// �����߽�ɫ������
    // char * szTitleMsg;	// ��ʾ�ִ�

    SMsgGroupCommandContext()
    { 
        memset(this, 0, sizeof(*this));
    }
};

// ��ǰ��ʾ������������
struct SClientCommandDrawInfo
{
    DWORD dwZoneID;						// ʹ������Ĳ��������ڳ���ID
    DWORD dwCommandID;					// ����ID,Ϊ0��ʾû��
    UID HeroUID;						// ����UID
    UID TargetUID;						// Ŀ��UID
    UID PositionUID;					// λ��UID
    BYTE nTargetType;					// λ������
    POINT ptTargetTile;					// λ��
    ulong lArrowEffectID;				// �������ϵļ�ͷЧ��ID
    ulong lHeroEffectID;				// �������ϵĸ���Ч��ID
    int	nSoundID;						// ЧID,0Ϊ��
    ulong lTargetPosEffectID;			// Ŀ�����ϵ�Ч��ID,��Ŀ��ʱ����
    ulong lTargetEffectID;				// Ŀ�����ϵĸ���Ч��ID,��Ŀ��ʱ����
    int nTopTipEffectID;				// ͷ���ƺ�Ч��ID
    DWORD dwTimeOut;					// ����ʱ��
    bool bFirstShowed;					// ��һ���Ѿ���ʾ
    char szCommandTip[32];				// ͷ���ƺ�

    SClientCommandDrawInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
    void clear(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// ��ǰ������ʾ�����������
struct SClientHeroDrawInfo
{
    ulong lArrowEffectID;				// �������ϵļ�ͷЧ��ID
    DWORD dwArrowTimeOut;				// ��ͷЧ������ʱ��
    ulong lHeroEffectID;				// �������ϵĸ���Ч��ID
    DWORD dwHeroTimeOut;				// ����Ч������ʱ��

    SClientHeroDrawInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
    void clear(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// �ͻ������ͳ��������Ϣ
struct SClientGroupTotalInfo
{
    DWORD	dwPlayerID;					// ��ɫID
    DWORD	dwTime;						// ����ʱ��
    DWORD	dwTotal[GROUP_TOTAL_MAXID];	// ���ͳ������,�ο� EMGroupTotalType
    BYTE	nLevel;						// �û��ȼ�
    BYTE	nNation;					// �û����ڹ���id
    BYTE	nProfession;				// �û�ְҵ
    BYTE	nSex;						// �û��Ա�
    BYTE	nOnline;					// ����  �ο� EMGroupTotalInfoFlag ���
    BYTE	nStrLens;					// �����ִ�����
    //char	szPlayerName[32];			// ��ɫ��

    SClientGroupTotalInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// ���ͳ��������Ϣ
struct SGroupTotalInfo
{
    DWORD	dwPlayerID;					// ��ɫID
    DWORD	dwTime;						// ����ʱ��
    DWORD	dwTotal[GROUP_TOTAL_MAXID];	// ���ͳ������,�ο� EMGroupTotalType
    BYTE	nLevel;						// �û��ȼ�
    BYTE	nNation;					// �û����ڹ���id
    BYTE	nProfession;				// �û�ְҵ
    BYTE	nSex;						// �û��Ա�
    BYTE	nOnline;					// ����  �ο� EMGroupTotalInfoFlag ���
    char	szPlayerName[GAME_NAME_MAXSIZE];			// ��ɫ��

    SGroupTotalInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// ���ͳ��������ϢLua��
struct SLuaGroupTotalInfo
{
    DWORD	dwPlayerID;					// ��ɫID
    DWORD	dwTime;						// ����ʱ��
    DWORD	dwTotalDead;				// ������
    DWORD	dwTotalPerson;				// ɱ����
    DWORD	dwTotalMonster;				// ɱ����
    DWORD	dwTotalDamage;				// �ۻ����˺���
    DWORD	dwMaxDamage;				// ����˺�
    DWORD	dwTotalCure;				// �ۻ���������	
    DWORD	dwMaxCure;					// �������
    BYTE	nLevel;						// �û��ȼ�
    BYTE	nNation;					// �û����ڹ���id
    BYTE	nProfession;				// �û�ְҵ
    BYTE	nSex;						// �û��Ա�
    BYTE	nOnline;					// ����  �ο� EMGroupTotalInfoFlag ���
    char	szPlayerName[GAME_NAME_MAXSIZE];			// ��ɫ��

    SLuaGroupTotalInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// �����Ŀ����Ϣ
struct SClientFollowTargetInfo
{
    LONGLONG uidTarget;			// Ŀ��UID
    BYTE	nFollowType;		// �������� 0:��ͨ���� 1:��������
    DWORD	dwMapID;			// ���ڵ�ͼID
    DWORD	dwZoneID;			// ���ڳ���ID
    POINT	ptTarget;			// λ��
    bool	bClientType;		// �ͻ��˷�ʽ����

    SClientFollowTargetInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
    void clear(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

struct STeamSyncPersonContext : public SMirrorPersonContext
{		
    int			nAttackPower;				// ��������
    int			nDefendPower;				// ��������	

    STeamSyncPersonContext(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SLuaInviteDialogMember
{
    char		szName[GAME_NAME_MAXSIZE];	// ����

    DWORD		dwPDBID;					// PDBID

    bool		bOnline;					// ���߷�

    int			nSex;						// �Ա�
    int			nVocation;					// ְҵ
    int			nLevel;						// �ȼ�
    int			nNation;					// ����		

    bool		bCaptain;					// �ӳ�

    char		szMapName[64];				// ��ͼ��

    int			nAttackPower;				// ��������
    int			nDefendPower;				// ��������

    bool		bSameKin;					// �Ƿ�ͬս��
    bool		bSameClan;					// �Ƿ�ͬ���

    SLuaInviteDialogMember(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// ͳ����Ϣ,�����IDΪ�ؼ���
typedef map<DWORD,SGroupTotalInfo> TMAP_GroupTotalInfo;






//////////////////////////////////////////////////////////////////////////
// ������Ϣ
struct TempTeamMemberInfo
{
    DWORD		dwPDBID;					// �û�DBID
    char		szName[GAME_NAME_MAXSIZE];	// �û�������
    uchar		nSex;						// �û��Ա�
    uchar		nNation;					// �û����ڹ���id
    uchar		nVocation;					// �û�ְҵ
    bool		bCaptain;					// �Ƿ��Ƕӳ�
    ulong		nLevel;						// �û��ȼ�
    int			nAttackPower;				// ��������
    int			nDefendPower;				// ��������
    int         nMapID;						// �û���ǰ�ĵ�ͼID
    TempTeamMemberInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// ȷ����Ϣ����
struct SMsgTeamClientConfirmInfo
{
    BYTE	nType;			// ����
    DWORD	dwGroupID;		// ��ID
    DWORD	dwPlayerID;		// ��������
    DWORD	dwSpare;		// ����

    BYTE	nStrLens;		// ��ʾ�ִ�����
    //char	szTitleMsg[128];	// ��ʾ�ִ�
    BYTE	nTeamSize;		// �������
    //TempTeamMemberInfo tTeamL[TEAM_MEMBER_MAXCOUNT]; // �������

    SMsgTeamClientConfirmInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// ȷ�Ͻ����Ϣ����
struct SMsgTeamClientConfirmResult
{
    BYTE	nType;			// ����
    DWORD	dwGroupID;		// ��ID
    DWORD	dwPlayerID;		// ��������
    DWORD	dwSpare;		// ����
    BYTE	nResult;		// ���	 0:ȡ��,1:ȷ��

    SMsgTeamClientConfirmResult(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// ���ټ�������
enum EMGroupConveneType
{
    GROUP_SELF_CONVENE,			// �����ټ�
    GROUP_COUNTRY_CONVENE,		// �����ټ�
};

// ��ʼ���ټ���ƥ���ŵ���Ϣ
struct SMsgTeamClientStartConvene
{
    EMGroupConveneType emConveneType;		// �ټ�����
    int		nLevel;							// �ټ��ȼ�����
    int		nCount;							// �ټ���������
    char	szDescr[GAME_TITLE_MAXSIZE];	// ������

    SMsgTeamClientStartConvene(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// ����Ϣ
struct SMsgTeamClientAllConvene
{
    DWORD	dwGroupNum;						// �Ÿ���
    SMsgTeamClientAllConvene(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// tolua_begin

// ����Ϣ
struct SMsgTeamClientGroupConvene
{
    DWORD	dwGroupID;						// ��ID
    int		nGroupCount;					// ������
    DWORD	dwLevelLimit;					// �ŵȼ�����
    char	szGroupName[GAME_NAME_MAXSIZE];	// �ټ�����
    char	szDescr[GAME_MSGSTR_MAXSIZE];	// ������

    SMsgTeamClientGroupConvene(void)
    {
        memset(this, 0, sizeof(*this));
    };
    bool operator < (const SMsgTeamClientGroupConvene &mNode) const
    {
        if(this->nGroupCount == mNode.nGroupCount)
            return (this->dwGroupID < mNode.dwGroupID);
        return (this->nGroupCount < mNode.nGroupCount);
    };	
};

// ����Ϣ
struct SMsgTeamClienConveneShowInfo
{
    DWORD	dwCountryNum;						// �����ټ�����
    SMsgTeamClienConveneShowInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// GROUP_CLIENT_ADD,GROUP_CLIENT_REMOVE
// �������Ϣ����
struct SMsgTeamClientGroupInfo
{
    DWORD	dwTeamID;		// С��TeamID
    BYTE	nGroupIndex;	// �����(0~9)

    SMsgTeamClientGroupInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};


// GROUP_EVENT_UPDATE ����������
// ������������Ϣ�ṹ
struct SMsgTeamClientUpdateBaseInfo
{
    DWORD	dwGroupID;			// ��ID
    DWORD	dwCaptain;			// �ų�PDBID
    bool	bGetTotal;			// �Ƿ�ͳ��
    BYTE	nStrLens;			// �����Ƴ���
    DWORD   dwPickMode;			// �ŷ���ģʽ
    DWORD	dwActionMode;		// ����ģʽ
    DWORD	dwLevelLimit;		// �Ŷӵȼ�����
    //char	szGroupName[32];	// ������

    SMsgTeamClientUpdateBaseInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// GROUP_EVENT_UPDATETEAM  ���¶����������
struct SMsgTeamClientUpdateTeamBaseInfo
{
    DWORD	dwTeamID;			// ����D
    DWORD	dwCaptain;			// �ӳ�PDBID
    bool	bGetTotal;			// �Ƿ�ͳ��

    SMsgTeamClientUpdateTeamBaseInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// GROUP_EVENT_TEAMINFO �����Ŷ�����Ϣ����

// �ͻ����ų�Ա��Ϣ
struct SMsgTeamClientTeamInfoNode
{
    int		nTeamID;					// ����ID
    DWORD	dwCaptain;					// �ų�ID
    BYTE	nCount;						// ��Ա��
    BYTE	nStrLens;					// �����ִ�����
    //char	szName[32];					// ����,�õ��ǾͶӳ���

    SMsgTeamClientTeamInfoNode(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// �ͻ��˶����Ա��Ϣ
struct SMsgTeamClientTeamInfoMemberNode
{
    DWORD	dwPlayerID;					// ��ҽ�ɫID
    char	szName[GAME_NAME_MAXSIZE];					// ��ɫ��

    SMsgTeamClientTeamInfoMemberNode(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// �����Ŷ�����Ϣ���ݽṹ
struct SMsgTeamClientUpdateTeamInfo
{
    BYTE	nTeamCount;			// �������
    // nTeamCount * SMsgTeamClientTeamInfoNode 
    // nTeamCount * char * szName 

    SMsgTeamClientUpdateTeamInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};


struct SMsgTeamClientMemberNode
{
    int			nTeamID;					// ����ID
    DWORD		dwPDBID;					// PDBID
    BYTE		nFaceID;					// ͷ��ID
    BYTE		nLevel;						// ��ǰ�ȼ�
    BYTE		nNation:4;					// ����
    BYTE		nVocation:4;				// ְҵ
    BYTE		nSex:4;						// �Ա�
    bool		bCaptainFlag:2;				// �Ƿ�ӳ�
    BYTE		nState:2;					// ״̬(0:���ߣ�1:����)
    BYTE		nIndex;						// �������
    BYTE		nStrLens;					// ���ִ�����

    SMsgTeamClientMemberNode(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// �������ж���Ͷ�Ա��Ϣ���ݽṹ
struct SMsgTeamClientUpdateTeamMemberInfo
{
    int	nCount;			// ����
    // nCount * SMsgTeamClientMemberNode 
    // nCount * char * szName 

    SMsgTeamClientUpdateTeamMemberInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// GROUP_EVENT_GROUPINFO �����ŵ�����Ϣ����
// ������Ϣ�ṹ
struct SMsgTeamClientUpdateGroupInfo
{
    BYTE	nTeamCount[GROUP_MAX_GROUP_INDEX];			// �������
    // nTeamCount * int nTeamID 

    SMsgTeamClientUpdateGroupInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// GROUP_EVENT_TOTALINFO ������ͳ����Ϣ����
// ������Ϣ�ṹ
struct SMsgTeamClientUpdateTotalInfo
{
    DWORD	dwCount;			// ����
    // dwCount * SClientGroupTotalInfo
    // dwCount * char* szPlayerName

    SMsgTeamClientUpdateTotalInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// GROUP_EVENT_TEAMTOTAL ���¶�ͳ����Ϣ����
// ������Ϣ�ṹ
struct SMsgTeamClientUpdateTeamTotalInfo
{
    DWORD	dwCount;			// ����
    // dwCount * SClientGroupTotalInfo
    // dwCount * char* szPlayerName

    SMsgTeamClientUpdateTeamTotalInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

//	GROUP_EVENT_CHANGECAPTAIN,	�����ų�
struct SMsgTeamServerCaptainInfo
{
    DWORD	dwPlayerID;					// �ų���ҽ�ɫID

    SMsgTeamServerCaptainInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

//	GROUP_EVENT_CHANGEPICKMODE,	����ʰȡģʽ
struct SMsgTeamServerPickModeInfo
{
    DWORD	dwPickMode;					// ʰȡģʽ

    SMsgTeamServerPickModeInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// GROUP_EVENT_SENDINFO 	 ��������
struct SMsgTeamServerUpdateTeamInfo
{
    BYTE	nTeamCount;			// �������
    // nTeamCount * int nTeamID 

    SMsgTeamServerUpdateTeamInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

//	GROUP_CLIENT_SETTOTAL  �趨ͳ�Ʋ���
struct SMsgTeamClientSetTotalConfig
{
    BYTE	nType;					// ��������,0:����ͳ��,1:��ͳ��
    BYTE	nFlag;					// ״̬ 0:��ͳ��,1:ͳ��,3:���

    SMsgTeamClientSetTotalConfig(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

//	GROUP_CLIENT_CHANGETEAM  ��������
struct SMsgTeamClientChangePlayerTeam
{
    DWORD	dwPlayerID;					// ��ҽ�ɫID
    DWORD	dwTeamID;					// �����ɶӵ�TeamID
    DWORD	dwOpType;					// �������� 0:�����,1:����,2:���,3:���ӳ�

    SMsgTeamClientChangePlayerTeam(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// ��Ϣ
#include "TeamMsgDef.h"