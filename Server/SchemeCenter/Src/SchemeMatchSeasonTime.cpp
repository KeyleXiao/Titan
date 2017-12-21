#include "stdafx.h"
#include "SchemeMatchSeasonTime.h"
#include "IServerGlobal.h"
#include <time.h>

CSchemeMatchSeasonTime::CSchemeMatchSeasonTime()
{
	m_vecSeasonTime.clear();
}

CSchemeMatchSeasonTime::~CSchemeMatchSeasonTime()
{
	
}

bool CSchemeMatchSeasonTime::LoadScheme()
{
	ISchemeEngine * pSchemeEngine = gServerGlobal->getSchemeEngine();
	if (pSchemeEngine == NULL)
	{
		return false;
	}

	string strPath = SCP_PATH_FORMAT(MATCH_SEASON_TIME_SCHEME_FILENAME);
	bool bResult = pSchemeEngine->LoadScheme(strPath.c_str(), (ISchemeUpdateSink *)this);
	if(bResult == false)
	{
		Error("config file load failed! filename = " << strPath.c_str() << endl);
		return false;
	}
	return true;
}

void CSchemeMatchSeasonTime::Close()
{
	m_vecSeasonTime.clear();
}

bool CSchemeMatchSeasonTime::OnSchemeLoad(SCRIPT_READER reader, const char * szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
	{
		return false;
	}

	m_vecSeasonTime.clear();

	int nLen = 0;
	char szBeginTime[GAME_NAME_MAXSIZE] = {0};
	char szEndTime[GAME_NAME_MAXSIZE] = {0};

	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		int nCol = 0;
		SSchemeMatchSeasonTime sNode;

		sNode.nIndex = pCSVReader->GetInt(nRow, nCol++, 0);

		nLen = sizeof(sNode.szName);
		pCSVReader->GetString(nRow, nCol++, sNode.szName, nLen);

		nLen = sizeof(szBeginTime);
		memset(szBeginTime, 0, nLen);
		pCSVReader->GetString(nRow, nCol++, szBeginTime, nLen);
		sNode.tBeginTime = MakeTime(szBeginTime);

		nLen = sizeof(szEndTime);
		memset(szEndTime, 0, nLen);
		pCSVReader->GetString(nRow, nCol++, szEndTime, nLen);
		sNode.tEndTime = MakeTime(szEndTime);
		
		sNode.nPrizeConfigID = pCSVReader->GetInt(nRow, nCol++, 0);

		m_vecSeasonTime.push_back(sNode);
	}

	AfterSchemeLoaded();
	return true;
}

bool CSchemeMatchSeasonTime::OnSchemeUpdate(SCRIPT_READER reader, const char * szFileName)
{
	CSchemeMatchSeasonTime * pNewInstance = NewInstance();
	if (pNewInstance == NULL)
	{
		return false;
	}

	if (pNewInstance->OnSchemeLoad(reader, szFileName))
	{
		return true;
	}
	else
	{
		safeDelete(pNewInstance);
		return false;
	}
}

SSchemeMatchSeasonTime*	CSchemeMatchSeasonTime::getSchemeMatchSeasonTimeByIndex(int nIndex)
{
	for (vector<SSchemeMatchSeasonTime>::iterator it = m_vecSeasonTime.begin(); it != m_vecSeasonTime.end(); ++it)
	{
		if (nIndex == it->nIndex)
		{
			return &(*it);
		}
	}

	return NULL;
}

SSchemeMatchSeasonTime*	CSchemeMatchSeasonTime::getSchemeMatchSeasonTimeByTime(time_t tTime)
{
	for (vector<SSchemeMatchSeasonTime>::iterator it = m_vecSeasonTime.begin(); it != m_vecSeasonTime.end(); ++it)
	{
		if (tTime >= it->tBeginTime && tTime < it->tEndTime)
		{
			return &(*it);
		}
	}
	
	return NULL;
}

SSchemeMatchSeasonTime* CSchemeMatchSeasonTime::getSchemeMatchSeasonTimeByLastBeginTime()
{
	time_t tNow = time(NULL);

	SSchemeMatchSeasonTime* sRet = NULL;
	for (vector<SSchemeMatchSeasonTime>::iterator it = m_vecSeasonTime.begin(); it != m_vecSeasonTime.end(); ++it)
	{
		if (tNow >= it->tBeginTime)
		{
			sRet = &(*it);
		}
		else
		{
			break;
		}
	}

	return sRet;
}

SSchemeMatchSeasonTime* CSchemeMatchSeasonTime::getSchemeMatchSeasonTimeByLastEndTime()
{
	time_t tNow = time(NULL);

	SSchemeMatchSeasonTime* sRet = NULL;
	for (vector<SSchemeMatchSeasonTime>::iterator it = m_vecSeasonTime.begin(); it != m_vecSeasonTime.end(); ++it)
	{
		if (tNow >= it->tEndTime)
		{
			sRet = &(*it);
		}
		else
		{
			break;
		}
	}

	return sRet;
}

SSchemeMatchSeasonTime* CSchemeMatchSeasonTime::getSchemeMatchSeasonTimeByNextBeginTime()
{
	time_t tNow = time(NULL);

	for (vector<SSchemeMatchSeasonTime>::iterator it = m_vecSeasonTime.begin(); it != m_vecSeasonTime.end(); ++it)
	{
		if (tNow >= it->tBeginTime)
			continue;

		return &(*it);
	}

	return NULL;
}

SSchemeMatchSeasonTime* CSchemeMatchSeasonTime::getSchemeMatchSeasonTimeByNextEndTime()
{
	time_t tNow = time(NULL);

	for (vector<SSchemeMatchSeasonTime>::iterator it = m_vecSeasonTime.begin(); it != m_vecSeasonTime.end(); ++it)
	{
		if (tNow >= it->tEndTime)
			continue;

		return &(*it);
	}

	return NULL;
}

time_t CSchemeMatchSeasonTime::MakeTime(char * ch)
{
	tm t;
	sscanf(ch, "%d/%d/%d %d:%d:%d", &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
	t.tm_year -= 1900;
	t.tm_mon -= 1;
	t.tm_sec = 0;

	return mktime(&t);
}

