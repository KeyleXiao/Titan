/*******************************************************************
** �ļ���:	E:\speed\Common\KinDef.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/15/2015	15��50
** ��  ��:	1.0
** ��  ��:	
            ���а���ض���
********************************************************************/

#pragma once
#include "PackagePtr.h"
#include "NameLenDef.h"
#include "NetMessageDef.h"
#include "IGameDatabaseDef.h"

//���DEBUG��Ϣ
#define KIN_DEBUG_OUTPUT
#ifdef KIN_DEBUG_OUTPUT
#	define	KIN_TRACELN(x)			TraceLn(x)
#	define	KIN_WARNINGLN(x)			WarningLn(x)
#	define	KIN_ERRORLN(x)			ErrorLn(x)
#	define	KIN_EMPHASISLN(x)			EmphasisLn(x)
#else
#	define	KIN_TRACELN(x)
#	define	KIN_WARNINGLN(x)
#	define	KIN_ERRORLN(x)
#	define	KIN_EMPHASISLN(x)
#endif

#pragma pack(1)
// ���峣���ű�
#define KIN_CONFIG_FILENAME			"Scp\\KinConfig.csv"

// ���������ű�
#define KIN_LEVEL_FILENAME			"Scp\\KinLevel.csv"

//============================= ����ϵͳ ====================================
// һ���������������ɵļ������
#define KIN_MAX_COUNT		2048

#define COUNTRY_MAXID       6
// �������ֳ���
//#define KIN_NAME_SIZE		17	// Delete by zhanggx ������ת��NameLenDef.h

// ���帱�峤�ĸ���
#define KIN_ELDER_LEN		4

// �������Եĳ���
#define KIN_MANIFESTO_LEN	256

// ���幫��ĳ���
#define KIN_NOTICE_LEN		256

// ����ںų���
#define KIN_SLOGAN_LEN		256

// ��������ȼ�
#define KIN_MAX_LEVEL		6

// �����б�һҳ�Ĵ�С
#define KIN_PAGE_SIZE		60

// �����ַ������Գ���
#define KIN_MAX_PROPLEN		256

// ���ڵȼ������ȼ�
#define KIN_MAX_MEMBERLEVEL 5

// ���ڵȼ�������󳤶�
#define KIN_GRADENAME_LEN	17

// ����ƺ�ID
#define KINNAME_TITLE_ID	220

// ����������ʱ��Key
#define KIN_JOINCLAN_TIME	82

// 6����������
#define KIN_LEVEL_PLACE_6	3

// 5����������
#define KIN_LEVEL_PLACE_5	15

// 4����������
#define KIN_LEVEL_PLACE_4	30

// 4����������
#define KIN_CALCULATE_FIGHT_INVTERVAL_TIME	7*24*3600

//============================= ���弼�� ====================================
// ���弼�����ֳ���
//#define KINSKILL_NAME_LEN	17	// Delete by zhanggx ������ת��NameLenDef.h

// ���弼����������
#define KINSKILL_DESC_LEN	256

// �����弼�ܸ���
#define KINSKILL_MAX_NUM	16

// �����弼�ܸ���
#define KINSKILL_RSDATA_LEN	64

// ���о�Buff������
#define KINSKILL_MAX_GS_COUNT	5

// ������Buff������
#define KINSKILL_MAX_DJ_COUNT	5

// ���鹲����ID
#define KINSKILL_SHAREEXP_SKILLID	1

//============================= ����Ұս ====================================
// ����ս������Ƶ�������
#define KINWAR_MAX_DATECOUNT	5

// ����ս��ʼǰ׼���׶�
#define KINWAR_DAWN_TIME		(5 * 60)

//============================= �����¼� ====================================
// �����¼�������
#define KIN_EVENT_COUNT			20

// ��������¼�������
#define KIN_SOLDIERPAY_EVENT_COUNT			20

//============================= ����Boss ====================================
// һ��Bossʳ������������
#define KIN_BOSS_MAXFOOD		3

// ����Boss���ݳ���
#define KIN_BOSSDATA_LEN		16

//============================= ����ս�� ====================================
// �ɴ���ս���������
#define KIN_TANK_MAXCOUNT		9

// ս��������
#define KIN_TANK_TYPECOUNT		3

// ����������
#define KIN_TANK_MAXSTUFF		3

// ����ս����Χ����ķ�Χ
#define KIN_TANK_BUFFRADIUS		16

// �����Աÿ��Ĭ�Ͽ�ʹ���ʽ� ��С0 ��� 1000000
#define KIN_MEMBER_CANUSEMONEY		10000000


// ������ҫ������
#define KIN_CREDIT_NAME		"�����ʽ�"

// tolua_begin

// �����ʽ�����С��λ"��"�ĳ�"��"�����Ŵ�100��
#define KIN_PROP_CREDIT_NUM		100

// int���������
#define MAX_INT_VALUE						2147483647

// ����״̬ö��
enum TKinState
{
    emKinState_Invaild  = -1,					// �Ƿ�״̬
    emKinState_Informal,						// ����ʽ��
    emKinState_Formal,							// ��ʽ��
    emKinState_Dismissing,						// ��ɢ��
    emKinState_Num,								// ״̬����
};


// �����־
enum TKinSortType
{
    emKinSort_Name = 1,							// ��������
    emKinSort_Level,							// �ȼ�����
    emKinSort_Vocation,							// ְҵ����
    emKinSort_Online,							// �����������
    emKinSort_BattleValue,						// ս��ֵ����
    emKinSort_BattleOrder,						// ս����������

};


// tolua_end
// ���������Ѷ���־
enum TKinRead
{
    emKinRead_Head			= 0x0001,				// �б�����
    emKinRead_BaseData		= 0x0002,				// ��������
    emKinRead_Member		= 0x0004,				// ��Ա����
	emKinRead_LegendAward	= 0x0008,				// ������������
	emKinRead_LegendWar		= 0x0010,				// ����ս������(����ʹ��)

    emKinRead_PlusData		= 0x001E,				// ���и������ݣ���Ա���ݣ� �������ݣ� �ƺ�����, �����������ݣ�
    emKinRead_AllData		= 0x001F,				// ��������
};

// ��ɢԭ��
enum TKinDismissType
{
    emKinDismiss_Informal = 0,					// ��ʱ�����ɢ
    emKinDismiss_Dismiss,						// ������ɢʱ�䵽
    emKinDismiss_OffLine,						// ����û��������ߣ��ѷ�����
    emKinDismiss_Activity,						// ��Ծ�Ƚ�ɢ
    emKinDismiss_Public,						// ������������ɢ���������ݿ⣩
    emKinDismiss_Test,							// ������
    emKinDismiss_Merge,							// ����ϲ�
};


// ��ȡ����״̬
enum
{
    emKinSoldierPayState_Init = 0,		// ��ʼ״̬
    emKinSoldierPayState_NoAlloted,		// δ����
    emKinSoldierPayState_Alloted,		// �ѷ���

    emKinSoldierPayState_Max,		// ״̬���ֵ
};

// ���屭��ս����ҫ
enum ELegendGlory
{
	emLegendGlory_GoldCount,								// �𽱴���
	emLegendGlory_SilverCount,								// ��������
	emLegendGlory_WinCount,									// ����ʤ��
	emLegendGlory_LoseCount,								// ��������
	emLegendGlory_GodCount,									// �Ŷӳ������
	emLegendGlory_5KillCount,								// �Ŷ�5ɱ����
	emLegendGlory_4KillCount,								// �Ŷ�4ɱ����
	emLegendGlory_3KillCount,								// �Ŷ�3ɱ����
	emLegendGlory_DeathCount,								// �Ŷ���������
	emLegendGlory_KillCount,								// �Ŷӻ�ɱ����
	emLegendGlory_AssistCount,								// �Ŷ���������

	emLegendGlory_Max,
};

// �Ƿ���ճ�Ա����
struct SKinInfo
{
    char szName[KIN_NAME_SIZE];                 // ��������
    int nKinProp_ID;							// ����ID
    int nKinProp_State;							// ״̬
    int nKinProp_Level;							// ����ȼ�
    int nKinProp_EstablishTime;					// ���崴��ʱ��
    int nKinProp_DismissApplyTime;				// �����ɢ��ʱ��
    int nKinProp_ShaikhID;						// �����峤PDBID
    int nKinProp_MemberLastLogoutTime;			// ��Ա���������ʱ��
    int nKinProp_ReadFlag;						// �Ѿ������ݿ�������ݵı�־
    int nKinProp_Money;                         // �ʽ�
    int nKinProp_Activity;						// ��Ծ��
    int nKinProp_Place;							// ����
    int nKinProp_WorldID;						// ��Ϸ����ID

    struct SKinInfo()
    {
        memset(this, 0, sizeof(SKinInfo));
    }
};

// �Ƿ���ճ�Ա����
struct SAutoAccept
{
    bool  bAutoAccept;							// �Ƿ��Զ����ճ�Ա
    short nAcceptLevel;							// �Զ����˵ȼ�(2�ֽ�)
    DWORD dwAcceptFlag;							// �Զ���������(4�ֽ�) TKinAutoFlag

    SAutoAccept()
    {
        memset(this, 0, sizeof(SAutoAccept));
    }
};

struct SKinListNode
{
    unsigned long dwID;							// ����ID
    char szName[KIN_NAME_SIZE];					// ������
    short nMemberCount;							// ��������
    unsigned char nLevel;						// ����ȼ�

    SKinListNode()
    {
        memset(this, 0, sizeof(SKinListNode));
    }
};

// �ͻ��˼����Ա��Ϣ�ṹ
struct SKinClientApplyMember
{
    DWORD dwPDBID;								// ���ID
    char  szName[ACTORNAME_MAXSIZE];			// �������
    BYTE  nLevel;								// ��ҵȼ�
    int   nVocation;							// ���ְҵ

    SKinClientApplyMember()
    {
        memset(this, 0, sizeof(SKinClientApplyMember));
    }
};

struct SClientKinMember
{
    DWORD   dwUDBID;                            // �˺�ID
    DWORD	dwPDBID;							// ���ID
    char	szName[ACTORNAME_MAXSIZE];			// �������
    BYTE	nLevel;								// ��ҵȼ�
    BYTE	nIdentity;							// ����ְλ
    bool	bIsOnline;							// ����Ƿ�����
    int		nLogoutTime;						// ����������ʱ��
    BYTE	nVocation;							// ���ְҵ
    BYTE    nFaceID;                            // ͷ��ID
    bool	bIsPublicOnline;					// ����Ƿ��ڹ���������
    DWORD   dwBattleVal;						// ս��ֵ
    DWORD   dwSrcBattleVal;						// ��һ��ս��ֵ
    BYTE	dwBVOrder;							// ս��ֵ����
    BYTE	dwLastOrder;						// ��һ��ս��ֵ����
    DWORD   dwState;                            // ��Ϸ״̬ 
    BYTE    bySex;                              // �Ա�

    // ����������λ��Ϣ��nGrade��
    BYTE    nMatchType;
    DWORD   nRankScore;
    DWORD   nGrade;

	DWORD		dwWorldID;					    // ����ID
	DWORD		dwSceneID;					    // ����ID
	DWORD		serverID;					    // ������ID

    // �ֽ��� 97
    SClientKinMember()
    {
        memset(this, 0, sizeof(SClientKinMember));
    }
};

// �����Ա��չ����(���ܳ���32λ)
struct SKinMemberData
{
    int nReserveData;							// ��������

    SKinMemberData()
    {
        memset(this, 0, sizeof(SKinMemberData));
    }
};

// �����Ա��Ϣ�ṹ
struct SKinMember
{
    BYTE  bySex;								// �Ա� 0�� 1Ů 2������
    DWORD dwUDBID;                              // �˺�ID
    DWORD dwPDBID;								// ��ҽ�ɫID
    char  szName[ACTORNAME_MAXSIZE];			// ��ɫ����
    DWORD dwKinID;								// ����ID
    int	  nIdentity;							// ����ְλ

	int   nDayActivity;                         // �����Ծ��
    int   nWeekActivity;                        // �ܻ�Ծ��
    int   nClanCtrb;							// ��ṱ�׶�,���ܾ�����
    int   nTotalClanCtrb;						// �ۼư�ṱ�׶ȣ��ۼƾ�����
    DWORD dwLastClanCtrbTime;                   // �ϴξ���ʱ��

    DWORD dwPopedomFlag;						// Ȩ�ޱ�־��һ��λ��ʾһ��Ȩ�ޣ�һ��������ʾ�����ӵ�е�Ȩ�ޣ�
    bool  bIsOnline;							// �Ƿ�����
    int	  nLogoutTime;							// �������ʱ��

    int   nVocation;                            // ְҵ
    int   nLevel;								// �ȼ�
    short nFaceID;								// ���ͷ��

    int   nLoginTime;							// ��Ա����ʱ�䣨�ռ���ĳ�Ա������ʱ��Ϊ��������ʱ�䣩
    DWORD dwBattleVal;							// ս��ֵ

    DWORD dwSrcBattleVal;						// ��ս��ֵ
    DWORD dwBVOrder;							// ս��ֵ����
    DWORD dwLastOrder;							// ��һ��ս��ֵ����
    DWORD dwState;                              // ���״̬

                                                // ����������λ��Ϣ��nGrade��
    BYTE    nMatchType;
    DWORD   nRankScore;
    DWORD   nGrade;

	DWORD		dwWorldID;					    // ����ID
	DWORD		dwSceneID;					    // ����ID
	DWORD		serverID;					    // ������ID

    SKinMemberData  ExtendData;					// ��չ����

    SKinMember()
    {
        memset(this, 0, sizeof(SKinMember));
    }
};

// ����ÿ����������Ϣ
struct SKinLevel
{
    int nMaxMemberCount;						// �������

    SKinLevel()
    {
        memset(this, 0, sizeof(SKinLevel));
    }
};

// �����������Ա��Ϣ�ṹ
struct SSceneKinMember
{
    DWORD   dwUDBID;                            // ����˺�ID
    DWORD	dwPDBID;							// ��ҽ�ɫID
    int     nLevel;                             // ��ҵȼ�
    char	szName[17];							// ��ҽ�ɫ����
    DWORD	dwKinID;							// ����ID
    int		nIdentity;							// ����ְλ
    int		nClanCtrb;							// ��ṱ�׶�
    int		nTotalClanCtrb;						// �ۼư�ṱ�׶�
    DWORD	dwPopedomFlag;						// Ȩ�ޱ�־��һ��λ��ʾһ��Ȩ�ޣ�һ��������ʾ�����ӵ�е�Ȩ�ޣ�
    SKinMemberData ExtendData;					// ��չ����
    DWORD   dwBattleVal;						// ս��ֵ
    DWORD   dwSrcBattleVal;						// ��ս��ֵ
    DWORD   dwBVOrder;							// ս��ֵ����
    DWORD	dwLastOrder;						// ��һ��ս��ֵ����


    SSceneKinMember()
    {
        memset(this, 0, sizeof(SSceneKinMember));
    }

    bool operator < (const SSceneKinMember& member) const 
    {
        return dwPDBID < member.dwPDBID;
    }
};

// �����¼��ṹ
struct SKinEventHead
{
    int nCount;
};

struct SKinEvent
{
    tm tTime;									// ����ʱ��
    char szCoutext[256];						// ʱ�����ݣ�UBB��ʽ��
    SKinEvent()
    {
        memset(this, 0, sizeof(SKinEvent));
    }
};


// �����Ա���ڵȼ���Ϣ
struct SKinMemberGrade
{
    char szName[KIN_GRADENAME_LEN];				// �ü����Ӧ�ĳ�ν
    int  nGradeContribute;						// �����ڵȼ���Ӧ���ۼƹ��׶�
    int  nWarScore;								// �����ڵȼ��ĳ�Ա����ս��ɱ�������

    SKinMemberGrade()
    {
        memset(this, 0, sizeof(SKinMemberGrade));
    }
};

// ���峣����Ϣ
struct SKinConfig
{
    int  nCreateLevel;							// ����������Ҫ�ĵȼ�
    int  nCreateMoney;							// ����������Ҫ�Ľ�Ǯ
    int  nJoinNeedLevel;						// ���������Ҫ�ĵȼ�
    int  nJoinKinFreeze;						// ���������ȴ
	int nDismissWeeks;			                // �ܻ�Ծ������{nDismissWeeks}�ܵ���{nDismissActivity}�Զ���ɢ
	int nDismissActivity;			            // �ܻ�Ծ������{nDismissWeeks}�ܵ���{nDismissActivity}�Զ���ɢ
	int nOfflineDays;                           // �ӳ�����{nOfflineDays}�첻�����Զ�����ж��״̬
	int nOutgoingDays;                          // ����ж��״̬{nOutgoingDays}����Զ���ʧ�ӳ�ְλ
    int  nRenameMoney;                          // ��������Ǯ
    SKinConfig()
    {
        memset(this, 0, sizeof(SKinConfig));
    }
};

// ����
struct SKinActivity
{
    short nActivityID;			// �ID
    // ��ʼʱ��
    // ʣ��ʱ��
};

// ��Ҽ�����ص�����
struct SKinUserInfo
{
    int nQuitKinTime;			// �˳�����ʱ��
};

// �۲��߽ṹ
struct SKinObserve
{
    DWORD dwPDBID;	// �۲��ɫ
    int nCount;		// ����

    SKinObserve()
    {
        memset(this, 0, sizeof(SKinObserve));
    }
};

// �۲��߽ṹ
struct SKinListObserve
{
    DWORD dwPDBID;	// �۲��ɫ
    int nCount;		// ����

    SKinListObserve()
    {
        memset(this, 0, sizeof(SKinListObserve));
    }
};

struct SKinCupAwardNode
{
	DWORD		dwKinID;								// ����ID
	LONGLONG	llLegengCupID;							// ����ID
	BYTE		byCupRank;								// ��������
	char		szCupName[DBDEF_LEGENDCUP_NAME_LEN];	// ��������
	int			nCupConfigID;							// ��������
	DWORD		dwCupEndTime;							// ��������ʱ��

	SKinCupAwardNode()
	{
		memset(this, 0, sizeof(SKinCupAwardNode));
	}
};

struct SKinWarRecordNode
{
	LONGLONG	llWarID;						// ս��ID
	DWORD		dwKin1ID;						// ����1ID
	DWORD		dwKin2ID;						// ����2ID
	DWORD		dwBeginTime;					// ս����ʼʱ��
	WORD		dwWinKin;						// ʤ������ID
	char		szKin1Name[DBDEF_KINNAME_MAXSIZE];		// ����1����
	char		szKin2Name[DBDEF_KINNAME_MAXSIZE];		// ����2����

	int			nKin1Glory[emLegendGlory_Max];	// ����1��������buff
	int			nKin2Glory[emLegendGlory_Max];	// ����2��������buff

	SKinWarRecordNode()
	{
		memset(this, 0, sizeof(SKinWarRecordNode));
	}
};

struct SKinSingleWarNode
{
	LONGLONG nWarID;							// ս��ID
	DWORD	dwBeginTime;                        // ս����ʼʱ��
	DWORD	dwEndTime;                          // ս������ʱ��
	WORD	wWarType;                           // ս������
	WORD	wEndBecause;                        // ����ԭ�� 1��������Ӫ1ʤ�� 2��������Ӫ2ʤ�� 3��������Ӫ1Ͷ�� 4��������Ӫ2Ͷ��
	WORD	wActorCount;						// ��ɫ����

	vector<DBREQ_PARAM_WAR_ACTOR_INFO> actorinfo;	// �����ɫ��Ϣ

	SKinSingleWarNode()
	{
		nWarID = 0;
		dwBeginTime = 0;
		dwEndTime = 0;
		wWarType = 0;
		wEndBecause = 0;
		wActorCount = 0;
	}
};

// �����ṹ�嶨��


template<typename TMsg>
bool getKinMsg(TMsg*& pMsg, void*& pData, size_t& stLen)
{
	if (nullptr == pData)
	{
		WarningLn("getMsg nullptr == pData");
		return false;
	}
	if ( stLen < sizeof(TMsg) )
	{
		WarningLn("getMsg stLen < sizeof(TMsg) stLen=" << stLen << " sizeof(TMsg)=" << sizeof(TMsg));
		return false;
	}

	pMsg	= (TMsg *)pData;
	pData	= (void*)(pMsg+1);
	stLen	-= sizeof(TMsg);

	return true;
}


static SNetMsgHead*	getKinMsgHead( PACKAGE_PTR msg, void*& pData, size_t& stLen )
{
	pData	= (void*)msg->c_str();
	stLen	= msg->size();

	SNetMsgHead* pMsg;
	if ( !getKinMsg( pMsg, pData, stLen ) )
	{
		WarningLn("getMsgHead failed! !getMsg");
		return nullptr;
	}
	return pMsg;
}

#include "KinMsgDef.h"

#pragma pack()