/*******************************************************************
** 文件名:	SchemeSpellUpgrade.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/30/2015
** 版  本:	1.0
** 描  述:	技能升级配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "EntityDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeSpellUpgrade : public ISchemeSpellUpgrade, public ISchemeUpdateSink, public CSchemeCache<CSchemeSpellUpgrade>
{
public:
	/////////////////////////ISchemeSpellUpgrade/////////////////////////
	/** 取得技能升级配置信息
	@param nVocation:	职业
	@param vSpellUpgrade:	技能升级容器数据
	@return 技能升级配置信息
	*/
	virtual void getSpellUpgrade(int nVocation, vector<SSpellUpgrade> &vSpellUpgrade);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeSpellUpgrade//////////////////////////
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
	CSchemeSpellUpgrade(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeSpellUpgrade(void);

private:
	// 技能升级容器
	typedef vector<SSpellUpgrade>		SCHEME_VECTOR;
	// 技能升级表
	typedef map<int, SCHEME_VECTOR>		SCHEME_MAP;
	SCHEME_MAP							m_schemeMap;
};