#pragma once
#include "ISchemeCenter.h"
#include "ISchemeEngine.h"
#include "SchemeCache.h"
#include "SchemeDef.h"

// ±¶œ‰≈‰÷√±Ì
class CSchemePrizeChest : public ISchemePrizeChest, public ISchemeUpdateSink, public CSchemeCache<CSchemePrizeChest>
{
public:
	// ISchemeUpdateSink
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char *szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName);

	// ISchemePrizeChest
	// ªÒµ√±¶œ‰≈‰÷√–≈œ¢
	virtual SPrizeChestConfig_S* getPrizeChestConfig(int nChestID);

public:
	bool LoadScheme();
	void Close();

private:
	// º”‘ÿΩ±¿¯±¶œ‰Ω≈±æ
	bool LoadPrizeChestScheme(ICSVReader *pCSVReader,const char *szFileName);

private:
	map<int, SPrizeChestConfig_S> m_mapPrizeChestConfig;
};