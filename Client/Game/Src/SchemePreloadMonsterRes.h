#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "IClientGlobal.h"

#define PRELOAD_MONSTER_RES "Scp\\PreloadMonsterRes.csv"

typedef std::vector<int> MonsterIdVec;
typedef std::map<int, MonsterIdVec> AllMonsterMap;

class CSchemePreloadMonsterRes: public ISchemePreloadMonsterRes, public ISchemeUpdateSink
{
public:
    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
    virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    ////////////////////////////////ISchemePreloadMonsterRes///////////////////////////////////
    virtual void getMapAllMonster(int nMapId, std::vector<int>& allMonster);
    
public:
    /////////////////////////////////CSchemePreloadMonsterRes/////////////////////////////////////////
    bool                    LoadScheme();

    void                    Close();
private:
    AllMonsterMap m_mapMonster;
};