/*******************************************************************
** 文件名:	SchemePlayerGloryInfo.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	称号配置表
********************************************************************/
#pragma once
#include "Singleton.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"

class CSchemePlayerGloryInfo:public ISchemePlayerGloryInfo,public ISchemeUpdateSink
{
public:
    /////////////////////////////////ISchemePlayerGloryInfo/////////////////////////////////////////

    virtual std::map<int, SGlorySchemeInfo>* getGlorySchemeInfo();

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
    virtual bool            OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool            OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);
    /////////////////////////////////CSchemePlayerGloryInfo/////////////////////////////////////////
    bool											LoadScheme(void);
    void											Close(void);

protected:
    // 解析获得称号的条件
    void parseConditionInfo(char* pValue, std::map<int, long>& item);

private:
    // 称号配置表信息
    std::map<int, SGlorySchemeInfo>      m_mapGloryInfo;
};