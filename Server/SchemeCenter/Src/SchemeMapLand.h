/*******************************************************************
** 文件名:	e:\Rocket\Server\ZoneServer\Src\SchemeMapLand.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/1/24  16:29
** 版  本:	1.0
** 描  述:	地图着陆配置
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

#include "SchemeMapInfo.h"
using namespace std;

class CSchemeMapLand : public ISchemeMapLand, public ISchemeUpdateSink, public CSchemeCache<CSchemeMapLand>
{
	typedef map<BYTE, SMapLandSchemeInfo >		TMAP_MAPModeLAND;       // 阵营ID为KEY
	typedef map<long, TMAP_MAPModeLAND >		TMAP_MAPLAND;       	// mapID为KEY

public:
	/////////////////////////ISchemeMapLand/////////////////////////
	/** 取得地图着陆配置信息
	@param   lMapID ：何个地图
    @param   nModeType:EGameMode 游戏类型（这个参数应当不起过滤作用）
	@param   nCampID ：阵营ID
	@return  
	*/
	virtual SMapLandSchemeInfo * GetMapLandSchemeInfo(long lMapID, BYTE nModeType, int nCampID);

	/** 取得地图着陆配置信息
	@param   lMapID ：何个地图
	@param   nModeType:EGameMode 游戏类型（这个参数应当不起过滤作用）
	@param   nCampID ：阵营ID
	@return
	*/
	virtual SMapLandSchemeInfo * GetRandMapLandSchemeInfo(long lMapID, BYTE nModeType);

	/////////////////////////ISchemeUpdateSink/////////////////////////
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

	/////////////////////////CSchemeMapLand//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool LoadScheme(CSchemeMapInfo* pMapInfo);

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
	CSchemeMapLand(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeMapLand(void);

private:

	/** 检测地图是否存在 
	@param   
	@param   
	@return  
	*/
	bool IsMapExist(DWORD dwMapID);

	/** 获取出生点 
	@param   
	@param   
	@return  
	*/
	bool parsePointList(ICSVReader* pCSVReader, int nRow, int nIndex,  std::vector<SPosWithAngle>& refList);

	/** 获取坐标点
	@param   
	@param   
	@return 
	*/
	bool parsePoint(ICSVReader* pCSVReader, int nRow, int nIndex, Vector3& pos);

private:
	// 地图着陆配置
	TMAP_MAPLAND m_mapMapLand;

	// 地图配置信息
	CSchemeMapInfo* m_pMapInfo;
};