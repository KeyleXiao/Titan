#pragma once

#include <unordered_map>
#include "ISchemeEngine.h"
#include "SchemeCache.h"
#include "MatchSchemeDef.h"

class CSchemeMatchSeasonPrize : public ISchemeMatchSeasonPrize, public ISchemeUpdateSink, public CSchemeCache<CSchemeMatchSeasonPrize> 
{
public:
	/////////////////////////////// ISchemeMatchSeasonPrize //////////////////////////////////////
	virtual SSchemeMatchSeasonPrize*	getSchemeMatchSeasonPrize(int nPrizeID, int nRankGrade);

	/////////////////////////////// ISchemeUpdateSink //////////////////////////////////////
	virtual bool		OnSchemeLoad(SCRIPT_READER reader, const char * szFileName);
	virtual bool		OnSchemeUpdate(SCRIPT_READER reader, const char * szFileName);

	/////////////////////////////// CSchemeMatchSeasonTime //////////////////////////////////////
	CSchemeMatchSeasonPrize();
	~CSchemeMatchSeasonPrize();
	bool				LoadScheme();
	void				Close();

private:
	typedef unordered_map<int, SSchemeMatchSeasonPrize> TMap_SchemeRankPrize;
	typedef unordered_map<int, unordered_map<int, SSchemeMatchSeasonPrize> > TMap_SchemeSeasonPrize;
	TMap_SchemeSeasonPrize m_mapSeasonPrize;
};