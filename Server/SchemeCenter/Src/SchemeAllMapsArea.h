/*******************************************************************
** 文件名:	CSchemeAllMapsArea.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	4/21/2015
** 版  本:	1.0
** 描  述:	
            所有地图的区域配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"
#include "SchemeCache.h"
#include "SingleMapArea.h"

// 所有地图区域
class CSchemeAllMapsArea :
    public ISchemeAllMapsArea, public ISchemeUpdateSink, public CSchemeCache<CSchemeAllMapsArea>
{
    typedef std::map< int,CSingleMapArea * >    TMAP_AREAS;  // 一个地图的不同类型的区域配置
    typedef std::map< int, TMAP_AREAS >  TMAP_ALLMAPAREAS;   // 所有地图的区域配置
    TMAP_ALLMAPAREAS m_allMaps;                     // 所有地图

public:
    /** 
	@param   
	@param   
	@return  
	*/
	CSchemeAllMapsArea();

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeAllMapsArea();

    /** 返回指定地图的区域配置
	@param   
	@param   
	@return  
	*/
	virtual ISingleMapArea * getMapArea( int nMapID, AreaType eType );

	/** 返回指定坐标点所在的区域ID
	@param   
	@param   
	@return  
	*/
	virtual int getAreaID( int nMapID,AreaType eType,Vector3 loc );

	///////////////////////////////ISchemeUpdateSink///////////////////	
	/** CSV配置载入时通知
	@param   type   : 读取器类型
		            : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
					: READER_XML : 就可以把pReader转换成TiXmlDocument*
	@param   pReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/ 
	virtual bool OnSchemeLoad( SCRIPT_READER reader,const char* szFileName );

	/** 配置动态更新时通知
	@param   type   : 读取器类型
		            : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
					: READER_XML : 就可以把pReader转换成TiXmlDocument*
	@param   pReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool OnSchemeUpdate( SCRIPT_READER reader,const char* szFileName );

	///////////////////////////// CSchemeAllMapsArea ////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool LoadScheme();

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void Close();
};
