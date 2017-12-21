/*******************************************************************
** 文件名:	SchemeFightingCapacityConfig.h
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

using namespace rkt;
class CSchemeFightingCapacityConfig : public ISchemeFightingCapacityConfig, public ISchemeUpdateSink, public CSchemeCache<CSchemeFightingCapacityConfig>
{
public:
    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
    virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////////////ISchemeHeroType/////////////////////////////////////////
    virtual const SFightingCapacityConfig* getFightConfig( int nWarTypeID );
public:
    /////////////////////////////////ISchemeFightingCapacityConfig/////////////////////////////////////////
    bool                    LoadScheme();

    void                    Close();

private:
    // 比赛类型为索引
    std::map<int, SFightingCapacityConfig>        m_mapFightingCapacity;

};