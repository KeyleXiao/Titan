/*******************************************************************
** 文件名:	SchemePersonBaseProp.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	3/27/2016
** 版  本:	1.0
** 描  述:	人物基础属性配置
********************************************************************/

#pragma once
#include "Singleton.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"


// 定义
class CSchemePersonBaseProp : public ISchemePersonBaseProp, public ISchemeUpdateSink
{
public:
	/////////////////////////ISchemePersonBaseProp/////////////////////////
    virtual SCHEME_MAP*             getAllHeroInfo();
    virtual SPersonBasePropScheme   getOneHeroDetailInfo(int nHeroID);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemePersonBaseProp//////////////////////////
	bool											LoadScheme(void);
	void											Close(void);

	CSchemePersonBaseProp(void);
	virtual ~CSchemePersonBaseProp(void);

private:
    // 基础属性表
    typedef map<int, SPersonBasePropScheme>		SCHEME_MAP;
    SCHEME_MAP									m_schemeMap;
};