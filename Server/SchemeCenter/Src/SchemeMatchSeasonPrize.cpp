#include "stdafx.h"
#include "SchemeMatchSeasonPrize.h"
#include "IServerGlobal.h"

CSchemeMatchSeasonPrize::CSchemeMatchSeasonPrize()
{
	m_mapSeasonPrize.clear();
}

CSchemeMatchSeasonPrize::~CSchemeMatchSeasonPrize()
{

}

bool CSchemeMatchSeasonPrize::LoadScheme()
{
	ISchemeEngine * pSchemeEngine = gServerGlobal->getSchemeEngine();
	if (pSchemeEngine == NULL)
	{
		return false;
	}

	string strPath = SCP_PATH_FORMAT(MATCH_SEASON_PRIZE_SCHEME_FILENAME);
	bool bResult = pSchemeEngine->LoadScheme(strPath.c_str(), (ISchemeUpdateSink *)this);
	if (bResult == false)
	{
		Error("config file load failed! filename = " << strPath.c_str() << endl);
		return false;
	}
	return true;
}

void CSchemeMatchSeasonPrize::Close()
{
	m_mapSeasonPrize.clear();
}

bool CSchemeMatchSeasonPrize::OnSchemeLoad(SCRIPT_READER reader, const char * szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if (pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
	{
		return false;
	}

	m_mapSeasonPrize.clear();

	int nLen = 0;
	char szTemp[GAME_NAME_MAXSIZE] = {0};

	int nRecordCount = pCSVReader->GetRecordCount();
	for	(int nRow = 0; nRow	< nRecordCount; ++nRow)
	{
		int nCol = 0;
		SSchemeMatchSeasonPrize sNode;

		sNode.nPrizeConfigID = pCSVReader->GetInt(nRow, nCol++, 0);

		nLen = sizeof(sNode.szSeasonDesc);
		pCSVReader->GetString(nRow, nCol++, sNode.szSeasonDesc, nLen);

		sNode.nRankGrade = pCSVReader->GetInt(nRow, nCol++, 0);

		nLen = sizeof(sNode.szRankDesc);
		pCSVReader->GetString(nRow, nCol++, sNode.szRankDesc, nLen);

		nLen = sizeof(szTemp);
		memset(szTemp, 0, nLen);
		pCSVReader->GetString(nRow, nCol++, szTemp, nLen);
		parseIntArray(szTemp, sNode.arrNormalPrize, MATCH_SEASON_PRIZE_COUNT, ';');

		 nLen = sizeof(szTemp);
		 memset(szTemp, 0, nLen);
		 pCSVReader->GetString(nRow, nCol++, szTemp, nLen);
		 parseIntArray(szTemp, sNode.arrExtraCondition, MATCH_SEASON_PRIZE_COUNT, ';');

		 nLen = sizeof(szTemp);
		 memset(szTemp, 0, nLen);
		 pCSVReader->GetString(nRow, nCol++, szTemp, nLen);
		 parseIntArray(szTemp, sNode.arrExtraPrize, MATCH_SEASON_PRIZE_COUNT, ';');

		 sNode.nAccountScore = pCSVReader->GetInt(nRow, nCol++, 0);

		 //if (m_mapSeasonPrize.find(sNode.nPrizeID) == m_mapSeasonPrize.end())
		 //{
			// TMap_SchemeRankPrize mapRankPrize;
			// m_mapSeasonPrize.insert(make_pair(sNode.nPrizeID, mapRankPrize));
		 //}
		 m_mapSeasonPrize[sNode.nPrizeConfigID].insert(make_pair(sNode.nRankGrade, sNode));
	}

	AfterSchemeLoaded();
	return true;
}

bool CSchemeMatchSeasonPrize::OnSchemeUpdate(SCRIPT_READER reader, const char * szFileName)
{
	CSchemeMatchSeasonPrize * pNewInstance = NewInstance();
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

SSchemeMatchSeasonPrize* CSchemeMatchSeasonPrize::getSchemeMatchSeasonPrize(int nPrizeID, int nRankGrade)
{
	TMap_SchemeSeasonPrize::iterator itSeason = m_mapSeasonPrize.find(nPrizeID);
	if (itSeason == m_mapSeasonPrize.end())
	{
		return NULL;
	}

	TMap_SchemeRankPrize::iterator itRank = itSeason->second.find(nRankGrade);
	if (itRank == itSeason->second.end())
	{
		return NULL;
	}

	return &(itRank->second);
}
