/*******************************************************************
** 文件名:	CSchemeWarSubFuncSetting.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/4/13
** 版  本:	1.0
** 描  述:	
            战场子功能设置脚本
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeDef.h"
#include "SchemeWarDef.h"
#include <map>

using namespace rkt;

// 系统提示配置
class CSchemeWarSubFunc: public ISchemeWarSubFunc, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarSubFunc>
{
	typedef std::map< int, SWarSubFunc> WAR_SUBFUNC_MAP;
	typedef WAR_SUBFUNC_MAP::iterator WAR_SUBFUNC_MAP_ITER;

    // 配置文件对应的列
    enum SCHEME_COLUMN
    {
        COLNUM_MatchType,			// 比赛类型
		COLNUM_XPSkill_OnOff,		// xp技能开关
		COLNUM_Support_OnOff,		// 点赞开关
		COLNUM_SaveOftenEuip_OnOff,	// 保存常用装备
		COLNUM_AIEcmCtrl_OnOff,		// AI经济控制开关
		COLNUM_AIExpCtrl_OnOff,		// AI经验控制开关
    };

public:
	CSchemeWarSubFunc(void);
	virtual ~CSchemeWarSubFunc(void);

	// 载入脚本
	bool LoadScheme(void);

	// 关闭
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV配置载入时通知
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// 配置动态更新时通知
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarSubFuncSetting///////////////////////
	// 是否开启了某项功能
	virtual bool isEnableSubFunc(int nMatchType, int nSubFuncType);

public:

private:
	WAR_SUBFUNC_MAP     m_mapScheme;
};