#include "StdAfx.h"
#include "SchemeBenefitCard.h"
#include "IServerGlobal.h"

bool CSchemeBenefitCard::OnSchemeLoad(SCRIPT_READER reader, const char *szFileName)
{
	ICSVReader *pCSVReader = reader.pCSVReader;
	if (pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
	{
		return false;
	}

	bool bRet = false;

	if ( rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(BENEFITCARD_SCHEME_FILENAME)) == 0)
	{
		bRet = LoadBenefitCardScheme(pCSVReader,szFileName);
	}

	if (!bRet)
	{
		return false;
	}	

	AfterSchemeLoaded();

	return true;
}

bool CSchemeBenefitCard::OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}

// 获得宝箱配置信息
SBenefitCardConfig_S* CSchemeBenefitCard::getBenefitCardConfig(int nCardID)
{
	map<int, SBenefitCardConfig_S>::iterator iter = m_mapBenefitCardConfig.find(nCardID);
	if (iter != m_mapBenefitCardConfig.end())
		return &(*iter).second;
	return NULL;
}

bool CSchemeBenefitCard::LoadScheme()
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if (pSchemeEngine == NULL)
	{
		return false;
	}

	string stringPath = SCP_PATH_FORMAT(BENEFITCARD_SCHEME_FILENAME);
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
	if (!bResult)
	{
		ErrorLn("config file load failed filename = " << stringPath.c_str());;
		return false;
	}

	return true;
}

// 关闭
void CSchemeBenefitCard::Close()
{
}

enum EMBenefitCardCol
{
	BENEFITCARD_ID = 0,
	BENEFITCARD_DESC,
	BENEFITCARD_TYPE,
	BENEFITCARD_SUBTYPE,
	BENEFITCARD_VALUE,
	BENEFITCARD_MULIPLE,
	BENEFITCARD_PRICEID,

	BENEFITCARD_MAX,
};

// 加载奖励宝箱脚本
bool CSchemeBenefitCard::LoadBenefitCardScheme(ICSVReader *pCSVReader,const char *szFileName)
{
	m_mapBenefitCardConfig.clear();

	int nRecordCount = (int)pCSVReader->GetRecordCount();
	for (int i = 0; i < nRecordCount; i ++)
	{
		SBenefitCardConfig_S cardInfo;

		// 增益卡属性
		cardInfo.nBenefitCardID = pCSVReader->GetInt(i, BENEFITCARD_ID, 0);
		int nLen = sizeof(cardInfo.szBenefitCardDesc);
		pCSVReader->GetString(i, BENEFITCARD_DESC, cardInfo.szBenefitCardDesc, nLen);
		cardInfo.nBenefitCardType = pCSVReader->GetInt(i, BENEFITCARD_TYPE, 0);
		cardInfo.nBenefitCardSubType = pCSVReader->GetInt(i, BENEFITCARD_SUBTYPE, 0);
		cardInfo.nBenefitValue = pCSVReader->GetInt(i, BENEFITCARD_VALUE, 0);
		cardInfo.nMultiple = pCSVReader->GetInt(i, BENEFITCARD_MULIPLE, 0);
		cardInfo.nPriceID = pCSVReader->GetInt(i, BENEFITCARD_PRICEID, 0);

		m_mapBenefitCardConfig[cardInfo.nBenefitCardID] = cardInfo;
	}

	return true;
}