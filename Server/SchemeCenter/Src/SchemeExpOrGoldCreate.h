/*******************************************************************
** 文件名:	SchemeExpOrGoldCreate.h
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
class CSchemeExpOrGoldCreate : public ISchemeExpOrGoldCreate, public ISchemeUpdateSink, public CSchemeCache<CSchemeExpOrGoldCreate>
{
public:
    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
    virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////////////ISchemeExpOrGoldCreate/////////////////////////////////////////
    virtual SExpOrGoldCreateConfig* getExpOrGoldCreateConfig( int nWarTypeID , int ExpType);
public:
    /////////////////////////////////CSchemeExpOrGoldCreate/////////////////////////////////////////
    bool                    LoadScheme();

    void                    Close();
private:
    // 经验金币系数
    std::map<int, std::map<int, SExpOrGoldCreateConfig>> m_mapExpOrGoldConfig;
};
