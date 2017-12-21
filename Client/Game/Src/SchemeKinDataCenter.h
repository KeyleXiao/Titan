/*******************************************************************
** 文件名:	SchemeKinDataCenter.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	战队相关配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "KinSchemeDef.h"
#include "KinDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeKinDataCenter : public ISchemeKinDataCenter, public ISchemeUpdateSink
{
public:
	/////////////////////////ISchemeKinDataCenter/////////////////////////
    /// purpose: 取得战队常量信息
    virtual SKinConfig*     getConfig();

    virtual SKinLevel*      getLevelInfo(int nKinLevel);


	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeKinDataCenter//////////////////////////
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
	CSchemeKinDataCenter(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeKinDataCenter(void);

private:

    /// purpose:  加载战队常量脚本
    bool LoadConfigScheme(ICSVReader * pCSVReader);

    /// purpose:  加载战队升级脚本
    bool LoadLevelScheme(ICSVReader * pCSVReader);

private:

    // 战队常量
    SKinConfig m_Config;

    // 战队升级
    SKinLevel m_LevelInfoList[KIN_MAX_LEVEL + 1];
};