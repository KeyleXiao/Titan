#include "StdAfx.h"
#include "SchemeWarPoint.h"
#include "IServerGlobal.h"
#include "EntityDef.h"
#include <string>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeWarPoint::CSchemeWarPoint(void)
{	

}

/** 
@param   
@param   
@return  
*/
CSchemeWarPoint::~CSchemeWarPoint(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeWarPoint::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }

    string stringPath = SCP_PATH_FORMAT( WARPOINT_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        Error("CSchemeWarPoint::LoadScheme failed! filename=" << stringPath.c_str() << endl);
        return false;
    }

    return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeWarPoint::Close(void)
{	
    // 清空
    m_WarPointMap.clear();
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
bool CSchemeWarPoint::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
    {
        return false;
    }
    // 清空
    m_WarPointMap.clear();

    int nLen = 0;
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow = 0; nRow < nRecordCount; ++nRow)
    {
        int nIndex = 0;
        SWarPointScheme item;
        // 怪物列表序列号
        item.nIndex = pCSVReader->GetInt(nRow, nIndex++, 0);
        // 怪物ID
        item.nMonsterID = pCSVReader->GetInt(nRow, nIndex++, 0);
        // 怪物名字，不处理
        nIndex++;

        // 朝向信息
        item.fAX = pCSVReader->GetFloat(nRow, nIndex++, 0);
        item.fAY = pCSVReader->GetFloat(nRow, nIndex++, 0);
        item.fAZ = pCSVReader->GetFloat(nRow, nIndex++, 0);

        // 坐标信息
        char strXPosList[52] = {0};
        int nPosLen = sizeof(strXPosList);
        
        pCSVReader->GetString(nRow, nIndex++, strXPosList, nPosLen);
        int nXCount = parseFloatArray(strXPosList, item.fX, MAX_WAR_POINT_POS_RADOM, ';');

        char strYPosList[52] = {0};
        nPosLen = sizeof(strYPosList);
        pCSVReader->GetString(nRow, nIndex++, strYPosList, nPosLen);
        int nYCount = parseFloatArray(strYPosList, item.fY, MAX_WAR_POINT_POS_RADOM, ';');

        char strZPosList[52] = {0};
        nPosLen = sizeof(strZPosList);
        pCSVReader->GetString(nRow, nIndex++, strZPosList, nPosLen);
        int nZCount = parseFloatArray(strZPosList, item.fZ, MAX_WAR_POINT_POS_RADOM, ';');
        if (nXCount != nYCount || nXCount != nZCount)
        {
            ErrorLn(__FUNCTION__": nXCount != nYCount || nXCount != nZCount "<<item.nMonsterID)
            return  false;
        }
        item.nPosCount = nZCount;

        //广播类型列表
        char strBroadCastTypeList[128] = {0};
        nLen = sizeof(strBroadCastTypeList);
        int nBroadCastTypeList[MAX_WAR_BROADCAST_TYPE_COUNT] = {0};
        pCSVReader->GetString(nRow, nIndex++, strBroadCastTypeList, nLen);
        int nBroadCastTypeCount = parseIntArray(strBroadCastTypeList, nBroadCastTypeList, MAX_WAR_BROADCAST_TYPE_COUNT, ';');

        //广播索引列表(广播id)
        char strBroadCastIndexList[128] = {0};
        nLen = sizeof(strBroadCastIndexList);
        int nBroadCastIndexList[MAX_WAR_BROADCAST_TYPE_COUNT] = {0};
        pCSVReader->GetString(nRow, nIndex++, strBroadCastIndexList, nLen);
        int nBroadCastIDCount = parseIntArray(strBroadCastIndexList, nBroadCastIndexList, MAX_WAR_BROADCAST_TYPE_COUNT, ';');

        //广播参数列表
        char strBroadCastParamList[128] = {0};
        nLen = sizeof(strBroadCastParamList);
        int nBroadCastParamList[MAX_WAR_BROADCAST_TYPE_COUNT] = {0};
        pCSVReader->GetString(nRow, nIndex++, strBroadCastParamList, nLen);
        int nBroadCastParameterCount = parseIntArray(strBroadCastParamList, nBroadCastParamList, MAX_WAR_BROADCAST_TYPE_COUNT, ';');

		for (int i = 0; i < nBroadCastTypeCount; ++i)
		{
			SWarBroadCastTypeInfo WarCastTypeInfo;
			WarCastTypeInfo.eBroadCastType	= (EWarBroadCastType)nBroadCastTypeList[i];
			WarCastTypeInfo.nIndex			= nBroadCastIndexList[i];
			item.BroadCastTypeList[i]		= WarCastTypeInfo;

            if (WarCastTypeInfo.eBroadCastType != EWBCT_None)
            {
                item.bNeedBroadCast = true;
            }
		}
        for (int i = 0; i < nBroadCastIDCount; ++i)
        {
            SWarBroadCastIDInfo WarCastIDInfo;
            WarCastIDInfo.nBroadCastID	= nBroadCastIndexList[i];
            item.BroadCastIDList[i]		= WarCastIDInfo;
        }
        for (int i = 0; i < nBroadCastParameterCount; ++i)
        {
            SWarBroadCastParameterInfo WarCastParameterInfo;
            WarCastParameterInfo.nBroadCastParameter = nBroadCastParamList[i];
            item.BroadCastParameterList[i]           = WarCastParameterInfo;
        }
        // 位置索引
        item.nLocalIndex = pCSVReader->GetInt(nRow, nIndex++, 0);

        // 经验类型
        int nExpType = pCSVReader->GetInt(nRow, nIndex++, 0);
        if( nExpType >= EWEXP_MAX)
        {
            WarningLn(__FUNCTION__": not have this exp type nExpType = " << nExpType);
            continue;
        }

        item.nAddEXPType = (EWarAddEXPType)nExpType;

        // 经验
        item.nExp = pCSVReader->GetInt(nRow, nIndex++, 0);

        // 掉落天赋类型
        int nTmpTalentType = pCSVReader->GetInt(nRow, nIndex++, 0);
        if ( nTmpTalentType >= WETT_MAX)
        {
            WarningLn(__FUNCTION__": not have this Talent type nTmpTalentType = " << nTmpTalentType);
            continue;
        }
        item.nTalentType = (EWarTalentType)nTmpTalentType ;

        // 天赋值
        item.nTalentValue = pCSVReader->GetInt(nRow, nIndex++, 0);

        // 掉落衰减系数
        item.fDecayFactor = pCSVReader->GetFloat(nRow, nIndex++, 0);

        // 掉落buff类型
        int nTmpBuffType = pCSVReader->GetInt(nRow, nIndex++, 0);
        if ( nTmpBuffType >= EWBT_MAX )
        {
            WarningLn(__FUNCTION__": not have this buff type nTmpBuffType = " << nTmpBuffType);
            continue;
        }
        item.nBuffType = (EWarBuffType)nTmpBuffType;

        // buff值
        item.nBuffValue = pCSVReader->GetInt(nRow, nIndex++, 0);

        // buff等级
        item.nBuffLevel = pCSVReader->GetInt(nRow, nIndex++, 0);

        // 经验掉落范围
        item.nDistanceValue = pCSVReader->GetInt(nRow, nIndex++, 0);

        item.nDistanceTalent = pCSVReader->GetInt(nRow, nIndex++, 0);

        // 累积多少次数刷怪
        item.nRefreshCumulant = pCSVReader->GetInt(nRow, nIndex++, 0);

        // 同步累积多少次数刷怪列
        item.nSynRefreshCumulantIndex = pCSVReader->GetInt(nRow, nIndex++, 0);

        // 开启小地图信息防守进攻标志
        item.nMiniMapAttackProtectFlag = pCSVReader->GetInt(nRow, nIndex++, 0);

		// 气势掉落类型
		item.eVigourPathType = (EWarSoliderPathType)pCSVReader->GetInt(nRow, nIndex++, 0);

		// 气势掉落增加值
		item.nVigourAddValue = pCSVReader->GetInt(nRow, nIndex++, 0);
        // 从SchemeMonster中的MonsterID获取其怪物类型
        SMonsterScheme * pSMonsterScheme = gServerGlobal->getSchemeCenter()->getSchemeMonster()->getMonsterShemeInfo(item.nMonsterID);
        if (pSMonsterScheme == NULL)
        {
            ErrorLn(__FUNCTION__": pSMonsterScheme == NULL nMonsterID ="<<item.nMonsterID)
            return false;
        }
        else
        {
            item.bIsInvisibleMonster = (pSMonsterScheme->nMonsType == MONSTER_TYPE_WILD_MONITOR);
        }

        // 插入到列表中
        WarPointMap::iterator it = m_WarPointMap.find( item.nIndex);
        if(it != m_WarPointMap.end() )
        {
            it->second.push_back(item);
        }
        else
        {
            WarPointVect tempWarPoint;
            tempWarPoint.push_back(item);
            m_WarPointMap[item.nIndex] = tempWarPoint;
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
bool CSchemeWarPoint::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeWarPoint* pNewInstance = NewInstance();
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


// 通过序列号，获得怪物信息
WarPointVect CSchemeWarPoint::GetWarPointByIndex(int nIndex)
{
    WarPointVect temp;
    WarPointMap::iterator it = m_WarPointMap.find(nIndex);
    if(it == m_WarPointMap.end())
    {
        Error("CSchemeWarPoint::GetWarPointByIndex failed! nIndex=" << nIndex << endl);
        return temp;
    }

    return it->second;
}