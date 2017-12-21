/*******************************************************************
** 文件名:	EffectFactory.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	penglu
** 日  期:	2014/12/23
** 版  本:	1.0
** 描  述:	效果管理器实现
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "StdAfx.h"
#include "IServerGlobal.h"
#include "EffectFactory.h"
#include "EffectDef.h"
#include "EffectRoot.h"
#include "EffectCoolDown.h"
#include "EffectAddBuff.h"
#include "EffectSetSpellFlag.h"
#include "EffectTransLocation.h"
#include "EffectSpellCombo.h"
#include "EffectFreeze.h"
#include "EffectTimerSpellOverlay.h"
#include "EffectChangeProperty.h"
#include "EffectSilence.h"
#include "EffectImmune.h"
#include "EffectResistDamage.h"
#include "EffectCreateMonster.h"
#include "EffectUseSpell.h"
#include "EffectBeatBack.h"
#include "EffectKillSelf.h"
#include "EffectImmuneBuff.h"
#include "EffectAddRangeBuff.h"
#include "EffectAddSummonMonster.h"
#include "EffectLessSummonMonster.h"
#include "EffectBreakSpell.h"
#include "EffectImmuneBreakSpell.h"
#include "EffectInvisible.h"
#include "EffectActiveTalent.h"
#include "EffectContinuedDamage.h"
#include "EffectDisableSpell.h"
#include "EffectDisableSlotIndex.h"
#include "EffectFeedback.h"
#include "EffectChangeReliveTime.h"
#include "EffectRebound.h"
#include "EffectContinuedDamageEffect.h"
#include "EffectContinuedDamagePropertyEffect.h"
#include "EffectContinuedCureEffect.h"
#include "EffectAddCureEffect.h"
#include "EffectApendDamage.h"
#include "EffectReduceDamage.h"
#include "EffectAddBuffOnDamage.h"
#include "EffectWarAddBuff.h"
#include "EffectApparentDeath.h"
#include "EffectNoSelect.h"
#include "EffectAccuracy.h"
#include "EffectEvade.h"
#include "EffectAttackAddBuff.h"
#include "EffectCastSpell.h"
#include "EffectContinuedAddTalentPoint.h"
#include "EffectEntityDieNearAddTalentPoint.h"
#include "EffectEntityKillAddTalentPoint.h"
#include "EffectRemoveBuffType.h"
#include "EffectCleanEnmity.h"
#include "EffectKillAddBuff.h"
#include "EffectKillMonsterAddBuff.h"
#include "EffectPropertyTransform.h"
#include "EffectPropertyTransformLoop.h"
#include "EffectTargetPropertyTransform.h"
#include "EffectContinuedSetProperty.h"
#include "EffectSetProperty.h"
#include "EffectChangeStateAddBuff.h"
#include "EffectSetShopBuy.h"
#include "EffectDragToMove.h"
#include "EffectChangePropertyAddBuff.h"
#include "EffectImmuneDrains.h"
#include "EffectDrinkingBlood.h"
#include "EffectChangeBuff.h"
#include "EffectChargedStrike.h"
#include "EffectBuffEffectChange.h"
#include "EffectAssistAddBuff.h"
#include "EffectDieIncreaseMoneyExp.h"
#include "EffectIncreaseMonneyExpCoefficient .h"
#include "EffectChargedTransformProperty.h"
#include "EffectDragToMoveEX.h"
#include "EffectAddEnmityValue.h"
#include "EffectSetStealth.h"
#include "EffectAntiStealth.h"
#include "EffectSetSight.h"
#include "EffectGoodsRestartCharged.h"
#include "EffectCDOverlay.h"
#include "EffectBeatBackAddBuff.h"
#include "EffectAccumulateDamage.h"
#include "EffectJump.h"
#include "EffectCheckTargetRange.h"
#include "EffectImmuneEffect.h"
#include "EffectTimingAddBuff.h"
#include "EffectShowFloatText.h"
#include "EffectFindTarget.h"
#include "EffectSetSingleSpellData.h"
#include "EffectSetSingleStrSpellData.h"
#include "EffectUpgrade.h"
#include "EffectSetWing.h"
#include "EffectSetWingState.h"
#include "EffectRandAddBuff.h"
#include "EffectSetWingFlyTime.h"
#include "EffectCaptureBuff.h"
#include "EffectCheckPropertyAddBuff.h"
#include "EffectCheckMpOrDistanceAddBuff.h"
#include "EffectContinuePositionAddBuff.h"
#include "EffectDistanceDamage.h"
#include "EffectAccumulateDamageAddBuff.h"
#include "EffectMoveDamage.h"
#include "EffectKillSummonMonster.h"
#include "EffectEnterForestAddBuff.h"
#include "EffectMatchingSpellAddBuff.h"
#include "EffectCheckBuffAddBuff.h"
#include "EffectAttackEntityAddBuff.h"
#include "EffectAttackEffectAddBuff.h"
#include "EffectCheckDistanceAddBuff.h"
#include "EffectInsideOuterCircleAddBuff.h"
#include "EffectDistanceAddBuff.h"
#include "EffectReplaceSlotSpell.h"
#include "EffectReplaceSlotSpellGroup.h"
#include "EffectCheckRangeBuffAddBuff.h"
#include "EffectUpgradeChangeProperty.h"
#include "EffectChooseMonsterTransport.h"
#include "EffectDieAddBuff.h"
#include "EffectDieEventAddBuff.h"
#include "EffectChangePropertyChangeValue.h"
#include "EffectPromoteDamage.h"
#include "EffectPromoteShield.h"
#include "EffectKeepInsight.h"
#include "EffectGlobalVisible.h"
#include "EffectChangeMaskFlag.h"
#include "EffectBLDSummon.h"
#include "EffectBLDDamage.h"
#include "EffectCheckTargetFilterAddBuff.h"
#include "EffectSpecificDamageAddBuff.h"
#include "EffectShareDamage.h"
#include "EffectAddPropertyLimit.h"
#include "EffectContinuedAddProperty.h"
#include "EffectSetTargetLoc.h"
#include "EffectRecordPointBack.h"
#include "EffectSetSlotLevel.h"
#include "EffectAddGoods.h"
#include "EffectCastSpellAddBuff.h"
#include "EffectCastSpellCheckBuff.h"
#include "EffectReplicationMonster.h"
#include "EffectStaticAddBuff.h"
#include "EffectCheckBuffDamageBonus.h"
#include "EffectAttackCountAddBuff.h"
#include "EffectBeLongsKillAddBuff.h"
#include "EffectAttackAddCRD.h"
#include "EffectDisableSpellUseCount.h"
#include "EffectDelayUseSpell.h"
#include "EffectCharge.h"
#include "EffectRepulse.h"
#include "EffectStrike.h"
#include "EffectRoll.h"
#include "EffectPerspectiveShow.h"
#include "EffectGoodBuyLimit.h"
#include "EffectKillingControl.h"
#include "EffectThrillingCure.h"
#include "EffectGroupControl.h"
#include "EffectDieSummonMonster.h"
#include "EffectDamageCountAttackAddBuff.h"
#include "EffectRangeDamageTransmit.h"
#include "EffectChooseRangeAddBuff.h"
#include "EffectWarMonsterAddBuff.h"
#include "EffectSetWingProperty.h"
#include "EffectMapFly.h"
#include "EffectDisableActiveTalent.h"
#include "EffectContinuousAttackBeHitAddBuff.h"
#include "EffectBeSneered.h"
#include "EffectNotifyClientData.h"
#include "EffectAddBuffOnRightButtonDown.h"
#include "EffectStopSpell.h"
#include "EffectTraceAI.h"
#include "EffectPrepareBullet.h"
#include "EffectDecBulletCount.h"
#include "EffectUseSpellIfNoBullet.h"
#include "EffectSetCanPassBarrier.h"
#include "EffectAddBuffRemoveOnTargetDie.h"
#include "EffectCreateMoveBarrier.h"
#include "EffectCheckCDAddEffect.h"
#include "EffectContinuedChangeProperty.h"
#include "EffectWarChangeVoc.h"
#include "EffectFakeDeadRelive.h"
#include "EffectStealthAntiedAddBuff.h"

using namespace EFFECT_SERVER;

CEffectFactory	*g_pEffectFactory = NULL;

CEffectFactory::CEffectFactory()
{
    m_effectMap.clear();
    m_tempEffectMap.clear();
}


CEffectFactory::~CEffectFactory()
{

}


// 加载效果工厂
bool CEffectFactory::Load( const char * szFileName )
{
    string stringPath = SCP_PATH_FORMAT( szFileName );
	if ( !gServerGlobal->getSchemeEngine()->LoadScheme(stringPath.c_str(), this, true ) )
	{
		ErrorLn("The effect config file load failed!"<<stringPath.c_str() );
		return false;
	}

	return true;
}

/**
@name				: 创建效果对象
@param				: 效果ID
@param				: 效果依附的实体
@param				: 创建即开启
@return
*/
IEffect* CEffectFactory::BuildEffect(int nEffectID)
{
	EFFECT_MAP::iterator it = m_effectMap.find(nEffectID);
	if ( it==m_effectMap.end() )
		return 0;

	return it->second->Clone();
}


// 载入脚本 
bool CEffectFactory::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    IBinReader * pBinReader = reader.pBINReader;
    if ( pBinReader==0)
        return false;

    if(reader.type==READER_CSV)
    {
        return LoadEffectCsv(reader, szFileName);
    }
    else
    {
        return LoadEffectBin(reader, szFileName);
    }

	return true;
}


bool CEffectFactory::LoadEffectCsv(SCRIPT_READER reader, const char* szFileName)
{
    if (szFileName == NULL)
        return false;

    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    // 清空表
    m_tempEffectMap.clear();

    bool bRepeat = false;
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow=0; nRow<nRecordCount; ++nRow)
    {
        // 取得效果ID
        int nID = pCSVReader->GetInt(nRow, 1, 0);

        pair<TEMP_EFFECT_MAP::iterator, bool> result = m_tempEffectMap.insert(TEMP_EFFECT_MAP::value_type(nID, nID));
        if (!result.second)
        {
            bRepeat = true;
            ErrorLn("There is repeated effect,ID="<<nID<<",szFileName="<<szFileName);
        }	
    }

    // 清空表(因为只是做检验用的)
    m_tempEffectMap.clear();

    // 有重复的
    if (bRepeat)
    {
        return false;
    }

    return true;
}



bool CEffectFactory::LoadEffectBin(SCRIPT_READER reader, const char* szFileName)
{
    if (szFileName == NULL)
        return false;

    IBinReader * pBinReader = reader.pBINReader;
    if ( pBinReader==0 || reader.type!=READER_BIN )
        return false;

    EFFECT_MAP::iterator it = m_effectMap.begin();
    for ( ;it!=m_effectMap.end();++it )
    {
        it->second->Release();
    }
    m_effectMap.clear();

    BIN_FILE_HEADER * pEffectHeader = 0;
    void * pEffectData = 0;
    int nDataLen = 0;

    while( pEffectHeader = pBinReader->getNextSegment( pEffectHeader,pEffectData,nDataLen ) )
    {
        std::string typeName = string(pEffectHeader->szTypeName);

        // 客户端效果不读入
        if ( typeName.find("EffectClient")!=std::string::npos )
        {
            continue;
        }

		obuf buf;
		// 载入效果
#		define     LOAD_EFFECT(typeData,typeImp)                                  \
        if ( typeName==#typeData && pEffectHeader->nTypeSize==sizeof(typeData) )  \
        {                                                                         \
            for ( int i=0;i<pEffectHeader->nItemTotal;++i )                       \
            {                                                                     \
                typeData * pEffect = (typeData*)pEffectData + i;                  \
                long nID = pEffect->nID;                                           \
				typeImp* pImp = new typeImp(*pEffect);								\
				if (!pImp->Check())	return false;									\
                pair<EFFECT_MAP::iterator, bool> result = m_effectMap.insert(EFFECT_MAP::value_type(nID, pImp));          \
                if (!result.second)												  \
                {																  \
                    buf << nID << szFileName;                                     \
                    ShowErrorMsg(buf.data());		                              \
                    return false;												  \
                }																  \
            }                                                                     \
            continue;                                                             \
        }


        // 依次载入
        LOAD_EFFECT( EffectServer_Root,CEffectRoot );
        LOAD_EFFECT( EffectServer_CoolDown,CEffectCoolDown );
        LOAD_EFFECT( EffectServer_AddBuff,CEffectAddBuff );
        LOAD_EFFECT( EffectServer_SetSpellFlag,CEffectSetSpellFlag );
        LOAD_EFFECT( EffectServer_TransLocation,CEffectTransLocation );
        LOAD_EFFECT( EffectServer_SpellCombo,CEffectSpellCombo );
        LOAD_EFFECT( EffectServer_Freeze,CEffectFreeze );
        LOAD_EFFECT( EffectServer_TimerSpellOverlay,CEffectTimerSpellOverlay );
        LOAD_EFFECT( EffectServer_ChangeProperty,CEffectChangeProperty );
		LOAD_EFFECT( EffectServer_ContinuedChangeProperty, CEffectContinuedChangeProperty);
        LOAD_EFFECT( EffectServer_Silence, CEffectSilence );
        LOAD_EFFECT( EffectServer_Immune, CEffectImmune );
        LOAD_EFFECT( EffectServer_ResistDamage, CEffectResistDamage );
        LOAD_EFFECT( EffectServer_CreateMonster, CEffectCreateMonster );
        LOAD_EFFECT( EffectServer_UseSpell, CEffectUseSpell );
        LOAD_EFFECT( EffectServer_BeatBack, CEffectBeatBack );
        LOAD_EFFECT( EffectServer_KillSelf, CEffectKillSelf );
        LOAD_EFFECT( EffectServer_ImmuneBuff, CEffectImmuneBuff );
        LOAD_EFFECT( EffectServer_AddRangeBuff, CEffectAddRangeBuff );
        LOAD_EFFECT( EffectServer_AddSummonMonster, CEffectAddSummonMonster );
        LOAD_EFFECT( EffectServer_LessSummonMonster, CEffectLessSummonMonster );
        LOAD_EFFECT( EffectServer_BreakSpell, CEffectBreakSpell );
        LOAD_EFFECT( EffectServer_ImmuneBreakSpell, CEffectImmuneBreakSpell );
        LOAD_EFFECT( EffectServer_Invisible, CEffectInvisible );
        LOAD_EFFECT( EffectServer_ActiveTalent, CEffectActiveTalent );
        LOAD_EFFECT( EffectServer_ContinuedDamage, CEffectContinuedDamage );
        LOAD_EFFECT( EffectServer_DisableSpell, CEffectDisableSpell );
        LOAD_EFFECT( EffectServer_DisableSlotIndex, CEffectDisableSlotIndex );
        LOAD_EFFECT( EffectServer_Feedback, CEffectFeedback );
        LOAD_EFFECT( EffectServer_ReliveTime, CEffectChangeReliveTime );
        LOAD_EFFECT( EffectServer_Rebound, CEffectRebound );
        LOAD_EFFECT( EffectServer_ContinuedDamageEffect, CEffectContinuedDamageEffect );
        LOAD_EFFECT( EffectServer_ContinuedDamagePropertyEffect, CEffectContinuedDamagePropertyEffect );
        LOAD_EFFECT( EffectServer_ContinuedCureEffect, CEffectContinuedCureEffect );
        LOAD_EFFECT( EffectServer_AddCureEffect, CEffectAddCureEffect );
        LOAD_EFFECT( EffectServer_ApendDamage, CEffectApendDamage );
        LOAD_EFFECT( EffectServer_ReduceDamage, CEffectReduceDamage );
        LOAD_EFFECT( EffectServer_AddBuffOnDamage, CEffectAddBuffOnDamage );
        LOAD_EFFECT( EffectServer_WarAddBuff, CEffectWarAddBuff );
        LOAD_EFFECT( EffectServer_ApparentDeath, CEffectApparentDeath );
        LOAD_EFFECT( EffectServer_NoSelect, CEffectNoSelect );
        LOAD_EFFECT( EffectServer_Accuracy, CEffectAccuracy );
        LOAD_EFFECT( EffectServer_Evade, CEffectEvade );
        LOAD_EFFECT( EffectServer_AttackAddBuff, CEffectAttackAddBuff );
        LOAD_EFFECT( EffectServer_CastSpell, CEffectCastSpell );
        LOAD_EFFECT( EffectServer_ContinuedAddTalentPoint, CEffectContinuedAddTalentPoint );
        LOAD_EFFECT( EffectServer_EntityDieNearAddTalentPoint, CEffectEntityDieNearAddTalentPoint );
        LOAD_EFFECT( EffectServer_EntityKillAddTalentPoint, CEffectEntityKillAddTalentPoint );
        LOAD_EFFECT( EffectServer_RemoveBuffType, CEffectRemoveBuffType );
        LOAD_EFFECT( EffectServer_CleanEnmity, CEffectCleanEnmity );
        LOAD_EFFECT( EffectServer_KillAddBuff, CEffectKillAddBuff );
        LOAD_EFFECT( EffectServer_KillMonsterAddBuff, CEffectKillMonsterAddBuff );
        LOAD_EFFECT( EffectServer_PropertyTransform, CEffectPropertyTransform );
        LOAD_EFFECT( EffectServer_PropertyTransformLoop, CEffectPropertyTransformLoop );
        LOAD_EFFECT( EffectServer_TargetPropertyTransform, CEffectTargetPropertyTransform);
        LOAD_EFFECT( EffectServer_ContinuedSetProperty, CEffectContinuedSetProperty );
        LOAD_EFFECT( EffectServer_SetProperty, CEffectSetProperty );
        LOAD_EFFECT( EffectServer_ChangeStateAddBuff, CEffectChangeStateAddBuff );
        LOAD_EFFECT( EffectServer_SetShopBuy, CEffectSetShopBuy );
        LOAD_EFFECT( EffectServer_DragToMove, CEffectDragToMove );
        LOAD_EFFECT( EffectServer_ChangePropertyAddBuff, CEffectChangePropertyAddBuff );
        LOAD_EFFECT( EffectServer_ImmuneEffect, CEffectImmuneEffect);
        LOAD_EFFECT( EffectServer_DrinkingBlood, CEffectDrinkingBlood );
        LOAD_EFFECT( EffectServer_ChangeBuff, CEffectChangeBuff );
        LOAD_EFFECT( EffectServer_ChargedStrike, CEffectChargedStrike );
        LOAD_EFFECT( EffectServer_BuffEffectChange, CEffectBuffEffectChange );
        LOAD_EFFECT( EffectServer_AssistAddBuff, CEffectAssistAddBuff );
        LOAD_EFFECT( EffectServer_DieIncreaseMonneyExp , CEffectDieIncreaseMoneyExp );
        LOAD_EFFECT( EffectServer_DieIncreaseMonneyExp, CEffectIncreaseMonneyExpCoefficient );
        LOAD_EFFECT( EffectServer_ChargedToProPerty, CEffectChargedTransformProperty );
        LOAD_EFFECT( EffectServer_DragToMoveEX, CEffectDragToMoveEX );
        LOAD_EFFECT( EffectServer_AddEnmity, CEffectAddEnmityValue );
        LOAD_EFFECT( EffectServer_SetStealth, CEffectSetStealth );
        LOAD_EFFECT( EffectServer_AntiStealth, CEffectAntiStealth );
        LOAD_EFFECT( EffectServer_SetSight, CEffectSetSight );
        LOAD_EFFECT( EffectServer_GoodsRestartCharged, CEffectGoodsRestartCharged);
        LOAD_EFFECT( EffectServer_MultipleCoolDown,CEffectCDOverlay);
        LOAD_EFFECT( EffectServer_BeatBackAddBuff, CEffectBeatBackAddBuff);
        LOAD_EFFECT( EffectServer_AccumulateDamage, CEffectAccumulateDamage);
        LOAD_EFFECT( EffectServer_Jump, CEffectJump);
        LOAD_EFFECT( EffectServer_CheckTargetRange, CEffectCheckTargetRange);
        LOAD_EFFECT( EffectServer_ImmuneVoteEffect, CEffectImmuneVoteEffect);
        LOAD_EFFECT( EffectServer_TimingAddBuff, CEffectTimingAddBuff);
        LOAD_EFFECT( EffectServer_ShowFloatText, CEffectShowFloatText);
        LOAD_EFFECT( EffectServer_FindTarget, CEffectFindTarget);
        LOAD_EFFECT( EffectServer_SetSingleSpellData, CEffectSetSingleSpellData);
        LOAD_EFFECT( EffectServer_SetSingleStrSpellData, CEffectSetSingleStrSpellData);
        LOAD_EFFECT( EffectServer_Upgrade, CEffectUpgrade);
        LOAD_EFFECT( EffectServer_SetWing, CEffectSetWing);
        LOAD_EFFECT( EffectServer_SetWingState, CEffectSetWingState);
        LOAD_EFFECT( EffectServer_RandAddBuff, CEffectRandAddBuff);
        LOAD_EFFECT( EffectServer_SetWingFlyTime, CEffectSetWingFlyTime);
        LOAD_EFFECT( EffectServer_CaptureBuff, CEffectCaptureBuff);
        LOAD_EFFECT( EffectServer_CheckPropertyAddBuff, CEffectCheckPropertyAddBuff);
        LOAD_EFFECT( EffectServer_CheckMpOrDistanceAddBuff, CEffectCheckMpOrDistanceAddBuff);
        LOAD_EFFECT( EffectServer_ContinuePositionAddBuff, CEffectContinuePositionAddBuff);
        LOAD_EFFECT( EffectServer_DistanceDamage, CEffectDistanceDamage);
        LOAD_EFFECT( EffectServer_AccumulateDamageAddBuff, CEffectAccumulateDamageAddBuff);
        LOAD_EFFECT( EffectServer_MoveDamage, CEffectMoveDamage);
        LOAD_EFFECT( EffectServer_KillSummonMonster, CEffectKillSummonMonster);
        LOAD_EFFECT( EffectServer_EnterForestAddBuff, CEffectEnterForestAddBuff);
        LOAD_EFFECT( EffectServer_MatchingSpellAddBuff, CEffectMatchingSpellAddBuff);
        LOAD_EFFECT( EffectServer_CheckBuffAddBuff, CEffectCheckBuffAddBuff);
        LOAD_EFFECT( EffectServer_AttackEntityAddBuff, CEffectAttackEntityAddBuff);
        LOAD_EFFECT( EffectServer_AttackEffectAddBuff, CEffectAttackEffectAddBuff);
        LOAD_EFFECT( EffectServer_CheckDistanceAddBuff, CEffectCheckDistanceAddBuff);
        LOAD_EFFECT( EffectServer_InsideOuterCircleAddBuff, CEffectInsideOuterCircleAddBuff);
        LOAD_EFFECT( EffectServer_DistanceAddBuff, CEffectDistanceAddBuff);
        LOAD_EFFECT( EffectServer_ReplaceSlotSpell, CEffectReplaceSlotSpell);
        LOAD_EFFECT( EffectServer_ReplaceSlotSpellGroup, CEffectReplaceSlotSpellGroup);
        LOAD_EFFECT( EffectServer_CheckRangeBuffAddBuff, CEffectCheckRangeBuffAddBuff);
        LOAD_EFFECT( EffectServer_UpgradeChangeProperty, CEffectUpgradeChangeProperty);
        LOAD_EFFECT( EffectServer_ChooseMonsterTransport, CEffectChooseMonsterTransport);
    	LOAD_EFFECT( EffectServer_DieAddBuff, CEffectDieAddBuff);
        LOAD_EFFECT( EffectServer_DieEventAddBuff, CEffectDieEventAddBuff);
        LOAD_EFFECT( EffectServer_ChangePropertyChangeValue, CEffectChangePropertyChangeValue);
        LOAD_EFFECT( EffectServer_PromoteDamage, CEffectPromoteDamage);
        LOAD_EFFECT( EffectServer_PromoteShield, CEffectPromoteShield);
        LOAD_EFFECT( EffectServer_KeepInsight, CEffectKeepInsight);
        LOAD_EFFECT( EffectServer_GlobalVisible, CEffectGlobalVisible);
        LOAD_EFFECT( EffectServer_ChangeMaskFlag, CEffectChangeMaskFlag);
        LOAD_EFFECT( EffectServer_BLDSummon, CEffectBLDSummon);
        LOAD_EFFECT( EffectServer_BLDDamage, CEffectBLDDamage);
        LOAD_EFFECT( EffectServer_CheckTargetFilterAddBuff, CEffectCheckTargetFilterAddBuff);
        LOAD_EFFECT( EffectServer_SpecificDamageAddBuff, CEffectSpecificDamageAddBuff);
        LOAD_EFFECT( EffectServer_ShareDamage, CEffectShareDamage);
        LOAD_EFFECT( EffectServer_AddPropertyLimit, CEffectAddPropertyLimit);
        LOAD_EFFECT( EffectServer_ContinuedAddProperty, CEffectContinuedAddProperty);
        LOAD_EFFECT( EffectServer_SetTargetLoc, CEffectSetTargetLoc);
        LOAD_EFFECT( EffectServer_RecordPointBack, CEffectRecordPointBack);
        LOAD_EFFECT( EffectServer_SetSlotLevel, CEffectSetSlotLevel);
        LOAD_EFFECT( EffectServer_AddGoods, CEffectAddGoods);
        LOAD_EFFECT( EffectServer_CastSpellAddBuff, CEffectCastSpellAddBuff);
        LOAD_EFFECT( EffectServer_CastSpellCheckBuff, CEffectCastSpellCheckBuff);
        LOAD_EFFECT( EffectServer_ReplicationMonster, CEffectReplicationMonster);
        LOAD_EFFECT( EffectServer_StaticAddBuff, CEffectStaticAddBuff);
        LOAD_EFFECT( EffectServer_CheckBuffDamageBonus, CEffectCheckBuffDamageBonus);
        LOAD_EFFECT( EffectServer_AttackCountAddBuff, CEffectAttackCountAddBuff);
        LOAD_EFFECT( EffectServer_BeLongsKillAddBuff, CEffectBeLongsKillAddBuff);
        LOAD_EFFECT( EffectServer_AttackAddCRD, CEffectAttackAddCRD);
        LOAD_EFFECT( EffectServer_DisableSpellUseCount, CEffectDisableSpellUseCount);
        LOAD_EFFECT( EffectServer_DelayUseSpell, CEffectDelayUseSpell);
        LOAD_EFFECT( EffectServer_Charge, CEffectCharge);
        LOAD_EFFECT( EffectServer_Repulse, CEffectRepulse);
        LOAD_EFFECT( EffectServer_Strike, CEffectStrike);
        LOAD_EFFECT( EffectServer_Roll, CEffectRoll);
        LOAD_EFFECT( EffectServer_PerspectiveShow, CEffectPerspectiveShow);
        LOAD_EFFECT( EffectServer_GoodBuyLimit, CEffectGoodBuyLimit);
        LOAD_EFFECT( EffectServer_KillingControl, CEffectKillingControl);
        LOAD_EFFECT( EffectServer_ThrillingCure, CEffectThrilingCure);
        LOAD_EFFECT( EffectServer_GroupControl, CEffectGroupControl);
        LOAD_EFFECT( EffectServer_DieSummonMonster, CEffectDieSummonMonster);
        LOAD_EFFECT( EffectServer_DamageCountAttackAddBuff, CEffectDamageCountAttackAddBuff);
        LOAD_EFFECT( EffectServer_RangeDamageTransmit, CEffectRangeDamageTransmit);
        LOAD_EFFECT( EffectServer_ChooseRangeAddBuff, CEffectChooseRangeAddBuff);
        LOAD_EFFECT( EffectServer_WarMonsterAddBuff, CEffectWarMonsterAddBuff);
        LOAD_EFFECT( EffectServer_SetWingProperty, CEffectSetWingProperty);
        LOAD_EFFECT( EffectServer_MapFly, CEffectMapFly);
        LOAD_EFFECT( EffectServer_DisableActiveTalent, CEffectDisableActiveTalent);
        LOAD_EFFECT( EffectServer_ContinuousAttackBeHitAddBuff, CEffectContinuousAttackBeHitAddBuff);
        LOAD_EFFECT( EffectServer_BeSneered, CEffectBeSneered);
        LOAD_EFFECT( EffectServer_NotifyClientData, CEffectNotifyClientData);
        LOAD_EFFECT( EffectServer_AddBuffOnRightButtonDown, CEffectAddBuffOnRightButtonDown);
        LOAD_EFFECT( EffectServer_StopSpell, CEffectStopSpell);
        LOAD_EFFECT( EffectServer_TraceAI, CEffectTraceAI);
		LOAD_EFFECT( EffectServer_PrepareBullet, CEffectPrepareBullet);
		LOAD_EFFECT( EffectServer_DecBulletCount, CEffectDecBulletCount);
		LOAD_EFFECT( EffectServer_UseSpellIfNoBullet, CEffectUseSpellIfNoBullet);
        LOAD_EFFECT( EffectServer_SetCanPassBarrier, CEffectSetCanPassBarrier);
        LOAD_EFFECT( EffectServer_AddBuff_And_RmoveOnTargetDie, CEffectAddBuffRmvOnTargetDie);
        LOAD_EFFECT( EffectServer_CreateMoveBarrier, CEffectCreateMoveBarrier);
        LOAD_EFFECT( EffectServer_CheckCDAddEffect, CEffectCheckCDAddEffect);
        LOAD_EFFECT( EffectServer_WarChangeVoc, CEffectWarChangeVoc);
        LOAD_EFFECT( EffectServer_FakeDeadRelive, CEffectFakeDeadRelive);
        LOAD_EFFECT( EffectServer_StealthAntiedAddBuff, CEffectStealthAntiedAddBuff);

        // 没有发现的类型
        ErrorLn("undefined effect, type[" << typeName << "]");
        return false;
    }

    return true;
}

/**
@name				: 执行一个内部效果(比如技能系统注册的内置效果)
@param	strType  	: 效果类型名
@param  pData       : 效果数据
@param  nLen        : 效果数据长度
@return
*/
bool   CEffectFactory::DoInnerEffect(const char * strType,void * pData,int nLen,EFFECT_CONTEXT *pContext )
{
		// 载入效果
#undef LOAD_EFFECT
#		define     LOAD_EFFECT(typeData,typeImp)                                  \
	    if ( strcmp(strType,#typeData)==0 && nLen==sizeof(typeData) )             \
		{                                                                         \
		    typeData data = *(typeData*)pData;                                    \
			typeImp effectImp(data);                                              \
			return effectImp.Start(pContext,NULL);                                     \
		}                                                                         \

		// 依次载入
		LOAD_EFFECT( EffectServer_CoolDown,CEffectCoolDown );
		LOAD_EFFECT( EffectServer_AddBuff,CEffectAddBuff );
		LOAD_EFFECT( EffectServer_SetSpellFlag,CEffectSetSpellFlag );
		LOAD_EFFECT( EffectServer_TransLocation,CEffectTransLocation );
		LOAD_EFFECT( EffectServer_SpellCombo,CEffectSpellCombo );
		LOAD_EFFECT( EffectServer_Freeze,CEffectFreeze );
		LOAD_EFFECT( EffectServer_TimerSpellOverlay,CEffectTimerSpellOverlay );
		LOAD_EFFECT( EffectServer_ChangeProperty,CEffectChangeProperty );
		LOAD_EFFECT( EffectServer_ContinuedChangeProperty, CEffectContinuedChangeProperty);
		LOAD_EFFECT( EffectServer_CreateMonster, CEffectCreateMonster );
		LOAD_EFFECT( EffectServer_UseSpell, CEffectUseSpell );
		LOAD_EFFECT( EffectServer_BeatBack, CEffectBeatBack );
		LOAD_EFFECT( EffectServer_KillSelf, CEffectKillSelf );
		LOAD_EFFECT( EffectServer_AddRangeBuff, CEffectAddRangeBuff );
		LOAD_EFFECT( EffectServer_AddSummonMonster, CEffectAddSummonMonster );
		LOAD_EFFECT( EffectServer_LessSummonMonster, CEffectLessSummonMonster );
		LOAD_EFFECT( EffectServer_BreakSpell, CEffectBreakSpell );
		LOAD_EFFECT( EffectServer_Invisible, CEffectInvisible );
		LOAD_EFFECT( EffectServer_ContinuedDamage, CEffectContinuedDamage );
		LOAD_EFFECT( EffectServer_Feedback, CEffectFeedback );
		LOAD_EFFECT( EffectServer_ContinuedDamageEffect, CEffectContinuedDamageEffect );
        LOAD_EFFECT( EffectServer_ContinuedDamagePropertyEffect, CEffectContinuedDamagePropertyEffect );
		LOAD_EFFECT( EffectServer_ContinuedCureEffect, CEffectContinuedCureEffect );
		LOAD_EFFECT( EffectServer_WarAddBuff, CEffectWarAddBuff );
		LOAD_EFFECT( EffectServer_NoSelect, CEffectNoSelect );
		LOAD_EFFECT( EffectServer_ContinuedAddTalentPoint, CEffectContinuedAddTalentPoint );
		LOAD_EFFECT( EffectServer_RemoveBuffType, CEffectRemoveBuffType );
		LOAD_EFFECT( EffectServer_CleanEnmity, CEffectCleanEnmity );
		LOAD_EFFECT( EffectServer_ContinuedSetProperty, CEffectContinuedSetProperty );
		LOAD_EFFECT( EffectServer_SetProperty, CEffectSetProperty );
		LOAD_EFFECT( EffectServer_DragToMove, CEffectDragToMove );
		LOAD_EFFECT( EffectServer_ChangeBuff, CEffectChangeBuff );
        LOAD_EFFECT( EffectServer_SetStealth, CEffectSetStealth );
        LOAD_EFFECT( EffectServer_AntiStealth, CEffectAntiStealth );
        LOAD_EFFECT( EffectServer_SetSight, CEffectSetSight );
        LOAD_EFFECT( EffectServer_Jump, CEffectJump);
        LOAD_EFFECT( EffectServer_TimingAddBuff, CEffectTimingAddBuff);
        LOAD_EFFECT( EffectServer_ShowFloatText, CEffectShowFloatText);
        LOAD_EFFECT( EffectServer_FindTarget, CEffectFindTarget);
        LOAD_EFFECT( EffectServer_SetSingleSpellData, CEffectSetSingleSpellData);
        LOAD_EFFECT( EffectServer_SetSingleStrSpellData, CEffectSetSingleStrSpellData);
        LOAD_EFFECT( EffectServer_Upgrade, CEffectUpgrade);
        LOAD_EFFECT( EffectServer_SetWing, CEffectSetWing);
        LOAD_EFFECT( EffectServer_SetWingState, CEffectSetWingState);
        LOAD_EFFECT( EffectServer_RandAddBuff, CEffectRandAddBuff);
        LOAD_EFFECT( EffectServer_SetWingFlyTime, CEffectSetWingFlyTime);
        LOAD_EFFECT( EffectServer_CaptureBuff, CEffectCaptureBuff);
        LOAD_EFFECT( EffectServer_DistanceDamage, CEffectDistanceDamage);
        LOAD_EFFECT( EffectServer_CheckDistanceAddBuff, CEffectCheckDistanceAddBuff);
        LOAD_EFFECT( EffectServer_DistanceAddBuff, CEffectDistanceAddBuff);
        LOAD_EFFECT( EffectServer_CheckRangeBuffAddBuff, CEffectCheckRangeBuffAddBuff);
        LOAD_EFFECT( EffectServer_ChooseMonsterTransport, CEffectChooseMonsterTransport);
        LOAD_EFFECT( EffectServer_SetTargetLoc, CEffectSetTargetLoc);
        LOAD_EFFECT( EffectServer_SetSlotLevel, CEffectSetSlotLevel);
        LOAD_EFFECT( EffectServer_AddGoods, CEffectAddGoods);
        LOAD_EFFECT( EffectServer_ReplicationMonster, CEffectReplicationMonster);
        LOAD_EFFECT( EffectServer_DelayUseSpell, CEffectDelayUseSpell);
        LOAD_EFFECT( EffectServer_Roll, CEffectRoll);
        LOAD_EFFECT( EffectServer_GlobalVisible, CEffectGlobalVisible);
        LOAD_EFFECT( EffectServer_KillingControl, CEffectKillingControl);
        LOAD_EFFECT( EffectServer_ThrillingCure, CEffectThrilingCure);
        LOAD_EFFECT( EffectServer_GroupControl, CEffectGroupControl);
        LOAD_EFFECT( EffectServer_ChooseRangeAddBuff, CEffectChooseRangeAddBuff);
        LOAD_EFFECT( EffectServer_WarMonsterAddBuff, CEffectWarMonsterAddBuff);
        LOAD_EFFECT( EffectServer_SetWingProperty, CEffectSetWingProperty);
        LOAD_EFFECT( EffectServer_MapFly, CEffectMapFly);
        LOAD_EFFECT( EffectServer_NotifyClientData, CEffectNotifyClientData);
        LOAD_EFFECT( EffectServer_AddBuffOnRightButtonDown, CEffectAddBuffOnRightButtonDown);
        LOAD_EFFECT( EffectServer_StopSpell, CEffectStopSpell);
        LOAD_EFFECT( EffectServer_TraceAI, CEffectTraceAI);
		LOAD_EFFECT( EffectServer_PrepareBullet, CEffectPrepareBullet);
		LOAD_EFFECT( EffectServer_DecBulletCount, CEffectDecBulletCount);
		LOAD_EFFECT( EffectServer_UseSpellIfNoBullet, CEffectUseSpellIfNoBullet);
        LOAD_EFFECT( EffectServer_SetCanPassBarrier, CEffectSetCanPassBarrier);
        LOAD_EFFECT( EffectServer_AddBuff_And_RmoveOnTargetDie, CEffectAddBuffRmvOnTargetDie);
        LOAD_EFFECT( EffectServer_CreateMoveBarrier, CEffectCreateMoveBarrier);
        LOAD_EFFECT( EffectServer_StealthAntiedAddBuff, CEffectStealthAntiedAddBuff);

		ErrorLn("undefined effect, type[" << strType << "]");
		return false;
}

bool CEffectFactory::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return OnSchemeLoad( reader,szFileName );
}

/** 执行Service的on_stop
@return  
*/
void CEffectFactory::onStop(void)
{
    EFFECT_MAP::iterator it = m_effectMap.begin();
    for ( ;it!=m_effectMap.end();++it )
    {
        it->second->Release();
    }
    m_effectMap.clear();

    // 置空全局指针
    g_pEffectFactory = NULL;
}

// 释放
void CEffectFactory::release()
{
	delete this;
}


// 某个效果ID是否存在
bool CEffectFactory::IsExistEffectID(int nEffectID)
{
	EFFECT_MAP::iterator it = m_effectMap.find(nEffectID);
	if ( it==m_effectMap.end() )
		return false;

	return true;
}


API_EXPORT IEffectFactory* CreateEffectFactory()
{
	if(g_pEffectFactory != NULL)
	{
		return g_pEffectFactory;
	}

	// 实例化服务
	CEffectFactory *pEffectFactory = new CEffectFactory();
	if(pEffectFactory == NULL)
	{
		safeRelease(pEffectFactory);
		return NULL;
	}

	g_pEffectFactory = pEffectFactory;

	return g_pEffectFactory;
}
