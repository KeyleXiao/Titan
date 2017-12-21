/*******************************************************************
** 文件名:	CSchemeWarSafetyZone.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/11/1
** 版  本:	1.0
** 描  述:	
			大逃杀-安全区/毒气区配置
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"
#include "SchemeWarDef.h"
#include "SchemeCache.h"
#include <map>

using namespace rkt;

// 系统提示配置
class CSchemeWarSafetyZone: public ISchemeWarSafetyZone, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarSafetyZone>
{
	typedef std::map< int, SWarSafetyZone> WAR_SAFETYZONE_MAP;
	typedef WAR_SAFETYZONE_MAP::iterator WAR_SAFETYZONE_MAP_ITER;

public:
	CSchemeWarSafetyZone(void);
	virtual ~CSchemeWarSafetyZone(void);

	// 载入脚本
	bool LoadScheme(void);

	// 关闭
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV配置载入时通知
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// 配置动态更新时通知
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarSafetyZone///////////////////////
	// 获取安全区信息
	virtual SWarSafetyZone* getWarSafetyZone(int nRound);

public:

private:
	WAR_SAFETYZONE_MAP     m_mapScheme;
};