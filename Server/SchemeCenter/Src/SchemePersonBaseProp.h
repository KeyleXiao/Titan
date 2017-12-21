/*******************************************************************
** 文件名:	SchemePersonBaseProp.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/17/2015
** 版  本:	1.0
** 描  述:	人物基础属性配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "EntityDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemePersonBaseProp : public ISchemePersonBaseProp, public ISchemeUpdateSink, public CSchemeCache<CSchemePersonBaseProp>
{
public:
	/////////////////////////ISchemePersonBaseProp/////////////////////////
	/** 取得人物基础属性配置信息
	@param nVocation:	职业ID
	@return 人物基础属性配置信息
	*/
	virtual SPersonBasePropScheme* getPersonBaseProp(int nVocation, int nPropType);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemePersonBaseProp//////////////////////////
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
	CSchemePersonBaseProp(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemePersonBaseProp(void);

private:
    bool loadPropScheme(SCRIPT_READER reader,const char* szFileName);

    bool loadPropHkeeScheme(SCRIPT_READER reader,const char* szFileName);

private:
	// 基础属性表
	typedef map<int, SPersonBasePropScheme>		SCHEME_MAP;
	SCHEME_MAP									m_schemeMap;
    SCHEME_MAP                                  m_schemeHkeeMap;
};