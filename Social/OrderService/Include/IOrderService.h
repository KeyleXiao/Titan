/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\MatchServer\Include\IMatchService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	10/15/2015	17:50
** ��  ��:	1.0
** ��  ��:	
            ����ϵͳ�����ṩ�Ľӿ�
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
using Gateway::ClientID;

struct IOrderService
{
    // �����������������͹�������Ϣ
    virtual void     handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen) = 0;

    // ����ͻ��˷��͹�������Ϣ
    virtual void     handleClientMsg(DWORD client, SNetMsgHead head, void* pData, size_t nLen) = 0;
    // ��ȡս������
    // ������Ա����
    virtual DWORD getKinOrder(DWORD dwKinID)= 0;

    // ��ð������
    // ������Ա����
    virtual DWORD getClanOrder(DWORD dwClanID)= 0;

    // ���ݷ���
    // ���ض�ȡ���а�����
    virtual void onDBReturnOrderInfoSetting(int nQueueIndex, PACKAGE_PTR msg) = 0;
};