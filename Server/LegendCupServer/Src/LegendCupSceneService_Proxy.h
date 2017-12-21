/*******************************************************************
** �ļ���:	E:\speed\Server\LegendCupServer\Src\LegendCupSceneService_Proxy.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	���а� ����
********************************************************************/

#pragma once


#include "IServiceContainer.h"
#include "buffer.h"
#include "ILegendCupSceneService.h"
#include "LegendCupSceneService.h"

using namespace rkt;


class LegendCupSceneService_Proxy : public ILegendCupSceneService
{
public:

    // �����������������͹�������Ϣ
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);


public:
	SERVICE_PTR    m_pContainer;


private:
	void PostMsg(obuf256& oMsg);
};
