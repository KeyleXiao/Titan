#include "StdAfx.h"
#include "SchemeMapLand.h"
#include "IServerGlobal.h"
#include <string>

/** 
@param   
@param   
@return  
*/
CSchemeMapLand::CSchemeMapLand(void)
{	
	// 地图着陆配置
	m_mapMapLand.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemeMapLand::~CSchemeMapLand(void)
{	
	// 地图着陆配置
	m_mapMapLand.clear();
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeMapLand::LoadScheme(CSchemeMapInfo* pMapInfo)
{
	if (pMapInfo==NULL)
	{
		return false;
	}

	m_pMapInfo = pMapInfo;

	ISchemeEngine * pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本	
    string stringPath = SCP_PATH_FORMAT( MAPLAND_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this, false);
	if(!bResult)
	{
		Error("CSchemeMapLand::LoadScheme stringPath= " << stringPath.c_str() << endl);
		return false;
	}

	return true;
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeMapLand::Close(void)
{	
	// 地图着陆配置
	m_mapMapLand.clear();
}

/** 取得地图着陆配置信息
@param   lMapID ：何个地图
@param   nModeType:EGameMode 游戏类型（这个参数应当不起过滤作用）
@param   nCampID ：阵营ID
@return  
*/
SMapLandSchemeInfo * CSchemeMapLand::GetMapLandSchemeInfo(long lMapID, BYTE nModeType, int nCampID)
{
    lMapID = FormatMap(lMapID);
	TMAP_MAPLAND::iterator itMap = m_mapMapLand.find(lMapID);
	if(itMap != m_mapMapLand.end())
	{
        TMAP_MAPModeLAND & mapCampList = itMap->second;

        // 用阵营ID来查找
		TMAP_MAPModeLAND::iterator itMapNode = mapCampList.find(nCampID);
        return (itMapNode != mapCampList.end() ? &(itMapNode->second) : NULL);
	}

	return NULL;
}

SMapLandSchemeInfo * CSchemeMapLand::GetRandMapLandSchemeInfo(long lMapID, BYTE nModeType)
{
	return GetMapLandSchemeInfo(lMapID, nModeType, -1);
}

/** CSV配置载入时通知
@param   type   : 读取器类型
		        : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
				: READER_XML : 就可以把pReader转换成TiXmlDocument*
@param   pReader：读取CSV的返回接口
@param   szFileName：配置文件名
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/
bool CSchemeMapLand::OnSchemeLoad(SCRIPT_READER reader, const char* szFileName)
{	
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
	{
		return false;
	}

	// 地图着陆配置
	m_mapMapLand.clear();

	char szTemp[256] = {0};
	long lArray[10] = {0};
	
	// 重载
	for(size_t i = 0; i < pCSVReader->GetRecordCount(); i++)
	{
		// 记录列序号
		int nColumnIndex = 0;

		// 地图ID		
		long lMapID = pCSVReader->GetInt(i, nColumnIndex, 0); nColumnIndex++;
		if(lMapID <= INVALID_MAPID || lMapID >= MAP_MAXCOUNT)
		{
			ErrorLn("invalid mapid.mapid="<<lMapID);
			return false;
		}

		if(!IsMapExist(lMapID))
		{
			continue;
		}
		
		SMapLandSchemeInfo schemeinfo;
		// 模式
		schemeinfo.nModelType = pCSVReader->GetInt(i, nColumnIndex, 0); nColumnIndex++;
		// 阵营ID
		schemeinfo.nCampID = pCSVReader->GetInt(i, nColumnIndex, 0); nColumnIndex++;
		// 出生点
        //TraceLn(__FUNCTION__":map="<<lMapID << ", vecBornPos.....");
		if ( !parsePointList(pCSVReader, i, nColumnIndex++, schemeinfo.vecBornPos) )
		{
			ErrorLn("CSchemeMapLand::parsePointList !born points parse error, mapid="<<lMapID);
			return false;
		}

        // 复活点
        //TraceLn(__FUNCTION__":map="<<lMapID << ", vecRebornPos.....");
        if ( !parsePointList(pCSVReader, i, nColumnIndex++, schemeinfo.vecRebornPos) )
        {
            ErrorLn("CSchemeMapLand::parsePointList !reborn points parse error mapid="<<lMapID);
            return false;
        }
        // 回城点
        //TraceLn(__FUNCTION__":map="<<lMapID << ", vecTransportBackPos.....");
        if ( !parsePointList(pCSVReader, i, nColumnIndex++, schemeinfo.vecTransportBackPos) )
        {
            ErrorLn("CSchemeMapLand::parsePointList !transport back points parse error mapid="<<lMapID);
            return false;
        }

		// 复活倒计时长
		schemeinfo.lReliveCountDown = pCSVReader->GetInt(i, nColumnIndex++, 0); 

		// 复活消耗的物品
		schemeinfo.lReliveGoods = pCSVReader->GetInt(i, nColumnIndex++, 0);

		// 区域中心
		if (parsePoint(pCSVReader, i, nColumnIndex++, schemeinfo.ptCenterZone) == false)
		{
			ErrorLn("CSchemeMapLand::parsePoint !transport back points parse error mapid=" << lMapID);
			return false;
		}

		// 区域半径
		schemeinfo.fRadiusZone = pCSVReader->GetFloat(i, nColumnIndex++, 0.f);

		// 阵营间隔
		schemeinfo.fCampDistance = pCSVReader->GetFloat(i, nColumnIndex++, 0.f);

		//if (schemeinfo.isRandomPos)
		//{
		//	// 取第一个阵营的区域随机出生信息
		//	if (m_mapRandBornPos.find(lMapID) == m_mapRandBornPos.end())
		//	{
		//		m_mapRandBornPos[lMapID].ptCenterZone = schemeinfo.ptCenterZone;
		//		m_mapRandBornPos[lMapID].fCampDistance = schemeinfo.fCampDistance;
		//		m_mapRandBornPos[lMapID].fRadiusZone = schemeinfo.fRadiusZone;
		//	}
		//	// 保存需要随机出生点的阵营
		//	m_mapRandBornPos[lMapID].vCampList.push_back(schemeinfo.nCampID);
		//}

		// 放入map中
		//TMAP_MAPLAND::iterator itMap = m_mapMapLand.find(lMapID);
		//if(itMap != m_mapMapLand.end())
		//{
		//	TMAP_MAPModeLAND * pMapModeLand = &((*itMap).second);
		//	TMAP_MAPModeLAND::iterator itMode = pMapModeLand->find(schemeinfo.nModelType);
		//	if (itMode != pMapModeLand->end())
		//	{
		//		itMode->second.push_back(schemeinfo);
		//	}
		//	else
		//	{
		//		TVECT_SchemeLand tempLand;
		//		tempLand.push_back(schemeinfo);
		//		(*pMapModeLand)[schemeinfo.nModelType] = tempLand;
		//	}
		//	
		//}
		//else
		//{
		//	TMAP_MAPModeLAND tempMapMode;
		//	TVECT_SchemeLand tempLand;
		//	tempLand.push_back(schemeinfo);
		//	tempMapMode[schemeinfo.nModelType] = tempLand;

		//	m_mapMapLand[lMapID] = tempMapMode;
		//}
        TMAP_MAPModeLAND & mapCampList = m_mapMapLand[lMapID];
        // 将阵营数据加入容器
        mapCampList[schemeinfo.nCampID] = schemeinfo;
	}

	AfterSchemeLoaded();
	return true;
}

/** 配置动态更新时通知
@param   type   : 读取器类型
				: 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
				: READER_XML : 就可以把pReader转换成TiXmlDocument*
@param   pReader：读取CSV的返回接口
@param   szFileName：配置文件名
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/
bool CSchemeMapLand::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeMapLand* pNewInstance = NewInstance();
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

/** 检测地图是否存在 
@param   
@param   
@return  
*/
bool CSchemeMapLand::IsMapExist(DWORD dwMapID)
{
	if(m_pMapInfo == NULL)
	{
		return false;
	}
	
	if(m_pMapInfo->GetMapSchemeInfo(dwMapID) == NULL)
	{
		return false;
	}

	return true;
}

bool CSchemeMapLand::parsePointList( ICSVReader* pCSVReader, int nRow, int nIndex, std::vector<SPosWithAngle>& refList )
{
	static uint32 const scu32CharCountMax = 1024;
	static uint32 const scu32PointCountMax = 100;
	static uint32 const scu32PointFloatCount = 4;

    refList.clear();

	char stTemp[ scu32CharCountMax ];
	int nLen = scu32CharCountMax;
	pCSVReader->GetString(nRow, nIndex, stTemp, nLen);
	if (nLen > scu32CharCountMax)
	{
		ErrorLn("CSchemeMapLand::parsePointList nCount > scu32CharCountMax= " << scu32CharCountMax);
		return false;
	}
	
	float fPos[scu32PointCountMax * scu32PointFloatCount];
	int nCount = parseFloatArray(stTemp, fPos, scu32PointCountMax * scu32PointFloatCount , ';');
	if ((nCount % scu32PointFloatCount )!= 0)
	{
		ErrorLn("CSchemeMapLand::parsePointList (nCount % " << scu32PointFloatCount << " )!= 0 nCount= " << nCount);
		return false;
	}

	refList.reserve( nCount / scu32PointFloatCount );
	for (int i = 0; i < nCount; i += scu32PointFloatCount)
	{
		SPosWithAngle Pos;
		Pos.ptPos.x = fPos[i];
		Pos.ptPos.y = fPos[i+1];
		Pos.ptPos.z = fPos[i+2];
		Pos.fAngleY = fPos[i+3];

		refList.push_back( Pos );
        //TraceLn(__FUNCTION__":x="<<Pos.ptPos.x<<",y="<<Pos.ptPos.y<<",z="<<Pos.ptPos.z<<", angle="<< Pos.fAngleY);
	}
	return true;
}

bool CSchemeMapLand::parsePoint(ICSVReader * pCSVReader, int nRow, int nIndex, Vector3 & pos)
{
	static uint32 const scu32CharCountMax = 1024;
	static uint32 const scu32PointCountMax = 1;
	static uint32 const scu32PointFloatCount = 3;

	char stTemp[scu32CharCountMax];
	int nLen = scu32CharCountMax;
	pCSVReader->GetString(nRow, nIndex, stTemp, nLen);
	if (nLen > scu32CharCountMax)
	{
		ErrorLn("CSchemeMapLand::parsePoint nCount > scu32CharCountMax= " << scu32CharCountMax);
		return false;
	}

	float fPos[scu32PointCountMax * scu32PointFloatCount];
	int nCount = parseFloatArray(stTemp, fPos, scu32PointCountMax * scu32PointFloatCount, ';');
	if ((nCount % scu32PointFloatCount) != 0)
	{
		ErrorLn("CSchemeMapLand::parsePoint (nCount % " << scu32PointFloatCount << " )!= 0 nCount= " << nCount);
		return false;
	}

	pos.x = fPos[0];
	pos.y = fPos[1];
	pos.z = fPos[2];

	return true;
}
