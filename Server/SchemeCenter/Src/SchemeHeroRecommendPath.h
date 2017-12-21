/*******************************************************************
** 文件名:	SchemeHeroRecommendPath.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	3/22/2017
** 版  本:	1.0
** 描  述:	

			英雄推荐线路(用于支持AI初始选线)
********************************************************************/
#pragma once
#include "Singleton.h"
#include "ISchemeEngine.h"
#include "SchemeCache.h"
#include "ISchemeCenter.h"
#include <map>

using namespace rkt;

class CHeroRecommendPathScheme : public ISchemeHeroRecommendPath, public ISchemeUpdateSink, public CSchemeCache<CHeroRecommendPathScheme>
{
public:
    virtual bool Load();

    // 获取英雄的推荐线路
    virtual int getRecommendPath(int nHeroID, int nBattleField);

    // 获得某个战场的pathID列表
    virtual bool getPathList(int nBattleField, PathRecommendHeroNum* pResultBuf, int& nRefCount);

    virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    void Close();

private:
    // <战场ID, <英雄ID, 推荐线路>>
    std::map<int, std::map<int, int>> m_mapRecommendPath;

    // <战场ID, <线路ID, 推荐人数>>
    std::map<int, std::map<int, int>> m_mapBattleFieldPath;
};