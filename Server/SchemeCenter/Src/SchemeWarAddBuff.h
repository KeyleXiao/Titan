/*******************************************************************
** 文件名:	SchemeWarAddBuf.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/6/7
** 版  本:	1.0
** 描  述:	
           战场添加buf 
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
class CSchemeWarAddBuff: public ISchemeWarAddBuff, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarAddBuff>
{
	typedef std::map< int, vector<SWarAddBuff>> WAR_ADDBUFF_MAP;
	typedef WAR_ADDBUFF_MAP::iterator WAR_ADDBUFF_MAP_ITER;

    // 配置文件对应的列
    enum SCHEME_COLUMN
    {
        COLNUM_MatchType,	// 比赛类型
		COLNUM_LimitLv,		// 玩家等级限制
		COLNUM_Time,		// 时间段
		COLNUM_BuffID,		// buf ID
		COLNUM_BuffLv,		// buf 等级
		COLNUM_IsAddToAI,	// 是否给AI增加
		COLNUM_IsAddToAIEx,	// 是否给具有玩家阵营的AI增加
    };

public:
	CSchemeWarAddBuff(void);
	virtual ~CSchemeWarAddBuff(void);

	// 载入脚本
	bool LoadScheme(void);

	// 关闭
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV配置载入时通知
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// 配置动态更新时通知
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarAddBuf///////////////////////
	// 获取添加Buff信息
	virtual vector<SWarAddBuff>* getAddBuffInfo(int nMatchID);

private:
	WAR_ADDBUFF_MAP     m_mapScheme;
};