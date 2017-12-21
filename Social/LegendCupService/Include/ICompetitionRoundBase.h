/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\MatchServer\Include\IKin.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/15/2015	17:50
** ��  ��:	1.0
** ��  ��:	
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

    // ���ñ��ֵĲ���ս�� ( bBreakSort �Ƿ���Ҫ����˳�� bStartRound �ǲ��Ǳ�����ʼ������)
    virtual bool setKinListAndGroup(list<DWORD> dwKinList,bool bBreakSort = false, bool bStartRound = false) = 0;

    // �������ֱ���
    virtual bool continueRound() = 0;

    // �������ֱ���
    virtual bool endRound() = 0;

    // ��ʼ���ֱ���
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