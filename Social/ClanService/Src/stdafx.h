// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "Base.h"
#define new RKT_NEW
using namespace rkt;
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(Base))
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(Framework))


#include "ICenterServerProtocol.h"
#include "ICenterServerConnector.h"
using namespace CenterServer;

// ���
#define fillClanData(ob, destEP, msgId) \
    SNetMsgHead* ___msgHead = (SNetMsgHead*)ob.data();\
    ___msgHead->bySrcEndPoint = MSG_ENDPOINT_SOCIAL;\
    ___msgHead->byDestEndPoint = destEP;\
    ___msgHead->byKeyModule = MSG_MODULEID_CLAN;\
    ___msgHead->byKeyAction = (ushort)msgId;\
    ob.offset(sizeof(SNetMsgHead))

// 0��ʾ������Ϣ
// !=0��ֹ��ʾ������Ϣ
#define MatchServer_DisableShowDebug 0