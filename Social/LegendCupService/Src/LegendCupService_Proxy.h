/*******************************************************************
** 文件名:	E:\speed\Social\LegendCupServer\Src\LegendCupService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/16/2015	10:50
** 版  本:	1.0
** 描  述:	杯赛代理
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "ILegendCupService.h"


using namespace rkt;
using namespace std;

class LegendCupService_Proxy:public ILegendCupService
{
public:
	// ILegendCupService
	//////////////////////////////////////////////////////////////////////////
    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen);

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void* pData, size_t nLen);

	// 设置杯赛比赛结果
	virtual void setLegendCupMatchResult(SMsgRecordNodeWarResult sNodeWarResult);

    // 设置杯赛节点状态
    virtual void setLegendCupNodeState(SMsgSetCompetitionNodeState sNodeState);

    // 指定参加某杯赛的战队列表（报名阶段有效）
    virtual void luaAddCupRegister(LONGLONG llLegendCupID, PACKAGE_PTR pkKinList);

    // 移除杯赛所有参赛战队列表（报名阶段有效）
    virtual void luaRemoveCupRegister(LONGLONG llLegendCupID);

    // 退出正在比赛的节点
    virtual void exitStartCompetitionNode(LONGLONG llLegendCupID, int nSerchID, DWORD dwNewStartTime);

	// 弃权战队邮件提示
	virtual void sendAbstentionKinMail(LONGLONG llLegendCupID, DWORD dwFailedKinID);

    // 移除杯赛服务记录的信息
    virtual void endCupRemoveCreaterRecInfo(LONGLONG llLegendCupID);

    // 移除杯赛所有相关信息
    virtual void    removeServiceCupInfo(LONGLONG llLegendCupID);

    // 得到联盟杯赛信息
    virtual SCreaterLegendCupInfo getCreaterLegendCupInfo(BYTE byCupCreateType, DWORD dwCreaterID);

	// 获取免费联盟杯赛的奖励次数
	virtual int		getPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID);

	// 设置免费联盟杯赛的奖励次数
	virtual bool	setPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID, int nRecvPrizeCount);

    // 是否在参加杯赛
    virtual bool	isKinInLegendCup(DWORD dwKinID);

public:
	SERVICE_PTR m_pContainer;

private:
	void PostMsg(obuf256& oMsg);
};