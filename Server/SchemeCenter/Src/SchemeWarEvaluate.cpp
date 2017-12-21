/*******************************************************************
** 文件名:	SchemeWarEvaluate.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2015/12/4
** 版  本:	1.0
** 描  述:	战场最后评分配置表
********************************************************************/
#include "StdAfx.h"
#include "SchemeWarEvaluate.h"
#include "IServerGlobal.h"


CSchemeWarEvaluate::CSchemeWarEvaluate()
{

}
CSchemeWarEvaluate::~CSchemeWarEvaluate()
{

}

bool CSchemeWarEvaluate::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader* pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV )
    {
        return false;
    }
    // 清空
    m_warEvaluateMap.clear();

    int nCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nCount; ++nRow )
    {
        int nIndex = 0;
        SWarEvaluathInfo itemInfo;

        itemInfo.nMatchTypeID = pCSVReader->GetInt(nRow, nIndex++, 0);
        itemInfo.nComprehensiveScoreEnlargeR = pCSVReader->GetInt(nRow, nIndex++, 0);
		itemInfo.fKillDestroyWR = pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fTeamContributionWR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fSpecialContributionWR = pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.nAliveMultiKillT = pCSVReader->GetInt(nRow, nIndex++, 0);
		itemInfo.fAliveMultiKillR = pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fAliveMultiKillTScore = pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.nTimeMultiKillT = pCSVReader->GetInt(nRow, nIndex++, 0);
		itemInfo.fTimeMultiKillR = pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fTimeMultiKillTScore = pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fDestoryTowerR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fKillDragenR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fKDAWR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fAliveMultiKillWR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fTimeMultiKillWR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fDestoryTowerWR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fKillDragenWR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fKillScoreFunctionParam1= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fKillScoreFunctionParam2= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fOutputDamageWithMoneyBaseWR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fOutputDamageWithMoneyWinLoseWR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fKDAR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fThrillSaveHPT= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fThrillSaveTimeT= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fThrillSaveTimeCool= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fThrillSaveRange= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fDeadlyControlTimeT= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fDeadlyControlTimeCool= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.nTeamBattleControlT= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fTeamBattleControlR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fTeamBattleControlTScore= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fBearDamageWR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fCureWR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fThrillSaveWR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fDeadlyControlWR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fTeamBattleControlWR= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fCureRateHonorT= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.nThrillSaveHonorT= pCSVReader->GetInt(nRow, nIndex++, 0);
		itemInfo.nDeadlyControlHonorT= pCSVReader->GetInt(nRow, nIndex++, 0);
		itemInfo.fTeamBattleControlHonorT= pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fKillDestroyMin = pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fKillDestroyMax = pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fTeamContributionMin = pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fTeamContributionMax = pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fSpecialContributionMin = pCSVReader->GetFloat(nRow, nIndex++, 0);
		itemInfo.fSpecialContributionMax = pCSVReader->GetFloat(nRow, nIndex++, 0);

		m_warEvaluateMap[itemInfo.nMatchTypeID] = itemInfo;
	}
    AfterSchemeLoaded();
    return true;
}

bool CSchemeWarEvaluate::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return false;
}

bool CSchemeWarEvaluate::LoadScheme(void)
{
    ISchemeEngine* pSchemeEngine = gServerGlobal->getSchemeEngine();
    if ( pSchemeEngine == NULL )
    {
        return false;
    }

    string stringPath = SCP_PATH_FORMAT( WAR_SCHEME_EVALUATH );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink* )this);
    if ( !bResult )
    {
        ErrorLn(__FUNCTION__ " failed : stPath = " << stringPath.c_str() );
        return false;
    }
    return true;
}

void CSchemeWarEvaluate::Close(void)
{
    m_warEvaluateMap.clear();
}

SWarEvaluathInfo* CSchemeWarEvaluate::getEvaluathInfo( int nMatchTypeID )
{
    std::map<int, SWarEvaluathInfo>::iterator it = m_warEvaluateMap.find(nMatchTypeID);
    if ( it != m_warEvaluateMap.end())
    {
        return &it->second;
    }
    return NULL;
}