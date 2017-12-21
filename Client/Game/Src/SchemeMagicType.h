/*******************************************************************
** 文件名:	SchemeMagicType.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/09/2015
** 版  本:	1.0
** 描  述:	魔法类型配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeMagicType : public ISchemeMagicType, public ISchemeUpdateSink
{
public:
	/////////////////////////ISchemeMagicType/////////////////////////
	/** 取得魔法类型配置信息
	@param nType:	类型
	@return 魔法类型配置信息
	*/
	virtual SMagicType* getMagicType(int nType);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeMagicType//////////////////////////
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
	CSchemeMagicType(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeMagicType(void);

private:
	// 魔法类型表
	typedef map<int, SMagicType>		SCHEME_MAP;
	SCHEME_MAP							m_schemeMap;
};