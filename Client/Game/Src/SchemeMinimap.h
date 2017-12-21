/*******************************************************************
** 文件名:	SchemeMinimap.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李建伟
** 日  期:	4/22/2015
** 版  本:	1.0
** 描  述:	

			小地图配置管理
********************************************************************/
#pragma once
#include "Singleton.h"
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "Minimap_ManageDef.h"
#include <map>

using namespace rkt;

class CMinimapScheme : public IMinimapScheme, public Singleton<CMinimapScheme>, public ISchemeUpdateSink
{
public:
    CMinimapScheme()
    {
        m_mapMinimap.clear();
        m_mapMinimapIcon.clear();
        m_mapStaticIcon.clear();
    }

    virtual bool Load();

    virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    // 获取小地图信息
    virtual void* GetMinimapInfo(int nMapID);

    // 获取小地图图标信息
    virtual void* GetMinimapIconInfo(int nIconID);

    // 根据地图ID、静态打点序号 获得小地图静态打点信息
    virtual void* GetMinimapStaticIcon(int nMapID, int nIndex);

    // 根据序号，获取下一个静态打点信息
    virtual void* GetNextMinimapStaticIcon(int nMapID, int nIndex); 

private:
    bool LoadMinimapInfo(ICSVReader* pCsvReader);
    bool LoadMinimapIcon(ICSVReader* pCsvReader);
    bool LoadMinimapStaticIcon(ICSVReader* pCsvReader);

    // 小地图信息表
    std::map<int, SMinimapInfo> m_mapMinimap;    

    // 小地图图标信息表
    std::map<int, SMinimapIcon> m_mapMinimapIcon;

    // 小地图静态打点信息表<地图ID--<序号--静态打点>>
    std::map<int, std::map<int, SMinimapStaticIcon>> m_mapStaticIcon;
};