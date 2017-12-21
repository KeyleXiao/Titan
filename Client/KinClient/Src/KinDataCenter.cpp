//==========================================================================
/**
* @file	  : KinDataCenter.cpp
* @author : xiaobao
* created : 2009-3-7
* purpose : 家族数据中心
*/
//==========================================================================

#include "stdafx.h"
#include "KinDataCenter.h"
#include "ExternalHelper.h"
#include "KinDef.h"
#include "ISocialGlobal.h"

// 全局家族数据中心对象
CKinDataCenter g_KinDataCenter;

/////////////////////////////// 初始化 ////////////////////////////////////
/// purpose: 构造函数
CKinDataCenter::CKinDataCenter()
{

}

/// purpose: 虚构函数
CKinDataCenter::~CKinDataCenter()
{

}

/// purpose: 创建
bool CKinDataCenter::Create()
{
    ISchemeEngine * pSchemeEngine = gSocialGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL )
    {
        return false;
    }
	
	// 家族常量脚本
	string strPath = KIN_CONFIG_FILENAME;
	bool bResult = pSchemeEngine->LoadScheme(strPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("加载配置文件失败。文件名 = " << strPath.c_str() << endl);
		return false;
	}

    strPath = KIN_LEVEL_FILENAME;
    bResult = pSchemeEngine->LoadScheme(strPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        Error("加载配置文件失败。文件名 = " << strPath.c_str() << endl);
        return false;
    }

	return true;
}

/// purpose: 销毁
void CKinDataCenter::Close()
{
	
}

///////////////////////// ISchemeUpdateSink /////////////////////////
/// purpose: CSV配置载入时通知
bool CKinDataCenter::OnSchemeLoad(SCRIPT_READER reader, const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    Close(); 
	
	if (strcmpi(szFileName, KIN_CONFIG_FILENAME) == 0)
	{
		return LoadConfigScheme(pCSVReader);
	}
    else if (strcmpi(szFileName, KIN_CONFIG_FILENAME) == 0)
    {
        return LoadLevelScheme(pCSVReader);
    }

	return false;
}



/// purpose: CSV配置动态更新时通知
bool CKinDataCenter::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}


/// purpose:  加载家族常量脚本
bool CKinDataCenter::LoadConfigScheme(ICSVReader * pCSVReader)
{
	string strLoadFileName = KIN_CONFIG_FILENAME;

	char szTemp[512] = {0};
	int arrTime[4] = {0};
	
	memset(&m_Config, 0, sizeof(m_Config));

	// 创建家族需要的等级
	m_Config.nCreateLevel = pCSVReader->GetInt(0, 1, 0);
	if (m_Config.nCreateLevel <= 0 || m_Config.nCreateLevel > 100)
	{
		return false;
	}
	// 创建家族需要的金钱
	m_Config.nCreateMoney = pCSVReader->GetInt(1, 1, 0);
	if (m_Config.nCreateMoney < 0)
	{
		return false;
	}
	// 响应家族需要的等级
	m_Config.nRespondLevel = pCSVReader->GetInt(2, 1, 0);
	if (m_Config.nRespondLevel <= 0 || m_Config.nRespondLevel > 100)
	{
		return false;
	}
	// 响应家族需要的人数
	m_Config.nRespondNeedCount = pCSVReader->GetInt(3, 1, 0);
	if (m_Config.nRespondNeedCount < 0)
	{
		return false;
	}
	// 临时家族的维持时间
	m_Config.nInformalLife = pCSVReader->GetInt(4, 1, 0);
	if (m_Config.nInformalLife < 0)
	{
		return false;
	}
	// x天没人上线，家族将被解散（单位：天）
	m_Config.nNologinLife = pCSVReader->GetInt(5, 1, 0);
	if (m_Config.nNologinLife < 0)
	{
		return false;
	}

	// 解散家族的生存周期
	m_Config.nDismissingLife = pCSVReader->GetInt(6, 1, 0);
	if (m_Config.nDismissingLife < 0)
	{
		return false;
	}
	
	// 开战消耗的家族荣誉值
	m_Config.nFightCostCredit = pCSVReader->GetInt(7, 1, 0);

	// 家族战的维持时间
	m_Config.nWarLastTime = pCSVReader->GetInt(8, 1, 0);
	
	// 每日最大发起挑战次数限制
	m_Config.dwFightDayMaxCount = pCSVReader->GetInt(9, 1, 0);
	
	// 每日对同一家族发起挑战次数限制
	m_Config.dwFightKinMaxCount = pCSVReader->GetInt(10, 1, 0);
	
	// 同时可以发起挑战的个数限制
	m_Config.dwFightOneTimeMaxCount = pCSVReader->GetInt(11, 1, 0);
	
	// 家族战击杀同一个敌人积分衰减（x/100）
	m_Config.nWarScoreFallRate = pCSVReader->GetInt(12, 1, 0);
	
	// 家族战击杀族长的加成（x/100）
	m_Config.nWarShaikhRate = pCSVReader->GetInt(13, 1, 0);
	
	// 家族战击杀副族长的加成（x/100）
	m_Config.nWarElderRate = pCSVReader->GetInt(14, 1, 0);
	
	// 是否启动每日限制
	int nLen = sizeof(szTemp);									// 改到这里
	pCSVReader->GetString(15, 1, szTemp, nLen);
	if (strncmp(szTemp, "", 128) == 0)
	{
		m_Config.bDayFightLimit = false;
	}
	else
	{
		m_Config.bDayFightLimit = true;
		int nCount = parseIntArray<int>(szTemp, m_Config.nDayFightLimitTime, 4);
		if (nCount != 4)
		{
			Error(strLoadFileName.c_str() << "（24, 1）位置读取错误" << endl);
			return false;
		}
		nLen = sizeof(m_Config.szDayLimitDecs);
		pCSVReader->GetString(15, 2, m_Config.szDayLimitDecs, nLen);
	}
	
	// 周几限制
	for (int week = 0; week < 7; week ++)
	{
		nLen = sizeof(szTemp);
		pCSVReader->GetString(16 + week, 1, szTemp, nLen);
		if (strncmp(szTemp, "", 128) == 0)
		{
			m_Config.bWeekFightLimit[week] = false;
		}
		else
		{
			m_Config.bWeekFightLimit[week] = true;
			int nCount = parseIntArray<int>(szTemp, m_Config.nWeekFightLimitTime[week], 4);
			if (nCount != 4)
			{
				Error(strLoadFileName.c_str() << " （" << (25 + week) << ", 1）位置读取错误" << endl);
				return false;
			}
			nLen = sizeof(m_Config.szWeekLimitDecs[week]);
			pCSVReader->GetString(16 + week, 2, m_Config.szWeekLimitDecs[week], nLen);
		}
	}
	
	// 具体日期限制
	int index = 0;
	for (int line = 23; line < 23 + KINWAR_MAX_DATECOUNT * 2; line += 2)
	{
		nLen = sizeof(szTemp);
		pCSVReader->GetString(line, 1, szTemp, nLen);
		if (strncmp(szTemp, "", 128) == 0)
		{
			m_Config.bDateFightLimit[index] = false;
		}
		else
		{
			m_Config.bWeekFightLimit[index] = true;
			int nCount = parseIntArray<int>(szTemp, m_Config.nDateFightLimitDate[index], 3);
			if (nCount != 3)
			{
				Error(strLoadFileName.c_str() << " （" << (line) << ", 1）位置读取错误" << endl);
				return false;
			}
			nLen = sizeof(m_Config.szDateLimitDecs[index]);
			pCSVReader->GetString(line, 2, m_Config.szDateLimitDecs[index], nLen);
	
			// 读取时间段
			nLen = sizeof(szTemp);
			pCSVReader->GetString(line + 1, 1, szTemp, nLen);
			nCount = parseIntArray<int>(szTemp, m_Config.nDateFightLimitTime[index], 4);
			if (nCount != 4)
			{
				Error(strLoadFileName.c_str() << " （" << (line + 1) << ", 1）位置读取错误" << endl);
			}
		}

		index ++;
	}

	// 是否启动国战时段限制
	nLen = sizeof(szTemp);									// 改到这里
	pCSVReader->GetString(33, 1, szTemp, nLen);
	if (strncmp(szTemp, "", 128) == 0)
	{
		m_Config.bSiegeWarLimit = false;
	}
	else
	{
		m_Config.bSiegeWarLimit = true;
		int nCount = parseIntArray<int>(szTemp, m_Config.nSiegeWarLimitTime, 4);
		if (nCount != 4)
		{
			Error(strLoadFileName.c_str() << "（33, 1）位置读取错误" << endl);
			return false;
		}
		nLen = sizeof(m_Config.szSiegeWarLimitDecs);
		pCSVReader->GetString(33, 2, m_Config.szSiegeWarLimitDecs, nLen);
	}

	nLen = sizeof(szTemp);
	pCSVReader->GetString(34, 1, szTemp, nLen);
	if (strncmp(szTemp, "", 128) != 0)
	{
		parseIntArray<int>(szTemp, m_Config.nGSBuffID, KINSKILL_MAX_GS_COUNT);
	}
	pCSVReader->GetString(35, 1, szTemp, nLen);
	if (strncmp(szTemp, "", 128) != 0)
	{
		parseIntArray<int>(szTemp, m_Config.nDJBuffID, KINSKILL_MAX_DJ_COUNT);
	}

	// 五行决组合Buff
	m_Config.nGSCountBuffID = pCSVReader->GetInt(36, 1, 0);
	// 断生组合Buff
	m_Config.nDJCountBuffID = pCSVReader->GetInt(37, 1, 0);

	// 经验共享比例系数（x/100）
	m_Config.nShareRate = pCSVReader->GetInt(38, 1, 0);

	// 一次最多只能喂多少个食物
	m_Config.nMaxFoodCount = pCSVReader->GetInt(39, 1, 0);

	// 一天最多只能喂多少次
	m_Config.nMaxFeedCount = pCSVReader->GetInt(40, 1, 0);

	// 家族活跃度初始值
	m_Config.nInitActivity = pCSVReader->GetInt(41, 1, 0);

	// 解散活跃度
	m_Config.nDismissActivity = pCSVReader->GetInt(42, 1, 0);

	// 最高活跃度（高于这个值不能加）
	m_Config.nMaxActivity = pCSVReader->GetInt(43, 1, 0);

	// 开区几天后才开始排名
	m_Config.nSortDay = pCSVReader->GetInt(44, 1, 0);

	// 战车重命名消耗家族资金
	m_Config.nTankRenameCost = pCSVReader->GetInt(45, 1, 0);

	// 多长时间战车没人则归还（单位：秒）
	m_Config.nMaxNoPersonTime = pCSVReader->GetInt(46, 1, 0);

	// 租用时间（单位：秒）
	m_Config.nHireLastTime = pCSVReader->GetInt(47, 1, 0);

	// 禁用家族战车
	m_Config.nIsLimitKinTank = pCSVReader->GetInt(48, 1, 0);

	// 是否启动国王战时段限制
	nLen = sizeof(szTemp);
	pCSVReader->GetString(49, 1, szTemp, nLen);
	if (strncmp(szTemp, "", 128) == 0)
	{
		m_Config.bKingWarLimit = false;
	}
	else
	{
		m_Config.bKingWarLimit = true;
		int nCount = parseIntArray<int>(szTemp, m_Config.nKingWarLimitTime, 5);
		if (nCount != 5)
		{
			Error(strLoadFileName.c_str() << "（49, 1）位置读取错误" << endl);
			return false;
		}
		nLen = sizeof(m_Config.szKingWarLimitDecs);
		pCSVReader->GetString(49, 2, m_Config.szKingWarLimitDecs, nLen);
	}

	// 增加家族资金，百分多少给帮会
	//m_Config.nClanGoldPercent = pCSVReader->GetInt(50, 1, 0);
	// 同时可以接受挑战的个数限制
	m_Config.dwAcceptFightMaxCount = pCSVReader->GetInt(50, 1, 0);

	// 重命名资金
	m_Config.nRenameMoney = pCSVReader->GetInt(53, 1, 0);
	if( m_Config.nRenameMoney <= 0 )
	{
		Error(KIN_CONFIG_FILENAME << "（53, 1）位置读取错误" << endl);
		return false;
	}

	m_Config.nChangeLandMinNum = pCSVReader->GetInt(54, 1, 0);
	if( m_Config.nChangeLandMinNum <= 0 )
	{
		ErrorLn("CKinDataCenter::LoadConfigScheme nChangeLandMinNum值非法");
		return false;
	}

	return true;
}

/// purpose:  加载家族升级脚本
bool CKinDataCenter::LoadLevelScheme(ICSVReader * pCSVReader)
{
    int nRecordCount = (int)pCSVReader->GetRecordCount();
    SKinLevel levelInfo;
    memset(m_LevelInfoList, 0, sizeof(m_LevelInfoList));
    for (int i = 0; i < nRecordCount; i ++)
    {
        // 家族等级
        int nLevel = pCSVReader->GetInt(i, 0, 0);
        if (nLevel <= 0 || nLevel > KIN_MAX_LEVEL)
        {
            Error(KIN_LEVEL_FILENAME << "（"<< i << "，0）位置读取错误" << endl);
            return false;
        }

        // 最大人数
        levelInfo.nMaxMemberCount = pCSVReader->GetInt(i, 1, 0);
        if (levelInfo.nMaxMemberCount < 0)
        {
            Error(KIN_LEVEL_FILENAME << "（"<< i << "，1）位置读取错误" << endl);
            return false;
        }


        // 家族Boss成长速度
        levelInfo.nBossSpeedUp = pCSVReader->GetInt(i, 2, 0);
        if (levelInfo.nBossSpeedUp < 100)
        {
            Error(KIN_LEVEL_FILENAME << "（"<< i << "，3）位置读取错误 不能小于100" << endl);
            return false;
        }

        levelInfo.nNeedActivity = pCSVReader->GetInt(i, 3, 0);
        levelInfo.nMinusActivity = pCSVReader->GetInt(i, 4, 0);

        levelInfo.nMaxKinTankNum = pCSVReader->GetInt(i, 5, 0);

        m_LevelInfoList[nLevel] = levelInfo;
    }

    return true;
}


/// purpose: 取得家族常量信息
SKinConfig* CKinDataCenter::GetConfig()
{
	return &m_Config;
}

/////////////////////////////// CKinDataCenter ////////////////////////////////////
/// purpose: 取得家族每级信息
SKinLevel* CKinDataCenter::GetLevelInfo(int nKinLevel)
{
    if (nKinLevel <= 0 || nKinLevel > KIN_MAX_LEVEL)
    {
        return NULL;
    }


    return &m_LevelInfoList[nKinLevel];
}