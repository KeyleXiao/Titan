/*******************************************************************
** 文件名:	SchemeSpellSlot.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/25/2015
** 版  本:	1.0
** 描  述:	技能槽位配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "EntityDef.h"
#include "SpellDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeSpellSlot : public ISchemeSpellSlot, public ISchemeUpdateSink, public CSchemeCache<CSchemeSpellSlot>
{
public:
	/** 取得技能槽配置信息
	@param nVocation:	职业
	@return 技能槽配置信息
	*/
	virtual SPELL_SLOT *getSpellSlot(int nVocation);

	/** 取得技能槽位索引
	@param nVocation:	职业
	@param nSpellID:	技能ID
	@return 技能槽位索引
	*/
	virtual int getSpellSlotIndex(int nVocation, int nSpellID);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeSpellSlot//////////////////////////
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
	CSchemeSpellSlot(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeSpellSlot(void);

private:
	typedef map<int, SPELL_SLOT>		SCHEME_MAP;
	SCHEME_MAP							m_schemeMap;
};