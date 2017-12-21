//==========================================================================
/**
* @file	  : LegendCupEvent.h
* @author : 秦其勇
* created : 2016-5-16
* purpose : 排行榜事件定义
*/
//==========================================================================
#ifndef __DLegendCupEvent_H__
#define __DLegendCupEvent_H__

#include "LegendCupDef.h"
#include "EventRefBase.h"

/***************************************************************/
///////////////////// 排行榜事件码 SOURCE_TYPE_LEDENDCUP////////////////
/***************************************************************/
#pragma pack(1)


// 更新Rank信息

struct SEventLegendCup_UpdateRank : public event_base
{
    
};

#pragma pack()

#endif