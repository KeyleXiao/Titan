/*******************************************************************
** 文件名:	EffectFactory.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	penglu
** 日  期:	2014/12/23
** 版  本:	1.0
** 描  述:	效果管理器
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "IEffect.h"
#include "ISchemeEngine.h"
#include <map>
using namespace rkt;

// 效果管理器
class CEffectFactory : public IEffectFactory,public ISchemeUpdateSink
{
public:
	CEffectFactory();

	virtual ~CEffectFactory();

	// 加载效果工厂
	virtual bool            Load( const char * szFileName );

	/**
	@name				: 创建效果对象
	@param				: 效果ID
	@param				: 效果依附的实体
	@param				: 创建即开启
	@return
	*/
	virtual IEffect*		BuildEffect(int nEffectID);

	/**
	@name				: 执行一个内置效果(比如技能系统注册的内置效果)
	@param	strType  	: 效果类型名
	@param  pData       : 效果数据
	@param  nLen        : 效果数据长度
	@return
	*/
	virtual bool            DoInnerEffect(const char * strType,void * pData,int nLen,EFFECT_CONTEXT *pContext );

	// 释放
	virtual void			release();

	// 某个效果ID是否存在
	virtual bool			IsExistEffectID(int nEffectID);
	// 载入脚本 
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

private:
    bool LoadEffectCsv(SCRIPT_READER reader, const char* szFileName);
    bool LoadEffectBin(SCRIPT_READER reader, const char* szFileName);

private:
	typedef std::map<int,IEffect*>  EFFECT_MAP;
	EFFECT_MAP    m_effectMap;

    // 此表用来发现客户端和服务器重复效果ID，方便报错，及时修正
    typedef std::map<int,int>       TEMP_EFFECT_MAP;
    TEMP_EFFECT_MAP m_tempEffectMap;
};
