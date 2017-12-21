/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\SchemeCenter\Src\SchemePathFindService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/12/2015	18:36
** 版  本:	1.0
** 描  述:	
            寻路服务配置表读取
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"

using namespace std;

class CSchemePathFindServiceInfo : public IPathFindServiceInfo, public ISchemeUpdateSink
{
	typedef map<unsigned int, PathFindServiceScheme>	T_SchemeInfo;
public:
	///////////////////////////////IPathFindServiceInfo///////////////////	
	virtual PathFindServiceScheme * GetSchemeInfo(unsigned int uPathFindID);
	virtual void					GetAllID(vector<unsigned int>& vecRet);
	virtual void					GetAllPath2ID(map<string, unsigned int>& ret);

	///////////////////////////////ISchemeUpdateSink///////////////////	
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	///////////////////////////////CSchemePathFindServiceInfo//////////////////////
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
	CSchemePathFindServiceInfo(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemePathFindServiceInfo(void);

private:
	// 所有寻路服务配置属性	
	T_SchemeInfo m_SchemeInfo;
};
