/*******************************************************************
** 文件名:	e:\Rocket\Server\ZoneServer\Src\SchemeMapInfo.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/1/24  16:55
** 版  本:	1.0
** 描  述:	地图信息配置脚本
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 陈俊涛
** 日  期: 2015-03-04
** 描  述: 移植
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"
#include "SchemeCache.h"

using namespace std;

class CSchemeMapInfo : public ISchemeMapInfo, public ISchemeUpdateSink, public CSchemeCache<CSchemeMapInfo>
{
	typedef map<long, SMapSchemeInfo>	TMAP_MAPINFO;
public:
	///////////////////////////////ISchemeMapInfo///////////////////	
	/** 取得地图配置信息
	@param   
	@param   
	@return  
	*/
	virtual SMapSchemeInfo * GetMapSchemeInfo(long lMapID);

	virtual void GetMapSchemeInfoNoPathID(vector<SMapSchemeInfo*>& vecRet);

	/** 格式化地图特殊配置信息
	@param   
	@param   
	@return  
	*/
	virtual bool FormatMapSchemeInfo(void);

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
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);

	/** 配置动态更新时通知
	@param   type   : 读取器类型
					: 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
					: READER_XML : 就可以把pReader转换成TiXmlDocument*
	@param   pReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	///////////////////////////////CSchemeMapInfo//////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeMapInfo(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeMapInfo(void);

private:
	// 所有地图配置属性	
	TMAP_MAPINFO m_mapSchemeInfo;
};
