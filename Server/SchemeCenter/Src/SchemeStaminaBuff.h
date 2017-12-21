/*******************************************************************
** 文件名:	SchemeStaminaBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/17/2015
** 版  本:	1.0
** 描  述:	体力buff配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "EntityDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeStaminaBuff : public ISchemeStaminaBuff, public ISchemeUpdateSink, public CSchemeCache<CSchemeStaminaBuff>
{
public:
	/////////////////////////ISchemeStaminaBuff/////////////////////////
	/** 取得体力BUFF配置信息
	@param nVocation:	职业ID
	@return 体力BUFF配置信息
	*/
	virtual SStaminaBuff* getStaminaBuff(int nVocation);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeStaminaBuff//////////////////////////
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
	CSchemeStaminaBuff(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeStaminaBuff(void);

private:
	// 体力buff表
	typedef map<int, SStaminaBuff>		SCHEME_MAP;
	SCHEME_MAP							m_schemeMap;
};