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
#include "SchemeMapInfo.h"
#include "SchemeMatchRoom.h"
#include "SchemeMatchRoomRule.h"
#include "SchemeSystemMessage.h"
#include "SchemeKinDataCenter.h"
#include "SchemeClanDataCenter.h"
#include "SchemeAIIDTable.h"
#include "SchemeTeamDataCenter.h"
#include "SchemePersonModel.h"
#include "SchemeMatchComputerName.h"
#include "SchemeMatchPlan.h"
#include "SchemeLegendCup.h"
#include "SchemeMentorshipDataCenter.h"
#include "SchemeMailDataCenter.h"
#include "SchemeMatchRankLimit.h"
#include "SchemeEntity.h"
#include "SchemeMatchSeasonTime.h"
#include "SchemeMatchRankConfig.h"

extern "C" __declspec(dllexport) ISchemeCenter * createSchemeCenter()
{
	return new SchemeCenter;
}


bool SchemeCenter::load()
{
	CSchemeMatchRoom* pSchemeMatchRoom = CSchemeMatchRoom::NewInstance();
	if(pSchemeMatchRoom == NULL || !pSchemeMatchRoom->LoadScheme())
	{
		safeDelete(pSchemeMatchRoom);
		return false;
	}

	//C_SchemeMatchRoomMap* pSchemeMatchRoomMap = C_SchemeMatchRoomMap::NewInstance();
	//if(pSchemeMatchRoomMap == NULL || !pSchemeMatchRoomMap->LoadScheme())
	//{
	//	safeDelete(pSchemeMatchRoomMap);
	//	return false;
	//}

	//C_SchemeMatchRoomMode* pSchemeMatchRoomMode = C_SchemeMatchRoomMode::NewInstance();
	//if(pSchemeMatchRoomMode == NULL || !pSchemeMatchRoomMode->LoadScheme())
	//{
	//	safeDelete(pSchemeMatchRoomMode);
	//	return false;
	//}


    // 地图信息解析
	CSchemeMapInfo* pSchemeMapInfo = CSchemeMapInfo::NewInstance(); 
	if(pSchemeMapInfo == NULL || !pSchemeMapInfo->LoadScheme())
	{
		safeDelete(pSchemeMapInfo);
		return false;
	}

	CSchemeMatchRoomRule* pSchemeMatchRoomRule = CSchemeMatchRoomRule::NewInstance();
	if(pSchemeMatchRoomRule == NULL || !pSchemeMatchRoomRule->LoadScheme())
	{
		safeDelete(pSchemeMatchRoomRule);
		return false;
	}

    CSchemeKinDataCenter* pSchemeKinDataCenter = CSchemeKinDataCenter::NewInstance();
    if(pSchemeKinDataCenter == NULL || !pSchemeKinDataCenter->LoadScheme())
    {
        safeDelete(pSchemeKinDataCenter);
        return false;
    }

    CSchemeSystemMessage* pSchemeSystemMessage = CSchemeSystemMessage::NewInstance();
    if(pSchemeSystemMessage == NULL || !pSchemeSystemMessage->LoadScheme())
    {
        safeDelete(pSchemeSystemMessage);
        return false;
    }

    CSchemeClanDataCenter* pSchemeClanDataCenter = CSchemeClanDataCenter::NewInstance();
    if(pSchemeClanDataCenter == NULL || !pSchemeClanDataCenter->LoadScheme())
    {
        safeDelete(pSchemeClanDataCenter);
        return false;
    }

    CSchemeAIIDTable* pCSchemeAIIDTable = CSchemeAIIDTable::NewInstance();
    if(pCSchemeAIIDTable == NULL || !pCSchemeAIIDTable->LoadScheme())
    {
        safeDelete(pCSchemeAIIDTable);
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
	
	CSchemeTeamDataCenter* pSchemeTeamDataCenter = CSchemeTeamDataCenter::NewInstance();
    if(pSchemeTeamDataCenter == NULL || !pSchemeTeamDataCenter->LoadScheme())
    {
        safeDelete(pSchemeTeamDataCenter);
        return false;
    }

    CSchemeMatchComputerName* pSchemeMatchComputerName = CSchemeMatchComputerName::NewInstance();
    if(pSchemeMatchComputerName == NULL || !pSchemeMatchComputerName->LoadScheme())
    {
        safeDelete(pSchemeMatchComputerName);
        return false;
    }

    CSchemeMatchPlan* pSchemeMatchPlan = CSchemeMatchPlan::NewInstance();
    if(pSchemeMatchPlan == NULL || !pSchemeMatchPlan->LoadScheme())
    {
        safeDelete(pSchemeMatchPlan);
        return false;
    }

	CSchemeMentorshipDataCenter* pSchemeMentorshipDataCenter = CSchemeMentorshipDataCenter::NewInstance();
	if(pSchemeMentorshipDataCenter == NULL || !pSchemeMentorshipDataCenter->LoadScheme())
	{
		safeDelete(pSchemeMentorshipDataCenter);
		return false;
	}

	CSchemeLegendCup* pSchemeLegendCup = CSchemeLegendCup::NewInstance();
	if(pSchemeLegendCup == NULL || !pSchemeLegendCup->LoadScheme())
    {
        safeDelete(pSchemeLegendCup);
        return false;
    }

	CSchemeMailDataCenter* pSchemeMailDataCenter = CSchemeMailDataCenter::NewInstance();
	if(pSchemeMailDataCenter == NULL || !pSchemeMailDataCenter->LoadScheme())
	{
		safeDelete(pSchemeMailDataCenter);
		return false;
	}

    CSchemeMatchRankLimit* pSchemeMatchRankLimit = CSchemeMatchRankLimit::NewInstance();
    if(pSchemeMatchRankLimit == NULL || !pSchemeMatchRankLimit->LoadScheme())
    {
        safeDelete(pSchemeMatchRankLimit);
        return false;
    }

    CSchemeEntity *pSchemeEntity = CSchemeEntity::NewInstance();
    if(pSchemeEntity == NULL || !pSchemeEntity->LoadScheme())
    {
        safeDelete(pSchemeEntity);
        return false;
    }
    
	// 加载排位赛季时间配置
	CSchemeMatchSeasonTime* pSchemeMatchSeasonTime = CSchemeMatchSeasonTime::NewInstance();
	if (pSchemeMatchSeasonTime == NULL || pSchemeMatchSeasonTime->LoadScheme() == false)
	{
		safeDelete(pSchemeMatchSeasonTime);
		return false;
	}
	
	CSchemeMatchRankConfig* pSchemeMatchRankConfig = CSchemeMatchRankConfig::NewInstance();
    if(pSchemeMatchRankConfig == NULL || !pSchemeMatchRankConfig->LoadScheme())
    {
        safeDelete(pSchemeMatchRankConfig);
        return false;
    }
     
	return true;
}

void SchemeCenter::release()
{
	CSchemeMapInfo::CloseAll();
	CSchemeMatchRoom::CloseAll();
	CSchemeMatchRoomRule::CloseAll();
	CSchemeSystemMessage::CloseAll();
    CSchemeKinDataCenter::CloseAll();
    CSchemeClanDataCenter::CloseAll();
    CSchemeAIIDTable::CloseAll();
    // 删除人物皮肤模型数据
    CSchemePersonModel::CloseAll();
	CSchemeTeamDataCenter::CloseAll();
    CSchemeMatchComputerName::CloseAll();
    CSchemeMatchPlan::CloseAll();
	CSchemeMentorshipDataCenter::CloseAll();
    CSchemeLegendCup::CloseAll();
	CSchemeMailDataCenter::CloseAll();
    CSchemeMatchRankLimit::CloseAll();
    CSchemeEntity::CloseAll();
	CSchemeMatchSeasonTime::CloseAll();
	CSchemeMatchRankConfig::CloseAll();
    
    delete this;
}



/** 取得地图信息
@return  
*/
ISchemeMapInfo* SchemeCenter::getSchemeMapInfo()
{
	return  CSchemeMapInfo::GetWorkInstance();
}

ISchemeMatchRoom * SchemeCenter::getSchemeMatchRoom()
{
	return CSchemeMatchRoom::GetWorkInstance();
}

ISchemeMatchRoomRule * SchemeCenter::getSchemeMatchRoomRule()
{
	return CSchemeMatchRoomRule::GetWorkInstance();
}

// 取得飘字提示配置信息
ISchemeSystemMessage* SchemeCenter::getSchemeSystemMessage()
{
    return CSchemeSystemMessage::GetWorkInstance();
}

ISchemeKinDataCenter * SchemeCenter::getSchemeKinDataCenter()
{
    return CSchemeKinDataCenter::GetWorkInstance();
}

// 取得帮会数据中心
ISchemeClanDataCenter *	SchemeCenter::getSchemeClanDataCenter()
{
    return CSchemeClanDataCenter::GetWorkInstance();
}

// 取得AI配置相关
ISchemeAIIDTable *	SchemeCenter::getSchemeAIIDTable()
{
    return CSchemeAIIDTable::GetWorkInstance();
}

/** 取得人物模型对象
@return  
*/
ISchemePersonModel*	SchemeCenter::getSchemePersonModel()
{
    return CSchemePersonModel::GetWorkInstance();
}

// 取得组队配置相关
ISchemeTeamDataCenter *	SchemeCenter::getSchemeTeamDataCenter()
{
    return CSchemeTeamDataCenter::GetWorkInstance();
}

// 取得电脑名称配置相关
ISchemeMatchComputerName *	SchemeCenter::getSchemeMatchComputerName()
{
    return CSchemeMatchComputerName::GetWorkInstance();
}

ISchemeMatchPlan * SchemeCenter::getSchemeMatchPlan()
{
    return CSchemeMatchPlan::GetWorkInstance();
}


ISchemeLegendCup * SchemeCenter::getSchemeLegendCup()
{
    return CSchemeLegendCup::GetWorkInstance();
}

// 取得师徒数据中心
ISchemeMentorshipDataCenter *	SchemeCenter::getSchemeMentorshipDataCenter()
{
	return CSchemeMentorshipDataCenter::GetWorkInstance();
}

ISchemeMailDataCenter *	SchemeCenter::getSchemeMailDataCenter()
{
	return CSchemeMailDataCenter::GetWorkInstance();
}

ISchemeMatchRankLimitInfo* SchemeCenter::getSchemeMatchRankLimit()
{
    return CSchemeMatchRankLimit::GetWorkInstance();
}

ISchemeEntity* SchemeCenter::getSchemeEntity()
{
    return CSchemeEntity::GetWorkInstance();
}

// 获取排位赛季时间配置
ISchemeMatchSeasonTime* SchemeCenter::getSchemeMatchSeasonTime()
{
	return CSchemeMatchSeasonTime::GetWorkInstance();
}

ISchemeMatchRankConfigInfo* SchemeCenter::getSchemeMatchRankConfig()
{
    return CSchemeMatchRankConfig::GetWorkInstance();
}