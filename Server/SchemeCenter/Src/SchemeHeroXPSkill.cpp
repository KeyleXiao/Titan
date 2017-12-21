#include "stdafx.h"
#include "SchemeHeroXpSkill.h"
#include "IServerGlobal.h"
#include <string>

// 0为全英雄
#define  SCHEME_ALL_HERO_ID     0

//////////////////////////////////////////////////////////////////////////
struct XpConfigCompare
{
    XpConfigCompare(SHeroXPSkillConfig_S data) : _data(data)
    {
    }

    bool operator()(SHeroXPSkillConfig_S data)
    {
        return _data.nHeroID == data.nHeroID && _data.nXPSkillID == data.nXPSkillID;
    }
private:
    SHeroXPSkillConfig_S _data;
};
//////////////////////////////////////////////////////////////////////////
bool CSchemeHeroXpSkill::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if (pSchemeEngine == NULL)
	{
		return false;
	}

	string stringPath = SCP_PATH_FORMAT(HERXPSKILL_SCHEME_FILENAME);
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
	if (!bResult)
	{
		ErrorLn("config file load failed filename = " << stringPath.c_str());;
		return false;
	}

	return true;
}

void CSchemeHeroXpSkill::Close(void)
{	
    m_schemeInfo.clear();
}


bool CSchemeHeroXpSkill::OnSchemeLoad(SCRIPT_READER reader, LPCSTR szFileName)
{
	ICSVReader *pCSVReader = reader.pCSVReader;
	if (pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
	{
		return false;
	}

    // 清空
    m_schemeInfo.clear();

    int nHeroID = 0;
    // 读取
    DWORD dwRecordCount = pCSVReader->GetRecordCount();
    for (DWORD nRow = 0; nRow < dwRecordCount; ++nRow)
    {
        SHeroXPSkillConfig_S node;
       
        node.nHeroID            = pCSVReader->GetInt(nRow, COL_XP_HERO_ID, 0);      // 英雄ID
        node.nXPSkillID         = pCSVReader->GetInt(nRow, COL_XP_SPELL_ID, 0);     // XPID
        node.nXPType            = pCSVReader->GetInt(nRow, COL_XP_SPELL_TYPE, 0);   // XP类型(0通用 1 专属)
        node.bAutoActive        = pCSVReader->GetInt(nRow, COL_XP_AUTO_UNLOCK, 0);  // 是否自动激活  
        node.nLockXPFragmentNum = pCSVReader->GetInt(nRow, COL_XP_CHIP_NUM, 0);     // 解锁xp所需碎片数
        node.bDefualtEquip      = pCSVReader->GetInt(nRow,COL_XP_DRESS, 0);         // 解锁后默认装备

        nHeroID = node.nHeroID;
        if(node.nHeroID == SCHEME_ALL_HERO_ID)
        {
            for (int nHeroID = 1; nHeroID < VOCATION_MAX; ++nHeroID )
            {
                node.nHeroID = nHeroID;
                if( !updateXpConfigToCache(node) )
                {
                    ErrorLn(__FUNCTION__": Load failed! row="<< (nRow+1) <<", heroID="<< nHeroID <<", xp="<< node.nXPSkillID);
                    return false;
                }
            }
        }
        else
        {
            if( !updateXpConfigToCache(node) )
            {
                ErrorLn(__FUNCTION__": Load failed! row="<< (nRow+1) <<", heroID="<< nHeroID <<", xp="<< node.nXPSkillID);
                return false;
            }
        }		
    }

	AfterSchemeLoaded();

    return true;
}

bool CSchemeHeroXpSkill::OnSchemeUpdate(SCRIPT_READER reader, LPCSTR szFileName)
{
    return false;
}


bool CSchemeHeroXpSkill::updateXpConfigToCache( SHeroXPSkillConfig_S& sConfigInfo )
{
    XP_LIST& nodeList = m_schemeInfo[sConfigInfo.nHeroID];
    XP_LIST::iterator iter = find_if( nodeList.begin(), nodeList.end(), XpConfigCompare(sConfigInfo) );
    if(iter != nodeList.end())
    {
        return false;
    }

    nodeList.push_back(sConfigInfo);
    return true;
}


// 获得XP信息
SHeroXPSkillConfig_S* CSchemeHeroXpSkill::getHeroXpSkillConfig(int nHeroID,int nXPSkillID) 
 {
	 if(m_schemeInfo.find(nHeroID) == m_schemeInfo.end())
	    return NULL;
	 
     SHeroXPSkillConfig_S node;
     node.nHeroID = nHeroID;
     node.nXPSkillID = nXPSkillID;

     XP_LIST& nodeList = m_schemeInfo[nHeroID];
     XP_LIST::iterator iter = find_if( nodeList.begin(), nodeList.end(), XpConfigCompare(node) );
     return (iter == nodeList.end()) ? NULL : &(*iter);
 }


bool CSchemeHeroXpSkill::getHeroXpActiveList( SHeroXPSkillConfig_S activeXpSkillList[],int nListCount )
{
	int nListIndex = 0;
	for(HEROXP_MAP::iterator iterHero = m_schemeInfo.begin(); iterHero!=m_schemeInfo.end(); ++iterHero)
	{
		XP_LIST& nodeList = iterHero->second;		
		for (XP_LIST::iterator itBegin = nodeList.begin(); itBegin!=nodeList.end(); ++itBegin)
		{
			if(nListIndex>=nListCount)
				return true;

			if (itBegin->bAutoActive)
			{
				activeXpSkillList[nListIndex++] = *itBegin;
			}
		}
	}

	return nListIndex>0;
}
