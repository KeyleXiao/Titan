/*******************************************************************
** 文件名:	CSchemeAllMapsArea.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	4/21/2015
** 版  本:	1.0
** 描  述:	
            所有地图的区域配置
********************************************************************/

#include "stdafx.h"
#include "SchemeAllMapsArea.h"
#include "IServerGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeAllMapsArea::CSchemeAllMapsArea()
{

}

/** 
@param   
@param   
@return  
*/
CSchemeAllMapsArea::~CSchemeAllMapsArea()
{
	Close();
}

/** 返回指定地图的区域配置
@param   
@param   
@return  
*/
ISingleMapArea * CSchemeAllMapsArea::getMapArea( int nMapID, AreaType eType )
{
	nMapID = FormatMap(nMapID);

    TMAP_ALLMAPAREAS::iterator it = m_allMaps.find( nMapID );
    if ( it==m_allMaps.end() )
        return NULL;

    TMAP_AREAS::iterator it1 = it->second.find(eType);
    if( it1==it->second.end() )
        return NULL;

    return it1->second;
}

/** 返回指定坐标点所在的区域ID
@param   
@param   
@return  返回0表示不在任何区域中
*/
int CSchemeAllMapsArea::getAreaID( int nMapID,AreaType eType,Vector3 loc )
{
	nMapID = FormatMap(nMapID);

	ISingleMapArea * pMapArea = getMapArea( nMapID,eType );
    return ( pMapArea==NULL )? -1 : pMapArea->getAreaID( loc );
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
bool CSchemeAllMapsArea::OnSchemeLoad( SCRIPT_READER reader,LPCSTR szFileName )
{
#define READ_CONFIG_ERROR(errinfo)\
	ErrorLn( errinfo);\
	Close();\


	TiXmlDocument * pXMLReader = reader.pXMLReader;
	if ( pXMLReader==NULL || szFileName==NULL || reader.type!=READER_XML )
	{
		return false;
	}

    // 先清掉已有的配置
    Close();

    // 读取
    TiXmlElement * pRootElement = pXMLReader->FirstChildElement( "maps" );
    if ( pRootElement==NULL )
    {
        return false;
    }

    for ( TiXmlElement * pSubElement = pRootElement->FirstChildElement( "map" );
          pSubElement!=NULL;
          pSubElement = pSubElement->NextSiblingElement( "map" ) 
        )
    {
        // 读取一个地图的配置
        CSingleMapArea * pMapArea = new CSingleMapArea;
        if ( !pMapArea->parse( pSubElement ) )
        {
            delete pMapArea;
        }
        else
        {
            m_allMaps[pMapArea->getMapID()][pMapArea->getType()] = pMapArea;
        }
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
bool CSchemeAllMapsArea::OnSchemeUpdate( SCRIPT_READER reader,LPCSTR szFileName )
{
	return OnSchemeLoad( reader,szFileName );
}

/** 
@param   
@param   
@return  
*/
bool CSchemeAllMapsArea::LoadScheme()
{
    ISchemeEngine * pSchemeEngine = gServerGlobal->getSchemeEngine();
    if ( pSchemeEngine==NULL )
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT( MAPAREA_SCHEME_FILENAME );	
    bool bResult = pSchemeEngine->LoadScheme( stringPath.c_str(),(ISchemeUpdateSink *)this,false );
    if ( !bResult )
    {
        ErrorLn("CSchemeAllMapsArea::LoadScheme error stringPath=" << stringPath.c_str());
        return false;
    }
    // 加载每个地图的区域文件
    for(TMAP_ALLMAPAREAS::iterator it = m_allMaps.begin(); it != m_allMaps.end(); ++it)
    {
        for(TMAP_AREAS::iterator it1 = it->second.begin(); it1 != it->second.end(); ++it1)
        {
            it1->second->load();
            //it1->second->print();
        }
    }

    return true;
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeAllMapsArea::Close()
{
    for(TMAP_ALLMAPAREAS::iterator it = m_allMaps.begin(); it != m_allMaps.end(); ++it)
    {
        for(TMAP_AREAS::iterator it1 = it->second.begin(); it1 != it->second.end(); ++it1)
        {
            delete it1->second;
            //it1->second->print();
        }
    }

    m_allMaps.clear();
}
