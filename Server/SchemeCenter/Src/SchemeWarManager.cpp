#include "StdAfx.h"
#include "SchemeWarManager.h"
#include "IServerGlobal.h"
#include <string>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeWarManager::CSchemeWarManager(void)
{	
	
}

/** 
@param   
@param   
@return  
*/
CSchemeWarManager::~CSchemeWarManager(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeWarManager::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

    string stringPath = SCP_PATH_FORMAT( WARMANAGER_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("CSchemeWarManager::CSchemeWarManager failed! filename=" << stringPath.c_str() << endl);
		return false;
	}

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeWarManager::Close(void)
{	
	// 清空
	m_WarConfigMap.clear();
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
bool CSchemeWarManager::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{

	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_WarConfigMap.clear();

	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		int nIndex = 0;
		SWarConfig itemWar;
		// 战场静态ID
		itemWar.sWarScheme.nStaticID = pCSVReader->GetInt(nRow, nIndex++, 0);

		// 战场的名字
		int nLen = sizeof(itemWar.sWarScheme.szName);
		pCSVReader->GetString(nRow, nIndex++, itemWar.sWarScheme.szName, nLen);
		// 游戏类型
		itemWar.sWarScheme.nGameType = pCSVReader->GetInt(nRow, nIndex++, 0);
		// 玩法类型
		itemWar.sWarScheme.nPlayType = pCSVReader->GetInt(nRow, nIndex++, 0);
		
        //流程广播时间列表
        char strTimeParmList[128] = {0};
        nLen = sizeof(strTimeParmList);
        int nTimeParmList[MAX_WAR_BROADCAST_PROCESS] = {0};
        pCSVReader->GetString(nRow, nIndex++, strTimeParmList, nLen);
        int nTimeParmCount = parseIntArray(strTimeParmList, nTimeParmList, MAX_WAR_BROADCAST_PROCESS, ';');

        //流程广播时间ID列表
        char strTimeIDParmList[128] = {0};
        nLen = sizeof(strTimeIDParmList);
        int nTimeIDParmList[MAX_WAR_BROADCAST_PROCESS] = {0};
        pCSVReader->GetString(nRow, nIndex++, strTimeIDParmList, nLen);
        int nTimeIDParmCount = parseIntArray(strTimeIDParmList, nTimeIDParmList, MAX_WAR_BROADCAST_PROCESS, ';');

        if (nTimeParmCount != nTimeIDParmCount)
        {
            ErrorLn("error count can't match check the file WarManager.csv");
            return false;
        }


        for (int i = 0; i < nTimeParmCount; ++i)
        {
            SBroadcastProcess BroadcastProcess;
            BroadcastProcess.BroadCastTime = nTimeParmList[i];
            BroadcastProcess.BroadCastID = nTimeIDParmList[i];
            itemWar.sWarScheme.BroadCastTimeList[i] = BroadcastProcess;
        }
        itemWar.sWarScheme.nBroadcastProcessCount = nTimeParmCount;
        itemWar.sWarScheme.MaxMonsterCount = pCSVReader->GetInt(nRow, nIndex++, 0);

		m_WarConfigMap[itemWar.sWarScheme.nStaticID] = itemWar;
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
bool CSchemeWarManager::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return false;
}

/** 添加战场描述信息
@param nWarStaticID: 战场ID  
@param eType： 刷新的类型
@param sWarInfo：怪物的信息
@return 是否添加成功
*/
bool CSchemeWarManager::AddWarInfo(int nWarStaticID, EWarMonsterExec eType, SWarInfoScheme sWarInfo)
{
	WarConfigMap::iterator itWar = m_WarConfigMap.find(nWarStaticID);
	if(itWar != m_WarConfigMap.end())
	{
		SWarInfoSchemeMap::iterator itInfo = itWar->second.sWarInfoSchemeMap.find(eType);
		if(itInfo != itWar->second.sWarInfoSchemeMap.end())
		{
			itInfo->second.push_back(sWarInfo);
		}
		else
		{
			SWarInfoSchemeVect tempInfoVect;
			tempInfoVect.push_back(sWarInfo);
			itWar->second.sWarInfoSchemeMap[eType] = tempInfoVect;
		}
		bool bIsInsert = false;
		EWarMonsTypeVect::iterator itType = itWar->second.eWarMonsTypeVect.begin();
		for(; itType != itWar->second.eWarMonsTypeVect.end(); ++itType)
		{
			if(*itType == eType)
			{
				bIsInsert = true;
				break;
			}
		}
		if(!bIsInsert)
		{
			itWar->second.eWarMonsTypeVect.push_back(eType);
		}
	}

	return true;
}


// 通过静态ID和类型，获得当前怪物的的列表
SWarInfoSchemeVect* CSchemeWarManager::GetWarInfoSchemeMap(int nStaticID, EWarMonsterExec eType)
{
	WarConfigMap::iterator itWar = m_WarConfigMap.find(nStaticID);
	if(itWar == m_WarConfigMap.end())
	{
		Error("CSchemeWarManager::GetWarInfoSchemeMap failed! nStaticID=" << nStaticID << endl);
		return NULL;
	}

	SWarInfoSchemeMap::iterator itInfo = itWar->second.sWarInfoSchemeMap.find(eType);
	if(itInfo == itWar->second.sWarInfoSchemeMap.end())
	{
		Error("CSchemeWarManager::GetWarInfoSchemeMap failed! eType=" << eType << endl);
		return NULL;
	}

	return &(itInfo->second);
}


// 通过静态ID获得当前战场信息
SWarScheme CSchemeWarManager::GetWarScheme(int nStaticID)
{
	SWarScheme temp;
	WarConfigMap::iterator itWar = m_WarConfigMap.find(nStaticID);
	if(itWar == m_WarConfigMap.end())
	{
		Error("CSchemeWarManager::GetWarInfoSchemeMap failed! nStaticID=" << nStaticID << endl);
		return temp;
	}

	return itWar->second.sWarScheme;
}

// 获得刷新类型
EWarMonsTypeVect* CSchemeWarManager::GetWarEMonstTypeVect(int nStaticID)
{
	WarConfigMap::iterator itWar = m_WarConfigMap.find(nStaticID);
	if(itWar == m_WarConfigMap.end())
	{
		Error("CSchemeWarManager::GetWarInfoSchemeMap failed! nStaticID=" << nStaticID << endl);
		return NULL;
	}

	return &(itWar->second.eWarMonsTypeVect);
}
