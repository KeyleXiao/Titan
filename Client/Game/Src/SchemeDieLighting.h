/*******************************************************************
** 文件名:	SchemeDieLighting.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/17/2015
** 版  本:	1.0
** 描  述:	死亡光效配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeDieLighting : public ISchemeDieLighting, public ISchemeUpdateSink
{
public:
	/////////////////////////ISchemeDieLighting/////////////////////////
	/** 取得死亡光效配置信息
	@param nVocation:	职业ID,怪物ID
	@param nType:		实体类型
	@return 死亡光效配置信息
	*/
	virtual SDieLighting* getDieLighting(int nVocation, int nType);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeDieLighting//////////////////////////
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
	CSchemeDieLighting(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeDieLighting(void);

private:
	// 死亡光效表
	typedef map<SDieLightingKey, SDieLighting>		SCHEME_MAP;
	SCHEME_MAP							m_schemeMap;
};