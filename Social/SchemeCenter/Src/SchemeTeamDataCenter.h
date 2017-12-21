/*******************************************************************
** 文件名:	SchemeTeamDataCenter.h
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
#include "TeamSchemeDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeTeamDataCenter : public ISchemeTeamDataCenter, public ISchemeUpdateSink, public CSchemeCache<CSchemeTeamDataCenter>
{
public:
	/////////////////////////ISchemeTeamDataCenter/////////////////////////
    /// purpose: 取得命令配置信息
    virtual const SGroupCommandSchemeInfo* GetCommandSchemeInfo(DWORD dwCommandID);


	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeTeamDataCenter//////////////////////////
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
	CSchemeTeamDataCenter(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeTeamDataCenter(void);

private:

    /// purpose:  加载战队常量脚本
    bool LoadCommandScheme(ICSVReader * pCSVReader);



private:

    // 命令脚本列表,以命令ID为关键字
    TMAP_GroupCommandSchemeInfo	m_mapCommandSchemeInfo;
};