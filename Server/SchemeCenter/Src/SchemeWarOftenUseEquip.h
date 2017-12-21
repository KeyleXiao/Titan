/*******************************************************************
** 文件名:	SchemeWarOftenUseEquip.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/4/17
** 版  本:	1.0
** 描  述:	
            常用装备配置脚本
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeDef.h"
#include "SchemeWarDef.h"
#include <map>

using namespace rkt;

// 系统提示配置
class CSchemeWarOftenUseEquip: public ISchemeWarOftenUseEquip, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarOftenUseEquip>
{
    // 配置文件对应的列
    enum SCHEME_COLUMN
    {
        COLNUM_BaseWeight,		// 基础权重
        COLNUM_DampRatio,		// 衰减比例
    };

public:
	CSchemeWarOftenUseEquip(void);
	virtual ~CSchemeWarOftenUseEquip(void);

	// 载入脚本
	bool LoadScheme(void);

	// 关闭
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV配置载入时通知
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// 配置动态更新时通知
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarOftenuseEquip///////////////////////
	// 获取基本权重
	virtual int getBaseWeight();
	
	// 获取衰减比例
	virtual float getDampRatio();

public:

private:
	SWarOftenUseEquip m_OftenUseEquipConfig;
	
};