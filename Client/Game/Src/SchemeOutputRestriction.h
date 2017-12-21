/*******************************************************************
** �ļ���:	SchemeOutputRestriction.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	18/7/2016
** ��  ��:	1.0
** ��  ��:	OutputRestriction.csv

********************************************************************/
#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"

using namespace rkt;
class CSchemeOutputRestriction : public ISchemeOutputRestriction, public ISchemeUpdateSink
{
public:
    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
    virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////////////ISchemeOutputRestriction/////////////////////////////////////////
    virtual const SOutputRestrictionConfig* getOutputRestrictionConfig( int nMatchTypeID );

    virtual int getCampPersonNumRestriction(int nMatchTypeID, int nCamp);

    virtual int getMatchAllPersonRestriction(int nMatchTypeID);

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