/*******************************************************************
** �ļ���:	SchemePersonBaseProp.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	3/27/2016
** ��  ��:	1.0
** ��  ��:	���������������
********************************************************************/

#pragma once
#include "Singleton.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"


// ����
class CSchemePersonBaseProp : public ISchemePersonBaseProp, public ISchemeUpdateSink
{
public:
	/////////////////////////ISchemePersonBaseProp/////////////////////////
    virtual SCHEME_MAP*             getAllHeroInfo();
    virtual SPersonBasePropScheme   getOneHeroDetailInfo(int nHeroID);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemePersonBaseProp//////////////////////////
	bool											LoadScheme(void);
	void											Close(void);

	CSchemePersonBaseProp(void);
	virtual ~CSchemePersonBaseProp(void);

private:
    // �������Ա�
    typedef map<int, SPersonBasePropScheme>		SCHEME_MAP;
    SCHEME_MAP									m_schemeMap;
};