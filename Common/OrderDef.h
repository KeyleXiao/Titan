/*******************************************************************
** �ļ���:	E:\speed\Common\OrderDef.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/15/2015	15��50
** ��  ��:	1.0
** ��  ��:	
            ���а���ض���
********************************************************************/

#pragma once
#include "PackagePtr.h"
#include "IGameDatabaseDef.h"
#include "Order_ManagedDef.h"
using namespace Order_ManagedDef;

// ���DEBUG��Ϣ
#ifdef VERSEION_DEBUG
#   define ORDER_DEBUG_OUTPUT
#endif
#ifdef ORDER_DEBUG_OUTPUT
#	define	ORDER_TRACELN(x)			TraceLn(x)
#	define	ORDER_WARNINGLN(x)			WarningLn(x)
#	define	ORDER_ERRORLN(x)			ErrorLn(x)
#	define	ORDER_EMPHASISLN(x)			EmphasisLn(x)
#else
#	define	ORDER_TRACELN(x)
#	define	ORDER_WARNINGLN(x)
#	define	ORDER_ERRORLN(x)
#	define	ORDER_EMPHASISLN(x)
#endif



#define ORDER_MAX_ITEM  200


typedef struct DBREQ_RESULT_MATCHTYPE_ORDERINFO_PERSON      OrderMatchtype_Item;

typedef struct DBREQ_RESULT_HEROORDER_DETAIL_INFO           OrderHero_Item;     // Ӣ�����ɰ�����

typedef struct DBREQ_RESULT_HEROTYPE_ORDER_DETAIL_INFO      OrderHeroType_Item; // Ӣ�����Ͱ�����

typedef struct DBREQ_PARAM_TABLE_ORDERINFO_PERSON           OrderComPersonKey_Item;// ��ɫ��ֵͨ�����а�


struct PersonKeyOrderItem_OC
{
    int		nOrder;  								// ��������
    WORD    wHeadID;                                // ͷ��Id
    DWORD   dwUserID;                               // �˺�ID
    int		nLastOrder;								// �ϴ�����
    DWORD	dwActorID;								// ���DBID
    char	szActorName[DBDEF_ACTORNAME_MAXSIZE];	// ��ɫ����
    long	lValue1;								// ��������ֵ1
    long	lValue2;								// ��������ֵ2
    DWORD   dwKinID;                                // ����ID
    char    szKinName[DBDEF_KINNAME_MAXSIZE];		// ս������
    DWORD   dwClanID;                               // ���ID
    char    szClanName[DBDEF_CLANNAME_MAXSIZE];		// �������
    BYTE    byMatchType;                             // ��λ�����Ϣ
    DWORD   nRankScore;
    DWORD   nGrade;
    

    PersonKeyOrderItem_OC(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

struct OrderKin_Item
{
    int     nOrder;
    DWORD   dwKinID;			                        // ս��ID
    char    szKinName[DBDEF_KINNAME_MAXSIZE];			// ս������
    DWORD   dwTotalFightCapacity;                       // ս���ܺ�
    DWORD   dwShaikhID;							        // �峤ID
    char    szShaikhName[DBDEF_ACTORNAME_MAXSIZE];	    // �峤����
    DWORD   dwPlayerCount;                              // ս������
    OrderKin_Item(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

struct OrderClan_Item
{
    int     nOrder;
    DWORD   dwClanID;			                        // ���ID
    char    szClanName[DBDEF_CLANNAME_MAXSIZE];			// ս������
    DWORD   dwTotalFightCapacity;                       // ս���ܺ�
    DWORD   dwShaikhID;							        // ����ID
    char    szShaikhName[DBDEF_ACTORNAME_MAXSIZE];	    // ��������
    DWORD   dwPlayerCount;                              // �������

    OrderClan_Item(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// ���а񷵻���������
enum OrderSetType
{
	ORDER_PERSON_SET_NOTIFY = 0,			 //	������֪ͨ�ͻ���
	ORDER_PERSON_SET_FAILED,				 //	�������з���ʧ��
	ORDER_PERSON_SET_SUCESS,				 // �������а�ɹ�
};


// ���а���ʾ����
struct Order_Set_Info 
{
    char personSetInfo[EOS_Max_Set_Class];
    Order_Set_Info()
    {
        memset(personSetInfo,0,sizeof(personSetInfo));
    }
};

// �����ṹ�嶨��

template<typename TMsg>
bool getOrderMsg(TMsg*& pMsg, void*& pData, size_t& stLen)
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


static SNetMsgHead*	getOrderMsgHead( PACKAGE_PTR msg, void*& pData, size_t& stLen )
{
	pData	= (void*)msg->c_str();
	stLen	= msg->size();

	Trace("SNetMsgHead*	getOrderMsgHead" << msg->size() );

	SNetMsgHead* pMsg;
	if ( !getOrderMsg( pMsg, pData, stLen ) )
	{
		WarningLn("getMsgHead failed! !getMsg");
		return nullptr;
	}
	return pMsg;
}

// ��Ϣ
//////////////////////////////////////////////////////////////////////////
// CHSS	ORDER_SCENE_SERVER		����������Ϣ
// C	CLIENT					�ͻ���
// CHS	ORDER_SERVER				�����

#include "OrderMsgDef.h"

