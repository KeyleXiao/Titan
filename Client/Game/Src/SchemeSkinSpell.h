/*******************************************************************
** 文件名:	SchemeSkinSpell.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/14/2017
** 版  本:	1.0
** 描  述:	皮肤技能
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeSkinSpell : public ISchemeSkinSpell, public ISchemeUpdateSink
{
public:
	/////////////////////////ISchemeSkinSpell/////////////////////////
	/** 取得PK配置信息
	@return PK配置信息
	*/
	virtual SSkinSpell* getSkinSpell(int nSkinID, int nType, int nValue);

    /** 取得皮肤技能配置信息
    @return 皮肤技能配置
    */
    virtual int getSkinSpell(int nSkinID, int nType, SSkinSpell* arraySkinSpell, int nArrayCount);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeSkinSpell//////////////////////////
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
	CSchemeSkinSpell(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeSkinSpell(void);

private:
    typedef int  SKIN_ID;
    typedef int  SKIN_TYPE_ID;
    // 单个皮肤对应的所有配置数据
    typedef vector<SSkinSpell>                  SKIN_VECTOR;
    // 单个皮肤对应各自配置的数据列表
    typedef map<SKIN_ID, SKIN_VECTOR>           SKIN_MAP;
    // 所有皮肤对应各自配置的数据列表
    typedef map<SKIN_TYPE_ID, SKIN_MAP>        SKIN_SPELL_MAP;

    SKIN_SPELL_MAP					           m_schemeMap;
};