/*******************************************************************
** �ļ���:	SchemeWarGodRecommdEquip.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/4/17
** ��  ��:	1.0
** ��  ��:
			�����Ƽ�װ�ű�
********************************************************************/

#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeWarGodRecommdEquip.h"
#include "SchemeWarDef.h"

CSchemeWarGodRecommdEquip::CSchemeWarGodRecommdEquip(void)
{

}

CSchemeWarGodRecommdEquip::~CSchemeWarGodRecommdEquip(void)
{

}

// ����ű�
bool CSchemeWarGodRecommdEquip::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // ���ؽű�
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_GOD_RECOMMD_EQUIP);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// �ر�
void CSchemeWarGodRecommdEquip::Close(void)
{
    m_listScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV��������ʱ֪ͨ
bool CSchemeWarGodRecommdEquip::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_listScheme.clear();

    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
		SWarGodRecommdEquip node;

		int nLen = sizeof(node.szLeagueName);
		pCSVReader->GetString(nRow, COLNUM_LeagueName, node.szLeagueName, nLen);

		node.LeagueType = pCSVReader->GetInt(nRow, COLNUM_LeagueType, 0);

		nLen = sizeof(node.szTitle);
		pCSVReader->GetString(nRow, COLNUM_Title, node.szTitle, nLen);

		node.nHeroID = pCSVReader->GetInt(nRow, COLNUM_HeroID, 0);

		nLen = sizeof(node.szSchemeName);
		pCSVReader->GetString(nRow, COLNUM_SchemeName, node.szSchemeName, nLen);

		nLen = sizeof(node.szAuthorName);
		pCSVReader->GetString(nRow, COLNUM_AuthorName, node.szAuthorName, nLen);

		// ��ȡװ����
		char szEquipList[256] = { 0 };
		nLen = sizeof(szEquipList);
		pCSVReader->GetString(nRow, COLNUM_EquipIDList, szEquipList, nLen);
		parseIntArray(szEquipList, node.nEquipList, WARQUIPSCHEME_EQUIP_MAXSIZE, ';');

		m_listScheme.push_back(node);
    }

	AfterSchemeLoaded();

    return true;
}

	
// ���ö�̬����ʱ֪ͨ
bool CSchemeWarGodRecommdEquip::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeWarGodRecommdEquip* pNewInstance = NewInstance();
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

vector<SWarGodRecommdEquip>* CSchemeWarGodRecommdEquip::getGodRecommdEquip()
{
	return &m_listScheme;
}
