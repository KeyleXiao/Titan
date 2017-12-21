/*******************************************************************
** 文件名:	CSchemeWarForceEnd.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/3/28
** 版  本:	1.0
** 描  述:	
            战场强制脚本
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeDef.h"
#include <map>

using namespace rkt;

// 系统提示配置
class CSchemeWarForceEnd: public ISchemeWarForceEnd, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarForceEnd>
{
	typedef std::map< int, SWarForceEndInfo > WAR_FORCEEND_MAP;
	typedef WAR_FORCEEND_MAP::iterator WAR_FORCEEND_MAP_ITER;

    // 配置文件对应的列
    enum SCHEME_COLUMN
    {
        COLNUM_MatchType,		// 比赛类型
        COLNUM_IsEnable,		// 是否开启
        COLNUM_Time,			// 触发事件（秒）
        COLNUM_KillDiffWeight,	// 人头差权重
        COLNUM_EcmDiffWeight,	// 经济差权重
        COLNUM_TowerDiffWeight, // 推塔差权重
    };

public:
	CSchemeWarForceEnd(void);
	virtual ~CSchemeWarForceEnd(void);

    /////////////////////////////////CSchemeChatCD/////////////////////////////////////////
	// 载入脚本
	bool LoadScheme(void);

	// 关闭
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV配置载入时通知
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// 配置动态更新时通知
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarForceEnd///////////////////////
	SWarForceEndInfo* getWarForceEndInfo(int nMatchType);

	// 是否开启了强制结算
	virtual bool isEnableForceEnd(int nMatchType);
public:

private:
	WAR_FORCEEND_MAP     m_mapScheme;
};