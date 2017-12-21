/*******************************************************************
** 文件名:	SchemeWarGodRecommdEquip.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/4/17
** 版  本:	1.0
** 描  述:
			大神推荐装脚本
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

// 载入脚本
bool CSchemeWarGodRecommdEquip::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_GOD_RECOMMD_EQUIP);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// 关闭
void CSchemeWarGodRecommdEquip::Close(void)
{
    m_listScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV配置载入时通知
bool CSchemeWarGodRecommdEquip::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_listScheme.clear();

    // 读取
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

		// 读取装备组
		char szEquipList[256] = { 0 };
		nLen = sizeof(szEquipList);
		pCSVReader->GetString(nRow, COLNUM_EquipIDList, szEquipList, nLen);
		parseIntArray(szEquipList, node.nEquipList, WARQUIPSCHEME_EQUIP_MAXSIZE, ';');

		m_listScheme.push_back(node);
    }

	AfterSchemeLoaded();

    return true;
}

	
// 配置动态更新时通知
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
