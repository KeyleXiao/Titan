/*******************************************************************
** �ļ���:	SchemeFightingCapacityConfig.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2016/8/3
** ��  ��:	1.0
** ��  ��:	

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
    // ��������Ϊ����
    std::map<int, SFightingCapacityConfig>        m_mapFightingCapacity;

};