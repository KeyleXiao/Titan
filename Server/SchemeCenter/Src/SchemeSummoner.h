/*******************************************************************
** 文件名:	SchemeSummoner.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016/8/3
** 版  本:	1.0
** 描  述:	

********************************************************************/
#pragma once
#include "SchemeDef.h"
#include "SchemeCache.h"
#include "ISchemeCenter.h"


using namespace std;
class CSchemeSummonerSkill: public ISchemeSummonerSkill, public ISchemeUpdateSink, public CSchemeCache<CSchemeSummonerSkill>
{
public:
    virtual bool            OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool            OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

public:
    /////////////////////////////ISchemeSummonerSkill/////////////////////////////////////////////

    // 根据技能id获得iconId
    virtual int getIconId( int nSkillId );

    //////////////////////////////////////////////////////////////////////////
    bool											LoadScheme(void);

    void											Close(void);

private:
    // 召唤师技能配置表
    std::map<int, SSummonerSkillData>        m_mapSummonerSkillInfo;
};