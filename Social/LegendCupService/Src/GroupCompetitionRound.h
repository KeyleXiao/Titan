//==========================================================================
/**
* @file	  : CGroupCompetitionRound.h
* @author : 秦其勇
* created : 2010-4-29   
* purpose : 小组赛操作类
            此类继承自CCompetitionRoundBase 
            用来处理小组赛相关的操作
*/
//==========================================================================

#ifndef __GROUP_COMPETITION_ROUND_H__
#define __GROUP_COMPETITION_ROUND_H__

#include <vector>
#include <hash_map>
#include "ISocialGlobal.h"
#include "IDBEngineService.h"
#include "LegendCup.h"
#include "CompetitionRoundBase.h"

class CGroupCompetitionRound : public CCompetitionRoundBase
{	
public:
	CGroupCompetitionRound();
	virtual ~CGroupCompetitionRound();

	void Release();
    // 结束本轮比赛
    virtual bool endRound();

    // 设置本轮的参赛战队 ( bBreakSort 是否需要打乱顺序 bStartRound 是不是比赛开始的轮数)
    virtual bool setKinListAndGroup(list<DWORD> dwKinList,bool bBreakSort = false, bool bStartRound = false);

    // 裁定比赛
    virtual bool judgeNodeWarResult(SMsgJudgeWarResult* pJudgeInfo);

private:
    // 战队分组
    bool groupKinList(int nRoundID, int nMaxGroup, int nGroupMaxKin,list<DWORD> KinList, bool bStartRound = false);

};
#endif