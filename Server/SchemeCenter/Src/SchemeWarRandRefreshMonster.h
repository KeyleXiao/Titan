/*******************************************************************
** 文件名:	SchemeWarRandRefreshMonster.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/11/25
** 版  本:	1.0
** 描  述:	
            战场随机刷怪脚本
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeDef.h"
#include <map>

using namespace rkt;

// 系统提示配置
class CSchemeWarRandRefreshMons: public ISchemeWarRandRefreshMons, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarRandRefreshMons>
{
	// <configWarID,怪物随机刷新列表>
	/*typedef std::map<int, vector<SWarRandRefreshMons>> RANDREFRESHMONSLIST_MAP;
	typedef RANDREFRESHMONSLIST_MAP::iterator RANDREFRESHMONSLIST_MAP_ITER;*/

	// <怪物组类型ID,怪物随机刷新列表>
	typedef std::map<int, SWarRandRefreshMons> RANDREFRESHMONS_MAP;
	typedef RANDREFRESHMONS_MAP::iterator RANDREFRESHMONS_MAP_ITER;

public:
	CSchemeWarRandRefreshMons(void);
	virtual ~CSchemeWarRandRefreshMons(void);

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


	//////////////////////////////////////////ISchemeWarRandRefreshMons///////////////////////
	// 获取随机刷怪信息
	virtual vector<SWarRandRefreshMons> getRandRefreshMonsList(int nConfigWarID);

	// 获取随机刷怪信息
	virtual SWarRandRefreshMons* getRandRefreshMons(int nMonsterIndex);
public:

private:
	//RANDREFRESHMONSLIST_MAP m_mapListScheme;

	RANDREFRESHMONS_MAP m_mapScheme;
};