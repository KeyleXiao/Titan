/*******************************************************************
** 文件名:	SchemeSpeedDecay.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/03/2016
** 版  本:	1.0
** 描  述:	速度衰减
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "EntityDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeSpeedDecay : public ISchemeSpeedDecay, public ISchemeUpdateSink, public CSchemeCache<CSchemeSpeedDecay>
{
public:
	/////////////////////////ISchemeSpeedDecay/////////////////////////
	/** 取得速度衰减信息
	@param nSpeed:	速度
	@return 速度衰减信息
	*/
	virtual SSpeedDecay* getSpeedDecay(int nSpeed);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeSpeedDecay//////////////////////////
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
	CSchemeSpeedDecay(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeSpeedDecay(void);

private:
	typedef vector<SSpeedDecay>		SCHEME_VECTOR;
	SCHEME_VECTOR					m_schemeVector;
};