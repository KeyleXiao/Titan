/*******************************************************************
** �ļ���:	E:\speed\Server\OrderServer\Src\StaticWarSceneService_Proxy.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	����ս�����ݷ��� ����
********************************************************************/

#pragma once


#include "IServiceContainer.h"
#include "buffer.h"
#include "IStaticWarSceneService.h"
#include "StaticWarSceneService.h"

using namespace rkt;


class StaticWarSceneService_Proxy : public IStaticWarSceneService
{
public:

    // �����������������͹�������Ϣ
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

	// ����������һ�������
	virtual void reqOtherPlayerInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake);

	// �����������Ӣ������
	virtual void reqOtherPlayerHeroInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake);

public:
	SERVICE_PTR    m_pContainer;


private:
	void PostMsg(obuf256& oMsg);
};
