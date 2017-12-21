/*******************************************************************
** �ļ���:	SchemeHeroFreeWeekly.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2016/8/23
** ��  ��:	1.0
** ��  ��:	����HeroFreeWeekly.csv

			
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeHeroFreeWeekly.h"
#include "IClientGlobal.h"
#include <string>
#include <time.h>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeHeroFreeWeekly::CSchemeHeroFreeWeekly(void)
{
	vecHeroFree.clear();
	memset(arrayFreeHeroID, 0, sizeof(arrayFreeHeroID));
}

/** 
@param   
@param   
@return  
*/
CSchemeHeroFreeWeekly::~CSchemeHeroFreeWeekly(void)
{
	vecHeroFree.clear();
	memset(arrayFreeHeroID, 0, sizeof(arrayFreeHeroID));
}

/** ����ű�
@param   
@param   
@return  
*/
bool CSchemeHeroFreeWeekly::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// ����HeroFreeWeekly.csv�ű�
	string stringPath = SCP_PATH_FORMAT( HEROFREEWEEKLY_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("config file load failed! filename=" << stringPath.c_str() << endl);
		return false;
	}

	return true;	
}

/** �ر�
@param   
@param   
@return  
*/
void CSchemeHeroFreeWeekly::Close(void)
{	
	// ���
	vecHeroFree.clear();
	memset(arrayFreeHeroID, 0, sizeof(arrayFreeHeroID));
}

/** CSV��������ʱ֪ͨ
@param   pCSVReader����ȡCSV�ķ��ؽӿ�
@param   szFileName�������ļ���
@param   
@return  
@note     
@warning ��Ҫ�ڴ˷����ͷ�pCSVReader����pTiXmlDocument,��Ϊһ���ļ���Ӧ���sink
@retval buffer 
*/ 
bool CSchemeHeroFreeWeekly::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// ���
	vecHeroFree.clear();

	// ��ʱ����
	int nLen = 0;
	const int HEROFREEWEEKLY_TIME =	32;
	char szBeginTime[HEROFREEWEEKLY_TIME] = {0};
	char szEndTime[HEROFREEWEEKLY_TIME] = {0};
	char szArrayHeroIDLow[HEROFREEWEEKLY_FREEID];
	char szArrayHeroIDHigh[HEROFREEWEEKLY_FREEID];

	// ��ȡ
	int nRecordCount = pCSVReader->GetRecordCount();
	for ( int nRow = 0; nRow < nRecordCount; ++nRow )
	{
		SHeroFreeWeeklyScheme sItem;
		int nColIndex = 0;

		// ���⿪ʼʱ��
		memset(szBeginTime, 0, sizeof(szBeginTime));
		nLen = sizeof(szBeginTime) / sizeof(char);
		pCSVReader->GetString(nRow, nColIndex++, szBeginTime, nLen);
		sItem.tBeginTime = MakeTime(szBeginTime);

		// �������ʱ��
		memset(szEndTime, 0, sizeof(szEndTime));
		nLen = sizeof(szEndTime) / sizeof(char);
		pCSVReader->GetString(nRow, nColIndex++, szEndTime, nLen);
		sItem.tEndTime = MakeTime(szEndTime);

		// ����͵ȼ�Ӣ���б�
		memset(szArrayHeroIDLow, 0, sizeof(szArrayHeroIDLow));
		nLen = sizeof(szArrayHeroIDLow) / sizeof(char);
		pCSVReader->GetString(nRow, nColIndex++, szArrayHeroIDLow, nLen);
		parseIntArray(szArrayHeroIDLow, sItem.arrayHeroIDLow, sizeof(sItem.arrayHeroIDLow)/sizeof(char));

		// �����Ӣ�۵ȼ�
		sItem.nPlayerLvLow = pCSVReader->GetInt(nRow, nColIndex++, 0);

		// ����ߵȼ�Ӣ���б�
		memset(szArrayHeroIDHigh, 0, sizeof(szArrayHeroIDHigh));
		nLen = sizeof(szArrayHeroIDHigh) / sizeof(char);
		pCSVReader->GetString(nRow, nColIndex++, szArrayHeroIDHigh, nLen);
		parseIntArray(szArrayHeroIDHigh, sItem.arrayHeroIDHigh, sizeof(sItem.arrayHeroIDHigh)/sizeof(char));

		// �����Ӣ�۵ȼ�
		sItem.nPlayerLvHigh = pCSVReader->GetInt(nRow, nColIndex++, 0);

		vecHeroFree.push_back(sItem);
	}

	return true;
}

/** ���ö�̬����ʱ֪ͨ
@param   pCSVReader����ȡCSV�ķ��ؽӿ�
@param   szFileName�������ļ���
@param   
@return  
@note     
@warning ��Ҫ�ڴ˷����ͷ�pCSVReader����pTiXmlDocument,��Ϊһ���ļ���Ӧ���sink
@retval buffer 
*/
bool CSchemeHeroFreeWeekly::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}

/** ��ȡ��ǰ����Ӣ���б�
@param   
@param   
@return  
*/
int	CSchemeHeroFreeWeekly::GetFreeHeroListByLv( int arrayFreeHero[], int arrayLen, int nLv )
{
	time_t now = time(0);
	SHeroFreeWeeklyVec::iterator iter = vecHeroFree.begin();
	for ( ; iter != vecHeroFree.end(); iter++)
	{
		if (now	>= iter->tBeginTime && now < iter->tEndTime)
		{
			// ���ص͵ȼ�Ӣ���б�
			int nLowLen = 0;
			for (int i = 0; i < HEROFREEWEEKLY_FREEID; i++)
			{
				if (iter->arrayHeroIDLow[i] == 0)
				{
					break;
				}
				nLowLen++;
			}
			std::sort(iter->arrayHeroIDLow, iter->arrayHeroIDLow + nLowLen);

			if (nLv >= iter->nPlayerLvLow && nLv < iter->nPlayerLvHigh)
			{
				for (int i = 0; i < nLowLen; i++)
				{
					arrayFreeHero[i] = iter->arrayHeroIDLow[i];
					arrayFreeHeroID[i] = iter->arrayHeroIDLow[i];
				}
				return nLowLen;
			}

			// ���ظߵȼ�Ӣ���б�
			int nHighLen = 0;
			for (int i = 0; i < HEROFREEWEEKLY_FREEID; i++)
			{
				if (iter->arrayHeroIDHigh[i] == 0)
				{
					break;
				}
				nHighLen++;
			}
			std::sort(iter->arrayHeroIDHigh, iter->arrayHeroIDHigh + nHighLen);

			int arrayMerge[nFreeHeroArrayLen] = {0};
			int * arrayMergeEnd = std::set_union(iter->arrayHeroIDLow, iter->arrayHeroIDLow + nLowLen, iter->arrayHeroIDHigh, iter->arrayHeroIDHigh + nHighLen, arrayMerge);
			int arrayMergeCount = arrayMergeEnd - arrayMerge;

			// ���߻������Ƿ����ظ�
			if (arrayMergeCount < nLowLen + nHighLen)
			{
				Error("Hero Free Weekly Scheme is wrong, lowLv and highLv has the same ID!" << endl);
				return 0;
			}

			// ��鳤���Ƿ񳬳���Χ
			if (arrayMergeCount > nFreeHeroArrayLen || arrayMergeCount > arrayLen)
			{
				Error("Hero Free Weekly Scheme is wrong, arrayMergeCount is too long!" << endl);
				return 0;
			}

			if (nLv >= iter->nPlayerLvHigh)
			{
				for (int i = 0; i < arrayMergeCount; i++)
				{
					arrayFreeHero[i] = arrayMerge[i];
					arrayFreeHeroID[i] = arrayMerge[i];
				}
				return arrayMergeCount;
			}
		}
	}
	return 0;
}

/** �Ƿ�Ϊ����Ӣ��
@param   
@param   
@return  
*/
bool CSchemeHeroFreeWeekly::CheckIsWeekFree( const int nHeroID, const int nLv )
{
	int freeHeroList[nFreeHeroArrayLen] = {0};
	bool bIsFree = false;

	int nCount = GetFreeHeroListByLv(freeHeroList, nFreeHeroArrayLen, nLv);
	for (int i = 0; i < nCount; i++)
	{
		if (nHeroID == freeHeroList[i])
		{
			bIsFree = true;
			break;
		}
	}

	return bIsFree;
}

/** 
@param   
@param   
@return  
*/
time_t CSchemeHeroFreeWeekly::MakeTime(char * ch)
{
	tm t;
	sscanf(ch, "%d/%d/%d %d:%d:%d", &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
	t.tm_year -= 1900;
	t.tm_mon -= 1;
	t.tm_sec = 0;
	return mktime(&t);
}