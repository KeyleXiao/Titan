/*******************************************************************
** 文件名:	ISchemeCenter.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	9/1/2014
** 版  本:	1.0
** 描  述:	
            游戏脚本配置中心
********************************************************************/


#include "stdafx.h"
#include "SchemeCenter.h"
#include "SchemeSkinInfo.h"
#include "SchemePersonModel.h"
#include "SchemeMonster.h"
#include "SchemeAllMapsArea.h"
#include "SchemeMapInfo.h"
#include "SchemeMapLand.h"
#include "SchemeSceneMatron.h"
#include "SchemeTankInfo.h"
#include "SchemeLevelUpgrade.h"
#include "SchemePathFindService.h"
#include "SchemePersonBaseProp.h"
#include "SchemeWarPoint.h"
#include "SchemeWarManager.h"
#include "SchemeWarInfo.h"
#include "SchemePatrol.h"
#include "SchemeLandBuff.h"
#include "SchemeMatchRoom.h"
#include "SchemeTalent.h"
#include "SchemeEntity.h"
#include "SchemeWarPersonConfigInfo.h"
#include "SchemeStaminaBuff.h"
#include "SchemeMagicType.h"
#include "SchemeSpellUpgrade.h"
#include "SchemeMobaGoods.h"
#include "SchemeSpellSlot.h"
#include "SchemeSummoner.h"
#include "SchemeWarEvaluate.h"
#include "SchemeSlotMapedShortcut.h"
#include "SchemeMatchRankConfig.h"
#include "SchemeWing.h"
#include "SchemeDynamicObstacle.h"
#include "SchemeSystemMessage.h"
#include "SchemeTalentPage.h"
#include "SchemeTalentPageEffect.h"
#include "SchemeRuneInfo.h"
#include "SchemeRuneConfig.h"
#include "SchemeRunePage.h"
#include "SchemePlayerGloryInfo.h"
#include "SchemeHeroStarData.h"
#include "SchemeInitialHero.h"
#include "SchemeKinDataCenter.h"
#include "SchemeClanDataCenter.h"
#include "SchemeAIIDTable.h"
#include "SchemeHeroCard.h"
#include "SchemeHeroCardCreat.h"
#include "SchemeMatchCoefficient.h"
#include "SchemeExpOrGoldCreate.h"
#include "SchemeFightingCapacityConfig.h"
#include "SchemeHeroAchievement.h"
#include "SchemeOutputRestriction.h"
#include "SchemeHeroTalentConfig.h"
#include "SchemeSpeedDecay.h"
#include "SchemeActorPrize.h"
#include "SchemePrizeChest.h"
#include "SchemePlayerKill.h"
#include "SchemePointShop.h"
#include "SchemePrizeUnlock.h"
#include "SchemeHeroFreeWeekly.h"
#include "SchemeBenefitCard.h"
#include "SchemeAIBuyItem.h"
#include "SchemeCrownGemstoneInfo.h"
#include "SchemeLegendCup.h"
#include "SchemeMentorshipDataCenter.h"
#include "SchemeWarEndSupportPlayer.h"
#include "SchemeHeroXPSkill.h"
#include "SchemeChatCD.h"
#include "SchemeTask.h"
#include "SchemeEconomyControl.h"
#include "SchemeHeroRecommendPath.h"
#include "SchemeWarForceEnd.h"
#include "SchemeGuideStep.h"
#include "SchemeWarSubFunc.h"
#include "SchemeWarOftenUseEquip.h"
#include "SchemeWarGodRecommdEquip.h"
#include "SchemeMatchRankLimit.h"
#include "SchemeHeroReliveTime.h"
#include "SchemeActivityCenter.h"
#include "SchemeWarAddBuff.h"
#include "SchemeAIEcmAdjust.h"
#include "SchemeLifeHero.h"
#include "SchemeAIEcmExpCtrl.h"
#include "SchemeWarSafetyZone.h"
#include "SchemeWarRandRefreshMonster.h"
#include "SchemeWarDevilKing.h"
#include "SchemeWarDevilKingNPC.h"
#include "SchemeWebUrl.h"
#include "SchemeMatchSeasonTime.h"
#include "SchemeMatchSeasonPrize.h"
#include "SchemeRecommPrize.h"

extern "C" __declspec(dllexport) ISchemeCenter * createSchemeCenter()
{
	return new SchemeCenter;
}


bool SchemeCenter::load()
{
    TraceLn("Load SchemeMatchRoom...");
	CSchemeMatchRoom* pSchemeMatchRoom = CSchemeMatchRoom::NewInstance();
	if(pSchemeMatchRoom == NULL || !pSchemeMatchRoom->LoadScheme())
	{
		safeDelete(pSchemeMatchRoom);
		return false;
	}

    TraceLn("Load PathFindServiceInfo...");
	CSchemePathFindServiceInfo* pSchemePathFindServiceInfo = CSchemePathFindServiceInfo::NewInstance();
	if(pSchemePathFindServiceInfo == NULL || !pSchemePathFindServiceInfo->LoadScheme())
	{
		safeDelete(pSchemePathFindServiceInfo);
		return false;
	}

	// 加载皮肤配置
    TraceLn("Load SchemeSkinInfo...");
	CSchemeSkinInfo* pSchemeSkinInfo = CSchemeSkinInfo::NewInstance();
	if(pSchemeSkinInfo == NULL || !pSchemeSkinInfo->LoadScheme())
	{
		safeDelete(pSchemeSkinInfo);
		return false;
	}

	// 加载人物皮肤模型数据
    TraceLn("Load SchemePersonModel...");
	CSchemePersonModel* pSchemePersonModel = CSchemePersonModel::NewInstance();
	if(pSchemePersonModel == NULL || !pSchemePersonModel->LoadScheme())
	{
		safeDelete(pSchemePersonModel);
		return false;
	}

	// 加载怪物配置数据
    TraceLn("Load SchemeMonster...");
	CSchemeMonster* pSchemeMonster = CSchemeMonster::NewInstance(); 
	if(pSchemeMonster == NULL || !pSchemeMonster->LoadScheme())
	{
		safeDelete(pSchemeMonster);
		return false;
	}

	// 加载巡逻配置数据
    TraceLn("Load SchemePatrol...");
	CSchemePatrol* pSchemePatrol = CSchemePatrol::NewInstance(); 
	if(pSchemePatrol == NULL || !pSchemePatrol->LoadScheme())
	{
		safeDelete(pSchemePatrol);
		return false;
	}

	// 加载Tank配置数据
    TraceLn("Load SchemeTankInfo...");
	CSchemeTankInfo* pSchemeTankInfo = CSchemeTankInfo::NewInstance(); 
	if(pSchemeTankInfo == NULL || !pSchemeTankInfo->LoadScheme())
	{
		safeDelete(pSchemeTankInfo);
		return false;
	}

    // 地图信息解析
    TraceLn("Load SchemeMapInfo...");
	CSchemeMapInfo* pSchemeMapInfo = CSchemeMapInfo::NewInstance(); 
	if(pSchemeMapInfo == NULL || !pSchemeMapInfo->LoadScheme())
	{
		safeDelete(pSchemeMapInfo);
		return false;
	}
    TraceLn("Load SchemeMapLand...");
	CSchemeMapLand* pSchemeMapLand = CSchemeMapLand::NewInstance(); 
	if(pSchemeMapLand == NULL || !pSchemeMapLand->LoadScheme(pSchemeMapInfo))
	{
		safeDelete(pSchemeMapLand);
		return false;
	}
    TraceLn("Load SchemeMapArea...");
    CSchemeAllMapsArea* pSchemeMapArea = CSchemeAllMapsArea::NewInstance(); 
    if(pSchemeMapArea == NULL || !pSchemeMapArea->LoadScheme())
    {
        safeDelete(pSchemeMapArea);
        return false;
    }

    // 场景保姆
    TraceLn("Load SchemeSceneMatron...");
    CSchemeSceneMatron* pSchemeSceneMatron = CSchemeSceneMatron::NewInstance(); 
    if(pSchemeSceneMatron == NULL || !pSchemeSceneMatron->LoadScheme())
    {
        safeDelete(pSchemeSceneMatron);
        return false;
    }    

	// 加载升级配置数据
    TraceLn("Load SchemeLevelUpgrade...");
	CSchemeLevelUpgrade* pSchemeLevelUpgrade = CSchemeLevelUpgrade::NewInstance(); 
	if(pSchemeLevelUpgrade == NULL || !pSchemeLevelUpgrade->LoadScheme())
	{
		safeDelete(pSchemeLevelUpgrade);
		return false;
	}
	// 加载人物基础数据配置
    TraceLn("Load SchemePersonBaseProp...");
	CSchemePersonBaseProp* pSchemePersonBaseProp = CSchemePersonBaseProp::NewInstance(); 
	if(pSchemePersonBaseProp == NULL || !pSchemePersonBaseProp->LoadScheme())
	{
		safeDelete(pSchemePersonBaseProp);
		return false;
	}

	// 加载战场管理
	TraceLn("Load WarManager...");
	CSchemeWarManager* pSchemeWarManager = CSchemeWarManager::NewInstance();
	if(pSchemeWarManager == NULL || !pSchemeWarManager->LoadScheme())
	{
		safeDelete(pSchemeWarManager);
		return false;
	}
	// 加载战场信息
	TraceLn("Load WarInfo...");
	CSchemeWarInfo* pSchemeWarInfo = CSchemeWarInfo::NewInstance();
	if(pSchemeWarInfo == NULL || !pSchemeWarInfo->LoadScheme(pSchemeWarManager))
	{
		safeDelete(pSchemeWarInfo);
		return false;
	}
	// 加载战场怪物
	TraceLn("Load SchemeWarPoint...");
	CSchemeWarPoint* pSchemeWarPoint = CSchemeWarPoint::NewInstance();
	if(pSchemeWarPoint == NULL || !pSchemeWarPoint->LoadScheme())
	{
		safeDelete(pSchemeWarPoint);
		return false;
	}

	// 加载实体着陆BUFF配置
	CSchemeLandBuff* pSchemeLandBuff = CSchemeLandBuff::NewInstance();
	if(pSchemeLandBuff == NULL || !pSchemeLandBuff->LoadScheme())
	{
		safeDelete(pSchemeLandBuff);
		return false;
	}

	CSchemeEntity *pSchemeEntity = CSchemeEntity::NewInstance();
	if(pSchemeEntity == NULL || !pSchemeEntity->LoadScheme())
	{
		safeDelete(pSchemeEntity);
		return false;
	}
    // 加载天赋配置
    CTalentScheme* pSchemeTalent = CTalentScheme::NewInstance();
    if(pSchemeTalent == NULL || !pSchemeTalent->Load())
    {
        safeDelete(pSchemeTalent);
        return false;
    }

	// 加载战场人物掉落配置
	TraceLn("Load WarPersonConfigInfo...");
	CSchemeWarPersonConfigInfo* pSchemeWarPersonConfigInfo = CSchemeWarPersonConfigInfo::NewInstance();
	if(pSchemeWarPersonConfigInfo == NULL || !pSchemeWarPersonConfigInfo->LoadScheme())
	{
		safeDelete(pSchemeWarPersonConfigInfo);
		return false;
	}

	// 加载体力buff配置
	CSchemeStaminaBuff* pSchemeStaminaBuff = CSchemeStaminaBuff::NewInstance(); 
	if(pSchemeStaminaBuff == NULL || !pSchemeStaminaBuff->LoadScheme())
	{
		safeDelete(pSchemeStaminaBuff);
		return false;
	}

	// 加载魔法类型配置
	CSchemeMagicType* pSchemeMagicType = CSchemeMagicType::NewInstance(); 
	if(pSchemeMagicType == NULL || !pSchemeMagicType->LoadScheme())
	{
		safeDelete(pSchemeMagicType);
		return false;
	}

	// 加载技能升级配置
	CSchemeSpellUpgrade* pSchemeSpellUpgrade = CSchemeSpellUpgrade::NewInstance(); 
	if(pSchemeSpellUpgrade == NULL || !pSchemeSpellUpgrade->LoadScheme())
	{
		safeDelete(pSchemeSpellUpgrade);
		return false;
	}

	// 物品配置
	CSchemeMobaGoods* pSchemeMobaGoods = CSchemeMobaGoods::NewInstance(); 
	if(pSchemeMobaGoods == NULL || !pSchemeMobaGoods->LoadScheme())
	{
		safeDelete(pSchemeMobaGoods);
		return false;
	}

	// 技能槽位配置
	CSchemeSpellSlot* pSchemeSpellSlot = CSchemeSpellSlot::NewInstance(); 
	if(pSchemeSpellSlot == NULL || !pSchemeSpellSlot->LoadScheme())
	{
		safeDelete(pSchemeSpellSlot);
		return false;
	}

    // 加载召唤师技能平配置表
    CSchemeSummonerSkill* pSchemeSkill = CSchemeSummonerSkill::NewInstance();
    if ( pSchemeSkill == NULL || !pSchemeSkill->LoadScheme())
    {
        safeDelete(pSchemeSkill);
        return false;
    }

    // 加载快捷键配置表
    CSchemeSlotMapedShortcut* pSchemeSlot = CSchemeSlotMapedShortcut::NewInstance();
    if ( pSchemeSlot == NULL || !pSchemeSlot->LoadScheme())
    {
        safeDelete(pSchemeSlot);
        return false;
    }

    // 战场分数系数配置表
    CSchemeWarEvaluate* pSchemeEvaluate = CSchemeWarEvaluate::NewInstance();
    if ( pSchemeEvaluate == NULL || !pSchemeEvaluate->LoadScheme())
    {
        safeDelete(pSchemeEvaluate);
        return false;
    }

    // 撮合匹配配置
    CSchemeMatchRankConfig* pSchemeMatchRankConfig = CSchemeMatchRankConfig::NewInstance();
    if ( pSchemeMatchRankConfig == NULL || !pSchemeMatchRankConfig->LoadScheme())
    {
        safeDelete(pSchemeMatchRankConfig);
        return false;
    }

    // 加载翅膀配置
    CSchemeWing* pSchemeWing = CSchemeWing::NewInstance(); 
    if(pSchemeWing == NULL || !pSchemeWing->LoadScheme())
    {
        safeDelete(pSchemeWing);
        return false;
    }

    // 加载动态障碍配置
    TraceLn("Load CSchemeDynamicObstacle...");
    CSchemeDynamicObstacle* pSchemeDynObs = CSchemeDynamicObstacle::NewInstance(); 
    if(pSchemeDynObs == NULL || !pSchemeDynObs->LoadScheme())
    {
        safeDelete(pSchemeDynObs);
        return false;
    }

    // 加载飘字提示配置
    TraceLn("Load CSchemeSystemMessage...");
    CSchemeSystemMessage* pSchemeSystemMessage = CSchemeSystemMessage::NewInstance(); 
    if(pSchemeSystemMessage == NULL || !pSchemeSystemMessage->LoadScheme())
    {
        safeDelete(pSchemeSystemMessage);
        return false;
    }

#ifdef FUNCTION_TALENTPAGE_OPEN
    // 加载天赋页配置
    TraceLn("Load CSchemeTalentPage...");
    CSchemeTalentPage* pSchemeTalentPage = CSchemeTalentPage::NewInstance(); 
    if(pSchemeTalentPage == NULL || !pSchemeTalentPage->LoadScheme())
    {
        safeDelete(pSchemeTalentPage);
        return false;
    }

    // 加天赋效果配置
    TraceLn("Load pSchemeTalentPageEffect...");
    CSchemeTalentPageEffect* pSchemeTalentPageEffect = CSchemeTalentPageEffect::NewInstance(); 
    if(pSchemeTalentPageEffect == NULL || !pSchemeTalentPageEffect->LoadScheme())
    {
        safeDelete(pSchemeTalentPageEffect);
        return false;
    }
#endif

#ifdef FUNCTION_RUNE_OPEN
    // 加载符文信息配置
    TraceLn("Load CSchemeRuneInfo...");
    CSchemeRuneInfo* pSchemeRuneInfo = CSchemeRuneInfo::NewInstance();
    if(pSchemeRuneInfo == NULL || !pSchemeRuneInfo->LoadScheme())
    {
        safeDelete(pSchemeRuneInfo);
        return false;
    }

    // 加载玩家符文配置信息
    TraceLn("Load CSchemeRuneConfig...");
    CSchemeRuneConfig* pSchemeRuneConfig = CSchemeRuneConfig::NewInstance();
    if(pSchemeRuneConfig == NULL || !pSchemeRuneConfig->LoadScheme())
    {
        safeDelete(pSchemeRuneConfig);
        return false;
    }

    // 加载玩家符文页配置信息
    TraceLn("Load CSchemeRunePage...");
    CSchemeRunePage* pSchemeRunePage = CSchemeRunePage::NewInstance(); 
    if(pSchemeRunePage == NULL || !pSchemeRunePage->LoadScheme())
    {
        safeDelete(pSchemeRunePage);
        return false;
    }
#endif // #ifdef FUNCTION_RUNE_OPEN

	// 加载玩家符文页配置信息
	TraceLn("Load CSchemeHeroTalentConfig...");
	CSchemeHeroTalentConfig* pSchemHeroTalentPage = CSchemeHeroTalentConfig::NewInstance(); 
	if(pSchemHeroTalentPage == NULL || !pSchemHeroTalentPage->LoadScheme())
	{
		safeDelete(pSchemHeroTalentPage);
		return false;
	}

    // 加载成就需求配置
    TraceLn("Load CSchemePlayerGloryInfo....");
    CSchemePlayerGloryInfo* pSchemeGlory = CSchemePlayerGloryInfo::NewInstance();
    if ( pSchemeGlory == NULL || !pSchemeGlory->LoadScheme())
    {
        safeDelete(pSchemeGlory);
        return false;
    }

    // 加载英雄星级配置信息
    TraceLn("Load CSchemeHeroStarData...");
    CSchemeHeroStarData* pSchemeHeroStar = CSchemeHeroStarData::NewInstance();
    if ( pSchemeHeroStar == NULL || !pSchemeHeroStar->LoadScheme())
    {
        safeDelete(pSchemeHeroStar);
        return false;
    }

    // 加载创建角色时初始英雄配置信息
    TraceLn("Load CSchemeInitialHero...");
    CSchemeInitialHero* pSchemeInitialHero = CSchemeInitialHero::NewInstance();
    if ( pSchemeInitialHero == NULL || !pSchemeInitialHero->LoadScheme() )
    {
        safeDelete(pSchemeInitialHero);
        return false;
    }

  // 加载战队配置信息
    TraceLn("Load CSchemeKinDataCenter...");
    CSchemeKinDataCenter* pSchemeKinDataCenter = CSchemeKinDataCenter::NewInstance();
    if ( pSchemeKinDataCenter == NULL || !pSchemeKinDataCenter->LoadScheme() )
    {
        safeDelete(pSchemeKinDataCenter);
        return false;
    }

	 // 加载帮会配置信息
    TraceLn("Load CSchemeClanDataCenter...");
    CSchemeClanDataCenter* pSchemeClanDataCenter = CSchemeClanDataCenter::NewInstance();
    if ( pSchemeClanDataCenter == NULL || !pSchemeClanDataCenter->LoadScheme() )
    {
        safeDelete(pSchemeClanDataCenter);
        return false;
    }

    // 加载AI配置列表
    TraceLn("Load CAISchemeList...");
    CSchemeAIIDTable* pSchemeAIList = CSchemeAIIDTable::NewInstance();
    if(pSchemeAIList == NULL || !pSchemeAIList->LoadScheme())
    {
        safeDelete(pSchemeAIList);
        return false;
    }

    // 加载英雄卡牌配置信息
    TraceLn("Load CSchemeHeroCard...");
    CSchemeHeroCard* pSchemeHeroCard = CSchemeHeroCard::NewInstance();
    if ( pSchemeHeroCard == NULL || !pSchemeHeroCard->LoadScheme())
    {
        safeDelete(pSchemeHeroCard);
        return false;
    }

    // 加载HeroCardCreat配置列表
    TraceLn("Load HeroCardCreat...");
    CSchemeHeroCardCreat* pSchemeHeroCardCreat = CSchemeHeroCardCreat::NewInstance();
    if ( pSchemeHeroCardCreat == NULL || ! pSchemeHeroCardCreat->LoadScheme())
    {
        safeDelete(pSchemeHeroCardCreat );
        return false;
    }

    // 加载战场系数
    TraceLn("Load CSchemeMatchCoefficient...");
    CSchemeMatchCoefficient* pSchemeMatchCoeff = CSchemeMatchCoefficient::NewInstance();
    if ( pSchemeMatchCoeff == NULL || !pSchemeMatchCoeff->LoadScheme() )
    {
        safeDelete(pSchemeMatchCoeff);
        return false;
    }

    // 加载金钱金币产出配置信息
    TraceLn("Load ExpOrGoldCreate...");
    CSchemeExpOrGoldCreate* pSchemeExpOrGold = CSchemeExpOrGoldCreate::NewInstance();
    if ( pSchemeExpOrGold == NULL || !pSchemeExpOrGold->LoadScheme())
    {
        safeDelete(pSchemeExpOrGold);
        return false;
    }

    CSchemeFightingCapacityConfig* pSchemeFightConfig = CSchemeFightingCapacityConfig::NewInstance();
    if ( pSchemeFightConfig == NULL || !pSchemeFightConfig->LoadScheme())
    {
        safeDelete(pSchemeFightConfig);
        return false;
    }

    // 加载英雄成就配置信息
    TraceLn("Load SchemeHeroAchievement...");
    CSchemeHeroAchievement* pSchemeHeroAchievement = CSchemeHeroAchievement::NewInstance();
    if ( pSchemeHeroAchievement == NULL || !pSchemeHeroAchievement->LoadScheme())
    {
        safeDelete(pSchemeHeroAchievement);
        return false;
    }

    // 加载产出限制配置信息
    TraceLn("Load CSchemeOutputRestriction...");
    CSchemeOutputRestriction* pSchemeOutputRestriction = CSchemeOutputRestriction::NewInstance();
    if ( pSchemeOutputRestriction == NULL || !pSchemeOutputRestriction->LoadScheme())
    {
        safeDelete(pSchemeOutputRestriction);
        return false;
    }

    TraceLn("Load CSchemeSpeedDecay...");
    CSchemeSpeedDecay* pSchemeSpeedDecay = CSchemeSpeedDecay::NewInstance();
    if(pSchemeSpeedDecay == NULL || !pSchemeSpeedDecay->LoadScheme())
    {
        safeDelete(pSchemeSpeedDecay);
        return false;
    }

	// 加载角色奖励配置信息
	TraceLn("Load CSchemeActorPrize...");
	CSchemeActorPrize* pSchemeActorPrize = CSchemeActorPrize::NewInstance();
	if ( pSchemeActorPrize == NULL || !pSchemeActorPrize->LoadScheme())
	{
		safeDelete(pSchemeActorPrize);
		return false;
	}

	// 加载宝箱配置信息
	TraceLn("Load CSchemePrizeChest...");
	CSchemePrizeChest* pSchemePrizeChest = CSchemePrizeChest::NewInstance();
	if ( pSchemePrizeChest == NULL || !pSchemePrizeChest->LoadScheme())
	{
		safeDelete(pSchemePrizeChest);
		return false;
	}

    TraceLn("Load CSchemePlayerKill...");
    CSchemePlayerKill* pSchemePlayerKill = CSchemePlayerKill::NewInstance();
    if(pSchemePlayerKill == NULL || !pSchemePlayerKill->LoadScheme())
    {
        safeDelete(pSchemePlayerKill);
        return false;
    }

	// 加载商城配置信息
	TraceLn("Load CSchemePointShop...");
	CSchemePointShop* pSchemePointShop = CSchemePointShop::NewInstance();
	if ( pSchemePointShop == NULL || !pSchemePointShop->LoadScheme())
	{
		safeDelete(pSchemePointShop);
		return false;
	}
	
	// 加载解锁配置信息
    TraceLn("Load pSchemePrizeUnlock...");
    CSchemePrizeUnlock* pSchemePrizeUnlock = CSchemePrizeUnlock::NewInstance();
    if ( pSchemePrizeUnlock == NULL || !pSchemePrizeUnlock->LoadScheme())
    {
        safeDelete(pSchemePrizeUnlock);
        return false;
    }

	// 加载周免英雄配置信息
	TraceLn("Load CSchemeHeroFreeWeekly...");
	CSchemeHeroFreeWeekly* pCSchemeHeroFreeWeekly = CSchemeHeroFreeWeekly::NewInstance();
	if ( pCSchemeHeroFreeWeekly == NULL || !pCSchemeHeroFreeWeekly->LoadScheme())
	{
		safeDelete(pCSchemeHeroFreeWeekly);
		return false;
	}

	// 加载增益卡配置信息
	TraceLn("Load CSchemeBenefitCard...");
	CSchemeBenefitCard *pCSchemeBenefitCard = CSchemeBenefitCard::NewInstance();
	if (NULL == pCSchemeBenefitCard || !pCSchemeBenefitCard->LoadScheme())
	{
		safeDelete(pCSchemeBenefitCard);
		return false;
	}

    // 加载AI购买物品配置
    TraceLn("Load AI Shopping Scheme...");
    CAIBuyItemScheme* pSchemeAIBuyItem = CAIBuyItemScheme::NewInstance();
    if(pSchemeAIBuyItem == NULL || !pSchemeAIBuyItem->Load())
    {
        safeDelete(pSchemeAIBuyItem);
        //return false;
    }

	// 加载皇冠主配置
	TraceLn("Load CrownGemstoneInfo Scheme...");
	CSchemeCrownGemstoneInfo* pSchemeCrownPage = CSchemeCrownGemstoneInfo::NewInstance();
	if(pSchemeCrownPage == NULL || !pSchemeCrownPage->LoadScheme())
	{
		safeDelete(pSchemeCrownPage);
		//return false;
	}

	// 加载师徒配置信息
	TraceLn("Load CSchemeMentorshipDataCenter...");
	CSchemeMentorshipDataCenter* pSchemeMentorshipDataCenter = CSchemeMentorshipDataCenter::NewInstance();
	if ( pSchemeMentorshipDataCenter == NULL || !pSchemeMentorshipDataCenter->LoadScheme() )
	{
		safeDelete(pSchemeMentorshipDataCenter);
		return false;
	}
	
	// 获得点赞配置
	TraceLn("Load CSchemeWarEndSupportPlayer Scheme...");
	CSchemeWarEndSupportPlayer* pSchemeWarEndSupport = CSchemeWarEndSupportPlayer::NewInstance();
	if(pSchemeWarEndSupport == NULL || !pSchemeWarEndSupport->LoadScheme())
	{
		safeDelete(pSchemeWarEndSupport);
		return false;
	}

	// 获得XP赞配置
	TraceLn("Load CSchemeHeroXpSkill Scheme...");
	CSchemeHeroXpSkill* pSchemeHeroXpSkill = CSchemeHeroXpSkill::NewInstance();
	if(pSchemeHeroXpSkill == NULL || !pSchemeHeroXpSkill->LoadScheme())
	{
		safeDelete(pSchemeHeroXpSkill);
		return false;
	}

    TraceLn("Load SchemeLegendCup Scheme...");
    CSchemeLegendCup* pSchemeLegendCup = CSchemeLegendCup::NewInstance();
    if(pSchemeLegendCup == NULL || !pSchemeLegendCup->LoadScheme())
    {
        safeDelete(pSchemeLegendCup);
        return false;
    }

	TraceLn("Load SchemeChatCD Scheme...");
    CSchemeChatCD* pSchemeChatCD = CSchemeChatCD::NewInstance();
    if(pSchemeChatCD == NULL || !pSchemeChatCD->LoadScheme())
    {
        safeDelete(pSchemeChatCD);
        return false;
    }

    // 加载任务配置
    TraceLn("Load SchemeTask Scheme...");
    CSchemeTask* pSchemeTask = CSchemeTask::NewInstance(); 
    if(pSchemeTask == NULL || !pSchemeTask->LoadScheme())
    {
        safeDelete(pSchemeTask);
        return false;
    }

	// 加载战场经济控制配置
	TraceLn("Load SchemeEconomyControl Scheme...");
	CSchemeEconomyControl* pSchemeEconoCtrl = CSchemeEconomyControl::NewInstance();
	if (pSchemeEconoCtrl == NULL || !pSchemeEconoCtrl->LoadScheme())
	{
		safeDelete(pSchemeEconoCtrl);
		return false;
	}

    // 加载英雄推荐线路配置
    TraceLn("Load SchemeHeroRecommendPath Scheme...");
    CHeroRecommendPathScheme* pSchemeRecommendPath = CHeroRecommendPathScheme::NewInstance();
    if (pSchemeRecommendPath == NULL || !pSchemeRecommendPath->Load())
    {
        safeDelete(pSchemeRecommendPath);
        return false;
    }

	// 加载战场强制结算配置
    TraceLn("Load SchemeWarForceEnd Scheme...");
    CSchemeWarForceEnd* pSchemeWarForceEnd = CSchemeWarForceEnd::NewInstance();
    if (pSchemeWarForceEnd == NULL || !pSchemeWarForceEnd->LoadScheme())
    {
        safeDelete(pSchemeWarForceEnd);
        return false;
    }

    // 加载新手引导步骤配置
    TraceLn("Load SchemeGuideStep Scheme...");
    CSchemeGuideStep* pSchemeGuideStep = CSchemeGuideStep::NewInstance();
    if (pSchemeGuideStep == NULL || !pSchemeGuideStep->LoadScheme())
    {
        safeDelete(pSchemeGuideStep);
        return false;
    }

	// 加载战场子功能配置
	TraceLn("Load CSchemeWarSubFunc Scheme...");
	CSchemeWarSubFunc* pSchemeWarSubFunc = CSchemeWarSubFunc::NewInstance();
	if (pSchemeWarSubFunc == NULL || !pSchemeWarSubFunc->LoadScheme())
	{
		safeDelete(pSchemeWarSubFunc);
		return false;
	}

	// 加载战场常用装备配置
	TraceLn("Load CSchemeWarOftenUseEquip Scheme...");
	CSchemeWarOftenUseEquip* pSchemeWarOftenUseEquip = CSchemeWarOftenUseEquip::NewInstance();
	if (pSchemeWarOftenUseEquip == NULL || !pSchemeWarOftenUseEquip->LoadScheme())
	{
		safeDelete(pSchemeWarOftenUseEquip);
		return false;
	}

	// 加载大神推荐常用装备配置
	TraceLn("Load CSchemeWarGodRecommdEquip Scheme...");
	CSchemeWarGodRecommdEquip* pSchemeWarGodRecommdEquip = CSchemeWarGodRecommdEquip::NewInstance();
	if (pSchemeWarGodRecommdEquip == NULL || !pSchemeWarGodRecommdEquip->LoadScheme())
	{
		safeDelete(pSchemeWarGodRecommdEquip);
		return false;
	}

    // 撮合限制相关配置
    TraceLn("Load CSchemeMatchRankLimit Scheme...");
    CSchemeMatchRankLimit* pSchemeMatchRankLimit = CSchemeMatchRankLimit::NewInstance();
    if (pSchemeMatchRankLimit == NULL || !pSchemeMatchRankLimit->LoadScheme())
    {
        safeDelete(pSchemeMatchRankLimit);
        return false;
    }

	// 英雄复活时间相关配置
    TraceLn("Load CSchemeHeroReliveTime Scheme...");
    CSchemeHeroReliveTime* pSchemeHeroReliveTime = CSchemeHeroReliveTime::NewInstance();
    if (pSchemeHeroReliveTime == NULL || !pSchemeHeroReliveTime->LoadScheme())
    {
        safeDelete(pSchemeHeroReliveTime);
        return false;
    }

	// 加载活跃度配置信息
	TraceLn("Load CSchemeActivityCenter...");
	CSchemeActivityCenter* pSchemeActivityCenter = CSchemeActivityCenter::NewInstance();
	if (pSchemeActivityCenter == NULL || !pSchemeActivityCenter->LoadScheme())
	{
		safeDelete(pSchemeActivityCenter);
		return false;
	}

	// 加载添加buf配置信息
	TraceLn("Load CSchemeWarAddBuf...");
	CSchemeWarAddBuff* pSchemeWarAddBuff = CSchemeWarAddBuff::NewInstance();
	if (pSchemeWarAddBuff == NULL || !pSchemeWarAddBuff->LoadScheme())
	{
		safeDelete(pSchemeWarAddBuff);
		return false;
	}

	// 加载AI经济动态调整配置
	TraceLn("Load CSchemeAIEcmAdjust...");
	CSchemeAIEcmAdjust* pSchmeAIEcmAdjust = CSchemeAIEcmAdjust::NewInstance();
	if (pSchmeAIEcmAdjust == NULL || !pSchmeAIEcmAdjust->LoadScheme())
	{
		safeDelete(pSchmeAIEcmAdjust);
		return false;
	}

	// 加载本命英雄配置
	TraceLn("Load CSchemeLifeHero...");
	CSchemeLifeHero* pSchmeLifeHero = CSchemeLifeHero::NewInstance();
	if (pSchmeLifeHero == NULL || !pSchmeLifeHero->LoadScheme())
	{
		safeDelete(pSchmeLifeHero);
		return false;
	}

	// 加载AI经济经验控制配置
	TraceLn("Load CSchemeAIEcmExpCtrl...");
	CSchemeAIEcmExpCtrl* pSchmeAIEcmExpCtrl = CSchemeAIEcmExpCtrl::NewInstance();
	if (pSchmeAIEcmExpCtrl == NULL || !pSchmeAIEcmExpCtrl->LoadScheme())
	{
		safeDelete(pSchmeAIEcmExpCtrl);
		return false;
	}

	// 加载安全圈配置
	TraceLn("Load CSchemeWarSafetyZone...");
	CSchemeWarSafetyZone* pSchemeWarSafetyZone = CSchemeWarSafetyZone::NewInstance();
	if (pSchemeWarSafetyZone == NULL || !pSchemeWarSafetyZone->LoadScheme())
	{
		safeDelete(pSchemeWarSafetyZone);
		return false;
	}

	// 加载随机刷怪配置
	TraceLn("Load CSchemeWarRandRefreshMons...");
	CSchemeWarRandRefreshMons* pSchemeWarRandRefreshMons = CSchemeWarRandRefreshMons::NewInstance();
	if (pSchemeWarRandRefreshMons == NULL || !pSchemeWarRandRefreshMons->LoadScheme())
	{
		safeDelete(pSchemeWarRandRefreshMons);
		return false;
	}

	// 加载魔王玩法配置
	TraceLn("Load CSchemeWarDevilKing...");
	CSchemeWarDevilKing* pSchemeWarDevilKing = CSchemeWarDevilKing::NewInstance();
	if (pSchemeWarDevilKing == NULL || !pSchemeWarDevilKing->LoadScheme())
	{
		safeDelete(pSchemeWarDevilKing);
		return false;
	}

	// 加载魔王玩法配置
	TraceLn("Load CSchemeWarDevilKingNPC...");
	CSchemeWarDevilKingNPC* pSchemeWarDevilKingNPC = CSchemeWarDevilKingNPC::NewInstance();
	if (pSchemeWarDevilKingNPC == NULL || !pSchemeWarDevilKingNPC->LoadScheme())
	{
		safeDelete(pSchemeWarDevilKingNPC);
		return false;
	}

    // 加载WebUrl配置
    TraceLn("Load WebUrl...");
    CSchemeWebUrl* pSchemeWebUrl = CSchemeWebUrl::NewInstance();
    if (pSchemeWebUrl == NULL || !pSchemeWebUrl->LoadScheme())
    {
        safeDelete(pSchemeWebUrl);
        return false;
    }

	// 加载排位赛季时间配置
	TraceLn("Load CSchemeMatchSeasonTime...");
	CSchemeMatchSeasonTime* pSchemeMatchSeasonTime = CSchemeMatchSeasonTime::NewInstance();
	if (pSchemeMatchSeasonTime == NULL || pSchemeMatchSeasonTime->LoadScheme() == false)
	{
		safeDelete(pSchemeMatchSeasonTime);
		return false;
	}

	// 加载排位赛季奖励配置
	TraceLn("Load CSchemeMatchSeasonPrize...");
	CSchemeMatchSeasonPrize* pSchemeMatchSeasonPrize = CSchemeMatchSeasonPrize::NewInstance();
	if (pSchemeMatchSeasonPrize == NULL || pSchemeMatchSeasonPrize->LoadScheme() == false)
	{
		safeDelete(pSchemeMatchSeasonPrize);
		return false;
	}

    // 加载推广奖励配置
    TraceLn("Load CSchemeRecommPrize...");
    CSchemeRecommPrize* pSchemeRecommPrize = CSchemeRecommPrize::NewInstance();
    if (pSchemeRecommPrize == NULL || pSchemeRecommPrize->LoadScheme() == false)
    {
        safeDelete(pSchemeRecommPrize);
        return false;
    }

	return true;
}

void SchemeCenter::release()
{
	// 删除皮肤配置
	CSchemeSkinInfo::CloseAll();
	
	// 删除人物皮肤模型数据
	CSchemePersonModel::CloseAll();

	// 删除怪物配置数据
	CSchemeMonster::CloseAll();
	
	// 删除巡逻配置
	CSchemePatrol::CloseAll();

	// 删除怪物配置数据
	CSchemeTankInfo::CloseAll();


	// 地图信息解析
	CSchemeAllMapsArea::CloseAll();
	CSchemeMapInfo::CloseAll();
	CSchemeMapLand::CloseAll();
    // 场景保姆
    CSchemeSceneMatron::CloseAll();

	CSchemeLevelUpgrade::CloseAll();
	CSchemePersonBaseProp::CloseAll();

	CSchemeWarManager::CloseAll();
	CSchemeWarInfo::CloseAll();
	CSchemeWarPoint::CloseAll();

	// 删除实体着陆BUFF配置
	CSchemeLandBuff::CloseAll();

	CSchemeMatchRoom::CloseAll();
	CSchemePathFindServiceInfo::CloseAll();
	CSchemeEntity::CloseAll();
    CTalentScheme::CloseAll();

	CSchemeStaminaBuff::CloseAll();
	CSchemeMagicType::CloseAll();
	CSchemeSpellUpgrade::CloseAll();
	CSchemeMobaGoods::CloseAll();
	CSchemeSpellSlot::CloseAll();
    CTalentScheme::CloseAll();
    CSchemeSummonerSkill::CloseAll();
    CSchemeSlotMapedShortcut::CloseAll();
    CSchemeWarEvaluate::CloseAll();
    CSchemeMatchRankConfig::CloseAll();
    CSchemeWing::CloseAll();
    CSchemeDynamicObstacle::CloseAll();

    CSchemeSystemMessage::CloseAll();

#ifdef FUNCTION_TALENTPAGE_OPEN
    CSchemeTalentPage::CloseAll();
    CSchemeTalentPageEffect::CloseAll();
#endif

#ifdef FUNCTION_RUNE_OPEN
    CSchemeRuneInfo::CloseAll();
    CSchemeRuneConfig::CloseAll();
    CSchemeRunePage::CloseAll();
#endif // #ifdef FUNCTION_RUNE_OPE

    CSchemeHeroTalentConfig::CloseAll();
    CSchemePlayerGloryInfo::CloseAll();
    CSchemeHeroStarData::CloseAll();
    CSchemeInitialHero::CloseAll();
	CSchemeKinDataCenter::CloseAll();
    CSchemeClanDataCenter::CloseAll();
    CSchemeAIIDTable::CloseAll();
    CSchemeHeroCard::CloseAll();
    CSchemeHeroCardCreat::CloseAll();
    CSchemeMatchCoefficient::CloseAll();
    CSchemeExpOrGoldCreate::CloseAll();
    CSchemeFightingCapacityConfig::CloseAll();
    CSchemeHeroAchievement::CloseAll();
    CSchemeOutputRestriction::CloseAll();
    CSchemeSpeedDecay::CloseAll();
	CSchemeActorPrize::CloseAll();
	CSchemePrizeChest::CloseAll();
    CSchemePlayerKill::CloseAll();
	CSchemePointShop::CloseAll();
    CSchemePrizeUnlock::CloseAll();
	CSchemeHeroFreeWeekly::CloseAll();
	CSchemeBenefitCard::CloseAll();
    CAIBuyItemScheme::CloseAll();
	CSchemeCrownGemstoneInfo::CloseAll();
	CSchemeMentorshipDataCenter::CloseAll();
	CSchemeWarEndSupportPlayer::CloseAll();
	CSchemeHeroXpSkill::CloseAll();
	CSchemeLegendCup::CloseAll();
	CSchemeChatCD::CloseAll();
    CSchemeTask::CloseAll();
	CSchemeEconomyControl::CloseAll();
    CHeroRecommendPathScheme::CloseAll();
	CSchemeWarForceEnd::CloseAll();
    CSchemeGuideStep::CloseAll();
    CSchemeWarSubFunc::CloseAll();
	CSchemeWarOftenUseEquip::CloseAll();
	CSchemeWarGodRecommdEquip::CloseAll();
    CSchemeMatchRankLimit::CloseAll();
	CSchemeHeroReliveTime::CloseAll();
	CSchemeActivityCenter::CloseAll();
	CSchemeWarAddBuff::CloseAll();
	CSchemeAIEcmAdjust::CloseAll();
	CSchemeLifeHero::CloseAll();
	CSchemeAIEcmAdjust::CloseAll();
	CSchemeAIEcmExpCtrl::CloseAll();
	CSchemeWarSafetyZone::CloseAll();
	CSchemeWarRandRefreshMons::CloseAll();
	CSchemeWarDevilKing::CloseAll();
	CSchemeWarDevilKingNPC::CloseAll();
    CSchemeWebUrl::CloseAll();
	CSchemeMatchSeasonTime::CloseAll();
	CSchemeMatchSeasonPrize::CloseAll();
    CSchemeRecommPrize::CloseAll();

	delete this;
}

/** 取得皮肤配置
@return  
*/
ISchemeSkinInfo* SchemeCenter::getSchemeSkinInfo()
{
	return CSchemeSkinInfo::GetWorkInstance();
}

/** 取得人物模型对象
@return  
*/
ISchemePersonModel*	SchemeCenter::getSchemePersonModel()
{
	return CSchemePersonModel::GetWorkInstance();
}

/** 取得巡逻配置对象
@return  
*/
ISchemePatrol*	SchemeCenter::getSchemePatrol()
{
	return  CSchemePatrol::GetWorkInstance();
}

/** 取得怪物配置对象
@return  
*/
ISchemeMonster*	SchemeCenter::getSchemeMonster()
{
	return  CSchemeMonster::GetWorkInstance();
}


/** 取得地图区域
@return  
*/
ISchemeAllMapsArea* SchemeCenter::getSchemeMapArea()
{
	return  CSchemeAllMapsArea::GetWorkInstance();
}

/** 取得地图大陆
@return  
*/
ISchemeMapLand* SchemeCenter::getSchemeMapLand()
{
	return  CSchemeMapLand::GetWorkInstance();
}

/** 取得场景保姆接口
@return  
*/
ISchemeSceneMatron *  SchemeCenter::getSchemeSceneMatron()
{
    return CSchemeSceneMatron::GetWorkInstance();
}

/** 取得地图信息
@return  
*/
ISchemeMapInfo* SchemeCenter::getSchemeMapInfo()
{
	return  CSchemeMapInfo::GetWorkInstance();
}

/** 取得怪物配置对象
@return  
*/
ISchemeTankInfo*	SchemeCenter::getTankShemeInfo()
{
	return  CSchemeTankInfo::GetWorkInstance();
}

IPathFindServiceInfo * SchemeCenter::getPathFindServiceScheme()
{
	return CSchemePathFindServiceInfo::GetWorkInstance();
}
/** 取得怪物配置对象
@return  
*/
ISchemeLevelUpgrade*	SchemeCenter::getLevelUpgradeSheme()
{
	return  CSchemeLevelUpgrade::GetWorkInstance();
}
/** 取得配置人物基础属性对象
@return  
*/
ISchemePersonBaseProp* SchemeCenter::getShemePersonBaseProp()
{
	return CSchemePersonBaseProp::GetWorkInstance();
}




/** 取得战场管理
@return  
*/
ISchemeWarManager* SchemeCenter::getSchemeWarManager()
{
	return CSchemeWarManager::GetWorkInstance(); 
}

/** 取得怪配置
@return  
*/
ISchemeWarPoint* SchemeCenter::getSchemeWarPoint()
{
	return CSchemeWarPoint::GetWorkInstance(); 
}

/** 取得实体着陆BUFF配置
@return  
*/
ISchemeLandBuff* SchemeCenter::getSchemeLandBuff()
{
	return CSchemeLandBuff::GetWorkInstance(); 
}

ISchemeMatchRoom * SchemeCenter::getSchemeMatchRoom()
{
	return CSchemeMatchRoom::GetWorkInstance();
}


/** 取得实体通用配置
*/
ISchemeEntity *SchemeCenter::getSchemeEntity()
{
	return CSchemeEntity::GetWorkInstance();
} /** 取得天赋配置
@return  
*/
ISchemeTalent* SchemeCenter::getSchemeTalent()
{
    return CTalentScheme::GetWorkInstance();
}

/** 取得战场人物配置
	@return  
*/
ISchemeWarPersonConfigInfo* SchemeCenter::getSchemeWarPersonConfigInfo()
{
	return CSchemeWarPersonConfigInfo::GetWorkInstance();
}

/** 取得体力buff配置
@return  
*/
ISchemeStaminaBuff* SchemeCenter::getSchemeStaminaBuff()
{
	return CSchemeStaminaBuff::GetWorkInstance();
}

/** 取得魔法类型配置
@return  
*/
ISchemeMagicType* SchemeCenter::getSchemeMagicType()
{
	return CSchemeMagicType::GetWorkInstance();
}

/** 取得技能升级配置
@return  
*/
ISchemeSpellUpgrade* SchemeCenter::getSchemeSpellUpgrade()
{
	return CSchemeSpellUpgrade::GetWorkInstance();
}

ISchemeMobaGoods* SchemeCenter::getSchemeMobaGoods()
{
	return CSchemeMobaGoods::GetWorkInstance();
}

/** 取得技能槽配置信息
@return  
*/
ISchemeSpellSlot* SchemeCenter::getSchemeSpellSlot()
{
	return CSchemeSpellSlot::GetWorkInstance();
}

ISchemeSummonerSkill* SchemeCenter::getSchemeSummonerSkill()
{
    return CSchemeSummonerSkill::GetWorkInstance();
}

ISchemeSlotMapedShortcut* SchemeCenter::getSchemeSlotInfo()
{
    return CSchemeSlotMapedShortcut::GetWorkInstance();
}

ISchemeWarEvaluate* SchemeCenter::getSchemeWarEvaluate()
{
    return CSchemeWarEvaluate::GetWorkInstance();
}

ISchemeMatchRankConfigInfo* SchemeCenter::getSchemeMatchRankConfig()
{
    return CSchemeMatchRankConfig::GetWorkInstance();
}

ISchemeWing* SchemeCenter::getSchemeWing()
{
    return CSchemeWing::GetWorkInstance();
}

IDynamicObstacleInfo* SchemeCenter::getSchemeDynamicObstacle()
{
    return CSchemeDynamicObstacle::GetWorkInstance();
}

// 取得飘字提示配置信息
ISchemeSystemMessage* SchemeCenter::getSchemeSystemMessage()
{
    return CSchemeSystemMessage::GetWorkInstance();
}

#ifdef FUNCTION_TALENTPAGE_OPEN
ISchemeTalentPage* SchemeCenter::getSchemeTalentPage()
{
    return CSchemeTalentPage::GetWorkInstance();
}

// 取得飘字提示配置信息
ISchemeTalentPageEffect* SchemeCenter::getSchemeTalentPageEffect()
{
    return CSchemeTalentPageEffect::GetWorkInstance();
}
#endif

#ifdef FUNCTION_RUNE_OPEN
// 取得符文配置信息
ISchemeRuneInfo* SchemeCenter::getSchemeRuneInfo()
{
    return CSchemeRuneInfo::GetWorkInstance();
}

// 取得符文配置信息
ISchemeRuneConfig* SchemeCenter::getSchemeRuneConfig()
{
    return CSchemeRuneConfig::GetWorkInstance();
}

// 取得符文页配置信息
ISchemeRunePage* SchemeCenter::getSchemeRunePage()
{
    return CSchemeRunePage::GetWorkInstance();
}
#endif  // #ifdef FUNCTION_RUNE_OPEN

// 取得英雄天赋配置信息
ISchemeHeroTalentConfig* SchemeCenter::getSchemeHeroTalentConfig()
{
    return CSchemeHeroTalentConfig::GetWorkInstance();
}


ISchemePlayerGloryInfo* SchemeCenter::getSchemeGloryInfo()
{
    return CSchemePlayerGloryInfo::GetWorkInstance();
}

ISchemeHeroStarData* SchemeCenter::getSchemeHeroStarData()
{
    return CSchemeHeroStarData::GetWorkInstance();
}

ISchemeInitialHero* SchemeCenter::getSchemeInitialHeroInfo()
{
    return CSchemeInitialHero::GetWorkInstance();
}

ISchemeKinDataCenter* SchemeCenter::getSchemeKinDataCenter()
{
    return CSchemeKinDataCenter::GetWorkInstance();
}

// 取得帮会配置相关
ISchemeClanDataCenter* SchemeCenter::getSchemeClanDataCenter()
{
    return CSchemeClanDataCenter::GetWorkInstance();
}

// 取得AI配置列表
ISchemeAIIDTable* SchemeCenter::getAISchemeIDTable()
{
    return CSchemeAIIDTable::GetWorkInstance();
}


// 英雄卡牌配置接口
ISchemeHeroCard* SchemeCenter::getSchemeHeroCard()
{
    return CSchemeHeroCard::GetWorkInstance();
}

// 获得英雄卡牌创建配置列表
ISchemeHeroCardCreat* SchemeCenter::getSchemeHeroCardCreat()
{
    return CSchemeHeroCardCreat::GetWorkInstance();
}

// 获取战场系数信息
ISchemeMatchCoefficient* SchemeCenter::getSchemeMatchCoeff()
{
    return CSchemeMatchCoefficient::GetWorkInstance();
}

// 获取金钱或者经验产出配置信息
ISchemeExpOrGoldCreate* SchemeCenter::getSchemeExpOrGoldCreate()
{
    return CSchemeExpOrGoldCreate::GetWorkInstance();
}

// 获得战力值配置信息
ISchemeFightingCapacityConfig* SchemeCenter::getSchemeFightingCapacityConfig()
{
    return CSchemeFightingCapacityConfig::GetWorkInstance();
}

// 获得英雄成就配置信息
ISchemeHeroAchievementConfig* SchemeCenter::getSchemeHeroAchievementConfig()
{
    return CSchemeHeroAchievement::GetWorkInstance();
}

// 获得产出限制配置信息
ISchemeOutputRestriction* SchemeCenter::getSchemeOutputRestriction()
{
    return CSchemeOutputRestriction::GetWorkInstance();
}

// 取得速度衰减配置信息
ISchemeSpeedDecay* SchemeCenter::getSchemeSpeedDecay()
{
    return CSchemeSpeedDecay::GetWorkInstance();
}

// 获得奖励配置信息
ISchemeActorPrize* SchemeCenter::getSchemeActorPrize()
{
	return CSchemeActorPrize::GetWorkInstance();
}

// 获得宝箱配置信息
ISchemePrizeChest* SchemeCenter::getSchemePrizeChest()
{
	return CSchemePrizeChest::GetWorkInstance();
}

// 取得PK配置信息
ISchemePlayerKill* SchemeCenter::getSchemePlayerKill()
{
    return CSchemePlayerKill::GetWorkInstance();
}

// 获得商城配置信息
ISchemePointShop* SchemeCenter::getSchemePointShop()
{
	return CSchemePointShop::GetWorkInstance();
}

// 取得解锁角色英雄奖励配置
ISchemePrizeUnlock* SchemeCenter::getSchemePrizeUnlock()
{
    return CSchemePrizeUnlock::GetWorkInstance();
}

// 取得周免英雄配置
ISchemeHeroFreeWeekly* SchemeCenter::getSchemeHeroFreeWeekly()
{
	return CSchemeHeroFreeWeekly::GetWorkInstance();
}

// 取得AI购买物品配置
ISchemeAIBuyItem* SchemeCenter::getSchemeAIBuyItem()
{
    return CAIBuyItemScheme::GetWorkInstance();
}

// 获得宝箱配置信息
ISchemeBenefitCard* SchemeCenter::getSchemeBenefitCard()
{
	return CSchemeBenefitCard::GetWorkInstance();
}

// 获得皇冠主配置
ISchemeCrownGemstoneInfo* SchemeCenter::getSchemeCrownGemstoneInfo()
{
	return CSchemeCrownGemstoneInfo::GetWorkInstance();
}

ISchemeLegendCup* SchemeCenter::getSchemeLegendCup()
{
    return CSchemeLegendCup::GetWorkInstance();
}

// 取得师徒配置相关
ISchemeMentorshipDataCenter* SchemeCenter::getSchemeMentorshipDataCenter()
{
	return CSchemeMentorshipDataCenter::GetWorkInstance();
}

// 获得点赞配置
ISchemeWarEndSupportPlayer* SchemeCenter::getSchemeWarEndSupportPlayer()
{
	return CSchemeWarEndSupportPlayer::GetWorkInstance();
}

// 获得XP配置
ISchemeHeroXpSkill* SchemeCenter::getSchemeHeroXpSkill()
{
	return CSchemeHeroXpSkill::GetWorkInstance();
}

// 聊听冷却时间相关
ISchemeChatCD* SchemeCenter::getSchemeChatCD()
{
	return CSchemeChatCD::GetWorkInstance();
}

ISchemeTask* SchemeCenter::getSchemeTask()
{
    return CSchemeTask::GetWorkInstance();
}

// 取得战场经济控制配置
ISchemeEconomyControl* SchemeCenter::getSchemeEconomyControl()
{
	return CSchemeEconomyControl::GetWorkInstance();
}

// 取得英雄推荐线路配置
ISchemeHeroRecommendPath* SchemeCenter::getSchemeHeroRecommendPath()
{
    return CHeroRecommendPathScheme::GetWorkInstance();
}

ISchemeWarForceEnd* SchemeCenter::getSchemeWarForceEnd()
{
	return CSchemeWarForceEnd::GetWorkInstance();
}

// 取得新手引导步骤配置配置
ISchemeGuideStep * SchemeCenter::getSchemeGuideStep()
{
	return CSchemeGuideStep::GetWorkInstance();
}

// 取得战场子功能配置
ISchemeWarSubFunc* SchemeCenter::getSchemeWarSubFunc()
{
	return CSchemeWarSubFunc::GetWorkInstance();
}

// 取得战场常用装备配置
ISchemeWarOftenUseEquip* SchemeCenter::getSchemeWarOftenUseEquip()
{
	return CSchemeWarOftenUseEquip::GetWorkInstance();
}

ISchemeWarGodRecommdEquip* SchemeCenter::getSchemeWarGodRecommdEquip()
{
	return CSchemeWarGodRecommdEquip::GetWorkInstance();
}


// 取得撮合限制
ISchemeMatchRankLimitInfo* SchemeCenter::getSchemeMatchRankLimit()
{
    return CSchemeMatchRankLimit::GetWorkInstance();
}

// 取得英雄复活时间
ISchemeHeroReliveTime* SchemeCenter::getSchemeHeroReliveTime()
{
	return CSchemeHeroReliveTime::GetWorkInstance();
}

ISchemeActivityCenter* SchemeCenter::getSchemeActivityCenter()
{
	return CSchemeActivityCenter::GetWorkInstance();
}

ISchemeWarAddBuff* SchemeCenter::getSchemeWarAddBuff()
{
	return CSchemeWarAddBuff::GetWorkInstance();
}

// 获取AI动态经济调整配置
ISchemeAIEcmAdjust* SchemeCenter::getAIEcmAdjust()
{
	return CSchemeAIEcmAdjust::GetWorkInstance();
}

// 获取本命英雄相关配置
ISchemeLifeHero* SchemeCenter::getSchemeLifeHero()
{
	return CSchemeLifeHero::GetWorkInstance();
}

ISchemeAIEcmExpCtrl* SchemeCenter::getSchemeAIEcmExpCtrl()
{
	return CSchemeAIEcmExpCtrl::GetWorkInstance();
}

ISchemeWarSafetyZone * SchemeCenter::getSchemeWarSafetyZone()
{
	return CSchemeWarSafetyZone::GetWorkInstance();
}

ISchemeWarRandRefreshMons * SchemeCenter::getSchemeWarRandRefreshMons()
{
	return CSchemeWarRandRefreshMons::GetWorkInstance();
}

ISchemeWarDevilKing * SchemeCenter::getSchemeWarDevilKing()
{
	return CSchemeWarDevilKing::GetWorkInstance();
}

ISchemeWarDevilKingNPC * SchemeCenter::getSchemeWarDevilKingNPC()
{
	return CSchemeWarDevilKingNPC::GetWorkInstance();
}

ISchemeWebUrl * SchemeCenter::getSchemeWebUrl()
{
    return CSchemeWebUrl::GetWorkInstance();
}

// 获取排位赛季时间配置
ISchemeMatchSeasonTime* SchemeCenter::getSchemeMatchSeasonTime()
{
	return CSchemeMatchSeasonTime::GetWorkInstance();
}

// 获取排位赛季奖励配置
ISchemeMatchSeasonPrize* SchemeCenter::getSchemeMatchSeasonPrize()
{
	return CSchemeMatchSeasonPrize::GetWorkInstance();
}

ISchemeRecommPrize* SchemeCenter::getSchemeRecommPrize()
{
    return CSchemeRecommPrize::GetWorkInstance();
}