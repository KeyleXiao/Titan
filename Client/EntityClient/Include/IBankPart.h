/*******************************************************************
** 文件名:	IBankPart.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015/8/27
** 版  本:	1.0
** 描  述:	实体数据仓库部件

********************************************************************/

#pragma once

#include "IEntity.h"
#include "SchemeDef.h"


// 实体数据仓库部件
struct IBankPart : public IEntityPart
{
	// 取得来源游戏世界ID
	virtual DWORD   getWorldID(void) = 0;

    // 获取当前拥有的英雄信息
    virtual int     getPlayerHeroArray(SHeroInfo* pArrHero, int nArrayCount) = 0;
    // 获得系统英雄
    virtual int*    getSystemHeros(WORD& nCount) = 0;
    
    // 根据卡牌id获得对应的卡牌信息
    virtual DBREQ_PARAM_HERO_CARD getHeroCardInfo( int nCardID ) = 0;
    // 获得某个英雄对应的卡牌信息
    virtual SHeroStarSchemeInfo* getHeroCardInfoConfig(int nHeroID ) = 0;
    // 获得玩家信息
    virtual const SPlayerDatabasesInfo* getPlayerDatabasesInfo() = 0;

    // 获得某个英雄的信息
    virtual SHeroInfo*  getHeroInfoByID(int nHeroID) = 0;

    // 是否是系统临时可用英雄
    virtual bool    isSystemHero(int nHeroID) = 0;

	// 获取当前战场可用的英雄信息
	virtual int     getPlayerCanUseHeroArray(SHeroInfo* pArrHero, int nArrayCount) = 0;

	// 获得某个英雄的信息 (自己拥有的和周免的)
	virtual SHeroInfo*  getPossessOrFreeHeroInfoByID(int nHeroID) = 0;

    // 获得引导数据
    virtual int     getPlayerNoviceGuideTypeData() = 0;

	/* 获取本命英雄引导完成情况数据，表示是否走完本命英雄的引导流程
	* Return: 1表示已走完完所有本命英雄，0表示还未完成
	*/
	virtual int     getLifeHeroTaskData() = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
//extern "C" IDiePart * createDiePart();