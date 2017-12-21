/*******************************************************************
** 文件名:	IClanService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/31/2016	16:57
** 版  本:	1.0
** 描  述:	帮会服务接口
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
using Gateway::ClientID;

struct IClanService
{

    virtual void release() = 0;

    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg) = 0;

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg) = 0;

    // 查找战队所属帮会ID
    virtual DWORD findKinClan(DWORD dwKinID) = 0;

    // 移除正在读取的战队
	virtual void removeReadingKin(DWORD dwKinID) = 0;

	// 获取帮会成员列表
	virtual int getClanKinList(DWORD dwClanID, DWORD *pKinList, int nLen) = 0;

    // 取得帮会数值
    virtual int getClanNumProp(DWORD dwClanID, int nNumIndex) = 0;

	// 获取帮会名称
	virtual string getClanName(DWORD dwClanID) = 0;

	// 联盟杯赛创建
	virtual void notifyCreateClanLegendCup(DWORD dwClanID, LONGLONG llLegendCupID, int nMaxKinCount, LPCSTR pCupName) = 0;

};