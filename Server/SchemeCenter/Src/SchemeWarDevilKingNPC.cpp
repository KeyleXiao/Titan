/*******************************************************************
** �ļ���:	SchemeWarDevilKingNPC.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/11/30
** ��  ��:	1.0
** ��  ��:   ս��ħ������NPC����
********************************************************************/


#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeWarDevilKingNPC.h"
#include "SchemeWarDef.h"

CSchemeWarDevilKingNPC::CSchemeWarDevilKingNPC(void)
{

}

CSchemeWarDevilKingNPC::~CSchemeWarDevilKingNPC(void)
{

}

// ����ű�
bool CSchemeWarDevilKingNPC::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // ���ؽű�
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_DEVILKINGNPC);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// �ر�
void CSchemeWarDevilKingNPC::Close(void)
{
	m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV��������ʱ֪ͨ
bool CSchemeWarDevilKingNPC::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;


    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
		SWarDevilKingNPC node;

		int nCol = 0;
		node.nHeroID = pCSVReader->GetInt(nRow, nCol++, 0);
		node.nMonsterID = pCSVReader->GetInt(nRow, nCol++, 0);
		node.nMonsterBuff = pCSVReader->GetInt(nRow, nCol++, 0);

		m_mapScheme[node.nHeroID] = node;
    }

	AfterSchemeLoaded();

    return true;
}

	
// ���ö�̬����ʱ֪ͨ
bool CSchemeWarDevilKingNPC::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeWarDevilKingNPC* pNewInstance = NewInstance();
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

SWarDevilKingNPC * CSchemeWarDevilKingNPC::getWarDevilKingNPC(int nHeroID)
{
	auto iter = m_mapScheme.find(nHeroID);
	if (iter == m_mapScheme.end())
	{
		return NULL;
	}

	return &iter->second;
}

