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
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen);

    // ����ͻ��˷��͹�������Ϣ
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void* pData, size_t nLen);

    LegendCupSceneService_Proxy() : m_pContainer(0)
    {

    }

public:
	SERVICE_PTR    m_pContainer;


private:
	void PostMsg(obuf256& oMsg);
};
