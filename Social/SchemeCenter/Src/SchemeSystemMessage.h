#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "ChatDef.h"
#include <map>

using namespace rkt;

// Ʈ����ʾ����
class CSchemeSystemMessage : public ISchemeSystemMessage, public ISchemeUpdateSink, public CSchemeCache<CSchemeSystemMessage>
{
    // �����ļ���Ӧ����
    enum SCHEME_COLUMN
    {
        COLUNM_ID = 0,					    // ��ϢId
        COLUNM_TEXT,						// ��������
        COLUNM_SHOWTYPE,					// ��ʾ����(EMChatTipType)
        COLUNM_SHOWPOS,					    // ��ʾλ��(EMChatInfoPos, �ɶ���)
        COLUNM_SHOWTIME,					// ������ʾʱ��(��λms)
        COLUNM_FLAG,					    // ʹ�ñ�ʶ
    };

public:
	CSchemeSystemMessage(void);
	virtual ~CSchemeSystemMessage(void);

    /////////////////////////////////CSchemeSystemMessage/////////////////////////////////////////
	// ����ű�
	bool LoadScheme(void);
	// �ر�
	void Close(void);

    /////////////////////////////////ISchemeSystemMessage/////////////////////////////////////////
	// ȡ��������Ϣ
    virtual SSchemeSystemMessage* getSystemMessageInfo(int nId);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV��������ʱ֪ͨ
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	// ���ö�̬����ʱ֪ͨ
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

private:
    std::map<int, SSchemeSystemMessage>     m_mapScheme;

};