#include "StdAfx.h"
#include "SchemePrizeChest.h"
#include "IServerGlobal.h"

bool CSchemePrizeChest::OnSchemeLoad(SCRIPT_READER reader, const char *szFileName)
{
	ICSVReader *pCSVReader = reader.pCSVReader;
	if (pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
	{
		return false;
	}

	bool bRet = false;

	if ( rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(PRIZECHEST_SCHEME_FILENAME)) == 0)
	{
		bRet = LoadPrizeChestScheme(pCSVReader,szFileName);
	}

	if (!bRet)
	{
		return false;
	}	

	AfterSchemeLoaded();

	return true;
}

bool CSchemePrizeChest::OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}

// ��ñ���������Ϣ
SPrizeChestConfig_S* CSchemePrizeChest::getPrizeChestConfig(int nChestID)
{
	map<int, SPrizeChestConfig_S>::iterator iter = m_mapPrizeChestConfig.find(nChestID);
	if (iter != m_mapPrizeChestConfig.end())
		return &(*iter).second;
	return NULL;
}

bool CSchemePrizeChest::LoadScheme()
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if (pSchemeEngine == NULL)
	{
		return false;
	}

	string stringPath = SCP_PATH_FORMAT(PRIZECHEST_SCHEME_FILENAME);
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
	if (!bResult)
	{
		ErrorLn("config file load failed filename = " << stringPath.c_str());;
		return false;
	}

	return true;
}

// �ر�
void CSchemePrizeChest::Close()
{
}

enum EMSchemePrizeChestCol
{
	CHEST_ID = 0,
	CHEST_FLAG,
	CHEST_NAME,
	CHEST_SHOWPRIZEID,
	CHEST_PRIZEGROUPID = 17,

	CHEST_MAX
};

// ���ؽ�������ű�
bool CSchemePrizeChest::LoadPrizeChestScheme(ICSVReader *pCSVReader,const char *szFileName)
{
	m_mapPrizeChestConfig.clear();

	int nRecordCount = (int)pCSVReader->GetRecordCount();
	for (int i = 0; i < nRecordCount; i ++)
	{
		SPrizeChestConfig_S chestInfo;
		// ����ID
		chestInfo.nChestID = pCSVReader->GetInt(i, CHEST_ID, 0);
		chestInfo.nChestFlag = pCSVReader->GetInt(i, CHEST_FLAG, 0);
		int nLen = sizeof(chestInfo.szChestName);
		pCSVReader->GetString(i, CHEST_NAME, chestInfo.szChestName, nLen);
		chestInfo.nShowPrizeID = pCSVReader->GetInt(i, CHEST_SHOWPRIZEID, 0);
		// Ŀǰ���֧��1-100�ţ��ټ�Ҫ��Ӧ�޸Ľ�������
		if (chestInfo.nChestID <= 0 || chestInfo.nChestID > PRIZECHEST_TYPE_MAX || m_mapPrizeChestConfig.find(chestInfo.nChestID) != m_mapPrizeChestConfig.end())
		{
			ErrorLn(__FUNCTION__" nChestID = " << chestInfo.nChestID);
			return false;
		}

		// ������ID
		chestInfo.nPrizeGroupID = pCSVReader->GetInt(i, CHEST_PRIZEGROUPID, 0);

		m_mapPrizeChestConfig[chestInfo.nChestID] = chestInfo;
	}

	return true;
}