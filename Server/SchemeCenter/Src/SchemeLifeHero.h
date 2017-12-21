/*******************************************************************
** 文件名:	SchemeLifeHero.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	王全申
** 日  期:	2017-9-13
** 版  本:	1.0
** 描  述:	本命英雄相关配置 LifeHero.csv、LifeHeroLight.csv、LifeHeroPrize.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeCache.h"


// 定义
class CSchemeLifeHero : public ISchemeLifeHero, public ISchemeUpdateSink, public CSchemeCache<CSchemeLifeHero>
{
public:
	//////////////////////////////// ISchemeLifeHero ////////////////////////////////
	// 取得本命英雄奖励配置
	virtual vector<SLifeHeroPrizeScheme>* getLifeHeroPrizeScehme()
	{
		return &m_vecPrize;
	}

	// 取得本命英雄配置
	virtual SLifeHeroScheme* getLifeHeroScehme(int i)
	{
		if (i < 0 || i >= ELifeHeroType::LIFEHERO_MAX_COUNT)
			return NULL;

		return &m_lifeHeroConfig[i];
	}

	//////////////////////////////// ISchemeUpdateSink ////////////////////////////////
	/** CSV配置载入时通知
	@param   type   : 读取器类型
		            : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
					: READER_XML : 就可以把pReader转换成TiXmlDocument*
	@param   pReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/ 
	virtual bool			                        OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	/** 配置动态更新时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			                        OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeLifeHero//////////////////////////
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
	CSchemeLifeHero(void);

private:
	bool LoadPrizeConfig(ICSVReader * pCSVReader);
	bool LoadLifeHeroConfig(ICSVReader * pCSVReader);

private:
	vector<SLifeHeroPrizeScheme> m_vecPrize;
	SLifeHeroScheme m_lifeHeroConfig[ELifeHeroType::LIFEHERO_MAX_COUNT];
};