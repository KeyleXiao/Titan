/*******************************************************************
** �ļ���:	SchemeExpOrGoldCreate.h
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
    // ������ϵ��
    std::map<int, std::map<int, SExpOrGoldCreateConfig>> m_mapExpOrGoldConfig;
};
