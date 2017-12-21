#include "stdafx.h"
#include "SchemeClanDataCenter.h"
#include "ISchemeEngine.h"
#include "IServerGlobal.h"

/// purpose: 构造函数
CSchemeClanDataCenter::CSchemeClanDataCenter()
{
	memset(m_LevelInfoList, 0, sizeof(m_LevelInfoList));
}

/// purpose: 虚构函数
CSchemeClanDataCenter::~CSchemeClanDataCenter()
{

}

/// purpose: 载入
bool CSchemeClanDataCenter::LoadScheme()
{
	ISchemeEngine* pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 帮会升级脚本
	string strPath = SCP_PATH_FORMAT(CLAN_LEVEL_FILENAME);
	bool bResult = pSchemeEngine->LoadScheme(strPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("加载配置文件失败。文件名 = " << strPath.c_str() << endl);
		return false;
	}

	// 帮会常量脚本
	strPath = SCP_PATH_FORMAT(CLAN_CONFIG_FILENAME);
	bResult = pSchemeEngine->LoadScheme(strPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("加载配置文件失败。文件名 = " << strPath.c_str() << endl);
		return false;
	}

	// 帮会卡牌脚本
	strPath = SCP_PATH_FORMAT(CLAN_CARD_FILENAME);
	bResult = pSchemeEngine->LoadScheme(strPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		ErrorLn("加载配置文件失败。文件名 = " << strPath.c_str() << endl);
		return false;
	}

	return true;
}

/// purpose: 销毁
void CSchemeClanDataCenter::Close()
{
	memset(m_LevelInfoList, 0, sizeof(m_LevelInfoList));
}

///////////////////////////////ISchemeUpdateSink///////////////////	
bool CSchemeClanDataCenter::OnSchemeLoad(SCRIPT_READER reader, const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    bool bRet = false;

	if (rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(CLAN_LEVEL_FILENAME)) == 0)
    {
        bRet = LoadLevelScheme(pCSVReader,szFileName);
    }
	else if (rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(CLAN_CONFIG_FILENAME)) == 0)
	{
		bRet = LoadConfigScheme(pCSVReader,szFileName);
	}
	else if (rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT( CLAN_CARD_FILENAME )) == 0)
	{
		bRet = LoadCardScheme(pCSVReader,szFileName);
	}

    if(!bRet)
    {
        return false;
    }	

    AfterSchemeLoaded();
	
	return true;
}

bool CSchemeClanDataCenter::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeClanDataCenter* pNewInstance = NewInstance();
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
    
    return false;
}

/// purpose: 取得帮会每级信息
SClanLevel* CSchemeClanDataCenter::getLevelInfo(int nClanLevel)
{
	if (nClanLevel <= 0 || nClanLevel > CLAN_MAX_LEVEL)
	{
		return NULL;
	}


	return &m_LevelInfoList[nClanLevel];
}

/// purpose: 取得帮会常量信息
SClanConfig* CSchemeClanDataCenter::getConfig()
{
	return &m_Config;
}

/// purpose: 帮会卡牌信息
SClanCardSchemeInfo* CSchemeClanDataCenter::getClanCardInfo(int nLevel)
{
	CLAN_CARD_MAP::iterator Iter = m_ClanCardMap.find(nLevel);
	if (Iter == m_ClanCardMap.end())
	{
		return NULL;
	}

	return &Iter->second;
}

// 加载帮会升级脚本
bool CSchemeClanDataCenter::LoadLevelScheme(ICSVReader * pCSVReader,const char* szFileName)
{
	int nRecordCount = (int)pCSVReader->GetRecordCount();
	SClanLevel levelInfo;
	memset(m_LevelInfoList, 0, sizeof(m_LevelInfoList));
	for (int i = 0; i < nRecordCount; i ++)
	{
		// 帮会等级
		int nLevel = pCSVReader->GetInt(i, 0, 0);
		if (nLevel <= 0 || nLevel > CLAN_MAX_LEVEL)
		{
			Error(szFileName << "（"<< i << "，0）位置读取错误" << endl);
			return false;
		}

		// 最大战队数
		levelInfo.nMaxKinCount = pCSVReader->GetInt(i, 1, 0);
		if (levelInfo.nMaxKinCount < 0)
		{
			Error(szFileName << "（"<< i << "，1）位置读取错误" << endl);
			return false;
		}

		// 本等级需要活跃度
		levelInfo.nNeedActivity = pCSVReader->GetInt(i, 2, 0);

		// 每天扣除活跃度
		levelInfo.nMinusActivity = pCSVReader->GetInt(i, 3, 0);

		// 最大人数
		levelInfo.nMaxMemberCount = pCSVReader->GetInt(i, 4, 0);
		if (levelInfo.nMaxMemberCount < 0)
		{
			Error(szFileName << "（"<< i << "，1）位置读取错误" << endl);
			return false;
		}

		m_LevelInfoList[nLevel] = levelInfo;
	}

	return true;
}

// 加载帮会配置脚本
bool CSchemeClanDataCenter::LoadConfigScheme(ICSVReader * pCSVReader,const char* szFileName)
{
    char szTemp[512] = {0};
    int nLine = 0;
	// 创建帮会需要的等级
	m_Config.nCreateLevel = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nCreateLevel <= 0 || m_Config.nCreateLevel > PERSON_MAX_LEVEL)
	{
		ErrorLn(szFileName << (nLine - 1) << "位置读取错误" << endl);
		return false;
	}
	// 创建帮会需要的金钱
	m_Config.nCreateMoney = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nCreateMoney < 0)
	{
		ErrorLn(szFileName << (nLine - 1) << "位置读取错误" << endl);
		return false;
	}
	// 解散帮会的生存周期（单位秒）
	m_Config.nDismissingLife = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nDismissingLife < 0)
	{
		ErrorLn(szFileName << (nLine - 1) << "位置读取错误" << endl);
		return false;
	}
	// 周活跃度连续{B6}周低于{B7}自动解散
	m_Config.nDismissWeeks = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nDismissWeeks < 0)
	{
		ErrorLn(szFileName << (nLine - 1) << "位置读取错误" << endl);
		return false;
	}
	// 周活跃度连续{B6}周低于{B7}自动解散
	m_Config.nDismissActivity = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nDismissActivity < 0)
	{
		ErrorLn(szFileName << (nLine - 1) << "位置读取错误" << endl);
		return false;
	}
	// 每天能踢出的战队数量
	m_Config.nKickNum = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nKickNum < 0)
	{
		ErrorLn(szFileName << (nLine - 1) << "位置读取错误" << endl);
		return false;
	}
	// 盟主连续{B9}天不上线自动进入卸任状态
	m_Config.nOfflineDays = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nOfflineDays < 0)
	{
		ErrorLn(szFileName << (nLine - 1) << "位置读取错误" << endl);
		return false;
	}
	// 进入卸任状态{nOutgoingDays}天后自动丢失盟主职位
	m_Config.nOutgoingDays = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nOutgoingDays < 0)
	{
		ErrorLn(szFileName << (nLine - 1) << "位置读取错误" << endl);
		return false;
	}
    nLine++;
    m_Config.nClanCupMiniWeekActivity = pCSVReader->GetInt(nLine++, 1, 0);
    if (m_Config.nClanCupMiniWeekActivity < 0)
    {
        ErrorLn(szFileName << (nLine - 1) << "位置读取错误" << endl);
        return false;
    }
	return true;
}

// 加载帮会卡牌脚本
bool CSchemeClanDataCenter::LoadCardScheme(ICSVReader * pCSVReader,const char* szFileName)
{
	if(pCSVReader == NULL || szFileName == NULL)
	{
		return false;
	}

	m_ClanCardMap.clear();

	SClanCardSchemeInfo card;
	int nRecordCount = (int)pCSVReader->GetRecordCount();
	for (int i = 0; i < nRecordCount; i ++)
	{
		memset(&card, 0, sizeof(card));

		int nRow = 0;

		// 英雄等级
		card.nLevel = pCSVReader->GetInt(i, nRow++, 0);

		// 每日可请求次数
		card.nRequestDayCountLimit = pCSVReader->GetInt(i, nRow++, 0);

		// 请求冷却时间
		card.nRequestCooldownTime = pCSVReader->GetInt(i, nRow++, 0);

		// 单次请求获得最大数量
		card.nRequestSingleNumLimit = pCSVReader->GetInt(i, nRow++, 0);

		// 每日请求获得最大数量
		card.nRequestDayNumLimit = pCSVReader->GetInt(i, nRow++, 0);

		// 单次个人捐赠最大数量
		card.nDonateSingleNumLimit = pCSVReader->GetInt(i, nRow++, 0);

		// 捐赠日最大数量
		card.nDonateDayNumLimit = pCSVReader->GetInt(i, nRow++, 0);

		// 捐赠获得经验
		card.nDonatePrizeExp = pCSVReader->GetInt(i, nRow++, 0);

		// 捐赠获得金币
		card.nDonatePrizeGold = pCSVReader->GetInt(i, nRow++, 0);

		// 捐赠请求持续时间
		card.nRequestItemLastTime = pCSVReader->GetInt(i, nRow++, 0);

		CLAN_CARD_MAP::iterator Iter = m_ClanCardMap.find(card.nLevel);
		if (Iter != m_ClanCardMap.end())
		{
			ErrorLn("nRecordCount="<<nRecordCount<<" repeated,nLevel="<<card.nLevel);
			return false;
		}

		m_ClanCardMap.insert(CLAN_CARD_MAP::value_type(card.nLevel, card));
	}

	return true;
}