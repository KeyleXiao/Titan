/*******************************************************************
** 文件名:	SchemeLevelUpgrade.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林飞
** 日  期:	1/9/2015
** 版  本:	1.0
** 描  述:	升级配置表 LevelUpgrade.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeCache.h"

// 定义
class CSchemeLevelUpgrade : public ISchemeLevelUpgrade, public ISchemeUpdateSink, public CSchemeCache<CSchemeLevelUpgrade>
{
public:
	/////////////////////////ISchemeLevelUpgrade/////////////////////////	
	virtual int				GetNextLevelExp(int nLevel, int nAttributton = ENEXPTYPE_WARHEROEXP, int nExpTypeID = 0);
	virtual int				GetLevelAllExp(int nLevel, int nAttributton = ENEXPTYPE_WARHEROEXP, int nExpTypeID = 0);
	virtual int				GetSchemeLevelUpgradeMaxLevel(int nAttributton = ENEXPTYPE_WARHEROEXP);
	virtual int				GetSchemeLevelUpgradeMinLevel(int nAttributton = ENEXPTYPE_WARHEROEXP);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeLevelUpgrade//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool											LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void											Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeLevelUpgrade(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeLevelUpgrade(void);

private:



    typedef std::vector<SLevelUpgradeScheme>        LEVEL_UPGRADE_SCHEMEVEC;        // key为等级
    typedef std::map<int, LEVEL_UPGRADE_SCHEMEVEC>  EXP_ARRAY_SCHEMEMAP;            // key为经验所属
    typedef std::map<int, int>                      LEVEL_MAX_SCHEMEMAP;            // key为经验所属
    typedef std::map<int, int>                      LEVEL_MIN_SCHEMEMAP;            // key为经验所属

    // 所有的等级对应经验
    EXP_ARRAY_SCHEMEMAP                             m_mapExpScheme;
    // 最大等级
    LEVEL_MAX_SCHEMEMAP                             m_mapLvMaxScheme;
    // 最少等级
    LEVEL_MIN_SCHEMEMAP                             m_mapLvMinScheme;
};