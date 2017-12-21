/*******************************************************************
** 文件名:	SchemeHeroStarData.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	英雄星级数据
********************************************************************/
#include "StdAfx.h"
#include "SchemeHeroStarData.h"
#include "IClientGlobal.h"

/////////////////////////////////ISchemeHeroStarData/////////////////////////////////////////
int CSchemeHeroStarData::getUpgradeNeedExpTypeID( int nHeroId, int nStarLv)
{
    HERO_STAR_SCHEMEINFOMAP::iterator itStarID = m_mapHeroStarSchemeInfo.find(nHeroId);
    if ( itStarID == m_mapHeroStarSchemeInfo.end() )
    {
        WarningLn(__FUNCTION__": m_mapHeroStarSchemeInfo can't find nHeroId = " << nHeroId );
        return 0;
    }

    std::map<int, SHeroStarSchemeInfo> mapStarInfo = itStarID->second;

    std::map<int, SHeroStarSchemeInfo>::iterator itStarLv = mapStarInfo.find(nStarLv);
    if ( itStarLv == mapStarInfo.end() )
    {
        WarningLn(__FUNCTION__": m_mapHeroStarSchemeInfo can't find nHeroId = " << nHeroId << ", nStarLv = " << nStarLv);
        return 0;
    }

	return itStarLv->second.nExpType;
}

SHeroGrowProInfo* CSchemeHeroStarData::getGrowProInfo( int nHeroID, int nStarLv)
{
    HERO_STAR_SCHEMEINFOMAP::iterator itStarID = m_mapHeroStarSchemeInfo.find(nHeroID);
    if ( itStarID == m_mapHeroStarSchemeInfo.end() )
    {
        WarningLn(__FUNCTION__": m_mapHeroStarSchemeInfo can't find nHeroId = " << nHeroID );
        return NULL;
    }

    std::map<int, SHeroStarSchemeInfo>::iterator itStarLv = itStarID->second.find(nStarLv);
    if ( itStarLv == itStarID->second.end() )
    {
        WarningLn(__FUNCTION__": m_mapHeroStarSchemeInfo can't find nHeroId = " << nHeroID << ", nStarLv = " << nStarLv);
        return NULL;
    }

    int nProID = itStarLv->second.nPropertyID;

    GROW_PROPERTY_SCHEMEINFOMAP::iterator itPro = m_mapGrowSchemeInfo.find( nProID );
    if ( itPro != m_mapGrowSchemeInfo.end() )
    {
        return &itPro->second;
    }
    return NULL;
}

SHeroStarSchemeInfo* CSchemeHeroStarData::getHeroStarData( int nHeroID, int nStarLv)
{
    HERO_STAR_SCHEMEINFOMAP::iterator itStarID = m_mapHeroStarSchemeInfo.find(nHeroID);
    if ( itStarID == m_mapHeroStarSchemeInfo.end() )
    {
        WarningLn(__FUNCTION__": m_mapHeroStarSchemeInfo can't find nHeroId = " << nHeroID );
        return NULL;
    }

    std::map<int, SHeroStarSchemeInfo>::iterator itStarLv = itStarID->second.find(nStarLv);
    if ( itStarLv == itStarID->second.end() )
    {
        WarningLn(__FUNCTION__": m_mapHeroStarSchemeInfo can't find nHeroId = " << nHeroID << ", nStarLv = " << nStarLv);
        return NULL;
    }

    return &itStarLv->second;
}

int CSchemeHeroStarData::getHeroStarLvLimit(int nHeroID )
{
    std::map<int, int>::iterator it = m_mapHeroStarLvLimit.find(nHeroID);
    if ( it == m_mapHeroStarLvLimit.end() )
    {
        return 0;
    }
    return it->second;
}

/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
bool CSchemeHeroStarData::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    bool bResult = false;
    string stHeroStarPath = SCP_PATH_FORMAT(HEROSTAR_SCHEME_FILENAME);
    string stHeroProperty  = SCP_PATH_FORMAT(HEROPROPERTY_SCHEME_FILENAME);
    string stExclusiveTalent = SCP_PATH_FORMAT(HEROEXCLUSIVETALENT_SCHEME_FILENAME);

    if ( strcmp( szFileName, stHeroStarPath.c_str()) == 0 )
    {
        bResult = LoadStarLvScheme(reader);
    }else if ( strcmp(szFileName, stHeroProperty.c_str()) == 0 )
    {
        bResult = LoadHeroPropertyScheme(reader);
    }else if ( strcmp( szFileName, stExclusiveTalent.c_str()) == 0)
    {
        bResult = LoadExclusiveTalentScheme(reader);
    }

    return true;
}

bool CSchemeHeroStarData::LoadStarLvScheme(SCRIPT_READER reader)
{
    ICSVReader* pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapHeroStarSchemeInfo.clear();
    int nCount = pCSVReader->GetRecordCount();

    
    int nLen = 0;
    std::map<int, SHeroStarSchemeInfo> mapHeroStarSchemeInfo;
    for ( int i = 0; i < nCount; ++i )
    {
        int nColumnIndex = 0;
        SHeroStarSchemeInfo item;
        item.nHeroID                    = pCSVReader->GetInt(i, nColumnIndex++, 0);
        item.nStarLV                    = pCSVReader->GetInt(i, nColumnIndex++, 0);
        item.nPropertyID                = pCSVReader->GetInt(i, nColumnIndex++, 0);
        item.nLimitLV                   = pCSVReader->GetInt(i, nColumnIndex++, 0);
        item.nExpType                   = pCSVReader->GetInt(i, nColumnIndex++, 0);
        item.nExclusiveTalentMaxCount   = pCSVReader->GetInt(i, nColumnIndex++, 0);
        nLen = sizeof(item.chExclusiveTalent);
        pCSVReader->GetString(i, nColumnIndex++, item.chExclusiveTalent, nLen);

        char chTmp[512] = {0};
        nLen = sizeof(chTmp);
        pCSVReader->GetString(i, nColumnIndex++, chTmp, nLen);
        DWORD dwArr[32] = {0};
        int nCount = parseIntArray( chTmp, dwArr, 32);
        if ( nCount < 2 )
        {
            ErrorLn(__FUNCTION__": nColumnIndex is failed " << nColumnIndex - 1);
            continue;
        }
        item.dwNeedCardID = dwArr[0];
        item.nNeedCount = dwArr[1];
        item.nExpendGold = pCSVReader->GetInt(i, nColumnIndex++, 0);
		item.nStarIconID = pCSVReader->GetInt(i, nColumnIndex++, 0);
		nLen = sizeof(item.chStarDesc);
		pCSVReader->GetString(i, nColumnIndex++, item.chStarDesc, nLen);
		item.nStarFrameID = pCSVReader->GetInt(i, nColumnIndex++, 0);
		item.nStarEffectID = pCSVReader->GetInt(i, nColumnIndex++, 0);

		nColumnIndex = 13;
		item.nCardDiscount = pCSVReader->GetInt(i, nColumnIndex++, 0);
		item.nGoldDiscount = pCSVReader->GetInt(i, nColumnIndex++, 0);

        int nHeroId = item.nHeroID;
        HERO_STAR_SCHEMEINFOMAP::iterator itStarInfo = m_mapHeroStarSchemeInfo.find(nHeroId);
        if ( itStarInfo == m_mapHeroStarSchemeInfo.end() )
        {
            mapHeroStarSchemeInfo[item.nStarLV] = item;
            m_mapHeroStarSchemeInfo[nHeroId] = mapHeroStarSchemeInfo;
        }else
        {
            itStarInfo->second[item.nStarLV] = item;
        }

        std::map<int, int>::iterator itHeroStarLv = m_mapHeroStarLvLimit.find(nHeroId);
        if ( itHeroStarLv == m_mapHeroStarLvLimit.end() )
        {
            m_mapHeroStarLvLimit[nHeroId] = item.nStarLV;
        }else
        {
            int nStarlvLimit = m_mapHeroStarLvLimit[nHeroId];
            m_mapHeroStarLvLimit[nHeroId] = nStarlvLimit > item.nStarLV ? nStarlvLimit : item.nStarLV;
        }
    }
    return true;
}

bool CSchemeHeroStarData::LoadHeroPropertyScheme(SCRIPT_READER reader)
{
    ICSVReader* pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    int nPropID[PERSON_STAR_PROP_COUNT] = 
    {
        PROPERTY_APPEND_HP, PROPERTY_APPEND_MP, PROPERTY_APPEND_PA, PROPERTY_APPEND_MA, 
        PROPERTY_APPEND_PD, PROPERTY_APPEND_MD
    };

    m_mapGrowSchemeInfo.clear();
    int nCount = pCSVReader->GetRecordCount();

    for ( int i = 0; i < nCount; ++i )
    {
        int nColumnIndex = 0;
        SHeroGrowProInfo item;
        item.nGrowID = pCSVReader->GetInt( i, nColumnIndex++, 0);
        for (int j = 0; j < PERSON_STAR_PROP_COUNT; ++j )
        {
            item.sStarProp[j].nPropID = nPropID[j];
            item.sStarProp[j].nStarBase = pCSVReader->GetInt(i, nColumnIndex++, 0);
            item.sStarProp[j].nStarGrow = pCSVReader->GetFloat(i, nColumnIndex++, 0);
            item.sStarProp[j].nStarGrowRate = pCSVReader->GetFloat(i, nColumnIndex++, 0);
            item.sStarProp[j].nStarRateGrow = pCSVReader->GetFloat(i, nColumnIndex++, 0);
        }

        m_mapGrowSchemeInfo[item.nGrowID] = item;
    }

    return true;
}

bool CSchemeHeroStarData::LoadExclusiveTalentScheme(SCRIPT_READER reader)
{
    ICSVReader* pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapHeroExclusiveTalentInfo.clear();
    int nCount = pCSVReader->GetRecordCount();

    char chTmp[512] = {0};
    int nLen = sizeof(chTmp);
    for ( int i = 0; i <nCount; ++i )
    {
        int j = 0;
        memset(chTmp, 0, nLen);
        int nColumnIndex = 0;
        SHeroExclusiveTalentSchemeInfo item;
        item.nID = pCSVReader->GetInt(i, nColumnIndex++, 0);
        pCSVReader->GetString(i,nColumnIndex++, chTmp, nLen);
        item.stName = chTmp;

        memset(chTmp, 0, nLen);
        pCSVReader->GetString(i, nColumnIndex++, chTmp, nLen);
        item.stDesc = chTmp;

        memset(chTmp, 0, nLen);
        pCSVReader->GetString(i, nColumnIndex++, chTmp, nLen);

        int nArr[32] = {0};
        int nEffectCount = parseIntArray(chTmp,nArr, 32, ';');
        for (j = 0; j < nEffectCount; ++j )
        {
            item.vecEffect.push_back(nArr[j]);
        }

        memset(chTmp, 0, nLen);
        pCSVReader->GetString(i, nColumnIndex++, chTmp, nLen);
        parseConditionInfo(chTmp, item.mapCondition);

        m_mapHeroExclusiveTalentInfo[item.nID] = item;
    }

    return true;
}

void CSchemeHeroStarData::parseConditionInfo(char* pValue, std::map<int, int>& mapCondition)
{
    int nCondition[GLORY_CONDITION_NUM];
    char* pCondition = strtok( pValue, ";");
    int nCount = 0;
    while( pCondition )
    {
        memset(nCondition, 0, sizeof(nCondition));
        nCount = parseIntArray( pCondition, nCondition, GLORY_CONDITION_NUM, '_');
        if ( nCount > GLORY_CONDITION_NUM )
        {
            WarningLn(__FUNCTION__":nCount >= CONDITION_NUM");
            continue;
        }
        int nTypeID = nCondition[GLORY_CONDITION_TYPE];
        std::map<int,int>::iterator it = mapCondition.find(nTypeID);
        if( it != mapCondition.end())
        {
            WarningLn(__FUNCTION__": map is have id = " << nTypeID);
            continue;
        }

        mapCondition[nTypeID] = nCondition[GLORY_CONDITION_VALUE];
        pCondition = strtok(NULL,";");
    }
}

bool CSchemeHeroStarData::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return true;
}
/////////////////////////////////CSchemePlayerGloryInfo/////////////////////////////////////////
bool CSchemeHeroStarData::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载英雄星级脚本
    string stringPath = SCP_PATH_FORMAT( HEROSTAR_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        Error("config file load failed! filename=" << stringPath.c_str() << endl);
        return false;
    }


    // 加载属性类型脚本
    stringPath = SCP_PATH_FORMAT(HEROPROPERTY_SCHEME_FILENAME);
    bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        Error("config file load failed! filename=" << stringPath.c_str() << endl);
        return false;
    }

    // 加载英雄专属天赋脚本
    stringPath = SCP_PATH_FORMAT(HEROEXCLUSIVETALENT_SCHEME_FILENAME);
    bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        Error("config file load failed! filename=" << stringPath.c_str() << endl);
        return false;
    }


    return true;
}
void CSchemeHeroStarData::Close(void)
{
    m_mapHeroStarSchemeInfo.clear();
}