/*******************************************************************
** 文件名:	SchemeTalentPage.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	1/3/2016	
** 版  本:	1.0
** 描  述:	天赋页脚本配置
            
********************************************************************/

#pragma once

#ifdef FUNCTION_TALENTPAGE_OPEN

#include <map>
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeDef.h"

#define TalentDefaultMaxLevel 6
#define TalentMaxCanUseCount 30
typedef map<int, STalentPageData>		TMAP_TALENTSCHEME;
class CSchemeTalentPage :  public ISchemeTalentPage,public ISchemeUpdateSink,public CSchemeCache<CSchemeTalentPage>
{
public:
    /////////////////////////ISchemeTalentPage/////////////////////////
	/** 取得天赋页属性配置信息
	@param nVocation:	天赋ID
	@return 天赋页属性配置信息
	*/
    virtual STalentPageData* getTalentPageSchemeInfo(int nTalentId);
    
	///////////////////////////////ISchemeUpdateSink///////////////////	
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeTalentPage//////////////////////////
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
    
 	/** 返回缓存
	@param   
	@param   
	@return TMAP_TALENTSCHEME
	*/
    virtual TMAP_TALENTSCHEME*                       getTalentPageSchemeInfoMapAll();
     /** 
	@param   
	@param   
	@return  
	*/
	CSchemeTalentPage(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeTalentPage(void);


private:
	
   
    TMAP_TALENTSCHEME						m_mapTalentScheme;

};
#endif