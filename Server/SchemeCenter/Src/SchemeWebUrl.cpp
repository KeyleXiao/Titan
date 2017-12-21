/*******************************************************************
** 文件名:	SchemeWebUrl.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/6/2017
** 版  本:	1.0
** 描  述:	游戏Web配置 WebUrl.csv


**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#include "StdAfx.h"
#include "IServerGlobal.h"
#include "SchemeDef.h"
#include "SchemeWebUrl.h"
#include <string>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeWebUrl::CSchemeWebUrl(void)
{

}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeWebUrl::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载csv脚本
    string stringPath = SCP_PATH_FORMAT(WEBURL_SCHEME_FILENAME);
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeWebUrl::Close(void)
{	
	// 清空
    ReleaseMapNodes();
}

// 释放所有的节点数据
void CSchemeWebUrl::ReleaseMapNodes()
{
    for (auto iter = m_mapSchemes.begin(); iter != m_mapSchemes.end(); ++iter)
    {
        SSchemeWebUrl* pNode = iter->second;
        if (nullptr != pNode)
        {
            delete pNode;
        }
    }
    
    m_mapSchemes.clear();
}

/** CSV配置载入时通知
@param   pCSVReader：读取CSV的返回接口
@param   szFileName：配置文件名
@param   
@return  
@note     
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/ 
bool CSchemeWebUrl::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;

    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    // 清空
    ReleaseMapNodes();

    const int ARRAY_SIZE = 256;
    char szArray[ARRAY_SIZE] = {0};

    int nLen = 0;
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow=0; nRow<nRecordCount; ++nRow)
    {
        SSchemeWebUrl * pNode = new SSchemeWebUrl;

        // ID
        pNode->nID = pCSVReader->GetInt(nRow, COL_ID, 0);

        // 名称
        nLen = sizeof(pNode->szName) / sizeof(char);
        pCSVReader->GetString(nRow, COL_NAME, pNode->szName, nLen);

        // 客户端导航显示
        pNode->isClientNav = (0 != pCSVReader->GetInt(nRow, COL_IS_SHOWNAV, 0));

        // 功能类型(客户端函数channel；0上发到逻辑层)
        pNode->nClientFunType = static_cast<BYTE>(pCSVReader->GetInt(nRow, COL_CLIENT_FUNC_TYPE, 0));

        // 是否自动登录方式
        pNode->isAutoLogin = (0 != pCSVReader->GetInt(nRow, COL_IS_AUTOLOGIN, 0));

        // 前置URL
        nLen = sizeof(pNode->szPrevUrl) / sizeof(char);
        pCSVReader->GetString(nRow, COL_PREV_URL, pNode->szPrevUrl, nLen);

        // 结尾URL
        nLen = sizeof(pNode->szFollowUrl) / sizeof(char);
        pCSVReader->GetString(nRow, COL_FOLLOW_URL, pNode->szFollowUrl, nLen);

        m_mapSchemes.insert(make_pair<DWORD, SSchemeWebUrl*>(pNode->nID, pNode));
    }

    AfterSchemeLoaded();

    return true;
}


/** 配置动态更新时通知
@param   pCSVReader：读取CSV的返回接口
@param   szFileName：配置文件名
@param   
@return  
@note     
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/
bool CSchemeWebUrl::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeWebUrl* pNewInstance = NewInstance();
	if(pNewInstance == NULL)
	{
		return false;
	}
	
	if(pNewInstance->OnSchemeLoad(reader, szFileName))
	{
		return true;
	}
	else
	{
		safeDelete(pNewInstance);
		return false;
	}
}


//////////////////////////////////////////////////////////////////////////
/** 取得指定卡牌的配置信息
@param nCardID: 卡牌ID 
@return  
*/
SSchemeWebUrl*  CSchemeWebUrl::getByID(DWORD nID)
{
    URL_SCHEME_MAP::iterator iter = m_mapSchemes.find(nID);
    if(iter == m_mapSchemes.end())
    {
        return NULL;
    }

    return iter->second;
}

// 获取所有的配置
std::map<DWORD, SSchemeWebUrl*>*  CSchemeWebUrl::getScheme()
{
    return &m_mapSchemes;
}