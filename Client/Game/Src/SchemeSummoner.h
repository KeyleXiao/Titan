/*******************************************************************
** 文件名:	SchemeSummoner.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	召唤师技能配置表
********************************************************************/
#pragma once
#include "Singleton.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"

using namespace rkt;
class CSchemeSummonerSkill : public ISchemeSummonerSkill, public ISchemeUpdateSink
{
public:
    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////////////ISchemeSummonerSkill/////////////////////////////////////////
    virtual std::map<int, SSummonerSkillInfo>* getAllSummonerSkillInfo();

	virtual std::map<int, SSummonerSkillInfo>* getSummonerSkillByWarMode(int nWarMode);

    virtual int getIconId( int nSkillId );

    virtual SSummonerSkillInfo* getSummonerSkillInfo(int nSkillId );

public:
    /////////////////////////////////CSchemeSummonerSkill/////////////////////////////////////////
    bool                    LoadScheme();

    void                    Close();

private:
    // 技能id为索引
    std::map<int, SSummonerSkillInfo>		m_mapSummonerSkillInfo;

	// 战场模式为索引
	typedef std::map<int, std::map<int, SSummonerSkillInfo> >		TMap_WarMode2Summon;
	TMap_WarMode2Summon						m_mapWarType2Summon;
};