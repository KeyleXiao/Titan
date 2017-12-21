/*******************************************************************
** 文件名:	E:\E:\speed\Common\LegendCupSchemeDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/18/2015	16:36
** 版  本:	1.0
** 描  述:	
            比赛 文件数据定义
********************************************************************/

#pragma once

#include <string>
#include "LegendCup_ManagedDef.h"
using namespace LegendCup_ManagedDef;
// 比赛大厅房间配置
#define LEGENDCUP_SCHEME_FILENAME "Scp\\LegendCup.csv"

#define LEGENDCUP_DETAIL_FILENAME "Scp\\LegendCupDetail.csv"

#define LEGENDCUP_PRIZE_FILENAME "Scp\\LegendCupPrizeConfig.csv"

#define LEGENDCUP_LIMIT_FILENAME "Scp\\LegendCupLimit.csv"

#define MAX_END_REGISTER_COUT   5
#define LEGENDCUP_DES_MAX_SIZE  32
#define TIME_DES_COUNT_SIZE     3
#define MAX_RANK_RATIO_COUNT    64
#define CUP_PRIZEID_COUNT_SIZE     6
#define EVERY_MATCH_PRIZE_ID_MAX_SIZE  5

struct SSchemeLegendCup
{

    // 杯赛id
    int         m_nConfigID;

    // 杯赛描述
    char        m_szLegendCupDes[LEGENDCUP_DES_MAX_SIZE];

	// 轮数
	BYTE	    m_byRoundCount;

	// 最大战队数量
	int		    m_nMaxKinCount;

    //配置类型ID
    int         m_nDetailID;

	//确定报名截至时间
	int			m_nComfirmRegisterDeadLine[MAX_END_REGISTER_COUT];
    
    //报名截至时间提前多少(小时)
	int			m_nRegisterDeadLine;

    //线上比赛类型
	int			m_nOnlineMatchType;

    //线下比赛类型
	int			m_nOfflineMatchType;

    //需要的金币
    int			m_nNeedMoneyGold;

    //报名需要的金币
    int			m_nRegisterNeedGold;

    //报名最高的金币
    int			m_nHighestRegisterGold;

    //排名分配比例
    int         m_nPrizeID;

    //是否免费
    bool         m_bIsFree;

    //最少开始比赛的战队数量
    int			m_nMiniStartKinCount;


    SSchemeLegendCup()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SSchemeLegendCupPrize
{

    //奖励ID
    int m_nPrizeID;

    //名次
    int m_nRankID;

    //奖励ID列表
    int m_nPrizeList[CUP_PRIZEID_COUNT_SIZE];
    //奖金分配比例
    BYTE m_byPrizeRatio;

    SSchemeLegendCupPrize()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SSchemeLegendCupLimit
{

    // 杯赛id
    int         m_nConfigID;

    //联盟杯赛限定
    int         m_nLevelLimitPersonCount;   // 限定多少等级的多少人才能才能举办联盟杯赛
    // 等级
    int	        m_nLevelLimit;  

    // 周活跃度限制
    int         m_nLimitWeekActivity;

    // 失败描述ID
    int	        m_nFailedDesID;

    SSchemeLegendCupLimit()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SSchemeLegendCupDetail
{

    //比赛配置类型	
    int     m_nDetailID;
    //第几轮
    int       m_nRoundID;
    //该轮比赛类型（0空1淘汰2小组）
    int       m_nCompetitionType;
    //小组数量
    int       m_nMaxGroup;
    //每组战队数量
    int       m_nGroupMaxKin;
    //晋级成员数量
    int       m_nPromotionKinCount;
    //开始时间天数间隔
    int       m_nStartDayInterval;
    
    // 提前进场时间(分钟)
    int       m_nPreEnterTime;    

    //开始时间点
    int       m_nCreateRoomTime[TIME_DES_COUNT_SIZE];

    //开始时间点
    int       m_nStartTime[TIME_DES_COUNT_SIZE];

    //结束时间天数间隔
    int       m_nEndDayInterval;

    //本轮可裁定结束时间点
    int       m_nResetEndTime[TIME_DES_COUNT_SIZE];

    //本轮结束时间点
    int       m_nEndTime[TIME_DES_COUNT_SIZE];
    //比赛类型(bo1 bo3 bo5)	
    int       m_nBoType;
    //bo赛的时间间隔(分钟)
    int       m_nBoInterval;

    //奖励ID列表
    int       m_nPrizeIDList[EVERY_MATCH_PRIZE_ID_MAX_SIZE];

    SSchemeLegendCupDetail()
    {
        memset(this, 0, sizeof(*this));
    }
};
