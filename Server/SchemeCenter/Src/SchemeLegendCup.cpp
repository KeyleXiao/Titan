/*******************************************************************
** 文件名:	SchemeLegendCup.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	杯赛相关配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeLegendCup.h"
#include "IServerGlobal.h"


/** 
@param   
@param   
@return  
*/
CSchemeLegendCup::CSchemeLegendCup(void)
{
	
}

/** 
@param   
@param   
@return  
*/
CSchemeLegendCup::~CSchemeLegendCup(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeLegendCup::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( LEGENDCUP_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        ErrorLn(__FUNCTION__":config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    stringPath = SCP_PATH_FORMAT( LEGENDCUP_DETAIL_FILENAME );
    bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        ErrorLn(__FUNCTION__":config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    stringPath = SCP_PATH_FORMAT( LEGENDCUP_LIMIT_FILENAME );
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
void CSchemeLegendCup::Close(void)
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
bool CSchemeLegendCup::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    bool bRet = false;
    string strConfigPath = SCP_PATH_FORMAT( LEGENDCUP_SCHEME_FILENAME );
    string strDetailPath = SCP_PATH_FORMAT( LEGENDCUP_DETAIL_FILENAME );
    string strLimitPath = SCP_PATH_FORMAT( LEGENDCUP_LIMIT_FILENAME );
    

    if(rkt::StringHelper::casecmp(szFileName, strConfigPath.c_str()) == 0)
    {
        bRet = LoadConfigScheme(reader.pCSVReader);
    }

    if(rkt::StringHelper::casecmp(szFileName, strDetailPath.c_str()) == 0)
    {
        bRet = LoadDetailScheme(reader.pCSVReader);
    }

    if(rkt::StringHelper::casecmp(szFileName, strLimitPath.c_str()) == 0)
    {
        bRet = LoadLimitScheme(reader.pCSVReader);
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
bool CSchemeLegendCup::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader, szFileName);
}


bool CSchemeLegendCup::LoadLimitScheme(ICSVReader * pCSVReader)
{
    int nRecordCount = pCSVReader->GetRecordCount();
    for(int nRow=0; nRow<nRecordCount; ++nRow)
    {
        
        int nCol = 0;
        SSchemeLegendCupLimit node;
        node.m_nConfigID = pCSVReader->GetInt(nRow, nCol++, 0);
        if (node.m_nConfigID <= 0)
        {
            ErrorLn(__FUNCTION__": configid <= 0 configid = "<<node.m_nConfigID);
            return false;
        }

        if (m_LimitConfig.find(node.m_nConfigID) != m_LimitConfig.end())
        {
            ErrorLn(__FUNCTION__": have found m_dwConfigID = "<<node.m_nConfigID<<" nCol ="<<nCol - 1);
            return false;
        }

        //联盟杯赛限定
        node.m_nLevelLimitPersonCount = pCSVReader->GetInt(nRow, nCol++, 0);   // 限定多少等级的多少人才能才能举办联盟杯赛

        // 等级
        node.m_nLevelLimit = pCSVReader->GetInt(nRow, nCol++, 0);

        // 周活跃度
        node.m_nLimitWeekActivity = pCSVReader->GetInt(nRow, nCol++, 0);

        // 失败描述ID
        node.m_nFailedDesID = pCSVReader->GetInt(nRow, nCol++, 0);

        m_LimitConfig.insert(pair<int, SSchemeLegendCupLimit>(node.m_nConfigID, node));
    }

    return true;
}


/// purpose:  加载战队常量脚本
bool CSchemeLegendCup::LoadConfigScheme(ICSVReader * pCSVReader)
{
    int nRecordCount = pCSVReader->GetRecordCount();
    for(int nRow=0; nRow<nRecordCount; ++nRow)
    {
        int nCol = 0;
        SSchemeLegendCup node;
        node.m_nConfigID = pCSVReader->GetInt(nRow, nCol++, 0);
        if (node.m_nConfigID <= 0)
        {
             ErrorLn(__FUNCTION__": configid <= 0 configid = "<<node.m_nConfigID);
            return false;
        }

        if (m_Config.find(node.m_nConfigID) != m_Config.end())
        {
            ErrorLn(__FUNCTION__": have found m_dwConfigID = "<<node.m_nConfigID<<" nCol ="<<nCol - 1);
            return false;
        }
        

        int nDesLen = sizeof(node.m_szLegendCupDes);
        pCSVReader->GetString(nRow, nCol++, node.m_szLegendCupDes, nDesLen);

        // 轮数
        node.m_byRoundCount = pCSVReader->GetInt(nRow, nCol++, 0);
        if (node.m_byRoundCount <= 0)
        {
            ErrorLn(__FUNCTION__": m_byRoundCount <= 0 m_byRoundCount = "<<node.m_byRoundCount<<" nCol ="<<nCol - 1);
            return false;
        }

        // 最大战队数量
        node.m_nMaxKinCount = pCSVReader->GetInt(nRow, nCol++, 0);
        if (node.m_nMaxKinCount <= 0)
        {
            ErrorLn(__FUNCTION__": m_dwMaxKinCount <= 0 m_dwMaxKinCount = "<<node.m_nMaxKinCount<<" nCol ="<<nCol - 1);
            return false;
        }

        //配置类型ID
        node.m_nDetailID = pCSVReader->GetInt(nRow, nCol++, 0);
        if (node.m_nDetailID <= 0)
        {
            ErrorLn(__FUNCTION__": m_byRoundCount <= 0 m_byRoundCount = "<<node.m_nDetailID<<" nCol ="<<nCol - 1);
            return false;
        }

        char strComfirmRegisterList[128] = {0};
        int nLen = sizeof(strComfirmRegisterList);
        pCSVReader->GetString(nRow, nCol++, strComfirmRegisterList, nLen);
        int nParmCount = parseIntArray(strComfirmRegisterList, node.m_nComfirmRegisterDeadLine, MAX_END_REGISTER_COUT, ';');

        node.m_nRegisterDeadLine = pCSVReader->GetInt(nRow, nCol++, 0);
        if ( node.m_nRegisterDeadLine <= 0 ||  node.m_nRegisterDeadLine >= 24)
        {
            ErrorLn(__FUNCTION__": node.m_RegisterDeadLine <= 0 ||  node.m_RegisterDeadLine >= 24 = "<<node.m_nRegisterDeadLine<<" nCol ="<<nCol - 1);
            return false;
        }

        //线上比赛类型
        node.m_nOnlineMatchType = pCSVReader->GetInt(nRow, nCol++, 0);
        if (node.m_nOnlineMatchType <= 0)
        {
            ErrorLn(__FUNCTION__": m_byRoundCount <= 0 m_byRoundCount = "<<node.m_nOnlineMatchType<<" nCol ="<<nCol - 1);
            return false;
        }

        //线下比赛类型
        node.m_nOfflineMatchType = pCSVReader->GetInt(nRow, nCol++, 0);
        if (node.m_nOfflineMatchType <= 0)
        {
            ErrorLn(__FUNCTION__": m_byRoundCount <= 0 m_byRoundCount = "<<node.m_nOfflineMatchType<<" nCol ="<<nCol - 1);
            return false;
        }

        //创建杯赛需要的金币
        node.m_nNeedMoneyGold = pCSVReader->GetInt(nRow, nCol++, 0);
        if (node.m_nNeedMoneyGold < 0)
        {
            ErrorLn(__FUNCTION__": m_byRoundCount < 0 m_byRoundCount = "<<node.m_nNeedMoneyGold<<" nCol ="<<nCol - 1);
            return false;
        }

        //报名需要的金币
        node.m_nRegisterNeedGold = pCSVReader->GetInt(nRow, nCol++, 0);
        if (node.m_nRegisterNeedGold < 0)
        {
            ErrorLn(__FUNCTION__": m_byRoundCount < 0 m_byRoundCount = "<<node.m_nRegisterNeedGold<<" nCol ="<<nCol - 1);
            return false;
        }

        //报名最高的金币
        node.m_nHighestRegisterGold = pCSVReader->GetInt(nRow, nCol++, 0);
        if (node.m_nHighestRegisterGold < 0)
        {
            ErrorLn(__FUNCTION__": m_nHighestRegisterGold < 0 m_nHighestRegisterGold = "<<node.m_nHighestRegisterGold<<" nCol ="<<nCol - 1);
            return false;
        }

        // 奖励ID
        node.m_nPrizeID = pCSVReader->GetInt(nRow, nCol++, 0);
        if (node.m_nPrizeID < 0)
        {
            ErrorLn(__FUNCTION__": m_nPrizeID < 0 = "<<node.m_nPrizeID<<" nCol ="<<nCol - 1);
            return false;
        }

        //是否免费
        node.m_bIsFree = pCSVReader->GetInt(nRow, nCol++, 0) == 0? false: true;

        //最少开战的战队数量
        node.m_nMiniStartKinCount = pCSVReader->GetInt(nRow, nCol++, 0);
        if (node.m_nMiniStartKinCount < 0)
        {
            ErrorLn(__FUNCTION__": m_nMiniStartKinCount < 0 = "<<node.m_nMiniStartKinCount<<" nCol ="<<nCol - 1);
            return false;
        }

        m_Config.insert(pair<int, SSchemeLegendCup>(node.m_nConfigID, node));
 
    }

    return true;
}

/// purpose:  加载战队升级脚本
bool CSchemeLegendCup::LoadDetailScheme(ICSVReader * pCSVReader)
{
    int nRecordCount = pCSVReader->GetRecordCount();
    for(int nRow=0; nRow<nRecordCount; ++nRow)
    {
        int nCol = 0;

        SSchemeLegendCupDetail node;
        //比赛配置类型	
        node.m_nDetailID = pCSVReader->GetInt(nRow, nCol++, 0);
        if (node.m_nDetailID <= 0)
        {
            ErrorLn(__FUNCTION__": configid <= 0 configid = "<<node.m_nDetailID);
            return false;
        }

        //第几轮
        node.m_nRoundID            = pCSVReader->GetInt(nRow, nCol++, 0);

        map<int, map<int, SSchemeLegendCupDetail>>::iterator iter =  m_ConfigDetail.find(node.m_nDetailID);
        if (iter != m_ConfigDetail.end())
        {
            map<int, SSchemeLegendCupDetail>::iterator iterRound = iter->second.find(node.m_nRoundID);
            if (iterRound != iter->second.end())
            {
                ErrorLn(__FUNCTION__": have found dwDetailID = "<<node.m_nDetailID<<" nCol ="<<nCol-1);
                return false;
            }
        }

        //该轮比赛类型（0空1淘汰2小组）
        node.m_nCompetitionType    = pCSVReader->GetInt(nRow, nCol++, 0);
        //小组数量
        node.m_nMaxGroup            = pCSVReader->GetInt(nRow, nCol++, 0);
        //每组战队数量
        node.m_nGroupMaxKin          = pCSVReader->GetInt(nRow, nCol++, 0);
        //晋级成员数量
        node.m_nPromotionKinCount    = pCSVReader->GetInt(nRow, nCol++, 0);
        //开始比赛时间天数间隔
        node.m_nStartDayInterval     = pCSVReader->GetInt(nRow, nCol++, 0);

        //每天比赛开始时间点
        char strCreateRoomTimeList[128] = {0};
        int nLen = sizeof(strCreateRoomTimeList);
        pCSVReader->GetString(nRow, nCol++, strCreateRoomTimeList, nLen);
        parseIntArray(strCreateRoomTimeList, node.m_nCreateRoomTime, TIME_DES_COUNT_SIZE, ';');

        //每天比赛开始时间点
        char strStartTimeList[128] = {0};
        nLen = sizeof(strStartTimeList);
        pCSVReader->GetString(nRow, nCol++, strStartTimeList, nLen);
        parseIntArray(strStartTimeList, node.m_nStartTime, TIME_DES_COUNT_SIZE, ';');
        //比赛类型(bo1 bo3 bo5)	
        node.m_nBoType              = pCSVReader->GetInt(nRow, nCol++, 0);
        //bo赛的时间间隔(分钟)
        node.m_nBoInterval          = pCSVReader->GetInt(nRow, nCol++, 0);
        //本轮比赛结束时间天数间隔
        node.m_nEndDayInterval       = pCSVReader->GetInt(nRow, nCol++, 0);

        //本轮可重置比赛时间点
        char strResetEndTimeList[128] = {0};
        nLen = sizeof(strResetEndTimeList);
        pCSVReader->GetString(nRow, nCol++, strResetEndTimeList, nLen);
        parseIntArray(strResetEndTimeList, node.m_nResetEndTime, TIME_DES_COUNT_SIZE, ';');

        //本轮结束时间点
        char strEndTimeList[128] = {0};
        nLen = sizeof(strEndTimeList);
        pCSVReader->GetString(nRow, nCol++, strEndTimeList, nLen);
        parseIntArray(strEndTimeList, node.m_nEndTime, TIME_DES_COUNT_SIZE, ';');

        // 比赛进程描述
        nCol++;

        // 每场完成之后的奖励列表
        char strPrizeIDList[128] = {0};
        nLen = sizeof(strPrizeIDList);
        pCSVReader->GetString(nRow, nCol++, strPrizeIDList, nLen);
        parseIntArray(strPrizeIDList, node.m_nPrizeIDList, EVERY_MATCH_PRIZE_ID_MAX_SIZE, ';');

        // 添加进配置里面
        m_ConfigDetail[node.m_nDetailID][node.m_nRoundID] = node;

    }
   
    return true;
}

SSchemeLegendCupLimit* CSchemeLegendCup::getLimitConfig(int nConfigID)
{
    map<int, SSchemeLegendCupLimit>::iterator iter =  m_LimitConfig.find(nConfigID);
    if (iter == m_LimitConfig.end())
    {
        return NULL;
    }

    return &(iter->second);
}

SSchemeLegendCup* CSchemeLegendCup::getConfig(int nConfigID)
{
    map<int, SSchemeLegendCup>::iterator iter =  m_Config.find(nConfigID);
    if (iter == m_Config.end())
    {
        return NULL;
    }

    return &(iter->second);
}

SSchemeLegendCupDetail* CSchemeLegendCup::getDetailInfo(int nDetailID, int nRoundID)
{
    map<int, map<int, SSchemeLegendCupDetail>>::iterator iter =  m_ConfigDetail.find(nDetailID);
    if (iter == m_ConfigDetail.end())
    {
        return NULL;
    }
    map<int, SSchemeLegendCupDetail>::iterator iterRound = iter->second.find(nRoundID);
    if (iterRound == iter->second.end())
    {
        return NULL;
    }

    return &(iterRound->second);
}

int CSchemeLegendCup::getRoundIDByCount(int nDetailID, int dwKinCount)
{
    map<int, map<int, SSchemeLegendCupDetail>>::iterator iter =  m_ConfigDetail.find(nDetailID);
    if (iter == m_ConfigDetail.end())
    {
        return 0;
    }
    map<int, SSchemeLegendCupDetail>::iterator iterRound = iter->second.begin();
    if (iterRound == iter->second.end())
    {
        return 0;
    }

    do 
    {
        if (dwKinCount > iterRound->second.m_nMaxGroup * iterRound->second.m_nGroupMaxKin)
        {
            break;
        }
        iterRound++;
    } while (iterRound != iter->second.end());

    if (iterRound == iter->second.end())
    {
        return 0;
    }

    return iterRound->second.m_nRoundID - 1;
}
