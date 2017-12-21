/*******************************************************************
** �ļ���:	Clan_ManagedDef.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��ɭ
** ��  ��:	5/26/2016
** ��  ��:	1.0
** ��  ��:	�������йܹ�������
********************************************************************/

#ifndef __COMMON_CLAN_MANAGED_DEF_H__
#define __COMMON_CLAN_MANAGED_DEF_H__

#include "GameDef.h"
#include "ManagedCode.h"

namespace Clan_ManagerDef
{
    ENUM EMCLAN_LIST
    {
        // ����б�һҳ�Ĵ�С
        CLAN_LIST_PAGE_SIZE = 6,
    };


    // ����б�״̬����
    ENUM EMClanListClanStatus
    {
	    CLAN_LIST_STATUS_NORMAL,		// ��ͨ
	    CLAN_LIST_STATUS_REQUESTING,	// ������
	    CLAN_LIST_STATUS_FULL,			// ����
		CLAN_LIST_STATUS_DISMISSING,	// ��ɢ��
	    CLAN_LIST_STATUS_MAX			// ���״̬��
    };

    // ���ְλ
    ENUM EMClanIdentity
    {
	    EMCLANIDENTITY_INVALID = -1,		// ��Ч
	    EMCLANIDENTITY_MEMBER = 0,			// ��Ա
	    EMCLANIDENTITY_SHAIKH,				// ָ�ӹ�
	    EMCLANIDENTITY_ELDER,				// ����
    };

    ENUM EMClanOrderMax
    {
        EMCLANORDERMAX      = 3,             // ��������������
    };

    // ���״̬ö��
    ENUM EMClanState
    {
        emClanState_Invaild  = -1,			// �Ƿ�״̬
        emClanState_Normal = 0,				// ͨ��״̬
        emClanState_Dismissing,				// ��ɢ��
        emClanState_Num,					// ״̬����
    };

	// ����״̬ö��
	ENUM EMClanShaikhState
	{
		ClanShaikhState_Unknow = 0,			    // δ֪
		ClanShaikhState_Reigning,			    // ��λ��
		ClanShaikhState_Offline,				// ������
		ClanShaikhState_Outgoing,			    // ж����
	};
}

#endif // __COMMON_CLAN_MANAGED_DEF_H__