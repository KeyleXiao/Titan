/*******************************************************************
** �ļ���:	SchemeHeroReliveTime.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/5/10
** ��  ��:	1.0
** ��  ��:
Ӣ�۸���ʱ��
********************************************************************/


#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeHeroReliveTime.h"

CSchemeHeroReliveTime::CSchemeHeroReliveTime(void)
{

}

CSchemeHeroReliveTime::~CSchemeHeroReliveTime(void)
{

}

// ����ű�
bool CSchemeHeroReliveTime::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // ���ؽű�
    string stringPath = SCP_PATH_FORMAT(HERORELIVETIME_SCHEME_FILENAME);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// �ر�
void CSchemeHeroReliveTime::Close(void)
{
    m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV��������ʱ֪ͨ
bool CSchemeHeroReliveTime::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapScheme.clear();

    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
		SHeroReliveTime node;
		
		node.nConfigWarID = pCSVReader->GetInt(nRow, COLNUM_ConfigWarID, 0);
		node.nLevel = pCSVReader->GetInt(nRow, COLNUM_Level, 0);
		node.nReliveTime = pCSVReader->GetInt(nRow, COLNUM_ReliveTime, 0) * 1000;		// ת��Ϊ����

		m_mapScheme[node.nConfigWarID][node.nLevel] = node.nReliveTime;
    }

	AfterSchemeLoaded();

    return true;
}

	
// ���ö�̬����ʱ֪ͨ
bool CSchemeHeroReliveTime::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeHeroReliveTime* pNewInstance = NewInstance();
    if (pNewInstance == NULL)
    {
        return false;
    }

    if (pNewInstance->OnSchemeLoad(reader, szFileName))
    {
        return true;
    }
    else
    {
        safeDelete(pNewInstance);
        return false;
    }
}

int CSchemeHeroReliveTime::getHeroReliveTime(int nConfigWarID, int nLevel)
{
	HERO_RELIVETIME_MAP_ITER it = m_mapScheme.find(nConfigWarID);
	if (it == m_mapScheme.end())
	{
		return 0;
	}

	map<int, int>::iterator it2 = it->second.find(nLevel);
	if (it2 == it->second.end())
	{
		return 0;
	}

	return it2->second;
}