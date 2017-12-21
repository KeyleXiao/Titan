/*******************************************************************
** 文件名:	SchemeLevelUpgrade.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林飞
** 日  期:	3/12/2015
** 版  本:	1.0
** 描  述:	配置 LevelUpgrade.csv

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeLevelUpgrade.h"
#include "IServerGlobal.h"
#include <string>
using namespace std;


//////////////////////////////////////////////////////////////////////////

struct LevelExpCompare
{
    LevelExpCompare(SLevelUpgradeScheme data) : _data(data)
    {
    }

    bool operator()(SLevelUpgradeScheme data)
    {
        return _data.nLv == data.nLv && _data.nExpType == data.nExpType;
    }
private:
    SLevelUpgradeScheme _data;
};

//////////////////////////////////////////////////////////////////////////
/** 
@param   
@param   
@return  
*/
CSchemeLevelUpgrade::CSchemeLevelUpgrade(void)
{	
    m_mapExpScheme.clear();
    m_mapLvMaxScheme.clear();
    m_mapLvMinScheme.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemeLevelUpgrade::~CSchemeLevelUpgrade(void)
{	
    m_mapExpScheme.clear();
    m_mapLvMaxScheme.clear();
    m_mapLvMinScheme.clear();
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeLevelUpgrade::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载LevelUpgrade.csv脚本
    string stringPath = SCP_PATH_FORMAT( LEVELUPGRADE_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("config file load failed! filename=" << stringPath.c_str() << endl);
		return false;
	}

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeLevelUpgrade::Close(void)
{	
	// 清空
    m_mapExpScheme.clear();
    m_mapLvMaxScheme.clear();
    m_mapLvMinScheme.clear();
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
bool CSchemeLevelUpgrade::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
    m_mapExpScheme.clear();
    m_mapLvMaxScheme.clear();
    m_mapLvMinScheme.clear();

	int nExpAll = 0;
	int nLen = 0;
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();

    SLevelUpgradeScheme sItem;

    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
        memset( &sItem, 0, sizeof(sItem) );
        // 等级
        sItem.nLv = pCSVReader->GetInt( nRow, ENLVUPSCHEMECOL_LV, 0);
        // 经验
        sItem.nExp = pCSVReader->GetInt( nRow, ENLVUPSCHEMECOL_EXP, 0);
        // 经验所属
        sItem.nExpAttribution = pCSVReader->GetInt( nRow, ENLVUPSCHEMECOL_ATTRIBUTION, 0);
        // 经验类型
        sItem.nExpType = pCSVReader->GetInt( nRow, ENLVUPSCHEMECOL_EXPTYPE, 0);

        EXP_ARRAY_SCHEMEMAP::iterator it = m_mapExpScheme.find( sItem.nExpAttribution );
        if ( it == m_mapExpScheme.end() )
        {
            LEVEL_UPGRADE_SCHEMEVEC mapNode;
            mapNode.push_back(sItem);

            m_mapExpScheme[sItem.nExpAttribution] = mapNode;

            // 设置最少等级
            m_mapLvMinScheme[sItem.nExpAttribution] = sItem.nLv;
            // 设置最大等级
            m_mapLvMaxScheme[sItem.nExpAttribution] = sItem.nLv;

        }
        else
        {
            LEVEL_UPGRADE_SCHEMEVEC& mapNode = it->second;;
            mapNode.push_back( sItem );

            // 设置最大等级
            if(m_mapLvMaxScheme[sItem.nExpAttribution] < sItem.nLv)
            {
                m_mapLvMaxScheme[sItem.nExpAttribution] = sItem.nLv;
            }
            // 设置最少等级
            if(m_mapLvMinScheme[sItem.nExpAttribution] > sItem.nLv)
            {
                m_mapLvMinScheme[sItem.nExpAttribution] = sItem.nLv;
            }
        }
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
bool CSchemeLevelUpgrade::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeLevelUpgrade* pNewInstance = NewInstance();
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


/////////////////////////ISchemeLevelUpgrade/////////////////////////
/** 取得某等级经验配置信息
@param nLevel:	等级
@return  
*/
int CSchemeLevelUpgrade::GetNextLevelExp(int nLevel, int nAttributton, int nExpTypeID )
{
    // 配置经验所属（0、代表战场内英雄经验，1、代表场外英雄经验，2、代表角色经验）
    EXP_ARRAY_SCHEMEMAP::iterator itExpArr = m_mapExpScheme.find(nAttributton);
    // 没有配置的经验所属将用最大INT数值
    if ( itExpArr == m_mapExpScheme.end())
    {
        ErrorLn(__FUNCTION__": can't find nAttributton exp nAttributton = " << nAttributton );
        return INT_MAX;
    }

    // 没有找到将用最大等级对应的经验
    int nNewLevel = max(nLevel, m_mapLvMinScheme[nAttributton]);
    nNewLevel = min(nNewLevel, m_mapLvMaxScheme[nAttributton]);

    SLevelUpgradeScheme node;
    node.nLv = nNewLevel;
    node.nExpType = nExpTypeID;

    LEVEL_UPGRADE_SCHEMEVEC& nodeList = itExpArr->second;
    LEVEL_UPGRADE_SCHEMEVEC::iterator iter = find_if( nodeList.begin(), nodeList.end(), LevelExpCompare(node) );

    // 存在就返回当前值，否则返回最大整数
    return (iter != nodeList.end() ? (*iter).nExp : INT_MAX);
}

// 取得升级到某等级所需最低总经验值
int CSchemeLevelUpgrade::GetLevelAllExp(int nLevel, int nAttributton, int nExpTypeID )
{
    EXP_ARRAY_SCHEMEMAP::iterator itExpArr = m_mapExpScheme.find(nAttributton);
    // 没有配置的经验所属将用最大INT数值
    if ( itExpArr == m_mapExpScheme.end())
    {
        ErrorLn(__FUNCTION__": can't find nAttributton exp nAttributton = " << nAttributton );
        return INT_MAX;
    }
    LEVEL_UPGRADE_SCHEMEVEC& nodeList = itExpArr->second;
    if(nodeList.empty())
    {
        return INT_MAX;
    }

    // 没有找到将用最大等级对应的经验
    int nMinLevel = m_mapLvMinScheme[nAttributton];
    int nMaxLevel = m_mapLvMaxScheme[nAttributton];
    int nNewLevel = max(nLevel, nMinLevel);
    nNewLevel = min(nNewLevel, nMaxLevel);
    
    // 取得最小等级对应的EXP
    int nMinLvExp = nodeList[nMinLevel].nExp;

    int nSumExp = 0;
    for (LEVEL_UPGRADE_SCHEMEVEC::iterator iter = nodeList.begin(); iter != nodeList.end(); ++iter )
    {
        SLevelUpgradeScheme & node = (*iter);
        if ( node.nExpType == nExpTypeID && node.nLv <= nNewLevel )
            nSumExp += node.nExp;
    }

    // 取不到就用最少值
	//return (nSumExp==0 ? nMinLvExp : nSumExp);
	return nSumExp - nMinLvExp;
}

/** 取得升级配置等级对应最大等级
@param 
@return  最大等级
*/
int CSchemeLevelUpgrade::GetSchemeLevelUpgradeMaxLevel(int nAttributton)
{
    LEVEL_MAX_SCHEMEMAP::iterator itLvMax = m_mapLvMaxScheme.find(nAttributton);
    // 配置中不存在将用120等级
    if ( itLvMax == m_mapLvMaxScheme.end())
    {
        ErrorLn(__FUNCTION__": can't find nAttributton exp nAttributton = " << nAttributton );
        return 120;
    }

	return itLvMax->second;
}

/** 取得升级配置等级对应最小等级
@param 
@return  最小等级
*/
int CSchemeLevelUpgrade::GetSchemeLevelUpgradeMinLevel(int nAttributton)
{
    LEVEL_MIN_SCHEMEMAP::iterator itLvMin = m_mapLvMinScheme.find(nAttributton);
    // 配置中不存在将用1等级
    if ( itLvMin == m_mapLvMinScheme.end())
    {
        ErrorLn(__FUNCTION__": can't find nAttributton exp nAttributton = " << nAttributton );
        return 1;
    }

    return itLvMin->second;
}
