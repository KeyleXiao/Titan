/*******************************************************************
** 文件名:	SchemeKinDataCenter.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	战队相关配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeKinDataCenter.h"
#include "IClientGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeKinDataCenter::CSchemeKinDataCenter(void)
{
	
}

/** 
@param   
@param   
@return  
*/
CSchemeKinDataCenter::~CSchemeKinDataCenter(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeKinDataCenter::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( KIN_CONFIG_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		ErrorLn(__FUNCTION__":config file load failed! filename=" << stringPath.c_str());
		return false;
	}

    stringPath = SCP_PATH_FORMAT( KIN_LEVEL_FILENAME );
    bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        ErrorLn(__FUNCTION__":config file load failed! filename=" << stringPath.c_str());
        return false;
    }

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeKinDataCenter::Close(void)
{
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
bool CSchemeKinDataCenter::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    bool bRet = false;
    string strConfigPath = SCP_PATH_FORMAT( KIN_CONFIG_FILENAME );
    string strLevelPath = SCP_PATH_FORMAT( KIN_LEVEL_FILENAME );

    if(strcmp(szFileName, strConfigPath.c_str()) == 0)
    {
        bRet = LoadConfigScheme(reader.pCSVReader);
    }

    if(strcmp(szFileName, strLevelPath.c_str()) == 0)
    {
        bRet = LoadLevelScheme(reader.pCSVReader);
    }

    if(!bRet)
    {
        return false;
    }	

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
bool CSchemeKinDataCenter::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	if(OnSchemeLoad(reader, szFileName))
	{
		return true;
	}
    return false;
}

/// purpose:  加载战队常量脚本
bool CSchemeKinDataCenter::LoadConfigScheme(ICSVReader * pCSVReader)
{
    string strLoadFileName = KIN_CONFIG_FILENAME;

    char szTemp[512] = {0};
    int arrTime[4] = {0};

    memset(&m_Config, 0, sizeof(m_Config));

    // 创建战队需要的等级
    m_Config.nCreateLevel = pCSVReader->GetInt(0, 1, 0);
    if (m_Config.nCreateLevel <= 0 || m_Config.nCreateLevel > 100)
    {
        return false;
    }
    // 创建战队需要的金钱
    m_Config.nCreateMoney = pCSVReader->GetInt(1, 1, 0);
    if (m_Config.nCreateMoney < 0)
    {
        return false;
    }

    // 加入战队需要的等级
    m_Config.nJoinNeedLevel = pCSVReader->GetInt(2, 1, 0);
    if (m_Config.nCreateMoney < 0)
    {
        return false;
    }

    return true;
}

/// purpose:  加载战队升级脚本
bool CSchemeKinDataCenter::LoadLevelScheme(ICSVReader * pCSVReader)
{
    int nRecordCount = (int)pCSVReader->GetRecordCount();
    SKinLevel levelInfo;
    memset(m_LevelInfoList, 0, sizeof(m_LevelInfoList));
    for (int i = 0; i < nRecordCount; i ++)
    {
        // 战队等级
        int nLevel = pCSVReader->GetInt(i, 0, 0);
        if (nLevel <= 0 || nLevel > KIN_MAX_LEVEL)
        {
            Error(KIN_LEVEL_FILENAME << "（"<< i << "，0）位置读取错误" << endl);
            return false;
        }

        // 最大人数
        levelInfo.nMaxMemberCount = pCSVReader->GetInt(i, 1, 0);
        if (levelInfo.nMaxMemberCount < 0)
        {
            Error(KIN_LEVEL_FILENAME << "（"<< i << "，1）位置读取错误" << endl);
            return false;
        }

        m_LevelInfoList[nLevel] = levelInfo;
    }

    return true;
}

SKinConfig* CSchemeKinDataCenter::getConfig()
{
    return &m_Config;
}

SKinLevel* CSchemeKinDataCenter::getLevelInfo(int nKinLevel)
{
    if (nKinLevel <= 0 || nKinLevel > KIN_MAX_LEVEL)
    {
        return NULL;
    }

    return &m_LevelInfoList[nKinLevel];
}

