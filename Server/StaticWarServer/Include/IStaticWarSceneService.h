/*******************************************************************
** �ļ���:	E:\speed\Server\OrderServer\Include\IStaticWarSceneService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	����ս�����ݷ��� �����ṩ�Ľӿ�
            
********************************************************************/

#pragma once

#include "NetMessageDef.h"
#include "GatewayServiceDef.h"
#include "PackagePtr.h"

using Gateway::ClientID;

struct  IStaticWarSceneService
{
    // �����������������͹�������Ϣ
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg) = 0;

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg) = 0;

	// ����������һ�������
	virtual void reqOtherPlayerInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake = false) = 0;
	
	// �����������Ӣ������
	virtual void reqOtherPlayerHeroInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake = false) = 0;

};
