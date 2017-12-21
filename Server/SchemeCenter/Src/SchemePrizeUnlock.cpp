#include "stdafx.h"
#include "SchemePrizeUnlock.h"
#include "IServerGlobal.h"
#include <string>

CSchemePrizeUnlock::CSchemePrizeUnlock(void)
{
    m_mapHeroLevelUnlockInfo.clear();
	m_vecActorUnlockInfo.actorPrizeUnlockVec.clear();
	m_mapHeroStarUnlockInfo.clear();
}

CSchemePrizeUnlock::~CSchemePrizeUnlock(void)
{
	m_mapHeroLevelUnlockInfo.clear();
	m_vecActorUnlockInfo.actorPrizeUnlockVec.clear();
	m_mapHeroStarUnlockInfo.clear();
}

bool CSchemePrizeUnlock::LoadScheme(void)
{
    ISchemeEngine * pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL )
    {
        return false;
    }

    string stringPath = SCP_PATH_FORMAT( PRIZEUNLOCK_CONFIG_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this, false);
    if(!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

void CSchemePrizeUnlock::Close(void)
{	
	m_mapHeroLevelUnlockInfo.clear();
	m_vecActorUnlockInfo.actorPrizeUnlockVec.clear();
	m_mapHeroStarUnlockInfo.clear();
}

SHeroLevelPrizeUnlockMap* CSchemePrizeUnlock::getHeroLevelPrizeUnlockInfo()
{
    if(m_mapHeroLevelUnlockInfo.size() <= 0)
    {
        return NULL;
    }

    return &m_mapHeroLevelUnlockInfo;
}

SActorPrizeUnlockVec* CSchemePrizeUnlock::getActorPrizeUnlockInfo()
{
    if(m_vecActorUnlockInfo.actorPrizeUnlockVec.size() <= 0)
    {
        return NULL;
    }

    return &m_vecActorUnlockInfo;
}

SHeroStarPrizeUnlockMap* CSchemePrizeUnlock::getHeroStarPrizeUnlockInfo()
{
	if(m_mapHeroStarUnlockInfo.size() <= 0)
	{
		return NULL;
	}

	return &m_mapHeroStarUnlockInfo;
}

bool CSchemePrizeUnlock::OnSchemeLoad(SCRIPT_READER reader, LPCSTR szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    ISchemeActorPrize* pSchemeActorPrize = gServerGlobal->getSchemeCenter()->getSchemeActorPrize();

    // 清空
	m_mapHeroLevelUnlockInfo.clear();
	m_vecActorUnlockInfo.actorPrizeUnlockVec.clear();
	m_mapHeroStarUnlockInfo.clear();

    // 读取
    for (size_t nRow = 0; nRow < pCSVReader->GetRecordCount(); ++nRow)
    {
        int nColumnIndex = 0;

        nColumnIndex++;

        nColumnIndex++;

        int nPrizeUnlockType = pCSVReader->GetInt(nRow, nColumnIndex, 0); nColumnIndex++;

        int nHeroID = pCSVReader->GetInt(nRow, nColumnIndex, 0); nColumnIndex++;

        int nUnlockLevel = pCSVReader->GetInt(nRow, nColumnIndex, 0); nColumnIndex++;

		int nPrizeIDList[PRIZEUNLOCK_PRIZEID_MAXCOUNT] = {0}; 
		char szBuf[256] = {0};
		int nLen = sizeof(szBuf);
		pCSVReader->GetString(nRow, nColumnIndex, szBuf, nLen); nColumnIndex++;

		int nArrayCount = parseIntArray(szBuf, nPrizeIDList, PRIZEUNLOCK_PRIZEID_MAXCOUNT, ';');

		int nPrizeIconId = pCSVReader->GetInt(nRow, nColumnIndex, 0); nColumnIndex++;

		int nMailId = pCSVReader->GetInt(nRow, nColumnIndex, 0); nColumnIndex++;

        ////////////////////////////////////////////////////////////////
        // 校验奖励ID是否合法
        for (int i = 0; i < nArrayCount; ++i)
        {
            int& nCurrentPrizeID = nPrizeIDList[i];
            // 校验奖励ID是否合法
            if (nCurrentPrizeID != 0 && !pSchemeActorPrize->isExists(nCurrentPrizeID))
            {
                ErrorLn(__FUNCTION__" prize id is invalid ! prizeID=" << nCurrentPrizeID << ",check file=" << szFileName << ",row=" << (nRow + 1) );
                return false;
            }
        }


        ////////////////////////////////////////////////////////////////
		if (nPrizeUnlockType == (int)PrizeUnlockType_Actor)
		{
			SActorPrizeUnlockItem item;
			item.nUnlockLevel = nUnlockLevel;
			memcpy(item.nPrizeId, nPrizeIDList, sizeof(item.nPrizeId));
			item.nMailId = nMailId;
			m_vecActorUnlockInfo.actorPrizeUnlockVec.push_back(item);
		}
		else if (nPrizeUnlockType == (int)PrizeUnlockType_HeroLevel)
		{
			SHeroLevelPrizeUnlockItem item;
			item.nUnlockLevel = nUnlockLevel;
			memcpy(item.nPrizeId, nPrizeIDList, sizeof(item.nPrizeId));
			item.nMailId = nMailId;
			if (m_mapHeroLevelUnlockInfo.find(nHeroID) != m_mapHeroLevelUnlockInfo.end())
			{
				m_mapHeroLevelUnlockInfo[nHeroID].heroLevelPrizeUnlockVec.push_back(item);
			}
			else
			{
				SHeroLevelPrizeUnlockVec vecHeroLevelPrizeUnlockInfo;
				vecHeroLevelPrizeUnlockInfo.heroLevelPrizeUnlockVec.push_back(item);
				m_mapHeroLevelUnlockInfo.insert(make_pair<int,SHeroLevelPrizeUnlockVec>(nHeroID, vecHeroLevelPrizeUnlockInfo));
			}
		}
		else if (nPrizeUnlockType == (int)PrizeUnlockType_HeroStar)
		{
			SHeroStarPrizeUnlockItem item;
			item.nUnlockLevel = nUnlockLevel;
			item.nMailId = nMailId;
			memcpy(item.nPrizeId, nPrizeIDList, sizeof(item.nPrizeId));

			if (m_mapHeroStarUnlockInfo.find(nHeroID) != m_mapHeroStarUnlockInfo.end())
			{
				m_mapHeroStarUnlockInfo[nHeroID].heroStarPrizeUnlockVec.push_back(item);
			}
			else
			{
				SHeroStarPrizeUnlockVec vecHeroStarPrizeUnlockInfo;
				vecHeroStarPrizeUnlockInfo.heroStarPrizeUnlockVec.push_back(item);
				m_mapHeroStarUnlockInfo.insert(make_pair<int,SHeroStarPrizeUnlockVec>(nHeroID, vecHeroStarPrizeUnlockInfo));
			}
		}
  
    }

    AfterSchemeLoaded();
    return true;
}

bool CSchemePrizeUnlock::OnSchemeUpdate(SCRIPT_READER reader, LPCSTR szFileName)
{
    return false;
}