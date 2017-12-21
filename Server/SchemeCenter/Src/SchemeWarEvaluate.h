/*******************************************************************
** 文件名:	SchemeWarEvaluate.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2015/12/4
** 版  本:	1.0
** 描  述:	战场最后评分配置表
********************************************************************/
#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeWarDef.h"

class CSchemeWarEvaluate:public ISchemeWarEvaluate, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarEvaluate>
{
public:
    CSchemeWarEvaluate();
    ~CSchemeWarEvaluate();

public:
    ///////////////////////////ISchemeUpdateSink///////////////////////////////////////////////
    virtual bool            OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool            OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    ///////////////////////////CSchemeWarEvaluate///////////////////////////////////////////////
    //载入脚本
	bool LoadScheme(void);

	//关闭
	void Close(void);

    ////////////////////////////ISchemeWarEvaluate//////////////////////////////////////////////
    virtual SWarEvaluathInfo* getEvaluathInfo(int nMatchTypeID = 1);

private:
    std::map<int, SWarEvaluathInfo> m_warEvaluateMap;
};
