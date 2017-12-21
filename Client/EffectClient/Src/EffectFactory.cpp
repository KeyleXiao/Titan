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
#include "EffectFactory.h"
#include "EffectDef.h"
#include "IClientGlobal.h"
#include "EffectPlayAnimation.h"
#include "EffectAddLighting.h"
#include "EffectStopLighting.h"
#include "EffectRoot.h"
#include "EffectCoolDown.h"
#include "EffectBreakAnimation.h"
#include "EffectFlight.h"
#include "EffectStrafe.h"
#include "EffectCutOffAnimation.h"
#include "EffectReplaceSlot.h"
#include "EffectNoTurnTo.h"
#include "EffectStopMoveHandle.h"
#include "EffectDisableSpell.h"
#include "EffectTurnToDirection.h"
#include "EffectSetSpellSlot.h"
#include "EffectForbidMouseTurnTo.h"
#include "EffectPlaySound.h"
#include "EffectChangeGodEyeMode.h"
#include "EffectSetSingleSpellData.h"
#include "EffectSetSingleStrSpellData.h"
#include "EffectChangeWingState.h"
#include "EffectOrderPlayLighting.h"
#include "EffectNoSelect.h"
#include "EffectShowPromptUI.h"
#include "EffectStickyCamerra.h"
#include "EffectHoldChangeOperation.h"
#include "EffectClearAttackCommand.h"
#include "EffectSetObjRangeTip.h"
#include "EffectCastSpell.h"
#include "EffectRandPlayAnimation.h"
#include "EffectChangeDodgeState.h"
#include "EffectShowChooseTarget.h"
#include "EffectChangeSniperscope.h"
#include "EffectShowCommonMessageBox.h"
#include "EffectSpellAttribute.h"
#include "EffectSubscibePlayAnimation.h"
#include "EffectBeSneered.h"
#include "EffectLongJump.h"
#include "EffectGlide.h"
#include "EffectSetCameraHeight.h"
#include "EffectReportOnRightButtonDown.h"
#include "EffectRoll2Target.h"
#include "EffectAutoMove.h"
#include "EffectSwitchBulletPanel.h"
#include "EffectShowDebuffProgressBar.h"

using namespace EFFECT_CLIENT;

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
	if ( !gClientGlobal->getSchemeEngine()->LoadScheme(stringPath.c_str(),this,true ) )
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
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
    {
        return false;
    }

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

        // 服务器条件不读入
        if ( typeName.find("EffectServer")!=std::string::npos )
            continue;

        // 载入效果
#		define     LOAD_EFFECT(typeData,typeImp)                                  \
    if ( typeName==#typeData && pEffectHeader->nTypeSize==sizeof(typeData) )  \
        {                                                                         \
        for ( int i=0;i<pEffectHeader->nItemTotal;++i )                       \
        {                                                                     \
        typeData * pEffect = (typeData*)pEffectData + i;                  \
        pair<EFFECT_MAP::iterator, bool> result = m_effectMap.insert(EFFECT_MAP::value_type(pEffect->nID, new typeImp(*pEffect)));          \
        if (!result.second)												  \
        {																  \
        ErrorLn("There is repeated effect,ID="<<pEffect->nID);		  \
        return false;												  \
        }																  \
        }                                                                     \
        continue;                                                             \
        }                                                                         \

        // 依次载入
        LOAD_EFFECT(EffectClient_PlayAnimation, CEffectPlayAnimation);
        LOAD_EFFECT(EffectClient_AddLighting, CEffectAddLighting);
        LOAD_EFFECT(EffectClient_StopLighting, CEffectStopLighting);
        LOAD_EFFECT(EffectClient_Root, CEffectRoot);
        LOAD_EFFECT(EffectClient_CoolDown, CEffectCoolDown);
        LOAD_EFFECT(EffectClient_BreakAnimation, CEffectBreakAnimation);
        LOAD_EFFECT(EffectClient_Flight, CEffectFlight);
        LOAD_EFFECT(EffectClient_Strafe, CEffectStrafe);
        LOAD_EFFECT(EffectClient_CutOffAnimation, CEffectCutOffAnimation);
        LOAD_EFFECT(EffectClient_ReplaceSlot, CEffectReplaceSlot);
        LOAD_EFFECT(EffectClient_NoTurnTo, CEffectNoTurnTo);
        LOAD_EFFECT(EffectClient_StopMoveHandle, CEffectStopMoveHandle);
        LOAD_EFFECT(EffectClient_DisableSpell, CEffectDisableSpell);
        LOAD_EFFECT(EffectClient_TurnToDirection, CEffectTurnToDirection);
        LOAD_EFFECT(EffectClient_SetSpellSlot, CEffectSetSpellSlot);
        LOAD_EFFECT(EffectClient_ForbidMouseTurnTo, CEffectForbidMouseTurnTo);
        LOAD_EFFECT(EffectClient_PlaySound, CEffectPlaySound);
        LOAD_EFFECT(EffectClient_ChangeGodEyeMode, CEffectChangeGodEyeMode);
        LOAD_EFFECT(EffectClient_SetSingleSpellData, CEffectSetSingleSpellData);
        LOAD_EFFECT(EffectClient_SetSingleStrSpellData, CEffectSetSingleStrSpellData);
        LOAD_EFFECT(EffectClient_ChangeWingState, CEffectChangeWingState);
        LOAD_EFFECT(EffectClient_OrderPlayLighting, CEffectOrderPlayLighting);
        LOAD_EFFECT(EffectClient_NoSelect, CEffectNoSelect);
        LOAD_EFFECT(EffectClient_ShowPromptUI, CEffectShowPromptUI);
        LOAD_EFFECT(EffectClient_StickyCamerra, CEffectStickyCamerra);
        LOAD_EFFECT(EffectClient_HoldChangeOperation, CEffectHoldChangeOperation);
        LOAD_EFFECT(EffectClient_ClearAttackCommand, CEffectClearAttackCommand);
        LOAD_EFFECT(EffectClient_SetObjRangeTip, CEffectSetObjRangeTip);
        LOAD_EFFECT(EffectClient_CastSpell, CEffectCastSpell);
        LOAD_EFFECT(EffectClient_RandPlayAnimation, CEffectRandPlayAnimation);
        LOAD_EFFECT(EffectClient_ChangeDodgeState, CEffectChangeDodgeState);
        LOAD_EFFECT(EffectClient_ShowChooseTarget, CEffectShowChooseTarget);
        LOAD_EFFECT(EffectClient_ChangeSniperscope, CEffectChangeSniperscope);
        LOAD_EFFECT(EffectClient_ShowCommonMessageBox, CEffectShowCommonMessageBox);
        LOAD_EFFECT(EffectClient_SpellAttribute, CEffectSpellAttribute);
        LOAD_EFFECT(EffectClient_SubscibePlayAnimation, CEffectSubscibePlayAnimation);
        LOAD_EFFECT(EffectClient_BeSneered, CEffectBeSneered);
        LOAD_EFFECT(EffectClient_LongJump, CEffectLongJump);
        LOAD_EFFECT(EffectClient_Glide, CEffectGlide);
        LOAD_EFFECT(EffectClient_SetCameraHeight, CEffecSetCameraHeight);
        LOAD_EFFECT(EffectClient_ReportOnRightButtonDown, CEffectReportOnRightButtonDown);
		LOAD_EFFECT(EffectClient_Roll2Target, CEffectRoll2Target);
		LOAD_EFFECT(EffectClient_AutoMove, CEffectAutoMove);
		LOAD_EFFECT(EffectClient_SwitchBulletPanel, CEffectSwitchBulletPanel);
		LOAD_EFFECT(EffectClient_ShowDebuffProgressBar, CEffectShowDebuffProgressBar);

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
#undef LOAD_EFFECT
		// 载入效果
#		define     LOAD_EFFECT(typeData,typeImp)                                  \
	    if ( strcmp(strType,#typeData)==0 && nLen==sizeof(typeData) )             \
		{                                                                         \
		    typeData data = *(typeData*)pData;                                    \
			typeImp effectImp(data);                                              \
			return effectImp.Start(pContext);                                     \
		}                                                                         \

		// 依次载入
		LOAD_EFFECT(EffectClient_PlayAnimation, CEffectPlayAnimation);
		LOAD_EFFECT(EffectClient_AddLighting, CEffectAddLighting);
		LOAD_EFFECT(EffectClient_StopLighting, CEffectStopLighting);
		LOAD_EFFECT(EffectClient_Root, CEffectRoot);
		LOAD_EFFECT(EffectClient_CoolDown, CEffectCoolDown);
		LOAD_EFFECT(EffectClient_BreakAnimation, CEffectBreakAnimation);
		LOAD_EFFECT(EffectClient_Flight, CEffectFlight);
		LOAD_EFFECT(EffectClient_Strafe, CEffectStrafe);
		LOAD_EFFECT(EffectClient_CutOffAnimation, CEffectCutOffAnimation);
		LOAD_EFFECT(EffectClient_ReplaceSlot, CEffectReplaceSlot);
		LOAD_EFFECT(EffectClient_NoTurnTo, CEffectNoTurnTo);
		LOAD_EFFECT(EffectClient_StopMoveHandle, CEffectStopMoveHandle);
		LOAD_EFFECT(EffectClient_DisableSpell, CEffectDisableSpell);
		LOAD_EFFECT(EffectClient_TurnToDirection, CEffectTurnToDirection);
		LOAD_EFFECT(EffectClient_SetSpellSlot, CEffectSetSpellSlot);
		LOAD_EFFECT(EffectClient_ForbidMouseTurnTo, CEffectForbidMouseTurnTo);
		LOAD_EFFECT(EffectClient_PlaySound, CEffectPlaySound);
        LOAD_EFFECT(EffectClient_ChangeGodEyeMode, CEffectChangeGodEyeMode);
        LOAD_EFFECT(EffectClient_SetSingleSpellData, CEffectSetSingleSpellData);
        LOAD_EFFECT(EffectClient_SetSingleStrSpellData, CEffectSetSingleStrSpellData);
        LOAD_EFFECT(EffectClient_ChangeWingState, CEffectChangeWingState);
        LOAD_EFFECT(EffectClient_OrderPlayLighting, CEffectOrderPlayLighting);
        LOAD_EFFECT(EffectClient_ClearAttackCommand, CEffectClearAttackCommand);
        LOAD_EFFECT(EffectClient_CastSpell, CEffectCastSpell);
        LOAD_EFFECT(EffectClient_RandPlayAnimation, CEffectRandPlayAnimation);
        LOAD_EFFECT(EffectClient_ShowCommonMessageBox, CEffectShowCommonMessageBox);
        LOAD_EFFECT(EffectClient_SpellAttribute, CEffectSpellAttribute);
        LOAD_EFFECT(EffectClient_LongJump, CEffectLongJump);
        LOAD_EFFECT(EffectClient_Glide, CEffectGlide);
        LOAD_EFFECT(EffectClient_SetCameraHeight, CEffecSetCameraHeight);
        LOAD_EFFECT(EffectClient_ReportOnRightButtonDown, CEffectReportOnRightButtonDown);
		LOAD_EFFECT(EffectClient_Roll2Target, CEffectRoll2Target);
		LOAD_EFFECT(EffectClient_AutoMove, CEffectAutoMove);
		LOAD_EFFECT(EffectClient_SwitchBulletPanel, CEffectSwitchBulletPanel);
		LOAD_EFFECT(EffectClient_ShowDebuffProgressBar, CEffectShowDebuffProgressBar);

		ErrorLn("undefined effect, type[" << strType << "]");
		return false;
}


bool CEffectFactory::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return OnSchemeLoad( reader,szFileName );
}


// 释放
void CEffectFactory::release()
{
	EFFECT_MAP::iterator it = m_effectMap.begin();
	for ( ;it!=m_effectMap.end();++it )
	{
		it->second->Release();
	}
	m_effectMap.clear();

	// 置空全局指针
	g_pEffectFactory = NULL;

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
