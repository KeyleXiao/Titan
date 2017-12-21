/*******************************************************************
** 文件名:	SchemeTalent.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	3/28/2015
** 版  本:	1.0
** 描  述:	

			天赋配置管理
********************************************************************/
#pragma once
#include "SchemeTalentDef.h"
#include "Singleton.h"
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include <map>

using namespace rkt;

class CTalentScheme : public ISchemeTalent, public Singleton<CTalentScheme>, public ISchemeUpdateSink
{
public:
    // 加载天赋配置
    virtual bool Load();

    // 获取某个职 某个等级激活的天赋列表
    virtual const std::set<int>& GetVocationTalentListByLevel(int nVocation, int nLevel);

    // 获取某个职业的天赋列表
    virtual const std::set<int>& GetVocationTalentList(int nVocation);

    // 获取天赋
    virtual const STalent* GetTalent(int nTalentID); 

    // 获取天赋效果
    virtual const STalentEffectBase* GetTalentEffect(int nTalentEffectID); 

    // 获取某个天赋的天赋效果列表
    virtual void  GetEffectsOfTalent(int nTalentID, std::vector<const STalentEffectBase*>& refOut);

    virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    void Close();

private:
    bool LoadTalentScheme(ICSVReader* pCSVReader);
    bool LoadTalentEffectScheme(ICSVReader* pCSVReader);

    // 天赋效果ID -- 效果
    std::map<int, STalentEffectBase*> m_mapTalentEffect;

    // 天赋ID -- 天赋
    std::map<int, STalent> m_mapTalent;

    // 职业 --  [天赋map [激活等级--天赋列表]]
    std::map<int, std::map<int, std::set<int>>> m_mapVocationLevelTalents;

    // 职业 天赋列表
    std::map<int, std::set<int>> m_mapVocationTalents;

    // dummy
    std::set<int> m_setDummy;
};