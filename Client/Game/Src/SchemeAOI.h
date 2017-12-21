/*******************************************************************
** 文件名:	SchemeAOI.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	6/01/2015
** 版  本:	1.0
** 描  述:	AOI通用配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include <map>

// 定义
class CSchemeAOI : public ISchemeAOI, public ISchemeUpdateSink
{
public:
    CSchemeAOI(void);

    virtual ~CSchemeAOI(void);

    /////////////////////////CSchemeAOI//////////////////////////
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

    // 获取AOI警戒距离
    virtual float           getGuardDistance();

    // 获取警戒缓冲时间
    virtual int             getGuardBufTime();

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad( SCRIPT_READER reader,const char* szFileName );
	virtual bool			OnSchemeUpdate( SCRIPT_READER reader,const char* szFileName );

private:
	float			m_fCheckDistance;
    int             m_nBufTime;
};