/*******************************************************************
** 文件名:	SchemePlayerKill.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/06/2016
** 版  本:	1.0
** 描  述:	PK
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "EntityDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemePlayerKill : public ISchemePlayerKill, public ISchemeUpdateSink, public CSchemeCache<CSchemePlayerKill>
{
public:
	/////////////////////////ISchemePlayerKill/////////////////////////
	/** 取得PK信息
	@return PK信息
	*/
	virtual SPlayerKill* getPlayerKill(void);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemePlayerKill//////////////////////////
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
	CSchemePlayerKill(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemePlayerKill(void);

private:
	SPlayerKill					m_scheme;
};