/*******************************************************************
** �ļ���:	SchemeHeroCardCreate.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2016/8/3
** ��  ��:	1.0
** ��  ��:	

********************************************************************/
#include "StdAfx.h"
#include "SchemeHeroCardCreat.h"
#include "IServerGlobal.h"
#include "SchemeDef.h"

bool CSchemeHeroCardCreat::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    bool bResult = false;
    string stHeroCardCreatPro = SCP_PATH_FORMAT(HEROCARDCREATPRO_SCHEME_FILENAME);
    string stHeroCardCreatCount  = SCP_PATH_FORMAT(HEROCARDCREATCOUNT_SCHEME_FILENAME);

    if ( rkt::StringHelper::casecmp( szFileName, stHeroCardCreatPro.c_str()) == 0 )
    {
        bResult = loadHeroCardCreatProScheme(reader);
    }else if ( rkt::StringHelper::casecmp(szFileName, stHeroCardCreatCount.c_str()) == 0 )
    {
        bResult = loadHeroCardCreatCountScheme(reader);
    }

    return true;
}

bool CSchemeHeroCardCreat::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader, szFileName);
}

SHeroCardCreatPro* CSchemeHeroCardCreat::getHeroCardCreatPro(int nPlayerLV)
{
    std::map<int, SHeroCardCreatPro>::iterator it = m_mapHeroCardCreatePro.find( nPlayerLV);
    if ( it == m_mapHeroCardCreatePro.end() )
    {
        ErrorLn(__FUNCTION__": can't find nPlayerLv = " << nPlayerLV << " HeroCardCreatePro config");
        return NULL;
    }
    return &it->second;
}

SHeroCardCreatCount* CSchemeHeroCardCreat::getHeroCardCreateCount(int nPlayerLV, int nWarType)
{
    MAPHEROCARDCREATCOUNT::iterator it = m_mapHeroCardCreateCount.find( nPlayerLV );
    if ( it == m_mapHeroCardCreateCount.end() )
    {
        ErrorLn(__FUNCTION__": can't find nPlayerLv = " << nPlayerLV << ",HeroCardCreateCount config");
        return NULL;
    }
    std::map<int, SHeroCardCreatCount>* pInfo = &it->second;

    std::map<int, SHeroCardCreatCount>::iterator it2 = pInfo->find(nWarType);
    if ( it2 == pInfo->end())
    {
        ErrorLn(__FUNCTION__": can't find nWarType = " << nWarType << ", HeroCardCreateCount config");
        return NULL;
    }
    return &it2->second;
}

bool CSchemeHeroCardCreat::LoadScheme()
{
    ISchemeEngine* pSchemeEngine = gServerGlobal->getSchemeEngine();
    if ( pSchemeEngine == NULL)
    {
        return false;
    }

    // ����HeroCardCreatPro.csv�ű�
    string stringPath = SCP_PATH_FORMAT( HEROCARDCREATPRO_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*) this );
    if ( !bResult )
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());
        return false;
    }

    // ����HeroCardCreatCount.csv�ű�
    stringPath = SCP_PATH_FORMAT(HEROCARDCREATCOUNT_SCHEME_FILENAME);
    bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*) this );
    if ( !bResult )
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());
        return false;
    }
    AfterSchemeLoaded();
    return true;
}

// �ر�
void CSchemeHeroCardCreat::Close()
{
    // ���
    m_mapHeroCardCreatePro.clear();
    m_mapHeroCardCreateCount.clear();
}

bool CSchemeHeroCardCreat::loadHeroCardCreatProScheme(SCRIPT_READER reader)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if ( pCSVReader==0 || reader.type!=READER_CSV)
		return false;

	// ���
	m_mapHeroCardCreatePro.clear();

	int nLen = 0;
	// ��ȡ
	int nRecordCount = pCSVReader->GetRecordCount();

	for ( int nRow = 0; nRow < nRecordCount; ++nRow )
	{
		int nColumnIndex = 0;
		SHeroCardCreatPro item;
		item.nPlayerLv = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nMainHeroPro = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nTeamHeroPro = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nPrivateHeroPro = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nWeekFreePro = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		m_mapHeroCardCreatePro[item.nPlayerLv] = item;
	}
	return true;
}

bool CSchemeHeroCardCreat::loadHeroCardCreatCountScheme(SCRIPT_READER reader)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if ( pCSVReader==0 || reader.type!=READER_CSV)
        return false;

    // ���
    m_mapHeroCardCreateCount.clear();

    int nLen = 0;
    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();

    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
        int nColumnIndex = 0;
        SHeroCardCreatCount item;
        item.nPlayerLv = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nWarType = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nDayNum = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nWeekNum = pCSVReader->GetInt(nRow, nColumnIndex++, 0);

        for ( int i = 0 ; i < HEROCARD_NUM_TYPE; ++i )
        {
            item.sCardCount[i].nCardCount = pCSVReader->GetInt( nRow, nColumnIndex++, 0);
            item.sCardCount[i].nPro = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        }

        MAPHEROCARDCREATCOUNT::iterator it = m_mapHeroCardCreateCount.find(item.nPlayerLv);
        if ( it == m_mapHeroCardCreateCount.end() )
        {
            std::map<int, SHeroCardCreatCount> mapHeroCardCreatCount;
            mapHeroCardCreatCount[item.nWarType] = item;
            m_mapHeroCardCreateCount[item.nPlayerLv] = mapHeroCardCreatCount;
        }else
        {
            std::map<int, SHeroCardCreatCount>& mapHeroCardCreatCount = it->second;
            mapHeroCardCreatCount[item.nWarType] = item;
        }
    }
    return true;
}