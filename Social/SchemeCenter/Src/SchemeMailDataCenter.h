/*******************************************************************
** 文件名:	SchemeMailDataCenter.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	12/2/2016
** 版  本:	1.0
** 描  述:	邮件相关配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "MailDef.h"
#include <map>
using namespace stdext;
using namespace rkt;

// 定义
class CSchemeMailDataCenter : public ISchemeMailDataCenter, public ISchemeUpdateSink, public CSchemeCache<CSchemeMailDataCenter>
{
	// 配置文件对应的列
	enum SCHEME_MAIL_COLUMN
	{
		COLUNM_MAIL_ID = 0,					    // Id
		COLUNM_MAIL_LIFETIME,					// 有效时间
		COLUNM_MAIL_TITLE,						// 标题
		COLUNM_MAIL_CONTENT,					// 内容
		COLUNM_MAIL_SENDERNAME,					// 发送者名字
		COLUNM_MAIL_ARRAYPRIZE,					// 显示奖励数组
	};

public:
	/////////////////////////ISchemeMailDataCenter/////////////////////////
    /// purpose: 取得邮件常量信息
    virtual SMailConfig*     getConfig(int nMailFillID);


	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeMailDataCenter//////////////////////////
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
	CSchemeMailDataCenter(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeMailDataCenter(void);

private:

    /// purpose:  加载邮件常量脚本
    bool LoadConfigScheme(ICSVReader * pCSVReader);

private:

    // 邮件常量
	std::map<int, SMailConfig>     m_mapMailScheme;
};