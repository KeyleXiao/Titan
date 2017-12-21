#pragma once
#include "SchemeDef.h"
#include "SchemeCache.h"
#include "ISchemeCenter.h"

using namespace rkt;
class CSchemeMatchCoefficient : public ISchemeMatchCoefficient, public ISchemeUpdateSink, public CSchemeCache<CSchemeMatchCoefficient>
{
public:
    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
    virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////////////ISchemeMatchCoefficient/////////////////////////////////////////
    virtual SMatchCoefficient* getMatchCoeff( int nWarID );
public:
    /////////////////////////////////CSchemeMatchCoefficient/////////////////////////////////////////
    bool                    LoadScheme();

    void                    Close();

private:
    // 技能id为索引

    std::map<int, SMatchCoefficient>        m_mapMatchCoeff;

};