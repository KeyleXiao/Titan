#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "IClientGlobal.h"
#include "ChatDef.h"
#include <map>

using namespace rkt;

class CSchemeSystemMessage : public ISchemeSystemMessage, public ISchemeUpdateSink
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
		COLUNM_TITLE,					    // 窗口标题
	};

public:
	CSchemeSystemMessage(void);
	virtual ~CSchemeSystemMessage(void);

    /////////////////////////////////ISchemeSystemMessage/////////////////////////////////////////
	// 取得配置信息
     virtual SSchemeSystemMessage* getSystemMessageInfo(int nId);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV配置载入时通知
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	// 配置动态更新时通知
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////////////CSchemeSystemMessage/////////////////////////////////////////
	// 载入脚本
	bool LoadScheme(void);
	// 关闭
	void Close(void);

private:
    std::map<int, SSchemeSystemMessage> m_mapScheme;

};