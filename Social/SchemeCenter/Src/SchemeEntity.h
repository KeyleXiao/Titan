/*******************************************************************
** 文件名:	SchemeEntity.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	实体通用配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "EntityDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeEntity : public ISchemeEntity, public ISchemeUpdateSink, public CSchemeCache<CSchemeEntity>
{
public:
	/////////////////////////ISchemeEntity/////////////////////////
    virtual SEntityXml_Server *getEntityXml(void);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeEntity//////////////////////////
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
	CSchemeEntity(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeEntity(void);

private:
	SEntityXml_Server           m_EntityXml;
};