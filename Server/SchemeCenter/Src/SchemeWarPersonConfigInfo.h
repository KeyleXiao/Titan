/*******************************************************************
** 文件名:	SchemeWarPersonConfigInfo.h
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
#include "WarDef.h"
#include <map>
using namespace stdext;



// 定义
class CSchemeWarPersonConfigInfo : public ISchemeWarPersonConfigInfo, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarPersonConfigInfo>
{
public:
	/////////////////////////ISchemeWarPersonConfigInfo/////////////////////////
	/** 取得战场相关配置
	*/
	virtual SchemeWarPersonConfigInfo* GetSchemeWarPersonConfigInfo(int WarID);

    virtual SWarPersonConfig* GetWarPersonConfig();

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeWarPersonConfigInfo//////////////////////////
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
	CSchemeWarPersonConfigInfo(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeWarPersonConfigInfo(void);

protected:
	// 加载战场配置数据
	bool loadWarData(TiXmlDocument *pXMLReader);

    // 加载其他附加数据
    bool loadOtherData(TiXmlDocument *pXMLReader);

private:
	//WarID SchemeWarPersonConfigInfo
	map<int, SchemeWarPersonConfigInfo> m_mapSchemeWarConfig;

    SWarPersonConfig m_warPersonConfig;
};