/*******************************************************************
** 文件名:	SchemeWarDevilKingNPC.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/11/30
** 版  本:	1.0
** 描  述:	
            战场魔王怪物NPC配置
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
class CSchemeWarDevilKingNPC: public ISchemeWarDevilKingNPC, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarDevilKingNPC>
{

public:
	CSchemeWarDevilKingNPC(void);
	virtual ~CSchemeWarDevilKingNPC(void);

	// 载入脚本
	bool LoadScheme(void);

	// 关闭
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV配置载入时通知
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// 配置动态更新时通知
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarDevilKing///////////////////////
	virtual SWarDevilKingNPC* getWarDevilKingNPC(int nHeroID) override;

public:

private:
	// <英雄ID,配置>
	std::map<int,SWarDevilKingNPC> m_mapScheme;
};