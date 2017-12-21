#include "stdafx.h"
#include "SchemeTalent.h"
#include "IServerGlobal.h"
#include "SpellDef.h"

// 加载天赋配置
bool CTalentScheme::Load()
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }

    m_mapTalentEffect.clear();
    m_mapVocationLevelTalents.clear();
    m_mapVocationTalents.clear();
    m_setDummy.clear();
    m_mapTalent.clear();

    // 载入天赋学习脚本
    string stringPath = SCP_PATH_FORMAT( TALENT_LEARN_FILENAME );
    if(!pSchemeEngine->LoadScheme(stringPath.c_str(),this,true))
    {
        ErrorLn("load " << TALENT_LEARN_FILENAME << " failed");
        return false;
    }

    // 载入天赋效果脚本
    stringPath = SCP_PATH_FORMAT( TALENT_EFFECT_FILENAE );
    if(!pSchemeEngine->LoadScheme(stringPath.c_str(),this,true))
    {
        ErrorLn("load " << TALENT_EFFECT_FILENAE << " failed");
        return false;
    }

    return true;
}

// 获取某个职业天赋列表
void CTalentScheme::GetVocationTalentListByLevel(int nMatchType, int nMapID, int nVocation, int nLevel, int* pResultArray, int& nLen)
{
    if(pResultArray == NULL || nLen <= 0)
    {
        return;
    }

    std::map<int, std::map<int, std::set<int>>>::iterator iter = m_mapVocationLevelTalents.find(nVocation);
    if(iter == m_mapVocationLevelTalents.end())
    {
        return;
    }

	nMapID = FormatMap(nMapID);

    bool bEnd = false;
    int nCount = 0;
    for(std::map<int, std::set<int>>::iterator it = iter->second.begin(); it != iter->second.end(); ++it)
    {
        if (bEnd)
        {
            break;
        }

        if(it->first > nLevel)
        {
            continue;
        }

        for(std::set<int>::iterator it1 = it->second.begin(); it1 != it->second.end(); ++it1)
        {
            // 天赋ID
            int nTalentID = *it1;

            const STalent *pTalent = GetTalent(nTalentID);
            if (pTalent == NULL)
            {
                continue;
            }

            // 比赛类型不为0，需要匹配
            if (nMatchType != 0 && find(pTalent->vecMatchType.begin(), pTalent->vecMatchType.end(), nMatchType) == pTalent->vecMatchType.end())
            {
                continue;
            }

            // 地图ID不为0，需要匹配
            if (nMapID != 0 && find(pTalent->vecMapID.begin(), pTalent->vecMapID.end(), nMapID) == pTalent->vecMapID.end())
            {
                continue;
            }

            if(nCount < nLen)
            {
                pResultArray[nCount++] = *it1;
            }
            else
            {
                bEnd = true;
                ErrorLn("GetVocationTalentListByLevel error nCount >= nLen,"<<",nCount="<<nCount<<",nLen="<<nLen);
                break;
            }
        }
    }

    nLen = nCount;
}

// 获取天赋
const STalent* CTalentScheme::GetTalent(int nTalentID)
{
    std::map<int, STalent>::iterator it = m_mapTalent.find(nTalentID);
    if(it == m_mapTalent.end())
    {
        return NULL;
    }

    return &it->second;
}

// 获取天赋效果
const STalentEffectBase* CTalentScheme::GetTalentEffect(int nTalentEffectID)
{
    std::map<int, STalentEffectBase*>::iterator iterFind =  m_mapTalentEffect.find(nTalentEffectID);
    if(iterFind == m_mapTalentEffect.end())
    {
        return NULL;
    }

    return iterFind->second;
}

// 获取某个天赋的天赋效果列表
void CTalentScheme::GetEffectsOfTalent(int nTalentID, const STalentEffectBase** pArrayResult, int& nLen)
{
    const STalent* pTalent = GetTalent(nTalentID);
    if(pTalent == NULL || pArrayResult == NULL || nLen <= 0)
    {
        return;
    }

    int nCount = 0;
    for(std::vector<int>::const_iterator it = pTalent->vecEffect.begin(); it != pTalent->vecEffect.end(); ++it)
    {
        const STalentEffectBase* p = GetTalentEffect(*it);
        if(p != NULL)
        {
            if(nCount < nLen)
            {
                pArrayResult[nCount++] = p;
            }
            else
            {
                ErrorLn("GetEffectsOfTalent error nCount >= nLen,"<<",nCount="<<nCount<<",nLen="<<nLen);
                break;
            }
        }
    }

    nLen = nCount;

    return;
}

bool CTalentScheme::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    bool bRet = false;
    string strLearnPath = SCP_PATH_FORMAT( TALENT_LEARN_FILENAME );
    string strEffectPath = SCP_PATH_FORMAT( TALENT_EFFECT_FILENAE );
    
    if(rkt::StringHelper::casecmp(szFileName, strLearnPath.c_str()) == 0)
    {
        bRet = LoadTalentScheme(reader.pCSVReader);
    }
    else if(rkt::StringHelper::casecmp(szFileName, strEffectPath.c_str()) == 0)
    {
       bRet = LoadTalentEffectScheme(reader.pCSVReader);
    }

    if(!bRet)
    {
        return false;
    }

    AfterSchemeLoaded();

    return true;
}

bool CTalentScheme::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CTalentScheme* pNew = NewInstance();
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

bool CTalentScheme::LoadTalentScheme(ICSVReader* pCSVReader)
{
    char szData[256] = {0};
    int nDataLen = sizeof(szData);
    std::vector<std::string> vecDataList;

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for(int nRow = 0; nRow < nRecordCount; ++nRow)
    {
        int nCol = 0;

        // 天赋 
        STalent sTalent;
        
        // 天赋ID
        sTalent.nID = pCSVReader->GetInt(nRow, nCol++, 0);

        // 比赛类型 EMMatchType
        memset(&szData, 0, sizeof(szData));
        nDataLen = sizeof(szData);
        pCSVReader->GetString(nRow, nCol++, szData, nDataLen);
        vecDataList.clear();
        StringHelper::split(vecDataList, szData, ';');
        for(std::vector<std::string>::iterator it = vecDataList.begin(); it != vecDataList.end(); ++it)
        {
            sTalent.vecMatchType.push_back(StringHelper::toInt(*it));
        }

        // 地图ID
        memset(&szData, 0, sizeof(szData));
        nDataLen = sizeof(szData);
        pCSVReader->GetString(nRow, nCol++, szData, nDataLen);
        vecDataList.clear();
        StringHelper::split(vecDataList, szData, ';');
        for(std::vector<std::string>::iterator it = vecDataList.begin(); it != vecDataList.end(); ++it)
        {
            sTalent.vecMapID.push_back(StringHelper::toInt(*it));
        }

        // 职业ID
        sTalent.nVocation = pCSVReader->GetInt(nRow, nCol++, 0);

        // 人物等级
        sTalent.nCondLevel = pCSVReader->GetInt(nRow, nCol++, 0);

        // 前置天赋
        sTalent.nCondPreTalent = pCSVReader->GetInt(nRow, nCol++, 0);

        // 激活类型 TalentActiveMode
        sTalent.nActiveMode = pCSVReader->GetInt(nRow, nCol++, 0);

        // 消耗天赋点数
        sTalent.nCostPoint = pCSVReader->GetInt(nRow, nCol++, 0);

        // 天赋效果列表
        memset(&szData, 0, sizeof(szData));
        nDataLen = sizeof(szData);
        pCSVReader->GetString(nRow, nCol++, szData, nDataLen);
        vecDataList.clear();
        StringHelper::split(vecDataList, szData, ';');
        for(std::vector<std::string>::iterator it = vecDataList.begin(); it != vecDataList.end(); ++it)
        {
            sTalent.vecEffect.push_back(StringHelper::toInt(*it));
        }

        // 天赋名称
        int nNameLen = sizeof(sTalent.szName);
        pCSVReader->GetString(nRow, nCol++, sTalent.szName, nNameLen);

        // 天赋图标ID
        sTalent.nIconID = pCSVReader->GetInt(nRow, nCol++);

        // 描述信息
        int nDescLen = sizeof(sTalent.szDesc);
        pCSVReader->GetString(nRow, nCol++, sTalent.szDesc, nDescLen);
        
        // 天赋插入容器
        m_mapTalent[sTalent.nID] = sTalent;

        // 所有职业通用天赋
        if(sTalent.nVocation == VOCATION_INVALID)
        {
            for(int nVocation = VOCATION_INVALID + 1; nVocation < VOCATION_MAX; ++nVocation)
            {
                // 职业-等级-天赋列表
                m_mapVocationLevelTalents[nVocation][sTalent.nCondLevel].insert(sTalent.nID);

                m_mapVocationTalents[nVocation].insert(sTalent.nID);
            }
        }
        else
        {
            // 职业-等级-天赋列表
            m_mapVocationLevelTalents[sTalent.nVocation][sTalent.nCondLevel].insert(sTalent.nID);

            m_mapVocationTalents[sTalent.nVocation].insert(sTalent.nID);
        }
    }

    return true;
}

bool CTalentScheme::LoadTalentEffectScheme(ICSVReader* pCSVReader)
{
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for(int nRow = 0; nRow < nRecordCount; ++nRow)
    {
        // 天赋效果类型
        int nEffectType =  pCSVReader->GetInt(nRow, 1, 0);

        if(nEffectType == Talent_Effect_AddSpell)  //增加技能
        {
            STalentEffectAddSpell* pEffect = new STalentEffectAddSpell;

            int nCol = 0;

            // 效果ID
            pEffect->nID = pCSVReader->GetInt(nRow, nCol++, 0);

            // 效果类型
            pEffect->nEffectType = pCSVReader->GetInt(nRow, nCol++, 0);

            // 参数1(新增技能ID)
            pEffect->nSpellID = pCSVReader->GetInt(nRow, nCol++, 0);

            // 槽位类型
            pEffect->nSlotType = pCSVReader->GetInt(nRow, nCol++, 0);

            // 槽位索引
            pEffect->nSlotIndex = pCSVReader->GetInt(nRow, nCol++, 0);

            // 槽位等级
            pEffect->nSlotLevel = pCSVReader->GetInt(nRow, nCol++, 0);

            m_mapTalentEffect[pEffect->nID] = pEffect;
        }
        else if(nEffectType == Talent_Effect_InfluenceSpellField) // 影响技能字段
        {
            STalentEffectInfluenceSpellField* pEffect = new STalentEffectInfluenceSpellField;

            int nCol = 0;

            // 效果ID
            pEffect->nID = pCSVReader->GetInt(nRow, nCol++, 0);

            // 效果类型
            pEffect->nEffectType = pCSVReader->GetInt(nRow, nCol++, 0);

            // 参数1(影响技能ID)
            pEffect->nSpellID = pCSVReader->GetInt(nRow, nCol++, 0);
            
            // 参数2(影响技能字段)
            pEffect->nSpellIndex = pCSVReader->GetInt(nRow, nCol++, 0);
            
            // 参数3 add(0) or set(1)
            pEffect->nType = pCSVReader->GetInt(nRow, nCol++, 0);

            // 参数3(天赋数值)
            if(SPELL::getSpellIndexType(pEffect->nSpellIndex) == SPELL::SpellIndex_Int)
            {
                pEffect->nValue = pCSVReader->GetInt(nRow, nCol++, 0);
            }
            else 
            {
                pEffect->nType = TALENT_STRING;
                int nLen = sizeof(pEffect->strValue); 
                pCSVReader->GetString(nRow, nCol++, pEffect->strValue, nLen);
            }

            m_mapTalentEffect[pEffect->nID] = pEffect;
        }
        else if(nEffectType == Talent_Effect_InfluenceSpellCondEff) // 影响技能条件效果
        {
            STalentEffectInfluenceSpellCondEff* pEffect = new STalentEffectInfluenceSpellCondEff;

            int nCol = 0;

            // 效果ID
            pEffect->nID = pCSVReader->GetInt(nRow, nCol++, 0);

            // 效果类型
            pEffect->nEffectType = pCSVReader->GetInt(nRow, nCol++, 0);

            // 参数1(影响技能ID)
            pEffect->nSpellID = pCSVReader->GetInt(nRow, nCol++, 0);

            // 参数2(技能事件)
            pEffect->nEventID = pCSVReader->GetInt(nRow, nCol++, 0);

            // 参数3(子类型)
            pEffect->nSubType = pCSVReader->GetInt(nRow, nCol++, 0);

            // 参数4(影响的技能的条件或效果ID)
            pEffect->nCondOrEffID = pCSVReader->GetInt(nRow, nCol++, 0);

            m_mapTalentEffect[pEffect->nID] = pEffect;
        }
        else if(nEffectType == Talent_Effect_StartEffects) // 直接开启一批效果
        {
            STalentEffectStartEffects* pEffect = new STalentEffectStartEffects;

            int nCol = 0;

            // 效果ID
            pEffect->nID = pCSVReader->GetInt(nRow, nCol++, 0);

            // 效果类型
            pEffect->nEffectType = pCSVReader->GetInt(nRow, nCol++, 0);

            // 参数1(效果列表)
            char szEffects[256] = {0};
            int nLen = sizeof(szEffects);
            pCSVReader->GetString(nRow, nCol++, szEffects, nLen);
            std::vector<std::string> vecEffectList;
            StringHelper::split(vecEffectList, szEffects, ';');
            for(std::vector<std::string>::iterator it = vecEffectList.begin(); it != vecEffectList.end(); ++it)
            {
                int nEffectID = StringHelper::toInt(*it);
                if (nEffectID <= 0)
                {
                    continue;
                }
                pEffect->setEffects.insert(nEffectID);
            }

            // 此列为客户端效果,服务器不必操作
            nCol++;

            m_mapTalentEffect[pEffect->nID] = pEffect;
        }
        else if(nEffectType == Talent_Effect_ReplaceSpell) // 替换技能
        {
            STalentEffectReplaceSpell* pEffect = new STalentEffectReplaceSpell;

            int nCol = 0;

            // 效果ID
            pEffect->nID = pCSVReader->GetInt(nRow, nCol++, 0);

            // 效果类型
            pEffect->nEffectType = pCSVReader->GetInt(nRow, nCol++, 0);

            // 参数1(旧技能ID)
            pEffect->nOldSpellID = pCSVReader->GetInt(nRow, nCol++, 0);

            // 参数2(新技能ID)
            pEffect->nNewSpellID = pCSVReader->GetInt(nRow, nCol++, 0); 

            m_mapTalentEffect[pEffect->nID] = pEffect;
        }
        else if (nEffectType == Talent_Effect_AddBuff)
        {
            STalentEffectAddBuff *pEffect = new STalentEffectAddBuff;

            int nCol = 0;

            // 效果ID
            pEffect->nID = pCSVReader->GetInt(nRow, nCol++, 0);

            // 效果类型
            pEffect->nEffectType = pCSVReader->GetInt(nRow, nCol++, 0);

            // 参数1(BuffID)
            pEffect->nBuffID = pCSVReader->GetInt(nRow, nCol++, 0);

            // 参数2(Buff等级)
            pEffect->nBuffLevel = pCSVReader->GetInt(nRow, nCol++, 0);

            m_mapTalentEffect[pEffect->nID] = pEffect;
        }
        else
        {
            ErrorLn("undefined talent effect type: row " << nRow);
        }
    }

    return true;
}

void CTalentScheme::Close()
{
    for(std::map<int, STalentEffectBase*>::iterator it = m_mapTalentEffect.begin(); 
        it != m_mapTalentEffect.end(); ++it)
    {
        if(it->second != NULL)
        {
            delete it->second;
        }
    }
}