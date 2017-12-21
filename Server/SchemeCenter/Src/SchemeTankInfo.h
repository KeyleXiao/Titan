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
#include "SchemeCache.h"
#include "DTankModule.h"

// 定义
class CSchemeTankInfo : public ISchemeTankInfo, public ISchemeUpdateSink, public CSchemeCache<CSchemeTankInfo>
{
public:
	/////////////////////////ISchemeTankInfo/////////////////////////
	/** 取得怪物配置信息
	@param nMonsterId:	怪物ID
	@return  
	*/
	virtual STankSchemeInfo* getTankShemeInfo(int nTankId);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

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