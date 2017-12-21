#include "stdafx.h"
#include "SchemeHeroTalentConfig.h"
#include "IServerGlobal.h"
#include <string>

CSchemeHeroTalentConfig::CSchemeHeroTalentConfig(void)
{
    m_SchemeInfo.clear();
}

CSchemeHeroTalentConfig::~CSchemeHeroTalentConfig(void)
{
    m_SchemeInfo.clear();
}


bool CSchemeHeroTalentConfig::LoadScheme(void)
{
    ISchemeEngine * pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL )
    {
        return false;
    }

    string stringPath = SCP_PATH_FORMAT( HEROTALENT_CONFIG_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this, false);
    if(!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

void CSchemeHeroTalentConfig::Close(void)
{	
    m_SchemeInfo.clear();
}

// CSchemeHeroTalentConfig
std::map<int, std::vector<Single_Hero_Talent_InfoEx> >*  CSchemeHeroTalentConfig::GetHeroTalentInfo()
{
   return &m_SchemeInfo;
}

//////////////////////////////////////////////////////////////////////////
bool CSchemeHeroTalentConfig::OnSchemeLoad(SCRIPT_READER reader, LPCSTR szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    // 清空
    m_SchemeInfo.clear();

    const int STR_ARRAY_SIZE = 256;
    char szArray[STR_ARRAY_SIZE] = {0};
    int nLen = sizeof(szArray) / sizeof(char);

    std::vector<std::string> vecDataList;

    // 读取
    for (size_t nRow = 0; nRow < pCSVReader->GetRecordCount(); ++nRow)
    {
        Single_Hero_Talent_InfoEx talent;

		int nHeroId       = pCSVReader->GetInt(nRow, TC_HERO_ID, 0);      // 英雄ID
        talent.dwTalentID = pCSVReader->GetInt(nRow, TC_TALENT_ID, 0);    // 天赋ID
		talent.nActiveType   = pCSVReader->GetInt(nRow, TC_ACTIVE_TYPE, 0);  // 激活类型（0默认自动;1系统）
		talent.nStarActiveLevel  = pCSVReader->GetInt(nRow, TC_ACTIVE_LEVEL, 0);  // 激活等级
		talent.nLearnLevel = pCSVReader->GetInt(nRow, TC_LEARN_LEVEL, 0);	// 学习等级

		//if(talent.nActiveType == (int)TalentActiveType_DefualtActive)
		//{
		//	talent.bActive = true;
		//}
		//else if(talent.nActiveType ==(int)TalentActiveType_SystemActive)
		//{
		//	talent.bActive = false;
		//}

        //memset(szArray, 0, sizeof(szArray));
        //nLen = sizeof(szArray) / sizeof(char);
        //pCSVReader->GetString(nRow, TC_NAME, szArray, nLen); // 天赋名称
        //memset(szArray, 0, sizeof(szArray));
        //nLen = sizeof(szArray) / sizeof(char);
        //pCSVReader->GetString(nRow, TC_DESC, szArray, nLen);// 天赋描述
        //memset(szArray, 0, sizeof(szArray));
        //nLen = sizeof(szArray) / sizeof(char);
        //pCSVReader->GetString(nRow, TC_UNACTIVE_DESC, szArray, nLen); // 未激活描述
        //memset(szArray, 0, sizeof(szArray));
        //nLen = sizeof(szArray) / sizeof(char);
        //pCSVReader->GetString(nRow, TC_INNER_DESC, szArray, nLen); // 场内天赋描述
        //memset(szArray, 0, sizeof(szArray));
        //nLen = sizeof(szArray) / sizeof(char);
        //pCSVReader->GetString(nRow, TC_PRE_TALENTID, szArray, nLen); // 前置天赋

        // 比赛类型 EMMatchType
        memset(szArray, 0, sizeof(szArray));
        nLen = sizeof(szArray) / sizeof(char);
        pCSVReader->GetString(nRow, TC_WAR_TYPE, szArray, nLen);
        vecDataList.clear();
        StringHelper::split(vecDataList, szArray, ';');
        for(std::vector<std::string>::iterator it = vecDataList.begin(); it != vecDataList.end(); ++it)
        {
           talent.matchTypes.push_back(StringHelper::toInt(*it));
        }

        std::vector<Single_Hero_Talent_InfoEx>& talentList = m_SchemeInfo[nHeroId];
        talentList.push_back(talent);
	}

    AfterSchemeLoaded();
    return true;
}

bool CSchemeHeroTalentConfig::OnSchemeUpdate(SCRIPT_READER reader, LPCSTR szFileName)
{
    return false;
}

// 返回本英雄[0,nStarLv]星级的所有激活天赋列表
// @param nStarLv: 升星等级
vector<int> CSchemeHeroTalentConfig::getAllHeroStarActiveTalent(int nHeroID, int nStarLevel)
{
	vector<int> result;
	map<int, std::vector<Single_Hero_Talent_InfoEx> >::iterator iter = m_SchemeInfo.find(nHeroID);
	if (iter == m_SchemeInfo.end())
		return result;

	std::vector<Single_Hero_Talent_InfoEx>::iterator talentIter = iter->second.begin();
	for (; talentIter != iter->second.end(); ++talentIter)
	{
		if (talentIter->nActiveType == TalentActiveType_DefualtActive && talentIter->nStarActiveLevel <= nStarLevel)
		{
			result.push_back(talentIter->dwTalentID);
		}
	}

	return result;
}

Single_Hero_Talent_InfoEx * CSchemeHeroTalentConfig::getHeroTalent(int nHeroID, int nTalentID)
{
	map<int, std::vector<Single_Hero_Talent_InfoEx> >::iterator iter = m_SchemeInfo.find(nHeroID);
	if (iter == m_SchemeInfo.end())
		return NULL;

	std::vector<Single_Hero_Talent_InfoEx>::iterator iterTalent = iter->second.begin();
	for (; iterTalent != iter->second.end(); ++iterTalent)
	{
		if (iterTalent->dwTalentID == nTalentID)
			return &(*iterTalent);
	}

	return NULL;
}
