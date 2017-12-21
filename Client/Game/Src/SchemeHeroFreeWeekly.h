/*******************************************************************
** 文件名:	SchemeHeroFreeWeekly.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016/8/23
** 版  本:	1.0
** 描  述:	配置HeroFreeWeekly.csv

********************************************************************/
#pragma once

#include "SchemeDef.h"
#include "ISchemeCenter.h"
#include "ISchemeEngine.h"

class CSchemeHeroFreeWeekly : public ISchemeHeroFreeWeekly, public ISchemeUpdateSink
{
public:
	/////////////////////////////////ISchemeHeroFreeWeekly/////////////////////////////////////////
	// 取得周免英雄列表
	virtual int				GetFreeHeroListByLv( int arrayFreeHero[], int arrayLen, int nLv );
	// 是否为周免英雄
	virtual bool			CheckIsWeekFree( const int nHeroID, const int nLv );

	/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	virtual bool            OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool            OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////////////CSchemeHeroFreeWeekly/////////////////////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool					LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void					Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeHeroFreeWeekly(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeHeroFreeWeekly(void);

private:
	time_t MakeTime(char * ch);

private:
	// 周免英雄配置
	typedef std::vector<SHeroFreeWeeklyScheme> SHeroFreeWeeklyVec;
	SHeroFreeWeeklyVec vecHeroFree;     
	
	// 上次检测周免英雄列表
	static const int nFreeHeroArrayLen = 2 * HEROFREEWEEKLY_FREEID;
	int arrayFreeHeroID[nFreeHeroArrayLen];
};