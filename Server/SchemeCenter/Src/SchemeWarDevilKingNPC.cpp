/*******************************************************************
** 文件名:	SchemeWarDevilKingNPC.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/11/30
** 版  本:	1.0
** 描  述:   战场魔王怪物NPC配置
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

// 载入脚本
bool CSchemeWarDevilKingNPC::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_DEVILKINGNPC);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// 关闭
void CSchemeWarDevilKingNPC::Close(void)
{
	m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV配置载入时通知
bool CSchemeWarDevilKingNPC::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;


    // 读取
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

	
// 配置动态更新时通知
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

