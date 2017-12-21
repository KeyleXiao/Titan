/*******************************************************************
** 文件名:	SchemeMatchComputerName.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	组队相关配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "MatchSchemeDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeMatchComputerName : public ISchemeMatchComputerName, public ISchemeUpdateSink, public CSchemeCache<CSchemeMatchComputerName>
{
public:
	/////////////////////////ISchemeMatchComputerName/////////////////////////
    /// purpose: 取得命令配置信息
    virtual string GetComputerNameByIndex(int nIndex);

    virtual int GetConfigNameCount();


	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeMatchComputerName//////////////////////////
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
	CSchemeMatchComputerName(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeMatchComputerName(void);

private:

    /// purpose:  加载电脑名字脚本
    bool LoadConfigScheme(ICSVReader * pCSVReader);



private:

    // 命令脚本列表,以命令ID为关键字
    map<int,SMatchComputerName>	m_mapNameSchemeInfo;
};