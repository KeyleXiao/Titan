/*******************************************************************
** �ļ���:	SchemeWarDevilKing.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/11/30
** ��  ��:	1.0
** ��  ��:	
            ս��ħ���淨����
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
class CSchemeWarDevilKing: public ISchemeWarDevilKing, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarDevilKing>
{

public:
	CSchemeWarDevilKing(void);
	virtual ~CSchemeWarDevilKing(void);

	// ����ű�
	bool LoadScheme(void);

	// �ر�
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV��������ʱ֪ͨ
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// ���ö�̬����ʱ֪ͨ
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarDevilKing///////////////////////
	virtual SWarDevilKing* getWarDevilKing() override;

public:

private:
	SWarDevilKing m_Scheme;
};