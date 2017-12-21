#include "StdAfx.h"
#include "SchemeActorPrize.h"
#include "IServerGlobal.h"

bool CSchemeActorPrize::OnSchemeLoad(SCRIPT_READER reader, const char *szFileName)
{
	ICSVReader *pCSVReader = reader.pCSVReader;
	if (pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
	{
		return false;
	}

	bool bRet = false;

	if (rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(ACTORPRIZE_SCHEME_FILENAME)) == 0)
	{
		bRet = LoadActorPrizeScheme(pCSVReader,szFileName);
	}
	else if (rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(ACTORPRIZEGROUP_SCHEME_FILENAME)) == 0)
	{
		bRet = LoadActorPrizeGroupScheme(pCSVReader,szFileName);
	}
	else if (rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(ACTORPRIZEEXCHANGE_SCHEME_FILENAME)) == 0)
	{
		bRet = LoadActorPrizeExchangeScheme(pCSVReader,szFileName);
	}

	if (!bRet)
	{
		return false;
	}	

	AfterSchemeLoaded();

	return true;
}

bool CSchemeActorPrize::OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}

// ��ý���������Ϣ
SActorPrizeConfig_S* CSchemeActorPrize::getActorPrizeConfig(int nPrizeID)
{
	map<int, SActorPrizeConfig_S>::iterator iter = m_mapActorPrizeConfig.find(nPrizeID);
	if (iter != m_mapActorPrizeConfig.end())
		return &(*iter).second;
	return NULL;
}

// ��ý�����������Ϣ
SActorPrizeGroupConfig* CSchemeActorPrize::getActorPrizeGroupConfig(int nPrizeGroupID)
{
	map<int, SActorPrizeGroupConfig>::iterator iter = m_mapActorPrizeGroupConfig.find(nPrizeGroupID);
	if (iter != m_mapActorPrizeGroupConfig.end())
		return &(*iter).second;
	return NULL;
}

// ��ý���ת��������Ϣ
SActorPrizeExchangeConfig* CSchemeActorPrize::getPrizeExchangeConfig(int nPrizeID)
{
	map<int, SActorPrizeExchangeConfig>::iterator iter = m_mapActorPrizeExchangeConfig.find(nPrizeID);
	if (iter != m_mapActorPrizeExchangeConfig.end())
		return &(*iter).second;
	return NULL;
}

// У���Ƿ���ڴ˽���ID
bool CSchemeActorPrize::isExists(int nPrizeID)
{
    map<int, SActorPrizeConfig_S>::iterator iter = m_mapActorPrizeConfig.find(nPrizeID);
    return iter != m_mapActorPrizeConfig.end();
}

bool CSchemeActorPrize::LoadScheme()
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if (pSchemeEngine == NULL)
	{
		return false;
	}

	string stringPath = SCP_PATH_FORMAT(ACTORPRIZE_SCHEME_FILENAME);
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
	if (!bResult)
	{
		ErrorLn("config file load failed filename = " << stringPath.c_str());;
		return false;
	}

	stringPath = SCP_PATH_FORMAT(ACTORPRIZEGROUP_SCHEME_FILENAME);
	bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
	if (!bResult)
	{
		ErrorLn("config file load failed filename = " << stringPath.c_str());;
		return false;
	}

	stringPath = SCP_PATH_FORMAT(ACTORPRIZEEXCHANGE_SCHEME_FILENAME);
	bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
	if (!bResult)
	{
		ErrorLn("config file load failed filename = " << stringPath.c_str());;
		return false;
	}

	return true;
}

// �ر�
void CSchemeActorPrize::Close()
{
}

enum EMSchemeActorPrizeCol
{
	ACTOR_PRIZE_ID = 0,
	ACTOR_PRIZE_NAME,
	ACTOR_PRIZE_ICON,
	ACTOR_PRIZE_GRADE,
	ACTOR_PRIZE_DESC,
	ACTOR_PRIZE_BROADCASTFLAG,
	ACTOR_PRIZE_TYPE,
	ACTOR_PRIZE_CAKETRAILEFFECT,
	ACTOR_PRIZE_PEDESTALEFFECT,
	ACTOR_PRIZE_PEDESTALGLOWEFFECT,
	ACTOR_PRIZE_PARAM_START,

	ACTOR_PRIZE_MAX
};

// ���ؽ�ɫ�����ű�
bool CSchemeActorPrize::LoadActorPrizeScheme(ICSVReader *pCSVReader,const char *szFileName)
{
	m_mapActorPrizeConfig.clear();

	int nRecordCount = (int)pCSVReader->GetRecordCount();
	for (int i = 0; i < nRecordCount; i++)
	{
		SActorPrizeConfig_S prizeInfo;
		// ����ID
		prizeInfo.nPrizeID = pCSVReader->GetInt(i, ACTOR_PRIZE_ID, 0);
		if (prizeInfo.nPrizeID <= 0 || m_mapActorPrizeConfig.find(prizeInfo.nPrizeID) != m_mapActorPrizeConfig.end())
		{
			ErrorLn(__FUNCTION__" nPrizeID = " << prizeInfo.nPrizeID);
			return false;
		}
		// ��������
		int nLen = sizeof(prizeInfo.szPrizeName);
		pCSVReader->GetString(i, ACTOR_PRIZE_NAME, prizeInfo.szPrizeName, nLen);

		prizeInfo.nGrade = pCSVReader->GetInt(i, ACTOR_PRIZE_GRADE, 0);
        // �㲥��ʶ EMPrizeBroadcastFlagType
		prizeInfo.nBroadcastFlag = pCSVReader->GetInt(i, ACTOR_PRIZE_BROADCASTFLAG, 0);
		// �������� EMActorPrizeType
		prizeInfo.nPrizeType = pCSVReader->GetInt(i, ACTOR_PRIZE_TYPE, 0);
		// ��������
		for (int j = 0; j < ACTORPRIZE_PARAMMAX; ++j)
		{
			prizeInfo.nPrizeParam[j] = pCSVReader->GetInt(i, ACTOR_PRIZE_PARAM_START + j, 0);
		}

		m_mapActorPrizeConfig[prizeInfo.nPrizeID] = prizeInfo;
	}

	return true;
}

enum EMSchemeActorPrizeGroupCol
{
	ACTOR_PRIZEGROUP_ID = 0,
	ACTOR_PRIZEGROUP_BROADCASTFLAG,
	ACTOR_PRIZEGROUP_BROADCASTGRADE,
	ACTOR_PRIZEGROUP_CONSTPRIZE,
	ACTOR_PRIZEGROUP_RANDPRIZE_COUNT,
	ACTOR_PRIZEGROUP_RANDPRIZE_START,

	ACTOR_PRIZEGROUP_MAX
};

// ���ؽ�ɫ������ű�
bool CSchemeActorPrize::LoadActorPrizeGroupScheme(ICSVReader *pCSVReader,const char *szFileName)
{
	m_mapActorPrizeGroupConfig.clear();

	int nRecordCount = (int)pCSVReader->GetRecordCount();
	for (int i = 0; i < nRecordCount; i++)
	{
		SActorPrizeGroupConfig prizeGroupInfo;
		// ������ID
		prizeGroupInfo.nGroupID = pCSVReader->GetInt(i, ACTOR_PRIZEGROUP_ID, 0);
		if (prizeGroupInfo.nGroupID <= 0 || m_mapActorPrizeGroupConfig.find(prizeGroupInfo.nGroupID) != m_mapActorPrizeGroupConfig.end())
		{
			ErrorLn(__FUNCTION__ << " nGroupID = " << prizeGroupInfo.nGroupID);
			return false;
		}

		// �㲥���
		prizeGroupInfo.nBroadcastFlag = pCSVReader->GetInt(i, ACTOR_PRIZEGROUP_BROADCASTFLAG, 0);
		prizeGroupInfo.nBroadcastGrade = pCSVReader->GetInt(i, ACTOR_PRIZEGROUP_BROADCASTGRADE, 0);

		// �̶�����
		char szBuf[256] = {0};
		int nLen = sizeof(szBuf);
		pCSVReader->GetString(i, ACTOR_PRIZEGROUP_CONSTPRIZE, szBuf, nLen);
		int nArrayCount = parseIntArray(szBuf, prizeGroupInfo.nConstantPrize, ACTORPRIZEGROUP_SINGLEGROUPMAX, ';');
        // У�齱���Ƿ�Ϸ�
        if(!checkPrizeForArray(szFileName, i, prizeGroupInfo.nConstantPrize, nArrayCount))
        {
            return false;
        }

		// �����������
		prizeGroupInfo.nRandomPrizeCount = pCSVReader->GetInt(i, ACTOR_PRIZEGROUP_RANDPRIZE_COUNT, 0);
		if (prizeGroupInfo.nRandomPrizeCount > 0)
		{
			// �������
			int nCurRandomRate = 0;
			for (int j = 0; j < ACTORPRIZEGROUP_RANDOMPRIZEMAX; ++j)
			{
				int nRandomRate = pCSVReader->GetInt(i, ACTOR_PRIZEGROUP_RANDPRIZE_START + (j * 2) + 1, 0);
				if (nRandomRate == 0)
					break;

				nCurRandomRate += nRandomRate;
				int nLen = sizeof(szBuf);
				pCSVReader->GetString(i, ACTOR_PRIZEGROUP_RANDPRIZE_START + (j * 2), szBuf, nLen);
				nArrayCount = parseIntArray(szBuf, prizeGroupInfo.nRandomPrizeID[j], ACTORPRIZEGROUP_SINGLEGROUPMAX, ';');
                // У�齱���Ƿ�Ϸ�
                if(!checkPrizeForArray(szFileName, i, prizeGroupInfo.nRandomPrizeID[j], nArrayCount))
                {
                    return false;
                }

				if (prizeGroupInfo.nRandomPrizeID[j][0] == 0)
				{
					ErrorLn(__FUNCTION__" nRandomPrizeID == 0  nGroupID = " << prizeGroupInfo.nGroupID);
					return false;
				}

				prizeGroupInfo.nRandomPrizeRate[j] = nCurRandomRate;
			}

			if (nCurRandomRate != 1000000)
			{
				ErrorLn(__FUNCTION__" nRandomRate = 0" << nCurRandomRate << "  nGroupID = " << prizeGroupInfo.nGroupID);
				return false;
			}
		}

		m_mapActorPrizeGroupConfig[prizeGroupInfo.nGroupID] = prizeGroupInfo;
	}

	return true;
}


// ���ؽ�ɫ����ת���ű�
bool CSchemeActorPrize::LoadActorPrizeExchangeScheme(ICSVReader *pCSVReader,const char *szFileName)
{
	m_mapActorPrizeExchangeConfig.clear();

	int nRecordCount = (int)pCSVReader->GetRecordCount();
	for (int i = 0; i < nRecordCount; i++)
	{
		SActorPrizeExchangeConfig info;
		int nIndex = 0;
		// ����ID
		info.nPrizeID = pCSVReader->GetInt(i, nIndex++, 0);
		if (info.nPrizeID <= 0 || m_mapActorPrizeExchangeConfig.find(info.nPrizeID) != m_mapActorPrizeExchangeConfig.end())
		{
			ErrorLn(__FUNCTION__" nPrizeID = " << info.nPrizeID);
			return false;
		}

		// ת������ID
		info.nExchangePrizeID = pCSVReader->GetInt(i, nIndex++, 0);

		m_mapActorPrizeExchangeConfig[info.nPrizeID] = info;
	}

	return true;
}


// �ڲ�У�齱�������Ƿ�Ϸ�
inline bool CSchemeActorPrize::checkPrizeForArray(const char* szFileName, int nRow, int * pArray, int nCount)
{
    for(int i=0; i<nCount; ++i)
    {
        int &nPrizeID = pArray[i];
        if(!isExists(nPrizeID))
        {
            ErrorLn(__FUNCTION__" prize id is invalid ! prizeID="<< nPrizeID <<",check file="<< szFileName <<",row="<< (nRow+1) );
            return false;
        }
    }
    return true;
}