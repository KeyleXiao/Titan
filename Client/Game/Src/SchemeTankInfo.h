/*******************************************************************
** 文件名:	SchemeTankInfo.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	1/9/2015
** 版  本:	1.0
** 描  述:	怪物配置表 Tank.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "DTankModule.h"



// 定义
class CSchemeTankInfo : public ISchemeTankInfo, public ISchemeUpdateSink
{
public:
	/////////////////////////ISchemeTankInfo/////////////////////////
	/** 取得怪物配置信息
	@param nMonsterId:	怪物ID
	@return  
	*/
	virtual STankSchemeInfo* getTankShemeInfo(int nMonsterId);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	/** CSV配置载入时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/ 
	virtual bool									OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);


	/** 配置动态更新时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool									OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeTankInfo//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool											LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void											Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeTankInfo(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeTankInfo(void);

private:
	// 怪物ID为索引
	typedef std::map<int, STankSchemeInfo> TMAP_TANKSCHEME;
	TMAP_TANKSCHEME				m_mapTankScheme;
};