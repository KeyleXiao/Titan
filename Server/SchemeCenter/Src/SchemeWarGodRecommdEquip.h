/*******************************************************************
** �ļ���:	SchemeWarGodRecommdEquip.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/4/17
** ��  ��:	1.0
** ��  ��:	
            �����Ƽ�װ�ű�
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeWarDef.h"
#include <map>

using namespace rkt;

// ϵͳ��ʾ����
class CSchemeWarGodRecommdEquip: public ISchemeWarGodRecommdEquip, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarGodRecommdEquip>
{
	typedef std::vector< SWarGodRecommdEquip > WAR_GODRECOMMDEQUIP_VECTOR;
	typedef WAR_GODRECOMMDEQUIP_VECTOR::iterator WAR_GODRECOMMDEQUIP_VECTOR_ITER;

    // �����ļ���Ӧ����
    enum SCHEME_COLUMN
    {
        COLNUM_LeagueName,		// ��������
        COLNUM_LeagueType,		// ��������
        COLNUM_Title,			// �ƺ�
        COLNUM_HeroID,			// Ӣ��ID
        COLNUM_SchemeName,		// ��������
        COLNUM_AuthorName,		// ��������
        COLNUM_EquipIDList,		// ����ID
    };

public:
	CSchemeWarGodRecommdEquip(void);
	virtual ~CSchemeWarGodRecommdEquip(void);

    /////////////////////////////////CSchemeWarGodRecommdEquip/////////////////////////////////////////
	// ����ű�
	bool LoadScheme(void);

	// �ر�
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV��������ʱ֪ͨ
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// ���ö�̬����ʱ֪ͨ
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarGodRecommdEquip///////////////////////
	// ��ȡӢ�۰��һ�����Ƽ�װ��
	virtual vector<SWarGodRecommdEquip>* getGodRecommdEquip();

public:

private:
	WAR_GODRECOMMDEQUIP_VECTOR     m_listScheme;
};