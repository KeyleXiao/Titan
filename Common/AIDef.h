/*******************************************************************
** 文件名:	AIDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	12/13/2014
** 版  本:	1.0
** 描  述:	AI公用定义

			
********************************************************************/


#ifndef __COMMON_AI_DEF_H__
#define __COMMON_AI_DEF_H__

#pragma pack(1)

#define SCHEME_MONSTER_AI_CONFIG "Scp\\AIMonsterConfig.csv"

#define SCHEME_HERO_AI_CONFIG "Scp\\AIHero.csv"

#define  MAX_AICONFIG_COUNT			4

#define  MAX_AIRANK_COUNT			4

#define  MAX_AICONFIG_DES			16

struct SchemeAIInfo
{
    int  nAIID;
    char szAIDes[MAX_AICONFIG_DES];
    SchemeAIInfo()
    {
        memset(this, 0, sizeof(*this));
    };
};
struct HeroAISchemeNode
{
    bool bOpen;
    int nAIRankCount;
    int nRankList[MAX_AIRANK_COUNT];
    SchemeAIInfo AIInfoList[MAX_AICONFIG_COUNT];
    int nDisconnectBackAI; // 掉线回城AI
    int nDiscconnectBackAITime; // 掉线回城AI持续时间(超时后用正常AI替代)
    HeroAISchemeNode()
	{
		memset(this, -1, sizeof(*this));
        bOpen = false;
	};
};


#pragma pack()
#endif	// __COMMON_AI_DEF_H__