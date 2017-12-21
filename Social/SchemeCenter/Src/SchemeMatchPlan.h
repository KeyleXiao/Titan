/*******************************************************************
** 文件名:	SchemeMatchPlan.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	撮合阶段 条件配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "Match_SchemeDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeMatchPlan : public ISchemeMatchPlan, public ISchemeUpdateSink, public CSchemeCache<CSchemeMatchPlan>
{
public:
	/////////////////////////ISchemeMatchPlan/////////////////////////
    virtual SSchemeMatchPlan*			getMatchPlanByWaitSecond(int nMatchType, int nMatchFriendOrOpposite, DWORD nWaitSeconds);

    virtual SSchemeMatchPlan*			getSchemeFristMatchPlan(int nMatchType, int nMatchFriendOrOpposite);

    virtual SSchemeMatchPlan*			getSchemeMatchPlan(int nMatchType, int nStageID);

    virtual SSchemeMatchPlanRatio*          getSchemeMatchPlanRatio(int nMatchType);


	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeMatchPlan//////////////////////////
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
	CSchemeMatchPlan(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeMatchPlan(void);
private:
    bool LoadMatchPlanScheme(ICSVReader* pCSVReader);
    bool LoadMatchPlanRatioScheme(ICSVReader* pCSVReader);

private:
	
    typedef map<int,SSchemeMatchPlan> MAP_StageID2MatchPlan;

    //MatchType
    typedef map<int, MAP_StageID2MatchPlan> MAP_ALLMatchTypeMatchPlan;

    MAP_ALLMatchTypeMatchPlan m_ALLMatchTypeMatchPlan;

};