/*******************************************************************
** 文件名:	SchemeWarDevilKing.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/11/30
** 版  本:	1.0
** 描  述:   战场魔王玩法配置
********************************************************************/


#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeWarDevilKing.h"
#include "SchemeWarDef.h"

CSchemeWarDevilKing::CSchemeWarDevilKing(void)
{

}

CSchemeWarDevilKing::~CSchemeWarDevilKing(void)
{

}

// 载入脚本
bool CSchemeWarDevilKing::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_DEVILKING);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// 关闭
void CSchemeWarDevilKing::Close(void)
{
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV配置载入时通知
bool CSchemeWarDevilKing::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;


    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {

		SWarDevilKing& node = m_Scheme;

		int nCol = 0;
		node.nKillNumCond = pCSVReader->GetInt(nRow, nCol++, 0);
		node.nDevilKingBuff = pCSVReader->GetInt(nRow, nCol++, 0);
		node.nCloseTime = pCSVReader->GetInt(nRow, nCol++, 0) * 1000; // 转换成毫秒
		node.nBcastTime = pCSVReader->GetInt(nRow, nCol++, 0) * 1000; // 转换成毫秒

		// 暂时只有一行数据
		break;
    }

	AfterSchemeLoaded();

    return true;
}

	
// 配置动态更新时通知
bool CSchemeWarDevilKing::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeWarDevilKing* pNewInstance = NewInstance();
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

SWarDevilKing * CSchemeWarDevilKing::getWarDevilKing()
{
	return &m_Scheme;
}

