/*******************************************************************
** 文件名:	ClanSceneService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/6/2016
** 版  本:	1.0
** 描  述:	帮会场景代理
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "buffer.h"
#include "IClanSceneService.h"
#include "ClanSceneService.h"

using namespace rkt;

class ClanSceneService_Proxy : public IClanSceneService
{
public:
	//IClanSceneService////////////////////////////////////////////////////////////////////////
	// 获取帮会名称
	virtual string getClanName(DWORD nClanID);

    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, void *data, size_t len);

    //// 处理客户端发送过来的消息
    //virtual void    handleClientMsg(DWORD client, SNetMsgHead head, void *data, size_t len);

    virtual int getNumProp(DWORD dwClaneID, DWORD dwProp);

	virtual bool setNumProp(DWORD dwClanID, DWORD dwProp, int nValue);

	// 检查创建联盟杯赛
	virtual bool checkCreateClanLegendCup(DWORD dwClanID,int LegendCupConfigID);

    ClanSceneService_Proxy() : m_pContainer(0)
    {
    }

public:
	SERVICE_PTR    m_pContainer;

};
