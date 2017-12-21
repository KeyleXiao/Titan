/*******************************************************************
** 文件名:	SchemeWarPersonConfigInfo.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	实体通用配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeWarPersonConfigInfo.h"
#include "IServerGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeWarPersonConfigInfo::CSchemeWarPersonConfigInfo(void)
{
	
}

/** 
@param   
@param   
@return  
*/
CSchemeWarPersonConfigInfo::~CSchemeWarPersonConfigInfo(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeWarPersonConfigInfo::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( WAR_CONFIG_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("config file load failed! filename=" << stringPath.c_str() << endl);
		return false;
	}

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeWarPersonConfigInfo::Close(void)
{
}

/** CSV配置载入时通知
@param   pCSVReader：读取CSV的返回接口
@param   szFileName：配置文件名
@param   
@return  
@note     
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/ 
bool CSchemeWarPersonConfigInfo::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	TiXmlDocument *pXMLReader = reader.pXMLReader;
	if(pXMLReader == NULL || szFileName == NULL || reader.type!=READER_XML)
	{
		return false;
	}

	// 加载战场数据
	if (!loadWarData(pXMLReader))
		return false;

	// 加载其他数据
	if (!loadOtherData(pXMLReader))
		return false;

	AfterSchemeLoaded();

	return true;
}

bool CSchemeWarPersonConfigInfo::loadWarData(TiXmlDocument * pXMLReader)
{
	TiXmlElement *pRootElement = pXMLReader->FirstChildElement("scheme");
	if (pRootElement == NULL)
	{
		return false;
	}

	TiXmlElement * pChildElement = NULL;
	TiXmlElement * pGrandchildElement = NULL;
	LPCSTR pszAttrProp = NULL;

	TiXmlElement * pSubElement = pRootElement->FirstChildElement("war");
	if (pSubElement == NULL)
	{
		return false;
	}

	const char* stVigourPropertyType[VPT_ADD_MAX] = { "HP","PA" };
	while (pSubElement)
	{
		SchemeWarPersonConfigInfo WarConfigInfo;

		// WarID
		pszAttrProp = pSubElement->Attribute("war_id", (int *)&WarConfigInfo.nWarID);
		if (pszAttrProp == NULL || WarConfigInfo.nWarID <= 0)
		{
			ErrorLn("WarConfigInfo.nWarID < = 0! nWarID=" << WarConfigInfo.nWarID);
			return false;
		}

		// 不能有重复的nWarID
		if (GetSchemeWarPersonConfigInfo(WarConfigInfo.nWarID) != NULL)
		{
			ErrorLn("repeated WarConfigInfo.nWarID=" << WarConfigInfo.nWarID);
			return false;
		}

		// [config]
		pChildElement = pSubElement->FirstChildElement("config");
		if (pChildElement == NULL)
		{
			return false;
		}

        // 第一次加经验间隔
        pChildElement->Attribute("FirstAutoAddExpInterval", (int *)&WarConfigInfo.nFirstAutoAddExpInterval);
   
        // 战场初始化经验
        pChildElement->Attribute("InitialExp", (int *)&WarConfigInfo.nInitialExp);

        // 战场自动添加经验
        pChildElement->Attribute("AutoAddExp", (int *)&WarConfigInfo.nAutoAddExp);

        // 战场自动添加经验时间间隔(单位秒)
        pChildElement->Attribute("AutoAddExpInterval", (int *)&WarConfigInfo.nAutoAddExpInterval);

		// 第一次加天赋点间隔
		pChildElement->Attribute("FirstAutoAddTalentInterval", (int *)&WarConfigInfo.nFirstAutoAddTalentInterval);

		// 战场初始化天赋点
		pChildElement->Attribute("InitialTalentPoint", (int *)&WarConfigInfo.nInitialTalentPoint);

		//战场自动添加天赋点
		pChildElement->Attribute("AutoAddTalentPoint", (int *)&WarConfigInfo.nAutoAddTalentPoint);

		//战场自动添加天赋点时间间隔(单位秒)
		pChildElement->Attribute("AutoAddTalentInterval", (int *)&WarConfigInfo.nAutoAddTalentInterval);

		// 死亡经验比例
		pChildElement->QueryFloatAttribute("DieExpRate", (float *)&WarConfigInfo.fDieExpRate);

		// 死亡掉落天赋点
		pszAttrProp = pChildElement->Attribute("DieTalentPoint", (int *)&WarConfigInfo.nDieTalentPoint);
		if (pszAttrProp == NULL)
		{
			return false;
		}

		// 助攻掉落系数
		pChildElement->QueryFloatAttribute("AssistTalentRate", (float *)&WarConfigInfo.fAssistTalentRate);

		// 复活系数 秒
		pChildElement->Attribute("ReliveRatio", (int *)&WarConfigInfo.nReliveRatio);

		// 复活基数 秒
		pChildElement->Attribute("ReliveBaseTime", (int *)&WarConfigInfo.nReliveBaseTime);
		if (equalto0(WarConfigInfo.fAssistTalentRate))
		{
			ErrorLn("CSchemeEntity::OnSchemeLoad WarConfigInfo.fAssistTalentRate is 0");
			return false;
		}

		// 水晶被攻击后小地图信息时间间隔
		pChildElement->Attribute("CrystalDamageIntervalTime", (int *)&WarConfigInfo.nCrystalDamageIntervalTime);

		// 人物战斗小地图信息时间间隔
		pChildElement->Attribute("PersonFightIntervalTime", (int *)&WarConfigInfo.nPersonFightIntervalTime);

		// 掉落经验衰减系数
		pChildElement->QueryFloatAttribute("DropDumpingEXPRate", (float *)&WarConfigInfo.fDropDumpingEXPRate);

		// 掉落天赋衰减系数
		pChildElement->QueryFloatAttribute("DropDumpingTalentRate", (float *)&WarConfigInfo.fDropDumpingTalentRate);

		// 掉落经验增益系数
		pChildElement->QueryFloatAttribute("DropGainEXPRate", (float *)&WarConfigInfo.fDropGainEXPRate);

		// 掉落经验增益系数
		pChildElement->QueryFloatAttribute("DropGainTalentRate", (float *)&WarConfigInfo.fDropGainTalentRate);

		//未造成伤害掉落经验天赋添加范围
		pChildElement->Attribute("AddAroundEXPDistance", (int *)&WarConfigInfo.nAddAroundEXPDistance);

		// 击杀人数经验掉落加层系数
		pChildElement->QueryFloatAttribute("KillPersonDropEXPRate", (float *)&WarConfigInfo.fKillPersonDropEXPRate);

		// 首杀获取的金币
		pChildElement->Attribute("FirstKillGetMoney", (int*)&WarConfigInfo.nFirstKillGetMoney);

		// 野区小怪经验增益系数
		pChildElement->Attribute("IncreaseMonster", (int*)&WarConfigInfo.nIncreaseMonster);

		// 野区小怪经验增益系数上限
		pChildElement->Attribute("IncreaseMonsterMax", (int*)&WarConfigInfo.nIncreaseMonsterMax);

		// 野区小怪经验衰减系数
		pChildElement->Attribute("DecreaseMonster", (int*)&WarConfigInfo.nDecreaseMonster);

		// 野区小怪经验衰减系数
		pChildElement->Attribute("DecreaseMonsterMin", (int*)&WarConfigInfo.nDecreaseMonsterMin);

		// 小兵经验增益系数
		pChildElement->Attribute("IncreaseSolider", (int*)&WarConfigInfo.nIncreaseSolider);

		// 小兵经验增益系数上限
		pChildElement->Attribute("IncreaseSoliderMax", (int*)&WarConfigInfo.nIncreaseSoliderMax);

		// 等级控制
		pChildElement->Attribute("ControlLv", (int*)&WarConfigInfo.nControlLv);

		// 英雄增益系数
		pChildElement->Attribute("IncreaseHero", (int*)&WarConfigInfo.nIncreaseHero);

		// 英雄增益系数上限
		pChildElement->Attribute("IncreaseHeroMax", (int*)&WarConfigInfo.nIncreaseHeroMax);

		// 英雄衰减系数
		pChildElement->Attribute("DecreaseHero", (int*)&WarConfigInfo.nDecreaseHero);

		// 英雄衰减系数下限
		pChildElement->Attribute("DecreaseHeroMin", (int*)&WarConfigInfo.nDecreaseHeroMin);

		// 经济差控制系数
		pChildElement->QueryFloatAttribute("EconomyControlRate", (float*)&WarConfigInfo.fEconomyControlRate);

		// 最高连杀
		pChildElement->Attribute("MaxMultiKill", (int*)&WarConfigInfo.nMaxMultiKill);

		// 连杀间隔
		pChildElement->Attribute("TimeCountKillInterval", (int*)&WarConfigInfo.nTimeCountKillInterval);

		// 连杀间隔
		pChildElement->QueryFloatAttribute("GoldShareBase", (float*)&WarConfigInfo.fGoldShareBase);

		// 塔兵削减系数
		pChildElement->QueryFloatAttribute("TowerKillDecrease", (float*)&WarConfigInfo.fTowerKillDecrease);

		for (int nTypeIndex = VPT_ADD_HP; nTypeIndex < VPT_ADD_MAX; ++nTypeIndex)
		{
			for (int nLevelIndex = 0; nLevelIndex < MaxVigourRatioCount; ++nLevelIndex)
			{
				char	szVigourElement[32] = { 0 };
				ssprintf_s(szVigourElement, sizeof(szVigourElement), "Solider%sVigourLevel_%d", stVigourPropertyType[nTypeIndex], nLevelIndex + 1);
				pChildElement->Attribute(szVigourElement, (int*)&WarConfigInfo.VigourLevelRatio[nTypeIndex][nLevelIndex]);
			}
		}


		// 宝石经验
		pChildElement->Attribute("AddGemstoneExp", (int*)&WarConfigInfo.nAddGemstoneExp);

		// 压入map
		m_mapSchemeWarConfig[WarConfigInfo.nWarID] = WarConfigInfo;

		// 读取下一个
		pSubElement = pSubElement->NextSiblingElement("war");
	}

	return true;
}

bool CSchemeWarPersonConfigInfo::loadOtherData(TiXmlDocument *pXMLReader)
{
    TiXmlElement *pRootElement = pXMLReader->FirstChildElement("DistanceConfig");
    if(pRootElement == NULL)
    {
        return false;
    }

    // [config]
    TiXmlElement * pChildElement = NULL;
    pChildElement = pRootElement->FirstChildElement("config");
    if(pChildElement == NULL)
    {
        return false;
    }
    // 怪物的监视距离  ， 人进入这个距离之后才能打到前
    pChildElement->Attribute("MonsterDistance", (int*)&m_warPersonConfig.nMonsDistance);
    pChildElement->Attribute("TowerDistance", (int*)&m_warPersonConfig.nTowerDistance);

	return true;
}

/** 配置动态更新时通知
@param   pCSVReader：读取CSV的返回接口
@param   szFileName：配置文件名
@param   
@return  
@note     
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/
bool CSchemeWarPersonConfigInfo::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeWarPersonConfigInfo* pNewInstance = NewInstance();
	if(pNewInstance == NULL)
	{
		return false;
	}

	if(pNewInstance->OnSchemeLoad(reader, szFileName))
	{
		return true;
	}
	else
	{
		safeDelete(pNewInstance);
		return false;
	}
}

SchemeWarPersonConfigInfo* CSchemeWarPersonConfigInfo::GetSchemeWarPersonConfigInfo( int WarID )
{
	map<int,SchemeWarPersonConfigInfo>::iterator iter = m_mapSchemeWarConfig.find(WarID);
	if (iter != m_mapSchemeWarConfig.end())
	{
		return &(iter->second);
	}
	return NULL;
}

SWarPersonConfig* CSchemeWarPersonConfigInfo::GetWarPersonConfig()
{
    return &m_warPersonConfig;
}
