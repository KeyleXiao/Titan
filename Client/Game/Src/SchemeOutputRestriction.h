/*******************************************************************
** 文件名:	SchemeOutputRestriction.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	18/7/2016
** 版  本:	1.0
** 描  述:	OutputRestriction.csv

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
    // 战场类型为索引
    std::map<int, SOutputRestrictionConfig>         m_mapOutputRestriction;

};