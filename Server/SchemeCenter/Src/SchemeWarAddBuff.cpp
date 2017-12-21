/*******************************************************************
** 文件名:	CSchemeWarAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/4/13
** 版  本:	1.0
** 描  述:	
            战场子功能设置脚本
********************************************************************/


#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeWarAddBuff.h"
#include "SchemeWarDef.h"

CSchemeWarAddBuff::CSchemeWarAddBuff(void)
{

}

CSchemeWarAddBuff::~CSchemeWarAddBuff(void)
{

}

// 载入脚本
bool CSchemeWarAddBuff::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_ADD_BUFF);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// 关闭
void CSchemeWarAddBuff::Close(void)
{
    m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV配置载入时通知
bool CSchemeWarAddBuff::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapScheme.clear();

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
		SWarAddBuff node;

		node.nMatchType = pCSVReader->GetInt(nRow, COLNUM_MatchType, 0);
		node.nBuffID = pCSVReader->GetInt(nRow, COLNUM_BuffID, 0);
		node.nLimitLv = pCSVReader->GetInt(nRow, COLNUM_LimitLv, 0);
		node.nTime = pCSVReader->GetInt(nRow, COLNUM_Time, 0) * 1000;
		node.nBuffLv = pCSVReader->GetInt(nRow, COLNUM_BuffLv, 0);
		node.bAddToAI = pCSVReader->GetBool(nRow, COLNUM_IsAddToAI, false);
		node.bAddToAIEx = pCSVReader->GetBool(nRow, COLNUM_IsAddToAIEx, false);

		m_mapScheme[node.nMatchType].push_back(node);
    }

	AfterSchemeLoaded();

    return true;
}

	
// 配置动态更新时通知
bool CSchemeWarAddBuff::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeWarAddBuff* pNewInstance = NewInstance();
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

// 获取添加Buff信息
vector<SWarAddBuff>* CSchemeWarAddBuff::getAddBuffInfo(int nMatchID)
{
	WAR_ADDBUFF_MAP_ITER iter = m_mapScheme.find(nMatchID);
	if (iter == m_mapScheme.end())
		return NULL;

	return &iter->second;
}
