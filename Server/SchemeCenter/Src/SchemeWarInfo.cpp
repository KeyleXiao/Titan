#include "StdAfx.h"
#include "SchemeWarInfo.h"
#include "IServerGlobal.h"
#include <string>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeWarInfo::CSchemeWarInfo(void)
{
	m_WarManager = NULL;
}

/** 
@param   
@param   
@return  
*/
CSchemeWarInfo::~CSchemeWarInfo(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeWarInfo::LoadScheme(CSchemeWarManager * pWarManager)
{
	m_WarManager = pWarManager;
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

    string stringPath = SCP_PATH_FORMAT( WARINFO_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("CSchemeWarInfo::LoadScheme failed! filename=" << stringPath.c_str() << endl);
		return false;
	}

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeWarInfo::Close(void)
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
bool CSchemeWarInfo::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	if( m_WarManager == NULL)
	{
		return false;
	}

	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
	{
		return false;
	}

	int nLen = 0;
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		int nIndex = 0;
		SWarInfoScheme item;
		// 序列号
		item.nIndex = pCSVReader->GetInt(nRow, nIndex++, 0);
		// 战场的名字
		int nLen = sizeof(item.szName);
		pCSVReader->GetString(nRow, nIndex++, item.szName, nLen);
		// 是否直接刷出来
		item.bIsUse = pCSVReader->GetInt(nRow, nIndex++, 0) == 0 ? false: true;
		// 阵营
		item.nCamp = pCSVReader->GetInt(nRow, nIndex++, 0);
		// 怪物处理类型（基地，野怪，兵营，塔, 策略怪）
		item.eMonsterType = (EWarMonsterExec)pCSVReader->GetInt(nRow, nIndex++, 0);

        // 塔类型
        item.eTowerType = (EWarTowerType)pCSVReader->GetInt(nRow, nIndex++, 0);

		// 是否共享
		item.bIsShare = pCSVReader->GetInt(nRow, nIndex++, 0) == 0? false:true;

		// 图标类型
		item.nIconIndex = pCSVReader->GetInt(nRow, nIndex++, 0);	

		// 战场ID
		item.nWarID = pCSVReader->GetInt(nRow, nIndex++, 0);

        // 怪物刷新列表数量 记录配置的数量
        item.eMonsterRefreshType = (EWarMonsterRefreshType)pCSVReader->GetInt(nRow, nIndex++, 0);

        //怪物列表
        char strMonsterIndexList[128] = {0};
        nLen = sizeof(strMonsterIndexList);
        pCSVReader->GetString(nRow, nIndex++, strMonsterIndexList, nLen);
        item.nMonsterIndexCount = parseIntArray(strMonsterIndexList, item.nMonsterIndexList, MAX_REFRESH_INDEX_COUNT, ';');
        

        char strIntervalList[128] = {0};
        nLen = sizeof(strIntervalList);
        pCSVReader->GetString(nRow, nIndex++, strIntervalList, nLen);
        item.nIntervalCount = parseIntArray(strIntervalList, item.nInterval, MAX_REFRESH_INTERVAL_COUNT, ';');

        char strnRatioList[128] = {0};
        nLen = sizeof(strnRatioList);
        pCSVReader->GetString(nRow, nIndex++, strnRatioList, nLen);
        std::vector<string> vecTemp;
        StringHelper::split(vecTemp, strnRatioList, ';');
        int nMaxIndexCount = vecTemp.size() > MAX_REFRESH_INDEX_COUNT ? MAX_REFRESH_INDEX_COUNT: vecTemp.size();
        int *pReadData = (int *)(item.nRatioList);
        for (int nIndex = 0; nIndex < nMaxIndexCount; ++nIndex)
        {
            item.nRatioCount += parseIntArray(vecTemp[nIndex].c_str(), pReadData + nIndex* (MAX_REFRESH_INTERVAL_COUNT + 1), MAX_REFRESH_INTERVAL_COUNT + 1, '/');
        }

        // 做校验
        switch (item.eMonsterRefreshType)
        {
        case EWMRT_Normal:
            {
                if (item.nMonsterIndexCount != 1)
                {
                    ErrorLn("EWMRT_Normal nMonsterIndexCount != 1 nRow ="<<nRow);
                    return false;
                }

                if (item.nRatioCount > 0)
                {
                    ErrorLn("EWMRT_Normal nRatioCount > 0 nRow ="<<nRow);
                    return false;
                }

                if (item.nIntervalCount > 0)
                {
                    ErrorLn("EWMRT_Normal nIntervalCount > 0 nRow ="<<nRow);
                    return false;
                }
            }
            break;
        case EWMRT_Random:
            {
                if (item.nMonsterIndexCount <= 1)
                {
                    ErrorLn("EWMRT_Random nMonsterIndexCount <= 1 nRow ="<<nRow);
                    return false;
                }

                if (item.nIntervalCount > 0)
                {
                    ErrorLn("EWMRT_Random nIntervalCount > 0 nRow ="<<nRow);
                    return false;
                }
            }
            break;
        case EWMRT_Interval:
            {
                if (item.nMonsterIndexCount <= 1)
                {
                    ErrorLn("EWMRT_Interval nMonsterIndexCount <= 1 nRow ="<<nRow);
                    return false;
                }

                if (item.nIntervalCount <= 0)
                {
                    ErrorLn("EWMRT_Interval nIntervalListCount <= 0 nRow ="<<nRow);
                    return false;
                }

                // 刷新数量和时间段的概率配置不匹配
                if (item.nMonsterIndexCount != item.nIntervalCount+1)
                {
                    ErrorLn("EWMRT_Interval item.nMonsterIndexCount != item.nIntervalCount+1 nRow ="<<nRow);
                    return false;
                }
            }
            break;
        case EWMRT_RandomInterval:
            {
                if (item.nMonsterIndexCount <= 1)
                {
                    ErrorLn("EWMRT_RandomInterval nMonsterIndexCount <= 1 nRow ="<<nRow);
                    return false;
                }

                if (item.nIntervalCount <= 0)
                {
                    ErrorLn("EWMRT_RandomInterval nIntervalListCount <= 0 nRow ="<<nRow);
                    return false;
                }

                // 刷新数量和时间段的概率配置不匹配
                if (item.nRatioCount != item.nMonsterIndexCount*(item.nIntervalCount+1))
                {
                    ErrorLn("EWMRT_RandomInterval nRatioListCount != item.nMonsterIndexCount*(item.nIntervalCount+1) nRow ="<<nRow);
                    return false;
                }
            }
            break;
        case EWMRT_PosAndMonsterRandom:
            {
                if (item.nMonsterIndexCount <= 0)
                {
                    ErrorLn("EWMRT_PosAndMonsterRandom nMonsterIndexCount <= 0 nRow ="<<nRow);
                    return false;
                }
            }
            break;
        default:
            {
                ErrorLn("check the eMonsterRefreshType"<<item.eMonsterRefreshType<<" nRow ="<<nRow);
                return false;
            }
            break;
        }

		
		// 起始时间
		item.nStartTime = pCSVReader->GetInt(nRow, nIndex++, 0)*1000;
		// 刷新时间
		item.nRefreshTime = pCSVReader->GetInt(nRow, nIndex++, 0)*1000;

		// 同步到哪一行刷新时间
		char strRefreshTimeIndex[128] = {0};
		nLen = sizeof(strRefreshTimeIndex);
		pCSVReader->GetString(nRow, nIndex++, strRefreshTimeIndex, nLen);
		int nCount = parseIntArray(strRefreshTimeIndex, item.SynRefreshTimeIndexList, MAX_WAR_SYN_DEFINE, ';');

		if (nCount > 0 && item.SynRefreshTimeIndexList[0] > 0)
		{
			item.bIsNeedRefreshTime = true;
		}
	

		//基础等级	
		item.nBaseLevel = pCSVReader->GetInt(nRow, nIndex++, 0);
		if(item.nBaseLevel < 0)
		{
			ErrorLn("item.nBaseLevel < item.nBaseLevel < 0 nRow ="<<nRow);
			return false;
		}

		//升级时间
		item.nUpgradeTime = pCSVReader->GetInt(nRow, nIndex++, 0) * 1000;
		if(item.nUpgradeTime < 0)
		{
			ErrorLn("item.nUpgradeTime < 0 nRow ="<<nRow);
			return false;
		}

		// 最大等级
		item.nMaxLevel = pCSVReader->GetInt(nRow, nIndex++, 0);
		if(item.nMaxLevel < 0)
		{
			ErrorLn("item.nMaxLevel < 0 nRow ="<<nRow);
			return false;
		}
		

		// 怪物刷新的时间显示序列
		item.nTimeIndex = pCSVReader->GetInt(nRow, nIndex++, 0);

		// 怪物对应战绩表位置
		item.nTableIndex = pCSVReader->GetInt(nRow, nIndex++, 0);

		// 路径ID
		item.nPathID = pCSVReader->GetInt(nRow, nIndex++, 0);

		
		//出身影响事件列表
		char strEventList[128] = {0};
		nLen = sizeof(strEventList);
		int nEventList[MAX_WAR_EVENT_DEFINE] = {0};
		pCSVReader->GetString(nRow, nIndex++, strEventList, nLen);
		int nEventCount = parseIntArray(strEventList, nEventList, MAX_WAR_EVENT_DEFINE, ';');

		//影响事件索引列表
		char strCountIndexList[128] = {0};
		nLen = sizeof(strCountIndexList);
		int nCountIndexList[MAX_WAR_EVENT_DEFINE] = {0};
		pCSVReader->GetString(nRow, nIndex++, strCountIndexList, nLen);
		int nIndexCount = parseIntArray(strCountIndexList, nCountIndexList, MAX_WAR_EVENT_DEFINE, ';');

		if(nEventCount != nIndexCount)
		{
			ErrorLn("CSchemeWarInfo::OnSchemeLoad nIndexCount != nEventCount nIndex = "<< nIndex<<" nRow ="<<nRow);
			return false;
		}

		//影响参数列表
		char strParmList[128] = {0};
		nLen = sizeof(strParmList);
		int nParmList[MAX_WAR_EVENT_DEFINE] = {0};
		pCSVReader->GetString(nRow, nIndex++, strParmList, nLen);
		int nParmCount = parseIntArray(strParmList, nParmList, MAX_WAR_EVENT_DEFINE, ';');
		if(nEventCount != nParmCount)
		{
			ErrorLn("CSchemeWarInfo::OnSchemeLoad nIndexCount != nParmCount nIndex = "<< nIndex<<" nRow ="<<nRow);
			return false;
		}

		for (int i = 0; i < nEventCount; ++i)
		{
			SWarEventDefine WarEventDefine;
			WarEventDefine.eWarEvent	= (EWarInfoEvent)nEventList[i];
			WarEventDefine.nContIndex	= nCountIndexList[i];
			WarEventDefine.nParm		= nParmList[i];
			item.WarBornEventDefine[i] = WarEventDefine;

            if (WarEventDefine.eWarEvent != EWIE_None)
            {
                item.bHaveBornEvent = true;
            }
			
		}

		//死亡影响事件列表
		char strDeadEventList[128] = {0};
		nLen = sizeof(strDeadEventList);
		int nDeadEventList[MAX_WAR_EVENT_DEFINE] = {0};
		pCSVReader->GetString(nRow, nIndex++, strDeadEventList, nLen);
		int nDeadEventCount = parseIntArray(strDeadEventList, nDeadEventList, MAX_WAR_EVENT_DEFINE, ';');

		//死亡影响事件索引列表
		char strDeadCountIndexList[128] = {0};
		nLen = sizeof(strDeadCountIndexList);
		int nDeadCountIndexList[MAX_WAR_EVENT_DEFINE] = {0};
		pCSVReader->GetString(nRow, nIndex++, strDeadCountIndexList, nLen);
		int nDeadIndexCount = parseIntArray(strDeadCountIndexList, nDeadCountIndexList, MAX_WAR_EVENT_DEFINE, ';');

		if(nDeadEventCount != nDeadIndexCount)
		{
			ErrorLn("CSchemeWarInfo::OnSchemeLoad nIndexCount != nEventCount nIndex = "<< nIndex<<" nRow ="<<nRow);
			return false;
		}

		//死亡影响参数列表
		char strDeadParmList[128] = {0};
		nLen = sizeof(strParmList);
		int nDeadParmList[MAX_WAR_EVENT_DEFINE] = {0};
		pCSVReader->GetString(nRow, nIndex++, strDeadParmList, nLen);
		int nDeadParmCount = parseIntArray(strDeadParmList, nDeadParmList, MAX_WAR_EVENT_DEFINE, ';');
		if(nDeadEventCount != nDeadParmCount)
		{
			ErrorLn("CSchemeWarInfo::OnSchemeLoad nIndexCount != nParmCount nIndex = "<< nIndex<<" nRow ="<<nRow);
			return false;
		}

		for (int i = 0; i < nDeadEventCount; ++i)
		{
			SWarEventDefine WarEventDefine;
			WarEventDefine.eWarEvent	= (EWarInfoEvent)nDeadEventList[i];
			WarEventDefine.nContIndex	= nDeadCountIndexList[i];
			WarEventDefine.nParm		= nDeadParmList[i];
			item.WarDeadEventDefine[i] = WarEventDefine;

            if (WarEventDefine.eWarEvent != EWIE_None)
            {
                item.bHaveDeadEvent = true;
            }

		}

		//// 触发类型
		//item.eWarEvent = (EWarInfoEvent)pCSVReader->GetInt(nRow, nIndex++, 0);
		//// 影响序列号
		//item.nContIndex = pCSVReader->GetInt(nRow, nIndex++, 0);
		//// 参数
		//item.nParm = pCSVReader->GetInt(nRow, nIndex++, 0);
		// 路径类型
		item.ePathType = (EWarSoliderPathType)pCSVReader->GetInt(nRow, nIndex++, 0);

		m_WarManager->AddWarInfo(item.nWarID, item.eMonsterType, item);
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
bool CSchemeWarInfo::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return true;
}
