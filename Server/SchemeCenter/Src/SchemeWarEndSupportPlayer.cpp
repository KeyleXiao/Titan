#include "stdafx.h"
#include "SchemeWarEndSupportPlayer.h"
#include "IServerGlobal.h"
#include <string>


bool CSchemeWarEndSupportPlayer::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if (pSchemeEngine == NULL)
	{
		return false;
	}

	string stringPath = SCP_PATH_FORMAT(WARENDSUPPORTVOTE_SCHEME_FILENAME);
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
	if (!bResult)
	{
		ErrorLn("config file load failed filename = " << stringPath.c_str());;
		return false;
	}

	return true;
}

void CSchemeWarEndSupportPlayer::Close(void)
{	
    m_SchemeSupportVoteInfo.clear();
}

bool CSchemeWarEndSupportPlayer::OnSchemeLoad(SCRIPT_READER reader, const char *szFileName)
{
	ICSVReader *pCSVReader = reader.pCSVReader;
	if (pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
	{
		return false;
	}

	bool bRet = false;

	if (  rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(WARENDSUPPORTVOTE_SCHEME_FILENAME) ) == 0)
	{
		bRet = LoadWarEndSupportPlayerScheme(pCSVReader,szFileName);
	}

	if (!bRet)
	{
		return false;
	}	

	AfterSchemeLoaded();

    return true;
}

bool CSchemeWarEndSupportPlayer::OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName)
{
    return false;
}

bool CSchemeWarEndSupportPlayer::LoadWarEndSupportPlayerScheme( ICSVReader * pCSVReader, const char *szFileName )
{
	// 清空
	m_SchemeSupportVoteInfo.clear();

	// 读取
	int nLen = 0;
	for (size_t nRow = 0; nRow < pCSVReader->GetRecordCount(); ++nRow)
	{
		int nColumnIndex = 0;
		SWarEndSupportVoteConfig_S sVoteInfo;
		sVoteInfo.nVoteID = pCSVReader->GetInt(nRow, nColumnIndex++, 0); 
		nColumnIndex++;
		nColumnIndex++;

		// 标签列表
		char strLabelList[128] = { 0 };
		int nLen = sizeof(strLabelList);
		pCSVReader->GetString(nRow, nColumnIndex++, strLabelList, nLen);
		parseIntArray(strLabelList, sVoteInfo.nLabelIDList, MAX_SUPPORTVOTE_LABEL_NUM, ';');

		sVoteInfo.nParam1 = pCSVReader->GetInt(nRow, nColumnIndex++, 0); 

		m_SchemeSupportVoteInfo.insert(make_pair<int, SWarEndSupportVoteConfig_S>(sVoteInfo.nVoteID, sVoteInfo));
	}

	return true;
}

 bool CSchemeWarEndSupportPlayer::getEndSupportPlayerVoteInfoList(SWarEndSupportVoteConfig_S supportVoteList[]) 
 {
	 int nListIndex = 0;
	 MapSupportVoteInfo::iterator iterBegin = m_SchemeSupportVoteInfo.begin();
	 for (;iterBegin!=m_SchemeSupportVoteInfo.end();++iterBegin)
	 {
		 if(nListIndex>=(int)ESVT_MAX)
			 return true;

		 supportVoteList[nListIndex++] = iterBegin->second;
	 }

	 return true;
 }

 bool CSchemeWarEndSupportPlayer::isHasLabelID(int nVoteID, int nLabelID)
 {
	 MapSupportVoteInfo::iterator iter = m_SchemeSupportVoteInfo.find(nVoteID);
	 int* pLabelList = iter->second.nLabelIDList;
	 for (int i = 0; i < MAX_SUPPORTVOTE_LABEL_NUM; ++i)
	 {
		 if (pLabelList[i] <= 0)
			 continue;

		 if (pLabelList[i] == nLabelID)
		 {
			 return true;
		 }
	 }

	 return false;
 }

 int CSchemeWarEndSupportPlayer::getRandomLabel(int nVoteID)
 {
	 vector<int> labelList;

	 MapSupportVoteInfo::iterator iter = m_SchemeSupportVoteInfo.find(nVoteID);
	 int* pLabelList = iter->second.nLabelIDList;
	 for (int i = 0; i < MAX_SUPPORTVOTE_LABEL_NUM; ++i)
	 {
		 if (pLabelList[i] > 0)
			 labelList.push_back(pLabelList[i]);
	 }

	 if (labelList.empty())
	 {
		 return 0;
	 }

	 int nRandNum = mt_range_rand(0, labelList.size() - 1);

	 return labelList[nRandNum];
 }
