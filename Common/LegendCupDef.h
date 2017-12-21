/*******************************************************************
** �ļ���:	E:\speed\Common\LegendCupDef.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/15/2015	15��50
** ��  ��:	1.0
** ��  ��:	
            ������ض���
********************************************************************/

#pragma once
#include "PackagePtr.h"
#include "IGameDatabaseDef.h"
#include "LegendCup_ManagedDef.h"
using namespace LegendCup_ManagedDef;

// ���DEBUG��Ϣ
//#ifdef VERSEION_DEBUG
#   define LEDENDCUP_DEBUG_OUTPUT
//#endif
#ifdef LEDENDCUP_DEBUG_OUTPUT
#	define	LEDENDCUP_TRACELN(x)			TraceLn(x)
#	define	LEDENDCUP_WARNINGLN(x)			WarningLn(x)
#	define	LEDENDCUP_ERRORLN(x)			ErrorLn(x)
#	define	LEDENDCUP_EMPHASISLN(x)			EmphasisLn(x)
#else
#	define	LEDENDCUP_TRACELN(x)
#	define	LEDENDCUP_WARNINGLN(x)
#	define	LEDENDCUP_ERRORLN(x)
#	define	LEDENDCUP_EMPHASISLN(x)
#endif

#define CUP_MAX_KIN_COUNT         32

#define CUP_MAX_KIN_PLAYER_COUNT  10


typedef DBREQ_PARAM_COMPETITION_NODE SCompetitionNode;

typedef DBREQ_RESULT_LEGEND_CUP_BASE_INFO SLegendCupBaseInfo;

typedef DBREQ_PARAM_LEGENDCUP_MEMBER    SLegendCupMemberInfo;

typedef DBREQ_REQUEST_UPDATE_CUP_KININFO SCupKinInfo;

typedef DBREQ_PARAM_ROUND_GROUP_KIN_POS SGroupKinPos;

typedef DBREQ_PARAM_ROUND_TIMEINFO SRoundTimeInfo;

typedef DBREQ_RESULT_READ_PLAYER_CUP_INFO	SLegendCupPlayerInfo;

enum ELegendCupBO
{
    emBO_None,
    emBO_1,     // bo1
    emBO_3,	    // bo3
    emBO_5,	    // bo5
    emBO_Max,	// ���
};

// �������� �÷�ʤ������
static int g_OBWinCondition[emBO_Max] =
{
    0, 1, 2, 3
};

static int getWinScoreByType(BYTE byType)
{
    if (byType <= emBO_None ||  byType >= emBO_Max)
    {
        return 1;
    }
    else
    {
        return g_OBWinCondition[byType];
    }
}

// �������� �����ٳ���ȷ����Ӯ
static int g_OBWinMaxScore[emBO_Max] =
{
    0, 1, 3, 5
};

static int getWinMaxScoreByType(BYTE byType)
{
    if (byType <= emBO_None ||  byType >= emBO_Max)
    {
        return 1;
    }
    else
    {
        return g_OBWinMaxScore[byType];
    }
}

enum ELegendDBReadFlag
{
    emLegendRead_NoIni              = 0x0000,
    emLegendRead_BaseData           = 0x0001,				// ��������
    emLegendRead_MemberData         = 0x0002,				// �������
    emLegendRead_KinData            = 0x0004,				// ս�Ӽ�¼����
    emLegendRead_CompetitionData    = 0x0008,				// ��������
    emLegendRead_RoundInKinList     = 0x0010,               // ÿ�ֽ����б�
    emLegendRead_RoundTimeInfo      = 0x0020,               // ÿ��ʱ����Ϣ
    emLegendRead_EXListData         = 0x0040,				// �����б�����
    emLegendRead_AllData            = 0x007F,				// ��������
};

struct SCreaterLegendCupInfo
{
    LONGLONG llLegendCupID;
	char     szCupName[COMMON_LEGENDCUP_NAME_LEN];// ��������
    BYTE     byState;
    int      nRegisterKinCount;
    int      nRegisterKinMax;

    SCreaterLegendCupInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SCupRegisterInfo
{
    int nRankID;
    DWORD dwRegisterPDBID;

    SCupRegisterInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SNodeWarRecord
{
    BYTE     byWinFlag;     // ʤ����־ EJudgeWar
    LONGLONG llWarID;       // ս��ID
    DWORD    dwWinnerKin;   // ս��ʤ��ID
    int      nBoCount;     // �ڼ���
    bool     bDBHaveRecord;    // �Ƿ��Ѿ���¼
	// todo
	DWORD	 dwBeginTime;	// ������ʼʱ��

    SNodeWarRecord(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// ���������������
struct SCreateLegendCupRoomInfo
{
    DWORD           dwWorldID;                          // �������ĸ������ID
    int             nBoCount;                            // �ڼ��� (��1��ʼ)
    int             nRoundID;                           // �ڼ���
    bool            bIsOnlineCup;                       // ��������
    LONGLONG        dwLedendCupID;                      // ����ID
    int             nSerchID;                           // ��������ID
    DWORD           dwMatchType;                        // �����ı�������
    DWORD           dwKin1;                             // ��������1
    DWORD           dwKin2;                             // ��������2
    DWORD           dwCanEnterTime;                     // �ܽ��뷿��ʱ��
    DWORD           dwBeginTimeInterval;                // �ܽ��뷿���ʼʱ����
    int             nMaxCancelCount;                    // �����ȡ���Ĵ���
    int             nMaxRound;                          // ��������

    ERoundCompetitionType eRoundType;                   // �������� ��̭ or С����
    BYTE            byBoType;                           // bo����
    int             nNodeKin1Score;                     // Kin1�÷�
    int             nNodeKin2Score;                     // Kin2�÷�
    DWORD           dwWarInterval;                      // �ڵ�δ������һ������ʱ����
    bool            bKin1RoundLastNode;                 // kin1�������Ľڵ�
    DWORD           dwKin1NodeEndNextWarTime;           // �ڵ�������Kin1��һ��������ʱ��

    bool            bKin2RoundLastNode;                 // kin2�������Ľڵ�
    DWORD           dwKin2NodeEndNextWarTime;           // �ڵ�������Kin2��һ��������ʱ��
    
    SCreateLegendCupRoomInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// �鿴�����ڵ���Ϣ
struct SMsgLookUpClientNodeHead
{
    bool bHaveRightJudge;
    DWORD   dwKin1;             // ����ս��1
    int     nKin1Score;
    char    szKin1Name[KIN_NAME_SIZE];// ����

    DWORD   dwKin2;             // ����ս��2
    int     nKin2Score;
    char    szKin2Name[KIN_NAME_SIZE];// ����
    DWORD   dwResetEndTime;         // ����������ʱ���

    int  nCount;

    // SNodeWarRecord * nCount;
    SMsgLookUpClientNodeHead(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// �ڵ�ȷ����
struct SCompetitionScoreInfo
{
    DWORD           dwKinID;                            // ս��ID
    DWORD           dwNodeScore;                        // ����ʤ���÷�
    DWORD           dwKDA;                              // KDA

	bool operator < (SCompetitionScoreInfo & NodeInfo) 
	{
		int selfScore = dwNodeScore *10000 + dwKDA;
		int cmpareScore = NodeInfo.dwNodeScore *10000 + NodeInfo.dwKDA;
		return selfScore < cmpareScore;
	}

    SCompetitionScoreInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SCreateCupInfo
{
    DWORD           dwLedendCupID;                      // ����ID
    BYTE            bIsOnlineCup;                       // ���ϱ��� ���±���
    DWORD           dwWorldID;                          // ��������ID
    PDBID           dwCreataActorID;                    // �������
    char            szCupName[COMMON_LEGENDCUP_NAME_LEN];// ��������
    int             nMaxKinNumber;                    // ����������
    DWORD           dwRegisterDeadline;                 // ������ֹʱ��
    BYTE            byCupFlowType;                      // ������������
    BYTE            nRoundCount;                        // ����
    ECupState       eCurrentCupState;                   // ״̬

    SCreateCupInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SCreateRoundBaseInfo
{
    LONGLONG  llLegendCupID;
    int nDetailConfigID;
    int nRoundID; 
    int nMaxGroup;
    int nGroupMaxKin;
    int nGroupPromotionCount;
    BYTE byRoundCompetitionType;
    BYTE byBoType;

    SCreateRoundBaseInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SNextWarTimeInfo
{
    bool bRoundLastNode;
    DWORD   dwNextWarTime;

    SNextWarTimeInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// �����ṹ�嶨��

template<typename TMsg>
bool getLegendCupMsg(TMsg*& pMsg, void*& pData, size_t& stLen)
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


static SNetMsgHead*	getLegendCupMsgHead( PACKAGE_PTR msg, void*& pData, size_t& stLen )
{
	pData	= (void*)msg->c_str();
	stLen	= msg->size();

	Trace("SNetMsgHead*	getLegendCupMsgHead" << msg->size() );

	SNetMsgHead* pMsg;
	if ( !getLegendCupMsg( pMsg, pData, stLen ) )
	{
		WarningLn("getMsgHead failed! !getMsg");
		return nullptr;
	}
	return pMsg;
}

// ��Ϣ
//////////////////////////////////////////////////////////////////////////
// CHSS	LEDENDCUP_SCENE_SERVER		����������Ϣ
// C	CLIENT					�ͻ���
// CHS	LEDENDCUP_SERVER				�����

#include "LegendCupMsgDef.h"

