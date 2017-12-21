/*******************************************************************
** 文件名:	SchemeWarPersonConfig.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	3/10/2016
** 版  本:	1.0
** 描  述:	实体通用配置
********************************************************************/

#pragma once
#include "ISchemeCenter.h"
#include "ISchemeEngine.h"
#include "WarDef.h"

// 人物基础属性配置表
#define WAR_SCHEME_PERSON_CONFIG				"Scp\\WarPersonConfig.xml"

class CSchemeWarPersonConfig:public ISchemeUpdateSink, public ISchemeWarPersonConfig
{
public:
    /////////////////////////ISchemeUpdateSink/////////////////////////
    virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
    virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////ISchemeWarPersonConfig/////////////////////
    virtual SWarPersonConfig* CSchemeWarPersonConfig::GetSchemeWarPersonConfigInfo();
	virtual const SchemeWarPersonConfigInfo* GetSchemeWarPersonConfigInfo(int WarID);

    /////////////////////////CSchemeWarPersonConfig////////////////////
    bool                    LoadScheme(void);
    void					Close(void);

    CSchemeWarPersonConfig(void);

	virtual ~CSchemeWarPersonConfig(void);

private:
	// 加载战场配置
	bool loadWarData(TiXmlDocument *pXMLReader);

	// 加载其他数据
	bool loadOtherData(TiXmlDocument *pXMLReader);

private:

    SWarPersonConfig m_warPersonConfig;

	std::map<int, SchemeWarPersonConfigInfo> m_mapSchemeWarConfig;
};