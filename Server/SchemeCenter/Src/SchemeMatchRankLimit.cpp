#include "StdAfx.h"
#include "SchemeMatchRankLimit.h"
#include "IServerGlobal.h"
#include <string>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeMatchRankLimit::CSchemeMatchRankLimit(void)
{

}

/** 
@param   
@param   
@return  
*/
CSchemeMatchRankLimit::~CSchemeMatchRankLimit(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeMatchRankLimit::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

    string stringPath = SCP_PATH_FORMAT( MATCHROOM_RANK_HERO_LIMITE_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("CSchemeMatchRankLimit::LoadScheme failed! filename=" << stringPath.c_str() << endl);
		return false;
	}

    stringPath = SCP_PATH_FORMAT( MATCHROOM_RANK_TIME_LIMITE_SCHEME_FILENAME );
	bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("CSchemeMatchRankLimit::LoadScheme failed! filename=" << stringPath.c_str() << endl);
		return false;
	}

	stringPath = SCP_PATH_FORMAT(DMATCHROOM_SCHEME_PUNISH_FILENAME);
	bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if (!bResult)
	{
		Error("CSchemeMatchRankLimit::LoadScheme failed! filename=" << stringPath.c_str() << endl);
		return false;
	}

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeMatchRankLimit::Close(void)
{	
    m_mapMatchRankLimitTimeMgr.clear();
    m_mapMatchRankLimitHero.clear();
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
bool CSchemeMatchRankLimit::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    bool bRet = false;
    string strHeroLimitPath = SCP_PATH_FORMAT( MATCHROOM_RANK_HERO_LIMITE_SCHEME_FILENAME );
    string strTimeLimitPath = SCP_PATH_FORMAT( MATCHROOM_RANK_TIME_LIMITE_SCHEME_FILENAME );
    string strPunishLimitPath = SCP_PATH_FORMAT(DMATCHROOM_SCHEME_PUNISH_FILENAME);

    if(rkt::StringHelper::casecmp(szFileName, strHeroLimitPath.c_str()) == 0)
    {
        bRet = LoadHeroLimitScheme(reader.pCSVReader);
    }

    if(rkt::StringHelper::casecmp(szFileName, strTimeLimitPath.c_str()) == 0)
    {
        bRet = LoadTimeLimitScheme(reader.pCSVReader);
    }
	
    if(rkt::StringHelper::casecmp(szFileName, strPunishLimitPath.c_str()) == 0)
    {
        bRet = LoadPunishScheme(reader.pCSVReader);
    }

    if(!bRet)
    {
        return false;
    }	

    AfterSchemeLoaded();
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
bool CSchemeMatchRankLimit::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeMatchRankLimit* pNewInstance = NewInstance();
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


/// purpose:  加载战队常量脚本
bool CSchemeMatchRankLimit::LoadTimeLimitScheme(ICSVReader * pCSVReader)
{
    int nRecordCount = pCSVReader->GetRecordCount();
    for(int nRow=0; nRow<nRecordCount; ++nRow)
    {
        int nCol = 0;
        
        EMMatchType nMatchTypeID = (EMMatchType)pCSVReader->GetInt(nRow, nCol++, 0);
        int nDay = pCSVReader->GetInt(nRow, nCol++, 0);

        if (nDay < -1 || nDay >= WEEK_DAY_COUNT_MAX)
        {
            Error(__FUNCTION__": config is not right! nDay ="<<nDay<<" nRow="<<nRow<<" nCol"<<--nCol);
            return false;
        }

        SMatchRankLimitTimeSchemeInfo node;
        int nBeginTimeList[2] = {0};
        char strBeginTimeList[128] = {0};
        int nLen = sizeof(strBeginTimeList);
        pCSVReader->GetString(nRow, nCol++, strBeginTimeList, nLen);
        int nParmCount = parseIntArray(strBeginTimeList, nBeginTimeList, 2, ';');
        node.nBeginHour = nBeginTimeList[0];
        node.nBeginMinute = nBeginTimeList[1];

        int nEndTimeList[2] = {0};
        char strEndTimeList[128] = {0};
        nLen = sizeof(strEndTimeList);
        pCSVReader->GetString(nRow, nCol++, strEndTimeList, nLen);
        nParmCount = parseIntArray(strEndTimeList, nEndTimeList, 2, ';');
        node.nEndHour = nEndTimeList[0];
        node.nEndMinute = nEndTimeList[1];

        node.nFailedDesID = pCSVReader->GetInt(nRow, nCol++, 0);

        TMAP_WEEK_TIMELIMITE_MGR::iterator iter = m_mapMatchRankLimitTimeMgr.find(nMatchTypeID);
        if (iter == m_mapMatchRankLimitTimeMgr.end())
        {
            //创建
            list<SMatchRankLimitTimeSchemeInfo> tempList;
            tempList.push_back(node);


            TMAP_WEEK_TIMELIMITE tempTimeLimite;
            tempTimeLimite.insert(pair<int, list<SMatchRankLimitTimeSchemeInfo>>(nDay,tempList));

            m_mapMatchRankLimitTimeMgr.insert(pair<EMMatchType, TMAP_WEEK_TIMELIMITE>(nMatchTypeID,tempTimeLimite));
        }  
        else
        {
           TMAP_WEEK_TIMELIMITE::iterator iterDayLimit = iter->second.find(nDay);
           if (iterDayLimit != iter->second.end())
           {
                list<SMatchRankLimitTimeSchemeInfo>* pRankLimiteList = &iterDayLimit->second;
                pRankLimiteList->push_back(node);
           }
           else
           {
               list<SMatchRankLimitTimeSchemeInfo> tempList;
               tempList.push_back(node);

               iter->second.insert(pair<int, list<SMatchRankLimitTimeSchemeInfo>>(nDay, tempList));
           }
           
        }
    }

    return true;
}

/// purpose:  加载战队升级脚本
bool CSchemeMatchRankLimit::LoadHeroLimitScheme(ICSVReader * pCSVReader)
{
    int nRecordCount = pCSVReader->GetRecordCount();
    for(int nRow=0; nRow<nRecordCount; ++nRow)
    {
        int nCol = 0;

        SMatchRankLimitHeroSchemeInfo node;
        node.nMatchType = pCSVReader->GetInt(nRow, nCol++, 0);
        node.nLimitHeroCount = pCSVReader->GetInt(nRow, nCol++, 0);
        node.nLimitHeroStarLevel = pCSVReader->GetInt(nRow, nCol++, 0);
        node.nLimitPlayerLevel = pCSVReader->GetInt(nRow, nCol++, 0);

        node.nOtherFailedDesID = pCSVReader->GetInt(nRow, nCol++, 0);
        node.nSelfFailedDesID = pCSVReader->GetInt(nRow, nCol++, 0);

        node.nOtherLevelFailedID = pCSVReader->GetInt(nRow, nCol++, 0);
        node.nSelfLevelFailedDesID = pCSVReader->GetInt(nRow, nCol++, 0);
        
        m_mapMatchRankLimitHero.insert(pair<EMMatchType,SMatchRankLimitHeroSchemeInfo>(EMMatchType(node.nMatchType),node));
    }
   
    return true;
}

bool CSchemeMatchRankLimit::LoadPunishScheme(ICSVReader* pCSVReader)
{
	m_SchemePunish.clear();
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		int nIndex = 0;
		SSchemePunish* pData = new SSchemePunish();

		// 惩罚类型
		pData->byPunishType = (BYTE)pCSVReader->GetInt(nRow, nIndex++, 0);

		// 免罚次数
		pData->byOverlayCount = (BYTE)pCSVReader->GetInt(nRow, nIndex++, 0);

		// 惩罚时间基数
		pData->nEveryPunishTime = pCSVReader->GetInt(nRow, nIndex++, 0);

		// 观察周期
		pData->nObserveCount = pCSVReader->GetInt(nRow, nIndex++, 0);

		// 是否需要检测比赛类型
		pData->bMatchTypeCheck = pCSVReader->GetInt(nRow, nIndex++, 0) == 1;


		int nMatchTypeArray[MatchType_MaxID] = { 0 };
		char strMatchTypeArray[128] = { 0 };
		int nLen = sizeof(strMatchTypeArray);
		pCSVReader->GetString(nRow, nIndex++, strMatchTypeArray, nLen);
		pData->nMatchTypeCount = parseIntArray(strMatchTypeArray, pData->nMatchTypeArray, MatchType_MaxID, ';');

		m_SchemePunish[pData->byPunishType] = *pData;

		delete pData;

	}
	return true;
}

bool CSchemeMatchRankLimit::matchTypeRankIsOpen(int eMatchTypeID, int nDay, int nHour, int nMinute)
{
    TMAP_WEEK_TIMELIMITE_MGR::iterator iter = m_mapMatchRankLimitTimeMgr.find((EMMatchType)eMatchTypeID);
    if (iter == m_mapMatchRankLimitTimeMgr.end())
    {
       // 没有配置的没有限定
       return true;
    }  
    else
    {
        TMAP_WEEK_TIMELIMITE::iterator iterDayLimit = iter->second.find(nDay);
        if (iterDayLimit == iter->second.end())
        {
            return false;
        }

        list<SMatchRankLimitTimeSchemeInfo>::iterator pIterList = iterDayLimit->second.begin();
        for (;pIterList!= iterDayLimit->second.end(); ++pIterList )
        {
            SMatchRankLimitTimeSchemeInfo timeLimitInfo = *pIterList;
            int nCompareTime = nHour*60 + nMinute;

            int nBeginTime = timeLimitInfo.nBeginHour*60 + timeLimitInfo.nBeginMinute;
            int nEndLimiteTime = timeLimitInfo.nEndHour*60 + timeLimitInfo.nEndMinute;
            if(nBeginTime <= nCompareTime && nCompareTime <= nEndLimiteTime )
            {
                return true;
            }
        }
    }

    return false;
}

int CSchemeMatchRankLimit::getTimeOpenFailedDesID(int eMatchTypeID)
{
    TMAP_WEEK_TIMELIMITE_MGR::iterator iter = m_mapMatchRankLimitTimeMgr.find((EMMatchType)eMatchTypeID);
    if (iter == m_mapMatchRankLimitTimeMgr.end())
    {
        // 没有配置的没有限定
        return 0;
    }  
    else
    {
        TMAP_WEEK_TIMELIMITE::iterator iterDayLimit = iter->second.find(-1);
        if (iterDayLimit == iter->second.end())
        {
            return 0;
        }

        list<SMatchRankLimitTimeSchemeInfo>::iterator iter = iterDayLimit->second.begin();
        if (iter == iterDayLimit->second.end())
        {
            return 0;
        }

        return iter->nFailedDesID;
    }
}

SMatchRankLimitHeroSchemeInfo* CSchemeMatchRankLimit::getRankLimitHeroShemeInfo(int eMatchTypeID)
{
    std::map<EMMatchType, SMatchRankLimitHeroSchemeInfo>::iterator iter = m_mapMatchRankLimitHero.find((EMMatchType)eMatchTypeID);
    if (iter == m_mapMatchRankLimitHero.end())
    {
        return NULL;
    }

    return &(iter->second);
}

int CSchemeMatchRankLimit::getMatchHeroFailedDesID(int eMatchTypeID, bool isSelf)
{
    std::map<EMMatchType, SMatchRankLimitHeroSchemeInfo>::iterator iter = m_mapMatchRankLimitHero.find((EMMatchType)eMatchTypeID);
    if (iter == m_mapMatchRankLimitHero.end())
    {
        return 0;
    }
    return isSelf == true? iter->second.nSelfFailedDesID: iter->second.nOtherFailedDesID;
}

int CSchemeMatchRankLimit::getMatchPlayerLevelFailedDesID(int eMatchTypeID, bool isSelf)
{
    std::map<EMMatchType, SMatchRankLimitHeroSchemeInfo>::iterator iter = m_mapMatchRankLimitHero.find((EMMatchType)eMatchTypeID);
    if (iter == m_mapMatchRankLimitHero.end())
    {
        return 0;
    }
    return isSelf == true? iter->second.nSelfLevelFailedDesID: iter->second.nOtherLevelFailedID;
}

SSchemePunish* CSchemeMatchRankLimit::getPunishInfo(BYTE byPunishID)
{
	std::map<BYTE, SSchemePunish>::iterator iter = m_SchemePunish.find(byPunishID);
	if (iter == m_SchemePunish.end())
	{
		return NULL;
	}

	return &(iter->second);
}