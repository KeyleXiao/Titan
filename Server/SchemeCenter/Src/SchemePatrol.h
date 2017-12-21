/*******************************************************************
** 文件名:	SchemePatrol.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	饶建超
** 日  期:	3/24/2015
** 版  本:	1.0
** 描  述:	巡逻配置表 Patrol.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeCache.h"

// 定义
class CSchemePatrol : public ISchemePatrol, public ISchemeUpdateSink, public CSchemeCache<CSchemePatrol>
{
public:
	/////////////////////////ISchemePatrol/////////////////////////
	/** 取得巡逻配置信息
	@param nPatrolId:	怪物ID
	@return  
	*/
	virtual bool			getPatrolShemeInfo(int nPatrolID, vector<Vector3> &vctPatrolPath);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemePatrol//////////////////////////
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
	CSchemePatrol(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemePatrol(void);

private:
	// 怪物ID为索引
	typedef std::map<int, vector<Vector3> > TMAP_PATROLSCHEME;
	TMAP_PATROLSCHEME				m_mapPatrolScheme;
};