/*******************************************************************
** 文件名:	SchemeTalentPageEffect.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	1/3/2016	
** 版  本:	1.0
** 描  述:	天赋页效果脚本配置
            
********************************************************************/

#pragma once

#ifdef FUNCTION_TALENTPAGE_OPEN
#include <map>
#include <vector>
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeDef.h"


class CSchemeTalentPageEffect :  public ISchemeTalentPageEffect,public ISchemeUpdateSink,public CSchemeCache<CSchemeTalentPageEffect>
{
public:
    /////////////////////////ISchemeTalentPageEffect/////////////////////////
    /** 取得天赋页效果配置信息
    @param nVocation:	天赋组ID
    @return 天赋页效果配置信息
    */
    virtual STalentPageEffect* getTalentEffectSchemeInfo(int nTalentArrayId);


	///////////////////////////////ISchemeUpdateSink///////////////////	
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeTalentPageEffect//////////////////////////
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
	CSchemeTalentPageEffect(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeTalentPageEffect(void);


private:
	
    typedef map<int, STalentPageEffect>		    TMAP_TALENTEFFECTSCHEME;
    TMAP_TALENTEFFECTSCHEME						m_mapTalentEffectScheme;

};

#endif