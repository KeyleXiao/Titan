/*******************************************************************
** 文件名:	SchemeMentorshipDataCenter.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	12/2/2016
** 版  本:	1.0
** 描  述:	师徒相关配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "MentorshipDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeMentorshipDataCenter : public ISchemeMentorshipDataCenter, public ISchemeUpdateSink
{
public:
	/////////////////////////ISchemeMentorshipDataCenter/////////////////////////
    /// purpose: 取得师徒常量信息
    virtual SMentorshipConfig*     getConfig();


	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeMentorshipDataCenter//////////////////////////
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
	CSchemeMentorshipDataCenter(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeMentorshipDataCenter(void);

private:

    /// purpose:  加载师徒常量脚本
    bool LoadConfigScheme(ICSVReader * pCSVReader);

private:

    // 师徒常量
    SMentorshipConfig m_Config;
};