/*******************************************************************
** �ļ���:	SchemeWarAddBuf.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/6/7
** ��  ��:	1.0
** ��  ��:	
           ս�����buf 
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeDef.h"
#include "SchemeWarDef.h"
#include <map>

using namespace rkt;

// ϵͳ��ʾ����
class CSchemeWarAddBuff: public ISchemeWarAddBuff, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarAddBuff>
{
	typedef std::map< int, vector<SWarAddBuff>> WAR_ADDBUFF_MAP;
	typedef WAR_ADDBUFF_MAP::iterator WAR_ADDBUFF_MAP_ITER;

    // �����ļ���Ӧ����
    enum SCHEME_COLUMN
    {
        COLNUM_MatchType,	// ��������
		COLNUM_LimitLv,		// ��ҵȼ�����
		COLNUM_Time,		// ʱ���
		COLNUM_BuffID,		// buf ID
		COLNUM_BuffLv,		// buf �ȼ�
		COLNUM_IsAddToAI,	// �Ƿ��AI����
		COLNUM_IsAddToAIEx,	// �Ƿ�����������Ӫ��AI����
    };

public:
	CSchemeWarAddBuff(void);
	virtual ~CSchemeWarAddBuff(void);

	// ����ű�
	bool LoadScheme(void);

	// �ر�
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV��������ʱ֪ͨ
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// ���ö�̬����ʱ֪ͨ
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarAddBuf///////////////////////
	// ��ȡ���Buff��Ϣ
	virtual vector<SWarAddBuff>* getAddBuffInfo(int nMatchID);

private:
	WAR_ADDBUFF_MAP     m_mapScheme;
};