/*******************************************************************
** 文件名:	LegendCupClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/10/19  17:21
** 版  本:	1.0
** 描  述:	比赛系统客户端
** 应  用:  	
	
*******************************************************************/

#pragma once
#include "ILegendCupClient.h"
#include "IEntityClient.h"
#include "MatchDef.h"
#include "LegendCupDef.h"
#include "LegendCupMsgDef.h"

class LegendCupClient:public ILegendCupClient, public IGameViewEventHandler, public IMessageHandler
{
public:

	////////////////////////////ILegendCupClient//////////////////////////////////////////////
	virtual void		Release(void);

    ////////////////////////////IGameViewEventHandler////////////////////////////////////////////////////
    virtual bool		onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );

    /////////////////////////////IMessageHandler/////////////////////////////////////////////////////////
    virtual void		onMessage( SNetMsgHead* head, void* data, size_t len);

public:
	// LegendCupClient
	bool				Create(void);
private:
    // 杯赛的黑名单
    void onMsgSendAllBalcklist( SNetMsgHead* head, void* data, size_t len );

    // 杯赛的某个队伍的参赛成员
    void onMsgSendKinMember( SNetMsgHead* head, void* data, size_t len );

    // 所有杯赛信息
    void onMsgSendAllLegendcup( SNetMsgHead* head, void* data, size_t len );

    // 杯赛的所有战队信息
    void onMsgSendAllKinlist( SNetMsgHead* head, void* data, size_t len );

    // 杯赛的比赛信息
    void onMsgSendAllKinMemberinfo( SNetMsgHead* head, void* data, size_t len );

    // 杯赛每轮时间点信息
    void onMsgSendAllRoundTimeinfo( SNetMsgHead* head, void* data, size_t len );

    // 杯赛的每轮信息
    void onMsgSendAllRoundInfo( SNetMsgHead* head, void* data, size_t len );

    // 查询杯赛节点战场信息
    void onMsgSendNodeLookupWarInfo(SNetMsgHead* head, void* data, size_t len);

    // 更新单个比赛信息
    void onMsgSendUpdateSingelCupInfo(SNetMsgHead* head, void* data, size_t len);

    // 更新单个比赛节点信息
    void onMsgSendUpdateSingelCupNodeInfo(SNetMsgHead* head, void* data, size_t len);

    // 发送申请成功消息
    void onMsgSendRegisterSuccess(SNetMsgHead* head, void* data, size_t len);

    // 参赛者响应发布杯赛成功
    void onMsgPublicSuccess(SNetMsgHead* head, void* data, size_t len);

    // 举办者响应发布杯赛成功
    void onMsgCreaterPublicSuccess(SNetMsgHead* head, void* data, size_t len);


    // 获得创建杯赛类型相关
    void onMsgGetCreateCupTypeInfo(SNetMsgHead* head, void* data, size_t len);

    // 请求裁定比赛结果
    void reqJudgeSingalWarResult(SMsgJudgeWarResult *pMsgInfo);

    // 查看比赛节点战场信息
    void reqLookUpCupNodeWarInfo(SMsgLookUpCupNodeWarInfo *pMsgInfo);

    // 请求加入比赛房间
    void reqJoinLegendCupRoom(SMsgJoinLegendCupRoom *pMsgInfo);

    // 请求战队成员列表
    void reqKinMemberList(SMsgRequestKinMember *pMsgInfo);

    // 请求黑名单
    void reqAllBalcklist(SMsgRequestAllBlackKinList *pMsgInfo);

    // 查看比赛
    void reqlookOverLegendCup(SMsgRequestLookOverCup *pMsgInfo);

    // 移动到黑名单
    void reqAddToBlacklist(SMsgAddToBlackList *pMsgInfo);

    // 删除黑名单
    void reqRemoveBlacklist(SMsgRemoveBlackList *pMsgInfo);

    // 发布比赛
    void reqPublickCompetition(SMsgPublicCompetition *pMsgInfo);
    
    // 退出杯赛
    void reqExitCup(SMsgExitCup *pMsgInfo);

    // 请求加入杯赛
    void reqJoinCup(SMsgJoinCup *pMsgInfo);

    // 请求创建杯赛
    void reqCreateCup(SMsgCreateCup *pMsgInfo);

    // 请求杯赛列表
    void  requestCupList();

	// 请求创建类型
	void reqCreateType(SMsgCreateCupType *pMsgInfo);

	// 请求退出杯赛观察
	void reqQuitObserve(SMsgReqQuitObserve *pMsgInfo);

    // 请求结束线下杯赛报名阶段
    void  reqOfflinePreEndRegister(SMsgOfflineEndRegister *pMsgInfo);

private:
    
};

extern LegendCupClient* g_pLegendCupClient;