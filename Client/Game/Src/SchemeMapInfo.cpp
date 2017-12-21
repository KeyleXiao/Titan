 #include "stdafx.h"
#include "SchemeMapInfo.h"
#include "IClientGlobal.h"
#include "EntityViewDef.h"
#include <string>

/** 
@param   
@param   
@return  
*/
CSchemeMapInfo::CSchemeMapInfo(void)
{
	// 所有地图配置属性	
	m_mapSchemeInfo.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemeMapInfo::~CSchemeMapInfo(void)
{
	// 所有地图配置属性	
	m_mapSchemeInfo.clear();
}

/** 
@param   
@param   
@return  
*/
bool CSchemeMapInfo::LoadScheme(void)
{
	ISchemeEngine * pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL )
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( MAPINFO_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this, false);
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
void CSchemeMapInfo::Close(void)
{	
	// 所有地图配置属性	
	m_mapSchemeInfo.clear();
}

/** 取得地图配置信息
@param   
@param   
@return  
*/
SMapSchemeInfo * CSchemeMapInfo::GetMapSchemeInfo(long lMapID)
{
	TMAP_MAPINFO::iterator itM = m_mapSchemeInfo.find(lMapID);
	if(itM != m_mapSchemeInfo.end())
	{
		return &((*itM).second);
	}

	return NULL;
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
bool CSchemeMapInfo::OnSchemeLoad(SCRIPT_READER reader, LPCSTR szFileName)
{
	TiXmlDocument * pTiXmlDocument = reader.pXMLReader;
	if(pTiXmlDocument == NULL || szFileName == NULL || reader.type != READER_XML)
	{
		return false;
	}

	// 所有地图配置属性	
	m_mapSchemeInfo.clear();

	TiXmlElement * pRootElement = pTiXmlDocument->FirstChildElement("scheme");
	if(pRootElement == NULL)
	{
		return false;
	}

	TiXmlElement * pSubElement = pRootElement->FirstChildElement("map");
	if(pSubElement == NULL)
	{
		return false;
	}

	while(pSubElement)
	{
		SMapSchemeInfo mapschemeinfo;

		// 地图ID
		LPCSTR pszAttrProp = pSubElement->Attribute("mapid", (int *)&mapschemeinfo.lMapID);
		if(pszAttrProp == NULL || mapschemeinfo.lMapID <= INVALID_MAPID || mapschemeinfo.lMapID >= MAP_MAXCOUNT)		
		{
			ErrorLn("地图ID不能小于等于0,并且不能大于"<<MAP_MAXCOUNT<<", mapid="<<mapschemeinfo.lMapID);
			return false;
		}

		if(mapschemeinfo.lMapID >= 0xffff/*不能大于65535，因为物品生成唯一ID的时候，会需要这个地图ID*/)
		{
			ErrorLn("地图ID超过0xFFFF,mapid="<<mapschemeinfo.lMapID);
			return false;
		}

		// 不能有重复地图ID，并且不能大于最大地图数
		if(GetMapSchemeInfo(mapschemeinfo.lMapID) != NULL)
		{
			ErrorLn("重复的地图ID mapid="<<mapschemeinfo.lMapID);
			return false;
		}

		// 地图名字
		pszAttrProp = pSubElement->Attribute("mapname");
		if(pszAttrProp == NULL)
		{
			return false;
		}
		sstrcpyn(mapschemeinfo.szName, pszAttrProp, sizeof(mapschemeinfo.szName));

		// 开放场景服ID
		TiXmlElement * pChildElement = pSubElement->FirstChildElement("open");
		if(pChildElement == NULL)
		{
			return false;
		}
		pszAttrProp = pChildElement->Attribute("ZoneServerIDForJustLaunch", (int *)&mapschemeinfo.lZoneServerIDForJustLaunch);
		if(pszAttrProp == NULL)		
		{
			return false;
		}

		pszAttrProp = pChildElement->Attribute("ZoneServerIDForLongLaunch", (int *)&mapschemeinfo.lZoneServerIDForLongLaunch);
		if(pszAttrProp == NULL)		
		{
			return false;
		}

		pszAttrProp = pChildElement->Attribute("ZoneServerIDForCivilization", (int *)&mapschemeinfo.lZoneServerIDForCivilization);
		if(pszAttrProp == NULL)		
		{
			ErrorLn("地图ID没有配置 ZoneServerIDForCivilization mapid="<<mapschemeinfo.lMapID << " mapName = "<< mapschemeinfo.szName);
			return false;
		}

		if(mapschemeinfo.lZoneServerIDForJustLaunch <= 0 && mapschemeinfo.lZoneServerIDForLongLaunch <= 0 && mapschemeinfo.lZoneServerIDForCivilization <= 0)
		{
			pSubElement = pSubElement->NextSiblingElement("map");
			continue;
		}

		// 启动是否载入， 1：载入。2：不载入
		pChildElement = pSubElement->FirstChildElement("startload");
		if(pChildElement == NULL)
		{
			return false;
		}
		pszAttrProp = pChildElement->Attribute("flagForJustLaunch", (int *)&mapschemeinfo.lStartLoadForJustLaunch);
		if(pszAttrProp == NULL)		
		{
			return false;
		}

		pszAttrProp = pChildElement->Attribute("flagForLongLaunch", (int *)&mapschemeinfo.lStartLoadForLongLaunch);
		if(pszAttrProp == NULL)		
		{
			return false;
		}

		pszAttrProp = pChildElement->Attribute("flagForCivilization", (int *)&mapschemeinfo.lStartLoadfForCivilization);
		if(pszAttrProp == NULL)		
		{
			ErrorLn("地图ID没有配置 flagForCivilization mapid="<<mapschemeinfo.lMapID<< " mapName = "<< mapschemeinfo.szName);
			return false;
		}
		

		// 是否动态载入， 1：动态。2：不动态
		pChildElement = pSubElement->FirstChildElement("dynamic");
		if(pChildElement == NULL)
		{
			return false;
		}
		pszAttrProp = pChildElement->Attribute("flag", (int *)&mapschemeinfo.lDynamicFlag);
		if(pszAttrProp == NULL)		
		{
			return false;
		}
		
		// 地图文件集合
		pChildElement = pSubElement->FirstChildElement("mapfile");
		if(pChildElement == NULL)
		{
			return false;
		}
		pszAttrProp = pChildElement->Attribute("filename");
		if(pszAttrProp == NULL)		
		{
			return false;
		}
		sstrcpyn(mapschemeinfo.szMapFile, pszAttrProp, sizeof(mapschemeinfo.szMapFile));
		
		// 地图笼子脚本文件名
		pChildElement = pSubElement->FirstChildElement("cagefile");
		if(pChildElement == NULL)
		{
			return false;
		}
		pszAttrProp = pChildElement->Attribute("filename");
		if(pszAttrProp == NULL)		
		{
			return false;
		}
		sstrcpyn(mapschemeinfo.szCageFile, pszAttrProp, sizeof(mapschemeinfo.szCageFile));

		// 怪物生成配置文件
		pChildElement = pSubElement->FirstChildElement("monsterbuildfile");
		if(pChildElement == NULL)
		{
			return false;
		}
		pszAttrProp = pChildElement->Attribute("filename");
		if(pszAttrProp == NULL)		
		{
			return false;
		}
		sstrcpyn(mapschemeinfo.szMonsterBuildFile, pszAttrProp, sizeof(mapschemeinfo.szMonsterBuildFile));

		// 传送门生成配置文件
		pChildElement = pSubElement->FirstChildElement("chunnelbuildfile");
		if(pChildElement == NULL)
		{
			return false;
		}
		pszAttrProp = pChildElement->Attribute("filename");
		if(pszAttrProp == NULL)		
		{
			return false;
		}
		sstrcpyn(mapschemeinfo.szChunnelBuildFile, pszAttrProp, sizeof(mapschemeinfo.szChunnelBuildFile));

		// 草药，矿等等的生成配置文件
		pChildElement = pSubElement->FirstChildElement("boxbuildfile");
		if(pChildElement == NULL)
		{
			return false;
		}
		pszAttrProp = pChildElement->Attribute("filename");
		if(pszAttrProp == NULL)		
		{
			return false;
		}
		sstrcpyn(mapschemeinfo.szBoxBuildFile, pszAttrProp, sizeof(mapschemeinfo.szBoxBuildFile));

		// 背景音乐id
		pChildElement = pSubElement->FirstChildElement("backmusicid");
		if(pChildElement == NULL)
		{
			return false;
		}
		pszAttrProp = pChildElement->Attribute("sid", (int *)&mapschemeinfo.lBackMusicID);
		if(pszAttrProp == NULL)		
		{
			return false;
		}

		// 地图像素大小
		pChildElement = pSubElement->FirstChildElement("mappixelsize");
		if(pChildElement == NULL)
		{
			return false;
		}

		// 地图像素宽
		pszAttrProp = pChildElement->Attribute("width", (int *)&mapschemeinfo.lMapPixelWidth);
		if(pszAttrProp == NULL)		
		{
			return false;
		}

		// 地图像素高
		pszAttrProp = pChildElement->Attribute("height", (int *)&mapschemeinfo.lMapPixelHeight);
		if(pszAttrProp == NULL)		
		{
			return false;
		}

		// 是否需要光复
		pChildElement = pSubElement->FirstChildElement("recover");
		if(pChildElement == NULL)
		{
			mapschemeinfo.lRecover = 0;
		}
		else
		{
			pszAttrProp = pChildElement->Attribute("flag", (int *)&mapschemeinfo.lRecover);
			if(pszAttrProp == NULL)		
			{
				mapschemeinfo.lRecover = 0;
			}
		}	

		if (mapschemeinfo.lRecover == 1)
		{
			// 光复前 怪物生成配置文件
			pChildElement = pSubElement->FirstChildElement("recovermonsterbuildfile");
			if(pChildElement == NULL)
			{
				return false;
			}
			pszAttrProp = pChildElement->Attribute("filename");
			if(pszAttrProp == NULL)		
			{
				return false;
			}
			sstrcpyn(mapschemeinfo.szRecoverMonsterBuildFile, pszAttrProp, sizeof(mapschemeinfo.szRecoverMonsterBuildFile));

			// 光复前 草药，矿等等的生成配置文件
			pChildElement = pSubElement->FirstChildElement("recoverboxbuildfile");
			if(pChildElement == NULL)
			{
				return false;
			}
			pszAttrProp = pChildElement->Attribute("filename");
			if(pszAttrProp == NULL)		
			{
				return false;
			}
			sstrcpyn(mapschemeinfo.szRecoverBoxBuildFile, pszAttrProp, sizeof(mapschemeinfo.szRecoverBoxBuildFile));
		}

		// 地图偏移信息
		pChildElement = pSubElement->FirstChildElement("mapoffset");
		if(pChildElement == NULL)
		{
			return false;
		}

        double dValue = 0.0f;
		// x
		pszAttrProp = pChildElement->Attribute("x", &dValue);
		if(pszAttrProp == NULL)		
		{
			return false;
		}
        mapschemeinfo.vOffset.x = CAST_TYPE(float, dValue);
		// y
		pszAttrProp = pChildElement->Attribute("y", &dValue);
		if(pszAttrProp == NULL)		
		{
			return false;
		}
        mapschemeinfo.vOffset.y = CAST_TYPE(float, dValue);
		// z
		pszAttrProp = pChildElement->Attribute("z", &dValue);
		if(pszAttrProp == NULL)		
		{
			return false;
		}
        mapschemeinfo.vOffset.z = CAST_TYPE(float, dValue);

		// 寻路方式
		pChildElement = pSubElement->FirstChildElement("mappathfind");
		if(pChildElement == NULL)
		{
			return false;
		}
		pszAttrProp = pChildElement->Attribute("serviceid", (int *)&mapschemeinfo.uPathFindServiceID);
		if(pszAttrProp == NULL)		
		{
			return false;
		}

        // 地图x轴 z轴长度
        pChildElement = pSubElement->FirstChildElement("map_size");
        if(pChildElement == NULL)
        {
            ErrorLn("CSchemeMapInfo::OnSchemeLoad [map_size] not found");
            return false;
        }

        pszAttrProp = pChildElement->Attribute("xaixs_len", (int*)&mapschemeinfo.nXAxisLen);
        if(pszAttrProp == NULL)		
        {
            ErrorLn("CSchemeMapInfo::OnSchemeLoad [xaixs_len] not found");
            return false;
        }

        pszAttrProp = pChildElement->Attribute("zaixs_len", (int*)&mapschemeinfo.nZAxisLen);
        if(pszAttrProp == NULL)		
        {
            ErrorLn("CSchemeMapInfo::OnSchemeLoad [zaixs_len] not found");
            return false;
        }

        // 高度场文件
        pChildElement = pSubElement->FirstChildElement("height_field");
        if(pChildElement != NULL)
        {
            pszAttrProp = pChildElement->Attribute("filename");
            if(pszAttrProp != NULL)
            {
                sstrcpyn(mapschemeinfo.szHeightFieldFile, pszAttrProp, sizeof(mapschemeinfo.szHeightFieldFile));
            }
        }


		// 压入map
		m_mapSchemeInfo[mapschemeinfo.lMapID] = mapschemeinfo;

		// 读取下一个
		pSubElement = pSubElement->NextSiblingElement("map");
	}

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
bool CSchemeMapInfo::OnSchemeUpdate(SCRIPT_READER reader, LPCSTR szFileName)
{
	return false;
}

/** 格式化地图特殊配置信息
@param   
@param   
@return  
*/
bool CSchemeMapInfo::FormatMapSchemeInfo(void)
{
	
	return true;
}