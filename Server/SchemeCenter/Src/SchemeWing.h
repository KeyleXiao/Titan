/*******************************************************************
** 文件名:	SchemeWing.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/21/2016
** 版  本:	1.0
** 描  述:	翅膀配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "EntityDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeWing : public ISchemeWing, public ISchemeUpdateSink, public CSchemeCache<CSchemeWing>
{
public:
	/////////////////////////ISchemeWing/////////////////////////
	/** 取得翅膀配置信息
	@param nID:	翅膀ID
	@return 翅膀配置信息
	*/
	virtual SWing* getWing(int nID);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeWing//////////////////////////
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
	CSchemeWing(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeWing(void);

private:
	// 翅膀表
	typedef map<int, SWing>		SCHEME_MAP;
	SCHEME_MAP					m_schemeMap;
};