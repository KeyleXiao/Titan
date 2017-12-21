#pragma once
#include "ISchemeCenter.h"
#include "ISchemeEngine.h"
#include "SchemeCache.h"
#include "SchemeDef.h"

// 奖励配置表
class CSchemeActorPrize : public ISchemeActorPrize, public ISchemeUpdateSink, public CSchemeCache<CSchemeActorPrize>
{
public:
	// ISchemeUpdateSink
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char *szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName);

	// ISchemeActorPrize
	// 获得奖励配置信息
	virtual SActorPrizeConfig_S* getActorPrizeConfig(int nPrizeID);
	// 获得奖励组配置信息
	virtual SActorPrizeGroupConfig* getActorPrizeGroupConfig(int nPrizeGroupID);
	// 获得奖励转换配置信息
	virtual SActorPrizeExchangeConfig* getPrizeExchangeConfig(int nPrizeID);
    // 校验是否存在此奖励ID
    virtual bool isExists(int nPrizeID);

public:
	bool LoadScheme();
	void Close();

private:
	// 加载角色奖励脚本
	bool LoadActorPrizeScheme(ICSVReader *pCSVReader,const char *szFileName);
	// 加载角色奖励组脚本
	bool LoadActorPrizeGroupScheme(ICSVReader *pCSVReader,const char *szFileName);
	// 加载角色奖励转换脚本
	bool LoadActorPrizeExchangeScheme(ICSVReader *pCSVReader,const char *szFileName);

    // 内部校验奖励数组是否合法
    bool checkPrizeForArray(const char* szFileName, int nRow, int * pArray, int nCount);

private:
	map<int, SActorPrizeConfig_S> m_mapActorPrizeConfig;
	map<int, SActorPrizeGroupConfig> m_mapActorPrizeGroupConfig;
	map<int, SActorPrizeExchangeConfig> m_mapActorPrizeExchangeConfig;
};