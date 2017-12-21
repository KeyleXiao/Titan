//==========================================================================
/**
* @file	  : OrderEvent.h
* @author : 秦其勇
* created : 2016-5-16
* purpose : 排行榜事件定义
*/
//==========================================================================
#ifndef __DOrderEvent_H__
#define __DOrderEvent_H__

#include "OrderDef.h"
#include "EventRefBase.h"

/***************************************************************/
///////////////////// 排行榜事件码 SOURCE_TYPE_ORDER////////////////
/***************************************************************/
#pragma pack(1)


// 更新Rank信息

struct SEventOrder_UpdateRank : public event_base
{
    PDBID   ActorID;
    BYTE    nMatchType;
    DWORD   nRankScore;
    DWORD   nGrade;
};





#pragma pack()

#endif