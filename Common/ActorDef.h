/*******************************************************************
** �ļ���:	ActorDef.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2016 - All Rights Reserved
** ������:	�ΰ
** ��  ��:	2/25/2016
** ��  ��:	1.0
** ��  ��:	��ɫ�����ṹ�嶨��

********************************************************************/

#pragma once

#include "GameDef.h"
#include "GatewayServiceDef.h"

#pragma pack(push, 1)

using Gateway::ClientID;

// �ǽ�ģʽ
enum ELoginMode
{
    elogin_unknown,
    elogin_online,                  // ���߽�
    elogin_in_process,              // �����ڵǽ�
    elogin_cross_process,           // ����̵ǽ�
    elogin_cross_gameworld,         // �����ǽ�
    elogin_reconnect_center,        // �������ķ�
};

// �ǳ�ģʽ
enum ELogoutMode
{
    elogout_unknown,
    elogout_offline,                // ���ߵǳ�
    elogout_in_process,             // �������е�ͼ�ǳ�
    elogout_cross_process,          // ������е�ͼ�ǳ�
    elogout_cross_gameworld,        // �����ǳ�
    elogout_disconnect,             // ���ߵǳ�
    elogout_disconnect_center,		// �����ķ����ߵǳ�
};

enum ACTOR_CREATE_CONTEXT_FLAG
{
    CONTEXT_FLAG_NONE           = 0x0,             /* �� */
    CONTEXT_FLAG_ATTACH_THREAD       = 0x01,       /* ��������LOGIC�߳� */          
};

enum EDateSrcType
{
    eds_type_cache,    // �ڴ�����
    eds_type_db        // ���ݿ�
};

// ��ɫ����������
struct SActorCreateContext
{
    ClientID            client;
    BYTE                nFlag;                  // ���ӱ�ʶACTOR_CREATE_CONTEXT_FLAG  
    UDBID               udbid;
    DWORD               dwIdCard;               // ���֤ID ֻ��½������ɫ����
    bool                bIsAdult;               // �Ƿ������ ֻ��½������ɫ����
    DWORD	            dwPassportFlags;		// ͨ��֤��ر�ʶ PassportFlags
    DWORD               dwServiceFlags;         // ��������ʶ ServiceFlags
    char                szPwd[GAME_PWD_MAXSIZE];  // ��¼����

    PDBID               pdbid;  
    int                 nSceneID;               // ���������ĸ�����
    int                 nSceneSerialNo;          // Լ��nSceneID������ʱnSceneID�����Ѿ�����)
    Vector3             locBorn;                // ������
    float               fAngle;                 // ����
    EDateSrcType        eDStype;                // ��ɫ������Դ

    //  �������ڴ����� ...
    int                 nCreateDataLen;         // �������ݳ���
    int                 nCreateDataFlag;        // TO_
    int                 nUpdateDataLen;         // �������ݳ���
    int                 nUpdateDataFlag;        // TO_
    // ��������...      ��������...


    SActorCreateContext()
    {
        memset(this, 0, sizeof(*this));
    }
};

#pragma pack(pop)