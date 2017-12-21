#pragma once
#include "ISchemeCenter.h"
#include "ISchemeEngine.h"
#include "SchemeCache.h"
#include "SchemeDef.h"

struct SHolidayFormat
{
	int nYear;
	int nMonth;
	int nDay;

	SHolidayFormat()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 商城配置表
class CSchemePointShop : public ISchemePointShop, public ISchemeUpdateSink, public CSchemeCache<CSchemePointShop>
{
public:
	// ISchemeUpdateSink
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char *szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName);

	// ISchemePointShop
	// 获得商城物品配置信息
	virtual SPointShopGoodsConfig_S* getPointShopGoodsConfig(int nSellID);
    // 获取系统配置最大数量
    virtual int getLimitNumTotal(int nLimitID);
	// 获得物品随数量变动价格配置信息
	virtual int getPointShopNumLimitPrice(int nLimitID, int nResType, int nBoughtNum, int nNeedBuyNum);
	// 获得商城卡牌随机规则配置信息
	virtual SPointShopCardListRule* getPointShopCardListRuleConfig(int nHaveHeroNum);
	// 获得商城卡牌ID对应的出售ID
	virtual int getPointShopCardSellID(int nCardID);
	// 判断出售ID是否是商城卡牌
	virtual bool checkIfPointShopCardSellID(int nSellID);
	// 检查指定时间是否是节假日
	virtual bool checkIfHoliday(time_t lTime);
	// 获得某个类型的商品列表
	virtual bool getPointShopSellConfigList(EMPointShopGoodsType eType,vector<SPointShopGoodsConfig_S*> &listSellConfig);
public:
	bool LoadScheme();
	void Close();

private:
	// 加载商城物品配置脚本
	bool LoadPointShopGoodsScheme(ICSVReader *pCSVReader, const char *szFileName);
	// 加载物品随数量变动价格配置脚本
	bool LoadPointShopNumLimitPriceScheme(ICSVReader *pCSVReader, const char *szFileName);
	// 加载卡牌随机规则配置脚本
	bool LoadPointShopNumCardListRuleScheme(ICSVReader *pCSVReader, const char *szFileName);
	// 加载卡牌ID对应出售ID配置脚本
	bool LoadPointShopCardIDToSellIDScheme(ICSVReader *pCSVReader, const char *szFileName);
	// 加载节假日配置脚本
	bool LoadHolidayConfigScheme(ICSVReader *pCSVReader, const char *szFileName);

private:
	map<int, SPointShopGoodsConfig_S> m_mapPointShopGoodsConfig;

    typedef int LIMIT_ID, LIMIT_MAX;
	map<LIMIT_ID, SPointShopNumLimitPrice>  m_mapPointShopNumLimitPriceConfig;
    // 每种类型中的最大可购买数量
    map<LIMIT_ID, LIMIT_MAX>                m_mapSellLimitMax;

	vector<SPointShopCardListRule> m_vecRandomCardListRule;
	map<int, int> m_mapCardIDToSellID;
	map<int, int> m_mapSellIDToCardID;
	vector<SHolidayFormat> m_vecHoliday;
};