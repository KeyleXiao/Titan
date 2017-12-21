/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\SchemeCenter\Src\SchemeHeroTalentConfig.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	14/7/2016	
** 版  本:	1.0
** 描  述:	
            读策划符文配置文件
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "SchemeCache.h"

enum TalentActiveType
{
	TalentActiveType_DefualtActive = 0,
	TalentActiveType_SystemActive
};

class CSchemeHeroTalentConfig : public ISchemeHeroTalentConfig,
                        public ISchemeUpdateSink,
                        public CSchemeCache<CSchemeHeroTalentConfig>
{
    // 配置表中的列 							 					
    enum TC
    {
        TC_HERO_ID,         // 英雄ID
        TC_TALENT_ID,       // 天赋ID
        TC_LEARN_LEVEL,     // 天赋学习等级
        TC_ICON,            // 天赋图标
        TC_RECOMMAND_SHOW,  // 是否推荐显示
        TC_ACTIVE_TYPE,     // 激活类型（0默认自动;1系统）
        TC_ACTIVE_LEVEL,    // 激活等级
        TC_INNER_MUTEX,     // 场内是否等级互斥(0不互斥1互斥)
        TC_SHOW_TYPE,       // 显示类型（0未激活不显示1未激活显示）
        TC_EFFECT_SLOT,     // 影响技能槽位
        TC_NAME,            // 天赋名称
        TC_PRIZE_NAME,      // 天赋奖励名称
        TC_DESC,            // 天赋描述
        TC_UNACTIVE_DESC,   // 未激活描述
        TC_INNER_DESC,      // 场内天赋描述
        TC_PRE_TALENTID,    // 前置天赋
        TC_WAR_TYPE,        // 战场类型ID
        
    };
	typedef std::map<int, std::vector<Single_Hero_Talent_InfoEx> > T_HeroTalentConfigInfo;

public:
	
	///////////////////////////////IHeroTalentInfo///////////////////	
    // 获取指定玩家符文的配置数据
    virtual std::map<int, std::vector<Single_Hero_Talent_InfoEx> >*  GetHeroTalentInfo();

	///////////////////////////////ISchemeUpdateSink///////////////////
    /** CSV配置载入时通知
    @param   type   : 读取器类型
    : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
    : READER_XML : 就可以把pReader转换成TiXmlDocument*
    @param   pReader：读取CSV的返回接口
    @param   szFileName：配置文件名
    @warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
    @retval buffer
    */
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);

    /** 配置动态更新时通知
    @param   type   : 读取器类型
				    : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
				    : READER_XML : 就可以把pReader转换成TiXmlDocument*
    @param   pReader：读取CSV的返回接口
    @param   szFileName：配置文件名
    @warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
    @retval buffer
    */
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	///////////////////////////////CSchemePathFindServiceInfo//////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void Close(void);

	/** 
	@param   
	@param    
	@return  
	*/
	CSchemeHeroTalentConfig(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeHeroTalentConfig(void);
                           
	// 返回本英雄[0,nStarLv]星级的所有激活天赋列表
	// @param nStarLv: 升星等级
	virtual vector<int> getAllHeroStarActiveTalent(int nHeroID, int nStarLv);

	// 获取英雄天赋信息
	virtual Single_Hero_Talent_InfoEx* getHeroTalent(int nHeroID, int nTalentID);

private:
	T_HeroTalentConfigInfo m_SchemeInfo;
};
