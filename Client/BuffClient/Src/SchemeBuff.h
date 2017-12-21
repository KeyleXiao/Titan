/*******************************************************************
** 文件名:	SchemeBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/12  21:53
** 版  本:	1.0
** 描  述:	buff配置脚本
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IBuffClient.h"
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include <list>
#include <map>
using namespace std;
using namespace stdext;

// buff配置脚本名字
#define BUFF_SCHEME_FILENAME			"Scp\\buff.xml"

// buff配置
class CSchemeBuff : public ISchemeUpdateSink
{	
public:
	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	///////////////////////////////CSchemeBuff//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool					LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void					Close(void);

	/** 取得BUFF配置信息
	@param   
	@param   
	@return  
	*/
	SBuffSchemeInfo *		GetBuffSchemeInfo(DWORD dwBuffID);

	/** 取得所有配置信息
	@param   
	@param   
	@return  
	*/
	TMAP_BUFFSCHEMEINFO *	GetBuffSchemeInfo(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeBuff(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeBuff(void);

private:
	// 所有BUFF配置属性	
	TMAP_BUFFSCHEMEINFO		m_mapBuffSchemeInfo;
};
