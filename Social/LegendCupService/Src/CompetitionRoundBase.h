//==========================================================================
/**
* @file	  : CCompetitionRoundBase.h
* @author : 秦其勇
* created : 2010-4-29   
* purpose : 每轮比赛基类
            1、本类记录了本轮各个战队所在的位置信息
            2、本轮比赛节点信息 

*/
//==========================================================================

#ifndef __COMPETITION_ROUND_H__
#define __COMPETITION_ROUND_H__

#include <vector>
#include <hash_map>
#include "ISocialGlobal.h"
#include "IDBEngineService.h"
#include "LegendCup.h"
#include "ICompetitionRoundBase.h"
#include "LegendCupDef.h"
#include "KinHelper.h"

class CCompetitionRoundBase : public ICompetitionRoundBase, public IDBRetSink, public TimerHandler
{
public:
    CCompetitionRoundBase();
    virtual ~CCompetitionRoundBase();

    ////////////////////////////////////ICompetitionRoundBase//////////////////////////////////////
    virtual bool Create(CLegendCup* pLegendCup, SCreateRoundBaseInfo sCreateInfo);

    virtual void Release();

    // 从DB初始化本轮信息
    virtual bool CreateFromDB(CLegendCup* pLegendCup, SCreateRoundBaseInfo sCreateInfo);

    // 从DB设置参赛战队列表
    virtual bool setKinListFromDB(SGroupKinPos* posInfo);

    // 从DB添加比赛节点
    virtual bool addCompetitionNodeFromDB(SCompetitionNode* pNodeInfo);

    // 记录比赛的结果
    virtual bool recordCupWarResult(SMsgRecordNodeWarResult* pMsgInfo);

    virtual bool setLegendCupNodeState(SMsgSetCompetitionNodeState* sNodeState);

    // 裁定比赛
    virtual bool judgeNodeWarResult(SMsgJudgeWarResult* pJudgeInfo);

    // 设置本轮的参赛战队 ( bBreakSort 是否需要打乱顺序 bStartRound 是不是比赛开始的轮数)
    virtual bool setKinListAndGroup(list<DWORD> dwKinList,bool bBreakSort = false, bool bStartRound = false);

    // 继续本轮比赛
    virtual bool continueRound();

    // 开始本轮比赛
    virtual bool startRound();

    // 结束本轮比赛
    virtual bool endRound(list<DWORD> promotionList, list<DWORD> weedOutList);

    // 结束本轮比赛
    virtual int  competitionNode2Client(obuf &outBuf);

    // 加入杯赛房间
    virtual bool joinLegendCupRoom(SMsgJoinLegendCupRoom* pMsgInfo);

    // 查询节点战场信息
    virtual bool lookUpCupNodeWarInfo(SMsgLookUpCupNodeWarInfo* pMsgInfo);

    virtual bool updateNodeNewStartTime(int nSerchID, DWORD dwNewStartTime);

	// 更新杯赛当前轮战队节点信息到客户端
	virtual void updateSingleCupNodeInfo2Client(ClientID clientID, int nKinId);

    ///////////////////////////////IDBRetSink///////////////////////////////////////////
    virtual void		OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

    ///////////////////////////////TimerHandler///////////////////////////////////////////
    virtual void        OnTimer( unsigned long dwTimerID );

public:
    // 是否决赛
    bool    isFinalRound();

    // 能否结束本轮杯赛
    bool    bOfflineCupCanEndRound();

    // 保存节点信息到数据库
    bool    saveCompetitionNodeToDB(SCompetitionNode* pNodeInfo);

private:
    // 生成的快速定位ID
    int     buildSerchID(int nRound,int nGroup, int nDay, int pos);

    // 生成所有组的节点
    bool    buildAllCompetitionNode();

    // 生成所有组的节点
    bool    AllCompetitionNodeCreateRoom();

    // 生成某一组的节点信息
    bool    buildGroupCompetitionNode(int GroupID, map<int, DWORD> mapAllKins);

    // 获取本轮比赛开始时间
    DWORD   getRoundBeginTime();

    // 线上比赛结果记录
    bool    onlineRecordWarResult(SMsgRecordNodeWarResult* pMsgInfo);

    // 线上判定胜利
    bool    onlineJudgeSelectWinner(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo);

    // 线上判定撤销
    bool    onlineResetWarRecord(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo);

    // 线上判定淘汰
    bool    onlineJudgeAllOut(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo);

    // 线下比赛结果记录
    bool    offlineRecordWarResult(SMsgRecordNodeWarResult* pMsgInfo);

    // 维持原判
    bool    offlineJudgeConfirmNode(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo);

    // 判定胜利
    bool    offlineJudgeSelectWinner(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo);

    // 判定撤销
    bool    offlineJudgeCancel(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo);

    // 判定淘汰
    bool    offlineJudgeAllOut(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo);

    // 更新判定状态
    bool    sendJudgeResult2Client(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo);

    // 创建比赛节点房间
    bool    createCompetitionNodeRoom(SCompetitionNode* pNodeInfo, bool bReset = false);

    // 更新单个杯赛节点信息到客户端
    void    updateSingleCupNodeInfo2AllView( SCompetitionNode* pNodeInfo);

    // 更新单个杯赛节点战场信息到客户端
    void    updateCupNodeWarInfo2AllView( SCompetitionNode* pNodeInfo);

    // 发送到客户端
    void    send2Client( ClientID clientID, PACKAGE_PTR Package );

    // 得到bo时间间隔
    DWORD   getBoInterval();

    // 更新节点状态
    void    updateNodeState(SCompetitionNode* pNodeInfo, ECompetitionNodeState eState);

    // 更新淘汰组排名
    void    updateWeedOutListRank( list<DWORD> weedOutList);

    // 更新决赛排名
    void    updateFinalListRank( list<DWORD> promotionList);

    // 线下裁定比赛
    bool    offlinejudgeNodeWarResult(SMsgJudgeWarResult* pJudgeInfo);

    // 线上裁定比赛
    bool    onlinejudgeNodeWarResult(SMsgJudgeWarResult* pJudgeInfo);

    // 线上重置比赛时间检查
    bool    onlineCanResetWarTime(SMsgJudgeWarResult* pJudgeInfo);

    // 线上重置比分检查
    bool    onlineCanResetScoreCheck(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo);

    // 广播给参赛队员杯赛信息
    void    broadCastKinMemberCompetitionInfo(SCompetitionNode* pNodeInfo);

    // 获取指定节点下一场比赛时间
    SNextWarTimeInfo    getNextWarTime(int nCurSerchID, DWORD dwKinID);

    // 获取淘汰赛下场比赛时间
    SNextWarTimeInfo   getKnockoutTypeNextWarTime();

    SNextWarTimeInfo   getGroupTypeNextWarTime(int nCurSerchID,DWORD dwKinID);

    // 通知双方参赛人员已经裁定了比赛
    void                notifyNodeMemberJudgeWar(SCompetitionNode* pNodeInfo);

    // 通知创办者需要裁定比赛
    void                notifyCreaterNeedJudge(DWORD dwKin1, DWORD dwKin2);

protected :
    CLegendCup*     m_pLegendCup;
    LONGLONG        m_llLegendCupID;        // 杯赛ID
    int             m_nRoundID;             // 第几轮比赛
    int             m_nMaxGroup;             // 分组数量
    int             m_nGroupMaxKin;         // 每组最大战队数量
    int             m_nGroupPromotionCount; // 每组出线数量
    int             m_nDetailConfigID;        // round配置ID
    map<int,DWORD>  m_mapSerchID2RoomID;    // 当前节点对应的房间ID
    BYTE            m_byBoType;             // bo类型

    ERoundCompetitionType m_eCompetitionType;

    // groupID   <groupPosID, kinID>
    map<int, map<int,DWORD>> map_RoundGroup; // 每组分配的位置信息

    // serchID, SCompetitionNode
    map<int, SCompetitionNode> m_CompetitionNodeList;

    //serchID    <nBoCount, SNodeWarRecord>
    map<int, map<int,SNodeWarRecord>> m_CompetitionNodeWarInfo; 

};

#endif