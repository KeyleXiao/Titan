#pragma once

#include "ISchemeEngine.h"
#include "SchemeCache.h"
#include "MatchSchemeDef.h"

class CSchemeMatchSeasonTime : public ISchemeMatchSeasonTime, public ISchemeUpdateSink, public CSchemeCache<CSchemeMatchSeasonTime>
{
public:
	/////////////////////////////// ISchemeMatchSeasonTime //////////////////////////////////////
	virtual SSchemeMatchSeasonTime*		getSchemeMatchSeasonTimeByIndex(int nIndex);
	virtual SSchemeMatchSeasonTime*		getSchemeMatchSeasonTimeByTime(time_t tTime);
	virtual SSchemeMatchSeasonTime*		getSchemeMatchSeasonTimeByLastBeginTime();
	virtual SSchemeMatchSeasonTime*		getSchemeMatchSeasonTimeByLastEndTime();
	virtual SSchemeMatchSeasonTime*		getSchemeMatchSeasonTimeByNextBeginTime();
	virtual SSchemeMatchSeasonTime*		getSchemeMatchSeasonTimeByNextEndTime();

	/////////////////////////////// ISchemeUpdateSink //////////////////////////////////////
	virtual bool		OnSchemeLoad(SCRIPT_READER reader, const char * szFileName);
	virtual bool		OnSchemeUpdate(SCRIPT_READER reader, const char * szFileName);

	/////////////////////////////// CSchemeMatchSeasonTime //////////////////////////////////////
	CSchemeMatchSeasonTime();
	virtual ~CSchemeMatchSeasonTime();
	bool				LoadScheme();
	void				Close();

private:
	time_t				MakeTime(char * ch);

private:
	vector<SSchemeMatchSeasonTime> m_vecSeasonTime;
};