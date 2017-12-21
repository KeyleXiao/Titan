/*******************************************************************
** 文件名:	SchemeHeroReliveTime.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/5/10
** 版  本:	1.0
** 描  述:	
            英雄复活时间
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
class CSchemeHeroReliveTime: public ISchemeHeroReliveTime, public ISchemeUpdateSink, public CSchemeCache<CSchemeHeroReliveTime>
{
	typedef std::map<int, std::map<int, int> > HERO_RELIVETIME_MAP;
	typedef HERO_RELIVETIME_MAP::iterator HERO_RELIVETIME_MAP_ITER;

    // 配置文件对应的列
    enum SCHEME_COLUMN
    {
		COLNUM_ConfigWarID,		// 比赛类型
        COLNUM_Level,			// 等级
        COLNUM_ReliveTime,		// 复活时间
    };

public:
	CSchemeHeroReliveTime(void);
	virtual ~CSchemeHeroReliveTime(void);

	// 载入脚本
	bool LoadScheme(void);

	// 关闭
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV配置载入时通知
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// 配置动态更新时通知
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeReliveTime///////////////////////
	// 获取复活时间
	virtual int getHeroReliveTime(int nMatchTypeID, int nLevel);

private:
	HERO_RELIVETIME_MAP     m_mapScheme;
};