/*******************************************************************
** 文件名: E:\work\w1\speed\Server\SchemeCenter\Src\SchemeCrownPage.h
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 陈言蒙
** 日  期: 24/11/2016
** 版  本: 1.0
** 描  述:
            读策划皇冠宝石配置文件
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "SchemeCache.h"

class CSchemeCrownGemstoneInfo : public ISchemeCrownGemstoneInfo,
                        public ISchemeUpdateSink,
                        public CSchemeCache<CSchemeCrownGemstoneInfo>
{
    typedef std::map<int, SCrownGemstoneConfig_S> MapCrownInfo;
    typedef std::map<int, SCrownGemstoneEffectConfig_S> MapCrownEffect;
    typedef std::map<int, SCrownCobstoneRandomConfig_S> MapCobstoneRandom;
    typedef std::map<int, SCrownGemstoneRandomConfig_S*> MapGemstoneRandom;
    typedef std::map<int, SCrownGemstoneExteriorConfig_S> MapGemstoneExterior;
public:
    
    // ISchemeUpdateSink
    virtual bool OnSchemeLoad(SCRIPT_READER reader, const char *szFileName);
    virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName);

    // ISchemeCrownGemstoneInfo
    // 获得皇冠宝石配置信息
    virtual SCrownGemstoneConfig_S* getCrownGemstoneConfig(int nGemstoneID);
    // 获得皇冠宝石效果配置信息
    virtual SCrownGemstoneEffectConfig_S* getCrownGemstoneEffectConfig(int nPeopertyID);
    // 获得皇冠宝石随机配置信息
    virtual SCrownGemstoneRandomConfig_S* getCrownGemstoneRandomConfig(int nGemstoneGroupID);
    // 获得皇冠原石随机配置信息
    virtual SCrownCobstoneRandomConfig_S* getCrownCobstoneRandomConfig(int nCobstoneGroupID);
    // 获得皇冠外观配置信息
    virtual SCrownGemstoneExteriorConfig_S* getCrownCobstoneExteriorConfig(int nExteriorID);
public:
    bool LoadScheme();
    void Close();

private:
    bool LoadCrownGemstoneInfoScheme( ICSVReader * pCSVReader, LPCSTR szFileName );
    bool LoadCrownGemstoneEffectScheme( ICSVReader * pCSVReader, LPCSTR szFileName );
    bool LoadCrownGemstoneRandomScheme( ICSVReader * pCSVReader, LPCSTR szFileName );
    bool LoadCrownCobstoneRandomScheme( ICSVReader * pCSVReader, LPCSTR szFileName );
    bool LoadCrownCobstoneExteriorScheme( ICSVReader * pCSVReader, LPCSTR szFileName );

    void ClearGemstoneRandomNodeMap();
private:
    MapCrownInfo m_SchemeCrownGemstoneInfo;
    MapCrownEffect m_SchemeCrownGemstoneEffect;
    MapCobstoneRandom m_SchemeCobstoneRandom;
    MapGemstoneRandom m_SchemeGemstoneRandom;
    MapGemstoneExterior m_SchemeGemstoneExterior;
};
