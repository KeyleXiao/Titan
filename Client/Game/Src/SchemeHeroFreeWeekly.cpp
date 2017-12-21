/*******************************************************************
** 文件名:	SchemeHeroFreeWeekly.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016/8/23
** 版  本:	1.0
** 描  述:	配置HeroFreeWeekly.csv

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
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

/** 载入脚本
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

	// 加载HeroFreeWeekly.csv脚本
	string stringPath = SCP_PATH_FORMAT( HEROFREEWEEKLY_SCHEME_FILENAME );
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
void CSchemeHeroFreeWeekly::Close(void)
{	
	// 清空
	vecHeroFree.clear();
	memset(arrayFreeHeroID, 0, sizeof(arrayFreeHeroID));
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
bool CSchemeHeroFreeWeekly::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	vecHeroFree.clear();

	// 临时数据
	int nLen = 0;
	const int HEROFREEWEEKLY_TIME =	32;
	char szBeginTime[HEROFREEWEEKLY_TIME] = {0};
	char szEndTime[HEROFREEWEEKLY_TIME] = {0};
	char szArrayHeroIDLow[HEROFREEWEEKLY_FREEID];
	char szArrayHeroIDHigh[HEROFREEWEEKLY_FREEID];

	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for ( int nRow = 0; nRow < nRecordCount; ++nRow )
	{
		SHeroFreeWeeklyScheme sItem;
		int nColIndex = 0;

		// 周免开始时间
		memset(szBeginTime, 0, sizeof(szBeginTime));
		nLen = sizeof(szBeginTime) / sizeof(char);
		pCSVReader->GetString(nRow, nColIndex++, szBeginTime, nLen);
		sItem.tBeginTime = MakeTime(szBeginTime);

		// 周免结束时间
		memset(szEndTime, 0, sizeof(szEndTime));
		nLen = sizeof(szEndTime) / sizeof(char);
		pCSVReader->GetString(nRow, nColIndex++, szEndTime, nLen);
		sItem.tEndTime = MakeTime(szEndTime);

		// 周免低等级英雄列表
		memset(szArrayHeroIDLow, 0, sizeof(szArrayHeroIDLow));
		nLen = sizeof(szArrayHeroIDLow) / sizeof(char);
		pCSVReader->GetString(nRow, nColIndex++, szArrayHeroIDLow, nLen);
		parseIntArray(szArrayHeroIDLow, sItem.arrayHeroIDLow, sizeof(sItem.arrayHeroIDLow)/sizeof(char));

		// 周免低英雄等级
		sItem.nPlayerLvLow = pCSVReader->GetInt(nRow, nColIndex++, 0);

		// 周免高等级英雄列表
		memset(szArrayHeroIDHigh, 0, sizeof(szArrayHeroIDHigh));
		nLen = sizeof(szArrayHeroIDHigh) / sizeof(char);
		pCSVReader->GetString(nRow, nColIndex++, szArrayHeroIDHigh, nLen);
		parseIntArray(szArrayHeroIDHigh, sItem.arrayHeroIDHigh, sizeof(sItem.arrayHeroIDHigh)/sizeof(char));

		// 周免高英雄等级
		sItem.nPlayerLvHigh = pCSVReader->GetInt(nRow, nColIndex++, 0);

		vecHeroFree.push_back(sItem);
	}

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
bool CSchemeHeroFreeWeekly::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}

/** 获取当前周免英雄列表
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
			// 返回低等级英雄列表
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

			// 返回高等级英雄列表
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

			// 检查策划配置是否有重复
			if (arrayMergeCount < nLowLen + nHighLen)
			{
				Error("Hero Free Weekly Scheme is wrong, lowLv and highLv has the same ID!" << endl);
				return 0;
			}

			// 检查长度是否超出范围
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

/** 是否为周免英雄
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