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

    // 获取英雄AI配置ID(指定配置)
    virtual bool isHeroAIValid(int nBattleFieldID, int nVocation, int nAIID);

    // 获取NPC AI配置ID
    virtual int getNpcAISchemeID(int nBattleFieldID, int nNpcID);

    // 获取英雄 AI配置ID
    virtual int getHeroAISchemeID(int nBattleFieldID, int nVocation);

    // 获取英雄掉线回城AI nTime:持续时间
    virtual int getHeroDisconnectBackAI(int nBattleFieldID, int nVocation, int& nTime);

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