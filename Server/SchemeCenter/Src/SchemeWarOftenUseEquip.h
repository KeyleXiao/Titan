/*******************************************************************
** �ļ���:	SchemeWarOftenUseEquip.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/4/17
** ��  ��:	1.0
** ��  ��:	
            ����װ�����ýű�
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
class CSchemeWarOftenUseEquip: public ISchemeWarOftenUseEquip, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarOftenUseEquip>
{
    // �����ļ���Ӧ����
    enum SCHEME_COLUMN
    {
        COLNUM_BaseWeight,		// ����Ȩ��
        COLNUM_DampRatio,		// ˥������
    };

public:
	CSchemeWarOftenUseEquip(void);
	virtual ~CSchemeWarOftenUseEquip(void);

	// ����ű�
	bool LoadScheme(void);

	// �ر�
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV��������ʱ֪ͨ
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// ���ö�̬����ʱ֪ͨ
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarOftenuseEquip///////////////////////
	// ��ȡ����Ȩ��
	virtual int getBaseWeight();
	
	// ��ȡ˥������
	virtual float getDampRatio();

public:

private:
	SWarOftenUseEquip m_OftenUseEquipConfig;
	
};