//==========================================================================
/**
* @file	  : CKnockoutCompetitionRound.h
* @author : 秦其勇
* created : 2010-4-29   
* purpose : 淘汰赛操作类
            此类继承自CCompetitionRoundBase 
            用来处理淘汰赛相关的操作
*/
//==========================================================================

#ifndef __KNOCKOUT_COMPETITION_ROUND_H__
#define __KNOCKOUT_COMPETITION_ROUND_H__

#include <vector>
#include <hash_map>
#include "ISocialGlobal.h"
#include "IDBEngineService.h"
#include "LegendCup.h"
#include "CompetitionRoundBase.h"

class CKnockoutCompetitionRound : public CCompetitionRoundBase
{	
public:
	CKnockoutCompetitionRound();
	virtual ~CKnockoutCompetitionRound();

    // 释放
	void Release();
    
    // 结束本轮
    virtual bool endRound();

    // 设置本轮的参赛战队 ( bBreakSort 是否需要打乱顺序 bStartRound 是不是比赛开始的轮数)
    virtual bool setKinListAndGroup(list<DWORD> dwKinList,bool bBreakSort = false, bool bStartRound = false);

    // 裁定比赛
    virtual bool judgeNodeWarResult(SMsgJudgeWarResult* pJudgeInfo);

private:
    // 记录到DB
	bool WriteDataToDB();

    // 战队分组
    bool groupKinList(int nRoundID, int nMaxGroup, int nGroupMaxKin,list<DWORD> KinList, bool bStartRound = false);

    // 判定节点是否轮空
    bool bNodeKinIsBye(SCompetitionNode *pNode);

private:
    
};
#endif
