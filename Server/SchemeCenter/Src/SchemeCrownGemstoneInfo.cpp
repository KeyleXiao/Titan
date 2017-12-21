#include "stdafx.h"
#include "SchemeCrownGemstoneInfo.h"
#include "IServerGlobal.h"
#include <string>


bool CSchemeCrownGemstoneInfo::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    string stringPath = SCP_PATH_FORMAT(CROWGEMSTONE_SCHEME_FILENAME);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
    if (!bResult)
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());
        return false;
    }

    stringPath = SCP_PATH_FORMAT(CROWGEMSTONE_EFFECT_SCHEME_FILENAME);
    bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
    if (!bResult)
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());
        return false;
    }

    stringPath = SCP_PATH_FORMAT(CROWGEMSTONE_COBSTONE_RANDOM_SCHEME_FILENAME);
    bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
    if (!bResult)
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());
        return false;
    }

    stringPath = SCP_PATH_FORMAT(CROWGEMSTONE_RANDOM_SCHEME_FILENAME);
    bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
    if (!bResult)
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());
        return false;
    }

    stringPath = SCP_PATH_FORMAT(CROWGEMSTONE_EXTERIOR_SCHEME_FILENAME);
    bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
    if (!bResult)
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());
        return false;
    }

    return true;
}

void CSchemeCrownGemstoneInfo::Close(void)
{   
    ClearGemstoneRandomNodeMap();
    m_SchemeCrownGemstoneInfo.clear();
    m_SchemeCrownGemstoneEffect.clear();
}

bool CSchemeCrownGemstoneInfo::OnSchemeLoad(SCRIPT_READER reader, LPCSTR szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if (pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
    {
        return false;
    }

    bool bRet = false;

    if ( rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(CROWGEMSTONE_SCHEME_FILENAME)) == 0)
    {
        bRet = LoadCrownGemstoneInfoScheme(pCSVReader,szFileName);
    }
    else if (rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(CROWGEMSTONE_EFFECT_SCHEME_FILENAME)) == 0)
    {
        bRet = LoadCrownGemstoneEffectScheme(pCSVReader,szFileName);
    }
    else if (rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(CROWGEMSTONE_COBSTONE_RANDOM_SCHEME_FILENAME)) == 0)
    {
        bRet = LoadCrownCobstoneRandomScheme(pCSVReader,szFileName);
    }
    else if (rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(CROWGEMSTONE_RANDOM_SCHEME_FILENAME)) == 0)
    {
        bRet = LoadCrownGemstoneRandomScheme(pCSVReader,szFileName);
    }
    else if (rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(CROWGEMSTONE_EXTERIOR_SCHEME_FILENAME)) == 0)
    {
        bRet = LoadCrownCobstoneExteriorScheme(pCSVReader,szFileName);
    }

    if (!bRet)
    {
        return false;
    }   

    AfterSchemeLoaded();

    return true;
}

bool CSchemeCrownGemstoneInfo::OnSchemeUpdate(SCRIPT_READER reader, LPCSTR szFileName)
{
    return false;
}

enum EMSchemeCrownGemstoneInfo
{
    CROWN_GEMSTONE_ID = 0,                  // 宝石ID
    CROWN_GEMSTONE_NAME,                    // 宝石名称
    CROWN_COBSTONEGROUP_ID=3,               // 原石ID组
    CROWN_GEMSTONE_LEVELLIMITUPPER,         // 最高品级
    CROWN_GEMSTONE_EXPLIMITUPPER,           // 灵气上限
    CROWN_GEMSTONE_EXPARRAY,                // 升级灵气条件
    CROWN_GEMSTONE_PROPERTYIDARRAY,         // X级属性组

    CROWN_GEMSTONE_EXTERIORIDARRAY=16,      // X级外观
    CROWN_GEMSTONE_SEILMONEYARRAY=23,       // 出售价
    CROWN_GEMSTONE_APPRAISALPROCESSMAX,     // 增加抛光进度上限
    CROWN_GEMSTONE_APPRAISALCOSTMONEY,      // 消耗总金币
    CROWN_GEMSTONE_APPRAISALCOSTTICKET,     // 消耗总点券
    CROWN_GEMSTONE_APPRAISALPROCESS,        // 增加进度值
    CROWN_GEMSTONE_APPRAISAL2CRIT,          // 2倍暴击率
    CROWN_GEMSTONE_APPRAISAL5CRIT,          // 5倍暴击率
    CROWN_GEMSTONE_APPRAISAL10CRIT,         // 10倍暴击率
    CROWN_GEMSTONE_MAX
};

bool CSchemeCrownGemstoneInfo::LoadCrownGemstoneInfoScheme( ICSVReader * pCSVReader, LPCSTR szFileName )
{
    // 清空
    m_SchemeCrownGemstoneInfo.clear();

    SCrownGemstoneConfig_S sGemstoneInfo;

    // 读取
    int nLen = 0;
    for (DWORD nRow = 0; nRow < pCSVReader->GetRecordCount(); ++nRow)
    {
        sGemstoneInfo.Clear();

        // 宝石ID
        sGemstoneInfo.nGemstoneID = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_ID, 0);
        if (sGemstoneInfo.nGemstoneID <= 0)
        {
            ErrorLn(__FUNCTION__"() id is invalid, id=" << sGemstoneInfo.nGemstoneID << ",row=" << nRow + 1);
            return false;
        }

        // 宝石名称
        nLen = sizeof(sGemstoneInfo.chGemstoneName);
        pCSVReader->GetString(nRow,  CROWN_GEMSTONE_NAME, sGemstoneInfo.chGemstoneName, nLen);
        // 原石ID组
        sGemstoneInfo.nCobGroupID = pCSVReader->GetInt(nRow, CROWN_COBSTONEGROUP_ID, 0); 
        // 最高品级
        sGemstoneInfo.nGradeLimitUpper = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_LEVELLIMITUPPER, 0);
        if (sGemstoneInfo.nGradeLimitUpper >= EGGT_MAX)
        {
            ErrorLn(__FUNCTION__"() grade is more than max, grade="<< sGemstoneInfo.nGradeLimitUpper <<", MAX="<< EGGT_MAX <<", row="<< nRow+1);
            return false;
        }

        // 灵气上限
        sGemstoneInfo.nExpLimitUpper = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_EXPLIMITUPPER, 0); 
        // 升级灵气条件
        nLen = sizeof(sGemstoneInfo.stExpArray);
        pCSVReader->GetString(nRow,  CROWN_GEMSTONE_EXPARRAY, sGemstoneInfo.stExpArray, nLen);
        // X级属性组                
        for (int i=0;i<(int)CGI_LEVEL_COUNT_MAX;++i)
        {
            nLen = sizeof(sGemstoneInfo.stPropertyIDArray);
            pCSVReader->GetString(nRow,  CROWN_GEMSTONE_PROPERTYIDARRAY+i, sGemstoneInfo.stPropertyIDArray[i], nLen);
        }
        // X级外观
        for (int i = 0; i < (int)CGI_LEVEL_COUNT_MAX; ++i)
        {
            sGemstoneInfo.nExteriorIDArray[i] = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_EXTERIORIDARRAY + i, 0);
        }
        // 出售价
        nLen = sizeof(sGemstoneInfo.stSeilMoneyArray);
        pCSVReader->GetString(nRow,  CROWN_GEMSTONE_SEILMONEYARRAY, sGemstoneInfo.stSeilMoneyArray, nLen);
        // 增加抛光进度上限
        sGemstoneInfo.nAppraisalProcessMax = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_APPRAISALPROCESSMAX, 0); 
        // 消耗总金币
        sGemstoneInfo.nAppraisalCostMoney = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_APPRAISALCOSTMONEY, 0); 
        // 消耗总点券
        sGemstoneInfo.nAppraisalCostTicket = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_APPRAISALCOSTTICKET, 0); 
        // 增加进度值
        sGemstoneInfo.nAppraisalProcess = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_APPRAISALPROCESS, 0);

        for (int i=0; i < (int)CROWN_GEMSTONE_APPRAISALCRIT_MAX; ++i)
        {
            if (i == 0)
            {
                sGemstoneInfo.appraisalCritArray[i].nCritNum = 2;
                sGemstoneInfo.appraisalCritArray[i].nCritRate = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_APPRAISAL2CRIT, 0);
            }
            else if (i == 1)
            {
                sGemstoneInfo.appraisalCritArray[i].nCritNum = 5;
                sGemstoneInfo.appraisalCritArray[i].nCritRate = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_APPRAISAL5CRIT, 0);
            }
            else
            {
                sGemstoneInfo.appraisalCritArray[i].nCritNum = 10;
                sGemstoneInfo.appraisalCritArray[i].nCritRate = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_APPRAISAL10CRIT, 0);
            }
        }

        m_SchemeCrownGemstoneInfo.insert(make_pair<int, SCrownGemstoneConfig_S>(sGemstoneInfo.nGemstoneID, sGemstoneInfo));
    }

    return true;
}

// CrownGemstoneEffect.csv
enum EMSchemeCrownGemstoneEffect
{
    CROWN_GEMSTONE_EFFECT_ID = 0,               // 属性ID
    CROWN_GEMSTONE_EFFECT_VALUE,                // 属性威力

    CROWN_GEMSTONE_EFFECT_EFFECTIDARRAY=4,      // 效果id组(1级)
    CROWN_GEMSTONE_EFFECT_EFFECTVALUEARRAY,     // 改动的参数组(1级)

    CROWN_GEMSTONE_EFFECT_MAX,
};

bool CSchemeCrownGemstoneInfo::LoadCrownGemstoneEffectScheme( ICSVReader * pCSVReader, LPCSTR szFileName )
{
    // 清空
    m_SchemeCrownGemstoneEffect.clear();

    SCrownGemstoneEffectConfig_S sGemstoneEffectInfo;
    // 读取
    int nLen = 0;
    for (size_t nRow = 0; nRow < pCSVReader->GetRecordCount(); ++nRow)
    {
        sGemstoneEffectInfo.Clear();
        // 属性ID
        sGemstoneEffectInfo.nPeopertyID = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_EFFECT_ID, 0);
        // 属性威力
        sGemstoneEffectInfo.nPropertyValue = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_EFFECT_VALUE, 0);
        // 效果id组(1级)
        nLen = sizeof(sGemstoneEffectInfo.stEffectIDArray);
        pCSVReader->GetString(nRow,  CROWN_GEMSTONE_EFFECT_EFFECTIDARRAY, sGemstoneEffectInfo.stEffectIDArray, nLen);
        // 改动的参数组(1级)
        nLen = sizeof(sGemstoneEffectInfo.stEffectValueArray);
        pCSVReader->GetString(nRow,  CROWN_GEMSTONE_EFFECT_EFFECTVALUEARRAY, sGemstoneEffectInfo.stEffectValueArray, nLen);

        m_SchemeCrownGemstoneEffect.insert(make_pair<int, SCrownGemstoneEffectConfig_S>(sGemstoneEffectInfo.nPeopertyID, sGemstoneEffectInfo));
    }

    return true;
}

// CrownCobblestonRandom.csv
bool CSchemeCrownGemstoneInfo::LoadCrownCobstoneRandomScheme( ICSVReader * pCSVReader, LPCSTR szFileName )
{
    // 清空
    m_SchemeCobstoneRandom.clear();
    char szBuf[64] = {0};

    int  nCobstoneIDLowerArray[CROWN_GEMSTONE_RANDOMGROUP_MAX] = {0};
    int  nCobstoneIDUpperArray[CROWN_GEMSTONE_RANDOMGROUP_MAX] = {0};
    int  nCobstoneIDRandomRateArray[CROWN_GEMSTONE_RANDOMGROUP_MAX] = {0};

    SCrownCobstoneRandomConfig_S config;

    // 读取
    int nLen = 0;
    for (size_t nRow = 0; nRow < pCSVReader->GetRecordCount(); ++nRow)
    {
        int nColumnIndex = 0;

        config.Clear();
        // 原石ID组
        config.nCobstoneGroupID = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        // ID组下限组数字间不要有0 配是十个组 超过请联系程序
        memset(szBuf,0,sizeof(szBuf));
        nLen = sizeof(szBuf);
        pCSVReader->GetString(nRow, nColumnIndex++, szBuf, nLen);
        parseIntArray(szBuf, nCobstoneIDLowerArray, CROWN_GEMSTONE_RANDOMGROUP_MAX, ';');
        // ID组上限组数字间不要有0
        memset(szBuf,0,sizeof(szBuf));
        nLen = sizeof(szBuf);
        pCSVReader->GetString(nRow, nColumnIndex++, szBuf, nLen);
        parseIntArray(szBuf, nCobstoneIDUpperArray, CROWN_GEMSTONE_RANDOMGROUP_MAX, ';');
        // 概率组数字间不要有0
        memset(szBuf,0,sizeof(szBuf));
        nLen = sizeof(szBuf);
        pCSVReader->GetString(nRow, nColumnIndex++, szBuf, nLen);
        parseIntArray(szBuf, nCobstoneIDRandomRateArray, CROWN_GEMSTONE_RANDOMGROUP_MAX, ';');
    
        for (int i =0; i<CROWN_GEMSTONE_RANDOMGROUP_MAX; ++i)
        {
            SCrownCobstoneRandomItemConfig_S itemConfig ;
            itemConfig.nCobstoneIDLower = nCobstoneIDLowerArray[i];
            itemConfig.nCobstoneIDUpper = nCobstoneIDUpperArray[i];
            itemConfig.nCobstoneIDRandomRate = nCobstoneIDRandomRateArray[i];
            config.cobstoneRandomArray[i] = itemConfig;
        }

        m_SchemeCobstoneRandom.insert(make_pair<int, SCrownCobstoneRandomConfig_S>(config.nCobstoneGroupID, config));
    }

    return true;
}

// CrownGemstoneRandom.csv
bool CSchemeCrownGemstoneInfo::LoadCrownGemstoneRandomScheme( ICSVReader * pCSVReader, LPCSTR szFileName )
{
    // 清空
    ClearGemstoneRandomNodeMap();

    // 读取
    int nLen = 0;
    int  nGemstoneCountArray[CROWN_GEMSTONE_RANDOMGROUP_MAX] = {0};

    char szBuf[64] = {0};
    const int INNER_BUFF_SIZE = static_cast<int>(sizeof(szBuf));

    const DWORD LONG_BUFF_SIZE = 2000;
    char *szLongBuf = new char[LONG_BUFF_SIZE];
    if (nullptr == szLongBuf)
    {
        ErrorLn(__FUNCTION__": alloc memory failed for 2000");
        return false;
    }
    const int INNER_LONG_BUFF_SIZE = LONG_BUFF_SIZE*sizeof(char);

    int nGemStoneIDArray[CROWN_GEMSTONE_RANDOMID_MAX] = { 0 };

    int nColumnIndex = 0;
    for (DWORD nRow = 0; nRow < pCSVReader->GetRecordCount(); ++nRow)
    {
        memset(nGemstoneCountArray,0,sizeof(nGemstoneCountArray));
        nColumnIndex = 0;

        SCrownGemstoneRandomConfig_S* pConfig = new SCrownGemstoneRandomConfig_S;


        // 宝石组ID
        pConfig->nGemstoneGroupID = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        // 开窗消耗点券
        pConfig->nOpenWinCost = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        // 开窗消耗叠加点券
        pConfig->nAdditionWinCost = pCSVReader->GetInt(nRow, nColumnIndex++, 0);

        nLen = INNER_BUFF_SIZE;
        szBuf[0] = '\0';
        // 宝石组数量对应不宝石id组数字间不要有0
        if (!pCSVReader->GetString(nRow, nColumnIndex++, szBuf, nLen))
        {
            ErrorLn(__FUNCTION__": read data failed! row="<< (nRow+1)<<",col="<< nColumnIndex);
            goto failed;
        }

        parseIntArray(szBuf, nGemstoneCountArray, CROWN_GEMSTONE_RANDOMGROUP_MAX, ';');
        // 宝石ID组X 数字间不要有0
        for (int i =0;i<CROWN_GEMSTONE_RANDOMGROUP_MAX;++i)
        {
            SCrownGemstoneRandomItemConfig_S itemConfig;
            itemConfig.nGemstoneCount = nGemstoneCountArray[i];

            nLen = INNER_LONG_BUFF_SIZE;
            memset(szLongBuf, 0, nLen);
            pCSVReader->GetString(nRow, nColumnIndex++, szLongBuf, nLen);
            int nItemCount = parseIntArray(szLongBuf, nGemStoneIDArray, CROWN_GEMSTONE_RANDOMID_MAX, ';');
            if (nItemCount > 0) {
                itemConfig.GemStoneIDArray.resize(nItemCount);
                std::copy(nGemStoneIDArray, nGemStoneIDArray + nItemCount, itemConfig.GemStoneIDArray.begin());
            }
            pConfig->gemStoneIDArray[i] = itemConfig;
        }

        m_SchemeGemstoneRandom.insert(make_pair<int, SCrownGemstoneRandomConfig_S*>(pConfig->nGemstoneGroupID, pConfig));
    }

    if (szLongBuf) {
        delete[](szLongBuf);
    }
    return true;

failed:
    if (szLongBuf) {
        delete [] (szLongBuf);
    }

    return false;
}

enum EMSchemeCrownGemstoneExterior
{
    CROWN_GEMSTONE_EXTERIOR_ID = 0,         // 外观ID
    CROWN_GEMSTONE_EXTERIOR_FACESCORE,      // 颜值

    CROWN_GEMSTONE_EXTERIOR_MAX,
};
// CrownGemstoneExterior.csv
bool CSchemeCrownGemstoneInfo::LoadCrownCobstoneExteriorScheme( ICSVReader * pCSVReader, LPCSTR szFileName )
{
    // 清空
    m_SchemeGemstoneExterior.clear();

    SCrownGemstoneExteriorConfig_S sGemstoneExteriorInfo;
    // 读取
    for (size_t nRow = 0; nRow < pCSVReader->GetRecordCount(); ++nRow)
    {
        sGemstoneExteriorInfo.Clear();

        // 外观ID
        sGemstoneExteriorInfo.nExteriorID = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_EXTERIOR_ID, 0);
        // 颜值
        sGemstoneExteriorInfo.nFacescore = pCSVReader->GetInt(nRow, CROWN_GEMSTONE_EXTERIOR_FACESCORE, 0);  

        m_SchemeGemstoneExterior.insert(make_pair<int, SCrownGemstoneExteriorConfig_S>(sGemstoneExteriorInfo.nExteriorID, sGemstoneExteriorInfo));
    }

    return true;
}

void CSchemeCrownGemstoneInfo::ClearGemstoneRandomNodeMap()
{
    //for_each(m_SchemeGemstoneRandom.begin(), m_SchemeGemstoneRandom.end(), 
    //    [](std::pair<int, SCrownGemstoneRandomConfig_S*> my_pair) {
    //        SCrownGemstoneRandomConfig_S* pNode = my_pair.second;
    //        delete (pNode);
    //    }
    //);
    for (auto iter = m_SchemeGemstoneRandom.begin(); iter != m_SchemeGemstoneRandom.end(); ++iter)
    {
        SCrownGemstoneRandomConfig_S* pNode = iter->second;
        if (pNode)
        {
            delete pNode;
        }
    }
    m_SchemeGemstoneRandom.clear();
}


SCrownGemstoneConfig_S* CSchemeCrownGemstoneInfo::getCrownGemstoneConfig(int nGemstoneID)
{
    MapCrownInfo::iterator iterResult = m_SchemeCrownGemstoneInfo.find(nGemstoneID);
    if(iterResult == m_SchemeCrownGemstoneInfo.end())
        return NULL;

    return &(iterResult->second);
}

// 获得皇冠宝石效果配置信息
SCrownGemstoneEffectConfig_S* CSchemeCrownGemstoneInfo::getCrownGemstoneEffectConfig(int nPeopertyID)
{
     MapCrownEffect::iterator iterResult = m_SchemeCrownGemstoneEffect.find(nPeopertyID);
     if(iterResult == m_SchemeCrownGemstoneEffect.end())
         return NULL;

     return &(iterResult->second);
}


SCrownGemstoneRandomConfig_S* CSchemeCrownGemstoneInfo::getCrownGemstoneRandomConfig(int nGemstoneGroupID)
{
    MapGemstoneRandom::iterator iterResult = m_SchemeGemstoneRandom.find(nGemstoneGroupID);
    if(iterResult == m_SchemeGemstoneRandom.end())
        return NULL;

    return iterResult->second;
}

SCrownCobstoneRandomConfig_S* CSchemeCrownGemstoneInfo::getCrownCobstoneRandomConfig(int nCobstoneGroupID)
{
    MapCobstoneRandom::iterator iterResult = m_SchemeCobstoneRandom.find(nCobstoneGroupID);
    if(iterResult == m_SchemeCobstoneRandom.end())
        return NULL;

    return &(iterResult->second);
}

// 获得皇冠外观配置信息
SCrownGemstoneExteriorConfig_S* CSchemeCrownGemstoneInfo::getCrownCobstoneExteriorConfig(int nExteriorID)
{
    MapGemstoneExterior::iterator iterResult = m_SchemeGemstoneExterior.find(nExteriorID);
    if(iterResult == m_SchemeGemstoneExterior.end())
        return NULL;

    return &(iterResult->second);
}
