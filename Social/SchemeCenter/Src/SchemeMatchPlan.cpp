/*******************************************************************
** 文件名:	SchemeMatchPlan.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	实体通用配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeMatchPlan.h"
#include "ISocialGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeMatchPlan::CSchemeMatchPlan(void)
{
	
}

/** 
@param   
@param   
@return  
*/
CSchemeMatchPlan::~CSchemeMatchPlan(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeMatchPlan::LoadScheme(void)
{
    ISchemeEngine * pSchemeEngine = gSocialGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL )
    {
        return false;
    }

    string stringPath = SCP_PATH_FORMAT( MATCHPLAN_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        ErrorLn(__FUNCTION__":config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    stringPath = SCP_PATH_FORMAT( MATCHPLANRATIO_SCHEME_FILENAME );
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
void CSchemeMatchPlan::Close(void)
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
bool CSchemeMatchPlan::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    bool bRet = false;
    string strRulePath = SCP_PATH_FORMAT( MATCHPLAN_SCHEME_FILENAME );
    string strRoomPath = SCP_PATH_FORMAT( MATCHPLANRATIO_SCHEME_FILENAME );

    if(rkt::StringHelper::casecmp(szFileName, strRulePath.c_str()) == 0)
    {
        bRet = LoadMatchPlanScheme(reader.pCSVReader);
    }

    if(rkt::StringHelper::casecmp(szFileName, strRoomPath.c_str()) == 0)
    {
        bRet = LoadMatchPlanRatioScheme(reader.pCSVReader);
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
bool CSchemeMatchPlan::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeMatchPlan* pNewInstance = NewInstance();
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

bool CSchemeMatchPlan::LoadMatchPlanScheme(ICSVReader* pCSVReader)
{
	// 读取
    m_ALLMatchTypeMatchPlan.clear();
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		int nIndex = 0;
		SSchemeMatchPlan pData;
        pData.nStageID                  = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );           //比赛类型
        pData.nMatchType                = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );           //比赛类型
        pData.nMatchFriendOrOpposite    = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );           //匹配类型(1、匹配队友 2、匹配敌方)
        pData.nWaitSeconds              = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );           //等待时间（秒）
        pData.nCountLowerLimit          = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );           //队友人数下限
        pData.nCountUpperLimit          = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );           //队友人数上限
        pData.nRankLowerLimit           = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );           //匹配段位下限
        pData.nRankUpperLimit           = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );           //匹配段位上限
        pData.nNextStageID              = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );           //下一阶段ID
        int nStage = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );
        pData.bIsFirstStage             = nStage == EMP_FirstStage? true: false;                               // 是否是初始阶段
        pData.bIsEndStage               = nStage == EMP_EndStage? true: false;                                 // 是否是初始阶段
		pData.nFirstRecommendTime		= static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );		   // 首次推荐等待时间
		pData.nRecommendGapTime			= static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );		   // 再次推荐时间间隔
		pData.nRecommendActorCount		= static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );		   // 单次推荐人数
        pData.nStartPlayerCount        = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );		    // 开启人数
        pData.nDelayStartTime           = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );		   //进入战场延时
        pData.nStartLowestPlayerCount   = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );		   // 本阶段开始最低人数

        MAP_ALLMatchTypeMatchPlan::iterator iter = m_ALLMatchTypeMatchPlan.find(pData.nMatchType);
        if (iter == m_ALLMatchTypeMatchPlan.end())
        {
            m_ALLMatchTypeMatchPlan[pData.nMatchType].insert(pair<int,SSchemeMatchPlan>(pData.nStageID, pData));
        }
        else
        {
            iter->second.insert(pair<int,SSchemeMatchPlan>(pData.nStageID, pData));
        }
		
	}


	return true;
}

bool CSchemeMatchPlan::LoadMatchPlanRatioScheme(ICSVReader* pCSVReader)
{
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
        int nIndex = 0;
        SSchemeMatchPlanRatio* pData = new SSchemeMatchPlanRatio();
        pData->nMatchType           = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );                //比赛类型
        pData->nBaseContinueWin     = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );                //保底连胜场次
        pData->nContinueWinRatio    = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );                //连胜附加系数
        pData->nBaseContinueLose    = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );                //保底连负场次
        pData->nContinueLoseRatio   = static_cast<int>( pCSVReader->GetInt(nRow, nIndex++, 0) );                //连负附加系数
	}
	return true;
}


SSchemeMatchPlan* CSchemeMatchPlan::getSchemeMatchPlan(int nMatchType, int nStageID)
{
    MAP_ALLMatchTypeMatchPlan::iterator iter = m_ALLMatchTypeMatchPlan.find(nMatchType);
    if (iter == m_ALLMatchTypeMatchPlan.end())
    {
        return NULL;
    }
    else
    {
        MAP_StageID2MatchPlan::iterator itesub = iter->second.find(nStageID);
        if (itesub == iter->second.end())
        {
            return NULL;
        }

        return &(itesub->second);
    }
}

SSchemeMatchPlan* CSchemeMatchPlan::getSchemeFristMatchPlan(int nMatchType, int nMatchFriendOrOpposite)
{
    MAP_ALLMatchTypeMatchPlan::iterator iter = m_ALLMatchTypeMatchPlan.find(nMatchType);
    if (iter != m_ALLMatchTypeMatchPlan.end())
    {
        MAP_StageID2MatchPlan::iterator itersub = iter->second.begin();
        for(;itersub != iter->second.end();++itersub)
        {
            if(itersub->second.bIsFirstStage == true && itersub->second.nMatchFriendOrOpposite == nMatchFriendOrOpposite)
            {
                 return &(itersub->second);
            }
        }
    }

    ErrorLn(__FUNCTION__":can't find the shcme check the SchemeMatchPlan.csv nMatchType = "<<nMatchType<< ",nMatchFriendOrOpposite="<<nMatchFriendOrOpposite );

    return NULL;
}

SSchemeMatchPlan* CSchemeMatchPlan::getMatchPlanByWaitSecond(int nMatchType, int nMatchFriendOrOpposite, DWORD nWaitSeconds)
{
    SSchemeMatchPlan *pFindData = getSchemeFristMatchPlan(nMatchType, nMatchFriendOrOpposite);
    MAP_ALLMatchTypeMatchPlan::iterator iter = m_ALLMatchTypeMatchPlan.find(nMatchType);
    if (iter != m_ALLMatchTypeMatchPlan.end())
    {
        MAP_StageID2MatchPlan::iterator iterSub = iter->second.begin();
        for(;iterSub != iter->second.end();++iterSub )
        {
            if (iterSub->second.nMatchFriendOrOpposite == nMatchFriendOrOpposite)
            {
                if(nWaitSeconds >= (DWORD)iterSub->second.nWaitSeconds)
                {
                    pFindData = &iterSub->second;
                }
            }
        }
    }

    return &(*pFindData);
}

SSchemeMatchPlanRatio* CSchemeMatchPlan::getSchemeMatchPlanRatio(int nMatchType)
{
    return NULL;
}

