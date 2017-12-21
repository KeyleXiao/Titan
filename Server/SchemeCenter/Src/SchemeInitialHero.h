#pragma once
#include "SchemeDef.h"
#include "SchemeCache.h"
#include "ISchemeCenter.h"

class CSchemeInitialHero:public ISchemeInitialHero, public ISchemeUpdateSink, public CSchemeCache<CSchemeInitialHero>
{
public:
    virtual bool            OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool            OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

public:
    /////////////////////////////ISchemeInitialHero/////////////////////////////////////////////
    virtual INITIAL_HERO_SCHEMEINFOMAP      getInitialHero() { return m_initialHeroMap;};

    /////////////////////////////CSchemeInitialHero/////////////////////////////////////////////
    bool											LoadScheme(void);

    void											Close(void);

private:
    INITIAL_HERO_SCHEMEINFOMAP      m_initialHeroMap;
};