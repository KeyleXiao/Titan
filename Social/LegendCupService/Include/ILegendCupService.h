/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\ILegendCupService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	10/15/2015	17:50
** 版  本:	1.0
** 描  述:	
            聊天系统服务提供的接口
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "LegendCupMsgDef.h"
#include "LegendCupDef.h"
using Gateway::ClientID;

struct ILegendCupService
{
    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg) = 0;

    // 处理客户端发送过来的消息
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg) = 0;

	// 设置杯赛比赛结果
	virtual void    setLegendCupMatchResult(SMsgRecordNodeWarResult sNodeWarResult)= 0;

    // 设置杯赛比赛节点状态
    virtual void    setLegendCupNodeState(SMsgSetCompetitionNodeState sNodeState)= 0;

    // 退出正在比赛的节点
    virtual void    exitStartCompetitionNode(LONGLONG llLegendCupID, int nSerchID, DWORD dwNewStartTime)= 0;

    // 指定参加某杯赛的战队列表（报名阶段有效）
    virtual void    luaAddCupRegister(LONGLONG llLegendCupID, PACKAGE_PTR pkKinList) = 0;

    // 移除杯赛所有参赛战队列表（报名阶段有效）
    virtual void    luaRemoveCupRegister(LONGLONG llLegendCupID) = 0;

    // 移除杯赛服务记录的信息
    virtual void    endCupRemoveCreaterRecInfo(LONGLONG llLegendCupID) = 0;

    // 通过创建者获取杯赛基础信息
    virtual SCreaterLegendCupInfo getCreaterLegendCupInfo(BYTE byCupCreateType,DWORD dwCreaterID) = 0;

	// 获取免费联盟杯赛的奖励次数
	virtual int		getPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID) = 0;

	// 设置免费联盟杯赛的奖励次数
	virtual bool	setPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID, int nRecvPrizeCount) = 0;

    // 是否正在参加比赛
    virtual bool	isKinInLegendCup(DWORD dwKinID) = 0;

    // 移除杯赛所有相关信息
    virtual void    removeServiceCupInfo(LONGLONG llLegendCupID) = 0;
};