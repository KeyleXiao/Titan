#include "stdafx.h"
#include "SchemePathFindService.h"
#include "IClientGlobal.h"
#include <string>

/** 
@param   
@param   
@return  
*/
CSchemePathFindServiceInfo::CSchemePathFindServiceInfo(void)
{
	// 所有地图配置属性	
	m_SchemeInfo.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemePathFindServiceInfo::~CSchemePathFindServiceInfo(void)
{
	// 所有地图配置属性	
	m_SchemeInfo.clear();
}

/** 
@param   
@param   
@return  
*/
bool CSchemePathFindServiceInfo::LoadScheme(void)
{
	ISchemeEngine * pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL )
	{
		return false;
	}

    string stringPath = SCP_PATH_FORMAT( PATHFIND_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this, false);
	if(!bResult)
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	return true;
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemePathFindServiceInfo::Close(void)
{	
	// 所有地图配置属性	
	m_SchemeInfo.clear();
}

/** 取得地图配置信息
@param   
@param   
@return  
*/
PathFindServiceScheme * CSchemePathFindServiceInfo::GetSchemeInfo(unsigned int uPathFindID)
{
	T_SchemeInfo::iterator itM = m_SchemeInfo.find( uPathFindID );
	if(itM != m_SchemeInfo.end())
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
bool CSchemePathFindServiceInfo::OnSchemeLoad(SCRIPT_READER reader, LPCSTR szFileName)
{
	TiXmlDocument * pTiXmlDocument = reader.pXMLReader;
	if(pTiXmlDocument == NULL || szFileName == NULL || reader.type != READER_XML)
	{
		return false;
	}
	m_SchemeInfo.clear();

	TiXmlElement * pRootElement = pTiXmlDocument->FirstChildElement("Scheme");
	if(pRootElement == NULL)
	{
		return false;
	}
	TiXmlElement * pSubElement = pRootElement->FirstChildElement("PathFindService");
	if(pSubElement == NULL)
	{
		return false;
	}

	PathFindServiceScheme SchemeInfo;
	LPCSTR pszAttrProp = nullptr;
	TiXmlElement * pChildElement = nullptr;
	while(pSubElement)
	{
		pszAttrProp = pSubElement->Attribute("id", (int *)&SchemeInfo.uPathFindServiceID);
		if(pszAttrProp == NULL || 0 == SchemeInfo.uPathFindServiceID)		
		{
			ErrorLn("寻路服务ID不能等于0");
			return false;
		}
		if(nullptr != GetSchemeInfo(SchemeInfo.uPathFindServiceID))
		{
			ErrorLn("重复的寻路服务ID=" << SchemeInfo.uPathFindServiceID);
			return false;
		}

        // NavMesh文件配置
		pChildElement = pSubElement->FirstChildElement("NavMeshFile");
		if(pChildElement == NULL)
		{
			return false;
		}
		pszAttrProp = pChildElement->Attribute("filename");
		if(pszAttrProp == NULL)
		{
			return false;
		}
		sstrcpyn(SchemeInfo.szName, pszAttrProp, sizeof(SchemeInfo.szName));

        // CHF文件配置
        pChildElement = pSubElement->FirstChildElement("ChfFile");
        if(pChildElement)
        {
            pszAttrProp = pChildElement->Attribute("filename");
            if(pszAttrProp)
            {
                sstrcpyn(SchemeInfo.szChfName, pszAttrProp, sizeof(SchemeInfo.szChfName));
            }
        }
        
		m_SchemeInfo[ SchemeInfo.uPathFindServiceID ] = SchemeInfo;
		pSubElement = pSubElement->NextSiblingElement("PathFindService");
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
bool CSchemePathFindServiceInfo::OnSchemeUpdate(SCRIPT_READER reader, LPCSTR szFileName)
{
	return false;
}

void CSchemePathFindServiceInfo::GetAllID( vector<unsigned int>& vecRet )
{
	vecRet.clear();
	if ( m_SchemeInfo.empty() )
	{
		return;
	}

	vecRet.reserve( m_SchemeInfo.size() );
	T_SchemeInfo::iterator itor = m_SchemeInfo.begin();
	for ( ; itor != m_SchemeInfo.end(); ++itor)
	{
		vecRet.push_back( itor->second.uPathFindServiceID );
	}
}

void CSchemePathFindServiceInfo::GetAllPath2ID( map<string, unsigned int>& ret )
{
	ret.clear();
	if ( m_SchemeInfo.empty() )
	{
		return;
	}

	T_SchemeInfo::iterator itor = m_SchemeInfo.begin();
	for ( ; itor != m_SchemeInfo.end(); ++itor)
	{
		ret.insert( make_pair(itor->second.szName , itor->second.uPathFindServiceID) );
	}
}
