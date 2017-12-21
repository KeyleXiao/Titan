#include "StdAfx.h"
#include "SchemeMatchRankConfig.h"
#include "IServerGlobal.h"
#include <string>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeMatchRankConfig::CSchemeMatchRankConfig(void)
{

}

/** 
@param   
@param   
@return  
*/
CSchemeMatchRankConfig::~CSchemeMatchRankConfig(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeMatchRankConfig::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }

    string stringPath = SCP_PATH_FORMAT( MATCHRANKCONFIG_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        Error("CSchemeMatchRankConfig::LoadScheme failed! filename=" << stringPath.c_str() << endl);
        return false;
    }

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeMatchRankConfig::Close(void)
{	
    // 清空
    m_mapMatchRankConfigManager.clear();
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
bool CSchemeMatchRankConfig::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    bool bRet = false;
    string strMatchRankConfig = SCP_PATH_FORMAT( MATCHRANKCONFIG_SCHEME_FILENAME );

    if(rkt::StringHelper::casecmp(szFileName, strMatchRankConfig.c_str()) == 0)
    {
        bRet = LoadMatchRankScheme(reader.pCSVReader);
    }

    if(!bRet)
    {
        return false;
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
bool CSchemeMatchRankConfig::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return true;
}

SMatchRankConfigSchemeInfo* CSchemeMatchRankConfig::getMatchRankConfigShemeInfo(int eMatchTypeID, int nGradeId)
{
    TMAP_MATCHRANKCONFIG_MANAGER::iterator iter = m_mapMatchRankConfigManager.find((EMMatchType)eMatchTypeID);
    if (iter == m_mapMatchRankConfigManager.end())
    {
        return NULL;
    }

    TMAP_MATCHRANKCONFIGSCHEME::iterator findIter = iter->second.find(nGradeId);
    if (findIter == iter->second.end())
    {
        return NULL;
    }
    return &(findIter->second);
}

SMatchRankConfigSchemeInfo* CSchemeMatchRankConfig::getShemeInfoByTypeAndScore(int eMatchTypeID, int nScore)
{

    if (eMatchTypeID >= MatchType_MaxID || eMatchTypeID <= MatchType_None)
    {
        return NULL;
    }

    TMAP_MATCHRANKCONFIG_MANAGER::iterator iter = m_mapMatchRankConfigManager.find((EMMatchType)eMatchTypeID);
    if (iter == m_mapMatchRankConfigManager.end())
    {
        ErrorLn(__FUNCTION__": eMatchTypeID = "<<eMatchTypeID)
        return NULL;
    }

    SMatchRankConfigSchemeInfo *pSchemeInfo = NULL;
    TMAP_MATCHRANKCONFIGSCHEME::iterator findIter = iter->second.begin();
    for (;findIter != iter->second.end(); ++findIter)
    {
        if ( nScore >= findIter->second.nGradeScore )
        {
            pSchemeInfo = &(findIter->second);
            continue;
        }
        else
        {
            break;
        }
    }

    return pSchemeInfo;
}

int CSchemeMatchRankConfig::getMatchTypeKingRankBaseScore(int eMatchTypeID)
{
    map<BYTE, int>::iterator iter = m_mapMatchTypeKingRankBase.find(eMatchTypeID);
    if (iter == m_mapMatchTypeKingRankBase.end())
    {
        return 0;
    }
    return iter->second;
}

bool CSchemeMatchRankConfig::LoadMatchRankScheme(ICSVReader* pCSVReader)
{
    if(pCSVReader == NULL )
    {
        return false;
    }
    m_mapMatchTypeKingRankBase.clear();
    m_mapMatchRankConfigManager.clear();

    int nLen = 0;
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow = 0; nRow < nRecordCount; ++nRow)
    {
        int nIndex = 0;
        SMatchRankConfigSchemeInfo item;

        //撮合类型ID
        item.eMatchType = (EMMatchType)pCSVReader->GetInt(nRow, nIndex++, 0);;
        //段位 id
        item.nGradeID       = pCSVReader->GetInt(nRow, nIndex++, 0);
        //保底段位标识	
        item.bIsBaseGrade   = pCSVReader->GetInt(nRow, nIndex++, 0) == 1 ? true : false;
        //段位分数	
        item.nGradeScore    = pCSVReader->GetInt(nRow, nIndex++, 0);
        //系数	
        item.nKRatio        = pCSVReader->GetInt(nRow, nIndex++, 0);

        //隐藏段位分数	
        item.nHideGradeScore    = pCSVReader->GetInt(nRow, nIndex++, 0);
        //隐藏分系数	
        item.nHideKRatio        = pCSVReader->GetInt(nRow, nIndex++, 0);

        //图标ID	
        item.nIconID        = pCSVReader->GetInt(nRow, nIndex++, 0);
        //升级动画ID	
        item.nRaiseAnimationID  =  pCSVReader->GetInt(nRow, nIndex++, 0);
        //降级动画ID	
        item.nFallAnimationID   =  pCSVReader->GetInt(nRow, nIndex++, 0);

        //匹配加成界限
        item.nMatchAddLimit     = pCSVReader->GetInt(nRow, nIndex++, 0);
        //匹配加成值
        item.nMatchAddValue     = pCSVReader->GetInt(nRow, nIndex++, 0);

        //描述
        nLen = sizeof(item.szDes);
        pCSVReader->GetString(nRow, nIndex++, item.szDes, nLen);

        // 星级分段
        item.nStarSection   =  pCSVReader->GetInt(nRow, nIndex++, 0);

        //王者段位标识
        item.bKingFlag      = pCSVReader->GetInt(nRow, nIndex++, 0) == 1;

        //王者排名区间
        item.nKingOrderSection      = pCSVReader->GetInt(nRow, nIndex++, 0);
        // 插入到列表中
        TMAP_MATCHRANKCONFIG_MANAGER::iterator iter = m_mapMatchRankConfigManager.find(item.eMatchType);
        if (iter == m_mapMatchRankConfigManager.end())
        {
            // 插入
            m_mapMatchRankConfigManager[item.eMatchType][item.nGradeID] = item;
        }
        else
        {
            // 更新
            TMAP_MATCHRANKCONFIGSCHEME::iterator findIter = iter->second.find(item.nGradeID);
            if (findIter == iter->second.end())
            {
                iter->second[item.nGradeID] = item;
            }
            else
            {
                ErrorLn(__FUNCTION__": config Repeat eMatchType = "<<item.eMatchType<<" nGradeID = "<<item.nGradeID );
                return false;
            }
        }

        if (item.bKingFlag)
        {
            if (m_mapMatchTypeKingRankBase.find(item.eMatchType) == m_mapMatchTypeKingRankBase.end())
            {
                m_mapMatchTypeKingRankBase[item.eMatchType] = item.nGradeScore;
            }

            if (item.nGradeScore < m_mapMatchTypeKingRankBase[item.eMatchType])
            {
                m_mapMatchTypeKingRankBase[item.eMatchType] = item.nGradeScore;
            }
        }
    }
    return true;
}

SMatchRankConfigSchemeInfo* CSchemeMatchRankConfig::getShemeHideInfoByTypeAndScore(int eMatchTypeID, int nScore)
{
    if (eMatchTypeID >= MatchType_MaxID || eMatchTypeID <= MatchType_None)
    {
        return NULL;
    }

    TMAP_MATCHRANKCONFIG_MANAGER::iterator iter = m_mapMatchRankConfigManager.find((EMMatchType)eMatchTypeID);
    if (iter == m_mapMatchRankConfigManager.end())
    {
        ErrorLn(__FUNCTION__": eMatchTypeID = "<<eMatchTypeID)
        return NULL;
    }

    SMatchRankConfigSchemeInfo *pSchemeInfo = NULL;
    TMAP_MATCHRANKCONFIGSCHEME::iterator findIter = iter->second.begin();
    for (;findIter != iter->second.end(); ++findIter)
    {
        if ( nScore >= findIter->second.nHideGradeScore )
        {
            pSchemeInfo = &(findIter->second);
            continue;
        }
        else
        {
            break;
        }
    }

    return pSchemeInfo;
}
