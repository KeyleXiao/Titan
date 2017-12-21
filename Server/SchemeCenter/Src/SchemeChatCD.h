/*******************************************************************
** 文件名:	CSchemeChatCD.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/1/3
** 版  本:	1.0
** 描  述:	
            聊听CD时间
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "ChatDef.h"
#include <map>

using namespace rkt;

// 系统提示配置
class CSchemeChatCD: public ISchemeChatCD, public ISchemeUpdateSink, public CSchemeCache<CSchemeChatCD>
{
    // 配置文件对应的列
    enum SCHEME_COLUMN
    {
        COLNUM_CHANNEL = 0,		// 频道ID
        COLNUM_LEVEL,			// 等级限制
		COLNUM_TIME,			// 冷却时间
    };

public:
	CSchemeChatCD(void);
	virtual ~CSchemeChatCD(void);

    /////////////////////////////////CSchemeChatCD/////////////////////////////////////////
	// 载入脚本
	bool LoadScheme(void);

	// 关闭
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV配置载入时通知
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// 配置动态更新时通知
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

public:
	/////////////////////////////////ISchemeChatCD/////////////////////////////////////////
	// 获取等级限制
	virtual int getLimitLevel(int nChannel);
	// 获取CD时间
	virtual int getCDTime(int nChannel);

private:
    std::map<int, SSchemeChatCD>     m_mapScheme;
};