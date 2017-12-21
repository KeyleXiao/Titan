/*******************************************************************
** 文件名:	SchemeMatchRoomRule.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	战场配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "Match_SchemeDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeMatchRoomRule : public ISchemeMatchRoomRule, public ISchemeUpdateSink, public CSchemeCache<CSchemeMatchRoomRule>
{
public:
	/////////////////////////ISchemeMatchRoomRule/////////////////////////
	/** 取得战场相关配置
	*/
	virtual SchemeMatchRoomRule* getSchemeMatchRoomRule(int RuleID);


	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeMatchRoomRule//////////////////////////
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
	CSchemeMatchRoomRule(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeMatchRoomRule(void);

private:
	//WarID SchemeMatchRoomRule
	map<int, SchemeMatchRoomRule> m_mapSchemeRule;
};