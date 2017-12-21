#include "StdAfx.h"
#include "SchemePointShop.h"
#include "IServerGlobal.h"
#include "EntityDef.h"

bool CSchemePointShop::OnSchemeLoad(SCRIPT_READER reader, const char *szFileName)
{
	ICSVReader *pCSVReader = reader.pCSVReader;
	if (pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
	{
		return false;
	}

	bool bRet = false;

	if ( rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(POINTSHOPGOODS_SCHEME_FILENAME)) == 0)
	{
		bRet = LoadPointShopGoodsScheme(pCSVReader,szFileName);
	}

	if ( rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(POINTSHOPNUMLIMITPRICE_SCHEME_FILENAME)) == 0)
	{
		bRet = LoadPointShopNumLimitPriceScheme(pCSVReader,szFileName);
	}

	if ( rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(POINTSHOPCARDLISTRULE_SCHEME_FILENAME)) == 0)
	{
		bRet = LoadPointShopNumCardListRuleScheme(pCSVReader,szFileName);
	}

	if ( rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(POINTSHOPCARDIDTOSELLID_SCHEME_FILENAME)) == 0)
	{
		bRet = LoadPointShopCardIDToSellIDScheme(pCSVReader,szFileName);
	}

	if (rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(POINTSHOPHOLIDAYCONFIG_SCHEME_FILENAME)) == 0)
	{
		bRet = LoadHolidayConfigScheme(pCSVReader,szFileName);
	}

	if (!bRet)
	{
		return false;
	}	

	AfterSchemeLoaded();

	return true;
}

bool CSchemePointShop::OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}

// 获得商城物品配置信息
SPointShopGoodsConfig_S* CSchemePointShop::getPointShopGoodsConfig(int nSellID)
{
	map<int, SPointShopGoodsConfig_S>::iterator iter = m_mapPointShopGoodsConfig.find(nSellID);
	if (iter != m_mapPointShopGoodsConfig.end())
		return &(*iter).second;
	return NULL;
}

// 获取系统配置最大数量
int CSchemePointShop::getLimitNumTotal(int nLimitID)
{
    map<LIMIT_ID, LIMIT_MAX>::iterator iterMap = m_mapSellLimitMax.find(nLimitID);

    return ( (iterMap != m_mapSellLimitMax.end()) ? (iterMap->second) : 0 );
}

// 获得物品随数量变动价格配置信息
int CSchemePointShop::getPointShopNumLimitPrice(int nLimitID, int nResType, int nBoughtNum, int nNeedBuyNum)
{
	map<int, SPointShopNumLimitPrice>::iterator iterMap = m_mapPointShopNumLimitPriceConfig.find(nLimitID);
	if (iterMap == m_mapPointShopNumLimitPriceConfig.end())
		return -1;

    int nHasBuyNum = nBoughtNum;    // 已经购买数量
    int nToBuyNum = nNeedBuyNum;    // 本次将要购买数量

	int nPriceTotal = 0;

    SPointShopNumLimitPrice &config = iterMap->second;
    vector<SPointShopNumLimitPriceSub>::iterator iter = config.vecLimitStep.begin();
    vector<SPointShopNumLimitPriceSub>::iterator iterEnd = config.vecLimitStep.end();
    for ( ; iter != iterEnd; ++iter)
    {
        if (nHasBuyNum>iter->nLimitNum)
            continue;

        int nSinglePrice = ( (nResType == ENPOINTSHOPRESTYPE_POINT) ? iter->nLimitPointPrice : iter->nLimitMoneyPrice );
        if (nSinglePrice <= 0)
            return -1;

        int nDiffNum = iter->nLimitNum - nHasBuyNum;
        // 当次可购买数量,如果不够将下次循环中继续扣除本次的数量
        int nCanBuyNum = (nDiffNum >= nToBuyNum ? nToBuyNum : nDiffNum) ;

        nPriceTotal += nSinglePrice * nCanBuyNum;

        nHasBuyNum += nCanBuyNum;
        nToBuyNum -= nCanBuyNum;
        if (nToBuyNum <= 0)
        {
            break;
        }
    }

	if (nPriceTotal > INT_MAX)
		return -1;

	return nPriceTotal;
}

// 获得商城卡牌随机规则配置信息
SPointShopCardListRule* CSchemePointShop::getPointShopCardListRuleConfig(int nHaveHeroNum)
{
	vector<SPointShopCardListRule>::iterator iter = m_vecRandomCardListRule.begin();
	vector<SPointShopCardListRule>::iterator iterEnd = m_vecRandomCardListRule.end();
	for ( ; iter != iterEnd; ++iter)
	{
		if (nHaveHeroNum <= iter->nNeedHeroNum)
			break;
	}

	if (iter != iterEnd)
		return &(*iter);

	return NULL;
}

// 获得商城卡牌ID对应的出售ID
int CSchemePointShop::getPointShopCardSellID(int nCardID)
{
	map<int, int>::iterator iter = m_mapCardIDToSellID.find(nCardID);
	if (iter != m_mapCardIDToSellID.end())
	{
		return iter->second;
	}

	return 0;
}

// 判断出售ID是否是商城卡牌
bool CSchemePointShop::checkIfPointShopCardSellID(int nSellID)
{
	map<int, int>::iterator iter = m_mapSellIDToCardID.find(nSellID);
	if (iter != m_mapSellIDToCardID.end())
	{
		return true;
	}

	return false;
}

// 检查指定时间是否是节假日
bool CSchemePointShop::checkIfHoliday(time_t lTime)
{
	tm sTM = *localtime(&lTime);
	vector<SHolidayFormat>::iterator iter = m_vecHoliday.begin();
	vector<SHolidayFormat>::iterator iterEnd = m_vecHoliday.end();
	for ( ; iter != iterEnd; ++iter)
	{
		if (iter->nYear == sTM.tm_year + 1900 && iter->nMonth == sTM.tm_mon + 1 && iter->nDay == sTM.tm_mday)
			return true;
	}

	return false;
}

bool CSchemePointShop::LoadScheme()
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if (pSchemeEngine == NULL)
	{
		return false;
	}

	string stringPath = SCP_PATH_FORMAT(POINTSHOPGOODS_SCHEME_FILENAME);
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
	if (!bResult)
	{
		ErrorLn("config file load failed filename = " << stringPath.c_str());
		return false;
	}

	stringPath = SCP_PATH_FORMAT(POINTSHOPNUMLIMITPRICE_SCHEME_FILENAME);
	bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
	if (!bResult)
	{
		ErrorLn("config file load failed filename = " << stringPath.c_str());
		return false;
	}

	stringPath = SCP_PATH_FORMAT(POINTSHOPCARDLISTRULE_SCHEME_FILENAME);
	bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
	if (!bResult)
	{
		ErrorLn("config file load failed filename = " << stringPath.c_str());
		return false;
	}

	stringPath = SCP_PATH_FORMAT(POINTSHOPCARDIDTOSELLID_SCHEME_FILENAME);
	bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
	if (!bResult)
	{
		ErrorLn("config file load failed filename = " << stringPath.c_str());
		return false;
	}

	stringPath = SCP_PATH_FORMAT(POINTSHOPHOLIDAYCONFIG_SCHEME_FILENAME);
	bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
	if (!bResult)
	{
		ErrorLn("config file load failed filename = " << stringPath.c_str());
		return false;
	}

	return true;
}

// 关闭
void CSchemePointShop::Close()
{
}

enum EMSchemePointShopGoodsCol
{
	GOODS_SELLID = 0,
	GOODS_NAME,
	GOODS_ISVALID,
	GOODS_SHOWINSHOP,
	GOODS_TYPE,
	GOODS_PRIZEID,
	GOODS_LABEL,
	GOODS_POSITIONWEIGHT,
	GOODS_VIPLVLIMIT,
	GOODS_ACTORLVLIMIT,
	GOODS_VALIDBEGINTIME,
	GOODS_VALIDENDTIME,
	GOODS_NUMPRICEID,
	GOODS_POINTPRICE,
	GOODS_MONEYPRICE,
	GOODS_MONEYUSETYPE,
	GOODS_DISCOUNTRATE,
	GOODS_DISCOUNTBEGINTIME,
	GOODS_DISCOUNTENDTIME,
	GOODS_LIMITNUM,
	GOODS_LIMITTYPE,
	GOODS_LIMITFLUSHTIME,
	GOODS_PLUSDESC,
	GOODS_TOOLTIP,
	GOODS_OPENSERVER,
	GOODS_CONSUMPAPPROACH,

	GOODS_MAX
};

// 加载商城物品配置脚本
bool CSchemePointShop::LoadPointShopGoodsScheme(ICSVReader *pCSVReader, const char *szFileName)
{
    ISchemeActorPrize* pSchemeActorPrize = gServerGlobal->getSchemeCenter()->getSchemeActorPrize();

	m_mapPointShopGoodsConfig.clear();
    
	int nCurZoneID = getThisGameWorldID();		// TODO : 看看怎么获取区ID
	int nRecordCount = (int)pCSVReader->GetRecordCount();
	for (int i = 0; i < nRecordCount; i ++)
	{
		int nIsValid = pCSVReader->GetInt(i, GOODS_ISVALID, 0);
		if (nIsValid <= 0)
			continue;

		char szBuf[256] = {0};
		int nLen = sizeof(szBuf);
		pCSVReader->GetString(i, GOODS_OPENSERVER, szBuf, nLen);
		if (szBuf[0] != '\0')
		{
			// 判断本区是否开放
			int nOpenZone[10] = {0};
			parseIntArray(szBuf, nOpenZone, 10, ';');
			bool bValid = (nOpenZone[0] <= 0) ? true : false;
			for (int j = 0; j < 10; ++j)
			{
				if (nOpenZone[j] == 0)
					break;

				if (nOpenZone[j] < 0)
				{
					if (nCurZoneID == -nOpenZone[j])
					{
						bValid = false;
						break;
					}
				}
				else
				{
					if (nCurZoneID == nOpenZone[j])
					{
						bValid = true;
						break;
					}
				}
			}

			if (!bValid)
				continue;
		}

		SPointShopGoodsConfig_S goodsInfo;
		goodsInfo.nSellID = pCSVReader->GetInt(i, GOODS_SELLID, 0);     // 出售ID
        if(goodsInfo.nSellID < 0 || goodsInfo.nSellID >= USHRT_MAX)
        {
            ErrorLn(__FUNCTION__": SellID is illegal digital, check config file["<< POINTSHOPGOODS_SCHEME_FILENAME <<"] ! sellID="<<goodsInfo.nSellID);
            return false;
        }
		if (m_mapPointShopGoodsConfig.find(goodsInfo.nSellID) != m_mapPointShopGoodsConfig.end())
		{
			ErrorLn(__FUNCTION__" id repeat " << goodsInfo.nSellID);
			return false;
		}

		goodsInfo.nShopGoodsType = pCSVReader->GetInt(i, GOODS_TYPE, 0);    // 商城物品类型 EMPointShopGoodsType
		if (goodsInfo.nShopGoodsType <= POINTSHOP_GOODSTYPE_INVALID || goodsInfo.nShopGoodsType >= POINTSHOP_GOODSTYPE_MAX)
		{
			ErrorLn(__FUNCTION__" goodsInfo.nShopGoodsType invalid sellID = " << goodsInfo.nSellID);
			return false;
		}

		goodsInfo.nPrizeID          = pCSVReader->GetInt(i, GOODS_PRIZEID, 0);      // 奖励ID
        // 校验奖励ID是否合法
        if (goodsInfo.nPrizeID != 0 && !pSchemeActorPrize->isExists(goodsInfo.nPrizeID))
        {
            ErrorLn(__FUNCTION__" prize id is invalid ! prizeID="<< goodsInfo.nPrizeID <<",check file="<< szFileName <<",row="<< (i+1) <<",sellID = " << goodsInfo.nSellID);
            return false;
        }

		goodsInfo.nNeedVipLevel     = pCSVReader->GetInt(i, GOODS_VIPLVLIMIT, 0);   // 需要VIP等级

		int nLevelLimit[2] = {0};
		memset(szBuf, 0, sizeof(szBuf));
		nLen = sizeof(szBuf);
		pCSVReader->GetString(i, GOODS_ACTORLVLIMIT, szBuf, nLen);
		parseIntArray(szBuf, nLevelLimit, 2, ';');
		goodsInfo.nLevelLimitMin = nLevelLimit[0];      // 最低等级限制
		goodsInfo.nLevelLimitMax = nLevelLimit[1];      // 最高等级限制
		if (goodsInfo.nLevelLimitMax <= 0)	goodsInfo.nLevelLimitMax = 999;

		goodsInfo.nBeginSellTime    = 0;	                                        // 开始时间	// 时间格式再想想
		goodsInfo.nEndSellTime      = 0;		                                    // 结束时间 // 时间格式再想想
		goodsInfo.nNumPriceID       = pCSVReader->GetInt(i, GOODS_NUMPRICEID, 0);   // 动态变更价格ID
		goodsInfo.nPointPrice       = pCSVReader->GetInt(i, GOODS_POINTPRICE, 0);   // 点券价格
		goodsInfo.nMoneyPrice       = pCSVReader->GetInt(i, GOODS_MONEYPRICE, 0);   // 金币价格
		goodsInfo.nMoneyUseType     = pCSVReader->GetInt(i, GOODS_MONEYUSETYPE, 0); // 金币使用类型
		goodsInfo.nDiscountRate     = pCSVReader->GetInt(i, GOODS_DISCOUNTRATE, 0); // 折扣率
		goodsInfo.nDiscountBeginTime= 0;		                                    // 折扣开始时间 // 时间格式再想想
		goodsInfo.nDiscountEndTime  = 0;		                                    // 折扣结束时间 // 时间格式再想想
		//goodsInfo.szSellName = pCSVReader->GetString(i, GOODS_NAME, "");			// 商品名称
		nLen = sizeof(goodsInfo.szSellName);
		pCSVReader->GetString(i, GOODS_NAME, goodsInfo.szSellName, nLen);
		goodsInfo.nConsumpApproach	= pCSVReader->GetInt(i, GOODS_CONSUMPAPPROACH, 0);
		m_mapPointShopGoodsConfig[goodsInfo.nSellID] = goodsInfo;
	}

	return true;
}

// 加载物品随数量变动价格配置脚本
bool CSchemePointShop::LoadPointShopNumLimitPriceScheme(ICSVReader *pCSVReader, const char *szFileName)
{
	m_mapPointShopNumLimitPriceConfig.clear();
    m_mapSellLimitMax.clear();

	int nRecordCount = (int)pCSVReader->GetRecordCount();
	for (int i = 0; i < nRecordCount; i ++)
	{
		int nLimitID = pCSVReader->GetInt(i, 0, 0);                 // 限制价格ID

		SPointShopNumLimitPriceSub subInfo;
		subInfo.nLimitNum        = pCSVReader->GetInt(i, 1, 0);     // 到下一段为止所需数量
		subInfo.nLimitPointPrice = pCSVReader->GetInt(i, 2, 0);     // 点券价格
		subInfo.nLimitMoneyPrice = pCSVReader->GetInt(i, 3, 0);     // 金币价格

        if(subInfo.nLimitNum <= 0)
        {
            ErrorLn(__FUNCTION__": limit num is invalid digit!! limitNum="<<subInfo.nLimitNum<<", row="<< (i+1) );
            return false;
        }
        if(subInfo.nLimitPointPrice<=0 && subInfo.nLimitMoneyPrice <= 0)
        {
            ErrorLn(__FUNCTION__": limit price is invalid digit!! ticket="<<subInfo.nLimitPointPrice<<", Money="<< subInfo.nLimitMoneyPrice <<", row="<< (i+1) );
            return false;
        }

        // 更新下最新的最大可购买数量
        m_mapSellLimitMax[nLimitID] = subInfo.nLimitNum;

		map<int, SPointShopNumLimitPrice>::iterator iter = m_mapPointShopNumLimitPriceConfig.find(nLimitID);
		if (iter != m_mapPointShopNumLimitPriceConfig.end())
		{
			SPointShopNumLimitPrice &configInfo = iter->second;
			int nCurSize = configInfo.vecLimitStep.size();
			if (nCurSize > 0)
			{
				int nLastLimitNum = configInfo.vecLimitStep[nCurSize - 1].nLimitNum;
				if (nLastLimitNum >= subInfo.nLimitNum)
				{
					ErrorLn(__FUNCTION__" ShopNumLimitPrice.csv  nLastLimitNum = " << nLastLimitNum <<", row="<< (i+1) <<",col=2");
					return false;
				}
			}
			configInfo.vecLimitStep.push_back(subInfo);
		}
		else
		{
			SPointShopNumLimitPrice configInfo;
			configInfo.nLimitPriceID = nLimitID;
			configInfo.vecLimitStep.push_back(subInfo);
			m_mapPointShopNumLimitPriceConfig[nLimitID] = configInfo;
		}
	}

	return true;
}

// 加载卡牌随机规则配置脚本
bool CSchemePointShop::LoadPointShopNumCardListRuleScheme(ICSVReader *pCSVReader, const char *szFileName)
{
	m_vecRandomCardListRule.clear();

	int nLastNeedHeroNum = 0;
	int nRecordCount = (int)pCSVReader->GetRecordCount();
	for (int i = 0; i < nRecordCount; i ++)
	{
		SPointShopCardListRule config;
		int nIndex = 0;
		config.nNeedHeroNum = pCSVReader->GetInt(i, nIndex++, 0);   // 可用英雄数量（需要英雄数量）
		if (config.nNeedHeroNum <= nLastNeedHeroNum)
		{
			ErrorLn(__FUNCTION__" PointShopCardListRule.csv  nNeedHeroNum = " << config.nNeedHeroNum);
			return false;
		}
		nLastNeedHeroNum = config.nNeedHeroNum; 

		for (int j = 0; j < POINTSHOP_CARDLISTRULE_MAXCOMBINECOUNT; ++j)
		{
			config.nCombineFlag[j]  = pCSVReader->GetInt(i, nIndex++, 0);   // 组合类型EMCardlistRuleCombineFlag(1已拥有英雄；2周免英雄；4不可使用英雄)
			config.nHeroNum[j]      = pCSVReader->GetInt(i, nIndex++, 0);   // 随机数量
			config.nHeroNumEx[j]    = pCSVReader->GetInt(i, nIndex++, 0);   // 节日随机数量
			config.nPassUsedHero[j] = pCSVReader->GetInt(i, nIndex++, 0);   // 是否排除已随机过的
		}

		config.nResetUsedHeroDay    = pCSVReader->GetInt(i, nIndex++, 0);   // 已随机英雄记录重置时间(商店数据清空时间)

		m_vecRandomCardListRule.push_back(config);
	}

	return true;
}

// 加载卡牌ID对应出售ID配置脚本
bool CSchemePointShop::LoadPointShopCardIDToSellIDScheme(ICSVReader *pCSVReader, const char *szFileName)
{
	m_mapCardIDToSellID.clear();
	int nRecordCount = (int)pCSVReader->GetRecordCount();
	for (int i = 0; i < nRecordCount; i ++)
	{
		int nCardID = pCSVReader->GetInt(i, 0, 0);
		int nSellID = pCSVReader->GetInt(i, 1, 0);
		if (m_mapCardIDToSellID.find(nCardID) != m_mapCardIDToSellID.end())
		{
			return false;
		}

		m_mapCardIDToSellID[nCardID] = nSellID;
		m_mapSellIDToCardID[nSellID] = nCardID;
	}

	return true;
}

// 加载节假日配置脚本
bool CSchemePointShop::LoadHolidayConfigScheme(ICSVReader *pCSVReader, const char *szFileName)
{
	m_vecHoliday.clear();
	int nRecordCount = (int)pCSVReader->GetRecordCount();
	for (int i = 0; i < nRecordCount; i ++)
	{
		char szBuf[256] = {0};
		int nLen = sizeof(szBuf);
		pCSVReader->GetString(i, 0, szBuf, nLen);
		int nFormat[3] = {0};
		parseIntArray(szBuf, nFormat, 3, '/');

		SHolidayFormat holiday;
		holiday.nYear = nFormat[0];
		holiday.nMonth = nFormat[1];
		holiday.nDay = nFormat[2];
		m_vecHoliday.push_back(holiday);
	}

	return true;
}

// 获得某个类型的商品列表
bool CSchemePointShop::getPointShopSellConfigList(EMPointShopGoodsType eType,vector<SPointShopGoodsConfig_S*> &listSellConfig)
{
	map<int, SPointShopGoodsConfig_S>::iterator iter = m_mapPointShopGoodsConfig.begin();
	for (;iter!=m_mapPointShopGoodsConfig.end();++iter)
	{
		if(iter->second.nShopGoodsType == (int)eType)
		{
			listSellConfig.push_back(&(iter->second));
		}
	}
	
	return true;
}