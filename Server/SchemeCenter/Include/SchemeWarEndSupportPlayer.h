/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\SchemeCenter\Src\SchemeCrownPage.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	12/12/2016	
** 版  本:	1.0
** 描  述:	
            读点赞系统配置文件
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "SchemeCache.h"

class CSchemeWarEndSupportPlayer : public ISchemeWarEndSupportPlayer,
                        public ISchemeUpdateSink,
                        public CSchemeCache<CSchemeWarEndSupportPlayer>
{
	typedef std::map<int, SWarEndSupportVoteConfig_S> MapSupportVoteInfo;

public:
	
	///////////////////////////////ISchemeUpdateSink///////////////////////////////
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char *szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName);

	///////////////////////////////ISchemeWarEndSupportPlayer///////////////////////////////
	// 获得点赞系统所有评选信息
	virtual bool getEndSupportPlayerVoteInfoList(SWarEndSupportVoteConfig_S supportVoteList[]);

	// 是否含有nVoteID对应的标签ID
	virtual bool isHasLabelID(int nVoteID, int nLabelID);

	// 随机一个标签
	virtual int getRandomLabel(int nVoteID);
public:
	bool LoadScheme();
	void Close();

private:
	bool LoadWarEndSupportPlayerScheme( ICSVReader * pCSVReader, const char *szFileName );

private:
	MapSupportVoteInfo m_SchemeSupportVoteInfo;
};
