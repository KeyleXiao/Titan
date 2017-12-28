/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\IKinService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/15/2015	17:50
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "KinDef.h"
using Gateway::ClientID;
struct IKin;

struct IKinService
{

    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void *data, size_t len) = 0;

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void *data, size_t len) = 0;

    /** 根据战队ID获取Kin接口
	@param dwKinID		:战队ID
	*/
	virtual IKin*	getKin(DWORD dwKinID) = 0;

    /** 外部接口删除战队成员
	@param dwKinID		:战队ID 
	*/
    virtual bool    deleteKinMember(DWORD dwKinID, DWORD ActorID) =0;

    /** 根据战队ID获取Kin数据
	@param dwKinID		:战队ID
	*/
	virtual SKinInfo getKinInfo(DWORD dwKinID) = 0;

    /** 根据战队ID获取成员数据接口
	@param dwKinID		:战队ID
	*/
	virtual SKinMember getKinMemberInfo(DWORD dwPBID) = 0;

    /** 根据战队ID获取成员列表接口
	@param dwKinID		:战队ID 
    @return obuf (都会有第一个DWORD 来保存成员数量 >= 0)
	*/
	virtual DWORD   getKinMemberList(DWORD dwKinID,PDBID* pReturnArray, DWORD dwArrayMaxSize) = 0;

    /** 获取战队成员数量
	@param dwKinID		:战队ID 
	*/
	virtual DWORD   getKinMemberCount(DWORD dwKinID) = 0;

    /// purpose: 设置战队贡献度
    virtual bool    addClanCtrb(DWORD dwPDBID, int nClanCtrb) = 0;

    // 获取总战力
    virtual DWORD   getTotalFightScore(DWORD dwKinID) = 0;

	/// purpose: 新增战队杯赛奖励
	virtual void addKinLegendAward(SKinCupAwardNode sAwardNode) = 0;

	/// purpose: 新增战队杯赛战绩
	virtual void addKinLegendWarResult(SKinWarRecordNode sRecordNode) = 0;

	// purpose:  战队杯赛荣誉增加
	virtual void addKinLegendGlory(int nKinID, int nNum, int nGloryType) = 0;

	// purpose: 取战队杯赛荣誉
	virtual int getKinLegendGlory(int nKinID, int nGloryType) = 0;

	/// purpose: 获取周活跃度
	virtual int getWeekActivity(DWORD dwKinID) = 0;
};