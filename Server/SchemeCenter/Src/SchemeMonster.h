/*******************************************************************
** 文件名:	SchemeMonster.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	1/9/2015
** 版  本:	1.0
** 描  述:	怪物配置表 Monster.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeCache.h"

// 定义
class CSchemeMonster : public ISchemeMonster, public ISchemeUpdateSink, public CSchemeCache<CSchemeMonster>
{
    enum EMMONSTER_SCHEME_COLUMN
    {
        MONSTER_COL_ID = 0,							// 怪物ID
        MONSTER_COL_TYPE,							// 怪物类型
        MONSTER_COL_SUBTYPE,                        // 怪物子类型
        MONSTER_COL_NAME,							// 怪物名
        MONSTER_COL_ICON,							// 怪物图标ID
        MONSTER_COL_DEFAULT_SKIN,					// 默认皮肤ID
        MONSTER_COL_HP,								// 生命值
        MONSTER_COL_ADD_HP,							// 生命增长值
        MONSTER_COL_MP,								// 法力值
        MONSTER_COL_ADD_MP,							// 法力增长值
        MONSTER_COL_PA,								// 物理攻击力
        MONSTER_COL_ADD_PA,							// 物理攻击增长值
        MONSTER_COL_MA,								// 法术攻击力
        MONSTER_COL_ADD_MA,							// 法术攻击增长值
        MONSTER_COL_PD,								// 物理防御力
        MONSTER_COL_ADD_PD,							// 物理防御增长值
        MONSTER_COL_MD,								// 法术防御力
        MONSTER_COL_ADD_MD,							// 法术防御增长值
        MONSTER_COL_ASD,							// 攻击速度
        MONSTER_COL_ADD_ASD,						// 攻击速度增长值
        MONSTER_COL_CRC,							// 暴击率
        MONSTER_COL_ADD_CRC,						// 暴击增长值
        MONSTER_COL_SPEED,							// 移动速度
        MONSTER_COL_EYESHOT,						// 视野距离
        MONSTER_COL_SEARCH_RADII,                   // 搜索范围 
        MONSTER_COL_CAMP,							// 阵营	
        MONSTER_COL_SKILL1,							// 怪物技能1
        MONSTER_COL_SKILL2   = MONSTER_COL_SKILL1+3,// 怪物技能2
        MONSTER_COL_SKILL3   = MONSTER_COL_SKILL2+3,// 怪物技能3
        MONSTER_COL_SKILL4	 = MONSTER_COL_SKILL3+3,// 怪物技能4
        MONSTER_COL_STRATEGY = MONSTER_COL_SKILL4+3,// 搜索策略
        MONSTER_COL_SEACH_ENMITY,					// 搜索仇恨
        MONSTER_COL_STRATEGY_VAL,					// 搜索频率（ms）
        MONSTER_COL_EXPVAL,							// 经验值
        MONSTER_COL_EXPRANGE,						// 经验范围
        MONSTER_COL_EXPTYPE,						// 经验类型
        MONSTER_COL_RETURN_DIS,						// 回归距离
        MONSTER_COL_RETURN_MASK,					// 回归标志
        MONSTER_COL_MASK,							// 标志	ENTITY_MASKFLAG
        MONSTER_COL_BOSSBLOODSHOWDISTASNCE,			// boss血条的显示距离
        MONSTER_COL_BOSSBLOODSHOWPRIORITY,			// boss血条的显示优先级
        MONSTER_COL_MINIMAPICONID,                  // 小地图图标ID
        MONSTER_COL_COLOR,                          // 怪物可秒杀时血条颜色
        MONSTER_COL_RESERVE,                        // 保留数值; 注:动态怪.障碍配置(DynamicObstacle.csv)索引号
    };
public:
	/////////////////////////ISchemeMonster/////////////////////////
	/** 取得怪物配置信息
	@param nMonsterId:	怪物ID
	@return  
	*/
	virtual SMonsterScheme* getMonsterShemeInfo(int nMonsterId);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeMonster//////////////////////////
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
	CSchemeMonster(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeMonster(void);

private:
	// 怪物ID为索引
	typedef std::map<int, SMonsterScheme> TMAP_MONSTERSCHEME;
	TMAP_MONSTERSCHEME				m_mapMonsterScheme;
};