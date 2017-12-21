/*******************************************************************
** 文件名:	SchemeHeroCardCreate.h
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
class CSchemeHeroCardCreat : public ISchemeHeroCardCreat, public ISchemeUpdateSink, public CSchemeCache<CSchemeHeroCardCreat>
{
public:
    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
    virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////////////ISchemeHeroCardCreat/////////////////////////////////////////
    virtual SHeroCardCreatPro* getHeroCardCreatPro(int nPlayerLV);

    virtual SHeroCardCreatCount* getHeroCardCreateCount(int nPlayerLV, int nWarType);

public:
    /////////////////////////////////CSchemeHeroCardCreat/////////////////////////////////////////
    bool                    LoadScheme();

    void                    Close();

protected:
    bool                    loadHeroCardCreatProScheme(SCRIPT_READER reader);
    bool                    loadHeroCardCreatCountScheme(SCRIPT_READER reader);

private:
    // 玩家等级为索引
    std::map<int, SHeroCardCreatPro>        m_mapHeroCardCreatePro;

    // 玩家等级为主索引  战场类型为次索引
    typedef std::map<int, std::map<int, SHeroCardCreatCount>> MAPHEROCARDCREATCOUNT;
    MAPHEROCARDCREATCOUNT m_mapHeroCardCreateCount;

};