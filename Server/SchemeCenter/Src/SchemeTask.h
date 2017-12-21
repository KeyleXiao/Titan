/*******************************************************************
** 文件名:	SchemeTask.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/13/2017
** 版  本:	1.0
** 描  述:	任务配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "EntityDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeTask : public ISchemeTask, public ISchemeUpdateSink, public CSchemeCache<CSchemeTask>
{
public:
	/////////////////////////ISchemeTask/////////////////////////
    /** 取得任务配置信息
    @return 任务配置信息
    */
    SCHEME_TASK *getTaskScheme();

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeTask//////////////////////////
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
	CSchemeTask(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeTask(void);

private:
	SCHEME_TASK					m_schemeMap;
};