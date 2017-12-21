#pragma once
#include "ISchemeCenter.h"
#include "ISchemeEngine.h"
#include "SchemeCache.h"
#include "AIDef.h"

// AI ID配置表
class CSchemeAIIDTable : public ISchemeAIIDTable, public ISchemeUpdateSink, public CSchemeCache<CSchemeAIIDTable>
{
public:
    bool LoadScheme();

    void Close(void);

    // 获取默认英雄AI配置
    virtual int getHeroAIDefault(int nBattleFieldID, int nVocation, int nRank = 0);

    // 获取英雄AI配置ID(指定配置)
    virtual bool isHeroAIValid(int nBattleFieldID, int nVocation, int nAIID);

    // 获取NPC AI配置ID
    virtual int getNpcAISchemeID(int nBattleFieldID, int nNpcID);

    // 获取NPC AI配置ID
    virtual const char* getHeroAIDes(int nBattleFieldID, int nVocation, int nAIID);

    // 是否有AI配置
    virtual bool isHaveAIConfig(int nBattleFieldID);


    virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

private:

    bool LoadMonsterAIScheme(ICSVReader* pCSVReader);
    bool LoadHeroAIScheme(ICSVReader* pCSVReader);

    // <战场, <职业,AI配置>>
    std::map<int, std::map<int, HeroAISchemeNode>> m_mapHeroAISchemeTable;

    // <战场, <npcid,AI配置>>
    std::map<int, std::map<int, int>> m_mapNpcAISchemeTable;

};