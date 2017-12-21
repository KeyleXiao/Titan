#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "ChatDef.h"
#include <map>

using namespace rkt;

// 系统提示配置
class CSchemeSystemMessage : public ISchemeSystemMessage, public ISchemeUpdateSink, public CSchemeCache<CSchemeSystemMessage>
{
    // 配置文件对应的列
    enum SCHEME_COLUMN
    {
        COLUNM_ID = 0,					    // 消息Id
        COLUNM_TEXT,						// 文字描述
        COLUNM_SHOWTYPE,					// 显示类型(EMChatTipType)
        COLUNM_SHOWPOS,					    // 显示位置(EMChatInfoPos, 可多项)
        COLUNM_SHOWTIME,					// 文字显示时间(单位ms)
        COLUNM_FLAG,					    // 使用标识
    };

public:
	CSchemeSystemMessage(void);
	virtual ~CSchemeSystemMessage(void);

    /////////////////////////////////CSchemeSystemMessage/////////////////////////////////////////
	// 载入脚本
	bool LoadScheme(void);

	// 关闭
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV配置载入时通知
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// 配置动态更新时通知
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////////////ISchemeSystemMessage/////////////////////////////////////////
	// 取得配置信息
	virtual SSchemeSystemMessage* getSystemMessageInfo(int nId);

private:
    std::map<int, SSchemeSystemMessage>     m_mapScheme;
};