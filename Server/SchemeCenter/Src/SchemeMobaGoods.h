/*******************************************************************
** 文件名:	SchemeMobaGoods.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	1/9/2015
** 版  本:	1.0
** 描  述:	怪物配置表 SchemeMobaGoods.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "MobaGoodsSchemeDef.h"
#include "SchemeCache.h"

// 定义
class CSchemeMobaGoods : public ISchemeMobaGoods, public ISchemeUpdateSink, public CSchemeCache<CSchemeMobaGoods>
{
public:
	/////////////////////////ISchemeMobaGoods/////////////////////////
	/** 取得怪物配置信息
	@param nMonsterId:	怪物ID
	@return  
	*/
	virtual SSchemeMobaGoods* getMobaGoodsShemeInfo(int nGoodId);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeMobaGoods//////////////////////////
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
	CSchemeMobaGoods(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeMobaGoods(void);

	virtual SSchemeMobaGoods* getSchemeMobaGoodsInfo(int nGoodID);

	virtual SSchemeGoodEffectGroupType* getSchemeMobaGoodsPropInfo(int nEffectID);

	virtual void getSchemeNeedComposeList(int nGoodID, int *NeedComposeList, int &nCount);

	virtual int getGoodsSkillFreeze(int nGoodsSkillID);

    /** 取得推荐物品信息
    @param 物品ID
    @return  
    */
    virtual SSchemeRecommendGood* getRecommandGoods(int nHeroID, int nMatchType);
private:
	bool	LoadMobaGoodsScheme(SCRIPT_READER reader);
	bool	LoadRecommendGoodScheme(SCRIPT_READER reader);
	bool	LoadGoodEffectGroupTypeScheme(SCRIPT_READER reader);


private:
	// 物品ID为索引
	typedef std::map<int, SSchemeMobaGoods> TMAP_GOODSCHEME;
	TMAP_GOODSCHEME				m_mapGoodsScheme;

	typedef std::map<int, list<SSchemeRecommendGood>> TMAP_RECOMMENDGOODSCHEME;
	TMAP_RECOMMENDGOODSCHEME	m_mapRecommendGoodScheme;

	typedef std::map<int, SSchemeGoodEffectGroupType> TMAP_EFFECTGROUPGOODSCHEME;
	TMAP_EFFECTGROUPGOODSCHEME	m_mapGoodEffectGroupTypeScheme;

	// 物品技能ID 对应 FreezeID
	typedef std::map<int, int> TMAP_GOODSSKILLFREEZE;
	TMAP_GOODSSKILLFREEZE		m_mapGoodsSkillFreeze;
};