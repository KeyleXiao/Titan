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

// �̳����ñ�
class CSchemePointShop : public ISchemePointShop, public ISchemeUpdateSink, public CSchemeCache<CSchemePointShop>
{
public:
	// ISchemeUpdateSink
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char *szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName);

	// ISchemePointShop
	// ����̳���Ʒ������Ϣ
	virtual SPointShopGoodsConfig_S* getPointShopGoodsConfig(int nSellID);
    // ��ȡϵͳ�����������
    virtual int getLimitNumTotal(int nLimitID);
	// �����Ʒ�������䶯�۸�������Ϣ
	virtual int getPointShopNumLimitPrice(int nLimitID, int nResType, int nBoughtNum, int nNeedBuyNum);
	// ����̳ǿ����������������Ϣ
	virtual SPointShopCardListRule* getPointShopCardListRuleConfig(int nHaveHeroNum);
	// ����̳ǿ���ID��Ӧ�ĳ���ID
	virtual int getPointShopCardSellID(int nCardID);
	// �жϳ���ID�Ƿ����̳ǿ���
	virtual bool checkIfPointShopCardSellID(int nSellID);
	// ���ָ��ʱ���Ƿ��ǽڼ���
	virtual bool checkIfHoliday(time_t lTime);
	// ���ĳ�����͵���Ʒ�б�
	virtual bool getPointShopSellConfigList(EMPointShopGoodsType eType,vector<SPointShopGoodsConfig_S*> &listSellConfig);
public:
	bool LoadScheme();
	void Close();

private:
	// �����̳���Ʒ���ýű�
	bool LoadPointShopGoodsScheme(ICSVReader *pCSVReader, const char *szFileName);
	// ������Ʒ�������䶯�۸����ýű�
	bool LoadPointShopNumLimitPriceScheme(ICSVReader *pCSVReader, const char *szFileName);
	// ���ؿ�������������ýű�
	bool LoadPointShopNumCardListRuleScheme(ICSVReader *pCSVReader, const char *szFileName);
	// ���ؿ���ID��Ӧ����ID���ýű�
	bool LoadPointShopCardIDToSellIDScheme(ICSVReader *pCSVReader, const char *szFileName);
	// ���ؽڼ������ýű�
	bool LoadHolidayConfigScheme(ICSVReader *pCSVReader, const char *szFileName);

private:
	map<int, SPointShopGoodsConfig_S> m_mapPointShopGoodsConfig;

    typedef int LIMIT_ID, LIMIT_MAX;
	map<LIMIT_ID, SPointShopNumLimitPrice>  m_mapPointShopNumLimitPriceConfig;
    // ÿ�������е����ɹ�������
    map<LIMIT_ID, LIMIT_MAX>                m_mapSellLimitMax;

	vector<SPointShopCardListRule> m_vecRandomCardListRule;
	map<int, int> m_mapCardIDToSellID;
	map<int, int> m_mapSellIDToCardID;
	vector<SHolidayFormat> m_vecHoliday;
};