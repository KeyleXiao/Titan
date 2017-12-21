/*******************************************************************
** 文件名:	SchemeHeroCardGradeColor.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016-6-13
** 版  本:	1.0
** 描  述:	英雄卡牌等级颜色配置 HeroCardGradeColorConfig.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"


// 定义
class CSchemeHeroCardGradeColor : public ISchemeHeroCardGradeColor, public ISchemeUpdateSink
{
	enum EMHEROCARDGRADECOLOR_COLUMN
	{
		COL_ID = 0,                         // 序号                                        
		COL_GRADE,							// 卡牌等级
		COL_COLOR,							// 颜色
	};

public:
	//////////////////////////////// ISchemeHeroCardGradeColor ////////////////////////////////
	/** 取得指定卡牌的等级颜色配置信息
	@param nCardID: 卡牌等级 
	@return  
	*/
    virtual SHeroCardGradeColorScheme*				getByCardGrade( int nCardGrade );

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
	CSchemeHeroCardGradeColor(void);

private:
    typedef int CARD_GRADE;
	typedef std::map<CARD_GRADE, SHeroCardGradeColorScheme>	    TMAP_HEROCARDGRADECOLOR;

	TMAP_HEROCARDGRADECOLOR                                   m_mapHeroCardGradeColor;
};