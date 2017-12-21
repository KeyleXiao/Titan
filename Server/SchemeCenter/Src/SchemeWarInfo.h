/*******************************************************************
** 文件名:	SchemeMonster.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈俊涛
** 日  期:	3/13/2015
** 版  本:	1.0
** 描  述:	战场怪物信息配置
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeWarDef.h"
#include "SchemeWarManager.h"
// 定义
class CSchemeWarInfo :  public ISchemeUpdateSink, public CSchemeCache<CSchemeWarInfo>
{
public:
	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeWarInfo//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool LoadScheme(CSchemeWarManager * pWarManager);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeWarInfo(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeWarInfo(void);

private:
	CSchemeWarManager * m_WarManager;
};