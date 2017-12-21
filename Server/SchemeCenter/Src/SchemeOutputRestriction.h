/*******************************************************************
** �ļ���:	SchemeOutputRestriction.h
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
class CSchemeOutputRestriction : public ISchemeOutputRestriction, public ISchemeUpdateSink, public CSchemeCache<CSchemeOutputRestriction>
{
public:
    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
    virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////////////ISchemeOutputRestriction/////////////////////////////////////////
    virtual const SOutputRestrictionConfig* getOutputRestrictionConfig( int nMatchTypeID );
public:
    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
    bool                    LoadScheme();

    void                    Close();

private:
    void parseConditionInfo(char* pValue, std::map<int, int>& mapCondition);

private:
    // ս������Ϊ����
    std::map<int, SOutputRestrictionConfig>         m_mapOutputRestriction;

};