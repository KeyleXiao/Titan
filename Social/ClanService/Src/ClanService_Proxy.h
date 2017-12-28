/*******************************************************************
** 文件名:	ClanService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/31/2016	17:19
** 版  本:	1.0
** 描  述:	帮会服务代理
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "IClanService.h"

using namespace rkt;
using namespace std;

class ClanService_Proxy : public IClanService
{
public:
	////////////////// ClanService_Proxy ////////////////////////////////////////////////////////

	ClanService_Proxy() :m_pContainer(0) {};
	~ClanService_Proxy() {};
public:
	///////////////////////////////////IClanService////////////////////////////
    virtual void release();

    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void *data, size_t len);

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void *data, size_t len);

	// 查找战队所属帮会ID
	virtual DWORD findKinClan(DWORD dwKinID);

    // 移除正在读取的战队
	virtual void removeReadingKin(DWORD dwKinID);

	// 获取帮会成员列表
	virtual int getClanKinList(DWORD dwClanID, DWORD *pKinList, int nLen);

    // 取得帮会数值
    virtual int getClanNumProp(DWORD dwClanID, int nNumIndex);

	// 获取帮会名称
	virtual string getClanName(DWORD dwClanID);

	// 联盟杯赛创建
	virtual void notifyCreateClanLegendCup(DWORD dwClanID, LONGLONG llLegendCupID, int nMaxKinCount, LPCSTR pCupName);
public:
	SERVICE_PTR m_pContainer;
};