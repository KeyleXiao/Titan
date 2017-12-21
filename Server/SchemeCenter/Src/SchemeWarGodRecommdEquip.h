/*******************************************************************
** 文件名:	SchemeWarGodRecommdEquip.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/4/17
** 版  本:	1.0
** 描  述:	
            大神推荐装脚本
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeWarDef.h"
#include <map>

using namespace rkt;

// 系统提示配置
class CSchemeWarGodRecommdEquip: public ISchemeWarGodRecommdEquip, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarGodRecommdEquip>
{
	typedef std::vector< SWarGodRecommdEquip > WAR_GODRECOMMDEQUIP_VECTOR;
	typedef WAR_GODRECOMMDEQUIP_VECTOR::iterator WAR_GODRECOMMDEQUIP_VECTOR_ITER;

    // 配置文件对应的列
    enum SCHEME_COLUMN
    {
        COLNUM_LeagueName,		// 联赛名称
        COLNUM_LeagueType,		// 联赛类型
        COLNUM_Title,			// 称号
        COLNUM_HeroID,			// 英雄ID
        COLNUM_SchemeName,		// 方案名字
        COLNUM_AuthorName,		// 作者名字
        COLNUM_EquipIDList,		// 方案ID
    };

public:
	CSchemeWarGodRecommdEquip(void);
	virtual ~CSchemeWarGodRecommdEquip(void);

    /////////////////////////////////CSchemeWarGodRecommdEquip/////////////////////////////////////////
	// 载入脚本
	bool LoadScheme(void);

	// 关闭
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV配置载入时通知
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// 配置动态更新时通知
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarGodRecommdEquip///////////////////////
	// 获取英雄榜第一名的推荐装备
	virtual vector<SWarGodRecommdEquip>* getGodRecommdEquip();

public:

private:
	WAR_GODRECOMMDEQUIP_VECTOR     m_listScheme;
};