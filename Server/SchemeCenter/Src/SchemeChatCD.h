/*******************************************************************
** �ļ���:	CSchemeChatCD.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/1/3
** ��  ��:	1.0
** ��  ��:	
            ����CDʱ��
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "ChatDef.h"
#include <map>

using namespace rkt;

// ϵͳ��ʾ����
class CSchemeChatCD: public ISchemeChatCD, public ISchemeUpdateSink, public CSchemeCache<CSchemeChatCD>
{
    // �����ļ���Ӧ����
    enum SCHEME_COLUMN
    {
        COLNUM_CHANNEL = 0,		// Ƶ��ID
        COLNUM_LEVEL,			// �ȼ�����
		COLNUM_TIME,			// ��ȴʱ��
    };

public:
	CSchemeChatCD(void);
	virtual ~CSchemeChatCD(void);

    /////////////////////////////////CSchemeChatCD/////////////////////////////////////////
	// ����ű�
	bool LoadScheme(void);

	// �ر�
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV��������ʱ֪ͨ
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// ���ö�̬����ʱ֪ͨ
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

public:
	/////////////////////////////////ISchemeChatCD/////////////////////////////////////////
	// ��ȡ�ȼ�����
	virtual int getLimitLevel(int nChannel);
	// ��ȡCDʱ��
	virtual int getCDTime(int nChannel);

private:
    std::map<int, SSchemeChatCD>     m_mapScheme;
};