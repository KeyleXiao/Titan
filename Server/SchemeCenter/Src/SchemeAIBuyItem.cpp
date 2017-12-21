#include "stdafx.h"
#include "SchemeAIBuyItem.h"
#include "IServerGlobal.h"

#define AIBUYITEM_FILENAME "Scp\\AIBuyItem.csv"

bool CAIBuyItemScheme::Load()
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }

    // 载入AI购买物品脚本
    std::string strPath = SCP_PATH_FORMAT( AIBUYITEM_FILENAME );
    if(!pSchemeEngine->LoadScheme(strPath.c_str(),this,true))
    {
        ErrorLn("load " << AIBUYITEM_FILENAME << " failed");
        return false;
    }

    return true;
}

SBuyScheme* CAIBuyItemScheme::getBuyScheme(int nAISchemeID)
{
    std::map<int, SBuyScheme>::iterator it = m_mapBuyScheme.find(nAISchemeID);
    if(it == m_mapBuyScheme.end())
    {
        return NULL;
    }

    return &(it->second);
}

bool CAIBuyItemScheme::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    bool bRet = false;

    bRet = loadBuyItemScheme(reader.pCSVReader);

    if(!bRet)
    {
        return false;
    }

    AfterSchemeLoaded();

    return true;
}

bool CAIBuyItemScheme::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CAIBuyItemScheme* pNew = NewInstance();
    if(pNew == NULL)
    {
        return false;
    }

    if(pNew->OnSchemeLoad(reader, szFileName))
    {
        return true;
    }
    else
    {
        safeDelete(pNew);
        return false;
    }
}

bool CAIBuyItemScheme::loadBuyItemScheme(ICSVReader* pCSVReader)
{
    char szData[256] = {0};
    int nDataLen = sizeof(szData);
    std::vector<std::string> vecDataList;

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for(int nRow = 0; nRow < nRecordCount; ++nRow)
    {
        int nCol = 0;
        
        // AI配置ID
        int nAISchemeID = pCSVReader->GetInt(nRow, nCol++, 0);

        // 购买配置
        SBuyScheme& refBuyScheme = m_mapBuyScheme[nAISchemeID];

        // 出装进度表
        memset(&szData, 0, sizeof(szData));
        nDataLen = sizeof(szData);
        pCSVReader->GetString(nRow, nCol++, szData, nDataLen);
        vecDataList.clear();
        StringHelper::split(vecDataList, szData, ';');
        int nIndex = 0;
        int nSellIndex = 0;
        for(std::vector<std::string>::iterator it = vecDataList.begin(); it != vecDataList.end() && nIndex < AI_BUY_PROGRESS_MAX; ++it)
        {
            int nItem = StringHelper::toInt(*it);
            if(nItem < 0)
            {
                if(nSellIndex < AI_SELL_ONCE_MAX_COUNT) refBuyScheme.arrayBuyProgress[nIndex].arraySellItem[nSellIndex++] = -nItem;
            }
            else
            {
                refBuyScheme.arrayBuyProgress[nIndex++].nBuyItem = nItem;
                nSellIndex = 0;
            }
        }

        // 消耗品表
        memset(&szData, 0, sizeof(szData));
        nDataLen = sizeof(szData);
        pCSVReader->GetString(nRow, nCol++, szData, nDataLen);
        vecDataList.clear();
        StringHelper::split(vecDataList, szData, ';');
        nIndex = 0;
        for(std::vector<std::string>::iterator it = vecDataList.begin(); it != vecDataList.end() && nIndex < AI_MAX_BUY_CONSUMABLES; ++it)
        {
            refBuyScheme.arrayConsumables[nIndex++] = StringHelper::toInt(*it);
        }
    }

    return true;
}

void CAIBuyItemScheme::Close()
{
}