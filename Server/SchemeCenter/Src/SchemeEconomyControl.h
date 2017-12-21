/*******************************************************************
** 文件名:	CSchemeEconomyControl.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/2/23
** 版  本:	1.0
** 描  述:	
            经济控制脚本
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeDef.h"
#include <map>

using namespace rkt;

// 系统提示配置
class CSchemeEconomyControl: public ISchemeEconomyControl, public ISchemeUpdateSink, public CSchemeCache<CSchemeEconomyControl>
{
	typedef std::map< int,map<int, SEconomyControlConfig_S> > ECO_CTRL_MAP;
	typedef std::map< int,map<int, SEconomyControlConfig_S> >::iterator ECO_CTRL_MAP_ITER;

    // 配置文件对应的列
    enum SCHEME_COLUMN
    {
        COLNUM_CONFIG_WAR_ID = 0,// 战场ID（用于配置文件）
        COLNUM_ID,				// 经济差类型ID
		COLNUM_A_RATIO,			// a系数
        COLNUM_B_RATIO,			// b系数
        COLNUM_SOLDIER,			// 士兵加成
        COLNUM_MONSTER,			// 野怪加成
        COLNUM_SALARY,			// 工资加成
        COLNUM_HERO,			// 英雄加成
    };

public:
	CSchemeEconomyControl(void);
	virtual ~CSchemeEconomyControl(void);

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


	//////////////////////////////////////////ISchemeEconomyControl///////////////////////
	virtual SEconomyControlConfig_S* getEconomyCtrl(int nMapID, int nLineID);
public:

private:
    ECO_CTRL_MAP     m_mapScheme;
};