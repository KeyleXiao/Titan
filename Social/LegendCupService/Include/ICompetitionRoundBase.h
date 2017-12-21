/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\IKin.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/15/2015	17:50
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once

#include "LegendCupDef.h"
class CLegendCup;

struct ICompetitionRoundBase
{
    virtual bool Create(CLegendCup* pLegendCup,SCreateRoundBaseInfo sCreateInfo) = 0;

    virtual void Release() = 0;

    virtual bool CreateFromDB(CLegendCup* pLegendCup, SCreateRoundBaseInfo sCreateInfo) = 0;

    virtual bool setKinListFromDB(SGroupKinPos* posInfo) = 0;

    virtual bool addCompetitionNodeFromDB(SCompetitionNode* nodeInfo) = 0;

    // 设置本轮的参赛战队 ( bBreakSort 是否需要打乱顺序 bStartRound 是不是比赛开始的轮数)
    virtual bool setKinListAndGroup(list<DWORD> dwKinList,bool bBreakSort = false, bool bStartRound = false) = 0;

    // 继续本轮比赛
    virtual bool continueRound() = 0;

    // 结束本轮比赛
    virtual bool endRound() = 0;

    // 开始本轮比赛
    virtual bool startRound() = 0;

    virtual int competitionNode2Client(obuf &outBuf) = 0;

    virtual bool recordCupWarResult(SMsgRecordNodeWarResult* pMsgInfo) = 0;

    virtual bool judgeNodeWarResult(SMsgJudgeWarResult* pJudgeInfo) = 0;

    virtual bool joinLegendCupRoom(SMsgJoinLegendCupRoom* pMsgInfo) = 0;

    virtual bool lookUpCupNodeWarInfo(SMsgLookUpCupNodeWarInfo* pMsgInfo)=0;

    virtual bool setLegendCupNodeState(SMsgSetCompetitionNodeState* sNodeState) = 0;

    virtual bool updateNodeNewStartTime(int nSerchID,DWORD dwNewStartTime) = 0;

	virtual void updateSingleCupNodeInfo2Client(ClientID clientID, int nKinId) = 0;
};