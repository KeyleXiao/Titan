/*******************************************************************
** 文件名:	SchemeWarPoint.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈俊涛
** 日  期:	3/13/2015
** 版  本:	1.0
** 描  述:	战场怪物信息配置
********************************************************************/
#pragma once

#include <vector>
#include <map>
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeWarDef.h"
using namespace std;


// 根据序列号，保存怪物ID
typedef map<int, WarPointVect> WarPointMap;

// 定义
class CSchemeWarPoint : public ISchemeWarPoint, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarPoint>
{
public:
	// 通过序列号，获得怪物信息
	WarPointVect GetWarPointByIndex(int nIndex);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeWarPoint//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool LoadScheme(void);

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
	CSchemeWarPoint(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeWarPoint(void);
private:
	WarPointMap m_WarPointMap;
};